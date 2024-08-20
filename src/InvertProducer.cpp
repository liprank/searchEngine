//对网页进行权重处理
#include "../include/myxml.h"
//引入分词工具
#include "../include/SplitToolCppJieba.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_set>
#include <cmath>
#include <algorithm>

using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::map;
using std::unordered_set;
using std::pair;
using std::log2;

using std::cerr;
using std::cout;
using std::endl;

SplitToolCppJieba* SplitToolCppJieba::_pInstance = nullptr;

//创建倒排索引
void RssReader::invertDict(const string &filename1,const string &filename2){
    ifstream web(filename1); 
    ifstream offset(filename2);

    ifstream stopwordsEN("../conf/stop_words_eng.txt");
    ifstream stopwordsZH("../conf/stop_words_zh.txt");

    ofstream output("../data/InvertDict.dat",std::ios::app);

    ofstream test("1.txt");
    //分词器
    SplitTool* t = SplitToolCppJieba::getInstance();

    if(!web.is_open()){
        cerr << "open file webpage failed" << endl;
    }

    if(!offset.is_open()){
        cerr << "open file offset failed" << endl;
    }

    if(!stopwordsZH.is_open()){
        cerr << "open file stopwordsZH failed" << endl;
    }

    if(!stopwordsEN.is_open()){
        cerr << "open file stopwordsEN failed" << endl;
    }

    //采集停用词
    unordered_set<string> stopwords;
    string word;

    while(getline(stopwordsZH,word)){
        word.erase(remove_if(word.begin(),word.end(),::isspace),word.end());
//cout << "stop word = " << word << " size = " << word.size() << endl;
        stopwords.insert(word);
    } 
    while(getline(stopwordsEN,word)){
        word.erase(remove_if(word.begin(),word.end(),::isspace),word.end());
//cout << "stop word = " << word << " size = " << word.size() << endl;
        stopwords.insert(word);
    }

    //进行分词
    //根据网页偏移库，读取网页库
    string context;
    string line;
    int docid, pos, length;

    vector<string> words;

    //记录该词在文章中的出现次数，需要记录出现在哪篇文章
    map<pair<string,int>,int> dewords;

    //记录单词出现的doc数量
    unordered_map<string,int> wordinDocs;

    //计算权重值
    //<单词，docid>，权重
    map<pair<string,int>,double> weight;

    //unordered_map<string,int>
    //每次读取一个doc
    while(getline(offset,line)){
        //获取网页偏移库
        istringstream iss(line);
        iss >> docid >> pos >> length;

cout << "1docid: " << docid << '\n';
cout << "pos: " << pos << "\n";
cout << "length: " << length << "\n";

        //读取网页库
        char buffer[length+1];
        web.seekg(pos);
        web.read(buffer,length);

        //进行分词
        string temp(buffer);
cout << temp << "\n";
        temp.erase(remove_if(temp.begin(),temp.end(),::isspace),temp.end());
//cout << temp << "\n";

        words = t->cut(temp);
        //去除停用词
        for(string word : words){
            if(word.size() > 2 && stopwords.find(word) == stopwords.end()){
                //得到词语在文章中的出现次数
//cout << "2docid: " << docid << "\n";
                dewords[make_pair(word,docid)]++;
                test << word << "\n";
            }else{
                continue;
            }
        }

        //遍历去停用词词库
        for(auto elem : dewords){
            string key = elem.first.first;
            int value = elem.second;
            if(elem.second > 0){
                wordinDocs[key]++;
            }
        }
    }

    //生成权重
    //TF: 在该文章中出现的次数
    //DF: 包含该词语的文档数量
    //IDF: 该词对该篇文章的重要性
    //现在得到在文章中的出现次数，包含该词的文章数量，计算IDF
    //IDF = log2(N/(DF+1)+1)
    //w = TF * IDF
    int N = 100;//TODO:假设总文档数为100
    double weightSum = 0.0;
    for(auto elem : dewords){
        string key = elem.first.first;
        int id = elem.first.second;
        int value = elem.second;

        int docs = wordinDocs[key];
        double IDF = log2(N/(docs+1)+1);
        double w = value * IDF; 

        //存储权重
        weight[make_pair(key,id)] = w;

        //记录权重平方和，用于归一化处理
        weightSum = w * w;
    }

    //得到每一个词的权重，进行归一化处理
    //1.设置一个存放权重的数据结构
    //2.得到一篇文章中所有词的权重
    //3.对一篇文章中的每一个词进行归一化处理 w1^2 / (w1^2 + ... + wn^2)^0.5
    //要得到的倒排索引表，一个网页生成一个倒排索引表，其中需要包含词，权重，所在的文章docid
    //因为打开的文件就包含所在的网页，所以不需要所在网页
    if(weightSum > 0.0){
        weightSum = pow(weightSum,0.5); 
    }else{
        cerr << "无法计算权重" << endl;
        return;
    }

    for(auto elem : weight){
        string key = elem.first.first;
        int id = elem.first.second;
        double value = elem.second;

        elem.second = (value*value) / weightSum; 

        //创建倒排索引表
        //TODO: 根据权重进行排序
        output << key << " " << id << " " << value << "\n";
    }
}

int main(){
    RssReader reader;
    reader.invertDict("../data/webpage1.dat","../data/offset1.dat");
}

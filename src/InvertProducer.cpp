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
#include <regex>

using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::map;
using std::unordered_set;
using std::pair;
using std::log2;
using std::regex;

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

    ofstream output("../data/InvertDict1.dat",std::ios::out);

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

    //记录总文档数
    int N = 0;

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
        N++;
        //获取网页偏移库
        istringstream iss(line);
        iss >> docid >> pos >> length;

// cout << "1docid: " << docid << '\n';
// cout << "pos: " << pos << "\n";
// cout << "length: " << length << "\n";

        //读取网页库
        char buffer[length+1];
        bzero(buffer,length+1);

        web.seekg(pos);
        web.read(buffer,length-1);

        string temp(buffer);
        // cout << temp << "\n";

        //使用正则表达式
        std::regex tag_pattern(R"(<(\w+)>(.*?)</\1>)",std::regex::ECMAScript);     
        std::sregex_iterator it_begin(temp.begin(), temp.end(), tag_pattern);
        std::sregex_iterator it_end;

        // 存储解析结果的map
        std::map<std::string, std::string> doc_data;

        // 遍历所有匹配项
        for (std::sregex_iterator i = it_begin; i != it_end; ++i) {
            std::smatch match = *i;
            std::string tag = match[1].str();
            std::string content = match[2].str();
            doc_data[tag] = content;
        }

        // for (const auto& pair : doc_data) {
        //     string des = pair.first;
        //     if(des == "description"){
        //         cout << pair.second << "\n";
        //     }
        // }

        //进行分词
        // XMLDocument doc;
        // if(doc.Parse(buffer) != XML_SUCCESS){
        //     cerr << "解析文章失败\n"; 
        // }
        // XMLElement* root = doc.FirstChildElement();
        // XMLElement* context = root->FirstChildElement("description");
        // XMLText* text = context->FirstChild()->ToText();
        // string temp = text->Value();

        words = t->cut(doc_data["description"]);
        //去除停用词
        for(string word : words){
            if(word.size() > 2 && stopwords.find(word) == stopwords.end()){
                pair<string,int> wordtext = make_pair(word,docid);
                dewords[wordtext]++;
            }else{
                continue;
            }
        }
    }

    for(auto elem : dewords){
        string key = elem.first.first;
        //记录出现的文章数
        wordinDocs[key]++;
    }

    //生成权重
    //TF: 在该文章中出现的次数
    //DF: 包含该词语的文档数量
    //IDF: 该词对该篇文章的重要性
    //现在得到在文章中的出现次数，包含该词的文章数量，计算IDF
    //IDF = log2(N/(DF+1)+1)
    //w = TF * IDF
    double weightSum = 0.0;
    for(auto elem : dewords){
        string word = elem.first.first;
        int docid = elem.first.second;
        int times = elem.second;

        int docs = wordinDocs[word];

        double IDF = log2(N/(docs+1)+1);

        double w = times * IDF;

cout << "docid: " << docid << "\n";
cout << "times: " << times << "\n";
cout << "docs: " << docs << "\n";
cout << "IDF: " << IDF << "\n";
cout << "w: " << w << "\n";

        //存储权重
        weight[make_pair(word,docid)] = w;

        //记录权重平方和，用于归一化处理
        weightSum += w * w;
    }



    //得到每一个词的权重，进行归一化处理
    //1.设置一个存放权重的数据结构
    //2.得到一篇文章中所有词的权重
    //3.对一篇文章中的每一个词进行归一化处理 w1^2 / (w1^2 + ... + wn^2)^0.5
    
    if(weightSum > 0.0){
        weightSum = pow(weightSum,0.5); 
    }else{
        cerr << "无法计算权重" << endl;
        return;
    }
cout << "weightsum: " << weightSum <<  "\n";
double test = 0;
    for(auto elem : weight){
        string word = elem.first.first;
        int docid = elem.first.second;
        double weight = elem.second;

test += weight*weight;
cout << "weightbefore: " << weight << "\n"; 
        weight = weight / weightSum; 
        //创建倒排索引表
        //TODO: 根据权重进行排序
cout << "weightend: " << weight << "\n"; 
        output << word << " " << docid << " " << weight << "\n";
    }
cout << "test: " << pow(test,0.5) << "\n";
}

int main(){
    RssReader reader;
    reader.invertDict("../data/webpage1.dat","../data/offset1.dat");
}

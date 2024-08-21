/**
 * Project SearchEngine
 */


#include "../include/WebPageQuery.h"
#include "../include/nlohmann/json.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <cmath>

using std::ifstream;
using std::istringstream;
using std::cerr;
using std::set;
using std::unordered_set;
using std::make_pair;
using std::endl;
using std::set_intersection;
/**
 * WebPageQuery implementation
 */

SplitToolCppJieba* SplitToolCppJieba::_pInstance = nullptr;

//求向量的模长
double WebPageQuery::getMold(const vector<double>& vec){           
    int n = vec.size();
    float sum = 0.0;
    for (int i = 0; i<n; ++i)
        sum += vec[i] * vec[i];
    return sqrt(sum);
}

double WebPageQuery::getSimilarity(const vector<double>& lhs, const vector<double>& rhs){
    int n = lhs.size();
    if(n != rhs.size()){
        cerr << "向量长度不等，无法计算相似度\n";
    }

    float tmp = 0.0;  //内积
    for (int i = 0; i<n; ++i)
        tmp += lhs[i] * rhs[i];
    return tmp / (getMold(lhs) * getMold(rhs));
}

WebPageQuery::WebPageQuery() 
{
    //基类指针指向派生类对象
    _wordCutTool = SplitToolCppJieba::getInstance();
}

string WebPageQuery::createJson(){
    //打开网页偏移库
    ifstream ifs("../data/offset.dat");
    if(!ifs.is_open()){
        cerr << "open file offset failed\n";
    }

    //打开网页库
    ifstream web("../data/webpage.dat");
    if(!web.is_open()){
        cerr << "open file webpage failed\n";
    }

    nlohmann::json json_object;

    string line;
    while(getline(ifs,line)){
        int docid;
        unsigned int pos;
        unsigned int length;
        istringstream iss(line);

        iss >> docid >> pos >> length;
        _offsetlib[docid] = make_pair(pos,length);
    }

    int count = 0;
    vector<int> docs;
    while(!_priQue.empty() && count < 10){
        auto elem = _priQue.top();
        _priQue.pop();
        //获取优先级队列当中的文章id
        docs.push_back(elem.first);
    }

    //通过网页偏移库来读取网页库
    for(int docid : docs){
        int pos = _offsetlib[docid].first;
        int length = _offsetlib[docid].second;

        char buffer[length+1];
        web.seekg(pos);
        web.read(buffer,length);

        json_object.push_back(string(buffer));
    }

    return json_object.dump();
}

/**
 * @param key
 * @return string
 */
string WebPageQuery::doQuery(string key) {
    //首先进行分词
    vector<string> words = _wordCutTool->cut(key);

    //获取去除停用词之后的结果
    vector<string> dewords;

    //采集停用词
    ifstream stopwordsEN("../conf/stop_words_eng.txt");
    ifstream stopwordsZH("../conf/stop_words_zh.txt");

    if(!stopwordsZH.is_open()){
        cerr << "open file stopwordsZH failed\n";
    }

    if(!stopwordsEN.is_open()){
        cerr << "open file stopwordsEN failed\n";
    }

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

    for(string str : words){
        if(stopwords.find(str) == stopwords.end()){
            dewords.push_back(str);
        }
    } 

    //查找倒排索引表
    ifstream ifs("../data/InvertDict.dat");

    //读取倒排索引表
    string line;
    unordered_map<string,vector<pair<int, double>>> invertTable;
    while(getline(ifs,line)){
        istringstream iss(line);
        string word;
        int docid;
        double weight;
        iss >> word >> docid >> weight;

        invertTable[word].push_back(make_pair(docid,weight));
    }

    //存入倒排索引表结构
    for(string word : dewords){
        _invertIndexLib[word] = invertTable[word];
    }

    //取完交集之后得到docid
    //获得每个词的权重docid,<string,weight>
    string s = dewords.back();
    dewords.pop_back();
    vector<pair<int,double>> vec = _invertIndexLib[s];

    vector<int> v1;
    for(auto elem : vec){
        v1.push_back(elem.first);
    }

    vector<int> target;
    for(string str : dewords){
        vec = _invertIndexLib[str];

        vector<int> v2;
        for(auto elem : vec){
            v2.push_back(elem.first);
        }

        set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),target.begin());

        //将target赋值给v1继续进行取交集
        v1 = target;

        //清空target
        target.clear();
        target.shrink_to_fit();
    }

    //现在得到了包含所有关键字的所在docid
    //         关键字1的权重 关键字2的权重 关键字3的权重
    //docid1
    //docid2
    //docid3

    //<docid,weight>
    unordered_map<int,vector<double>> weightmap;
    // for(int i : v1){
    //     weightmap[i].push_back();
    // }

    for(string word : dewords){
        vector<pair<int,double>> vec = _invertIndexLib[word];
        vector<int> docs;

        //存储所有的docid
        for(auto elem : vec){
            int doc = elem.first;
            int w = elem.second;
            auto it = find(v1.begin(),v1.end(),doc);
            if(it != v1.end()){
                //用位置来确定关键词
                weightmap[doc].push_back(w);                
            }
        }
    }

    //计算基准向量每个词的权重
    vector<double> basic;
    for(string word : dewords){
        int TF = count(dewords.begin(),dewords.end(),word);
        int DF = TF;
        double IDF = log2((1/(DF+1))+1);
        double w = TF * IDF;
        basic.push_back(w);
    }

    //基准向量与其计算余弦值
    for(auto elem : weightmap){
        int doc = elem.first;
        vector<double> w = elem.second;

        //计算余弦值
        int res = getSimilarity(basic,w);

        //全部加入优先级队列
        _priQue.push(make_pair(doc,res));
    }

    return "";
}

// int main(){
//     string searchword = "hello中国";
//     WebPageQuery w;
//     w.doQuery(searchword);
// }
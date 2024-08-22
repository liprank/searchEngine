/**
 * Project SearchEngine
 */


#ifndef _WEBPAGEQUERY_H
#define _WEBPAGEQUERY_H
#include "../include/SplitToolCppJieba.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>

using std::string;
using std::vector;
using std::unordered_map;
using std::unordered_multimap;
using std::priority_queue;

using std::pair;

class WebPageQuery {
public: 
    
    WebPageQuery();
    
/**
 * @param key
 */
    void doQuery(string key);

//获取文档内容，形成json格式
//从优先级队列当中取出文档，然后访问网页偏移库，访问网页，形成json格式并返回
    string createJson();

private:
    //计算向量的模长
    double getMold(const vector<double>& vec);

    //计算向量的相似度
    double getSimilarity(const vector<double>& lhs, const vector<double>& rhs);
private: 
    //vector<WebPage> _pages;

    //读取网页
    unordered_map<int, pair<int, int>> _offsetlib;

    //倒排索引表
    unordered_map<string,vector<pair<int, double>>> _invertIndexLib;

    //优先级队列
    //docid，相似度
    priority_queue<pair<int,double>> _priQue;

    SplitTool* _wordCutTool;
};

// namespace std{
// template<>
// struct less<pair<int,double>>{
//     bool operator()(const pair<int,double>& lhs,const pair<int,double> &rhs){
//         return lhs.second < rhs.second;
//     }
// };
// }

#endif //_WEBPAGEQUERY_H
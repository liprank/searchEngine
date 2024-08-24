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

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::make_pair;
using std::set;
using std::set_intersection;
using std::unordered_set;
/**
 * WebPageQuery implementation
 */

SplitToolCppJieba *SplitToolCppJieba::_pInstance = nullptr;

// 求向量的模长
double WebPageQuery::getMold(const vector<double> &vec)
{
    int n = vec.size();
    float sum = 0.0;
    for (int i = 0; i < n; ++i)
        sum += vec[i] * vec[i];
    return sqrt(sum);
}

double WebPageQuery::getSimilarity(const vector<double> &lhs, const vector<double> &rhs)
{
    int n = lhs.size();
    if (n != rhs.size())
    {
        cerr << "向量长度不等，无法计算相似度\n";
    }

    float tmp = 0.0; // 内积
    for (int i = 0; i < n; ++i)
        tmp += lhs[i] * rhs[i];
    return tmp / (getMold(lhs) * getMold(rhs));
}

WebPageQuery::WebPageQuery()
{
    // 基类指针指向派生类对象
    _wordCutTool = SplitToolCppJieba::getInstance();
}

string WebPageQuery::createJson()
{
    // 打开网页偏移库
    ifstream ifs("../data/offset.dat");
    if (!ifs.is_open())
    {
        cerr << "open file offset failed\n";
    }

    // 打开网页库
    ifstream web("../data/webpage.dat");
    if (!web.is_open())
    {
        cerr << "open file webpage failed\n";
    }

    nlohmann::json json_object;

    string line;
    while (getline(ifs, line))
    {
        int docid;
        unsigned int pos;
        unsigned int length;
        istringstream iss(line);

        iss >> docid >> pos >> length;
        _offsetlib[docid] = make_pair(pos, length);
    }

    int count = 0;
    vector<int> docs;

    while (!_priQue.empty() && count < 10)
    {
        auto elem = _priQue.top();
        _priQue.pop();

        // 获取优先级队列当中的文章id
        docs.push_back(elem.first);
    }

    // 通过网页偏移库来读取网页库
    char *buffer;

    for (int docid : docs)
    {

        int pos = _offsetlib[docid].first;
        int length = _offsetlib[docid].second;

        //读取长度过长会有脏数据
        buffer = (char *)calloc(length + 1,1);
        // bzero(buffer, length + 1);

        web.seekg(pos,std::ios::beg);
        web.read(buffer, length);

        json_object.push_back(string(buffer));

        free(buffer);
        web.seekg(0,std::ios::beg);
        web.clear();
    }

    return json_object.dump();
}

/**
 * @param key
 * @return string
 */
void WebPageQuery::doQuery(string key)
{
    // 首先进行分词
    vector<string> words = _wordCutTool->cut(key);

    // 获取去除停用词之后的结果
    vector<string> dewords;

    // 采集停用词
    ifstream stopwordsEN("../conf/stop_words_eng.txt");
    ifstream stopwordsZH("../conf/stop_words_zh.txt");

    if (!stopwordsZH.is_open())
    {
        cerr << "open file stopwordsZH failed\n";
    }

    if (!stopwordsEN.is_open())
    {
        cerr << "open file stopwordsEN failed\n";
    }

    unordered_set<string> stopwords;
    string word;

    while (getline(stopwordsZH, word))
    {
        word.erase(remove_if(word.begin(), word.end(), ::isspace), word.end());
        // cout << "stop word = " << word << " size = " << word.size() << endl;
        stopwords.insert(word);
    }
    while (getline(stopwordsEN, word))
    {
        word.erase(remove_if(word.begin(), word.end(), ::isspace), word.end());
        // cout << "stop word = " << word << " size = " << word.size() << endl;
        stopwords.insert(word);
    }

    for (string str : words)
    {
        if (stopwords.find(str) == stopwords.end())
        {
            dewords.push_back(str);
        }
    }

    // 查找倒排索引表
    ifstream ifs("../data/InvertDict.dat");

    // 读取倒排索引表
    string line;
    unordered_map<string, vector<pair<int, double>>> invertTable;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string word;
        int docid;
        double weight;
        iss >> word >> docid >> weight;

        invertTable[word].push_back(make_pair(docid, weight));
    }

    // 存入倒排索引表结构
    for (string word : dewords)
    {
        _invertIndexLib[word] = invertTable[word];
        cout << "word: " << word << "\n";
    }

    // 取完交集之后得到docid
    // 获得每个词的权重docid,<string,weight>
    string s = dewords[0];
    // dewords.pop_back();
    vector<pair<int, double>> vec = _invertIndexLib[s];

    cout << s << "\n";
    vector<int> v1;
    for (auto elem : vec)
    {
        v1.push_back(elem.first);
    }

    // cout << "\n";

    vector<int> target;
    for (int i = 1; i < dewords.size(); ++i)
    {
        string str = dewords[i];
        vec = _invertIndexLib[str];
        // cout << str << "\n";
        vector<int> v2;
        for (auto elem : vec)
        {
            v2.push_back(elem.first);
        }

        set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(target));

        // 将target赋值给v1继续进行取交集
        v1 = target;

        // 清空target
        target.clear();
    }

    vector<vector<double>> weightmap;

    // wordid docid weight

    //         关键字1的权重 关键字2的权重 关键字3的权重
    // docid1
    // docid2
    // docid3

    for (int docid : v1)
    {
        vector<pair<int, double>> vec;
        vector<double> w;

        for (string word : dewords)
        {
            vec = _invertIndexLib[word];

            for (auto elem : vec)
            {
                if (elem.first == docid)
                {
                    w.push_back(elem.second);
                }
            }
        }
        weightmap.push_back(w);
    }

    // 计算基准向量每个词的权重
    // 同样需要归一化处理
    double weightsum = 0.0;
    vector<double> basic;

    for (string word : dewords)
    {
        int TF = count(dewords.begin(), dewords.end(), word);

        // double IDF = log2((1/(1+1))+1);
        double w = TF;
        basic.push_back(w);
        weightsum += w * w;
    }

    weightsum = pow(weightsum, 0.5);

    // 基准向量与其计算余弦值
    int count = 0;
    for (auto elem : weightmap)
    {
        vector<double> w = elem;

        // 计算余弦值
        int res = getSimilarity(basic, w);

        // 全部加入优先级队列
        _priQue.push(make_pair(v1[count++], res));
    }

    return;
}

// int main()
// {
//     string searchword = "还进行";
//     WebPageQuery w;
//     w.doQuery(searchword);
//     cout << w.createJson() << endl;
// }
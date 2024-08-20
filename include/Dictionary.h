/**
 * Project SearchEngine
 */


#ifndef _DICTIONARY_H
#define _DICTIONARY_H
#include <vector>
#include <string>
#include <set>
#include <map>

using std::vector;
using std::string;
using std::pair;
using std::set;
using std::map;

class Candidate{
public:
    //重载比较规则
    bool operator<(const Candidate &a)const{
        if(this->_dist != a._dist){
            return this->_dist > a._dist;
        }else if(this->_freq != a._freq){
            return this->_freq > a._freq;
        }else{
            return this->_word > a._word;
        }
    }
public:
    //查询词
    string _word;

    //出现频率,因为返回的单词已经是根据出现频率进行排序的，所以根据位置即可判断出现频率
    int _freq;

    //最小编辑距离
    int _dist;
};


class Dictionary {
public: 
    //通过文件路径来初始化词典
    void init(const string& dictpath,const string& indexpath);

    //获取词典
    vector<pair<string,int>> &getDict();

    //获取词典索引
    map<string,set<int>> &getIndex();

/**
 * @param key
 * 返回候选词列表
 * 在查询的时候，找到查询字的并集，然后存储起来，计算其最小编辑距离
 * 然后存入候选词之中，根据最小编辑距离的排序来返回特定个数的候选词
 */
    vector<string> doQuery(const string& key);
   
//void queryIndex();
    
/**
 * @param candidate
 * 计算最小编辑距离，对候选词进行筛选
 */
    int distance(string query, string candidate);



private: 
    //词典
    vector<pair<string,int>> _dict;

    //索引表
    map<string,set<int>> _index;
};

#endif //_DICTIONARY_H
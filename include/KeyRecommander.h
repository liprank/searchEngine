/**
 * Project SearchEngine
 */


#ifndef _KEYRECOMMANDER_H
#define _KEYRECOMMANDER_H
#include "../include/Dictionary.h"
#include <string>
#include <queue>

using std::string;
using std::priority_queue;

class KeyRecommander {
public: 

    //用于查询,用dictonary类中的函数进行查询，并且计算最小编辑距离
    //并存放到优先级队列中 

    void doQuery();
    void doQuery(string word);

    KeyRecommander();

    KeyRecommander(string word);

public:
    //存储查询词
    string _sought;

    //存放优先级队列
    priority_queue<Candidate> _priQue;
};

#endif //_KEYRECOMMANDER_H
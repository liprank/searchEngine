/**
 * Project SearchEngine
 */


#ifndef _PAGELIBPREPROCESSOR_H
#define _PAGELIBPREPROCESSOR_H
#include "../include/Configuration.h"
#include "../include/SplitToolCppJieba.h"
#include <unordered_map>
#include <utility>

using std::unordered_map;
using std::pair;

class PageLibPreprocessor {
public: 
    
/**
 * @param Configuration &conf
 */
    //读取配置文件
    //PageLibPreprocessor(Configuration &conf);
    PageLibPreprocessor();
    
private: 
    SplitTool* _wordCutter;
    
    //网页偏移库
    //<docid,<pos,length>>
    unordered_map<int,pair<int,int>> _offsetLib;    

    //网页库

    //进行处理 
    void doProcess();
};

#endif //_PAGELIBPREPROCESSOR_H
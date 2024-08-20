/**
 * Project SearchEngine
 */


#ifndef _DICTPRODUCER_H
#define _DICTPRODUCER_H
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>


using std::string;
using std::vector;
using std::set;
using std::map;
using std::pair;

//进行前向声明
class SplitTool;
class SplitToolCppJieba;

class DictProducer {
public: 
    
/**
 * @param string
 * @param tool
 */

//英文
DictProducer();
//中文
DictProducer(SplitTool*);

~DictProducer();

void buildEnDict(const string &);
    
void buildCnDict(const string &);
    
//创建索引
void createIndex();

//存储索引库  
void store(const string &indexfile);

private: 
	//存放多个要读取的文件,这个通过配置文件来读取
    vector<string> _files;
	//词典
    vector<pair<string,int>> _dict;
	//词典索引
    map<string,set<int>> _index;
	//jieba分词
    SplitTool* _cuttor;
};

#endif //_DICTPRODUCER_H

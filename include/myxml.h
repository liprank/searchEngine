#include "../include/tinyxml2.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <regex>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ofstream;
using std::cerr;
using std::regex;

using namespace tinyxml2;

struct RssItem
{
    string title;
    string link;
    string description;
    //string content;
};

class RssReader
{
public:
    RssReader();
    void parseRss();                                             //解析xml文件,并调用去重
    void dump(const string & filename1,const string & filename2);//输出xml文件清洗完的网页文件，网页偏移文件
    bool isDuplication(string context);                          //判断网页是否重复，去重,输入一个网页内容，生成simhash，比较simhash
private:
    vector<RssItem> _rss;              //网页可能很大，vector无法存储
	XMLDocument doc;
    vector<string> _simhash;           //存储网页simhash值
 }; 
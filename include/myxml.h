#include "../include/tinyxml2.h"
#include "WebPage.h"

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
    void parseRss();                   //解析xml文件,并写入WebPage当中
    void dump(const string & filename);//输出xml文件
private:
    vector<RssItem> _rss;              //网页可能很大，vector无法存储
	XMLDocument doc;
 }; 
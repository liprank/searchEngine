#include "../include/myxml.h"
#include "../include/simhash/Simhasher.hpp"

#include <unordered_map>
#include <utility>
#include <string>

using std::unordered_map;
using std::pair;
using std::make_pair;
using std::to_string;

//this define can avoid some logs which you don't need to care about.
#define LOGGER_LEVEL LL_WARN 
using namespace simhash;

RssReader::RssReader(){
	doc.LoadFile("../conf/auto.xml");
}

//判断网页是否重复，如果重复则不进行解析
bool RssReader::isDuplication(string context){
 //simhash的构造函数
    Simhasher simhasher("../include/dictsimhash/jieba.dict.utf8", "../include/dictsimhash/hmm_model.utf8", "../include/dictsimhash/idf.utf8", "../include/dictsimhash/stop_words.utf8");
    string s(context);

    //有权重单词的个数
    size_t topN = 5;

    //u64用于存储指纹
    uint64_t u64 = 0;
    vector<pair<string ,double> > res;
    simhasher.extract(s, res, topN);

    //算指纹，存储指纹
    //因为simhash传入的参数为string，所以处理网页时也需要将网页变为string，来生成simhash
    simhasher.make(s, topN, u64);
    cout<< "文本：\"" << s << "\"" << endl;
    cout << "关键词序列是: " << res << endl;
    cout<< "simhash值是: " << u64<<endl;

    // const char * bin1 = "100010110110";
    // const char * bin2 = "110001110011";

	//将simhash值转为string
	string bin;
	Simhasher::toBinaryString(u64,bin);

    // uint64_t u1, u2;
    // u1 = Simhasher::binaryStringToUint64(bin1); 
    // u2 = Simhasher::binaryStringToUint64(bin2); 
    
	if(0 == _simhash.size()){
		//如果vector为空，则存放网页simhash
		_simhash.push_back(bin);
	}else{
		//如果vector不为空，则与之前vector中的元素进行比较	
		uint64_t hash; 
		for(string hashstr : _simhash){
			hash = Simhasher::binaryStringToUint64(hashstr);
			//如果网页重复,海明距离默认为3
			if(Simhasher::isEqual(hash,u64)){
				cout << "网页重复" << endl;
				return true;
			}
		}
		_simhash.push_back(bin);
	}

	cout << "网页不重复" << endl;
    return false;
}

void RssReader::parseRss()
{
	XMLNode* declaration = doc.FirstChild();
	XMLNode* rss = declaration->NextSibling(); 
	XMLElement* rssElement = rss->ToElement();

	int times = 1;
	XMLElement* itemElement = rssElement->FirstChildElement("channel")->FirstChildElement("item"); 

	while(itemElement){
		XMLElement* titleElement = itemElement->FirstChildElement("title");
		XMLElement* linkElement = itemElement->FirstChildElement("link");
		XMLElement* descriptionElement = itemElement->FirstChildElement("description");
		//XMLElement* contentElement = itemElement->FirstChildElement("content:encoded");

		XMLText* textNode0 = titleElement->FirstChild()->ToText();
		XMLText* textNode1 = linkElement->FirstChild()->ToText();
		XMLText* textNode2 = descriptionElement->FirstChild()->ToText();

		//XMLText* textNode3 = contentElement->FirstChild()->ToText();
	
		const char* title = textNode0->Value();
		const char* link = textNode1->Value();
		const char* description = textNode2->Value();

		//const char* content = textNode3->Value();

        //加入过滤器
		regex re1(R"(<p>|</p>|</li>|</ul>|<h4>|</h4>|<div>|</div>)");
		regex re2(R"(<ul>)");
		regex re3("<li>");
		//regex re2("</p>");

		string descriptionR1 = std::regex_replace(description,re1,"");
		//string contentR1 = std::regex_replace(content,re1,"");
		
		string descriptionR2 = std::regex_replace(descriptionR1,re2,"\n");
		//string contentR2 = std::regex_replace(contentR1,re2,"\n");

		string descriptionR3 = std::regex_replace(descriptionR2,re3,"->");
		//string contentR3 = std::regex_replace(contentR2,re3,"->");

		RssItem rssItem;
		rssItem.title = title;
		rssItem.link = link;
		rssItem.description = descriptionR3;
		//rssItem.content = contentR3;

		//在解析网页这里实现去重
		//TODO: 现在只根据description来判断是否重复
		//TODO:网页可能很大_rss可能无法存储，所以webpage需要一个个读
		if(isDuplication(rssItem.description) == false){
			_rss.push_back(rssItem);
		}

		//printf( "title: %s\n", title );
		//printf( "link: %s\n", link );
		//printf( "description: %s\n", description );

		//转到一个item部分
		itemElement = itemElement->NextSiblingElement();
		if(itemElement == nullptr) cout << "itemElement is null" <<" times:" << times << endl;
		times++;
cout << times << "\n";
	}
}

//输出xml格式化文件
//要在生成文件之前，存储网页偏移库
void RssReader::dump(const string& filename1,const string& filename2){
	ofstream output(filename1,std::ios::out);	
	ofstream offset(filename2,std::ios::out);	

	//创建网页偏移库结构
	unordered_map<int,pair<int,int>> offsetLib;

	int docid = 1;
	int pos = 0;
	int length = 0;
	for(auto item : _rss){
		output << "<doc>"<< endl;
		output << "    " << "<docid>" << docid << "</docid>" << endl;
		output << "    " << "<link>" << item.link << "</link>" << endl;
		output << "    " << "<title>" << item.title << "</title>" << endl;
		output << "    " << "<description>" << item.description << "</description>"<< endl;
		//output << "\t" << "<content>" << item.content << "</content>" << endl;
		output << "</doc>" << endl;

		length += strlen("<doc>") + 1;
		length += 4 + strlen("<docid>") + to_string(docid).size() + strlen("</docid>") + 1;
		length += 4 + strlen("<link>") + item.link.size() + strlen("</link>") +1;
		length += 4 + strlen("<title>") + item.title.size() + strlen("</title>") +1;
		length += 4 + strlen("<description>") + item.description.size() + strlen("</description>") +1;
		length += strlen("</doc>") + 1;

		//offsetLib[docid] = make_pair(pos,length);

	//写入网页偏移库
		offset << docid << " " << pos << " " << length << "\n";

		pos += length;
		docid++;
	}

	output.close();
	offset.close();
	cout << "file written successfully!" << endl;
}



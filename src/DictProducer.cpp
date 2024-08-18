/**
 * Project SearchEngine
 */

#include "../include/SplitToolCppJieba.h"
#include "../include/DictProducer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

using std::ifstream;
using std::istringstream;
using std::ofstream;
using std::cerr;

using std::cout;
using std::endl;

/**
 * DictProducer implementation
 */


/**
 * @param string
 * @param tool
 */
DictProducer::DictProducer(const string & filepath,SplitTool* t) 
//基类的指针指向派生类对象，进行重载
:_cuttor(t)
{
	_files.push_back(filepath);
	
	buildCnDict();
	createIndex();
	store();
}

DictProducer::DictProducer(const string & filepath){
	_files.push_back(filepath);
	buildEnDict();
	createIndex();
	store();
}

DictProducer::~DictProducer(){
}

/**
 * @return void
 * 生成英文词典
 */
void DictProducer::buildEnDict() {
	//英文词典
	//读取文件，清洗写入
	string filepath = _files.back();	
	_files.pop_back();
	
	ifstream files(filepath); 
	if(!files.is_open()){
		cerr << "open file failed\n";
	}
			
	string line;
	int num = 0;
	map<string,int> wordFq;//统计词频
	while(getline(files,line)){
		//清洗文件
		for(char &ch : line){
			if(isalpha(ch)){
				tolower(ch);
			}
			else if(isalnum(ch)){
				ch = ' ';
			}
			else if(ispunct(ch)){
				ch = ' ';
			}
			else if(ch == '\n' || ch == '\r'){
				ch = ' ';
			}
		}
		
		istringstream iss(line);
		string word;
		while(iss >> word){
			wordFq[word]++;
		}

		//TODO:erase remove的用法
	}

	//根据词频顺序来放入词典中,而不是原来的顺序
	//存储英文字典
	ofstream ofs("../data/enWordDict.dat");
	for(auto elem : wordFq){
		ofs << elem.first << " " << num << "\n";
		_dict.push_back(make_pair(elem.first,num++));	
		
	}

    return;
}

/**
 * @return void
 * 创建中文词典
 */
void DictProducer::buildCnDict() {
	//打开文件
	string filepath = _files.back();	
	_files.pop_back();
	
	ifstream ifs(filepath); 
	if(!ifs.is_open()){
		cerr << "open file failed\n";
	}
	
	//一次读取全部文件
	int num = 0;
	map<string,int> wordFq;//统计词频
	ifs.seekg(0,std::ios::end);
	size_t length = ifs.tellg();
	ifs.seekg(0,std::ios::beg);
	char *pdata = new char[length+1]();
	ifs.read(pdata,length);

	string str(pdata);
	str.erase(remove_if(str.begin(),str.end(),[](char c){
		return isspace(c);
	}));

	vector<string> temp = _cuttor->cut(str);	
	for(string word : temp){
		wordFq[word]++;
	}
	
	//while(getline(ifs,line)){
	//	//清洗文件,消除换行符
	//	//line.erase(remove_if(line.begin(),line.end(),[](char c){
	////		return c == '\n';
	////	}));

	//	//注意给指针进行初始化
	//	vector<string> temp = _cuttor->cut(line);	
	//	//获取中文分词，统计词频
	//	for(string word : temp){
	//		wordFq[word]++;
	//	}
	//}	

	//基类指针指向派生类对象，所以会调用虚函数
	ofstream ofs("../data/cnWordDict.dat");
	for(auto elem : wordFq){
		ofs << elem.first << " " << num << "\n";
		_dict.push_back(make_pair(elem.first,num++));	
	}
    
    return;
}

/**
 * @return void
 */
void DictProducer::createIndex() {
	//英文词典，对于每个字母创建一个索引
	//中文词典，对于每个汉字创建一个索引
	//中文字符占3个字节
	for(auto elem : _dict){
		string word = elem.first;
		int index = elem.second;
		for(size_t i = 0; i < word.size();){
			//位运算等级低
			string temp;
			if((word[i] & 0x80) == 0){
				temp = word.substr(i,1);
				++i;
			}else{
				temp = word.substr(i,3);
				i += 3;
			}
cout << temp << endl;
			_index[temp].insert(index); 
		}
	}
    return;
}

/**
 * @return void
 */
void DictProducer::store() {
	//附加模式打开文件
	ofstream ofs("../data/dictIndex.dat",std::ios::app);
	if(!ofs.is_open()){
		cerr << "open file failed\n";
	}

	auto it = _index.begin();
	for(;it != _index.end(); ++it){
		ofs << it->first << " "; 

		for(auto elem : it->second){
			ofs << elem << " ";
		}

		ofs << "\n";
	}
    return;
}

SplitToolCppJieba* SplitToolCppJieba::_pInstance = nullptr;
int main(){
	//基类的指针，指向派生类的对象
	//单例模式,类一加载就提前占用系统资源
	SplitTool* t = SplitToolCppJieba::getInstance();
	DictProducer dp("../conf/wordsCN.txt",t);
	//DictProducer dp2("../data/enWords.txt");

	//dp.buildCnDict();
	//dp2.buildEnDict();

	//dp.createIndex();
	//dp2.createIndex();

	//dp.store();
	//dp2.store();

	return 0;
}

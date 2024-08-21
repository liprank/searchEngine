/**
 * Project SearchEngine
 */

#include "../include/SplitToolCppJieba.h"
#include "../include/DictProducer.h"
#include "../include/Configuration.h"

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

map<string,string> configmap;
DirScanner dirscanner;

/**
 * @param string
 * @param tool
 */
DictProducer::DictProducer(SplitTool* t) 
//基类的指针指向派生类对象，进行重载
:_cuttor(t)
{
	//读取中文语料库，写入_files当中
	dirscanner.traverse(configmap["wordsLibCNDir"]);
	// if(!_files.empty()){
	// 	//清空_files并释放内存
	// 	vector<string>().swap(_files);
	// }

	_files = dirscanner.getFiles();
	dirscanner.clean();

	buildCnDict(configmap["wordsCNdict"]);
	createIndex();
	store(configmap["wordsCNindex"]);

}

//默认构造函数
DictProducer::DictProducer(){
	//读取英文语料库，写入_files当中
	//注意要清空dirscanner中的文件
	dirscanner.traverse(configmap["wordsLibENDir"]);
	_files = dirscanner.getFiles();
	dirscanner.clean();

for(string files : _files){
	cout << files << endl;
}

	buildEnDict(configmap["wordsENdict"]);
	createIndex();
	store(configmap["wordsENindex"]);
}

DictProducer::~DictProducer(){

}

/**
 * @return void
 * 生成英文词典
 */
void DictProducer::buildEnDict(const string &dictfileEN) {
	//英文词典
	//读取文件，清洗写入
	int num = 0;
	map<string,int> wordFq;//统计词频

	while(!_files.empty()){
		string filepath = _files.back();	
		_files.pop_back();

		ifstream files(filepath); 
		if(!files.is_open()){
			cerr << "open file failed\n";
		}

		string line;
		while(getline(files,line)){
			//清洗文件
			for(char &ch : line){
				//line清洗空白字符
				// line.erase(remove_if(line.begin(),line.end(),::isspace),line.end());
				if(isalpha(ch)){
					ch = tolower(ch);
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
				if(word.size() > 2){
					wordFq[word]++;
				}
			}

			//TODO:erase remove的用法 DONE
		}
	}

	//根据词频顺序来放入词典中,而不是原来的顺序
	//存储英文字典
	//TODO: 词频并没有进行排序
	//利用vector来对map进行排序
	vector<pair<string,int>> arr;
	for(const auto &item : wordFq){
		arr.emplace_back(item);
	}

	sort(arr.begin(),arr.end(),[](const auto &x,const auto &y){
		//降序排序，频率高的在前面
		return x.second > y.second;
	});

	ofstream ofs(dictfileEN);

	for(auto elem : arr){
		ofs << elem.first << " " << num << "\n";
		_dict.push_back(make_pair(elem.first,num++));	
		
	}

    return;
}

/**
 * @return void
 * 创建中文词典
 */
void DictProducer::buildCnDict(const string & dictfileCN) {
	//打开文件,读入语料库

	map<string,int> wordFq;//统计词频
	int num = 0;
	while(!_files.empty()){
		string filepath = _files.back();	
		_files.pop_back();
		ifstream ifs(filepath); 
		if(!ifs.is_open()){
			cerr << "open file failed\n";
		}

		//一次读取全部文件
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
			if(word.size() > 2){
				wordFq[word]++;
			}
		}
	
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

	vector<pair<string,int>> arr;
	for(const auto &item : wordFq){
		arr.emplace_back(item);
	}

	sort(arr.begin(),arr.end(),[](const auto &x,const auto &y){
		//降序排序，频率高的在前面
		return x.second > y.second;
	});

	//基类指针指向派生类对象，所以会调用虚函数
	ofstream ofs(dictfileCN);
	for(auto elem : arr){
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
			_index[temp].insert(index); 
		}
	}
    return;
}

/**
 * @return void
 */
void DictProducer::store(const string & indexfile) {
	//附加模式打开文件
	ofstream ofs(indexfile,std::ios::app);
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
Configuration* Configuration::_pInstance = nullptr;

int main(){
	//读取配置文件
	Configuration* config = Configuration::getInstance();
	configmap = config->getConfigMap();

	//通过配置文件来读取语料库文件
	//放入_files当中


	//基类的指针，指向派生类的对象
	//单例模式,类一加载就提前占用系统资源
	SplitTool* t = SplitToolCppJieba::getInstance();
	DictProducer dp1(t);
	DictProducer dp2;

	//dp1.buildCnDict();
	//dp2.buildEnDict();

	//dp.createIndex();
	//dp2.createIndex();

	//dp.store();
	//dp2.store();

	return 0;
}

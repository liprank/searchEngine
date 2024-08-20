/**
 * Project SearchEngine
 */


#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H
#include <string>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

using std::string;
using std::map;
using std::set;
using std::vector;
using std::ifstream;
using std::istringstream;
using std::cerr;
using std::cout;

class Configuration {
public:
	string _configFilePath;
	map<string,string> _configMap;
	set<string> _stopWordList;

	static Configuration* getInstance(){
		if(nullptr == _pInstance){
			_pInstance = new Configuration("../conf/myconf.conf");
		}
		return _pInstance;
	}

	map<string,string>& getConfigMap();
	set<string> getStopWordList();

private:
	Configuration & operator=(const Configuration &rhs) = delete;
	Configuration(const Configuration &rhs) = delete;

	Configuration(const string & filepath)
	:_configFilePath(filepath)	
	{
		
	}

	static Configuration* _pInstance;
};


//输入为目录名，输出为目录中的所有文件
class DirScanner{
private:
    //用于存储目录中所有的文件绝对路径
    vector<string> _files;

public:
    //用于返回_files
    vector<string> &getFiles();

    //用于传递所有文件
    void traverse(string dir);

	//用于清空_files
	void clean();
};

#endif //_CONFIGURATION_H
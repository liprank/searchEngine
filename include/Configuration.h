/**
 * Project SearchEngine
 */


#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <iostream>

using std::string;
using std::map;
using std::set;
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

Configuration* Configuration::_pInstance = nullptr;
#endif //_CONFIGURATION_H
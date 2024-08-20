/**
 * Project SearchEngine
 */


#include "../include/Configuration.h"
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

using std::cout;

/**
 * Configuration implementation
 */

//获取停用词
set<string> Configuration::getStopWordList(){
    string cnStopFile = _configMap["stopwordCN"];
    string enStopFile = _configMap["stopwordEN"];

    ifstream ifsCN(cnStopFile);
    ifstream ifsEN(enStopFile);

    if(!ifsCN.is_open()){
        cerr << "open configFile failed\n";
    }

    if(!ifsEN.is_open()){
        cerr << "open configFile failed\n";
    }

    string line;
    while(getline(ifsCN,line)){
        _stopWordList.insert(line);
	}

    while(getline(ifsEN,line)){
        _stopWordList.insert(line);
	}

    return _stopWordList;
}

map<string,string>& Configuration::getConfigMap(){
        //打开文件读入configMap
		//ip,端口号，要读入的文件
		ifstream ifs(_configFilePath);
		if(!ifs.is_open()){
			cerr << "open configFile failed\n";
		}

		string line;
		while(getline(ifs,line)){
			istringstream iss(line);
			//读取每一行的配置文件
			string key,value;
			iss >> key >> value;
			_configMap[key] = value;
		}

    return _configMap;
}

//Configuration* Configuration::_pInstance = nullptr;

// int main(){
//     Configuration* c = Configuration::getInstance();
//     map<string,string> m = c->getConfigMap();
//     set<string> s = c->getStopWordList();
//     for(string temp : s){
//         cout << temp << "\n";
//     }
// }



vector<string>& DirScanner::getFiles(){
    return _files;
}

void DirScanner::traverse(string path)
{
	DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str()))){
        cout<<"Folder doesn't Exist!"<<"\n";
        return;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0){
            //拼接绝对路径
            _files.push_back(path + "/" + ptr->d_name);
    	}
    }

    // for(string file : _files){
    //     cout << file << endl;
    // }
    closedir(pDir);
}

void DirScanner::clean(){
    //清空_files;
    vector<string>().swap(_files);
}
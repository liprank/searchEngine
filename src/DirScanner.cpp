#include "../include/DirScanner.h"
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <string.h>

using std::cout;
using std::endl;

vector<string>& DirScanner::getFiles(){
    return _files;
}

void DirScanner::traverse(string path)
{
	DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str()))){
        cout<<"Folder doesn't Exist!"<<endl;
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

// int main(){
//     DirScanner d;
//     d.traverse("../conf/wordsLib");
// }
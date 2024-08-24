/**
 * Project SearchEngine
 */


#include "../include/Dictionary.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

using std::ifstream;
using std::istringstream;
using std::cout;
using std::cerr;

/**
 * Dictionary implementation
 */

void Dictionary::init(const string& dictpath,const string& indexpath){
    ifstream ifsdict(dictpath);
    if(!ifsdict.is_open()){
        cerr << "open file failed\n";
    }

    string line;
	while(getline(ifsdict,line)){
		istringstream iss(line);
		//读取每一行的词典信息
		string key,value;
		iss >> key >> value;
           _dict.push_back(make_pair(key,stoi(value)));
	}

    ifstream ifsindex(indexpath);
    if(!ifsindex.is_open()){
        cerr << "open file failed\n";
    }

	while(getline(ifsindex,line)){
		istringstream iss(line);
		//读取每一行的词典信息
		string key;
        int value;
		iss >> key;
        while(iss >> value){
            _index[key].insert(value);
        }
	}
}

//进行查询
vector<string> Dictionary::doQuery(const string &key){
    //找到词典索引,<字，单词索引>
    set<int> index;
    vector<string> queryFind;

    for(char ch : key){
        string s(1,ch);
        //获取每个字的索引
        index = _index[s];
        for(int i : index){
            //根据索引去词典文件中进行查找
            //找到对应词典中的词
            string word = _dict[i].first;
            auto it = find(queryFind.begin(),queryFind.end(),word);
            if(it == queryFind.end()){
                queryFind.push_back(word);
            }
        }
    }
    return queryFind;
}

/**
 * @param key
 * @return vector<string>
 */
// vector<string> Dictionary::doQuery(const string& key) {
//     return null;
// }

/**
 * @return void
 */
// void Dictionary::queryIndex() {
//     return;
// }

size_t nBytesCode(const char ch)
{
    if(ch & (1 << 7)){
        int nBytes = 1;
        for(int idx = 0; idx != 6; ++idx){
            if(ch & (1 << (6 - idx))){
                ++nBytes;
            }
            else
                break;
        } 
        return nBytes;
    } 
    return 1;
}

std::size_t length(const std::string &str)
{
    std::size_t ilen = 0;
    for(std::size_t idx = 0; idx != str.size(); ++idx){
        int nBytes = nBytesCode(str[idx]);
        idx += (nBytes - 1);
        ++ilen;
    }
    return ilen;
}

int triple_min(const int &a, const int &b, const int &c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

/**
 * @param candidate
 * @return int
 */
int Dictionary::distance(string query,string candidate) {
//计算最小编辑距离-包括处理中英文

    string lhs = query;
    string rhs = candidate;

    size_t lhs_len = length(lhs);
    size_t rhs_len = length(rhs);

    int editDist[lhs_len + 1][rhs_len + 1];
    for(size_t idx = 0; idx <= lhs_len; ++idx)
    {
        editDist[idx][0] = idx;
    } 
    for(size_t idx = 0; idx <= rhs_len; ++idx)
    {
        editDist[0][idx] = idx;
    } 

    std::string sublhs, subrhs;
    for(std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i,++lhs_idx)
    {
        size_t nBytes = nBytesCode(lhs[lhs_idx]);
        sublhs = lhs.substr(lhs_idx, nBytes);
        lhs_idx += (nBytes - 1);
        for(std::size_t dist_j = 1, rhs_idx = 0;dist_j <= rhs_len; ++dist_j, ++rhs_idx){
            nBytes = nBytesCode(rhs[rhs_idx]);
            subrhs = rhs.substr(rhs_idx, nBytes);
            rhs_idx += (nBytes - 1);
            if(sublhs == subrhs)
            {
                editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j -1];
            } 
            else
            {
                editDist[dist_i][dist_j] =
                triple_min(editDist[dist_i][dist_j - 1] + 1,
                editDist[dist_i - 1][dist_j] + 1,
                editDist[dist_i - 1][dist_j - 1] + 1);
            }
        }
    } 

    return editDist[lhs_len][rhs_len];
}

// int main(){
//     Dictionary d;

//     //初始化英文字典和索引
//     d.init("../data/wordsENdict.txt","../data/wordsENindex.txt");
//     //vector<string> res = d.doQuery("hello");
//     int t = d.distance("hello","hhhhhh");
//     cout << t << "\n";

//     // for(string str : res){
//     //     cout << str << "\n";
//     // }
// }
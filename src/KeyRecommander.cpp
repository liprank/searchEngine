/**
 * Project SearchEngine
 */


#include "../include/KeyRecommander.h"
#include <iostream>

using std::cout;
/**
 * KeyRecommander implementation
 */

KeyRecommander::KeyRecommander(string word)
:_sought(word)
{
    doQuery();
}

/**
 * @return string
 */
void KeyRecommander::doQuery() {
    //
    Dictionary d;

    //初始化英文字典和索引
    d.init("../data/wordsENdict.txt","../data/wordsENindex.txt");

    //得到候选词
    vector<string> res = d.doQuery(_sought);

    //取得最小编辑距离
    //模板重写prique，定义排序的规则
    Candidate can;
    int num = 0;
    for(string str : res){
        can._word = str;
        can._dist = d.distance(_sought,str);

        //根据位置信息来表示词频，越靠前频率越高
        can._freq = num++;
        //放入优先级队列

        _priQue.push(can);
    }

    //遍历优先级队列
    // while(!_priQue.empty()){
    //     string word = _priQue.top()._word;
    //     _priQue.pop();
    //     cout << "word: " << word << "\n";
    // }
}

// int main(){
//     KeyRecommander("hello");
// }
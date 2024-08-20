/**
 * Project SearchEngine
 */


#include "../include/SplitToolCppJieba.h"
#include "../include/cppjieba/Jieba.hpp"
//#include "../include/DictProducer.h"

using namespace std;
const char* const DICT_PATH = "../conf/dictjieba/jieba.dict.utf8";
const char* const HMM_PATH = "../conf/dictjieba/hmm_model.utf8";
const char* const USER_DICT_PATH = "../conf/dictjieba/user.dict.utf8";
const char* const IDF_PATH = "../conf/dictjieba/idf.utf8";
const char* const STOP_WORD_PATH = "../conf/dictjieba/stop_words.utf8";

/**
 * SplitToolCppJieba implementation
 */

SplitTool::~SplitTool(){

}

/**
 * @return vector<string>
 */
vector<string> SplitToolCppJieba::cut(string sentence) {
   // 构建结巴对象 （操作非常耗时）
    cppjieba::Jieba jieba(DICT_PATH,
                          HMM_PATH,
                          USER_DICT_PATH,
                          IDF_PATH,
                          STOP_WORD_PATH);

	vector<string> words;

    jieba.Cut(sentence, words, true);
    // for(auto word : words){
    //     cout << "word = " << word << "\n";
    // }

	return words;
}

//  SplitToolCppJieba* SplitToolCppJieba::_pInstance = nullptr;
//  int main(){
//       SplitTool* t = SplitToolCppJieba::getInstance();
//       t->cut("你好吃了吗");
//       return 0;
//  }
/**
 * Project SearchEngine
 */


#include "PageLibPreprocessor.h"
#include "../include/myxml.h"
/**
 * PageLibPreprocessor implementation
 */

//进行清洗


/**
 * @param Configuration &conf
 */
// PageLibPreprocessor::PageLibPreprocessor(Configuration &conf) 
// //获取单例
// :_wordCutter(SplitToolCppJieba::getInstance())
// {
    
//}

PageLibPreprocessor::PageLibPreprocessor()
//,_offsetLib()
{
    doProcess(); 
}

/**
 * @return void
 */
// void PageLibPreprocessor::cutRedundantPage() {
//     return;
// }

/**
 * @return void
 */
// void PageLibPreprocessor::bulidInvertIndexMap() {
//     return;
// }

/**
 * @return void
 */
// void PageLibPreprocessor::storeOnDisk() {
//     return;
// }

/**
 * @return void
 */
//读取xml网页，并进行清洗
void PageLibPreprocessor::doProcess() {
	RssReader reader;
	reader.parseRss();             
	reader.dump("../data/webpage.dat");
    return;
}

//SplitToolCppJieba* SplitToolCppJieba::_pInstance = nullptr;
int main(){
    PageLibPreprocessor* p = new PageLibPreprocessor();
    return 0;
}
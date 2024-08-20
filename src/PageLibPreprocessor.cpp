/**
 * Project SearchEngine
 */


#include "../include/PageLibPreprocessor.h"
#include "../include/myxml.h"
#include "../include/Configuration.h" 
/**
 * PageLibPreprocessor implementation
 */

using std::to_string;
//进行清洗
map<string,string> configmap;
DirScanner dirscanner;
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
    //循环读取网页，生成网页偏移库
    dirscanner.traverse(configmap["xmlDir"]);
    vector files = dirscanner.getFiles();
    dirscanner.clean();

int count = 0;
for(string file: files){
    cout << file << endl;

	RssReader reader(file);
	reader.parseRss();             

    //存储文件需要拼接，目录+文件名
	reader.dump(configmap["webpageDir"]+ "/" + to_string(count)+".dat",configmap["webpageOffsetDir"]+ "/" +to_string(count)+".dat");
    count++;
}

    return;
}

Configuration* Configuration::_pInstance = nullptr;
//SplitToolCppJieba* SplitToolCppJieba::_pInstance = nullptr;
int main(){
    Configuration* config = Configuration::getInstance();
    configmap = config->getConfigMap();

    PageLibPreprocessor* p = new PageLibPreprocessor();
    return 0;
}
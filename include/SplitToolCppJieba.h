/**
 * Project SearchEngine
 */


#ifndef _SPLITTOOLCPPJIEBA_H
#define _SPLITTOOLCPPJIEBA_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class SplitToolCppJieba;

class SplitTool {
public:
//用于获取实例
	virtual vector<string> cut(string sentence) = 0;
	virtual ~SplitTool() = 0;
	
};

class SplitToolCppJieba: public SplitTool {
public: 
	static SplitTool* getInstance(){
		if(nullptr == _pInstance){
			_pInstance = new SplitToolCppJieba();
		}
		return _pInstance;
	}

	vector<string> cut(string sentence) override;

private:
//删除拷贝
	SplitToolCppJieba & operator=(const SplitToolCppJieba &rhs) = delete;
	SplitToolCppJieba(const SplitToolCppJieba &rhs) = delete;

	SplitToolCppJieba(){

	}

//重载虚析构
	~SplitToolCppJieba() override{
		if(_pInstance){
			delete _pInstance;
			_pInstance = nullptr;
		}
	}

	static SplitToolCppJieba* _pInstance;
};

#endif //_SPLITTOOLCPPJIEBA_H


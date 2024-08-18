/**
 * Project SearchEngine
 */


#ifndef _WEBPAGE_H
#define _WEBPAGE_H
#include <string>
#include "../include/SplitToolCppJieba.h"

using std::string;

class WebPage {
public:
    string _doc;           //存放整篇文章，包括xml
    int _docID;
    string _docTitle;
    string _docLink;
    string _docDescript;

public:
    WebPage(string &doc);
    WebPage(){

    }

    int getDocID();
    string getDoc();
};

#endif //_WEBPAGE_H
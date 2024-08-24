#include "../include/KeyRecommander.h"
#include "../include/WebPageQuery.h"
#include "../include/LRUCache.h"

#include <workflow/WFHttpServer.h>
#include <workflow/WFFacilities.h>
#include "../include/nlohmann/json.hpp" 
#include <wfrest/HttpServer.h>
#include <workflow/WFResourcePool.h>

#include <signal.h>
#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::cerr;
using std::string;
using std::ifstream;

static WFFacilities::WaitGroup waitGroup(1);

void sigHandler(int sigNum){
    waitGroup.done();
    cout << "done!\n";
}

WFResourcePool pool(1);

void httpCallback(WFHttpTask *httpTask){
    // 请求的信息
    protocol::HttpRequest * req = httpTask->get_req();//get_req 获取请求
    cout << "http request method = " << req->get_method() << "\n"; //get_method 获取请求的方法
    cout << "http request uri = " << req->get_request_uri() << "\n"; //get_uri 获取path和query
    cout << "http request version = " << req->get_http_version() << "\n";//get_http_version 获取http版本
    // 遍历首部字段
    protocol::HttpHeaderCursor reqCursor(req); //protocol::HttpHeaderCursor 是一个访问所有首部字段的迭代器
    string key,value;
    while(reqCursor.next(key,value)){//next是取出下一个首部字段，配合while循环使用
        cout << "key = " << key << " value = " << value << "\n";
    }
    // 响应的信息
    cout << "--------------------------------------------------\n";
    protocol::HttpResponse *resp = httpTask->get_resp();//get_resp 获取响应
    cout << "http response version = " << resp->get_http_version() << "\n"; //get_http_version 获取http版本
    cout << "http response status code = " << resp->get_status_code() << "\n"; //get_status_code 获取状态码
    cout << "http response reason phrase = " << resp->get_reason_phrase() << "\n"; //get_reason_phrase 获取原因字符串
    protocol::HttpHeaderCursor respCursor(resp);
    while(respCursor.next(key,value)){
        cout << "key = " << key << " value = " << value << "\n";
    }

    // workflow框架里面 http报文头和报文体不在一起
    const void *body; //不可修改指向的内容，可以修改指向
    size_t size;
    resp->get_parsed_body(&body,&size);//get_parsed_body 找到报文体内存的首地址
    cout << static_cast<const char *>(body) << "\n";


    //释放资源
    pool.post(nullptr);
}

int main(){
    signal(SIGINT,sigHandler);

    WFHttpTask * httpTask = WFTaskFactory::create_http_task( // 创建任务
        "http://www.baidu.com", // url
        10, // 重定向次数上限
        10, // 重试次数
        //nullptr); //回调函数
        httpCallback); //新的回调函数

    WFConditional *cond = pool.get(httpTask,&httpTask->user_data);
    cond->start();
}
#include "../include/KeyRecommander.h"
#include <workflow/WFHttpServer.h>
#include <workflow/WFFacilities.h>
#include "../include/nlohmann/json.hpp" 
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

void process(WFHttpTask* serverTask){
    //获取请求
    protocol::HttpRequest *req = serverTask->get_req();
    //获取响应
    protocol::HttpResponse *resp = serverTask->get_resp(); 

    //需要判断是推荐词还是搜索网页
    //TODO: http任务之间的区分，get,post有啥区别？
    string uri = req->get_request_uri();
    string CheckKV = uri.substr(uri.find("/")+1);
    string Check = CheckKV.substr(0,CheckKV.find("&")); 

    //获取查询词
    string word = CheckKV.substr(CheckKV.find("&")+1);
    string methodCheck = req->get_method();

    //建立查询词
    KeyRecommander wordQuery(word);

    //query为推荐，search为搜索网页
    if(Check == "query"){
        //如果是推荐词
        //1.接收词，将信息分割为每一个字
        //2.对每一个字在词典中进行查询
        //3.查询结果取并集,形成候选词
        //3.1 如何查询？ 在离线生成的字典库中，先访问字典索引，通过字典索引来访问字典
        //需要增加一个类来处理query
        //4.对候选词进行排序
        //5.以json格式返回候选词
        nlohmann::json json_object;

        //遍历优先级队列
        int count = 0;
        while(count < 10 && !wordQuery._priQue.empty()){
            count++;
            string word = wordQuery._priQue.top()._word;
            wordQuery._priQue.pop();
            json_object.push_back(word);

            cout << "word: " << word << '\n';
        }
        cout << json_object.dump() << "\n";        
        resp->append_output_body(json_object.dump());



    }else if(Check == "search"){
        resp->append_output_body("search");
    }
}

int main(){
    signal(SIGINT,sigHandler);

    // 构建一个httpserver作为服务端
    WFHttpServer server(process);
    if(server.start(12345) == 0){
        waitGroup.wait();
        cout << "finished\n";
        server.stop();
        return 0;
    }else{
        perror("server start failed");
        return -1;
    }
}
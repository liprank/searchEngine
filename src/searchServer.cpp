#include "../include/KeyRecommander.h"
#include "../include/WebPageQuery.h"
#include <workflow/WFHttpServer.h>
#include <workflow/WFFacilities.h>
#include "../include/nlohmann/json.hpp" 
#include <wfrest/HttpServer.h>
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

int main(){
    signal(SIGINT,sigHandler);

    // 构建一个httpserver作为服务端
    wfrest::HttpServer server;

    //get请求用于建立网页
    server.GET("/query",[](const wfrest::HttpReq *req,wfrest::HttpResp *resp){
       
    });

    //用post请求来接收查询
    server.POST("/query",[](const wfrest::HttpReq *req,wfrest::HttpResp *resp){
        //建立查询词
        KeyRecommander wordQuery("你好");
        nlohmann::json json_object;

        //查询缓存，redis



        //遍历优先级队列
        int count = 0;
        while(count < 10 && !wordQuery._priQue.empty()){
            count++;
            string word = wordQuery._priQue.top()._word;
            wordQuery._priQue.pop();
            json_object.push_back(word);
        }
        cout << json_object.dump() << "\n";        
        resp->append_output_body(json_object.dump());
    });

    
    server.POST("/search",[](const wfrest::HttpReq *req,wfrest::HttpResp *resp){
        //网页查询
        WebPageQuery webQuery;

        //获取报文体内容
        map<string,string>  mapForm = req->form_kv();
        for(auto pair:mapForm){
            cout << "key = " << pair.first << " value = " << pair.second << "\n";
        }


        //接收查询
        webQuery.doQuery("还进行");
        //获取文档
        //形成json格式并返回

        resp->append_output_body("search");

        //TODO: json为空 
// cout << webQuery.createJson();
        resp->append_output_body(webQuery.createJson());
    });



     if(server.track().start(12345) == 0){
        // start是非阻塞的
        server.list_routes();
        waitGroup.wait();
        cout << "finished!\n";
        server.stop();
        return 0;
    }
    else{
        perror("server start fail!");
        return -1;
    }
}
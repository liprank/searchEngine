#include "../include/KeyRecommander.h"
#include "../include/WebPageQuery.h"
#include "../include/LRUCache.h"

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

    LRUcache lru;
    lru._capacity = 10;

    // 构建一个httpserver作为服务端
    wfrest::HttpServer server;

    //get请求用于建立网页
    server.GET("/query",[](const wfrest::HttpReq *req,wfrest::HttpResp *resp){
       
    });

    //用post请求来接收查询
    server.POST("/query",[&lru](const wfrest::HttpReq *req,wfrest::HttpResp *resp,SeriesWork* series){
        //建立查询词
        KeyRecommander wordQuery;
        map<string,string>  mapForm = req->form_kv();

        string key , value;

        for(auto pair : mapForm){
            key = pair.first;
            value = pair.second;
            cout << "key = " << pair.first << " value = " << pair.second << "\n";
        }

        string res = lru.get(value);
        nlohmann::json json_object;
        
        if(res == "null"){
            //读取磁盘文件
            resp->String("lru fail\n");
            if(!json_object.is_array()){
                json_object = {};
            }

            KeyRecommander wordQuery(value);

            //遍历优先级队列
            int count = 0;
            while(count < 10 && !wordQuery._priQue.empty()){
                count++;
                string word = wordQuery._priQue.top()._word;
                wordQuery._priQue.pop();
                json_object.push_back(word);

            }
            string json = json_object.dump();

            lru.put(value,json);
            resp->String(json);

        }else{
            resp->String("lru success\n");
            resp->String(res);
        }

        lru.show();
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
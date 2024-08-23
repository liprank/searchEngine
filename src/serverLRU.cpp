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

    //构建一个lru结构
    LRUCache lru(10);

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

        string key,value;

        for(auto pair:mapForm){
            key = pair.first;
            value = pair.second;
            cout << "key = " << pair.first << " value = " << pair.second << "\n";
        }

        // resp->String("hello");
        // wordQuery.doQuery(word);

        nlohmann::json json_object;

        lru.get(value);



#if 0 
//************************* redis ***************************/
        //查询缓存，redis

        WFRedisTask * redisTask = WFTaskFactory::create_redis_task(
            "redis://127.0.0.1:6379",10,
            [resp,&wordQuery,value,&json_object](WFRedisTask *redisTask){
                protocol::RedisResponse *respRedis = redisTask->get_resp();
                protocol::RedisValue result;
                respRedis->get_result(result);
                //结果为list

                if(result.is_array() && result.arr_size() > 0){
                    // const auto& array = respRedis->; 
                    cout << "redis success\n";

                    for(int i = 0; i < result.arr_size(); ++i){
                        //TODO: 为什么push_back当中不加string会报错？
                        string res = result.arr_at(i).string_value();
                        json_object.push_back(string(res));
                    }

                    resp->String(json_object.dump());
                }
                else{
                    //如果查询缓存失败，则查询磁盘
                    resp->String("redis fail\n");
                    //查询磁盘
                    cout << "需要访问磁盘\n";

//TODO: 为什么无法查找到磁盘文件？
cout << "value: " << value << "\n";

                    wordQuery.doQuery(value);
                    //遍历优先级队列
                    int count = 0;
                    while(count < 10 && !wordQuery._priQue.empty()){
                        count++;
                        string word = wordQuery._priQue.top()._word;
                        wordQuery._priQue.pop();
                        json_object.push_back(word);
                    }

                    resp->String(json_object.dump());
                }
            }
        );
#endif
        //在缓存中进行查询
        redisTask->get_req()->set_request("lrange",{value,"0","-1"});
        series->push_back(redisTask);
   });

#if 0
    server.POST("/search",[](const wfrest::HttpReq *req,wfrest::HttpResp *resp){
        //网页查询
        WebPageQuery webQuery;

        //获取报文体内容
        //查询词放在报文体当中
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
#endif

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
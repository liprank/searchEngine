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
#include <chrono>
#include <thread>

using std::cout;
using std::cerr;
using std::string;
using std::ifstream;

static WFFacilities::WaitGroup waitGroup(1);

void sigHandler(int sigNum){
    waitGroup.done();
    cout << "done!\n";
}

// void timerfunction(){

// }


int main(){
    signal(SIGINT,sigHandler);

    //初始化lru数组
    size_t poolsize = 20;
    LRUcache* lrus[poolsize];

    for(int i = 0; i < poolsize; ++i){
        lrus[i] = new LRUcache(10);
        cout << "lrus[i] = " << lrus[i] << "\n";
    }

    //创建资源池
    WFResourcePool pool((void* const*)lrus,poolsize);

    auto gotask = WFTaskFactory::create_go_task("task",[](){});
    gotask->set_callback([&pool,poolsize](WFGoTask *gotask){
        while(true){
            // 使用 chrono 库中的秒来定义要睡眠的时间长度
            std::chrono::seconds duration(3);
    
            // 调用 std::this_thread::sleep_for 函数来睡眠指定的时间长度
            std::this_thread::sleep_for(duration);
            WFTimerTask *timer_task = WFTaskFactory::create_timer_task(3,0,[&pool,poolsize](WFTimerTask* task){

           cout << "timeout execute callback\n";
           //获取所有的lru位置，取出pendinglist
           // list<pair<string,string>> update = lrus[0]->_pendingList;

           //通过空的任务来获取资源池中的lru

           //将pendinglist当中的元素加入到set当中
           list<pair<string,string>> update;
           for(int i = 0; i < poolsize; ++i){
               //创建条件任务
               auto gotask = WFTaskFactory::create_go_task("task",[](){});
               auto condtask = pool.get(gotask,&gotask->user_data);
               gotask->set_callback([&pool,&update](WFGoTask *gotask){

                   LRUcache* lru = (LRUcache*) (gotask->user_data);            

                   for(auto elem : lru->_pendingList){
                       update.push_back(elem);
                   } 

               });
               pool.post(gotask->user_data);
          }

           //对所有lru进行同步
           for(int i = 0; i < poolsize; ++i){
               auto gotask = WFTaskFactory::create_go_task("task",[](){});
               auto condtask = pool.get(gotask,&gotask->user_data);
               gotask->set_callback([&pool,&update,poolsize](WFGoTask *gotask){
                   LRUcache* lru = (LRUcache*) (gotask->user_data);            
                   if(update.size()){
                       for(int i = 0; i < poolsize; ++i){
                           for(auto elem : update){
                               lru->put(elem.first,elem.second);
                           }
                       }
                   }
               });

                pool.post(gotask->user_data);
            }
        });

        timer_task->start();
        }
    });

    gotask->start();
    

    // 构建一个httpserver作为服务端
    wfrest::HttpServer server;

    //get请求用于建立网页
    server.GET("/query",[](const wfrest::HttpReq *req,wfrest::HttpResp *resp){
       
    });

    //用post请求来接收查询
    server.POST("/query",[&lrus,&pool](const wfrest::HttpReq *req,wfrest::HttpResp *resp,SeriesWork* series){
        //建立查询词
        //使用条件任务来获取资源
        map<string,string>  mapForm = req->form_kv();
        string key,value;

        KeyRecommander wordQuery;

        for(auto pair : mapForm){
            key = pair.first;
            value = pair.second;
            cout << "key = " << pair.first << " value = " << pair.second << "\n";
        }

        auto gotask = WFTaskFactory::create_go_task("task",[](){});
        auto condtask = pool.get(gotask,&gotask->user_data);
        gotask->set_callback([&pool,resp,&wordQuery,value](WFGoTask *gotask){

            cout << "gotask -> user_data " << gotask->user_data << "\n";
            LRUcache* lru = (LRUcache*) (gotask->user_data);            

            if(lru == nullptr){
                cout << "lru is null\n";
            }
        //获取一个lru资源，并对其进行访问
        //如果访问失败，则读取磁盘，并且存入lru
            KeyRecommander wordQuery;
            nlohmann::json json_object;
    
            string res = lru->get(value);

            if(res == "null"){
                //读取磁盘文件
                resp->String("lru fail\n");
                if(!json_object.is_array()){
                    json_object = {};
                }
    
                wordQuery.doQuery(value);

                //遍历优先级队列
                int count = 0;
                while(count < 10 && !wordQuery._priQue.empty()){
                    count++;
                    string word = wordQuery._priQue.top()._word;
                    wordQuery._priQue.pop();
                    json_object.push_back(word);
                }
    
                string json = json_object.dump();
    
                lru->put(value,json);
                resp->String(json);
    
            }else{
                resp->String("lru success\n");
                resp->String(res);
            }
    
            //归还lru节点
            //不用释放lru
            pool.post(gotask->user_data);
    
            lru->show();
        });

        series->push_back(condtask);
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
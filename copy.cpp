#include <workflow/WFHttpServer.h>
#include <workflow/WFFacilities.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::cerr;
using std::string;
using std::ifstream;

static WFFacilities::WaitGroup waitGroup(1);

struct SeriesContext{
    string username;
    string passwd;
    protocol::HttpResponse *resp;
};

void sigHandler(int sigNum){
    waitGroup.done();
    cout << "done!\n";
}

void redisCallback(WFRedisTask* redisTask){
    protocol::RedisResponse *resp = redisTask->get_resp();
    protocol::RedisValue result;
    resp->get_result(result);

    //需要增加注册验证功能？
    SeriesContext *context = static_cast<SeriesContext*> (series_of(redisTask)->get_context());
    //TODO:需要后续增加
    //context->resp->append_output_body("sucess");
}

void process(WFHttpTask* serverTask){
    //获取请求
    protocol::HttpRequest *req = serverTask->get_req();
    //获取响应
    protocol::HttpResponse *resp = serverTask->get_resp(); 

    //检测是否为登录login关键字
    string uri = req->get_request_uri();
    string loginCheck = uri.substr(uri.find("/")+1);
    string methodCheck = req->get_method();
    if(loginCheck == "login" && methodCheck != "POST"){
        cout << loginCheck << "\n";
        cout << "login success\n";
        //访问静态资源
        const char* filepath = "/home/yang/homework/WorkFlow/website.html";
        ifstream ifs(filepath);

        if(!ifs){
            cerr << "ifstream open file failed\n";
            return;
        }

        //获取文件有多少个字符
        ifs.seekg(0,std::ios::end);
        size_t len = ifs.tellg();
        ifs.seekg(0,std::ios::beg);

        char *pdata = new char[len+1]();
        ifs.read(pdata,len);

        resp->append_output_body(pdata,len+1);
        delete pdata;
        ifs.close();
    }else if(methodCheck == "POST"){//如果登陆关键字，而是为post请求，则接收用户名和密码，加入redis当中
        //解析post请求
        cout << loginCheck << "\n";
        cout << "get post request\n";
        const void *body;
        size_t size;
        req->get_parsed_body(&body,&size);//获取报文体
        string bodyContext = static_cast<const char*> (body);

        //解析用户名和密码
        string usernameR = bodyContext.substr(0,bodyContext.find("&")); 
        string passwdR = bodyContext.substr(bodyContext.find("&")+1);
        string name = usernameR.substr(usernameR.find("=")+1);
        string passwd = passwdR.substr(passwdR.find("=")+1);
        cout << "name = " << name << "\n";
        cout << "passwd = " << passwd << "\n";
        //添加redis hset任务
         WFRedisTask* redisTask = WFTaskFactory::create_redis_task(
             "redis://127.0.0.1:6379",
             10,
             redisCallback
         );

         //redis中注册用户名密码
         redisTask->get_req()->set_request("HSET",{"users",name,passwd});
         series_of(serverTask)->push_back(redisTask);
    }
}

int main(){
    signal(SIGINT,sigHandler);

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

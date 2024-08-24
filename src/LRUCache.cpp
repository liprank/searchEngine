/**
 * Project SearchEngine
 */


#include "../include/LRUCache.h"
#include <iostream>

using std::cout;
/**
 * LRUCache implementation
 */


/**
 * @param filename
 * @return void
 */
//如果没有命中LRU则读取磁盘文件
// void LRUCache::readFromFile(string& filename) {
//     return;
// }

/**
 * @param key
 * @param json
 * @return void
 */
//增加结点
//当加入节点的时候，同时加入到pendinglist当中用于多个线程的同步
int LRUcache::put(string key, string json) {

    //插入到pengdinglist当中
    _pendingList.push_front(make_pair(key,json));

    unordered_map<string, list<CacheNode>::iterator>::iterator it;
    if((it = _cache.find(key)) !=_cache.end())//如果该key已经存在,直接放在链表头
    {
        it->second->value = json;
        _nodes.splice(_nodes.begin(), _nodes, it->second);
    }
    else//如果该key不存在
    {
        if(_capacity == _nodes.size())//看链表是不是满的，满的话需要删除链表尾元素
        {
            auto &deleteNode = _nodes.back();//找到待删除节点
            _cache.erase(deleteNode.key);//在unordered_map中删除
            _nodes.pop_back();//在list中删除
        }
        //不论满还是不满，直接在链表前面插入并插入到unordered_map中
        _nodes.push_front(CacheNode(key, json));
        _cache.insert(std::make_pair(key, _nodes.begin()));
    }

    return 0;
}

/**
 * @param const LRUCache&
 * @return void
 */
//更新结点
string LRUcache::get(string key) {
    auto it = _cache.find(key);//查询该key在不在unordered_map中
    if(it == _cache.end())//不存在直接返回-1
    {
        return "null";
    }
    else//存在就放在链表头部,并返回其值
    {
        _nodes.splice(_nodes.begin(), _nodes, it->second);
        return it->second->value;
    }
}

void LRUcache::show(){
    for(auto elem : _nodes){
        cout << elem.key << " " << elem.value << "\n";
    }
}


/**
 * @param filename
 * @return void
 */
//写入文件
// void LRUCache::writeToFile(string& filename) {
//     return;
// }

// int main(){
//     LRUcache  lr(2);
//     lr.put("hello","000000001111");

//     cout << "get(hello) = " << lr.get("hello") << "\n";

//     lr.put("nihao","1111000000000");
//     cout << "get(nihao) = " << lr.get("nihao") << "\n";

//     lr.put("tank","222222222222");
//     cout << "get(hello) = " << lr.get("hello") << "\n";
// }
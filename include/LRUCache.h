/**
 * Project SearchEngine
 */


#ifndef _LRUCACHE_H
#define _LRUCACHE_H
#include <string>
#include <list>
#include <unordered_map>

using std::pair;
using std::unordered_map;
using std::list;

using std::string;

class LRUcache {
public: 
    LRUcache(int capacity)
    : _capacity(capacity)
    {
    }

    LRUcache(){

    }

    int put(string key, string json);
    
    string get(string key);
    
/**
 * @param filename
 */
// void writeToFile(string& filename);

    void show();

public:
    int _capacity; //记录缓存的大小

private:
    struct CacheNode{
        //word,json
        CacheNode(string k,string v)
        :key(k) 
        ,value(v) 
        {

        }

        string key;
        string value;
    };

public:
    list<pair<string,string>> _pendingList; //用于同步
private: 
    list<CacheNode> _nodes; //节点放在list链表之中
    unordered_map<string,list<CacheNode>::iterator> _cache;//存放key值，以及在链表中的位置
};

#endif //_LRUCACHE_H
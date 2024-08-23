#include <list>
#include <unordered_map>
#include <string>

using std::list;
using std::unordered_map;
using std::string;

class LRU
{
public:
    LRU(int capacity)
    : _capacity(capacity)
    {
    }

    int put(int key, int value);//插入数据
    int get(int key);//查找该key

private:
    struct CacheNode
    {
        //cachenode当中存放的结点应该是一个<string,vector<string>>格式的类型
        //为了方便起见，vector<string> 用json格式压成string类型
        CacheNode(int k, int v)
        : key(k)
        , value(v)
        {

        }

        int key;
        int value;
    };
private:
    int _capacity;//缓存的大小
    list<CacheNode> _nodes;//节点放在list链表之中
    unordered_map<int, list<CacheNode>::iterator> _cache;//存储key值以及在链表中的位置
};
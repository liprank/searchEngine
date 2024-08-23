#include "../include/LRU.h"

#include <iostream>
#include <list>
#include <unordered_map>

using std::cout;
using std::endl;
using std::list;
using std::unordered_map;

int LRU::put(int key, int value)//插入数据
{
    unordered_map<int, list<CacheNode>::iterator>::iterator it;
    if((it = _cache.find(key)) !=_cache.end())//如果该key已经存在,直接放在链表头
    {
        it->second->value = value;
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
        _nodes.push_front(CacheNode(key, value));
        _cache.insert(std::make_pair(key, _nodes.begin()));
    }
    return 0;
}

int LRU::get(int key)//查找该key
{
    auto it = _cache.find(key);//查询该key在不在unordered_map中
    if(it == _cache.end())//不存在直接返回-1
    {
        return -1;
    }
    else//存在就放在链表头部,并返回其值
    {
        _nodes.splice(_nodes.begin(), _nodes, it->second);
        return it->second->value;
    }
}

int main(int argc, char **argv)
{
    LRU lr(2);
    lr.put(1, 1);
    cout << "get(1) = " << lr.get(1) << endl;

    lr.put(3, 3);
    cout << "get(2) = " << lr.get(2) << endl;

    lr.put(4, 4);
    cout << "get(1) = " << lr.get(1) << endl;

    cout << "get(3) = " << lr.get(3) << endl;
    cout << "get(4) = " << lr.get(4) << endl;
    return 0;
}

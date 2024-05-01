/*
 * @Author: rubo
 * @Date: 2024-05-01 10:37:36
 * @LastEditors: HUAWEI-Ubuntu ruluy0205@163.com
 * @LastEditTime: 2024-05-01 20:21:43
 * @FilePath: /MySkipList/src/skipList.h
 * @Description: 基于C++11实现的KV存储引擎核心代码
 */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <mutex>
#include <vector>
#include <memory>

// 转存文件名
#define STORE_FILE "../config/dumpFile"

namespace node
{
    // 定义KV存储节点
    template <typename K, typename V>
    class Node
    {
    public:
        Node(){}
        ~Node(){}

        Node(const K k, const V v, int level)
            :key(k), value(v), node_level(level),
            forward(level, nullptr){}
        
        K get_key() const { return key;}
        V get_value() const { return value;}
        void set_value(V value) { value = value;}

        //存储每一层的下一个节点, 引入智能指针 shared_ptr 省去析构
        std::vector<std::shared_ptr<Node<K,V>>> forward;
        //节点层级
        int node_level;

    private:
        K key;
        V value;
    };

    template<typename K, typename V>
    using NodeVec = std::vector<std::shared_ptr<Node<K,V>>>;

} // namespace Node

namespace skiplist
{
    std::string delimiter = ":";
    //定义跳表类
    template<typename K, typename V>
    class SkipList{
    public:
        SkipList(int);
        ~SkipList();

        int get_random_level() const;
        std::shared_ptr<node::Node<K, V>> create_node(K, V, int);
        int insert_element(K, V);
        void print_list() const;
        bool search_element(K) const;
        void delete_element(K);
        void dumpFile();
        void loadFile();
        int size() const { return element_count; }
        
    private:
        void get_key_value_from_string(const std::string &str, std::string &key, std::string &value) const;
        bool isValid_string(const std::string &str) const;

    private:
        int max_level;
        int current_level;
        std::shared_ptr<node::Node<K, V>> header;
        std::ofstream file_writer;
        std::ifstream file_reader;
        int element_count;          //节点个数
        std::mutex mutex;

    };

    //构造函数初始化成员
    template<typename K, typename V>
    SkipList<K, V>::SkipList(int max_level)
        :max_level(max_level), current_level(0), element_count(0)
    {
        header = std::make_shared<node::Node<K, V>>(k(), V(), max_value);
    }




} // namespace SkipList

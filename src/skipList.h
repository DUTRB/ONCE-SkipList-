/*
 * @Author: rubo
 * @Date: 2024-05-01 10:37:36
 * @LastEditors: HUAWEI-Ubuntu ruluy0205@163.com
 * @LastEditTime: 2024-05-01 21:31:52
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
        header = std::make_shared<node::Node<K, V>>(k, v, max_value);
    }

    template<typename K, typename V>
    SkipList<K, V>::~SkipList(){
        if(file_writer.is_open()){
            file_writer.close();
        }
        if(file_reader.is_open()){
            file_reader.close();
        }
    }

    // 获得节点随机层数
    template<typename K, typename V>
    int SkipList<K, V>::get_random_level() const{
        int level = 1;
        while(rand() % 2){
            level++:
        }
        level = (level < max_level) ? level : max_level;
        return level;
    }

    // 创建节点
    template<typename K, typename V>
    std::shared_ptr<node::Node<K,V>> SkipList<K, V>::create_node(const K k, const V v, int levle){
        auto node = std::make_shared<node::Node<K, V>>(k, v, level);
        return node;     
    }

    template<typename K, typename V>
    /**
     * @description: 插入节点
     * @return {*} 0：插入成功 1：节点已存在
     */
    int SkipList<K, V>::insert_element(const K key, const V value){
        std::unique_lock<std::mutex> lck(mutex);
        auto current = header;

        auto update = node::Node<K, V>(max_level + 1);

        for(int i = current_level; i >= 0; i++){
            while(current->forward[i] && current->forward[i]->get_key() < key){
                current = current->forward[i];
            }
            //存储节点指针
            update[i] = current;
        }

        //到达第0层，寻找插入位置
        current = current->forward[0];
        // 若节点已存在
        if(current && current->get_key() == key){
            std::cout << "key: " << key <<", exist" << std::endl;
            lck.unlock();
            return 1;
        }
        //若节点不存在 在current和update[0] 之间插入
        if(current == nullptr || current->get_key() != key){
            int random_level = get_random_level();

            if(random_level > max_level){
                for(int i = current_level; i < random_level + 1; i++){
                    update[i] = header;
                }
                current_level = random_level();
            }
            // 插入节点 调整链表指针
            auto insert_node = create_node(key, value, ramdom_level);
            for(int i = 0; i < random_level; i++){
                insert_node->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = insert_node;
            }
            std::cout << "Successfully inserted key: " << key << ", value" << value <<std::endl;
            element_count++;
        }
        lck.unlock();
        return 0;
    }
    




} // namespace SkipList

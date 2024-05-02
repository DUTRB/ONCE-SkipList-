/*
 * @Author: rubo
 * @Date: 2024-05-01 10:37:36
 * @LastEditors: HUAWEI-Ubuntu ruluy0205@163.com
 * @LastEditTime: 2024-05-02 14:03:32
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

        // 实现跳表操作的基本功能函数
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
        // 文件转存相关函数
        void get_key_value_from_string(const std::string &str, std::string &key, std::string &value) const;
        bool isValid_string(const std::string &str) const;

    private:
        int max_level;              // 最大层级
        int current_level;          // 当前层级
        std::shared_ptr<node::Node<K, V>> header;
        std::ofstream file_writer;
        std::ifstream file_reader;
        int element_count;          // 节点个数
        std::mutex mutex;           // 互斥锁

    };

    //构造函数初始化成员
    template<typename K, typename V>
    SkipList<K, V>::SkipList(int max_level)
        :max_level(max_level), current_level(0), element_count(0)
    {
        header = std::make_shared<node::Node<K, V>>(K(), V(), max_level);
    }
    // 析构函数 释放文件操作
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
            level++;
        }
        level = (level < max_level) ? level : max_level;
        return level;
    }

    // 创建节点
    template<typename K, typename V>
    std::shared_ptr<node::Node<K,V>> SkipList<K, V>::create_node(const K k, const V v, int level){
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

        auto update = node::NodeVec<K, V>(max_level + 1);

        for(int i = current_level; i >= 0; i--){
            while(current->forward[i] != nullptr && current->forward[i]->get_key() < key){
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

            if(random_level > current_level){
                for(int i = current_level + 1; i < random_level + 1; i++){
                    update[i] = header;
                }
                current_level = random_level;
            }
            // 插入节点 调整链表指针
            auto insert_node = create_node(key, value, random_level);
            for(int i = 0; i < random_level; i++){
                insert_node->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = insert_node;
            }
            std::cout << "Successfully inserted key: " << key << ", value: " << value <<std::endl;
            element_count++;
        }
        lck.unlock();
        return 0;
    }
    
    template<typename K, typename V>
    /**
     * @description: 打印跳表所有节点
     * @return {*}
     */
    void SkipList<K, V>::print_list() const{
        std::cout << "\n*****Skip list*******" << "\n";
        for(int i = 0; i <= current_level; i++){
            auto node = header->forward[i];
            std::cout << "Level " << i << ": ";
            while(node != nullptr){
                std::cout << node->get_key() << ":" <<node->get_value() << ";";
                node = node->forward[i];
            }
            std::cout << std::endl;
        }
    }

    // 文件转存
    template<typename K, typename V>
    void SkipList<K, V>::dumpFile(){
        std::cout << "dump_file******************" << std::endl;
        file_writer.open(STORE_FILE);
        auto node = header->forward[0];
        while(node != nullptr){
            file_writer << node->get_key() << ":" << node->get_value() << "\n";
            std:: cout << node->get_key() << ":" << node->get_value() << "\n";
            node = node->forward[0];
        }
        file_writer.flush();
        file_writer.close();
        return ;
    }

    template<typename K, typename V>
    void SkipList<K, V>::loadFile(){
        file_reader.open(STORE_FILE);
        std::cout << "load_file*****************" << std::endl;
        std::string str;
        std::string key;
        std::string value;
        while(getline(file_reader,str)){
            get_key_value_from_string(str, key, value);
            if(key.empty() || value.empty()){
                continue;
            }
            insert_element(key, value);
            std::cout << "key: " << key << " value: " << value << std::endl;
        }
        file_reader.close();
    }

    template<typename K, typename V>
    void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string& key, std::string& value) const{
        if(!isValid_string(str)){
            return;
        }
        // 获取 key 和 value
        key = str.substr(0, str.find(delimiter));
        value = str.substr(str.find(delimiter) + 1, str.length());
    }   

    template<typename K, typename V>
    bool SkipList<K, V>::isValid_string(const std::string& str) const{
        if(str.empty()) return false;
        if(str.find(delimiter) == std::string::npos) return false;
        return true;
    }


    template<typename K, typename V>
    /**
     * @description: 删除节点
     * @return {*}
     */
    void SkipList<K, V>::delete_element(K key){
        std::unique_lock<std::mutex> lck(mutex);
        auto current = header;
        auto update = node::NodeVec<K, V>(max_level + 1);
        for(int i = current_level; i >= 0; i--){
            while(current->forward[i] && current->forward[i]->get_key() < key){
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];
        // 当删除节点存在时
        if(current && current->get_key() == key){
            for(int i = 0; i <= current_level; i++){
                if(update[i]->forward[i] != current){
                    break;
                }
                update[i]->forward[i] = current->forward[i];
            }
            while(current_level > 0 && header->forward[current_level] == 0){
                current_level--;
            }

            std::cout << "Successful delete key " << key << std::endl;
            element_count--;
        }
        lck.unlock();
        return ;
    }

    template<typename K, typename V>
    /**
     * @description: 根据 key 查找节点
     * @return {*}
     */
    bool SkipList<K, V>::search_element(K key) const{
        std::cout << "search_element **************" << std::endl;
        auto current = header;

        for(int i = current_level; i >= 0; i--){
            while(current->forward[i] && current->forward[i]->get_key() < key){
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        if(current && current->get_key() == key){
            std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
            return true;
        }
        std::cout << "Not found" << key << std::endl;
        return false;
    }


} // namespace SkipList

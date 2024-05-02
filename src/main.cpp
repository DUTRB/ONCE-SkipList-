/*
 * @Author: rubo
 * @Date: 2024-05-02 11:51:32
 * @LastEditors: HUAWEI-Ubuntu ruluy0205@163.com
 * @LastEditTime: 2024-05-02 13:49:57
 * @FilePath: /MySkipList/src/main.cpp
 * @Description: 
 */
#include <iostream>
#include "skipList.h"
#define FILE_PATH "../config/dumpFile"

int main() {
  skiplist::SkipList<int, std::string> skip_list(6);
  // key use int, if use other type, need define compare func
  // if want to modify type of key, modify LoadFile func as well
  skip_list.insert_element(1, "one"); 
  skip_list.insert_element(3, "three"); 
  skip_list.insert_element(7, "seven"); 
  skip_list.insert_element(8, "eight"); 
  skip_list.insert_element(9, "nine"); 
  skip_list.insert_element(19, "ten"); 
  skip_list.insert_element(19, "ninteen"); 

  std::cout << "skip_list size:" << skip_list.size() << std::endl;

  skip_list.dumpFile();

  //skip_list.loadFile();

  skip_list.search_element(9);
  skip_list.search_element(18);

  skip_list.print_list();

  skip_list.delete_element(3);
  skip_list.delete_element(7);

  std::cout << "skip_list size:" << skip_list.size() << std::endl;

  skip_list.print_list();
  
}

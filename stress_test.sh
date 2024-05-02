###
 # @Author: rubo
 # @Date: 2024-04-30 15:25:11
 # @LastEditors: HUAWEI-Ubuntu ruluy0205@163.com
 # @LastEditTime: 2024-05-02 14:26:08
 # @FilePath: /MySkipList/stress_test.sh
 # @Description: 
### 
#!/bin/bash
g++ test/stress_test.cpp -o ./bin/stress  --std=c++11 -pthread  
./bin/stress

//
//  main.cpp
//  cpp11atomicsample
//
//  Created by pebble8888 on 2016/07/31.
//  Copyright © 2016年 pebble8888. All rights reserved.
//

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

uint8_t nonatomic_value;

void process1() {
    bool flag = false;
    for (int i = 0; i < 1000000; ++i){
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        if (flag){
            nonatomic_value = 0xff; 
        } else {
            nonatomic_value = 0x0;
        }
        flag = !flag;
    }
}

void process2() {
    for (int i = 0; i < 1000000; ++i){
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        if (nonatomic_value != 0xff &&
            nonatomic_value != 0x0){
            printf("warningA! nonatomic_value %x\n", nonatomic_value);
        }
    }
}

int main(int argc, char** argv) {
    printf("start address %p\n", &nonatomic_value);
    nonatomic_value = 0;
    std::thread t1(process1);
    std::thread t2(process2);
    t1.join();
    t2.join();
    
    printf("end\n");
    return 0;
}
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
#include <mutex>
#include "StopWatch.h"
#include <libkern/OSAtomic.h>

#include <Accelerate/Accelerate.h>

std::atomic_short value = ATOMIC_VAR_INIT(0);
struct Hoge {
    int64_t a;
    int64_t b;
};

Hoge hoge1 = { 25, 26 };
Hoge hoge2;

//int16_t value = 0;
//std::mutex mtx;
//OSSpinLock lock = OS_SPINLOCK_INIT;

void process1() {
    bool flag = false;
    for (int i = 0; i < 100000; ++i){
        std::this_thread::yield;
        if (flag){
            value.store(255, std::memory_order_release);
            //std::lock_guard<std::mutex> lk(mtx);
            //OSSpinLockLock(&lock);
            //value = 32767;
            //OSSpinLockUnlock(&lock);
        } else {
            value.store(0, std::memory_order_release);
            //std::lock_guard<std::mutex> lk(mtx);
            //OSSpinLockLock(&lock);
            //value = 0;
            //OSSpinLockUnlock(&lock);
        }
        flag = !flag;
    }
}

void func1(Hoge hoge){
    hoge2 = hoge;
}

void func2(const Hoge& hoge){
    hoge2 = hoge;
}

void process2() {
    for (int i = 0; i < 100000; ++i){
        std::this_thread::yield;
        func1(hoge1);
        //func2(hoge1);
        //OSSpinLockLock(&lock);
        {
            //std::lock_guard<std::mutex> lk(mtx);
            //int16_t v = value;
            //int16_t v = value.load(std::memory_order_acquire);
            
            //vDSP_vadd(a, 1, b, 1, a, 1, 512);
            //OSSpinLockUnlock(&lock);
            //if (v != 32767 && v != 0){
            /*
            if (value != 32767 && value != 0){
                printf("warningA! i %d v %d\n", i, value);
            }
             */
        }
    }
}

int main(int argc, char** argv) {
    long min = 10000000;
    long max = 0;
    long sum = 0;
    long count = 20;
    for (int i = 0; i < count; ++i){
        //value.store(0);
        StopWatch sw;
        sw.start();
        //std::thread t1(process1);
        std::thread t2(process2);
        //t1.join();
        t2.join();
        sw.stop();
        //printf("%s\n", sw.description().c_str());
        long us = sw.microseconds();
        //printf("%ld\n", us);
        
        if (us < min){
            min = us;
        }
        if (max < us){
            max = us;
        }
        sum += us;
    }
    printf("min %d us max %d us mean %d us\n", min, max, sum/count);
    
    return 0;
}
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
#include "SLManagedPointer.h"

struct Hoge {
    float a;
    float b;
};

SLManagedPointer<Hoge> g_hoge;

int main(int argc, char** argv) {
    /*
    long min = 10000000;
    long max = 0;
    long sum = 0;
    long count = 20;
    for (int i = 0; i < count; ++i){
        StopWatch sw;
        sw.start();
        
        
        sw.stop();
        long us = sw.microseconds();
        if (us < min){
            min = us;
        }
        if (max < us){
            max = us;
        }
        sum += us;
    }
    printf("min %d us max %d us mean %d us\n", min, max, sum/count);
     */
    
    Hoge* p = g_hoge.val();
    assert(p==nullptr);
    
    bool ret;
    
    ret = g_hoge.try_increment();
    assert(!ret);
    
    g_hoge.setVal(new Hoge);
    
    ret = g_hoge.try_increment();
    assert(ret);
    
    g_hoge.decrement();
    
    return 0;
}
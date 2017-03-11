//
//  SLManagedPointer.h
//
//  Copyright © 2016年 Yamaha Corporation. All rights reserved.
//

#ifndef SLManagedPointer_h
#define SLManagedPointer_h

/**
 * @brief
 */
template <typename T>
class SLManagedPointer
{
    /*
     * A) _val へ nullptr 以外を設定する時
     *
     *    1) _val を設定する.
     *    2) _refcount に0を設定する.
     *
     * B) _val へ nullptr を設定する時
     *
     *    1) _refcount が0になるのを待ち、なったタイミングで _refcountを -1 に変更する.
     *    2) _val へ nullptr を設定する.
     *
     * C) _valへアクセスする際は _refcount をカウントアップし、アクセスが終わったらカウントダウンする.
     *
     *
     * A,B,C)の操作により
     *      i) _refcount が 1以上の時、どこかのスレッドが処理中である.
     *     ii) _refcount が 0の時、どこのスレッドも処理中ではなく、_val には nullptr 以外が入っている.
     *    iii) _refcount が -1の時、どこのスレッドも処理中ではなく、_val には nullptr が入っている.
     */
    std::atomic<T*> _val = ATOMIC_VAR_INIT(nullptr);
    std::atomic<int> _refcount = ATOMIC_VAR_INIT(-1);
    //spinlock _spinlock;
public:
    SLManagedPointer()
    {
    }
    
    ~SLManagedPointer()
    {
        setVal(nullptr);
    }
    
    T* val(void)
    {
        return _val.load(std::memory_order_acquire);
    }
    
    /**
     * @brief valのポインタ値をセットする
     * @param T* val  このポインタは後でdeleteするので、newで生成されていることを前提とする.
     * @note  リアルタイムスレッドからは呼ばれない
     */
    void setVal(T* val)
    {
        if (_refcount.load(std::memory_order_acquire) >= 0){
            // _valに入っている値がnullptrではなく,リアルタイムスレッドでそのポインタ値にアクセス中の場合.
            // _refcountの値が0になるのを待ってから-1に変更する. 
            // refcountが0の場合もポインタをdeleteする必要があるので、ここで処理する.
            // これが呼ばれるのはリアルタイムスレッドではないので、ここでスピンロックしてよい.
            // リアルタイムスレッドでのアクセスが終わるまでここでスピンロックする.
            int expect = 0;
            int new_value = -1;
            while(_refcount.compare_exchange_weak(expect, new_value, std::memory_order_release));
            // ここで_refcountは-1になっているので、以後、_valへアクセスされる恐れはない.
            // 時間がかかるので、リアルタイムスレッド以外でdeleteする.
            delete _val.load(std::memory_order_acquire);
        }
        // _valの値をセットしてから_refcountの値を0にする.この順序でないとダメ.
        _val.store(val, std::memory_order_release);
        _refcount.store(0, std::memory_order_release);
    }
    
    /**
     * @brief ポインタにnullptr以外の値が入っており、アクセス可能な場合、
     *        リファレンスカウントを1上げてからtrueを返す.
     *        ポインタにnullptrが入っている場合はfalseを返す.
     */
    bool try_increment(void)
    {
        if (_refcount.load(std::memory_order_acquire) >= 0){
            _refcount.fetch_add(1, std::memory_order_release);
            return true;
        }
        // refcountがマイナス値
        return false;
    }
    
    void decrement(void)
    {
        _refcount.fetch_sub(1, std::memory_order_release);
    }
    
    // スピンロックによる排他アクセス制御
    //void lock(void) { _spinlock.lock(); }
    //void unlock(void) { _spinlock.unlock(); }
};

#endif /* SLManagedPointer_h */

//同步工具类，它允许一个或多个线程一直等待，直到其他线程的操作执行完后再执行。
//CountDownLatch是通过一个计数器来实现的，计数器的初始值为线程的数量。
//每当一个线程完成了自己的任务后，计数器的值就会减1。
//当计数器值到达0时，它表示所有的线程已经完成了任务，然后在闭锁上等待的线程就可以恢复执行任务

#ifndef MUDUO_BASE_COUNTDOWNLATCH_H
#define MUDUO_BASE_COUNTDOWNLATCH_H

#include "muduo/base/Mutex.h"
#include "muduo/base/Condition.h"

namespace muduo
{
class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();
    int getCount() const;

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};
} // namespace muduo

#endif //MUDUO_BASE_COUNTDOWNLATCH_H
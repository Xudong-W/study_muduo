//无界队列
//属性：互斥锁 条件变量（full） std队列
//方法： 构造函数（有界队列的大小），put take 其他 size（） isEmpty()
#ifndef MUDUO_BASE_BLOCKINGQUEUE_H
#define MUDUO_BASE_BLOCKINGQUEUE_H

#include "muduo/base/Mutex.h"
#include "muduo/base/Condition.h"

#include <deque>
#include <assert.h>

namespace muduo
{

template <typename T>
class BlockingQueue : noncopyable
{
public:
    //使用Condition时，必须先创建MutexLock，并使用MutexLock对象构造条件变量
    BlockingQueue()
        : mutex_(),
          notEmpty_(mutex_),
          queue_()
    {
    }

    ~BlockingQueue()
    {
    }

    void put(T &x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify();
    }

    void put(T &&x) //c++11 &&
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(std::move(x));
        notEmpty_.notify();
    }

    // Condition的唯一正确用法
    //* 与mutex一起使用，布尔表达式的读写需要受mutex保护
    //* mutex上锁后才可能调用wait()
    //* 把判断条件和wait()放在while循环内  原因是条件变量存在spurious wakeup
    T take()
    {
        MutexLockGuard lock(mutex_); //mutex_加锁
        while (queue_.empty())       //必须用循环，且在判断之后再wait()
        {
            notEmpty_.wait(); //原子的unlock mutex_,并进入等待，不会与put死锁
                              //wait()执行万后，mutex_自动加锁
        }
        assert(!queue_.empty());
        T front(std::move(queue_.front())); // std::move c++11 移动语义 效率高
        queue_.pop_front();
        return std::move(front); // 完成消费，mutex_作用于结束，自动析构，mutex_解锁
    }

    int size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

    bool isEmpty()
    {
        MutexLockGuard lock(mutex_);
        return queue_.empty();
    }

private:
    //对列内部维护锁，对外不暴露任何同步相关的资源，对外表现为一个线程安全的队列
    //所以内部需要维护自己的互斥锁和条件变量
    mutable MutexLock mutex_;
    Condition notEmpty_;
    std::deque<T> queue_;
};

} // namespace muduo

#endif //MUDUO_BASE_BLOCKINGQUEUE_H
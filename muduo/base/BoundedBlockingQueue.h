//有界队列
//属性：互斥锁 条件变量（full empty） std队列
//方法： 构造函数（有界队列的大小），put take 其他 size（） 容量（） isFull() isEmpty()
#ifndef MUDUO_BASE_BOUNDEDBLOCKINGQUEUE_H
#define MUDUO_BASE_BOUNDEDBLOCKINGQUEUE_H

#include "muduo/base/Mutex.h"
#include "muduo/base/Condition.h"
#include <boost/circular_buffer.hpp>
#include <assert.h>
namespace muduo
{
template <typename T>
class BoundedBlockingQueue : noncopyable
{
public:
    explicit BoundedBlockingQueue(int maxSize)
        : mutex_(),
          notEmpty_(mutex_),
          notFull_(mutex_),
          queue_(maxSize)
    {
    }
    ~BoundedBlockingQueue()
    {
    }

    void put(const T &x)
    {
        MutexLockGuard lock(mutex_);
        while (queue_.full())
        {
            notFull_.wait(); //zhe yi bu hui yuan zi de unlock mutex bing jin ru deng dai
        }
        assert(!queue_.full());

        queue_.push_back(x);
        notEmpty_.notify();
    }

    void put(T&& x)
    {
        MutexLockGuard lock(mutex_);
        while (queue_.full())
        {
            notFull_.wait(); //zhe yi bu hui yuan zi de unlock mutex bing jin ru deng dai
        }
        assert(!queue_.full());
        queue_.push_back(std::move(x));
        notEmpty_.notify();
    }

    T take()
    {
        MutexLockGuard lock(mutex_);
        while (queue_.empty())
        {
            notEmpty_.wait();
        }
        assert(!queue_.empty());
        T front(std::move(queue_.front()));
        queue_.pop_front();

        notFull_.notify();
        return std::move(front);
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

    size_t capacity() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.capacity();
    }

private:
    //对列内部维护锁，对外不暴露任何同步相关的资源，对外表现为一个线程安全的队列
    //所以内部需要维护自己的互斥锁和条件变量
    mutable MutexLock mutex_;     //类成员变量的初始化顺序和声明顺序相同
    Condition notEmpty_;
    Condition notFull_;
    boost::circular_buffer<T> queue_;
};
} // namespace muduo

#endif
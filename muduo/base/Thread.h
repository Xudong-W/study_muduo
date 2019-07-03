//thread类
// 使用pthread -- linux自带的线程库
// 属性：线程id 线程名称 线程状态（是否运行 是否退出）  线程执行主函数指针
// 方法：创建， 开始 退出，销毁  获取id set名称 get名称 线程执行主函数（函数指针、）

//线程退出，垃圾回收的两种方式：
//线程是可结合（joinable）或可分离的(detached).默认情况下，线程被创建为可结合的，可结合线程必须显式由其他回收ptread_join
//1.其他线程调用pthread_join()，等待退出并回收垃圾
//2.本线程调用pthread_detach(),由操作系统回收

//线程库对比
//linux库函数
//c库函数
//c++11库函数

#ifndef MUDUO_BASE_THREAD_H
#define MUDUO_BASE_THREAD_H

#include "muduo/base/Atomic.h"
#include "muduo/base/CountDownLatch.h"
#include "muduo/base/Types.h"

#include <functional>
#include <memory>
#include <pthread.h>

namespace muduo
{
class Thread : noncopyable
{
public:
    typedef std::function<void()> ThreadFunc;

    explicit Thread(ThreadFunc, const string &name = std::string());
    ~Thread();

    void start();
    int join(); //return pthread_join() 实际线程为func_，操作该类的为源线程，可调用pthread_join()进行垃圾回收

    bool started() const { return started_; }
    pthread_t pthreadId() const { return pthreadId_; }
    pid_t tid() const { return tid_; }
    const std::string &name() const { return name_; }

    static int numCreated() { return numCreated_.get(); }

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    string name_;
    CountDownLatch latch_;

    static AtomicInt32 numCreated_;
};

} // namespace muduo

#endif //MUDUO_BASE_THREAD_H
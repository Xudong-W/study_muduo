//线程池：采用linux系统函数提供的pthread线程
//属性: 线程队列、 任务队列、 互斥锁、 条件变量、 runing状态、 线程池名称
//方法
//**构造函数 线程个数、线程池名字

//JAVA一个线程池可以包括四个基本部分
// （1）线程池管理器： 创建线程池，销毁线程池，添加新任务                     对应与C++的构造函数和析构函数+start函数+add函数（向任务队列中添加任务）
// （2）工作线程：线程池中的线程，在没有任务时处于等待状态，可以循环执行任务     对应于类中的runInThread函数，在while(running)内，循环消费任务队列，并通过获得的任务出发回调，出发任务函数
// （3）任务接口：每个任务必须实现该接口，供工作线程调度执行                  通过函数指针定义的任务函数
// （4）任务队列：存放没有处理的队列，提供一种缓存机制                       任务队列，

//线程池分类：
// 固定线程数的、动态调整线程数的、有存活时间的、有队列长度限制的、调用底层纤程的、支持多个线程池合并的、支持定时任务的、有优先级的

//简单线程池实现
//声明任务函数： 通过函数指针的方式，由线程池中的线程回调，实现该函数在线程中的执行 typedef std::function<void()> Task;
//定义线程队列， std::vector<std::unique_ptr<muduo::Thread>> threads_;
//定义任务队列typedef std::function<void()> Task;    BlockingQueue<Task> taskQueue
//启动线程池：  创建线程,并
//添加任务： 向任务队列中添加任务，并有线程队列中的线程执行

//（1）创建线程池，启动线程 包括numThreads个线程，线程的主体函数为workThread
// running = true;
// for(int i=0;i<numThreads;i++)
// {
//     creat_thread(&workThread);
// }

// （2）线程的主题函数
// void workThread()
// {
//     while(running)          //线程池的全局标志，true期间 保证所有线程永远执行
//     {
//         Task task =  taskQueue.take();    //没有任务时，条件变量taskQueue wait（），保证线程的空while,浪费资源。
//                                           //任务从队列中take之后，弹出
//         task();
//     }
// }

// (3)向线程池中添加任务
// Task task = std::bind(任务函数，...);
// taskQueue.run(task);

//(4)退出线程池
// running = false;
// for(int i=0;i<numThreads;i++)
// {
//     threads_[i].join();
// }

#ifndef MUDUO_BASE_THREADPOOL_H
#define MUDUO_BASE_THREADPOOL_H

#include "muduo/base/Condition.h"
#include "muduo/base/Mutex.h"
#include "muduo/base/Thread.h"
#include "muduo/base/Types.h"

#include <deque>
#include <vector>

namespace muduo
{
class ThreadPool : noncopyable
{
public:
    typedef std::function<void()> Task;
    explicit ThreadPool(const string &nameArg = string("ThreadPool"));
    ~ThreadPool();
    //start之前调用
    //且在运行期间不能更改大小
    void setMaxQueueSize(int maxSize)
    {
        maxQueueSize_ = maxSize;
    }

    void setThreadInitCallback(const Task &cb)
    {
        threadInitCallback_ = cb;
    }

    void start(int numThreads); //启动线程池，等待任务执行，并指定线程线程池大小
    void stop();
    const string &name() const
    {
        return name_;
    }
    size_t queueSize() const;

    void run(Task f); //执行任务
private:
    bool isFull() const;
    void runInThread();
    Task take();

    string name_;
    size_t maxQueueSize_;
    bool running_;
    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    Task threadInitCallback_;
    std::vector<std::unique_ptr<muduo::Thread>> threads_; //线程队列
    std::deque<Task> queue_;                              //任务队列
};

} // namespace muduo

#endif //MUDUO_BASE_THREADPOOL_H
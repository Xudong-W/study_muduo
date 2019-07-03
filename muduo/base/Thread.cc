//增加注释

#include "muduo/base/Thread.h"
#include "muduo/base/CurrentThread.h"
#include "muduo/base/Exception.h"
#include "muduo/base/Logging.h"

#include <type_traits>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace muduo
{
namespace detail
{
//通过系统调用
pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterFork()
{
    muduo::CurrentThread::t_cachedTid = 0;
    muduo::CurrentThread::t_threadName = "main";
    muduo::CurrentThread::tid();
}
class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        muduo::CurrentThread::t_threadName = "main";
        muduo::CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

ThreadNameInitializer init;

//供线程传参使用的结构体
struct ThreadData
{
    typedef muduo::Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    string name_;
    pid_t *tid_;
    CountDownLatch *latch_;

    ThreadData(ThreadFunc func,
               const std::string &name,
               pid_t *tid,
               CountDownLatch *latch)
        : func_(std::move(func)),
          name_(name),
          tid_(tid),
          latch_(latch)
    {
    }

    void runInThread()
    {
        *tid_ = muduo::CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();
        latch_ = NULL;
        muduo::CurrentThread::t_threadName = name_.empty() ? "muduoThread" : name_.c_str();
        ::prctl(PR_SET_NAME, muduo::CurrentThread::t_threadName);
        try
        {
            func_();
            muduo::CurrentThread::t_threadName = "finished";
        }
        //自己封装exception类
        catch (const Exception &ex)
        {
            muduo::CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in thread %s\n", name_.c_str());
            fprintf(stderr, "reason %s\n", ex.what());
            fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
            abort();
        }
        catch (const std::exception &ex)
        {
            muduo::CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            abort();
        }
        catch (...)
        {
            muduo::CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "unknown exception caught in thread %s\n", name_.c_str());
            throw;
        }
    }
};

void *startThread(void *obj)
{
    ThreadData *data = static_cast<ThreadData *>(obj);
    data->runInThread();
    delete data;
    return NULL;
}
} // namespace detail

void CurrentThread::cacheTid()
{
    if (t_cachedTid == 0)
    {
        t_cachedTid = detail::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
    }
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
    struct timespec ts = {0, 0};
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<time_t>(1000);
    ::nanosleep(&ts, NULL);
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(ThreadFunc func, const string &name)
    : started_(false),
      joined_(false),
      pthreadId_(0),
      tid_(0),
      func_(std::move(func)),
      name_(name),
      latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    //若线程正在运行，没有正常join，通过detach退出
    if (started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    int num = numCreated_.incrementAndGet();
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    detail::ThreadData *data = new detail::ThreadData(func_, name_, &tid_, &latch_);
    if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
    {
        started_ = false;
        delete data;
        // failed in pthread_created
    }
    else
    {
        latch_.wait(); // 保证线程启动成功
        assert(tid_ > 0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}

} // namespace muduo

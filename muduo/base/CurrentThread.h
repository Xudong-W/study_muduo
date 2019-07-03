//线程方法 配合thread类使用
// 包括：线程号 线程名词 错误日志

#ifndef MUDUO_BASE_CURRENTTHREAD_H
#define MUDUO_BASE_CURRENTTHREAD_H

#include "muduo/base/Types.h"

namespace muduo
{
namespace CurrentThread
{
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char *t_threadName;

    void cacheTid();

    inline int tid()
    {
        if (__builtin_expect(t_cachedTid == 0, 0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }

    inline const char *tidString() //for loging
    {
        return t_tidString;
    }

    inline int tidStringLength() //for logging
    {
        return t_tidStringLength;
    }

    inline const char *name()
    {
        return t_threadName;
    }

    bool isMainThread();

    void sleepUsec(int64_t usec); // for testing

    string stackTrace(bool demangle);
} // namespace CurrentThread

} // namespace muduo

#endif //MUDUO_BASE_CURRENTTHREAD_H
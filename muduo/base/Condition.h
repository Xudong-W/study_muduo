//条件变量-封装类
//属性：互斥锁（引用，必须由外部传入。同外部互斥锁共同使用），条件变量
//方法：构造函数（init 参数：互斥锁），希构函数(destroy) wait(内部使用外部传入的互斥锁)， wait(delay)  ,notify(), notifyAll
//****条件变量必须与互斥锁共同使用，由互斥锁保护wait,notify可不保护

// Condition的唯一正确用法
//******wait端*********//
//* 与mutex一起使用，布尔表达式（while循环的条件）的读写需要受mutex保护
//* mutex上锁后才可能调用wait()
//* 把判断条件和wait()放在while循环内  原因是条件变量存在spurious wakeup

//*****signal端********//
// signal等价于notify,
//在signal之前一般需要修改布尔表达式(wait侧 while循环条件)
//布尔表达式的修改通常需要mutex保护 （比如普通队列，并发读写，必须在读写两端都加锁）
//调用signal时，不一定需要mutex保护
// signal表示资源可用，broadcast表示状态变化

#ifndef MUDUO_BASE_CONDITION_H
#define MUDUO_BASE_CONDITION_H

#include "muduo/base/Mutex.h"
#include <pthread.h>

namespace muduo
{
class Condition : noncopyable
{
public:
    //单参数构造函数，或者除第一个参数外其余参数都有默认值时
    //作用：（1）构造函数  （2）类型转换
    //explicit 单参数构造函数使用，防止隐式转换
    //ex.单参构造函数，编译是有一个缺省的转换操作：将该构造函数参数对应类型的数据，转换为该类对象
    //*Condition test = (MutexLock)mutex; 《===等价于===》 Condition temp(mutex); Condition test = temp;
    //*加上explicit后，屏蔽该隐式转换(implicit)
    explicit Condition(MutexLock &mutex)
        : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        MutexLock::UnassignGuard ug(mutex_);
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }

    bool waitForSeconds(double seconds);

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }

private:
    //条件变量必须配合互斥锁使用，需要外部传入互斥锁
    MutexLock &mutex_;
    pthread_cond_t pcond_;
};

} // namespace muduo

#endif // MUDUO_BASE_CONDITION_H
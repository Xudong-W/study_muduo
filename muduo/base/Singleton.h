//单例模式：保证一个类只有一个实力，并提供一个全剧唯一的访问接口
//普通单例模式特点：
//**构造函数私有化，保证外部无法调用
//**生名一个私有静态类对象
//**通过全局唯一接口，获得该私有静态对象

//该类实现原理及功能
//

#ifndef MUDUO_BASE_SINGLETON_H
#define MUDUO_BASE_SINGLETON_H

#include "muduo/base/noncopyable.h"

#include <pthread.h>
#include <assert.h>
#include <stdlib.h> //for atexit

namespace muduo
{
namespace detail
{
template <typename T>
struct has_no_destroy
{
    template <typename C>
    static char test(decltype(&C::no_destroy));
    template <typename C>
    static int32_t test(...);
    const static bool value = sizeof(test<T>(0)) == 1;
};
} // namespace detail
template <typename T>
class Singleton : noncopyable
{
public:
    Singleton() = delete;
    ~Singleton() = delete;

    static T &instance()
    {
        pthread_once(&ponce_, &Singleton::init);
        assert(value_ != NULL);
        return *value_;
    }

private:
    static void init()
    {
        value_ = new T();
        if (!detail::has_no_destroy<T>::value)
        {
            ::atexit(destroy);
        }
    }

    static void destroy()
    {
        typedef char T_must_be_complete_type[sizeof(T)==0 ?-1:1];
        T_must_be_complete_type dummpy;(void) dummpy;

        delete value_;
        value_ = NULL;
    }

    static pthread_once_t ponce_;
    static T *value_;
};

template <typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template <typename T>

T *Singleton<T>::value_ = NULL;
} // namespace muduo
#endif // MUDUO_BASE_SINGLETON_H
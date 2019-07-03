//类的访问权限控制
//(1)类内部
//class XXX
// {
// public: 可以被该类的函数、字类的函数、友元函数、类对象访问
// protected: 可以被该类的函数、子类的函数、友元函数访问，但是不能被该类对象访问
// private: 只能由该类的函数和友元函数访问，不能被任何其他访问（包括字类函数、该类对象）。 即：private不会被继承
// }

//（2）继承
//private属性不能被继承，protected和private继承能降低父类成员和方法的访问权限
//继承后，相应访问权限参照上一条


#ifndef MUDUO_BASE_NONCOPYABLE_H
#define MUDUO_BASE_NONCOPYABLE_H

namespace muduo
{

class noncopyable
{
public:
    //delete 禁止使用特殊成员函数
    noncopyable(const noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;

    //default仅用于特殊成员函数，且该函数没有默认参数
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
} // namespace muduo

#endif // MUDUO_BASE_NONCOPYABLE_H
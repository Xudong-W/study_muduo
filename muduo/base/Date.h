//日期类
//属性：
//方法：构造函数（无参、年月日、tm结构，天数） year(), month(), day(), yearMonthDay()  get标准格式YYYY-MM-DD
// 注： 同时给出两个操作符重载 < 和 == 但是操作副重载函数在类外
//说明 julianDayNumber_ 儒略日 在儒略周期内以连续的日数计算时间的计时法，主要是天文学家使用

#ifndef MUDUO_BASE_DATE_H
#define MUDUO_BASE_DATE_H

#include "muduo/base/copyable.h"
#include "muduo/base/Types.h"

namespace muduo
{

class Date : public muduo::copyable
{
public:
    struct YearMonthDay
    {
        int year;
        int month;
        int day;
    };

    //静态成员变成必须初始化
    static const int kDaysPerWeek = 7;
    static const int KJulianDayOf1970_01_01;

    Date()
        : julianDayNumber_(0)
    {
    }

    Date(int year, int month, int day);

    explicit Date(int julianDayNum)
        : julianDayNumber_(julianDayNum)
    {
    }

    explicit Date(const struct tm &);

    void swap(Date &that)
    {
        std::swap(julianDayNumber_, that.julianDayNumber_);
    }

    bool valid() const
    {
        return julianDayNumber_ > 0;
    }

    // 转换成标准格式 YYYY-mm-dd
    string toIsoString() const;

    struct YearMonthDay yearMothDay() const;

    int year() const
    {
        return yearMothDay().year;
    }

    int month() const
    {
        return yearMothDay().month;
    }

    int day() const
    {
        return yearMothDay().day;
    }

    //星期[0,1,2,3,4,5,6]
    int weekDay() const
    {
        return (julianDayNumber_ + 1) % kDaysPerWeek;
    }

    int julianDayNumber() const
    {
        return julianDayNumber_;
    }

private:
    int julianDayNumber_;
};

inline bool operator<(Date x, Date y)
{
    return x.julianDayNumber() < y.julianDayNumber();
}

inline bool operator==(Date x, Date y)
{
    return x.julianDayNumber() == y.julianDayNumber();
}

} // namespace muduo
#endif
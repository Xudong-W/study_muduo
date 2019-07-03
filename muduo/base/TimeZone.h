//时区类
//地球分为24个时区，每个时区都有自己的时间。
//在国际无线电通信场合，为统一期间，使用一个统一的时间，称为通用协调时（universal time coordinated）. UTC与英国伦敦的本地时间相同
// 北京时区为东八区，领先UTC八个小时
//UTC+时差区 = 本地时间

#ifndef MUDUO_BASE_TIMEZONE_H
#define MUDUO_BASE_TIMEZONE_H

#include "muduo/base/copyable.h"
#include <memory>
#include <time.h>

namespace muduo
{

// TimeZone for 1970~2030
class TimeZone : public muduo::copyable
{
public:
  explicit TimeZone(const char *zonefile);
  TimeZone(int eastOfUtc, const char *tzname); // a fixed timezone
  TimeZone() = default;                        // an invalid timezone

  // default copy ctor/assignment/dtor are Okay.

  bool valid() const
  {
    // 'explicit operator bool() const' in C++11
    return static_cast<bool>(data_);
  }

  struct tm toLocalTime(time_t secondsSinceEpoch) const;
  time_t fromLocalTime(const struct tm &) const;

  // gmtime(3)
  static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);
  // timegm(3)
  static time_t fromUtcTime(const struct tm &);
  // year in [1900..2500], month in [1..12], day in [1..31]
  static time_t fromUtcTime(int year, int month, int day,
                            int hour, int minute, int seconds);

  struct Data;

private:
  std::shared_ptr<Data> data_;
};

} // namespace muduo

#endif // MUDUO_BASE_TIMEZONE_H

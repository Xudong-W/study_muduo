#ifndef MUDUO_BASE_TYPES_H
#define MUDUO_BASE_TYPES_H

#include <stdint.h>
#include <string.h>
#include <string>

#ifndef NDEBUG
#include <assert.h>
#endif

namespace muduo
{
using std::string;

inline void memZero(void *ptr, size_t n)
{
    memset(ptr, 0x00, n);
}

//from boost
//c++ transform:  static_cast, const_cast, dynamic_cast, interpret_cast
template <typename To, typename From>
inline To implicit_cast(From const &f)
{
    return f;
}

template <typename To, typename From>
inline To down_cast(From *f)
{
#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RITI)
    assert(f == NULL || dynamic_cast<To>(f) != NULL);
#endif
    return static_cast<To>(f);
}

} // namespace muduo

#endif //MUDUO_BASE_TYPES_H
#ifndef MUDUO_BASE_COPYABLE_H
#define MUDUO_BASE_COPYABLE_H

namespace muduo
{
class copyable
{
protected:
    copyable() = default;
    ~copyable() = default;
};

} // namespace muduo

#endif //MUDUO_BASE_COPYABLE_H
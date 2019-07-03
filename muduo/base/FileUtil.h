//文件操作类
//读文件
//属性： 文件描述符， 读缓冲区， 错误码
//方法
//**构造函数 打开文件
//**析构函数 关闭文件
//**读文件到制定缓冲区
//**读文件到自带的读缓冲区
//**获取默认的读缓冲区

//写文件
//属性：文件描述符 写缓冲区  错误玛 写位置
//方法：
//**构造函数 打开文件
//**析构函数 关闭文件
//**append 将制定缓冲区内的内容写入文件   (内容，长度)
//**刷磁盘 flush
//**

//注： 读的时候用系统函数，写的时候用c库函数

//文件操作函数
//linux系统函数： open, close, write, read等      特点：在内核层有缓冲区，很小，所以写写文件时需要频繁刷新缓冲区，降低写入速度，因此需要配合用户层的自定义缓冲区
//标准c语言库函数： fopen, fclose, fread, fwrite等  特点：自己维护了可变的缓冲区
//标准c++库函数： ofstream ifstream
#ifndef MUDUO_BASE_FILEUTIL_H
#define MUDUO_BASE_FILEUTIL_H

#include "muduo/base/noncopyable.h"
#include "muduo/base/StringPiece.h"
#include <sys/types.h> // for off_t

namespace muduo
{
namespace FileUtil
{

// read small file < 64KB
class ReadSmallFile : noncopyable
{
public:
  ReadSmallFile(StringArg filename);
  ~ReadSmallFile();

  // return errno
  template <typename String>
  int readToString(int maxSize,
                   String *content,
                   int64_t *fileSize,
                   int64_t *modifyTime,
                   int64_t *createTime);

  /// Read at maxium kBufferSize into buf_
  // return errno
  int readToBuffer(int *size);

  const char *buffer() const { return buf_; }

  static const int kBufferSize = 64 * 1024;

private:
  int fd_;
  int err_;
  char buf_[kBufferSize];
};

// read the file content, returns errno if error happens.
template <typename String>
int readFile(StringArg filename,
             int maxSize,
             String *content,
             int64_t *fileSize = NULL,
             int64_t *modifyTime = NULL,
             int64_t *createTime = NULL)
{
  ReadSmallFile file(filename);
  return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

// not thread safe
class AppendFile : noncopyable
{
public:
  explicit AppendFile(StringArg filename);

  ~AppendFile();

  void append(const char *logline, size_t len);

  void flush();

  off_t writtenBytes() const { return writtenBytes_; }

private:
  size_t write(const char *logline, size_t len);

  FILE *fp_;
  char buffer_[64 * 1024];
  off_t writtenBytes_;
};

} // namespace FileUtil
} // namespace muduo

#endif // MUDUO_BASE_FILEUTIL_H

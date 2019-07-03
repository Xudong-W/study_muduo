set(base_SRCS
    Condition.cc
    CurrentThread.cc
    CountDownLatch.cc
    Date.cc
    FileUtil.cc
    ProcessInfo.cc
    LogFile.cc
    Logging.cc
    LogStream.cc
    Thread.cc
    ThreadPool.cc
    AsyncLogging.cc
    Timestamp.cc
    TimeZone.cc
    Exception.cc
)

add_library(muduo_base ${base_SRCS})
target_link_libraries(muduo_base pthread rt)

if(MUDUO_BUILD_EXAMPLES)
    add_subdirectory(tests)
endif()
#include "muduo/base/CurrentThread.h"

#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>

namespace muduo
{
namespace CurrentThread
{
__thread int t_cachedTid;
__thread char t_tidString[32];
__thread int t_tidStringLength;
__thread const char *t_threadName;
static_assert(std::is_same<int, pid_t>::value, "pid_t should be a int type");

string stackTrace(bool demangle)
{
    string stack;
    const int max_frames = 200;
    void *frame[max_frames];
    int nptrs = ::backtrace(frame, max_frames);
    char **strings = ::backtrace_symbols(frame, nptrs);

    if (strings)
    {
        size_t len = 256;
        char *demangled = demangle ? static_cast<char *>(::malloc(len)) : nullptr;
        for (int i = 0; i < nptrs; i++)
        {
            if (demangle)
            {
                char *left_par = nullptr;
                char *plus = nullptr;

                for (char *p = strings[i]; *p; ++p)
                {
                    if (*p == '(')
                    {
                        left_par = p;
                    }
                    else if (*p == '+')
                    {
                        plus = p;
                    }
                }
                if (left_par && plus)
                {
                    *plus = '\0';
                    int status = 0;
                    char *ret = abi::__cxa_demangle(left_par + 1, demangled, &len, &status);
                    *plus = '+';

                    if (status == 0)
                    {
                        demangled = ret;
                        stack.append(strings[i], left_par + 1);
                        stack.append(demangled);
                        stack.append(plus);
                        stack.append("\n");
                        continue;
                    }
                }
            }
            stack.append(strings[i]);
            stack.append("\n");
        }
        free(demangled);
        free(strings);
    }
    return stack;
}

} // namespace CurrentThread

} // namespace muduo

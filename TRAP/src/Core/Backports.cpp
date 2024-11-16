#include "TRAPPCH.h"
#include "Backports.h"

#ifndef __cpp_lib_unreachable

namespace std
{
    [[noreturn]] void unreachable()
    {
#ifdef __GNUC__
        __builtin_unreachable();
#elif defined(_MSC_VER)
        __assume(false);
#endif
    }
}

#else

#if __cplusplus >= 202302L
#warning "Backported std::unreachable() should be removed!"
#endif /*__cplusplus >= 202302L*/

#endif /*__cpp_lib_unreachable*/

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

#ifndef __cpp_lib_debugging

#include "Utils/Win.h"
#include "Utils/Linux.h"

#if defined(__GNUC__) && (defined(__i386) || defined(__x86_64))
#include <csignal>
#endif

[[nodiscard]] bool std::is_debugger_present() noexcept
{
#ifdef TRAP_PLATFORM_WINDOWS
    return IsDebuggerPresent() != 0;
#elif defined(TRAP_PLATFORM_LINUX)
    //The standard POSIX way of detecting a debugger is to attempt to
    //ptrace() the process, but this needs to be done from a child and not
    //the process itself to still allow attaching to this process later if wanted,
    //so is rather heavy. Under Linux we have the PID of the "debugger" (which
    //doesn't need to be gdb, of course, it could also be strace, for example) in
    ///proc/$PID/statis, so just get it from there instead.
    std::ifstream in("/proc/self/status");
    for(std::string line{}; std::getline(in, line);)
    {
        static constexpr std::string_view Prefix = "TracerPid:\t";
        if(line.starts_with(Prefix))
        {
            //We're traced if the PID is not 0 and no other PID starts with
            //0 digit, so it's enough to check for just a single character.
            return line.length() > Prefix.length() && line[Prefix.length()] != '0';
        }
    }
    return false;
#else
    //Unable to detect debugger.
    return false;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void std::breakpoint() noexcept
{
#ifdef TRAP_PLATFORM_LINUX
    //If we can use inline assembler, do it because this allows us to break
    //directly at the location of the failing check instead of breaking inside
    //raise() called from it, i.e. one stack frame below.
#if defined(__GNUC__) && (defined(__i386) || defined(__x86_64))
    asm volatile ("int $3");
#else
    raise(SIGTRAP);
#endif
#elif defined(TRAP_PLATFORM_WINDOWS)
    __debugbreak();
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void std::breakpoint_if_debugging() noexcept
{
    if(std::is_debugger_present())
        std::breakpoint();
}

#endif /*__cpp_lib_debugging*/

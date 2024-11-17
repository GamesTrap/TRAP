#ifndef TRAP_SAFESYSTEM_H
#define TRAP_SAFESYSTEM_H

#include <string>
#include <span>

#include "Utils/Expected.h"

namespace TRAP::Utils
{
    /// @brief Safe replacement for system()/shell calls without invoking a command processor (ENV33-C).
    /// @param program Executable to run. Example: "explorer", "xdg-open".
    /// @param args Arguments for the executable.
    /// @param waitForChild Whether to wait for the called executable to exit or not.
    /// @return If waitForChild is false then void on successful invocation,
    ///         if waitForChild is true then void on successful invocation and normal exit of the executable.
    ///         String containing an error description otherwise.
    /// @threadsafe
    [[nodiscard]] TRAP::Expected<void, std::string> SafeSystem(const std::string& program, const std::vector<std::string>& args, bool waitForChild);
}

#endif /*TRAP_SAFESYSTEM_H*/

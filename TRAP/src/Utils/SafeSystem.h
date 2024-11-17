#ifndef TRAP_SAFESYSTEM_H
#define TRAP_SAFESYSTEM_H

#include <string>
#include <span>

#include "Utils/Expected.h"

namespace TRAP::Utils
{
    //TODO Document
    [[nodiscard]] TRAP::Expected<void, std::string> SafeSystem(const std::string& program, const std::vector<std::string>& args, bool waitForChild);
}

#endif /*TRAP_SAFESYSTEM_H*/

#ifndef TRAP_BACKPORTS_HARDWAREINTERFERENCE_H
#define TRAP_BACKPORTS_HARDWAREINTERFERENCE_H

#ifndef __cpp_lib_hardware_interference_size

#include <cstdint>

//This C++17 feature is backported because some compilers still don't provide use these values sadly.

#define __cpp_lib_hardware_interference_size 201703L

namespace std
{
    //These values are just an educated guess for x86_64 platform.

    /// @brief Minimum offset between two objects to avoid false sharing.
    ///        Guaranteed to be at least alignof(std::max_align_t).
    inline constexpr std::size_t hardware_destructive_interference_size = 64;

    /// @brief Maximum size of contiguous memory to promote true sharing.
    ///        Guaranteed to be at least alignof(std::max_align_t).
    inline constexpr std::size_t hardware_constructive_interference_size = 64;
}

#endif /*__cpp_lib_hardware_interference_size*/

#endif /*TRAP_BACKPORTS_HARDWAREINTERFERENCE_H*/

#ifndef TRAP_UTILS_MAP_H
#define TRAP_UTILS_MAP_H

#include <map>
#include <unordered_map>
#include <string_view>

#include "Core/Types.h"

namespace TRAP::Utils
{
    /// @brief Hasher for string like types with enabled heterogeneous lookup.
    struct StringHasher
    {
        using is_transparent = void; //Enable heterogeneous lookup.

        /// @threadsafe
        [[nodiscard]] usize operator()(const std::string_view sv) const noexcept
        {
            std::hash<std::string_view> hasher{};
            return hasher(sv);
        }
    };

    template<typename Value>
    using UnorderedStringMap = std::unordered_map<std::string, Value, TRAP::Utils::StringHasher, std::equal_to<>>;
}

#endif /*TRAP_UTILS_MAP_H*/

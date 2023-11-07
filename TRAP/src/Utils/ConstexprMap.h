#ifndef TRAP_CONSTEXPRMAP_H
#define TRAP_CONSTEXPRMAP_H

#include <array>
#include <algorithm>
#include <optional>

#include "Core/Types.h"

namespace TRAP::Utils
{
    /// @brief Compile-time and thread safe, fixed size map implementation
    /// @tparam Key Key type for the map.
    /// @tparam Value Value type for the map.
    /// @tparam Size Size for the map.
    template<typename Key, typename Value, usize Size>
    struct ConstexprMap
    {
        using key_type = Key;
        using mapped_type = Value;
        using value_type = std::pair<const Key, Value>;
        using size_type = usize;
        using difference_type = isize;

        std::array<value_type, Size> data;

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Retrieve the mapped value of the element with key equivalent to key.
        /// If no such element exists, an empty optional is returned.
        /// @param key Key of the element to find.
        /// @return Mapped value of the requested element or empty optional if not found.
        [[nodiscard]] constexpr std::optional<mapped_type> at(const key_type& key) const
        {
            const auto iterator = std::ranges::find_if(data, [&key](const auto& val){return val.first == key;});

            if(iterator != data.cend())
                return iterator->second;

            return std::nullopt;
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Check if there is an element with key equivalent to key in the container.
        /// @param key Key value of the element to search for.
        /// @return True if there is such an element, false otherwise.
        [[nodiscard]] constexpr bool contains(const key_type& key) const
        {
            const auto iterator = std::ranges::find_if(data, [&key](const auto& val){return val.first == key;});

            return iterator != data.end();
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Checks if the container has no elements.
        /// @return True if the container is empty, false otherwise.
        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return Size == 0;
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Returns the number of elements in the container.
        /// @return Number of elements in the container.
        [[nodiscard]] constexpr size_type size() const noexcept
        {
            return Size;
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Retrieve the value that is mapped to a key equivalent to key.
        /// @param key Key of the element to find.
        /// @return Mapped value of the requested element.
        /// @warning This function throws an exception if no element was found.
        [[nodiscard]] constexpr mapped_type operator[](const key_type& key) const
        {
            return at(key).value();
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Retrieve the number of elements with key that compares equal to the specified argument key,
        /// which is either 1 or 0 since this container does not allow duplicates.
        /// @param key Key value of the elements to count.
        /// @return Number of elements with key key, that is either 1 or 0.
        [[nodiscard]] constexpr size_type count(const key_type& key) const
        {
            return contains(key) ? 1 : 0;
        }
    };
}

#endif /*TRAP_CONSTEXPRMAP_H*/
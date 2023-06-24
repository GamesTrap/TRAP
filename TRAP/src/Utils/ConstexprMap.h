#ifndef TRAP_CONSTEXPRMAP_H
#define TRAP_CONSTEXPRMAP_H

#include <cstdint>
#include <array>
#include <algorithm>
#include <optional>

namespace TRAP::Utils
{
    /// <summary>
    /// Compile-time and thread safe, fixed size map implementation
    /// </summary>
    /// <typeparam name="Key">Key type for the map.</typeparam>
    /// <typeparam name="Value">Value type for the map.</typeparam>
    /// <typeparam name="Size">Size for the map.</typeparam>
    template<typename Key, typename Value, std::size_t Size>
    struct ConstexprMap
    {
        using key_type = Key;
        using mapped_type = Value;
        using value_type = std::pair<const Key, Value>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        std::array<value_type, Size> data;

        //-------------------------------------------------------------------------------------------------------------------//

        /// <summary>
        /// Retrieve the mapped value of the element with key equivalent to key.
        /// If no such element exists, an empty optional is returned.
        /// </summary>
        /// <param name="key">Key of the element to find.</param>
        /// <returns>Mapped value of the requested element or empty optional if not found.</returns>
        [[nodiscard]] constexpr std::optional<mapped_type> at(const key_type& key) const
        {
            const auto iterator = std::find_if(data.cbegin(), data.cend(), [&key](const auto& val){return val.first == key;});

            if(iterator != data.cend())
                return iterator->second;

            return std::nullopt;
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// <summary>
        /// Check if there is an element with key equivalent to key in the container.
        /// </summary>
        /// <param name="key">Key value of the element to search for.</param>
        /// <returns>True if there is such an element, false otherwise.</returns>
        [[nodiscard]] constexpr bool contains(const key_type& key) const
        {
            const auto iterator = std::find_if(data.cbegin(), data.cend(), [&key](const auto& val){return val.first == key;});

            return iterator != data.end();
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// <summary>
        /// Checks if the container has no elements.
        /// </summary>
        /// <returns>True if the container is empty, false otherwise.</returns>
        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return Size == 0;
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// <summary>
        /// Returns the number of elements in the container.
        /// </summary>
        /// <returns>Number of elements in the container.</returns>
        [[nodiscard]] constexpr size_type size() const noexcept
        {
            return Size;
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// <summary>
        /// Retrieve the value that is mapped to a key equivalent to key.
        /// Note: This function throws an exception if no element was found.
        /// </summary>
        /// <param name="key">Key of the element to find.</param>
        /// <returns>Mapped value of the requested element.</returns>
        [[nodiscard]] constexpr mapped_type operator[](const key_type& key) const
        {
            return at(key).value();
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// <summary>
        /// Retrieve the number of elements with key that compares equal to the specified argument key,
        /// which is either 1 or 0 since this container does not allow duplicates.
        /// </summary>
        /// <param name="key">Key value of the elements to count.</param>
        /// <returns>Number of elements with key key, that is either 1 or 0.</returns>
        [[nodiscard]] constexpr size_type count(const key_type& key) const
        {
            return contains(key) ? 1 : 0;
        }
    };
}

#endif /*TRAP_CONSTEXPRMAP_H*/
#ifndef TRAP_CONSTEXPRMAP_H
#define TRAP_CONSTEXPRMAP_H

#include <array>
#include <algorithm>
#include <functional>
#include <optional>

#include "Core/Types.h"

namespace TRAP::Utils
{
    /// @brief Compile-time and thread safe, fixed size map implementation
    /// @tparam Key Key type for the map.
    /// @tparam Value Value type for the map.
    /// @tparam Size Size for the map.
    template<typename Key, typename Value, usize Size, typename KeyEqual = std::equal_to<Key>>
    class ConstexprMap
    {
    public:
        using key_type = Key;
        using mapped_type = Value;
        using value_type = std::pair<const Key, Value>;
        using size_type = usize;
        using difference_type = isize;
        using key_equal = KeyEqual;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = value_type*;
        using const_iterator = const value_type*;

        //-------------------------------------------------------------------------------------------------------------------//

        constexpr ConstexprMap() = default;

        template<typename InputIt>
        constexpr ConstexprMap(const InputIt first, const InputIt last)
            : m_data(std::to_array(first, last))
        {
            static_assert(std::distance(first, last) == Size);
        }

        constexpr ConstexprMap(const ConstexprMap& other)
            : m_data(other.m_data)
        {
        }

        constexpr ConstexprMap(ConstexprMap&& other) noexcept
            : m_data(std::move(other.m_data))
        {
        }

        constexpr explicit ConstexprMap(std::array<value_type, Size> data)
            : m_data(std::move(data))
        {
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Destructs the ConstexprMap. The destructor of the elements are calaled and the used storage is deallocated.
        /// @note If the elements are pointers, the pointer-to objects are not destroyed.
        constexpr ~ConstexprMap() = default;

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Replaces the contents of the container.
        ///        Copy assignment operator. Replaces the contents with a copy of the contents of other.
        /// @param other Another container to use as data source.
        /// @return *this.
        constexpr ConstexprMap& operator=(const ConstexprMap& other)
        {
            if(this == &other)
                return *this;

            m_data = other.m_data;

            return *this;
        }

        /// @brief Replaces the contents of the container.
        ///        Move assignment operator. Replaces the contents with those of other using move semantics
        ///        (i.e. the data in other is moved from other into this container).
        ///        other is in a valid but unspecified state afterwards
        /// @param other Another container to use as data source.
        /// @return *this.
        constexpr ConstexprMap& operator=(ConstexprMap&& other) noexcept
        {
            if(this == &other)
                return *this;

            m_data = std::move(other.m_data);

            return *this;
        }

        /// @brief Replaces the contents of the container.
        ///        Replaces the contents with those identified by intializer list iList.
        /// @param iList Initializer list to use as data source.
        /// @return *this.
        constexpr ConstexprMap& operator=(const std::initializer_list<value_type> iList)
        {
            static_assert(iList.size() == Size);
            std::ranges::copy(iList, m_data.data());
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Returns an iterator to the first element of the ConstexprMap.
        /// @return Iterator to the first element.
        /// @note If the ConstexprMap is empty, the returned iterator will be equal to end().
        [[nodiscard]] constexpr iterator begin() noexcept
        {
            return m_data.begin();
        }

        /// @brief Returns an iterator to the first element of the ConstexprMap.
        /// @return Iterator to the first element.
        /// @note If the ConstexprMap is empty, the returned iterator will be equal to end().
        [[nodiscard]] constexpr const_iterator begin() const noexcept
        {
            return m_data.begin();
        }

        /// @brief Returns an iterator to the first element of the ConstexprMap.
        /// @return Iterator to the first element.
        /// @note If the ConstexprMap is empty, the returned iterator will be equal to end().
        [[nodiscard]] constexpr const_iterator cbegin() const noexcept
        {
            return m_data.cbegin();
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Returns an iterator to the element following the last element of the ConstexprMap.
        /// @return Iterator to the element following the last element.
        /// @note This element acts as a placeholder; attempting to access it results in undefined behavior.
        [[nodiscard]] constexpr iterator end() noexcept
        {
            return m_data.end();
        }

        /// @brief Returns an iterator to the element following the last element of the ConstexprMap.
        /// @return Iterator to the element following the last element.
        /// @note This element acts as a placeholder; attempting to access it results in undefined behavior.
        [[nodiscard]] constexpr const_iterator end() const noexcept
        {
            return m_data.end();
        }

        /// @brief Returns an iterator to the element following the last element of the ConstexprMap.
        /// @return Iterator to the element following the last element.
        /// @note This element acts as a placeholder; attempting to access it results in undefined behavior.
        [[nodiscard]] constexpr const_iterator cend() const noexcept
        {
            return m_data.cend();
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Retrieve the mapped value of the element with key equivalent to key.
        /// If no such element exists, an empty optional is returned.
        /// @param key Key of the element to find.
        /// @return Mapped value of the requested element or empty optional if not found.
        [[nodiscard]] constexpr std::optional<mapped_type> at(const key_type& key) const
        {
            const auto it = std::ranges::find_if(m_data, [&key](const auto& val){return key_equal{}(val.first, key);});

            if(it != m_data.cend())
                return it->second;

            return std::nullopt;
        }

        /// @brief Retrieve the mapped value of the element with key equivalent to key.
        /// If no such element exists, an empty optional is returned.
        /// @param key Key of the element to find.
        /// @return Mapped value of the requested element or empty optional if not found.
        template<typename K>
        [[nodiscard]] constexpr std::optional<mapped_type> at(const K& key) const
        {
            const auto it = std::ranges::find_if(m_data, [&key](const auto& val){return key_equal{}(val.first, key);});

            if(it != m_data.cend())
                return it->second;

            return std::nullopt;
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Finds an element with key equivalent to key.
        /// @param key Key value of the element to search for.
        /// @return An iterator to the requested element.
        ///         If no such element is found, past-the-end (see end()) iterator is returned.
        [[nodiscard]] constexpr iterator find(const Key& key)
        {
            return std::ranges::find_if(m_data, [&key](const auto& val){return key_equal{}(val.first, key);});
        }

        /// @brief Finds an element with key equivalent to key.
        /// @param key Key value of the element to search for.
        /// @return An iterator to the requested element.
        ///         If no such element is found, past-the-end (see end()) iterator is returned.
        [[nodiscard]] constexpr const_iterator find(const Key& key) const
        {
            return std::ranges::find_if(m_data, [&key](const auto& val){return key_equal{}(val.first, key);});
        }

        /// @brief Finds an element with key equivalent to key.
        /// @param key Key value of the element to search for.
        /// @return An iterator to the requested element.
        ///         If no such element is found, past-the-end (see end()) iterator is returned.
        template<typename K>
        [[nodiscard]] constexpr iterator find(const K& x)
        {
            return std::ranges::find_if(m_data, [&x](const auto& val){return key_equal{}(val.first, x);});
        }

        /// @brief Finds an element with key equivalent to key.
        /// @param key Key value of the element to search for.
        /// @return An iterator to the requested element.
        ///         If no such element is found, past-the-end (see end()) iterator is returned.
        template<typename K>
        [[nodiscard]] constexpr const_iterator find(const K& x) const
        {
            return std::ranges::find_if(m_data, [&x](const auto& val){return key_equal{}(val.first, x);});
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Check if there is an element with key equivalent to key in the container.
        /// @param key Key value of the element to search for.
        /// @return True if there is such an element, false otherwise.
        [[nodiscard]] constexpr bool contains(const key_type& key) const
        {
            const auto it = std::ranges::find_if(m_data, [&key](const auto& val){return key_equal{}(val.first, key);});

            return it != m_data.end();
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Checks if the container has no elements.
        /// @return True if the container is empty, false otherwise.
        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return Size == 0u;
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

        /// @brief Retrieve the value that is mapped to a key equivalent to key.
        /// @param key Key of the element to find.
        /// @return Mapped value of the requested element.
        /// @warning This function throws an exception if no element was found.
        template<typename K>
        [[nodiscard]] constexpr mapped_type operator[](K&& x) const
        {
            return at<K>(x).value();
        }

        //-------------------------------------------------------------------------------------------------------------------//

        /// @brief Retrieve the number of elements with key that compares equal to the specified argument key,
        /// which is either 1 or 0 since this container does not allow duplicates.
        /// @param key Key value of the elements to count.
        /// @return Number of elements with key key, that is either 1 or 0.
        [[nodiscard]] constexpr size_type count(const key_type& key) const
        {
            return contains(key) ? 1u : 0u;
        }

    private:
        std::array<value_type, Size> m_data{};
    };

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Compares the contents of two ConstexprMaps.
    /// @param lhs ConstexprMap to compare.
    /// @param rhs ConstexprMap to compare.
    /// @return true if the contents of the containers are equal, false otherwise.
    template<typename Key, typename Value, usize Size, typename KeyEqual = std::equal_to<Key>>
    [[nodiscard]] constexpr bool operator==(const ConstexprMap<Key, Value, Size, KeyEqual>& lhs,
                                            const ConstexprMap<Key, Value, Size, KeyEqual>& rhs)
    {
        return lhs.m_data == rhs.m_data;
    }

    /// @brief Compares the contents of two ConstexprMaps.
    /// @param lhs ConstexprMap to compare.
    /// @param rhs ConstexprMap to compare.
    /// @return true if the contents of the containers are not equal, false otherwise.
    template<typename Key, typename Value, usize Size, typename KeyEqual = std::equal_to<Key>>
    [[nodiscard]] constexpr bool operator!=(const ConstexprMap<Key, Value, Size, KeyEqual>& lhs,
                                            const ConstexprMap<Key, Value, Size, KeyEqual>& rhs)
    {
        return !(lhs == rhs);
    }
}

#endif /*TRAP_CONSTEXPRMAP_H*/

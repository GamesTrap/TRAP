#ifndef TRAP_BACKPORTS_RANGEASSIGN_H
#define TRAP_BACKPORTS_RANGEASSIGN_H

#if !defined(__cpp_lib_containers_ranges) || __cpp_lib_containers_ranges < 202202L

#include <ranges>
#include <concepts>

template<typename Range, typename T>
concept ContainerCompatibleRange = std::ranges::input_range<Range> && std::convertible_to<std::ranges::range_reference_t<Range>, T>;

//-------------------------------------------------------------------------------------------------------------------//

namespace INTERNAL
{
    template<typename Container>
    concept ContainerHasInsertMemberFunction = requires(Container c, typename Container::iterator it)
    {
        {
            c.insert(it, c.begin(), c.end())
        } -> std::same_as<typename Container::iterator>;
    };

    //-------------------------------------------------------------------------------------------------------------------//

    template<typename Container>
    concept ContainerHasEndIterator = requires(Container c) {c.end();};

    //-------------------------------------------------------------------------------------------------------------------//

    template<typename Container>
    concept ContainerHasAssignMemberFunction = requires(Container c)
    {
        {
            c.assign(c.begin(), c.end())
        } -> std::same_as<void>;
    };
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename Container, ContainerCompatibleRange<typename Container::value_type> Range>
requires INTERNAL::ContainerHasInsertMemberFunction<Container>
constexpr typename Container::iterator ContainerInsertRange(Container& container, typename Container::const_iterator position, Range&& range)
{
    return container.insert(position, std::ranges::begin(range), std::ranges::end(range));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename Container, ContainerCompatibleRange<typename Container::value_type> Range>
requires INTERNAL::ContainerHasEndIterator<Container>
constexpr void ContainerAppendRange(Container& container, Range&& range)
{
    ContainerInsertRange(container, container.end(), std::forward<Range>(range));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename Container, ContainerCompatibleRange<typename Container::value_type> Range>
requires INTERNAL::ContainerHasAssignMemberFunction<Container>
constexpr void ContainerAssignRange(Container& container, Range&& range)
{
    container.assign(std::ranges::begin(range), std::ranges::end(range));
}

#else

#warning "Backported std::<Container>::assign_range() should be removed!"
#warning "Backported std::<Container>::insert_range() should be removed!"
#warning "Backported std::<Container>::append_range() should be removed!"

#endif

#endif /*TRAP_BACKPORTS_RANGEASSIGN_H*/

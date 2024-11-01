#include "TRAPPCH.h"
#include "Base.h"

namespace
{
    std::atomic TRAPProfileSystems = ProfileSystems::All;
}

/// @brief Retrieve the TRAP systems to be profiled.
/// @return Systems to profile.
[[nodiscard]] ProfileSystems GetTRAPProfileSystems() noexcept
{
	return TRAPProfileSystems;
}

/// @brief Set the TRAP systems to profile.
/// @param systems Systems to profile.
void SetTRAPProfileSystems(const ProfileSystems systems) noexcept
{
	TRAPProfileSystems = systems;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRACY_ENABLE

namespace
{
    [[nodiscard]] void* AlignedAlloc(const std::size_t alignment, const std::size_t size)
    {
#ifdef TRAP_PLATFORM_WINDOWS
        return ::_aligned_malloc(size, alignment);
#else
        //aligned_alloc() requires that size is a multiple of alignment,
        //but for C++ [new.delete.general], only states "if the value of an
        //alignment argument passed to any of these functions is not a valid
        //alignment value, the behavior is undefined".
        //To handle calls such as ::operator new(1, std::align_val_t(128)), we
        //round size up to the next multiple of alignment.
        const size_t roundedSize = (size + alignment - 1) & ~(alignment - 1);
        //Rounding up could have wrapped around to zero, so we have to add another
        //max() ternary to the actual call site to avoid succeeded in that case.
        return std::aligned_alloc(alignment, size > roundedSize ? size : roundedSize);
#endif
    }

    [[nodiscard]] void* NewImpl(usize size)
    {
        if(size == 0)
            size = 1;

        void* ptr;
        while((ptr = std::malloc(size)) == nullptr)
        {
            std::new_handler newHandler = std::get_new_handler();
            if(newHandler)
                newHandler();
            else
                break;
        }

        if(ptr == nullptr) [[unlikely]]
            return ptr;

        TracyAllocS(ptr, size, 10);
        return ptr;
    }

    [[nodiscard]] void* AlignedNewImpl(usize size, std::align_val_t alignment)
    {
        if(size == 0)
            size = 1;
        if(static_cast<std::size_t>(alignment) < sizeof(void*))
            alignment = std::align_val_t(sizeof(void*));

        void* ptr;
        while((ptr = AlignedAlloc(static_cast<std::size_t>(alignment), size)) == nullptr)
        {
            std::new_handler newHandler = std::get_new_handler();
            if(newHandler)
                newHandler();
            else
                break;
        }

        if(ptr == nullptr) [[unlikely]]
            throw std::bad_alloc();

        TracyAllocS(ptr, size, 10);
        return ptr;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new(const usize size)
{
    void* ptr = NewImpl(size);
    if(ptr == nullptr)
        throw std::bad_alloc();

    return ptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new(const usize size, const std::nothrow_t&) noexcept
{
    return NewImpl(size);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new[](const usize size)
{
    void* ptr = NewImpl(size);
    if(ptr == nullptr)
        throw std::bad_alloc();

    return ptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new[](const usize size, const std::nothrow_t&) noexcept
{
    return NewImpl(size);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new(const usize size, const std::align_val_t alignment)
{
    void* ptr = AlignedNewImpl(size, alignment);
    if(ptr == nullptr)
        throw std::bad_alloc();

    return ptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new(const usize size, const std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    return AlignedNewImpl(size, alignment);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new[](const usize size, const std::align_val_t alignment)
{
    void* ptr = AlignedNewImpl(size, alignment);
    if(ptr == nullptr)
        throw std::bad_alloc();

    return ptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new[](const usize size, const std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    return AlignedNewImpl(size, alignment);
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
    void FreeImpl(void* ptr)
    {
        if(ptr != nullptr)
            TracyFreeS(ptr, 10);

        std::free(ptr);
    }

    void AlignedFreeImpl(void* ptr)
    {
        if(ptr != nullptr)
            TracyFreeS(ptr, 10);

#ifdef TRAP_PLATFORM_WINDOWS
        ::_aligned_free(ptr);
#else
        std::free(ptr);
#endif
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete(void* ptr) noexcept
{
    FreeImpl(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* ptr) noexcept
{
    FreeImpl(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete(void* ptr, std::align_val_t) noexcept
{
    AlignedFreeImpl(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* ptr, std::align_val_t) noexcept
{
    AlignedFreeImpl(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

// void operator delete(void* ptr, const usize) noexcept
// {
//     FreeImpl(ptr);
// }

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* ptr, const usize) noexcept
{
    FreeImpl(ptr);
}

void operator delete(void* ptr, usize, std::align_val_t) noexcept
{
    AlignedFreeImpl(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* ptr, usize, std::align_val_t) noexcept
{
    AlignedFreeImpl(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    FreeImpl(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    FreeImpl(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete(void* ptr, std::align_val_t, const std::nothrow_t&) noexcept
{
    AlignedFreeImpl(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* ptr, std::align_val_t, const std::nothrow_t&) noexcept
{
    AlignedFreeImpl(ptr);
}

#endif /*TRACY_ENABLE*/

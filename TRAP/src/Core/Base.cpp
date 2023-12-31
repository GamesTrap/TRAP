#include "TRAPPCH.h"
#include "Base.h"

#ifdef TRACY_ENABLE
[[nodiscard]] void* operator new(const usize count)
{
    if(void* const ptr = std::malloc(count)) [[likely]]
    {
        TracyAllocS(ptr, count, 10);
        return ptr;
    }

    throw std::bad_alloc();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new[](const usize count)
{
    if(void* const ptr = std::malloc(count)) [[likely]]
    {
        TracyAllocS(ptr, count, 10);
        return ptr;
    }

    throw std::bad_alloc();
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete(void* const ptr) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* const ptr) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete(void* const ptr, [[maybe_unused]] const usize count) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* const ptr, [[maybe_unused]] const usize count) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

#endif /*TRACY_ENABLE*/
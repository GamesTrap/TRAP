#include "TRAPPCH.h"
#include "Base.h"

#ifdef TRACY_ENABLE
[[nodiscard]] void* operator new(const usize count)
{
    auto ptr = malloc(count);

    if(!ptr && errno == ENOMEM)
        throw std::bad_alloc();

    TracyAllocS(ptr, count, 10);
    return ptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new[](const usize count)
{
    auto ptr = malloc(count);

    if(!ptr && errno == ENOMEM)
        throw std::bad_alloc();

    TracyAllocS(ptr, count, 10);
    return ptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete(void* ptr) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* ptr) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete(void* ptr, [[maybe_unused]] const usize count) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* ptr, [[maybe_unused]] const usize count) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

#endif /*TRACY_ENABLE*/
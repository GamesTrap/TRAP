#include "TRAPPCH.h"

#ifdef TRACY_ENABLE
void* operator new(const std::size_t count)
{
    auto ptr = malloc(count);
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

void operator delete(void* ptr, [[maybe_unused]] const std::size_t count) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

#endif /*TRACY_ENABLE*/
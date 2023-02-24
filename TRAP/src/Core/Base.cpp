#include "TRAPPCH.h"
#include "Base.h"

[[nodiscard]] uint32_t TRAP_MAKE_VERSION(const uint32_t major, const uint32_t minor, const uint32_t patch)
{
	TRAP_ASSERT(major < 1024 && "Major version number must be less than 1024.");
	TRAP_ASSERT(minor < 1024 && "Minor version number must be less than 1024.");
	TRAP_ASSERT(patch < 4096 && "Patch version number must be less than 4096.");

	return major << 22u | minor << 12u | patch;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRACY_ENABLE
[[nodiscard]] void* operator new(const std::size_t count)
{
    auto ptr = malloc(count);

    if(!ptr && errno == ENOMEM)
        throw std::bad_alloc();

    TracyAllocS(ptr, count, 10);
    return ptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] void* operator new[](const std::size_t count)
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

void operator delete(void* ptr, [[maybe_unused]] const std::size_t count) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void operator delete[](void* ptr, [[maybe_unused]] const std::size_t count) noexcept
{
    TracyFreeS(ptr, 10);
    free(ptr);
}

#endif /*TRACY_ENABLE*/
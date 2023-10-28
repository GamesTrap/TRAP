#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::unsigned_integral<T>
    consteval void RunCompileTimeIsPowerOfTwoTests()
    {
        static_assert(!TRAP::Math::IsPowerOfTwo(T(0x00)));
        static_assert( TRAP::Math::IsPowerOfTwo(T(0x01)));
        static_assert( TRAP::Math::IsPowerOfTwo(T(0x02)));
        static_assert(!TRAP::Math::IsPowerOfTwo(T(0x03)));
        static_assert( TRAP::Math::IsPowerOfTwo(T(0x04)));
        static_assert(!TRAP::Math::IsPowerOfTwo(T(0x0F)));

        if constexpr(sizeof(T) > 32 || std::same_as<T, u32>)
        {
            static_assert( TRAP::Math::IsPowerOfTwo(T(0x80)));
            static_assert( TRAP::Math::IsPowerOfTwo(T(0x80000000)));
        }
        else if constexpr(sizeof(T) > 8 || std::same_as<T, u8>)
        {
            static_assert( TRAP::Math::IsPowerOfTwo(T(0x80)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::unsigned_integral<typename T::value_type>
    consteval void RunCompileTimeIsPowerOfTwoVecTests()
    {
        static_assert(!TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x00))));
        static_assert( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x01))));
        static_assert( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x02))));
        static_assert(!TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x03))));
        static_assert( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x04))));
        static_assert(!TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x0F))));

        if constexpr(sizeof(T) > 32 || std::same_as<T, u32>)
        {
            static_assert( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x80))));
            static_assert( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x80000000))));
        }
        else if constexpr(sizeof(T) > 8 || std::same_as<T, u8>)
        {
            static_assert( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x80))));
        }
    }

    template<typename T>
    requires std::unsigned_integral<T>
    consteval void RunCompileTimeIsPowerOfTwoEdgeTests()
    {
        constexpr T max = std::numeric_limits<T>::max();
        constexpr T min = std::numeric_limits<T>::min();

        static_assert(!TRAP::Math::IsPowerOfTwo(0u));
        static_assert(!TRAP::Math::IsPowerOfTwo(max));
        static_assert(!TRAP::Math::IsPowerOfTwo(min));
    }
}

TEST_CASE("TRAP::Math::IsPowerOfTwo()", "[math][generic][ispoweroftwo]")
{
    SECTION("Scalar - u8")
    {
        RunCompileTimeIsPowerOfTwoTests<u8>();
        RunCompileTimeIsPowerOfTwoEdgeTests<u8>();
    }
    SECTION("Scalar - u16")
    {
        RunCompileTimeIsPowerOfTwoTests<u16>();
        RunCompileTimeIsPowerOfTwoEdgeTests<u16>();
    }
    SECTION("Scalar - u32")
    {
        RunCompileTimeIsPowerOfTwoTests<u32>();
        RunCompileTimeIsPowerOfTwoEdgeTests<u32>();
    }
    SECTION("Scalar - u64")
    {
        RunCompileTimeIsPowerOfTwoTests<u64>();
        RunCompileTimeIsPowerOfTwoEdgeTests<u64>();
    }

    SECTION("Vec2 - u8")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - u16")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - u32")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - u64")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec2ui64>();
    }

    SECTION("Vec3 - u8")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - u16")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - u32")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - u64")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec3ui64>();
    }

    SECTION("Vec4 - u8")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - u16")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - u32")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - u64")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec4ui64>();
    }
}
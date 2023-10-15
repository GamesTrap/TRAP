#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

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

        if constexpr(sizeof(T) > 32 || std::same_as<T, uint32_t>)
        {
            static_assert( TRAP::Math::IsPowerOfTwo(T(0x80)));
            static_assert( TRAP::Math::IsPowerOfTwo(T(0x80000000)));
        }
        else if constexpr(sizeof(T) > 8 || std::same_as<T, uint8_t>)
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

        if constexpr(sizeof(T) > 32 || std::same_as<T, uint32_t>)
        {
            static_assert( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x80))));
            static_assert( TRAP::Math::All(TRAP::Math::IsPowerOfTwo(T(0x80000000))));
        }
        else if constexpr(sizeof(T) > 8 || std::same_as<T, uint8_t>)
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
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeIsPowerOfTwoTests<uint8_t>();
        RunCompileTimeIsPowerOfTwoEdgeTests<uint8_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeIsPowerOfTwoTests<uint16_t>();
        RunCompileTimeIsPowerOfTwoEdgeTests<uint16_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeIsPowerOfTwoTests<uint32_t>();
        RunCompileTimeIsPowerOfTwoEdgeTests<uint32_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeIsPowerOfTwoTests<uint64_t>();
        RunCompileTimeIsPowerOfTwoEdgeTests<uint64_t>();
    }

    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec2ui64>();
    }

    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec3ui64>();
    }

    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeIsPowerOfTwoVecTests<TRAP::Math::Vec4ui64>();
    }
}
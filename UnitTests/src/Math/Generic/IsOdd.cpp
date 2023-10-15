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
    requires std::integral<T>
    consteval void RunCompileTimeIsOddTests()
    {
        if constexpr(std::unsigned_integral<T>)
        {
            static_assert( TRAP::Math::IsOdd(T(1)));
            static_assert(!TRAP::Math::IsOdd(T(2)));
            static_assert( TRAP::Math::IsOdd(T(37)));

            if constexpr(sizeof(T) >= 4)
            {
                static_assert(!TRAP::Math::IsOdd(T(888)));
                static_assert( TRAP::Math::IsOdd(T(1001)));
                static_assert( TRAP::Math::IsOdd(T(123456789)));
                static_assert( TRAP::Math::IsOdd(T(987654321)));
                static_assert(!TRAP::Math::IsOdd(T(1234567890)));
            }
            else if constexpr(sizeof(T) >= 2)
            {
                static_assert(!TRAP::Math::IsOdd(T(888)));
                static_assert( TRAP::Math::IsOdd(T(1001)));
            }
        }
        else if constexpr(std::signed_integral<T>)
        {
            static_assert(!TRAP::Math::IsOdd(T(-2)));
            static_assert( TRAP::Math::IsOdd(T(-1)));
            static_assert( TRAP::Math::IsOdd(T(1)));
            static_assert(!TRAP::Math::IsOdd(T(2)));
            static_assert( TRAP::Math::IsOdd(T(37)));

            if constexpr(sizeof(T) >= 4)
            {
                static_assert( TRAP::Math::IsOdd(T(-123456789)));
                static_assert( TRAP::Math::IsOdd(T(-999)));
                static_assert(!TRAP::Math::IsOdd(T(888)));
                static_assert( TRAP::Math::IsOdd(T(1001)));
                static_assert( TRAP::Math::IsOdd(T(123456789)));
                static_assert( TRAP::Math::IsOdd(T(987654321)));
                static_assert(!TRAP::Math::IsOdd(T(1234567890)));
            }
            else if constexpr(sizeof(T) >= 2)
            {
                static_assert( TRAP::Math::IsOdd(T(-999)));
                static_assert(!TRAP::Math::IsOdd(T(888)));
                static_assert( TRAP::Math::IsOdd(T(1001)));
            }
        }
    }

    template<typename T>
    requires std::integral<T>
    consteval void RunCompileTimeIsOddEdgeTests()
    {
        constexpr T min = std::numeric_limits<T>::min();
        constexpr T max = std::numeric_limits<T>::max();

        static_assert(!TRAP::Math::IsOdd(min));
        static_assert( TRAP::Math::IsOdd(max));
        static_assert(!TRAP::Math::IsOdd(T(0)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::integral<typename T::value_type>
    consteval void RunCompileTimeIsOddVecTests()
    {
        if constexpr(std::unsigned_integral<typename T::value_type>)
        {
            static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(1))));
            static_assert(!TRAP::Math::All(TRAP::Math::IsOdd(T(2))));
            static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(37))));

            if constexpr(sizeof(typename T::value_type) >= 4)
            {
                static_assert(!TRAP::Math::All(TRAP::Math::IsOdd(T(888))));
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(1001))));
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(123456789))));
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(987654321))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsOdd(T(1234567890))));
            }
            else if constexpr(sizeof(typename T::value_type) >= 2)
            {
                static_assert(!TRAP::Math::All(TRAP::Math::IsOdd(T(888))));
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(1001))));
            }
        }
        else if constexpr(std::signed_integral<typename T::value_type>)
        {
            static_assert(!TRAP::Math::All(TRAP::Math::IsOdd(T(-2))));
            static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(-1))));
            static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(1))));
            static_assert(!TRAP::Math::All(TRAP::Math::IsOdd(T(2))));
            static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(37))));

            if constexpr(sizeof(typename T::value_type) >= 4)
            {
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(-123456789))));
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(-999))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsOdd(T(888))));
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(1001))));
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(123456789))));
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(987654321))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsOdd(T(1234567890))));
            }
            else if constexpr(sizeof(typename T::value_type) >= 2)
            {
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(-999))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsOdd(T(888))));
                static_assert( TRAP::Math::All(TRAP::Math::IsOdd(T(1001))));
            }
        }
    }
}

TEST_CASE("TRAP::Math::IsOdd()", "[math][generic][isodd]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeIsOddEdgeTests<int8_t>();
        RunCompileTimeIsOddTests<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeIsOddEdgeTests<uint8_t>();
        RunCompileTimeIsOddTests<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeIsOddEdgeTests<int16_t>();
        RunCompileTimeIsOddTests<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeIsOddEdgeTests<uint16_t>();
        RunCompileTimeIsOddTests<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeIsOddEdgeTests<int32_t>();
        RunCompileTimeIsOddTests<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeIsOddEdgeTests<uint32_t>();
        RunCompileTimeIsOddTests<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeIsOddEdgeTests<int64_t>();
        RunCompileTimeIsOddTests<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeIsOddEdgeTests<uint64_t>();
        RunCompileTimeIsOddTests<uint64_t>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2ui64>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3ui64>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4ui64>();
    }
}
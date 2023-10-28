#include <cstdint>
#include <limits>

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
    SECTION("Scalar - i8")
    {
        RunCompileTimeIsOddEdgeTests<i8>();
        RunCompileTimeIsOddTests<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunCompileTimeIsOddEdgeTests<u8>();
        RunCompileTimeIsOddTests<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunCompileTimeIsOddEdgeTests<i16>();
        RunCompileTimeIsOddTests<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunCompileTimeIsOddEdgeTests<u16>();
        RunCompileTimeIsOddTests<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunCompileTimeIsOddEdgeTests<i32>();
        RunCompileTimeIsOddTests<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunCompileTimeIsOddEdgeTests<u32>();
        RunCompileTimeIsOddTests<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunCompileTimeIsOddEdgeTests<i64>();
        RunCompileTimeIsOddTests<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunCompileTimeIsOddEdgeTests<u64>();
        RunCompileTimeIsOddTests<u64>();
    }

    SECTION("Vec2 - i8")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - u8")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - i16")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - u16")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - i32")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - u32")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - i64")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - u64")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec2ui64>();
    }

    SECTION("Vec3 - i8")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - u8")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - i16")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - u16")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - i32")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - u32")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - i64")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - u64")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec3ui64>();
    }

    SECTION("Vec4 - i8")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - u8")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - i16")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - u16")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - i32")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - u32")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - i64")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - u64")
    {
        RunCompileTimeIsOddVecTests<TRAP::Math::Vec4ui64>();
    }
}
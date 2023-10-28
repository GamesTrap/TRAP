#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::integral<T>
    consteval void RunCompileTimeIsEvenTests()
    {
        if constexpr(std::unsigned_integral<T>)
        {
            static_assert(!TRAP::Math::IsEven(T(1)));
            static_assert( TRAP::Math::IsEven(T(2)));
            static_assert(!TRAP::Math::IsEven(T(37)));

            if constexpr(sizeof(T) >= 4)
            {
                static_assert( TRAP::Math::IsEven(T(888)));
                static_assert(!TRAP::Math::IsEven(T(1001)));
                static_assert(!TRAP::Math::IsEven(T(123456789)));
                static_assert(!TRAP::Math::IsEven(T(987654321)));
                static_assert( TRAP::Math::IsEven(T(1234567890)));
            }
            else if constexpr(sizeof(T) >= 2)
            {
                static_assert( TRAP::Math::IsEven(T(888)));
                static_assert(!TRAP::Math::IsEven(T(1001)));
            }
        }
        else if constexpr(std::signed_integral<T>)
        {
            static_assert( TRAP::Math::IsEven(T(-2)));
            static_assert(!TRAP::Math::IsEven(T(-1)));
            static_assert(!TRAP::Math::IsEven(T(1)));
            static_assert( TRAP::Math::IsEven(T(2)));
            static_assert(!TRAP::Math::IsEven(T(37)));

            if constexpr(sizeof(T) >= 4)
            {
                static_assert(!TRAP::Math::IsEven(T(-123456789)));
                static_assert(!TRAP::Math::IsEven(T(-999)));
                static_assert( TRAP::Math::IsEven(T(888)));
                static_assert(!TRAP::Math::IsEven(T(1001)));
                static_assert(!TRAP::Math::IsEven(T(123456789)));
                static_assert(!TRAP::Math::IsEven(T(987654321)));
                static_assert( TRAP::Math::IsEven(T(1234567890)));
            }
            else if constexpr(sizeof(T) >= 2)
            {
                static_assert(!TRAP::Math::IsEven(T(-999)));
                static_assert( TRAP::Math::IsEven(T(888)));
                static_assert(!TRAP::Math::IsEven(T(1001)));
            }
        }
    }

    template<typename T>
    requires std::integral<T>
    consteval void RunCompileTimeIsEvenEdgeTests()
    {
        constexpr T min = std::numeric_limits<T>::min();
        constexpr T max = std::numeric_limits<T>::max();

        static_assert( TRAP::Math::IsEven(min));
        static_assert(!TRAP::Math::IsEven(max));
        static_assert( TRAP::Math::IsEven(T(0)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::integral<typename T::value_type>
    consteval void RunCompileTimeIsEvenVecTests()
    {
        if constexpr(std::unsigned_integral<typename T::value_type>)
        {
            static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(1))));
            static_assert( TRAP::Math::All(TRAP::Math::IsEven(T(2))));
            static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(37))));

            if constexpr(sizeof(typename T::value_type) >= 4)
            {
                static_assert( TRAP::Math::All(TRAP::Math::IsEven(T(888))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(1001))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(123456789))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(987654321))));
                static_assert( TRAP::Math::All(TRAP::Math::IsEven(T(1234567890))));
            }
            else if constexpr(sizeof(typename T::value_type) >= 2)
            {
                static_assert( TRAP::Math::All(TRAP::Math::IsEven(T(888))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(1001))));
            }
        }
        else if constexpr(std::signed_integral<typename T::value_type>)
        {
            static_assert( TRAP::Math::All(TRAP::Math::IsEven(T(-2))));
            static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(-1))));
            static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(1))));
            static_assert( TRAP::Math::All(TRAP::Math::IsEven(T(2))));
            static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(37))));

            if constexpr(sizeof(typename T::value_type) >= 4)
            {
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(-123456789))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(-999))));
                static_assert( TRAP::Math::All(TRAP::Math::IsEven(T(888))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(1001))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(123456789))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(987654321))));
                static_assert( TRAP::Math::All(TRAP::Math::IsEven(T(1234567890))));
            }
            else if constexpr(sizeof(typename T::value_type) >= 2)
            {
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(-999))));
                static_assert( TRAP::Math::All(TRAP::Math::IsEven(T(888))));
                static_assert(!TRAP::Math::All(TRAP::Math::IsEven(T(1001))));
            }
        }
    }
}

TEST_CASE("TRAP::Math::IsEven()", "[math][generic][iseven]")
{
    SECTION("Scalar - i8")
    {
        RunCompileTimeIsEvenEdgeTests<i8>();
        RunCompileTimeIsEvenTests<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunCompileTimeIsEvenEdgeTests<u8>();
        RunCompileTimeIsEvenTests<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunCompileTimeIsEvenEdgeTests<i16>();
        RunCompileTimeIsEvenTests<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunCompileTimeIsEvenEdgeTests<u16>();
        RunCompileTimeIsEvenTests<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunCompileTimeIsEvenEdgeTests<i32>();
        RunCompileTimeIsEvenTests<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunCompileTimeIsEvenEdgeTests<u32>();
        RunCompileTimeIsEvenTests<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunCompileTimeIsEvenEdgeTests<i64>();
        RunCompileTimeIsEvenTests<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunCompileTimeIsEvenEdgeTests<u64>();
        RunCompileTimeIsEvenTests<u64>();
    }

    SECTION("Vec2 - i8")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - u8")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - i16")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - u16")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - i32")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - u32")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - i64")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - u64")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec2ui64>();
    }

    SECTION("Vec3 - i8")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - u8")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - i16")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - u16")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - i32")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - u32")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - i64")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - u64")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec3ui64>();
    }

    SECTION("Vec4 - i8")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - u8")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - i16")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - u16")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - i32")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - u32")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - i64")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - u64")
    {
        RunCompileTimeIsEvenVecTests<TRAP::Math::Vec4ui64>();
    }
}
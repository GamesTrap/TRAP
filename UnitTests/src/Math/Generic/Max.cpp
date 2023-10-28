#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunCompileTimeMaxScalarTest()
    {
        constexpr T N = static_cast<T>(0);
        constexpr T B = static_cast<T>(1);
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(N, B), B, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(B, N), B, TRAP::Math::Epsilon<T>()));

        constexpr T C = static_cast<T>(2);
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(N, B, C), C, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(B, N, C), C, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(C, N, B), C, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(C, B, N), C, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(B, C, N), C, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(N, C, B), C, TRAP::Math::Epsilon<T>()));

        constexpr T D = static_cast<T>(3);
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(D, N, B, C), D, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(B, D, N, C), D, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(C, N, D, B), D, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(C, B, D, N), D, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(B, C, N, D), D, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(N, C, B, D), D, TRAP::Math::Epsilon<T>()));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunMaxNaNTest()
    {
        static constexpr T NaN = std::numeric_limits<T>::quiet_NaN();
        static constexpr T B = static_cast<T>(1);
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Max(NaN, B)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, NaN)));

        static constexpr T C = static_cast<T>(2);
        REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Max(NaN, B, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, NaN, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, NaN, B)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, B, NaN)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, C, NaN)));
        REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Max(NaN, C, B)));

        static constexpr T D = static_cast<T>(3);
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(D, NaN, B, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, D, NaN, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, NaN, D, B)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, B, D, NaN)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, C, NaN, D)));
        REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Max(NaN, C, B, D)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T>
    consteval void RunCompileTimeMaxVecTest()
    {
        constexpr T N = static_cast<T>(0);
        constexpr T B = static_cast<T>(1);

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, static_cast<T::value_type>(N.x())), B, TRAP::Math::Epsilon<typename T::value_type>())));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, B), B, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, N), B, TRAP::Math::Epsilon<T>())));

        constexpr T C = static_cast<T>(2);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, B, C), C, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, N, C), C, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, N, B), C, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, B, N), C, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, C, N), C, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, C, B), C, TRAP::Math::Epsilon<T>())));

        constexpr T D = static_cast<T>(3);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(D, N, B, C), D, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, D, N, C), D, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, N, D, B), D, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, B, D, N), D, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, C, N, D), D, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, C, B, D), D, TRAP::Math::Epsilon<T>())));
    }
}

TEST_CASE("TRAP::Math::Max()", "[math][generic][max]")
{
    SECTION("Scalar - i8")
    {
        RunCompileTimeMaxScalarTest<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunCompileTimeMaxScalarTest<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunCompileTimeMaxScalarTest<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunCompileTimeMaxScalarTest<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunCompileTimeMaxScalarTest<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunCompileTimeMaxScalarTest<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunCompileTimeMaxScalarTest<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunCompileTimeMaxScalarTest<u64>();
    }
    SECTION("Scalar - f64")
    {
        RunCompileTimeMaxScalarTest<f64>();
        RunMaxNaNTest<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeMaxScalarTest<f32>();
        RunMaxNaNTest<f32>();
    }

    SECTION("Vec2 - i8")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - u8")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - i16")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - u16")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - i32")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - u32")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - i64")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - u64")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - f64")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - i8")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - u8")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - i16")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - u16")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - i32")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - u32")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - i64")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - u64")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - f64")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - i8")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - u8")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - i16")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - u16")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - i32")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - u32")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - i64")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - u64")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - f64")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4f>();
    }
}
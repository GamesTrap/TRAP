#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunCompileTimeMinScalarTest()
    {
        constexpr T N = static_cast<T>(0);
        constexpr T B = static_cast<T>(1);
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(N, B), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(B, N), N, TRAP::Math::Epsilon<T>()));

        constexpr T C = static_cast<T>(2);
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(N, B, C), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(B, N, C), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(C, N, B), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(C, B, N), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(B, C, N), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(N, C, B), N, TRAP::Math::Epsilon<T>()));

        constexpr T D = static_cast<T>(3);
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(D, N, B, C), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(B, D, N, C), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(C, N, D, B), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(C, B, D, N), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(B, C, N, D), N, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Min(N, C, B, D), N, TRAP::Math::Epsilon<T>()));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunMinNaNTest()
    {
        static constexpr T NaN = std::numeric_limits<T>::quiet_NaN();
        static constexpr T B = static_cast<T>(1);
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Min(NaN, B)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(B, NaN)));

        static constexpr T C = static_cast<T>(2);
        REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Min(NaN, B, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(B, NaN, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(C, NaN, B)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(C, B, NaN)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(B, C, NaN)));
        REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Min(NaN, C, B)));

        static constexpr T D = static_cast<T>(3);
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(D, NaN, B, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(B, D, NaN, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(C, NaN, D, B)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(C, B, D, NaN)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Min(B, C, NaN, D)));
        REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Min(NaN, C, B, D)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T>
    consteval void RunCompileTimeMinVecTest()
    {
        constexpr T N = static_cast<T>(0);
        constexpr T B = static_cast<T>(1);

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, static_cast<T::value_type>(N.x())), N, TRAP::Math::Epsilon<typename T::value_type>())));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, B), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, N), N, TRAP::Math::Epsilon<T>())));

        constexpr T C = static_cast<T>(2);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, B, C), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, N, C), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, N, B), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, B, N), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, C, N), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, C, B), N, TRAP::Math::Epsilon<T>())));

        constexpr T D = static_cast<T>(3);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(D, N, B, C), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, D, N, C), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, N, D, B), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, B, D, N), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, C, N, D), N, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, C, B, D), N, TRAP::Math::Epsilon<T>())));
    }
}

TEST_CASE("TRAP::Math::Min()", "[math][generic][min]")
{
    SECTION("Scalar - i8")
    {
        RunCompileTimeMinScalarTest<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunCompileTimeMinScalarTest<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunCompileTimeMinScalarTest<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunCompileTimeMinScalarTest<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunCompileTimeMinScalarTest<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunCompileTimeMinScalarTest<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunCompileTimeMinScalarTest<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunCompileTimeMinScalarTest<u64>();
    }
    SECTION("Scalar - f64")
    {
        RunCompileTimeMinScalarTest<f64>();
        RunMinNaNTest<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeMinScalarTest<f32>();
        RunMinNaNTest<f32>();
    }

    SECTION("Vec2 - i8")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - u8")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - i16")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - u16")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - i32")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - u32")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - i64")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - u64")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - f64")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - i8")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - u8")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - i16")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - u16")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - i32")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - u32")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - i64")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - u64")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - f64")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - i8")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - u8")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - i16")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - u16")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - i32")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - u32")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - i64")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - u64")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - f64")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4f>();
    }
}
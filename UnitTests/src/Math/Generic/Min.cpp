#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

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

TEST_CASE("TRAP::Math::Min()", "[math][generic][min]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeMinScalarTest<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeMinScalarTest<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeMinScalarTest<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeMinScalarTest<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeMinScalarTest<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeMinScalarTest<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeMinScalarTest<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeMinScalarTest<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeMinScalarTest<double>();
        RunMinNaNTest<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeMinScalarTest<float>();
        RunMinNaNTest<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeMinVecTest<TRAP::Math::Vec4f>();
    }
}
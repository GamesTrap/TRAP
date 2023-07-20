#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::is_arithmetic_v<T>
void RunMinScalarTest()
{
    constexpr T N = static_cast<T>(0);
    constexpr T B = static_cast<T>(1);
    constexpr bool a0 = TRAP::Math::Equal(TRAP::Math::Min(N, B), N, TRAP::Math::Epsilon<T>());
    constexpr bool a1 = TRAP::Math::Equal(TRAP::Math::Min(B, N), N, TRAP::Math::Epsilon<T>());
    static_assert(a0, "TRAP::Math::Min(N, B) == N failed");
    static_assert(a1, "TRAP::Math::Min(B, N) == N failed");
    REQUIRE(a0 == true);
    REQUIRE(a1 == true);

    constexpr T C = static_cast<T>(2);
    constexpr bool b0 = TRAP::Math::Equal(TRAP::Math::Min(N, B, C), N, TRAP::Math::Epsilon<T>());
    constexpr bool b1 = TRAP::Math::Equal(TRAP::Math::Min(B, N, C), N, TRAP::Math::Epsilon<T>());
    constexpr bool b2 = TRAP::Math::Equal(TRAP::Math::Min(C, N, B), N, TRAP::Math::Epsilon<T>());
    constexpr bool b3 = TRAP::Math::Equal(TRAP::Math::Min(C, B, N), N, TRAP::Math::Epsilon<T>());
    constexpr bool b4 = TRAP::Math::Equal(TRAP::Math::Min(B, C, N), N, TRAP::Math::Epsilon<T>());
    constexpr bool b5 = TRAP::Math::Equal(TRAP::Math::Min(N, C, B), N, TRAP::Math::Epsilon<T>());
    static_assert(b0, "TRAP::Math::Min(N, B, C) == N failed");
    static_assert(b1, "TRAP::Math::Min(B, N, C) == N failed");
    static_assert(b2, "TRAP::Math::Min(C, N, B) == N failed");
    static_assert(b3, "TRAP::Math::Min(C, B, N) == N failed");
    static_assert(b4, "TRAP::Math::Min(B, C, N) == N failed");
    static_assert(b5, "TRAP::Math::Min(N, C, B) == N failed");
    REQUIRE(b0 == true);
    REQUIRE(b1 == true);
    REQUIRE(b2 == true);
    REQUIRE(b3 == true);
    REQUIRE(b4 == true);
    REQUIRE(b5 == true);

    constexpr T D = static_cast<T>(3);
    constexpr bool c0 = TRAP::Math::Equal(TRAP::Math::Min(D, N, B, C), N, TRAP::Math::Epsilon<T>());
    constexpr bool c1 = TRAP::Math::Equal(TRAP::Math::Min(B, D, N, C), N, TRAP::Math::Epsilon<T>());
    constexpr bool c2 = TRAP::Math::Equal(TRAP::Math::Min(C, N, D, B), N, TRAP::Math::Epsilon<T>());
    constexpr bool c3 = TRAP::Math::Equal(TRAP::Math::Min(C, B, D, N), N, TRAP::Math::Epsilon<T>());
    constexpr bool c4 = TRAP::Math::Equal(TRAP::Math::Min(B, C, N, D), N, TRAP::Math::Epsilon<T>());
    constexpr bool c5 = TRAP::Math::Equal(TRAP::Math::Min(N, C, B, D), N, TRAP::Math::Epsilon<T>());
    static_assert(c0, "TRAP::Math::Min(D, N, B, C) == N failed");
    static_assert(c1, "TRAP::Math::Min(B, D, N, C) == N failed");
    static_assert(c2, "TRAP::Math::Min(C, N, D, B) == N failed");
    static_assert(c3, "TRAP::Math::Min(C, B, D, N) == N failed");
    static_assert(c4, "TRAP::Math::Min(B, C, N, D) == N failed");
    static_assert(c5, "TRAP::Math::Min(N, C, B, D) == N failed");
    REQUIRE(c0 == true);
    REQUIRE(c1 == true);
    REQUIRE(c2 == true);
    REQUIRE(c3 == true);
    REQUIRE(c4 == true);
    REQUIRE(c5 == true);
}

template<typename T>
requires std::floating_point<T>
void RunMinNaNTest()
{
    const T NaN = static_cast<T>(std::nan(""));
    constexpr T B = static_cast<T>(1);
    const bool a0 = TRAP::Math::IsNaN(TRAP::Math::Min(NaN, B));
    const bool a1 = !TRAP::Math::IsNaN(TRAP::Math::Min(B, NaN));
    REQUIRE(a0 == true);
    REQUIRE(a1 == true);

    constexpr T C = static_cast<T>(2);
    const bool b0 =  TRAP::Math::IsNaN(TRAP::Math::Min(NaN, B, C));
    const bool b1 = !TRAP::Math::IsNaN(TRAP::Math::Min(B, NaN, C));
    const bool b2 = !TRAP::Math::IsNaN(TRAP::Math::Min(C, NaN, B));
    const bool b3 = !TRAP::Math::IsNaN(TRAP::Math::Min(C, B, NaN));
    const bool b4 = !TRAP::Math::IsNaN(TRAP::Math::Min(B, C, NaN));
    const bool b5 =  TRAP::Math::IsNaN(TRAP::Math::Min(NaN, C, B));
    REQUIRE(b0 == true);
    REQUIRE(b1 == true);
    REQUIRE(b2 == true);
    REQUIRE(b3 == true);
    REQUIRE(b4 == true);
    REQUIRE(b5 == true);

    constexpr T D = static_cast<T>(3);
    const bool c0 = !TRAP::Math::IsNaN(TRAP::Math::Min(D, NaN, B, C));
    const bool c1 = !TRAP::Math::IsNaN(TRAP::Math::Min(B, D, NaN, C));
    const bool c2 = !TRAP::Math::IsNaN(TRAP::Math::Min(C, NaN, D, B));
    const bool c3 = !TRAP::Math::IsNaN(TRAP::Math::Min(C, B, D, NaN));
    const bool c4 = !TRAP::Math::IsNaN(TRAP::Math::Min(B, C, NaN, D));
    const bool c5 =  TRAP::Math::IsNaN(TRAP::Math::Min(NaN, C, B, D));
    REQUIRE(c0 == true);
    REQUIRE(c1 == true);
    REQUIRE(c2 == true);
    REQUIRE(c3 == true);
    REQUIRE(c4 == true);
    REQUIRE(c5 == true);
}

template<typename T>
requires TRAP::Math::IsVec<T>
void RunMinVecTest()
{
    constexpr T N = static_cast<T>(0);
    constexpr T B = static_cast<T>(1);

    constexpr T a0 = TRAP::Math::Min(B, static_cast<T::valueType>(N.x));
    constexpr bool a1 = TRAP::Math::All(TRAP::Math::Equal(a0, N, TRAP::Math::Epsilon<typename T::valueType>()));
    static_assert(a1, "TRAP::Math::Min(B, Scalar) == N failed");
    REQUIRE(a1 == true);

    constexpr bool b0 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, B), N, TRAP::Math::Epsilon<T>()));
    constexpr bool b1 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, N), N, TRAP::Math::Epsilon<T>()));
    static_assert(b0, "TRAP::Math::Min(N, B) == N failed");
    static_assert(b1, "TRAP::Math::Min(B, N) == N failed");
    REQUIRE(b0 == true);
    REQUIRE(b1 == true);

    constexpr T C = static_cast<T>(2);
    constexpr bool c0 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, B, C), N, TRAP::Math::Epsilon<T>()));
    constexpr bool c1 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, N, C), N, TRAP::Math::Epsilon<T>()));
    constexpr bool c2 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, N, B), N, TRAP::Math::Epsilon<T>()));
    constexpr bool c3 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, B, N), N, TRAP::Math::Epsilon<T>()));
    constexpr bool c4 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, C, N), N, TRAP::Math::Epsilon<T>()));
    constexpr bool c5 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, C, B), N, TRAP::Math::Epsilon<T>()));
    static_assert(c0, "TRAP::Math::Min(N, B, C) == N failed");
    static_assert(c1, "TRAP::Math::Min(B, N, C) == N failed");
    static_assert(c2, "TRAP::Math::Min(C, N, B) == N failed");
    static_assert(c3, "TRAP::Math::Min(C, B, N) == N failed");
    static_assert(c4, "TRAP::Math::Min(B, C, N) == N failed");
    static_assert(c5, "TRAP::Math::Min(N, C, B) == N failed");
    REQUIRE(c0 == true);
    REQUIRE(c1 == true);
    REQUIRE(c2 == true);
    REQUIRE(c3 == true);
    REQUIRE(c4 == true);
    REQUIRE(c5 == true);

    constexpr T D = static_cast<T>(3);
    constexpr bool d0 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(D, N, B, C), N, TRAP::Math::Epsilon<T>()));
    constexpr bool d1 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, D, N, C), N, TRAP::Math::Epsilon<T>()));
    constexpr bool d2 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, N, D, B), N, TRAP::Math::Epsilon<T>()));
    constexpr bool d3 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(C, B, D, N), N, TRAP::Math::Epsilon<T>()));
    constexpr bool d4 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(B, C, N, D), N, TRAP::Math::Epsilon<T>()));
    constexpr bool d5 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(N, C, B, D), N, TRAP::Math::Epsilon<T>()));
    static_assert(d0, "TRAP::Math::Min(D, N, B, C) == N failed");
    static_assert(d1, "TRAP::Math::Min(B, D, N, C) == N failed");
    static_assert(d2, "TRAP::Math::Min(C, N, D, B) == N failed");
    static_assert(d3, "TRAP::Math::Min(C, B, D, N) == N failed");
    static_assert(d4, "TRAP::Math::Min(B, C, N, D) == N failed");
    static_assert(d5, "TRAP::Math::Min(N, C, B, D) == N failed");
    REQUIRE(d0 == true);
    REQUIRE(d1 == true);
    REQUIRE(d2 == true);
    REQUIRE(d3 == true);
    REQUIRE(d4 == true);
    REQUIRE(d5 == true);
}

TEST_CASE("TRAP::Math::Min()", "[math][generic][min]")
{
    SECTION("Scalar - int8_t")
    {
        RunMinScalarTest<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunMinScalarTest<int8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunMinScalarTest<int8_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunMinScalarTest<int8_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunMinScalarTest<int8_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunMinScalarTest<int8_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunMinScalarTest<int8_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunMinScalarTest<int8_t>();
    }
    SECTION("Scalar - double")
    {
        RunMinScalarTest<int8_t>();
    }
    SECTION("Scalar - float")
    {
        RunMinScalarTest<int8_t>();
    }

    SECTION("NaN - double")
    {
        RunMinNaNTest<double>();
    }
    SECTION("NaN - float")
    {
        RunMinNaNTest<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunMinVecTest<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunMinVecTest<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunMinVecTest<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunMinVecTest<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunMinVecTest<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunMinVecTest<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunMinVecTest<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunMinVecTest<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunMinVecTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunMinVecTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunMinVecTest<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunMinVecTest<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunMinVecTest<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunMinVecTest<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunMinVecTest<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunMinVecTest<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunMinVecTest<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunMinVecTest<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunMinVecTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunMinVecTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunMinVecTest<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunMinVecTest<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunMinVecTest<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunMinVecTest<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunMinVecTest<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunMinVecTest<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunMinVecTest<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunMinVecTest<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunMinVecTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunMinVecTest<TRAP::Math::Vec4f>();
    }
}
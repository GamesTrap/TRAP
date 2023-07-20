#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::is_arithmetic_v<T>
void RunMaxScalarTest()
{
    constexpr T N = static_cast<T>(0);
    constexpr T B = static_cast<T>(1);
    constexpr bool a0 = TRAP::Math::Equal(TRAP::Math::Max(N, B), B, TRAP::Math::Epsilon<T>());
    constexpr bool a1 = TRAP::Math::Equal(TRAP::Math::Max(B, N), B, TRAP::Math::Epsilon<T>());
    static_assert(a0, "TRAP::Math::Max(N, B) == N failed");
    static_assert(a1, "TRAP::Math::Max(B, N) == N failed");
    REQUIRE(a0 == true);
    REQUIRE(a1 == true);

    constexpr T C = static_cast<T>(2);
    constexpr bool b0 = TRAP::Math::Equal(TRAP::Math::Max(N, B, C), C, TRAP::Math::Epsilon<T>());
    constexpr bool b1 = TRAP::Math::Equal(TRAP::Math::Max(B, N, C), C, TRAP::Math::Epsilon<T>());
    constexpr bool b2 = TRAP::Math::Equal(TRAP::Math::Max(C, N, B), C, TRAP::Math::Epsilon<T>());
    constexpr bool b3 = TRAP::Math::Equal(TRAP::Math::Max(C, B, N), C, TRAP::Math::Epsilon<T>());
    constexpr bool b4 = TRAP::Math::Equal(TRAP::Math::Max(B, C, N), C, TRAP::Math::Epsilon<T>());
    constexpr bool b5 = TRAP::Math::Equal(TRAP::Math::Max(N, C, B), C, TRAP::Math::Epsilon<T>());
    static_assert(b0, "TRAP::Math::Max(N, B, C) == C failed");
    static_assert(b1, "TRAP::Math::Max(B, N, C) == C failed");
    static_assert(b2, "TRAP::Math::Max(C, N, B) == C failed");
    static_assert(b3, "TRAP::Math::Max(C, B, N) == C failed");
    static_assert(b4, "TRAP::Math::Max(B, C, N) == C failed");
    static_assert(b5, "TRAP::Math::Max(N, C, B) == C failed");
    REQUIRE(b0 == true);
    REQUIRE(b1 == true);
    REQUIRE(b2 == true);
    REQUIRE(b3 == true);
    REQUIRE(b4 == true);
    REQUIRE(b5 == true);

    constexpr T D = static_cast<T>(3);
    constexpr bool c0 = TRAP::Math::Equal(TRAP::Math::Max(D, N, B, C), D, TRAP::Math::Epsilon<T>());
    constexpr bool c1 = TRAP::Math::Equal(TRAP::Math::Max(B, D, N, C), D, TRAP::Math::Epsilon<T>());
    constexpr bool c2 = TRAP::Math::Equal(TRAP::Math::Max(C, N, D, B), D, TRAP::Math::Epsilon<T>());
    constexpr bool c3 = TRAP::Math::Equal(TRAP::Math::Max(C, B, D, N), D, TRAP::Math::Epsilon<T>());
    constexpr bool c4 = TRAP::Math::Equal(TRAP::Math::Max(B, C, N, D), D, TRAP::Math::Epsilon<T>());
    constexpr bool c5 = TRAP::Math::Equal(TRAP::Math::Max(N, C, B, D), D, TRAP::Math::Epsilon<T>());
    static_assert(c0, "TRAP::Math::Max(D, N, B, C) == D failed");
    static_assert(c1, "TRAP::Math::Max(B, D, N, C) == D failed");
    static_assert(c2, "TRAP::Math::Max(C, N, D, B) == D failed");
    static_assert(c3, "TRAP::Math::Max(C, B, D, N) == D failed");
    static_assert(c4, "TRAP::Math::Max(B, C, N, D) == D failed");
    static_assert(c5, "TRAP::Math::Max(N, C, B, D) == D failed");
    REQUIRE(c0 == true);
    REQUIRE(c1 == true);
    REQUIRE(c2 == true);
    REQUIRE(c3 == true);
    REQUIRE(c4 == true);
    REQUIRE(c5 == true);
}

template<typename T>
requires std::floating_point<T>
void RunMaxNaNTest()
{
    const T NaN = static_cast<T>(std::nan(""));
    constexpr T B = static_cast<T>(1);
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Max(NaN, B)) == true);
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Max(B, NaN)) != true);

    constexpr T C = static_cast<T>(2);
    const bool b0 =  TRAP::Math::IsNaN(TRAP::Math::Max(NaN, B, C));
    const bool b1 = !TRAP::Math::IsNaN(TRAP::Math::Max(B, NaN, C));
    const bool b2 = !TRAP::Math::IsNaN(TRAP::Math::Max(C, NaN, B));
    const bool b3 = !TRAP::Math::IsNaN(TRAP::Math::Max(C, B, NaN));
    const bool b4 = !TRAP::Math::IsNaN(TRAP::Math::Max(B, C, NaN));
    const bool b5 =  TRAP::Math::IsNaN(TRAP::Math::Max(NaN, C, B));
    REQUIRE(b0 == true);
    REQUIRE(b1 == true);
    REQUIRE(b2 == true);
    REQUIRE(b3 == true);
    REQUIRE(b4 == true);
    REQUIRE(b5 == true);

    constexpr T D = static_cast<T>(3);
    const bool c0 = !TRAP::Math::IsNaN(TRAP::Math::Max(D, NaN, B, C));
    const bool c1 = !TRAP::Math::IsNaN(TRAP::Math::Max(B, D, NaN, C));
    const bool c2 = !TRAP::Math::IsNaN(TRAP::Math::Max(C, NaN, D, B));
    const bool c3 = !TRAP::Math::IsNaN(TRAP::Math::Max(C, B, D, NaN));
    const bool c4 = !TRAP::Math::IsNaN(TRAP::Math::Max(B, C, NaN, D));
    const bool c5 =  TRAP::Math::IsNaN(TRAP::Math::Max(NaN, C, B, D));
    REQUIRE(c0 == true);
    REQUIRE(c1 == true);
    REQUIRE(c2 == true);
    REQUIRE(c3 == true);
    REQUIRE(c4 == true);
    REQUIRE(c5 == true);
}

template<typename T>
requires TRAP::Math::IsVec<T>
void RunMaxVecTest()
{
    constexpr T N = static_cast<T>(0);
    constexpr T B = static_cast<T>(1);

    constexpr T a0 = TRAP::Math::Max(B, static_cast<T::valueType>(N.x));
    constexpr bool a1 = TRAP::Math::All(TRAP::Math::Equal(a0, B, TRAP::Math::Epsilon<typename T::valueType>()));
    static_assert(a1, "TRAP::Math::Max(B, Scalar) == B failed");
    REQUIRE(a1 == true);

    constexpr bool b0 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, B), B, TRAP::Math::Epsilon<T>()));
    constexpr bool b1 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, N), B, TRAP::Math::Epsilon<T>()));
    static_assert(b0, "TRAP::Math::Max(N, B) == B failed");
    static_assert(b1, "TRAP::Math::Max(B, N) == B failed");
    REQUIRE(b0 == true);
    REQUIRE(b1 == true);

    constexpr T C = static_cast<T>(2);
    constexpr bool c0 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, B, C), C, TRAP::Math::Epsilon<T>()));
    constexpr bool c1 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, N, C), C, TRAP::Math::Epsilon<T>()));
    constexpr bool c2 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, N, B), C, TRAP::Math::Epsilon<T>()));
    constexpr bool c3 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, B, N), C, TRAP::Math::Epsilon<T>()));
    constexpr bool c4 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, C, N), C, TRAP::Math::Epsilon<T>()));
    constexpr bool c5 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, C, B), C, TRAP::Math::Epsilon<T>()));
    static_assert(c0, "TRAP::Math::Max(N, B, C) == C failed");
    static_assert(c1, "TRAP::Math::Max(B, N, C) == C failed");
    static_assert(c2, "TRAP::Math::Max(C, N, B) == C failed");
    static_assert(c3, "TRAP::Math::Max(C, B, N) == C failed");
    static_assert(c4, "TRAP::Math::Max(B, C, N) == C failed");
    static_assert(c5, "TRAP::Math::Max(N, C, B) == C failed");
    REQUIRE(c0 == true);
    REQUIRE(c1 == true);
    REQUIRE(c2 == true);
    REQUIRE(c3 == true);
    REQUIRE(c4 == true);
    REQUIRE(c5 == true);

    constexpr T D = static_cast<T>(3);
    constexpr bool d0 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(D, N, B, C), D, TRAP::Math::Epsilon<T>()));
    constexpr bool d1 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, D, N, C), D, TRAP::Math::Epsilon<T>()));
    constexpr bool d2 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, N, D, B), D, TRAP::Math::Epsilon<T>()));
    constexpr bool d3 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, B, D, N), D, TRAP::Math::Epsilon<T>()));
    constexpr bool d4 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, C, N, D), D, TRAP::Math::Epsilon<T>()));
    constexpr bool d5 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, C, B, D), D, TRAP::Math::Epsilon<T>()));
    static_assert(d0, "TRAP::Math::Max(D, N, B, C) == D failed");
    static_assert(d1, "TRAP::Math::Max(B, D, N, C) == D failed");
    static_assert(d2, "TRAP::Math::Max(C, N, D, B) == D failed");
    static_assert(d3, "TRAP::Math::Max(C, B, D, N) == D failed");
    static_assert(d4, "TRAP::Math::Max(B, C, N, D) == D failed");
    static_assert(d5, "TRAP::Math::Max(N, C, B, D) == D failed");
    REQUIRE(d0 == true);
    REQUIRE(d1 == true);
    REQUIRE(d2 == true);
    REQUIRE(d3 == true);
    REQUIRE(d4 == true);
    REQUIRE(d5 == true);
}

TEST_CASE("TRAP::Math::Max()", "[math][generic][max]")
{
    SECTION("Scalar - int8_t")
    {
        RunMaxScalarTest<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunMaxScalarTest<int8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunMaxScalarTest<int8_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunMaxScalarTest<int8_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunMaxScalarTest<int8_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunMaxScalarTest<int8_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunMaxScalarTest<int8_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunMaxScalarTest<int8_t>();
    }
    SECTION("Scalar - double")
    {
        RunMaxScalarTest<int8_t>();
    }
    SECTION("Scalar - float")
    {
        RunMaxScalarTest<int8_t>();
    }

    SECTION("NaN - double")
    {
        RunMaxNaNTest<double>();
    }
    SECTION("NaN - float")
    {
        RunMaxNaNTest<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunMaxVecTest<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunMaxVecTest<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunMaxVecTest<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunMaxVecTest<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunMaxVecTest<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunMaxVecTest<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunMaxVecTest<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunMaxVecTest<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunMaxVecTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunMaxVecTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunMaxVecTest<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunMaxVecTest<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunMaxVecTest<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunMaxVecTest<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunMaxVecTest<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunMaxVecTest<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunMaxVecTest<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunMaxVecTest<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunMaxVecTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunMaxVecTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunMaxVecTest<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunMaxVecTest<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunMaxVecTest<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunMaxVecTest<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunMaxVecTest<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunMaxVecTest<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunMaxVecTest<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunMaxVecTest<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunMaxVecTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunMaxVecTest<TRAP::Math::Vec4f>();
    }
}
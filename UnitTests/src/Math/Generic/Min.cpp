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
}

template<typename T>
requires TRAP::Math::IsVec<T>
void RunMinVecTest()
{
    constexpr T a0 = TRAP::Math::Min(T(static_cast<T::valueType>(1)), T(static_cast<T::valueType>(1)));
    constexpr T a1 = TRAP::Math::Min(T(static_cast<T::valueType>(1)), static_cast<T::valueType>(1));
    constexpr bool a2 = TRAP::Math::All(TRAP::Math::Equal(a0, a1, TRAP::Math::Epsilon<typename T::valueType>()));
    static_assert(a2);
    REQUIRE(a2 == true);
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
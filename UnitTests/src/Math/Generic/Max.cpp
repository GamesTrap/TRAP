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
}

template<typename T>
requires std::floating_point<T>
void RunMaxNaNTest()
{
    const T NaN = static_cast<T>(std::nan(""));
    constexpr T B = static_cast<T>(1);
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Max(NaN, B)) == true);
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Max(B, NaN)) == false);
}

template<typename T>
requires TRAP::Math::IsVec<T>
void RunMaxVecTest()
{
    constexpr T a0 = TRAP::Math::Max(T(static_cast<T::valueType>(1)), T(static_cast<T::valueType>(1)));
    constexpr T a1 = TRAP::Math::Max(T(static_cast<T::valueType>(1)), static_cast<T::valueType>(1));
    constexpr bool a2 = TRAP::Math::All(TRAP::Math::Equal(a0, a1, TRAP::Math::Epsilon<typename T::valueType>()));
    static_assert(a2);
    REQUIRE(a2 == true);
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
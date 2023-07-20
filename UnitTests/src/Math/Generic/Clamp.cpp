#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::is_arithmetic_v<T>
void RunClampScalarTest()
{
    constexpr T N = static_cast<T>(1);
    constexpr T B = static_cast<T>(10);

    const bool a0 = TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N, B), T(5), TRAP::Math::Epsilon<T>());
    const bool a1 = TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N, B), T(1), TRAP::Math::Epsilon<T>());
    const bool a2 = TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N, B), T(10), TRAP::Math::Epsilon<T>());
    const bool a3 = TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N, B), T(1), TRAP::Math::Epsilon<T>());
    const bool a4 = TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N, B), T(10), TRAP::Math::Epsilon<T>());
    REQUIRE(a0 == true);
    REQUIRE(a1 == true);
    REQUIRE(a2 == true);
    REQUIRE(a3 == true);
    REQUIRE(a4 == true);
}

template<typename T>
requires TRAP::Math::IsVec<T>
void RunClampVecTest()
{
    constexpr T N = static_cast<T>(1);
    constexpr T B = static_cast<T>(10);

    const bool a0 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N, B), T(5), TRAP::Math::Epsilon<T>()));
    const bool a1 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N, B), T(1), TRAP::Math::Epsilon<T>()));
    const bool a2 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N, B), T(10), TRAP::Math::Epsilon<T>()));
    const bool a3 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N, B), T(1), TRAP::Math::Epsilon<T>()));
    const bool a4 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N, B), T(10), TRAP::Math::Epsilon<T>()));
    REQUIRE(a0 == true);
    REQUIRE(a1 == true);
    REQUIRE(a2 == true);
    REQUIRE(a3 == true);
    REQUIRE(a4 == true);

    constexpr typename T::valueType N1 = static_cast<T::valueType>(1);
    constexpr typename T::valueType B1 = static_cast<T::valueType>(10);

    const bool b0 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N1, B1), T(5), TRAP::Math::Epsilon<T>()));
    const bool b1 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N1, B1), T(1), TRAP::Math::Epsilon<T>()));
    const bool b2 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N1, B1), T(10), TRAP::Math::Epsilon<T>()));
    const bool b3 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N1, B1), T(1), TRAP::Math::Epsilon<T>()));
    const bool b4 = TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N1, B1), T(10), TRAP::Math::Epsilon<T>()));
    REQUIRE(b0 == true);
    REQUIRE(b1 == true);
    REQUIRE(b2 == true);
    REQUIRE(b3 == true);
    REQUIRE(b4 == true);
}

TEST_CASE("TRAP::Math::Clamp()", "[math][generic][clamp]")
{
    SECTION("Scalar - int8_t")
    {
        RunClampScalarTest<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunClampScalarTest<int8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunClampScalarTest<int8_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunClampScalarTest<int8_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunClampScalarTest<int8_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunClampScalarTest<int8_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunClampScalarTest<int8_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunClampScalarTest<int8_t>();
    }
    SECTION("Scalar - double")
    {
        RunClampScalarTest<int8_t>();
    }
    SECTION("Scalar - float")
    {
        RunClampScalarTest<int8_t>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunClampVecTest<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunClampVecTest<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunClampVecTest<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunClampVecTest<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunClampVecTest<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunClampVecTest<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunClampVecTest<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunClampVecTest<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunClampVecTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunClampVecTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunClampVecTest<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunClampVecTest<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunClampVecTest<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunClampVecTest<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunClampVecTest<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunClampVecTest<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunClampVecTest<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunClampVecTest<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunClampVecTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunClampVecTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunClampVecTest<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunClampVecTest<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunClampVecTest<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunClampVecTest<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunClampVecTest<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunClampVecTest<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunClampVecTest<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunClampVecTest<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunClampVecTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunClampVecTest<TRAP::Math::Vec4f>();
    }
}
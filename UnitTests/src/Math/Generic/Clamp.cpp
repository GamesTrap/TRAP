#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::is_arithmetic_v<T>
consteval void RunCompileTimeClampScalarTest()
{
    constexpr T N = static_cast<T>(1);
    constexpr T B = static_cast<T>(10);

    static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N, B), T(5), TRAP::Math::Epsilon<T>()));
    static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N, B), T(1), TRAP::Math::Epsilon<T>()));
    static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N, B), T(10), TRAP::Math::Epsilon<T>()));
    static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N, B), T(1), TRAP::Math::Epsilon<T>()));
    static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N, B), T(10), TRAP::Math::Epsilon<T>()));
}

template<typename T>
requires TRAP::Math::IsVec<T>
consteval void RunCompileTimeClampVecTest()
{
    constexpr T N = static_cast<T>(1);
    constexpr T B = static_cast<T>(10);

    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N, B), T(5), TRAP::Math::Epsilon<T>())));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N, B), T(1), TRAP::Math::Epsilon<T>())));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N, B), T(10), TRAP::Math::Epsilon<T>())));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N, B), T(1), TRAP::Math::Epsilon<T>())));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N, B), T(10), TRAP::Math::Epsilon<T>())));

    constexpr typename T::value_type N1 = static_cast<T::value_type>(1);
    constexpr typename T::value_type B1 = static_cast<T::value_type>(10);

    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N1, B1), T(5), TRAP::Math::Epsilon<T>())));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N1, B1), T(1), TRAP::Math::Epsilon<T>())));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N1, B1), T(10), TRAP::Math::Epsilon<T>())));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N1, B1), T(1), TRAP::Math::Epsilon<T>())));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N1, B1), T(10), TRAP::Math::Epsilon<T>())));
}

template<typename T>
requires std::is_arithmetic_v<T>
void RunClampEdgeTests()
{
    static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static constexpr T max = std::numeric_limits<T>::max();
    static constexpr T min = std::numeric_limits<T>::lowest();
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(max, min, max), max, Epsilon));
    if constexpr(std::floating_point<T>)
    {
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Clamp(nan, T(0.0), T(1.0))));
        static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(5.0), nan, nan), T(5.0), Epsilon));
    }
}

TEST_CASE("TRAP::Math::Clamp()", "[math][generic][clamp]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeClampScalarTest<int8_t>();
        RunClampEdgeTests<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeClampScalarTest<uint8_t>();
        RunClampEdgeTests<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeClampScalarTest<int16_t>();
        RunClampEdgeTests<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeClampScalarTest<uint16_t>();
        RunClampEdgeTests<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeClampScalarTest<int32_t>();
        RunClampEdgeTests<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeClampScalarTest<uint32_t>();
        RunClampEdgeTests<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeClampScalarTest<int64_t>();
        RunClampEdgeTests<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeClampScalarTest<uint64_t>();
        RunClampEdgeTests<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeClampScalarTest<double>();
        RunClampEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeClampScalarTest<float>();
        RunClampEdgeTests<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4f>();
    }
}
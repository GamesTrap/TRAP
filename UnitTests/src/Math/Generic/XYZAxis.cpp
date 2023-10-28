#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunXAxisTest()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::XAxis<T>(), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f))));
    }
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunYAxisTest()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::YAxis<T>(), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f))));
    }
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunZAxisTest()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ZAxis<T>(), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f))));
    }
}

TEST_CASE("TRAP::Math::XAxis()", "[math][generic][xaxis]")
{
    SECTION("Scalar - i8")
    {
        RunXAxisTest<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunXAxisTest<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunXAxisTest<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunXAxisTest<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunXAxisTest<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunXAxisTest<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunXAxisTest<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunXAxisTest<u64>();
    }
    SECTION("Scalar - f32")
    {
        RunXAxisTest<f32>();
    }
    SECTION("Scalar - f64")
    {
        RunXAxisTest<f64>();
    }
}

TEST_CASE("TRAP::Math::YAxis()", "[math][generic][yaxis]")
{
    SECTION("Scalar - i8")
    {
        RunYAxisTest<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunYAxisTest<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunYAxisTest<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunYAxisTest<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunYAxisTest<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunYAxisTest<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunYAxisTest<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunYAxisTest<u64>();
    }
    SECTION("Scalar - f32")
    {
        RunYAxisTest<f32>();
    }
    SECTION("Scalar - f64")
    {
        RunYAxisTest<f64>();
    }
}

TEST_CASE("TRAP::Math::ZAxis()", "[math][generic][zaxis]")
{
    SECTION("Scalar - i8")
    {
        RunZAxisTest<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunZAxisTest<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunZAxisTest<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunZAxisTest<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunZAxisTest<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunZAxisTest<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunZAxisTest<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunZAxisTest<u64>();
    }
    SECTION("Scalar - f32")
    {
        RunZAxisTest<f32>();
    }
    SECTION("Scalar - f64")
    {
        RunZAxisTest<f64>();
    }
}
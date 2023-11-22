#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunXAxisCompileTimeTest()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::XAxis<T>(), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f))));
    }
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunYAxisCompileTimeTest()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::YAxis<T>(), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f))));
    }
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunZAxisCompileTimeTest()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ZAxis<T>(), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f))));
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunXAxisRunTimeTest()
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::XAxis<T>(), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f))));
    }
    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunYAxisRunTimeTest()
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::YAxis<T>(), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f))));
    }
    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunZAxisRunTimeTest()
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ZAxis<T>(), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f))));
    }
}

TEST_CASE("TRAP::Math::XAxis()", "[math][generic][xaxis]")
{
    SECTION("Scalar - i8")
    {
        RunXAxisCompileTimeTest<i8>();
        RunXAxisRunTimeTest<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunXAxisCompileTimeTest<u8>();
        RunXAxisRunTimeTest<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunXAxisCompileTimeTest<i16>();
        RunXAxisRunTimeTest<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunXAxisCompileTimeTest<u16>();
        RunXAxisRunTimeTest<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunXAxisCompileTimeTest<i32>();
        RunXAxisRunTimeTest<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunXAxisCompileTimeTest<u32>();
        RunXAxisRunTimeTest<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunXAxisCompileTimeTest<i64>();
        RunXAxisRunTimeTest<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunXAxisCompileTimeTest<u64>();
        RunXAxisRunTimeTest<u64>();
    }
    SECTION("Scalar - f32")
    {
        RunXAxisCompileTimeTest<f32>();
        RunXAxisRunTimeTest<f32>();
    }
    SECTION("Scalar - f64")
    {
        RunXAxisCompileTimeTest<f64>();
        RunXAxisRunTimeTest<f64>();
    }
}

TEST_CASE("TRAP::Math::YAxis()", "[math][generic][yaxis]")
{
    SECTION("Scalar - i8")
    {
        RunYAxisCompileTimeTest<i8>();
        RunYAxisRunTimeTest<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunYAxisCompileTimeTest<u8>();
        RunYAxisRunTimeTest<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunYAxisCompileTimeTest<i16>();
        RunYAxisRunTimeTest<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunYAxisCompileTimeTest<u16>();
        RunYAxisRunTimeTest<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunYAxisCompileTimeTest<i32>();
        RunYAxisRunTimeTest<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunYAxisCompileTimeTest<u32>();
        RunYAxisRunTimeTest<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunYAxisCompileTimeTest<i64>();
        RunYAxisRunTimeTest<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunYAxisCompileTimeTest<u64>();
        RunYAxisRunTimeTest<u64>();
    }
    SECTION("Scalar - f32")
    {
        RunYAxisCompileTimeTest<f32>();
        RunYAxisRunTimeTest<f32>();
    }
    SECTION("Scalar - f64")
    {
        RunYAxisCompileTimeTest<f64>();
        RunYAxisRunTimeTest<f64>();
    }
}

TEST_CASE("TRAP::Math::ZAxis()", "[math][generic][zaxis]")
{
    SECTION("Scalar - i8")
    {
        RunZAxisCompileTimeTest<i8>();
        RunZAxisRunTimeTest<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunZAxisCompileTimeTest<u8>();
        RunZAxisRunTimeTest<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunZAxisCompileTimeTest<i16>();
        RunZAxisRunTimeTest<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunZAxisCompileTimeTest<u16>();
        RunZAxisRunTimeTest<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunZAxisCompileTimeTest<i32>();
        RunZAxisRunTimeTest<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunZAxisCompileTimeTest<u32>();
        RunZAxisRunTimeTest<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunZAxisCompileTimeTest<i64>();
        RunZAxisRunTimeTest<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunZAxisCompileTimeTest<u64>();
        RunZAxisRunTimeTest<u64>();
    }
    SECTION("Scalar - f32")
    {
        RunZAxisCompileTimeTest<f32>();
        RunZAxisRunTimeTest<f32>();
    }
    SECTION("Scalar - f64")
    {
        RunZAxisCompileTimeTest<f64>();
        RunZAxisRunTimeTest<f64>();
    }
}
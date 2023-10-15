#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

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
    SECTION("Scalar - int8_t")
    {
        RunXAxisTest<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunXAxisTest<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunXAxisTest<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunXAxisTest<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunXAxisTest<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunXAxisTest<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunXAxisTest<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunXAxisTest<uint64_t>();
    }
    SECTION("Scalar - float")
    {
        RunXAxisTest<float>();
    }
    SECTION("Scalar - double")
    {
        RunXAxisTest<double>();
    }
}

TEST_CASE("TRAP::Math::YAxis()", "[math][generic][yaxis]")
{
    SECTION("Scalar - int8_t")
    {
        RunYAxisTest<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunYAxisTest<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunYAxisTest<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunYAxisTest<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunYAxisTest<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunYAxisTest<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunYAxisTest<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunYAxisTest<uint64_t>();
    }
    SECTION("Scalar - float")
    {
        RunYAxisTest<float>();
    }
    SECTION("Scalar - double")
    {
        RunYAxisTest<double>();
    }
}

TEST_CASE("TRAP::Math::ZAxis()", "[math][generic][zaxis]")
{
    SECTION("Scalar - int8_t")
    {
        RunZAxisTest<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunZAxisTest<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunZAxisTest<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunZAxisTest<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunZAxisTest<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunZAxisTest<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunZAxisTest<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunZAxisTest<uint64_t>();
    }
    SECTION("Scalar - float")
    {
        RunZAxisTest<float>();
    }
    SECTION("Scalar - double")
    {
        RunZAxisTest<double>();
    }
}
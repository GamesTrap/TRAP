#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires (TRAP::Math::IsVec3<T> || TRAP::Math::IsVec4<T>) && std::floating_point<typename T::valueType>
void RunConvertLinearToSRGBTests()
{
    const T colorSourceRGB(TRAP::Math::Vec<4, typename T::valueType>(1.0, 0.5, 0.0, 1.0));

    {
        const T colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSourceRGB);
        const T expected(TRAP::Math::Vec<4, typename T::valueType>(1.0, 0.735361, 0.0, 1.0));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorSRGB, expected, typename T::valueType(0.000001f))));
    }
    {
        const T colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSourceRGB, typename T::valueType(2.2f));
        const T expected(TRAP::Math::Vec<4, typename T::valueType>(1.0, 0.714876, 0.0, 1.0));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorSRGB, expected, typename T::valueType(0.000001f))));
    }
}

template<typename T>
requires (TRAP::Math::IsVec3<T> || TRAP::Math::IsVec4<T>) && std::floating_point<typename T::valueType>
void RunConvertSRGBToLinearTests()
{
    const T colorSourceSRGB(TRAP::Math::Vec<4, typename T::valueType>(1.0, 0.714876, 0.0, 1.0));

    {
        const T colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSourceSRGB);
        const T expected(TRAP::Math::Vec<4, typename T::valueType>(1.0, 0.469466, 0.0, 1.0));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorRGB, expected, typename T::valueType(0.000001f))));
    }
    {
        const T colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSourceSRGB, typename T::valueType(2.2f));
        const T expected(TRAP::Math::Vec<4, typename T::valueType>(1.0, 0.5, 0.0, 1.0));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorRGB, expected, typename T::valueType(0.000001f))));
    }
}

TEST_CASE("TRAP::Math::ConvertLinearToSRGB()", "[math][generic][convertlineartosrgb]")
{
    SECTION("Vec3 - double")
    {
        RunConvertLinearToSRGBTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunConvertLinearToSRGBTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunConvertLinearToSRGBTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunConvertLinearToSRGBTests<TRAP::Math::Vec4f>();
    }
}

TEST_CASE("TRAP::Math::ConvertSRGBToLinear()", "[math][generic][convertsrgbtolinear]")
{
    SECTION("Vec3 - double")
    {
        RunConvertSRGBToLinearTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunConvertSRGBToLinearTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunConvertSRGBToLinearTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunConvertSRGBToLinearTests<TRAP::Math::Vec4f>();
    }
}
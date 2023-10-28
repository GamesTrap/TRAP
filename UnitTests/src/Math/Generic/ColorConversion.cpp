#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires (TRAP::Math::IsVec3<T> || TRAP::Math::IsVec4<T>) && std::floating_point<typename T::value_type>
    consteval void RunConvertLinearToSRGBCompileTimeTests()
    {
        constexpr T colorSourceRGB(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.5, 0.0, 1.0));

        {
            constexpr T colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSourceRGB);
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.735361, 0.0, 1.0));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(colorSRGB, expected, typename T::value_type(0.000001f))));
        }
        {
            constexpr T colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSourceRGB, typename T::value_type(2.2f));
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.714876, 0.0, 1.0));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(colorSRGB, expected, typename T::value_type(0.000001f))));
        }
    }

    template<typename T>
    requires (TRAP::Math::IsVec3<T> || TRAP::Math::IsVec4<T>) && std::floating_point<typename T::value_type>
    void RunConvertLinearToSRGBRunTimeTests()
    {
        const T colorSourceRGB(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.5, 0.0, 1.0));

        {
            const T colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSourceRGB);
            const T expected(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.735361, 0.0, 1.0));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorSRGB, expected, typename T::value_type(0.000001f))));
        }
        {
            const T colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSourceRGB, typename T::value_type(2.2f));
            const T expected(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.714876, 0.0, 1.0));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorSRGB, expected, typename T::value_type(0.000001f))));
        }
    }

    template<typename T>
    requires (TRAP::Math::IsVec3<T> || TRAP::Math::IsVec4<T>) && std::floating_point<typename T::value_type>
    consteval void RunConvertSRGBToLinearCompileTimeTests()
    {
        constexpr T colorSourceSRGB(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.714876, 0.0, 1.0));

        {
            constexpr T colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSourceSRGB);
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.469466, 0.0, 1.0));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(colorRGB, expected, typename T::value_type(0.000001f))));
        }
        {
            constexpr T colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSourceSRGB, typename T::value_type(2.2f));
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.5, 0.0, 1.0));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(colorRGB, expected, typename T::value_type(0.000001f))));
        }
    }

    template<typename T>
    requires (TRAP::Math::IsVec3<T> || TRAP::Math::IsVec4<T>) && std::floating_point<typename T::value_type>
    void RunConvertSRGBToLinearRunTimeTests()
    {
        const T colorSourceSRGB(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.714876, 0.0, 1.0));

        {
            const T colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSourceSRGB);
            const T expected(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.469466, 0.0, 1.0));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorRGB, expected, typename T::value_type(0.000001f))));
        }
        {
            const T colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSourceSRGB, typename T::value_type(2.2f));
            const T expected(TRAP::Math::Vec<4, typename T::value_type>(1.0, 0.5, 0.0, 1.0));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorRGB, expected, typename T::value_type(0.000001f))));
        }
    }
}

TEST_CASE("TRAP::Math::ConvertLinearToSRGB()", "[math][generic][convertlineartosrgb]")
{
    SECTION("Vec3 - f64")
    {
        RunConvertLinearToSRGBRunTimeTests<TRAP::Math::Vec3d>();
        RunConvertLinearToSRGBCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunConvertLinearToSRGBRunTimeTests<TRAP::Math::Vec3f>();
        RunConvertLinearToSRGBCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunConvertLinearToSRGBRunTimeTests<TRAP::Math::Vec4d>();
        RunConvertLinearToSRGBCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunConvertLinearToSRGBRunTimeTests<TRAP::Math::Vec4f>();
        RunConvertLinearToSRGBCompileTimeTests<TRAP::Math::Vec4f>();
    }
}

TEST_CASE("TRAP::Math::ConvertSRGBToLinear()", "[math][generic][convertsrgbtolinear]")
{
    SECTION("Vec3 - f64")
    {
        RunConvertSRGBToLinearRunTimeTests<TRAP::Math::Vec3d>();
        RunConvertSRGBToLinearCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunConvertSRGBToLinearRunTimeTests<TRAP::Math::Vec3f>();
        RunConvertSRGBToLinearCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunConvertSRGBToLinearRunTimeTests<TRAP::Math::Vec4d>();
        RunConvertSRGBToLinearCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunConvertSRGBToLinearRunTimeTests<TRAP::Math::Vec4f>();
        RunConvertSRGBToLinearCompileTimeTests<TRAP::Math::Vec4f>();
    }
}
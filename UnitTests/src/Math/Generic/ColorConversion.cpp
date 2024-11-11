#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::ConvertLinearToSRGB()", "[math][generic][convertlineartosrgb][vec]",
                   TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType colorSourceRGB(TRAP::Math::Vec<4, Scalar>(1.0, 0.5, 0.0, 1.0));

        {
            static constexpr TestType colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSourceRGB);
            static constexpr TestType expected(TRAP::Math::Vec<4, Scalar>(1.0, 0.735361, 0.0, 1.0));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(colorSRGB, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSourceRGB, Scalar(2.2f));
            static constexpr TestType expected(TRAP::Math::Vec<4, Scalar>(1.0, 0.714876, 0.0, 1.0));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(colorSRGB, expected, Scalar(0.000001f))));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType colorSourceRGB(TRAP::Math::Vec<4, Scalar>(1.0, 0.5, 0.0, 1.0));

        {
            const TestType colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSourceRGB);
            static constexpr TestType expected(TRAP::Math::Vec<4, Scalar>(1.0, 0.735361, 0.0, 1.0));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorSRGB, expected, Scalar(0.000001f))));
        }
        {
            const TestType colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSourceRGB, Scalar(2.2f));
            static constexpr TestType expected(TRAP::Math::Vec<4, Scalar>(1.0, 0.714876, 0.0, 1.0));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorSRGB, expected, Scalar(0.000001f))));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::ConvertSRGBToLinear()", "[math][generic][convertsrgbtolinear][vec]",
                   TRAP::Math::Vec3d, TRAP::Math::Vec3f, TRAP::Math::Vec4d, TRAP::Math::Vec4f)
{
    using Scalar = TestType::value_type;

    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType colorSourceSRGB(TRAP::Math::Vec<4, Scalar>(1.0, 0.714876, 0.0, 1.0));

        {
            static constexpr TestType colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSourceSRGB);
            static constexpr TestType expected(TRAP::Math::Vec<4, Scalar>(1.0, 0.469466, 0.0, 1.0));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(colorRGB, expected, Scalar(0.000001f))));
        }
        {
            static constexpr TestType colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSourceSRGB, Scalar(2.2f));
            static constexpr TestType expected(TRAP::Math::Vec<4, Scalar>(1.0, 0.5, 0.0, 1.0));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(colorRGB, expected, Scalar(0.000001f))));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType colorSourceSRGB(TRAP::Math::Vec<4, Scalar>(1.0, 0.714876, 0.0, 1.0));

        {
            const TestType colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSourceSRGB);
            static constexpr TestType expected(TRAP::Math::Vec<4, Scalar>(1.0, 0.469466, 0.0, 1.0));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorRGB, expected, Scalar(0.000001f))));
        }
        {
            const TestType colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSourceSRGB, Scalar(2.2f));
            static constexpr TestType expected(TRAP::Math::Vec<4, Scalar>(1.0, 0.5, 0.0, 1.0));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(colorRGB, expected, Scalar(0.000001f))));
        }
    }
}

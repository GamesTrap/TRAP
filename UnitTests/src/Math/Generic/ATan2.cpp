#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunATan2CompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(0.2), T(0.0)), T(1.5708), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(-0.2), T(0.0)), T(-1.5708), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(0.001), T(0.001)), T(0.785398), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(0.49), T(0.49)), T(0.785398), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(-0.5), T(-0.5)), T(-2.35619), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(0.5), T(-0.5)), T(2.35619), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(-0.5), T(0.5)), T(-0.785398), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(9.6), T(8.4)), T(0.851966), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(1.0), T(0.0)), T(1.5708), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(0.0), T(1.0)), T(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(-1.0), T(0.0)), T(-1.5708), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(0.0), T(-1.0)), T(3.14159), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(1.0), T(3.0)), T(0.321751), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(-5.0), T(2.5)), T(-1.10715), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(-1000.0), T(-0.001)), T(-1.5708), T(0.00001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunATan2RunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<std::pair<T, T>, 16> values
        {
            std::pair(T(0.0), T(0.0)), std::pair(T(0.2), T(0.0)), std::pair(T(-0.2), T(0.0)),
            std::pair(T(0.001), T(0.001)), std::pair(T(0.49), T(0.49)), std::pair(T(-0.5), T(-0.5)),
            std::pair(T(0.5), T(-0.5)), std::pair(T(-0.5), T(0.5)), std::pair(T(9.6), T(8.4)),
            std::pair(T(1.0), T(0.0)), std::pair(T(0.0), T(1.0)), std::pair(T(-1.0), T(0.0)),
            std::pair(T(0.0), T(-1.0)), std::pair(T(1.0), T(3.0)), std::pair(T(-5.0), T(2.5)),
            std::pair(T(-1000.0), T(-0.001)),
        };

        for(const auto& [y, x] : values)
        {
            REQUIRE_THAT(TRAP::Math::ATan(y, x), Catch::Matchers::WithinRel(std::atan2(y, x), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunATan2EdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(nan, T(1.0))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(T(1.0), nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(nan, nan)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunATan2VecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(0.2f)), T(typename T::value_type(0.0f))), T(1.5708f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(-0.2f)), T(typename T::value_type(0.0f))), T(-1.5708f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(0.001f)), T(typename T::value_type(0.001f))), T(0.785398f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(0.49f)), T(typename T::value_type(0.49f))), T(0.785398f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(-0.5f)), T(typename T::value_type(-0.5f))), T(-2.35619f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(0.5f)), T(typename T::value_type(-0.5f))), T(2.35619f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(-0.5f)), T(typename T::value_type(0.5f))), T(-0.785398f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(9.6f)), T(typename T::value_type(8.4f))), T(0.851966f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(1.0f)), T(typename T::value_type(0.0f))), T(1.5708f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(0.0f)), T(typename T::value_type(1.0f))), T(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(-1.0f)), T(typename T::value_type(0.0f))), T(-1.5708f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(0.0f)), T(typename T::value_type(-1.0f))), T(3.14159f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(1.0f)), T(typename T::value_type(3.0f))), T(0.321751f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(-5.0f)), T(typename T::value_type(2.5f))), T(-1.10715f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(typename T::value_type(-1000.0f)), T(typename T::value_type(-0.001f))), T(-1.5708f), T(0.00001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunATan2VecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<std::pair<T, T>, 16> values
        {
            std::pair(T(typename T::value_type(0.0)), T(typename T::value_type(0.0))),
            std::pair(T(typename T::value_type(0.2)), T(typename T::value_type(0.0))),
            std::pair(T(typename T::value_type(-0.2)), T(typename T::value_type(0.0))),
            std::pair(T(typename T::value_type(0.001)), T(typename T::value_type(0.001))),
            std::pair(T(typename T::value_type(0.49)), T(typename T::value_type(0.49))),
            std::pair(T(typename T::value_type(-0.5)), T(typename T::value_type(-0.5))),
            std::pair(T(typename T::value_type(0.5)), T(typename T::value_type(-0.5))),
            std::pair(T(typename T::value_type(-0.5)), T(typename T::value_type(0.5))),
            std::pair(T(typename T::value_type(9.6)), T(typename T::value_type(8.4))),
            std::pair(T(typename T::value_type(1.0)), T(typename T::value_type(0.0))),
            std::pair(T(typename T::value_type(0.0)), T(typename T::value_type(1.0))),
            std::pair(T(typename T::value_type(-1.0)), T(typename T::value_type(0.0))),
            std::pair(T(typename T::value_type(0.0)), T(typename T::value_type(-1.0))),
            std::pair(T(typename T::value_type(1.0)), T(typename T::value_type(3.0))),
            std::pair(T(typename T::value_type(-5.0)), T(typename T::value_type(2.5))),
            std::pair(T(typename T::value_type(-1000.0)), T(typename T::value_type(-0.001))),
        };

        for(const auto& [y, x] : values)
        {
            const T s = TRAP::Math::ATan(y, x);
            for(uint32_t l = 0; l < T::Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::atan2(y[l], x[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunATan2VecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(T(nan), T(1.0)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(T(1.0), T(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(T(nan), T(nan)))));
    }
}

TEST_CASE("TRAP::Math::ATan(y, x)", "[math][generic][atan2]")
{
    SECTION("Scalar - double")
    {
        RunATan2RunTimeTests<double>();
        RunATan2CompileTimeTests<double>();
        RunATan2EdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunATan2RunTimeTests<float>();
        RunATan2CompileTimeTests<float>();
        RunATan2EdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunATan2VecRunTimeTests<TRAP::Math::Vec2d>();
        RunATan2VecCompileTimeTests<TRAP::Math::Vec2d>();
        RunATan2VecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunATan2VecRunTimeTests<TRAP::Math::Vec2f>();
        RunATan2VecCompileTimeTests<TRAP::Math::Vec2f>();
        RunATan2VecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunATan2VecRunTimeTests<TRAP::Math::Vec3d>();
        RunATan2VecCompileTimeTests<TRAP::Math::Vec3d>();
        RunATan2VecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunATan2VecRunTimeTests<TRAP::Math::Vec3f>();
        RunATan2VecCompileTimeTests<TRAP::Math::Vec3f>();
        RunATan2VecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunATan2VecRunTimeTests<TRAP::Math::Vec4d>();
        RunATan2VecCompileTimeTests<TRAP::Math::Vec4d>();
        RunATan2VecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunATan2VecRunTimeTests<TRAP::Math::Vec4f>();
        RunATan2VecCompileTimeTests<TRAP::Math::Vec4f>();
        RunATan2VecEdgeTests<TRAP::Math::Vec4f>();
    }
}

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
    requires std::floating_point<T>
    consteval void RunLog2CompileTimeTests()
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Log2(T(0.5f)), T(-1.0f), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log2(T(1.0f)), T(0.0f), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log2(T(1.5f)), T(0.584962f), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log2(T(41.5f)), T(5.37504f), T(0.000001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunLog2RunTimeTests()
    {
        static constexpr std::array<std::tuple<T, T>, 4> values
        {
            std::tuple(T(0.5f), T(-1.0f)),
            std::tuple(T(1.0f), T(0.0f)),
            std::tuple(T(1.5f), T(0.584962f)),
            std::tuple(T(41.5f), T(5.37504f))
        };

        for(const auto& [x, expected] : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Log2(x), expected, T(0.000001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunLog2VecCompileTimeTests()
    {
        constexpr T x(TRAP::Math::Vec<4, typename T::value_type>(16.0f, 8.0f, 4.0f, 2.0f));
        constexpr T res(TRAP::Math::Vec<4, typename T::value_type>(4.0f, 3.0f, 2.0f, 1.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log2(x), res, T(0.01f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunLog2VecRunTimeTests()
    {
        static constexpr T x(TRAP::Math::Vec<4, typename T::value_type>(16.0f, 8.0f, 4.0f, 2.0f));
        static constexpr T res(TRAP::Math::Vec<4, typename T::value_type>(4.0f, 3.0f, 2.0f, 1.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log2(x), res, T(0.01f))));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunLog2EdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T min = std::numeric_limits<T>::lowest();

        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Log2(T(0.0f))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log2(min)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Log2(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log2(ninf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log2(nan)));
    }
}

TEST_CASE("TRAP::Math::Log2()", "[math][generic][log2]")
{
    SECTION("Scalar - double")
    {
        RunLog2RunTimeTests<double>();
        RunLog2CompileTimeTests<double>();
        RunLog2EdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunLog2RunTimeTests<float>();
        RunLog2CompileTimeTests<float>();
        RunLog2EdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunLog2VecRunTimeTests<TRAP::Math::Vec2d>();
        RunLog2VecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunLog2VecRunTimeTests<TRAP::Math::Vec2f>();
        RunLog2VecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunLog2VecRunTimeTests<TRAP::Math::Vec3d>();
        RunLog2VecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunLog2VecRunTimeTests<TRAP::Math::Vec3f>();
        RunLog2VecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunLog2VecRunTimeTests<TRAP::Math::Vec4d>();
        RunLog2VecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunLog2VecRunTimeTests<TRAP::Math::Vec4f>();
        RunLog2VecCompileTimeTests<TRAP::Math::Vec4f>();
    }
}
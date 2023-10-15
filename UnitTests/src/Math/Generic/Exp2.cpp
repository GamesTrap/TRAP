#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    void RunExp2Tests()
    {
        static constexpr std::array<std::tuple<T, T>, 6> values
        {
            std::tuple(T(23.0f), T(8388608.f)),
            std::tuple(T(2.5f), T(5.65685f)),
            std::tuple(T(-1.75f), T(0.297302f)),
            std::tuple(T(0.0f), T(1.0f)),
            std::tuple(T(10.0f), T(1024.0f)),
            std::tuple(T(-5.0f), T(0.03125f)),
        };

        for(const auto& [num, expected] : values)
        {
            const T res = TRAP::Math::Exp2(num);
            REQUIRE(TRAP::Math::Equal(res, expected, T(0.00001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunExp2VecTests()
    {
        static constexpr T x(TRAP::Math::Vec<4, typename T::value_type>(4.0f, 3.0f, 2.0f, 1.0f));
        static constexpr T res(TRAP::Math::Vec<4, typename T::value_type>(16.0f, 8.0f, 4.0f, 2.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp2(x), res, T(0.01f))));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunExp2EdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::lowest();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Exp2(nan)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Exp2(inf)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp2(ninf), T(0.0f), Epsilon));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Exp2(max)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp2(min), T(0.0f), Epsilon));
    }
}

TEST_CASE("TRAP::Math::Exp2()", "[math][generic][exp2]")
{
    SECTION("Scalar - double")
    {
        RunExp2Tests<double>();
        RunExp2EdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunExp2Tests<float>();
        RunExp2EdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunExp2VecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunExp2VecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunExp2VecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunExp2VecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunExp2VecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunExp2VecTests<TRAP::Math::Vec4f>();
    }
}
#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunCompileTimeLerpTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Lerp(T(5.0f), T(15.0f), T(0.5f)), T(10.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Lerp(T(2.5f), T(7.8f), T(0.75f)), T(6.475f), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Lerp(T(-10.0f), T(20.0f), T(0.2f)), T(-4.0f), T(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Lerp(T(8.0f), T(13.0f), T(0.0f)), T(8.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Lerp(T(8.0f), T(13.0f), T(1.0f)), T(13.0f), Epsilon));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunCompileTimeLerpVecTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            constexpr T x(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 3.0f, 4.0f));
            constexpr T y(TRAP::Math::Vec<4, typename T::value_type>(5.0f, 6.0f, 7.0f, 8.0f));
            constexpr typename T::value_type a = 0.3f;
            constexpr T a2(a);
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(2.2f, 3.2f, 4.2f, 5.2f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a), expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a2), expected, T(0.000001f))));
        }
        {
            constexpr T x(TRAP::Math::Vec<4, typename T::value_type>(-1.0f, -2.0f, -3.0f, -4.0f));
            constexpr T y(TRAP::Math::Vec<4, typename T::value_type>(-5.0f, -6.0f, -7.0f, -8.0f));
            constexpr typename T::value_type a = 0.5f;
            constexpr T a2(a);
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(-3.0f, -4.0f, -5.0f, -6.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a), expected, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a2), expected, Epsilon)));
        }
        {
            constexpr T x(TRAP::Math::Vec<4, typename T::value_type>(-1.0f,  2.0f, -3.5f,  4.0f));
            constexpr T y(TRAP::Math::Vec<4, typename T::value_type>( 4.0f, -3.0f,  1.5f, -2.0f));
            constexpr typename T::value_type a = 0.6f;
            constexpr T a2(a);
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(2.0f, -1.0f, -0.5f, 0.4f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a), expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a2), expected, T(0.000001f))));
        }
        {
            constexpr T x(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            constexpr T y(TRAP::Math::Vec<4, typename T::value_type>(5.0f, 3.0f, 10.0f, 2.0f));
            constexpr typename T::value_type a = 0.8f;
            constexpr T a2(a);
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(4.0f, 2.4f, 8.0f, 1.6f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a), expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(x, y, a2), expected, T(0.000001f))));
        }
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    consteval void RunCompileTimeLerpQuatTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            constexpr T q1(1.0f, 2.0f, 3.0f, 4.0f);
            constexpr T q2(5.0f, 6.0f, 7.0f, 8.0f);
            constexpr T res(3.0f, 4.0f, 5.0f, 6.0f);
            constexpr typename T::value_type a(0.5f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(q1, q2, a), res, Epsilon)));
        }
        {
            constexpr T q(1.0f, 0.0f, 0.0f, 0.0f);
            constexpr typename T::value_type a(0.3f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(q, q, a), q, Epsilon)));
        }
        {
            constexpr T q1(1.0f, 0.0f, 0.0f, 0.0f);
            constexpr T q2(0.5f, 0.5f, 0.5f, 0.5f);
            constexpr T res(0.65f, 0.35f, 0.35f, 0.35f);
            constexpr typename T::value_type a(0.7f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(q1, q2, a), res, typename T::value_type(0.0000001f))));
        }
        {
            constexpr T q1(-1.0f, -2.0f, -3.0f, -4.0f);
            constexpr T q2(-5.0f, -6.0f, -7.0f, -8.0f);
            constexpr T res(-2.0f, -3.0f, -4.0f, -5.0f);
            constexpr typename T::value_type a(0.25f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Lerp(q1, q2, a), res, Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunLerpEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();

        static_assert(TRAP::Math::Equal(TRAP::Math::Lerp(max, T(100.0f), T(0.5f)), (max / 2.0f), Epsilon));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Lerp(nan, T(10.0f), T(0.3f))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Lerp(ninf, inf, T(0.6f))));
    }
}

TEST_CASE("TRAP::Math::Lerp()", "[math][generic][lerp]")
{
    SECTION("Scalar - double")
    {
        RunCompileTimeLerpTests<double>();
        RunLerpEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeLerpTests<float>();
        RunLerpEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunCompileTimeLerpVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeLerpVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunCompileTimeLerpVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeLerpVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCompileTimeLerpVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeLerpVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunCompileTimeLerpQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunCompileTimeLerpQuatTests<TRAP::Math::Quatf>();
    }
}
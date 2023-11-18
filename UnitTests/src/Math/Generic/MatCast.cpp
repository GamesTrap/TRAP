#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    void RunMat3CastTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            static constexpr TRAP::Math::tQuat<T> q1(1.0f, 0.0f, 0.0f, 0.0f);
            static constexpr TRAP::Math::tMat3<T> res = TRAP::Math::Mat3Cast(q1);
            static constexpr TRAP::Math::tMat3<T> expected(1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> q1 = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(45.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            const TRAP::Math::tMat3<T> res = TRAP::Math::Mat3Cast(q1);
            const TRAP::Math::tMat3<T> expected = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(T(1.0f)), TRAP::Math::Radians(T(45.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q1(0.7071f, 0.0f, 0.0f, 0.7071f);
            static constexpr TRAP::Math::tMat3<T> res = TRAP::Math::Mat3Cast(q1);
            static constexpr TRAP::Math::tMat3<T> expected(0.000019f, 0.999981f, 0.0f, -0.999981f, 0.000019f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, T(0.000001f))));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunMat3CastEdgeTests()
    {
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            static constexpr TRAP::Math::tQuat<T> q1(min, min, min, min);
            const TRAP::Math::tMat3<T> res = TRAP::Math::Mat3Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q1(max, max, max, max);
            const TRAP::Math::tMat3<T> res = TRAP::Math::Mat3Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q1(inf, inf, inf, inf);
            const TRAP::Math::tMat3<T> res = TRAP::Math::Mat3Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q1(ninf, ninf, ninf, ninf);
            const TRAP::Math::tMat3<T> res = TRAP::Math::Mat3Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q1(nan, nan, nan, nan);
            const TRAP::Math::tMat3<T> res = TRAP::Math::Mat3Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunMat4CastTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            static constexpr TRAP::Math::tQuat<T> q1(1.0f, 0.0f, 0.0f, 0.0f);
            static constexpr TRAP::Math::tMat4<T> res = TRAP::Math::Mat4Cast(q1);
            static constexpr TRAP::Math::tMat4<T> expected(1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> q1 = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(45.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            const TRAP::Math::tMat4<T> res = TRAP::Math::Mat4Cast(q1);
            const TRAP::Math::tMat4<T> expected = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(T(1.0f)), TRAP::Math::Radians(T(45.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q1(0.7071f, 0.0f, 0.0f, 0.7071f);
            static constexpr TRAP::Math::tMat4<T> res = TRAP::Math::Mat4Cast(q1);
            static constexpr TRAP::Math::tMat4<T> expected(0.000019f, 0.999981f, 0.0f, 0.0f, -0.999981f, 0.000019f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, T(0.000001f))));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunMat4CastEdgeTests()
    {
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            static constexpr TRAP::Math::tQuat<T> q1(min, min, min, min);
            const TRAP::Math::tMat4<T> res = TRAP::Math::Mat4Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q1(max, max, max, max);
            const TRAP::Math::tMat4<T> res = TRAP::Math::Mat4Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q1(inf, inf, inf, inf);
            const TRAP::Math::tMat4<T> res = TRAP::Math::Mat4Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q1(ninf, ninf, ninf, ninf);
            const TRAP::Math::tMat4<T> res = TRAP::Math::Mat4Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tQuat<T> q1(nan, nan, nan, nan);
            const TRAP::Math::tMat4<T> res = TRAP::Math::Mat4Cast(q1);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
    }
}

TEST_CASE("TRAP::Math::Mat3Cast()", "[math][generic][mat3cast]")
{
    SECTION("Quat - f64")
    {
        RunMat3CastTests<f64>();
        RunMat3CastEdgeTests<f64>();
    }
    SECTION("Quat - f32")
    {
        RunMat3CastTests<f32>();
        RunMat3CastEdgeTests<f32>();
    }
}

TEST_CASE("TRAP::Math::Mat4Cast()", "[math][generic][mat4cast]")
{
    SECTION("Quat - f64")
    {
        RunMat4CastTests<f64>();
        RunMat4CastEdgeTests<f64>();
    }
    SECTION("Quat - f32")
    {
        RunMat4CastTests<f32>();
        RunMat4CastEdgeTests<f32>();
    }
}

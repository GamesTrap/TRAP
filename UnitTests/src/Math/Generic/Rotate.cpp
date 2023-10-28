#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunRotateCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            constexpr auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f));
            constexpr auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f));
            constexpr TRAP::Math::tMat4<T> expected(0.333333f, 0.910684f, -0.244017f, 0.0f, -0.244017f, 0.333333f, 0.910684f, 0.0f, 0.910684f, -0.244017f, 0.333333f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            constexpr auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            constexpr auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            constexpr auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(45.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            constexpr auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(45.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            constexpr TRAP::Math::tMat4<T> expected(0.707107f, 0.0f, -0.707107f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.707107f, 0.0f, 0.707107f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            constexpr auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
            constexpr auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
            constexpr TRAP::Math::tMat4<T> expected(0.5f, 0.866025f, 0.0f, 0.0f, -0.866025f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            constexpr auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.5f, 0.5f, 0.5f));
            constexpr auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.5f, 0.5f, 0.5f));
            constexpr TRAP::Math::tMat4<T> expected(0.666667f, 0.666667f, -0.333333f, 0.0f, -0.333333f, 0.666667f, 0.666667f, 0.0f, 0.666667f, -0.333333f, 0.666667f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunRotateRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f));
            static constexpr TRAP::Math::tMat4<T> expected(0.333333f, 0.910684f, -0.244017f, 0.0f, -0.244017f, 0.333333f, 0.910684f, 0.0f, 0.910684f, -0.244017f, 0.333333f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            static constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(45.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(45.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            static constexpr TRAP::Math::tMat4<T> expected(0.707107f, 0.0f, -0.707107f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.707107f, 0.0f, 0.707107f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.0f, 0.0f, 1.0f));
            static constexpr TRAP::Math::tMat4<T> expected(0.5f, 0.866025f, 0.0f, 0.0f, -0.866025f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.5f, 0.5f, 0.5f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(60.0f), TRAP::Math::tVec3<T>(0.5f, 0.5f, 0.5f));
            static constexpr TRAP::Math::tMat4<T> expected(0.666667f, 0.666667f, -0.333333f, 0.0f, -0.333333f, 0.666667f, 0.666667f, 0.0f, 0.666667f, -0.333333f, 0.666667f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, T(0.000001f))));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunRotateEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(min, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(min, 1.0f, 1.0f));
            static constexpr TRAP::Math::tMat4<T> expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(max, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(max, 1.0f, 1.0f));
            static constexpr TRAP::Math::tMat4<T> expected(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(t, t2, Epsilon)));
        }
        {
            const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(inf, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(inf, 1.0f, 1.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0>(t))));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0>(t2))));
        }
        {
            const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(ninf, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(ninf, 1.0f, 1.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0>(t))));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0>(t2))));
        }
        {
            const auto t = TRAP::Math::Rotate(TRAP::Math::tMat4<T>(1.0f), TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(nan, 1.0f, 1.0f));
            const auto t2 = TRAP::Math::Rotate(TRAP::Math::Radians<T>(90.0f), TRAP::Math::tVec3<T>(nan, 1.0f, 1.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0>(t))));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(std::get<0>(t2))));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunRotateQuatCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            constexpr TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(45.0f)), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            constexpr TRAP::Math::tVec3<T> axis(0.0f, 0.0f, 1.0f);
            constexpr T angle = TRAP::Math::Radians(T(90.0f));
            constexpr TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, angle, axis);
            constexpr TRAP::Math::tQuat<T> expected(0.653281f, 0.270598f, -0.270598f, 0.653281f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(rot, expected, T(0.000001f))));
        }
        {
            constexpr TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(30.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            constexpr TRAP::Math::tVec3<T> axis(0.0f, 1.0f, 0.0f);
            constexpr T angle = TRAP::Math::Radians(T(45.0f));
            constexpr TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, angle, axis);
            constexpr TRAP::Math::tQuat<T> expected(0.793353f, 0.0f, 0.608761f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(rot, expected, T(0.000001f))));
        }
        {
            constexpr TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(60.0f)), TRAP::Math::tVec3<T>(1.0f, 1.0f, 1.0f));
            constexpr TRAP::Math::tVec3<T> axis(0.0f, 1.0f, 0.0f);
            constexpr T angle = TRAP::Math::Radians(T(-30.0f));
            constexpr TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, angle, axis);
            constexpr TRAP::Math::tQuat<T> expected(0.965926f, 0.612372f, 0.258819f, 0.353553f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(rot, expected, T(0.000001f))));
        }
        {
            constexpr TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(90.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            constexpr TRAP::Math::tVec3<T> axis(1.0f, 0.0f, 0.0f);
            constexpr T angle = TRAP::Math::Radians(T(0.0f));
            constexpr TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, angle, axis);
            constexpr TRAP::Math::tQuat<T> expected(0.707107f, 0.0f, 0.707107f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(rot, expected, T(0.000001f))));
        }
        {
            constexpr TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(0.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            constexpr TRAP::Math::tVec3<T> axis(1.0f, 0.0f, 0.0f);
            constexpr T angle = TRAP::Math::Radians(T(180.0f));
            constexpr TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, angle, axis);
            constexpr TRAP::Math::tQuat<T> expected(0.0f, 1.0f, 0.0f, 0.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunRotateQuatRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(45.0f)), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            static constexpr TRAP::Math::tVec3<T> axis(0.0f, 0.0f, 1.0f);
            static constexpr T angle = TRAP::Math::Radians(T(90.0f));
            const TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, angle, axis);
            const TRAP::Math::tQuat<T> expected(0.653281f, 0.270598f, -0.270598f, 0.653281f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, expected, T(0.000001f))));
        }
        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(30.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            static constexpr TRAP::Math::tVec3<T> axis(0.0f, 1.0f, 0.0f);
            static constexpr T angle = TRAP::Math::Radians(T(45.0f));
            const TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, angle, axis);
            const TRAP::Math::tQuat<T> expected(0.793353f, 0.0f, 0.608761f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, expected, T(0.000001f))));
        }
        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(60.0f)), TRAP::Math::tVec3<T>(1.0f, 1.0f, 1.0f));
            static constexpr TRAP::Math::tVec3<T> axis(0.0f, 1.0f, 0.0f);
            static constexpr T angle = TRAP::Math::Radians(T(-30.0f));
            const TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, angle, axis);
            const TRAP::Math::tQuat<T> expected(0.965926f, 0.612372f, 0.258819f, 0.353553f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, expected, T(0.000001f))));
        }
        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(90.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            static constexpr TRAP::Math::tVec3<T> axis(1.0f, 0.0f, 0.0f);
            static constexpr T angle = TRAP::Math::Radians(T(0.0f));
            const TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, angle, axis);
            const TRAP::Math::tQuat<T> expected(0.707107f, 0.0f, 0.707107f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, expected, T(0.000001f))));
        }
        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(0.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            static constexpr TRAP::Math::tVec3<T> axis(1.0f, 0.0f, 0.0f);
            static constexpr T angle = TRAP::Math::Radians(T(180.0f));
            const TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, angle, axis);
            const TRAP::Math::tQuat<T> expected(0.0f, 1.0f, 0.0f, 0.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, expected, Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunRotateQuatEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        if constexpr(std::same_as<T, f32>)
        {
            {
                const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(45.0f)), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
                static constexpr TRAP::Math::tVec3<T> axis(0.0f, 1.0f, 0.0f);
                const TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, min, axis);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, TRAP::Math::tQuat<T>(-0.889284074f, -0.368353546f, -0.250453979f, -0.103741445f), Epsilon)));
            }
            {
                const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(30.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
                static constexpr TRAP::Math::tVec3<T> axis(0.0f, 0.0f, 1.0f);
                const TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, max, axis);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(rot, TRAP::Math::tQuat<T>(-0.929755926f, 0.0701631084f, -0.249127343f, 0.261852294f), Epsilon)));
            }
        }
        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(60.0f)), TRAP::Math::tVec3<T>(1.0f, 1.0f, 1.0f));
            static constexpr TRAP::Math::tVec3<T> axis(1.0f, 0.0f, 0.0f);
            const TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, inf, axis);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(rot)));
        }
        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(90.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            static constexpr TRAP::Math::tVec3<T> axis(1.0f, 0.0f, 0.0f);
            const TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, ninf, axis);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(rot)));
        }
        {
            const TRAP::Math::tQuat<T> q = TRAP::Math::AngleAxis(TRAP::Math::Radians(T(0.0f)), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            static constexpr TRAP::Math::tVec3<T> axis(1.0f, 0.0f, 0.0f);
            const TRAP::Math::tQuat<T> rot = TRAP::Math::Rotate(q, nan, axis);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(rot)));
        }
    }
}

TEST_CASE("TRAP::Math::Rotate()", "[math][generic][rotate]")
{
    SECTION("Scalar - f64")
    {
        RunRotateRunTimeTests<f64>();
        RunRotateCompileTimeTests<f64>();
        RunRotateEdgeTests<f64>();
        RunRotateQuatRunTimeTests<f64>();
        RunRotateQuatCompileTimeTests<f64>();
        RunRotateQuatEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunRotateRunTimeTests<f32>();
        RunRotateCompileTimeTests<f32>();
        RunRotateEdgeTests<f32>();
        RunRotateQuatRunTimeTests<f32>();
        RunRotateQuatCompileTimeTests<f32>();
        RunRotateQuatEdgeTests<f32>();
    }
}

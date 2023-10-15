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
    consteval void RunSLerpCompileTimeTests()
    {
        constexpr T Epsilon = T(0.00001f);

        {
            constexpr TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            constexpr TRAP::Math::tQuat<T> q2(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            constexpr TRAP::Math::tQuat<T> q3 = TRAP::Math::SLerp(q1, q2, T(0.5f));
            constexpr T f3 = TRAP::Math::Degrees(TRAP::Math::Angle(q3));
            static_assert(TRAP::Math::Equal(f3, T(45.0f), Epsilon));
        }
        {
            constexpr TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            constexpr TRAP::Math::tQuat<T> q2(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            constexpr TRAP::Math::tQuat<T> q3 = TRAP::Math::SLerp(q2, q1, T(0.5f));
            constexpr T f3 = TRAP::Math::Degrees(TRAP::Math::Angle(q3));
            static_assert(TRAP::Math::Equal(f3, T(45.0f), Epsilon));
        }


        constexpr T sqrt2 = TRAP::Math::Sqrt(T(2.0f)) / T(2.0f);
        constexpr TRAP::Math::tQuat<T> id(static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<float>(0.0f));
        constexpr TRAP::Math::tQuat<T> y90rot(sqrt2, 0.0f, sqrt2, 0.0f);
        [[maybe_unused]] constexpr TRAP::Math::tQuat<T> y180rot(0.0f, 0.0f, 1.0f, 0.0f);

        {
            constexpr TRAP::Math::tQuat<T> id2 = TRAP::Math::SLerp(id, y90rot, T(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(id, id2, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> y90rot2 = TRAP::Math::SLerp(id, y90rot, T(1.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot2, Epsilon)));
        }
        {
            [[maybe_unused]] constexpr TRAP::Math::tQuat<T> y45rot1 = TRAP::Math::SLerp(id, y90rot, T(0.5f));
        }

        constexpr TRAP::Math::tQuat<T> ym45rot2 = TRAP::Math::SLerp(y90rot, id, T(0.5f));

        {
            constexpr TRAP::Math::tQuat<T> y45rot3 = TRAP::Math::SLerp(id, -y90rot, T(0.5f));
            constexpr T y45angle3 = TRAP::Math::Angle(y45rot3);
            static_assert(TRAP::Math::Equal(y45angle3, TRAP::Math::PI<T>() * T(0.25f), Epsilon));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, y45rot3, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> y45rot4 = TRAP::Math::SLerp(-y90rot, id, T(0.5f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, -y45rot4, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> y90rot3 = TRAP::Math::SLerp(y90rot, y90rot, T(0.5f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot3, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> xz90rot = TRAP::Math::SLerp(id, -y90rot, T(0.5f));
            constexpr T xz90angle = TRAP::Math::Angle(xz90rot);
            static_assert(TRAP::Math::Equal(xz90angle, TRAP::Math::PI<T>() * T(0.25f), Epsilon));
        }
        {
            [[maybe_unused]] constexpr TRAP::Math::tQuat<T> almostid = TRAP::Math::SLerp(id, TRAP::Math::AngleAxis(T(0.1f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f)), T(0.5f));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunSLerpRunTimeTests()
    {
        static constexpr T Epsilon = T(0.00001f);

        {
            const TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const TRAP::Math::tQuat<T> q2(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            const TRAP::Math::tQuat<T> q3 = TRAP::Math::SLerp(q1, q2, T(0.5f));
            const T f3 = TRAP::Math::Degrees(TRAP::Math::Angle(q3));
            REQUIRE(TRAP::Math::Equal(f3, T(45.0f), Epsilon));
        }
        {
            const TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const TRAP::Math::tQuat<T> q2(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f));
            const TRAP::Math::tQuat<T> q3 = TRAP::Math::SLerp(q2, q1, T(0.5f));
            const T f3 = TRAP::Math::Degrees(TRAP::Math::Angle(q3));
            REQUIRE(TRAP::Math::Equal(f3, T(45.0f), Epsilon));
        }


        const T sqrt2 = TRAP::Math::Sqrt(T(2.0f)) / T(2.0f);
        static constexpr TRAP::Math::tQuat<T> id(static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<float>(0.0f));
        const TRAP::Math::tQuat<T> y90rot(sqrt2, 0.0f, sqrt2, 0.0f);
        [[maybe_unused]] static constexpr TRAP::Math::tQuat<T> y180rot(0.0f, 0.0f, 1.0f, 0.0f);

        {
            const TRAP::Math::tQuat<T> id2 = TRAP::Math::SLerp(id, y90rot, T(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, id2, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> y90rot2 = TRAP::Math::SLerp(id, y90rot, T(1.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot2, Epsilon)));
        }
        {
            [[maybe_unused]] const TRAP::Math::tQuat<T> y45rot1 = TRAP::Math::SLerp(id, y90rot, T(0.5f));
        }

        const TRAP::Math::tQuat<T> ym45rot2 = TRAP::Math::SLerp(y90rot, id, T(0.5f));

        {
            const TRAP::Math::tQuat<T> y45rot3 = TRAP::Math::SLerp(id, -y90rot, T(0.5f));
            const T y45angle3 = TRAP::Math::Angle(y45rot3);
            REQUIRE(TRAP::Math::Equal(y45angle3, TRAP::Math::PI<T>() * T(0.25f), Epsilon));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, y45rot3, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> y45rot4 = TRAP::Math::SLerp(-y90rot, id, T(0.5f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, -y45rot4, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> y90rot3 = TRAP::Math::SLerp(y90rot, y90rot, T(0.5f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot3, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> xz90rot = TRAP::Math::SLerp(id, -y90rot, T(0.5f));
            const T xz90angle = TRAP::Math::Angle(xz90rot);
            REQUIRE(TRAP::Math::Equal(xz90angle, TRAP::Math::PI<T>() * T(0.25f), Epsilon));
        }
        {
            [[maybe_unused]] const TRAP::Math::tQuat<T> almostid = TRAP::Math::SLerp(id, TRAP::Math::AngleAxis(T(0.1f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0f)), T(0.5f));
        }
        {
            static constexpr TRAP::Math::tQuat<T> a(-1.0f, 0.0f, 0.0f, 0.0f);
            const TRAP::Math::tQuat<T> result = TRAP::Math::SLerp(a, id, T(0.5f));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(TRAP::Math::Dot(id, result), T(2.0f)), T(1.0f), T(0.01f)));
        }
    }

    template<typename T, typename S>
    requires std::floating_point<T> && std::integral<S>
    consteval void RunSLerpSpinCompileTimeTests()
    {
        constexpr T Epsilon = T(0.00001f);

        constexpr T sqrt2 = TRAP::Math::Sqrt(T(2.0f)) / T(2.0f);
        constexpr TRAP::Math::tQuat<T> id(static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f));
        constexpr TRAP::Math::tQuat<T> y90rot(sqrt2, 0.0f, sqrt2, 0.0f);
        constexpr TRAP::Math::tQuat<T> y180rot(0.0f, 0.0f, 1.0f, 0.0f);

        {
            constexpr TRAP::Math::tQuat<T> id2 = TRAP::Math::SLerp(id, id, T(1.0f), S(1));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(id, id2, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> id3 = TRAP::Math::SLerp(id, id, T(1.0f), S(2));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(id, id3, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> y90rot2 = TRAP::Math::SLerp(id, y90rot, T(1.0f), S(1));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, -y90rot2, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> y90rot3 = TRAP::Math::SLerp(id, y90rot, T(8.0f) / T(9.0f), S(2));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(id, y90rot3, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> y90rot4 = TRAP::Math::SLerp(id, y90rot, T(0.2f), S(1));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot4, Epsilon)));
        }

        constexpr TRAP::Math::tQuat<T> ym45rot3 = TRAP::Math::SLerp(y90rot, id, T(0.5f));
        constexpr TRAP::Math::tQuat<T> ym45rot2 = TRAP::Math::SLerp(y90rot, id, T(0.9f), S(1));

        {
            static_assert(TRAP::Math::All(TRAP::Math::Equal(-ym45rot2, ym45rot3, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> y45rot3 = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(0));
            constexpr T y45angle3 = TRAP::Math::Angle(y45rot3);
            static_assert(TRAP::Math::Equal(y45angle3, TRAP::Math::PI<T>() * T(0.25f), Epsilon));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(ym45rot3, y45rot3, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> y45rot4 = TRAP::Math::SLerp(-y90rot, id, T(0.5f), S(0));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, y45rot4, Epsilon)));
        }
        if constexpr(std::same_as<T, float>)
        {
            constexpr TRAP::Math::tQuat<T> y90rot5 = TRAP::Math::SLerp(y90rot, y90rot, T(0.5f), S(2));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot5, Epsilon)));
        }
        {
            constexpr TRAP::Math::tQuat<T> xz90rot = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(1));
            constexpr T xz90angle = TRAP::Math::Angle(xz90rot);
            static_assert(TRAP::Math::Equal(xz90angle, TRAP::Math::PI<T>() * T(1.25f), Epsilon));
        }

        if constexpr(std::signed_integral<S>)
        {
            constexpr TRAP::Math::tQuat<T> neg90rot = TRAP::Math::SLerp(id, y90rot, T(2.0f) / T(3.0f), S(-1));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(y180rot, -neg90rot, Epsilon)));
        }
    }

    template<typename T, typename S>
    requires std::floating_point<T> && std::integral<S>
    void RunSLerpSpinRunTimeTests()
    {
        static constexpr T Epsilon = T(0.00001f);

        const T sqrt2 = TRAP::Math::Sqrt(T(2.0f)) / T(2.0f);
        static constexpr TRAP::Math::tQuat<T> id(static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f));
        const TRAP::Math::tQuat<T> y90rot(sqrt2, 0.0f, sqrt2, 0.0f);
        const TRAP::Math::tQuat<T> y180rot(0.0f, 0.0f, 1.0f, 0.0f);

        {
            const TRAP::Math::tQuat<T> id2 = TRAP::Math::SLerp(id, id, T(1.0f), S(1));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, id2, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> id3 = TRAP::Math::SLerp(id, id, T(1.0f), S(2));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, id3, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> y90rot2 = TRAP::Math::SLerp(id, y90rot, T(1.0f), S(1));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, -y90rot2, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> y90rot3 = TRAP::Math::SLerp(id, y90rot, T(8.0f) / T(9.0f), S(2));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, y90rot3, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> y90rot4 = TRAP::Math::SLerp(id, y90rot, T(0.2f), S(1));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot4, Epsilon)));
        }

        const TRAP::Math::tQuat<T> ym45rot3 = TRAP::Math::SLerp(y90rot, id, T(0.5f));
        const TRAP::Math::tQuat<T> ym45rot2 = TRAP::Math::SLerp(y90rot, id, T(0.9f), S(1));

        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(-ym45rot2, ym45rot3, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> y45rot3 = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(0));
            const T y45angle3 = TRAP::Math::Angle(y45rot3);
            REQUIRE(TRAP::Math::Equal(y45angle3, TRAP::Math::PI<T>() * T(0.25f), Epsilon));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ym45rot3, y45rot3, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> y45rot4 = TRAP::Math::SLerp(-y90rot, id, T(0.5f), S(0));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, y45rot4, Epsilon)));
        }
        if constexpr(std::same_as<T, float>)
        {
            const TRAP::Math::tQuat<T> y90rot5 = TRAP::Math::SLerp(y90rot, y90rot, T(0.5f), S(2));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot5, Epsilon)));
        }
        {
            const TRAP::Math::tQuat<T> xz90rot = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(1));
            const T xz90angle = TRAP::Math::Angle(xz90rot);
            REQUIRE(TRAP::Math::Equal(xz90angle, TRAP::Math::PI<T>() * T(1.25f), Epsilon));
        }

        if constexpr(std::signed_integral<S>)
        {
            const TRAP::Math::tQuat<T> neg90rot = TRAP::Math::SLerp(id, y90rot, T(2.0f) / T(3.0f), S(-1));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y180rot, -neg90rot, Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunSLerpEdgeTests()
    {
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();

        {
            const TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const TRAP::Math::tQuat<T> result = TRAP::Math::SLerp(q1, q1, min);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, TRAP::Math::tQuat<T>(0.0f, 0.0f, 0.0f, 0.0f))));
        }
        {
            const TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const TRAP::Math::tQuat<T> result = TRAP::Math::SLerp(q1, q1, max);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, TRAP::Math::tQuat<T>(0.0f, 0.0f, 0.0f, 0.0f))));
        }
        {
            const TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const TRAP::Math::tQuat<T> result = TRAP::Math::SLerp(q1, q1, inf);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(result)));
        }
        {
            const TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const TRAP::Math::tQuat<T> result = TRAP::Math::SLerp(q1, q1, ninf);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(result)));
        }
        {
            const TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const TRAP::Math::tQuat<T> result = TRAP::Math::SLerp(q1, q1, nan);
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(result)));
        }
    }

    template<typename T, typename S>
    requires std::floating_point<T> && std::integral<S>
    void RunSLerpSpinEdgeTests()
    {
        static constexpr S min = std::numeric_limits<S>::min();
        static constexpr S max = std::numeric_limits<S>::max();

        {
            const TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const TRAP::Math::tQuat<T> result = TRAP::Math::SLerp(q1, q1, T(1.0f), min);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, TRAP::Math::tQuat<T>(1.0f, 0.0f, 0.0f, 0.0f))));
        }
        {
            const TRAP::Math::tQuat<T> q1(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f));
            const TRAP::Math::tQuat<T> result = TRAP::Math::SLerp(q1, q1, T(1.0f), max);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, TRAP::Math::tQuat<T>(1.0f, 0.0f, 0.0f, 0.0f))));
        }
    }
}

TEST_CASE("TRAP::Math::SLerp()", "[math][generic][slerp]")
{
    SECTION("Scalar - double, int8_t")
    {
        RunSLerpRunTimeTests<double>();
        RunSLerpCompileTimeTests<double>();
        RunSLerpSpinRunTimeTests<double, int8_t>();
        RunSLerpSpinCompileTimeTests<double, int8_t>();
        RunSLerpEdgeTests<double>();
        RunSLerpSpinEdgeTests<double, int8_t>();
    }
    SECTION("Scalar - double, uint8_t")
    {
        RunSLerpRunTimeTests<double>();
        RunSLerpCompileTimeTests<double>();
        RunSLerpSpinRunTimeTests<double, uint8_t>();
        RunSLerpSpinCompileTimeTests<double, uint8_t>();
        RunSLerpEdgeTests<double>();
        RunSLerpSpinEdgeTests<double, uint8_t>();
    }
    SECTION("Scalar - double, int16_t")
    {
        RunSLerpRunTimeTests<double>();
        RunSLerpCompileTimeTests<double>();
        RunSLerpSpinRunTimeTests<double, int16_t>();
        RunSLerpSpinCompileTimeTests<double, int16_t>();
        RunSLerpEdgeTests<double>();
        RunSLerpSpinEdgeTests<double, int16_t>();
    }
    SECTION("Scalar - double, uint16_t")
    {
        RunSLerpRunTimeTests<double>();
        RunSLerpCompileTimeTests<double>();
        RunSLerpSpinRunTimeTests<double, uint16_t>();
        RunSLerpSpinCompileTimeTests<double, uint16_t>();
        RunSLerpEdgeTests<double>();
        RunSLerpSpinEdgeTests<double, uint16_t>();
    }
    SECTION("Scalar - double, int32_t")
    {
        RunSLerpRunTimeTests<double>();
        RunSLerpCompileTimeTests<double>();
        RunSLerpSpinRunTimeTests<double, int32_t>();
        RunSLerpSpinCompileTimeTests<double, int32_t>();
        RunSLerpEdgeTests<double>();
        RunSLerpSpinEdgeTests<double, int32_t>();
    }
    SECTION("Scalar - double, uint32_t")
    {
        RunSLerpRunTimeTests<double>();
        RunSLerpCompileTimeTests<double>();
        RunSLerpSpinRunTimeTests<double, uint32_t>();
        RunSLerpSpinCompileTimeTests<double, uint32_t>();
        RunSLerpEdgeTests<double>();
        RunSLerpSpinEdgeTests<double, uint32_t>();
    }
    SECTION("Scalar - double, int64_t")
    {
        RunSLerpRunTimeTests<double>();
        RunSLerpCompileTimeTests<double>();
        RunSLerpSpinRunTimeTests<double, int64_t>();
        RunSLerpSpinCompileTimeTests<double, int64_t>();
        RunSLerpEdgeTests<double>();
        RunSLerpSpinEdgeTests<double, int64_t>();
    }
    SECTION("Scalar - double, uint64_t")
    {
        RunSLerpRunTimeTests<double>();
        RunSLerpCompileTimeTests<double>();
        RunSLerpSpinRunTimeTests<double, uint64_t>();
        RunSLerpSpinCompileTimeTests<double, uint64_t>();
        RunSLerpEdgeTests<double>();
        RunSLerpSpinEdgeTests<double, uint64_t>();
    }

    SECTION("Scalar - float, int8_t")
    {
        RunSLerpRunTimeTests<float>();
        RunSLerpCompileTimeTests<float>();
        RunSLerpSpinRunTimeTests<float, int8_t>();
        RunSLerpSpinCompileTimeTests<float, int8_t>();
        RunSLerpEdgeTests<float>();
        RunSLerpSpinEdgeTests<float, int8_t>();
    }
    SECTION("Scalar - float, uint8_t")
    {
        RunSLerpRunTimeTests<float>();
        RunSLerpCompileTimeTests<float>();
        RunSLerpSpinRunTimeTests<float, uint8_t>();
        RunSLerpSpinCompileTimeTests<float, uint8_t>();
        RunSLerpEdgeTests<float>();
        RunSLerpSpinEdgeTests<float, uint8_t>();
    }
    SECTION("Scalar - float, int16_t")
    {
        RunSLerpRunTimeTests<float>();
        RunSLerpCompileTimeTests<float>();
        RunSLerpSpinRunTimeTests<float, int16_t>();
        RunSLerpSpinCompileTimeTests<float, int16_t>();
        RunSLerpEdgeTests<float>();
        RunSLerpSpinEdgeTests<float, int16_t>();
    }
    SECTION("Scalar - float, uint16_t")
    {
        RunSLerpRunTimeTests<float>();
        RunSLerpCompileTimeTests<float>();
        RunSLerpSpinRunTimeTests<float, uint16_t>();
        RunSLerpSpinCompileTimeTests<float, uint16_t>();
        RunSLerpEdgeTests<float>();
        RunSLerpSpinEdgeTests<float, uint16_t>();
    }
    SECTION("Scalar - float, int32_t")
    {
        RunSLerpRunTimeTests<float>();
        RunSLerpCompileTimeTests<float>();
        RunSLerpSpinRunTimeTests<float, int32_t>();
        RunSLerpSpinCompileTimeTests<float, int32_t>();
        RunSLerpEdgeTests<float>();
        RunSLerpSpinEdgeTests<float, int32_t>();
    }
    SECTION("Scalar - float, uint32_t")
    {
        RunSLerpRunTimeTests<float>();
        RunSLerpCompileTimeTests<float>();
        RunSLerpSpinRunTimeTests<float, uint32_t>();
        RunSLerpSpinCompileTimeTests<float, uint32_t>();
        RunSLerpEdgeTests<float>();
        RunSLerpSpinEdgeTests<float, uint32_t>();
    }
    SECTION("Scalar - float, int64_t")
    {
        RunSLerpRunTimeTests<float>();
        RunSLerpCompileTimeTests<float>();
        RunSLerpSpinRunTimeTests<float, int64_t>();
        RunSLerpSpinCompileTimeTests<float, int64_t>();
        RunSLerpEdgeTests<float>();
        RunSLerpSpinEdgeTests<float, int64_t>();
    }
    SECTION("Scalar - float, uint64_t")
    {
        RunSLerpRunTimeTests<float>();
        RunSLerpCompileTimeTests<float>();
        RunSLerpSpinRunTimeTests<float, uint64_t>();
        RunSLerpSpinCompileTimeTests<float, uint64_t>();
        RunSLerpEdgeTests<float>();
        RunSLerpSpinEdgeTests<float, uint64_t>();
    }
}
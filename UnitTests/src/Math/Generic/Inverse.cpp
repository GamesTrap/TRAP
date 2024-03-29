#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T> || TRAP::Math::IsQuat<T>) && std::floating_point<typename T::value_type>)
    consteval void RunCompileTimeInverseTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        if constexpr(TRAP::Math::IsMat<T>)
        {
            {
                constexpr T a(1.0f);
                constexpr T b = TRAP::Math::Inverse(a);
                constexpr T i = a * b;
                static_assert(TRAP::Math::All(TRAP::Math::Equal(i, a, Epsilon)));
            }
            {
                constexpr T a(TRAP::Math::tMat4<typename T::value_type>(0.6f, 0.2f, 0.3f, 0.4f, 0.2f, 0.7f, 0.5f, 0.3f, 0.3f, 0.5f, 0.7f, 0.2f, 0.4f, 0.3f, 0.2f, 0.6f));
                constexpr T b = TRAP::Math::Inverse(a);
                constexpr T i = a * b;
                static_assert(TRAP::Math::All(TRAP::Math::Equal(T(1.0f), i, typename T::value_type(0.000001f))));
            }
        }
        else if constexpr(TRAP::Math::IsQuat<T>)
        {
            {
                constexpr T q1(1.0f, 0.0f, 0.0f, 0.0f);
                constexpr T res = TRAP::Math::Inverse(q1);
                static_assert(TRAP::Math::All(TRAP::Math::Equal(res, T(1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
            }
            {
                constexpr T q1(0.5f, 0.5f, 0.5f, 0.5f);
                constexpr T res = TRAP::Math::Inverse(q1);
                static_assert(TRAP::Math::All(TRAP::Math::Equal(res, T(0.5f, -0.5f, -0.5f, -0.5f), Epsilon)));
            }
            {
                constexpr T q1(2.0f, 1.0f, 0.5f, 0.3f);
                constexpr T res = TRAP::Math::Inverse(q1);
                static_assert(TRAP::Math::All(TRAP::Math::Equal(res, T(0.374532f, -0.187266f, -0.093633f, -0.056180f), typename T::value_type(0.000001f))));
            }
            {
                constexpr T q1(-0.5f, 0.2f, -0.7f, -0.9f);
                constexpr T res = TRAP::Math::Inverse(q1);
                static_assert(TRAP::Math::All(TRAP::Math::Equal(res, T(-0.314465f, -0.125786f, 0.440252f, 0.566038f), typename T::value_type(0.000001f))));
            }
        }
    }

    template<typename T>
    requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T> || TRAP::Math::IsQuat<T>) && std::floating_point<typename T::value_type>)
    void RunInverseEdgeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::lowest();
        static constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        if constexpr(TRAP::Math::IsMat<T>)
        {
            {
                static constexpr T a(0.0f);
                static constexpr T b(TRAP::Math::tMat4<typename T::value_type>{nan});
                REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
            }
            {
                static constexpr T a(TRAP::Math::tMat4<typename T::value_type>{min});
                static constexpr T b(TRAP::Math::tMat4<typename T::value_type>{nan});
                REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
            }
            {
                static constexpr T a(TRAP::Math::tMat4<typename T::value_type>{max});
                static constexpr T b(TRAP::Math::tMat4<typename T::value_type>{nan});
                REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
            }
            {
                static constexpr T a(TRAP::Math::tMat4<typename T::value_type>{inf});
                static constexpr T b(TRAP::Math::tMat4<typename T::value_type>{nan});
                REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
            }
            {
                static constexpr T a(TRAP::Math::tMat4<typename T::value_type>{ninf});
                static constexpr T b(TRAP::Math::tMat4<typename T::value_type>{nan});
                REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), b))));
            }
            {
                static constexpr T a(TRAP::Math::tMat4<typename T::value_type>{nan});
                REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(TRAP::Math::Inverse(a), a))));
            }
        }
        else if constexpr(TRAP::Math::IsQuat<T>)
        {
            {
                static constexpr T q1(min, min, min, min);
                const T res = TRAP::Math::Inverse(q1);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, T(0.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
            }
            {
                static constexpr T q1(max, max, max, max);
                const T res = TRAP::Math::Inverse(q1);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, T(0.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
            }
            {
                static constexpr T q1(inf, inf, inf, inf);
                const T res = TRAP::Math::Inverse(q1);
                REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
            }
            {
                static constexpr T q1(ninf, ninf, ninf, ninf);
                const T res = TRAP::Math::Inverse(q1);
                REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
            }
            {
                static constexpr T q1(nan, nan, nan, nan);
                const T res = TRAP::Math::Inverse(q1);
                REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(res)));
            }
        }
    }
}

TEST_CASE("TRAP::Math::Inverse()", "[math][generic][inverse]")
{
    SECTION("Mat3 - f64")
    {
        RunCompileTimeInverseTests<TRAP::Math::Mat3d>();
        RunInverseEdgeTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - f32")
    {
        RunCompileTimeInverseTests<TRAP::Math::Mat3f>();
        RunInverseEdgeTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - f64")
    {
        RunCompileTimeInverseTests<TRAP::Math::Mat4d>();
        RunInverseEdgeTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - f32")
    {
        RunCompileTimeInverseTests<TRAP::Math::Mat4f>();
        RunInverseEdgeTests<TRAP::Math::Mat4f>();
    }

    SECTION("Quat - f64")
    {
        RunCompileTimeInverseTests<TRAP::Math::Quatd>();
        RunInverseEdgeTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - f32")
    {
        RunCompileTimeInverseTests<TRAP::Math::Quatf>();
        RunInverseEdgeTests<TRAP::Math::Quatf>();
    }
}
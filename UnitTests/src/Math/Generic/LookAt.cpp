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
    consteval void RunLookAtCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            constexpr TRAP::Math::tVec3<T> eye(0.0f);
            constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
            constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            constexpr TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            constexpr TRAP::Math::tVec3<T> eye(2.0f);
            constexpr TRAP::Math::tVec3<T> center(0.0f);
            constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            constexpr TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            constexpr TRAP::Math::tMat4<T> expected(0.707107f, -0.408248f, 0.577350f, 0.0f, 0.0f, 0.816497f, 0.577350f, 0.0f, -0.707107f, -0.408248f, 0.577350f, 0.0f, 0.0f, 0.0f, -3.464102f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, T(0.000001f))));
        }
        {
            constexpr TRAP::Math::tVec3<T> eye(-1.0f, 1.0f, 1.0f);
            constexpr TRAP::Math::tVec3<T> center(1.0f, -1.0f, -1.0f);
            constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            constexpr TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            constexpr TRAP::Math::tMat4<T> expected(0.707107f, 0.408248f, -0.577350f, 0.0f, 0.0f, 0.816497f, 0.577350f, 0.0f, 0.707107f, -0.408248f, 0.577350, 0.0f, 0.0f, 0.0f, -1.732051f, 1.0f);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(res, expected, T(0.000001f))));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunLookAtRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        {
            static constexpr TRAP::Math::tVec3<T> eye(0.0f);
            static constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
            static constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            static constexpr TRAP::Math::tMat4<T> expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, Epsilon)));
        }
        {
            static constexpr TRAP::Math::tVec3<T> eye(2.0f);
            static constexpr TRAP::Math::tVec3<T> center(0.0f);
            static constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            static constexpr TRAP::Math::tMat4<T> expected(0.707107f, -0.408248f, 0.577350f, 0.0f, 0.0f, 0.816497f, 0.577350f, 0.0f, -0.707107f, -0.408248f, 0.577350f, 0.0f, 0.0f, 0.0f, -3.464102f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, T(0.000001f))));
        }
        {
            static constexpr TRAP::Math::tVec3<T> eye(-1.0f, 1.0f, 1.0f);
            static constexpr TRAP::Math::tVec3<T> center(1.0f, -1.0f, -1.0f);
            static constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            static constexpr TRAP::Math::tMat4<T> expected(0.707107f, 0.408248f, -0.577350f, 0.0f, 0.0f, 0.816497f, 0.577350f, 0.0f, 0.707107f, -0.408248f, 0.577350, 0.0f, 0.0f, 0.0f, -1.732051f, 1.0f);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, expected, T(0.000001f))));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunLookAtEdgeTests()
    {
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        {
            static constexpr TRAP::Math::tVec3<T> eye(min, 0.0f, 0.0f);
            static constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
            static constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tVec3<T> eye(max, 0.0f, 0.0f);
            static constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
            static constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tVec3<T> eye(inf, 0.0f, 0.0f);
            static constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
            static constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tVec3<T> eye(ninf, 0.0f, 0.0f);
            static constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
            static constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
        {
            static constexpr TRAP::Math::tVec3<T> eye(nan, 0.0f, 0.0f);
            static constexpr TRAP::Math::tVec3<T> center(0.0f, 0.0f, -1.0f);
            static constexpr TRAP::Math::tVec3<T> up(0.0f, 1.0f, 0.0f);

            const TRAP::Math::tMat4<T> res = TRAP::Math::LookAt(eye, center, up);
            REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(std::get<0>(res))));
        }
    }
}

TEST_CASE("TRAP::Math::LookAt()", "[math][generic][lookat]")
{
    SECTION("Scalar - double")
    {
        RunLookAtRunTimeTests<double>();
        RunLookAtCompileTimeTests<double>();
        RunLookAtEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunLookAtRunTimeTests<float>();
        RunLookAtCompileTimeTests<float>();
        RunLookAtEdgeTests<float>();
    }
}

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
    consteval void RunLogCompileTimeTests()
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Log(T(0.5f)), T(-0.69314718055994529f), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log(T(1.0f)), T(0.0f), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log(T(1.5f)), T(0.405465f), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Log(T(41.5f)), T(3.72569346f), T(0.000001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunLogRunTimeTests()
    {
        static constexpr std::array<std::tuple<T, T>, 4> values
        {
            std::tuple(T(0.5f), T(-0.69314718055994529f)),
            std::tuple(T(1.0f), T(0.0f)),
            std::tuple(T(1.5f), T(0.405465f)),
            std::tuple(T(41.5f), T(3.72569346f))
        };

        for(const auto& [x, expected] : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Log(x), expected, T(0.000001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunLogVecCompileTimeTests()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log(T(TRAP::Math::e<typename T::value_type>())), T(1.0f), T(0.01f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunLogVecRunTimeTests()
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Log(T(TRAP::Math::e<typename T::value_type>())), T(1.0f), T(0.01f))));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    consteval void RunLogQuatCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = typename T::value_type(0.001f);

        constexpr T q(TRAP::Math::Vec<3, typename T::value_type>(1.0f, 0.0f, 0.0f), TRAP::Math::Vec<3, typename T::value_type>(0.0f, 1.0f, 0.0f));
        constexpr T p = TRAP::Math::Log(q);
        static_assert(TRAP::Math::Any(TRAP::Math::NotEqual(q, p, Epsilon)));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    void RunLogQuatRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = typename T::value_type(0.001f);

        const T q(TRAP::Math::Vec<3, typename T::value_type>(1.0f, 0.0f, 0.0f), TRAP::Math::Vec<3, typename T::value_type>(0.0f, 1.0f, 0.0f));
        const T p = TRAP::Math::Log(q);
        REQUIRE(TRAP::Math::Any(TRAP::Math::NotEqual(q, p, Epsilon)));
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunLogEdgeCompileTimeTests()
    {
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();
        constexpr T min = std::numeric_limits<T>::lowest();

        static_assert(TRAP::Math::IsInf(TRAP::Math::Log(T(0.0f))));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Log(min)));
        static_assert(TRAP::Math::IsInf(TRAP::Math::Log(inf)));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Log(ninf)));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Log(nan)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunLogEdgeRunTimeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T min = std::numeric_limits<T>::lowest();

        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Log(T(0.0f))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log(min)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Log(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log(ninf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Log(nan)));
    }
}

TEST_CASE("TRAP::Math::Log()", "[math][generic][log]")
{
    SECTION("Scalar - double")
    {
        RunLogRunTimeTests<double>();
        RunLogCompileTimeTests<double>();
        RunLogEdgeRunTimeTests<double>();
        RunLogEdgeCompileTimeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunLogRunTimeTests<float>();
        RunLogCompileTimeTests<float>();
        RunLogEdgeRunTimeTests<float>();
        RunLogEdgeCompileTimeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunLogVecRunTimeTests<TRAP::Math::Vec2d>();
        RunLogVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunLogVecRunTimeTests<TRAP::Math::Vec2f>();
        RunLogVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunLogVecRunTimeTests<TRAP::Math::Vec3d>();
        RunLogVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunLogVecRunTimeTests<TRAP::Math::Vec3f>();
        RunLogVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunLogVecRunTimeTests<TRAP::Math::Vec4d>();
        RunLogVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunLogVecRunTimeTests<TRAP::Math::Vec4f>();
        RunLogVecCompileTimeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunLogQuatRunTimeTests<TRAP::Math::Quatd>();
        RunLogQuatCompileTimeTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunLogQuatRunTimeTests<TRAP::Math::Quatf>();
        RunLogQuatCompileTimeTests<TRAP::Math::Quatf>();
    }
}
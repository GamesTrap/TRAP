#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    void RunPowCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(T(0.5f), T(2.0f)), T(0.25f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(T(1.5f), T(1.0f)), T(1.5f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(T(2.0f), T(1.0f)), T(2.0f), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(T(2.0f), T(2.0f)), T(4.0f), T(0.00000000000001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunPowTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<std::tuple<T, T, T>, 4> values
        {
            std::tuple(T(0.5f), T(2.0f), T(0.25f)),
            std::tuple(T(1.5f), T(1.0f), T(1.5f)),
            std::tuple(T(2.0f), T(1.0f), T(2.0f)),
            std::tuple(T(2.0f), T(2.0f), T(4.0f)),
        };

        for(const auto& [base, exponent, expected] : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(base, exponent), expected, Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunPowVecCompileTimeTests()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Pow(T(2.0f), T(2.0f)), T(4.0f), T(0.00000000000001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunPowVecRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Pow(T(2.0f), T(2.0f)), T(4.0f), Epsilon)));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    consteval void RunPowQuatCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = typename T::value_type(0.001f);

        constexpr T q(TRAP::Math::Vec<3, typename T::value_type>(1.0f, 0.0f, 0.0f), TRAP::Math::Vec<3, typename T::value_type>(0.0f, 1.0f, 0.0f));

        {
            constexpr typename T::value_type one = typename T::value_type(1.0f);
            constexpr T p = TRAP::Math::Pow(q, one);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(q, p, Epsilon)));
        }
        {
            constexpr typename T::value_type two = typename T::value_type(2.0f);
            constexpr T p = TRAP::Math::Pow(q, two);
            constexpr T r = q * q;
            static_assert(TRAP::Math::All(TRAP::Math::Equal(p, r, Epsilon)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    void RunPowQuatRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = typename T::value_type(0.001f);

        const T q(TRAP::Math::Vec<3, typename T::value_type>(1.0f, 0.0f, 0.0f), TRAP::Math::Vec<3, typename T::value_type>(0.0f, 1.0f, 0.0f));

        {
            const typename T::value_type one = typename T::value_type(1.0f);
            const T p = TRAP::Math::Pow(q, one);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(q, p, Epsilon)));
        }
        {
            const typename T::value_type two = typename T::value_type(2.0f);
            const T p = TRAP::Math::Pow(q, two);
            const T r = q * q;
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(p, r, Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunPowEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Pow(nan, T(2.0f))));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Pow(T(2.0f), nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Pow(nan, nan)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Pow(inf, T(2.0f))));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Pow(-inf, T(2.0f))));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(inf, T(-2.0f)), T(0.0f), Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(inf, T(0.0f)), T(1.0f), Epsilon));
    }
}

TEST_CASE("TRAP::Math::Pow()", "[math][generic][pow]")
{
    SECTION("Scalar - f64")
    {
        RunPowTests<f64>();
        RunPowCompileTimeTests<f64>();
        RunPowEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunPowTests<f32>();
        RunPowCompileTimeTests<f32>();
        RunPowEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunPowVecRunTimeTests<TRAP::Math::Vec2d>();
        RunPowVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunPowVecRunTimeTests<TRAP::Math::Vec2f>();
        RunPowVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunPowVecRunTimeTests<TRAP::Math::Vec3d>();
        RunPowVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunPowVecRunTimeTests<TRAP::Math::Vec3f>();
        RunPowVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunPowVecRunTimeTests<TRAP::Math::Vec4d>();
        RunPowVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunPowVecRunTimeTests<TRAP::Math::Vec4f>();
        RunPowVecCompileTimeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - f64")
    {
        RunPowQuatRunTimeTests<TRAP::Math::Quatd>();
        RunPowQuatCompileTimeTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - f32")
    {
        RunPowQuatRunTimeTests<TRAP::Math::Quatf>();
        RunPowQuatCompileTimeTests<TRAP::Math::Quatf>();
    }
}
#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunCosCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(T(-1.5)), T(0.0707372), T(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(T(0.0)), T(1.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(T(0.001)), T(1.0), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(T(1.001)), T(0.539461), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(T(1.5)), T(0.0707372), T(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(T(11.1)), T(0.104236), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(T(50.0)), T(0.964966), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Cos(T(150.0)), T(0.699251), T(0.00001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunCosRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 8> values
        {
            T(-1.5), T(0.0), T(0.001), T(1.001), T(1.5), T(11.1), T(50.0), T(150.0)
        };

        for(const T val : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Cos(val), std::cos(val), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunCosEdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Cos(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Cos(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Cos(ninf)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunCosVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(T(typename T::value_type(-1.5f))), T(0.0707372f), T(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(T(typename T::value_type(0.0f))), T(1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(T(typename T::value_type(0.001f))), T(1.0f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(T(typename T::value_type(1.001f))), T(0.539461f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(T(typename T::value_type(1.5f))), T(0.0707372f), T(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(T(typename T::value_type(11.1f))), T(0.104236f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(T(typename T::value_type(50.0f))), T(0.964966f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Cos(T(typename T::value_type(150.0f))), T(0.699251f), T(0.00001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunCosVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<T, 8> values
        {
            T(typename T::value_type(-1.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)), T(typename T::value_type(1.001)),
            T(typename T::value_type(1.5)), T(typename T::value_type(11.1)), T(typename T::value_type(50.0)), T(typename T::value_type(150.0))
        };

        for(const T val : values)
        {
            const T s = TRAP::Math::Cos(val);
            for(uint32_t l = 0; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::cos(val[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunCosVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Cos(T(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Cos(T(inf)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Cos(T(ninf)))));
    }
}

TEST_CASE("TRAP::Math::Cos()", "[math][generic][cos]")
{
    SECTION("Scalar - double")
    {
        RunCosRunTimeTests<double>();
        RunCosCompileTimeTests<double>();
        RunCosEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCosRunTimeTests<float>();
        RunCosCompileTimeTests<float>();
        RunCosEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunCosVecRunTimeTests<TRAP::Math::Vec2d>();
        RunCosVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunCosVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCosVecRunTimeTests<TRAP::Math::Vec2f>();
        RunCosVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunCosVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunCosVecRunTimeTests<TRAP::Math::Vec3d>();
        RunCosVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunCosVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCosVecRunTimeTests<TRAP::Math::Vec3f>();
        RunCosVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunCosVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunCosVecRunTimeTests<TRAP::Math::Vec4d>();
        RunCosVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunCosVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCosVecRunTimeTests<TRAP::Math::Vec4f>();
        RunCosVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunCosVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

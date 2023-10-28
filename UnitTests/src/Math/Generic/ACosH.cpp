#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunACosHCompileTimeTests()
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::ACosH(T(1.001)), T(0.0447176), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ACosH(T(1.5)), T(0.962424), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ACosH(T(11.1)), T(3.09806), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ACosH(T(50.0)), T(4.60507), T(0.000001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunACosHRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 4> values
        {
            T(1.001), T(1.5), T(11.1), T(50.0)
        };

        for(const T val : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ACosH(val), std::acosh(val), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunACosHEdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACosH(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ACosH(T(0.5))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunACosHVecCompileTimeTests()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACosH(T(1.001f)), T(0.0447176f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACosH(T(1.5f)), T(0.962424f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACosH(T(11.1f)), T(3.09806f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ACosH(T(50.0f)), T(4.60507f), T(0.000001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunACosHVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<T, 4> values
        {
            T(typename T::value_type(1.001)), T(typename T::value_type(1.5)),
            T(typename T::value_type(11.1)), T(typename T::value_type(50.0))
        };

        for(const T val : values)
        {
            const T s = TRAP::Math::ACosH(val);
            for(u32 l = 0; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::acosh(val[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunACosHVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACosH(T(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ACosH(T(typename T::value_type(0.5))))));
    }
}

TEST_CASE("TRAP::Math::ACosH()", "[math][generic][acosh]")
{
    SECTION("Scalar - f64")
    {
        RunACosHRunTimeTests<f64>();
        RunACosHCompileTimeTests<f64>();
        RunACosHEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunACosHRunTimeTests<f32>();
        RunACosHCompileTimeTests<f32>();
        RunACosHEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunACosHVecRunTimeTests<TRAP::Math::Vec2d>();
        RunACosHVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunACosHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunACosHVecRunTimeTests<TRAP::Math::Vec2f>();
        RunACosHVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunACosHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunACosHVecRunTimeTests<TRAP::Math::Vec3d>();
        RunACosHVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunACosHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunACosHVecRunTimeTests<TRAP::Math::Vec3f>();
        RunACosHVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunACosHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunACosHVecRunTimeTests<TRAP::Math::Vec4d>();
        RunACosHVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunACosHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunACosHVecRunTimeTests<TRAP::Math::Vec4f>();
        RunACosHVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunACosHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

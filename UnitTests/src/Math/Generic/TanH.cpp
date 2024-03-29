#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunTanHCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::TanH(T(-0.5)), T(-0.462117), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::TanH(T(0.0)), T(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::TanH(T(0.001)), T(0.001), T(0.000000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::TanH(T(0.5)), T(0.462117), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::TanH(T(1.0)), T(0.761594), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::TanH(T(5.0)), T(0.999909), T(0.000001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunTanHRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 6> values
        {
            T(-0.5), T(0.0), T(0.001), T(0.5), T(1.0), T(5.0)
        };

        for(const T val : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::TanH(val), std::tanh(val), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunTanHEdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::TanH(nan)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunTanHVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::TanH(T(-0.5f)), T(-0.462117f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::TanH(T(0.0f)), T(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::TanH(T(0.001f)), T(0.001f), T(0.000000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::TanH(T(0.5f)), T(0.462117f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::TanH(T(1.0f)), T(0.761594f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::TanH(T(5.0f)), T(0.999909f), T(0.000001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunTanHVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<T, 6> values
        {
            T(typename T::value_type(-0.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)),
            T(typename T::value_type(0.5)), T(typename T::value_type(1.0)), T(typename T::value_type(5.0))
        };

        for(const T val : values)
        {
            const T s = TRAP::Math::TanH(val);
            for(u32 l = 0; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::tanh(val[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunTanHVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::TanH(T(nan)))));
    }
}

TEST_CASE("TRAP::Math::TanH()", "[math][generic][tanh]")
{
    SECTION("Scalar - f64")
    {
        RunTanHRunTimeTests<f64>();
        RunTanHCompileTimeTests<f64>();
        RunTanHEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunTanHRunTimeTests<f32>();
        RunTanHCompileTimeTests<f32>();
        RunTanHEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunTanHVecRunTimeTests<TRAP::Math::Vec2d>();
        RunTanHVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunTanHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunTanHVecRunTimeTests<TRAP::Math::Vec2f>();
        RunTanHVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunTanHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunTanHVecRunTimeTests<TRAP::Math::Vec3d>();
        RunTanHVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunTanHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunTanHVecRunTimeTests<TRAP::Math::Vec3f>();
        RunTanHVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunTanHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunTanHVecRunTimeTests<TRAP::Math::Vec4d>();
        RunTanHVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunTanHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunTanHVecRunTimeTests<TRAP::Math::Vec4f>();
        RunTanHVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunTanHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

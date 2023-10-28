#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunATanHCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::ATanH(T(-0.99)), T(-2.64665), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATanH(T(0.0)), T(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATanH(T(0.001)), T(0.001), T(0.000000001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunATanHRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 3> values
        {
            T(-0.99), T(0.0), T(0.001)
        };

        for(const T val : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ATanH(val), std::atanh(val), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunATanHEdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATanH(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATanH(T(1.1))));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::ATanH(T(-1.0))));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::ATanH(T(1.0))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunATanHVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATanH(T(-0.99f)), T(-2.64665f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATanH(T(0.0f)), T(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATanH(T(0.001f)), T(0.001f), T(0.000000001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunATanHVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<T, 3> values
        {
            T(typename T::value_type(-0.99)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001))
        };

        for(const T val : values)
        {
            const T s = TRAP::Math::ATanH(val);
            for(u32 l = 0; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::atanh(val[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunATanHVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATanH(T(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATanH(T(typename T::value_type(1.1))))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::ATanH(T(typename T::value_type(-1.0))))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::ATanH(T(typename T::value_type(1.0))))));
    }
}

TEST_CASE("TRAP::Math::ATanH()", "[math][generic][atanh]")
{
    SECTION("Scalar - f64")
    {
        RunATanHRunTimeTests<f64>();
        RunATanHCompileTimeTests<f64>();
        RunATanHEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunATanHRunTimeTests<f32>();
        RunATanHCompileTimeTests<f32>();
        RunATanHEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunATanHVecRunTimeTests<TRAP::Math::Vec2d>();
        RunATanHVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunATanHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunATanHVecRunTimeTests<TRAP::Math::Vec2f>();
        RunATanHVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunATanHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunATanHVecRunTimeTests<TRAP::Math::Vec3d>();
        RunATanHVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunATanHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunATanHVecRunTimeTests<TRAP::Math::Vec3f>();
        RunATanHVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunATanHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunATanHVecRunTimeTests<TRAP::Math::Vec4d>();
        RunATanHVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunATanHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunATanHVecRunTimeTests<TRAP::Math::Vec4f>();
        RunATanHVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunATanHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

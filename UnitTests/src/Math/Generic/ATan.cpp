#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunATanCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(-1.5)), T(-0.982794), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(-0.5)), T(-0.463648), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(0.0)), T(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(0.001)), T(0.001), T(0.000000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(0.49)), T(0.455616), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(0.99)), T(0.780373), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(1.001)), T(0.785898), T(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(1.49)), T(0.979703), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(1.5)), T(0.982794), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(1.99)), T(1.10514), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(2.49)), T(1.18891), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(2.51)), T(1.19166), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(3.99)), T(1.32523), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(7.0)), T(1.4289), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(11.0)), T(1.48014), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(11.1)), T(1.48095), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(25.0)), T(1.53082), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(50.0)), T(1.5508), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(101.0)), T(1.5609), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(150.0)), T(1.56413), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(900.0)), T(1.56969), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(1001.0)), T(1.5698), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ATan(T(1000000.0)), T(1.5708), T(0.00001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunATanRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 23> values
        {
            T(-1.5), T(-0.5), T(0.0), T(0.001), T(0.49), T(0.99), T(1.001), T(1.49), T(1.5),
            T(1.99), T(2.49), T(2.51), T(3.99), T(7.0), T(11.0), T(11.1), T(25.0), T(50.0),
            T(101.0), T(150.0), T(900.0), T(1001.0), T(1000000.0)
        };

        for(const T val : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ATan(val), std::atan(val), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunATanEdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(nan)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunATanVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(-1.5f)), T(-0.982794f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(-0.5f)), T(-0.463648f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(0.0f)), T(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(0.001f)), T(0.001f), T(0.000000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(0.49f)), T(0.455616f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(0.99f)), T(0.780373f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(1.001f)), T(0.785898f), T(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(1.49f)), T(0.979703f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(1.5f)), T(0.982794f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(1.99f)), T(1.10514f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(2.49f)), T(1.18891f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(2.51f)), T(1.19166f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(3.99f)), T(1.32523f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(7.0f)), T(1.4289f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(11.0f)), T(1.48014f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(11.1f)), T(1.48095f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(25.0f)), T(1.53082f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(50.0f)), T(1.5508f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(101.0f)), T(1.5609f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(150.0f)), T(1.56413f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(900.0f)), T(1.56969f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(1001.0f)), T(1.5698f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ATan(T(1000000.0f)), T(1.5708f), T(0.00001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunATanVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<T, 23> values
        {
            T(typename T::value_type(-1.5)),  T(typename T::value_type(-0.5)),  T(typename T::value_type(0.0)),   T(typename T::value_type(0.001)),
            T(typename T::value_type(0.49)), T(typename T::value_type(0.99)), T(typename T::value_type(1.001)), T(typename T::value_type(1.49)),
            T(typename T::value_type(1.5)), T(typename T::value_type(1.99)),  T(typename T::value_type(2.49)),  T(typename T::value_type(2.51)),
            T(typename T::value_type(3.99)),   T(typename T::value_type(7.0)),  T(typename T::value_type(11.0)), T(typename T::value_type(11.1)),
            T(typename T::value_type(25.0)), T(typename T::value_type(50.0)), T(typename T::value_type(101.0)), T(typename T::value_type(150.0)),
            T(typename T::value_type(900.0)), T(typename T::value_type(1001.0)), T(typename T::value_type(1000000.0))
        };

        for(const T val : values)
        {
            const T s = TRAP::Math::ATan(val);
            for(u32 l = 0; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::atan(val[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunATanVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(T(nan)))));
    }
}

TEST_CASE("TRAP::Math::ATan(x)", "[math][generic][atan]")
{
    SECTION("Scalar - f64")
    {
        RunATanRunTimeTests<f64>();
        RunATanCompileTimeTests<f64>();
        RunATanEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunATanRunTimeTests<f32>();
        RunATanCompileTimeTests<f32>();
        RunATanEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunATanVecRunTimeTests<TRAP::Math::Vec2d>();
        RunATanVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunATanVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunATanVecRunTimeTests<TRAP::Math::Vec2f>();
        RunATanVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunATanVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunATanVecRunTimeTests<TRAP::Math::Vec3d>();
        RunATanVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunATanVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunATanVecRunTimeTests<TRAP::Math::Vec3f>();
        RunATanVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunATanVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunATanVecRunTimeTests<TRAP::Math::Vec4d>();
        RunATanVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunATanVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunATanVecRunTimeTests<TRAP::Math::Vec4f>();
        RunATanVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunATanVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

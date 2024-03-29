#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunASinHCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(T(-1.5)), T(-1.19476), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(T(0.0)), T(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(T(0.001)), T(0.001), T(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(T(1.001)), T(0.882081), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(T(1.5)), T(1.19476), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(T(11.1)), T(3.10212), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASinH(T(50.0)), T(4.60527), T(0.000001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunASinHRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 7> values
        {
            T(-1.5), T(0.0), T(0.001), T(1.001), T(1.5), T(11.1), T(50.0)
        };

        for(const T val : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ASinH(val), std::asinh(val), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunASinHEdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASinH(nan)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunASinHVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(T(-1.5f)), T(-1.19476f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(T(0.0f)), T(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(T(0.001f)), T(0.001f), T(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(T(1.001f)), T(0.882081f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(T(1.5f)), T(1.19476f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(T(11.1f)), T(3.10212f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASinH(T(50.0f)), T(4.60527f), T(0.000001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunASinHVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<T, 7> values
        {
            T(typename T::value_type(-1.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)),
            T(typename T::value_type(1.001)), T(typename T::value_type(1.5)), T(typename T::value_type(11.1)),
            T(typename T::value_type(50.0))
        };

        for(const T val : values)
        {
            const T s = TRAP::Math::ASinH(val);
            for(u32 l = 0; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::asinh(val[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunASinHVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASinH(T(nan)))));
    }
}

TEST_CASE("TRAP::Math::ASinH()", "[math][generic][asinh]")
{
    SECTION("Scalar - f64")
    {
        RunASinHRunTimeTests<f64>();
        RunASinHCompileTimeTests<f64>();
        RunASinHEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunASinHRunTimeTests<f32>();
        RunASinHCompileTimeTests<f32>();
        RunASinHEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunASinHVecRunTimeTests<TRAP::Math::Vec2d>();
        RunASinHVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunASinHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunASinHVecRunTimeTests<TRAP::Math::Vec2f>();
        RunASinHVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunASinHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunASinHVecRunTimeTests<TRAP::Math::Vec3d>();
        RunASinHVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunASinHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunASinHVecRunTimeTests<TRAP::Math::Vec3f>();
        RunASinHVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunASinHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunASinHVecRunTimeTests<TRAP::Math::Vec4d>();
        RunASinHVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunASinHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunASinHVecRunTimeTests<TRAP::Math::Vec4f>();
        RunASinHVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunASinHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunASinCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::ASin(T(-0.5)), T(-0.523599), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASin(T(0.0)), T(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASin(T(0.001)), T(0.001), T(0.000000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASin(T(0.5)), T(0.523599), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::ASin(T(1.0)), T(1.5708), T(0.00001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunASinRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 5> values
        {
            T(-0.5), T(0.0), T(0.001), T(0.5), T(1.0)
        };

        for(const T val : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::ASin(val), std::asin(val), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunASinEdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASin(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASin(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ASin(ninf)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunASinVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASin(T(typename T::value_type(-0.5f))), T(-0.523599f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASin(T(typename T::value_type(0.0f))), T(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASin(T(typename T::value_type(0.001f))), T(0.001f), T(0.000000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASin(T(typename T::value_type(0.5f))), T(0.523599f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::ASin(T(typename T::value_type(1.0f))), T(1.5708f), T(0.00001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunASinVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<T, 5> values
        {
            T(typename T::value_type(-0.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)),
            T(typename T::value_type(0.5)), T(typename T::value_type(1.0))
        };

        for(const T val : values)
        {
            const T s = TRAP::Math::ASin(val);
            for(u32 l = 0; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::asin(val[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunASinVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASin(T(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASin(T(inf)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ASin(T(ninf)))));
    }
}

TEST_CASE("TRAP::Math::ASin()", "[math][generic][asin]")
{
    SECTION("Scalar - f64")
    {
        RunASinRunTimeTests<f64>();
        RunASinCompileTimeTests<f64>();
        RunASinEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunASinRunTimeTests<f32>();
        RunASinCompileTimeTests<f32>();
        RunASinEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunASinVecRunTimeTests<TRAP::Math::Vec2d>();
        RunASinVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunASinVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunASinVecRunTimeTests<TRAP::Math::Vec2f>();
        RunASinVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunASinVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunASinVecRunTimeTests<TRAP::Math::Vec3d>();
        RunASinVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunASinVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunASinVecRunTimeTests<TRAP::Math::Vec3f>();
        RunASinVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunASinVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunASinVecRunTimeTests<TRAP::Math::Vec4d>();
        RunASinVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunASinVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunASinVecRunTimeTests<TRAP::Math::Vec4f>();
        RunASinVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunASinVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

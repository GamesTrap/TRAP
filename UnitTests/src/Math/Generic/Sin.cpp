#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunSinCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(T(-1.5)), T(-0.997495), T(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(T(0.0)), T(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(T(0.001)), T(0.001), T(0.000000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(T(1.001)), T(0.842011), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(T(1.5)), T(0.997495), T(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(T(11.1)), T(-0.994553), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(T(50.0)), T(-0.262375), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sin(T(150.0)), T(-0.714876), T(0.00001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunSinRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 8> values
        {
            T(-1.5), T(0.0), T(0.001), T(1.001), T(1.5), T(11.1), T(50.0), T(150.0)
        };

        for(const T val : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Sin(val), std::sin(val), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunSinEdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sin(ninf)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunSinVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(T(typename T::value_type(-1.5f))), T(-0.997495f), T(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(T(typename T::value_type(0.0f))), T(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(T(typename T::value_type(0.001f))), T(0.001f), T(0.000000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(T(typename T::value_type(1.001f))), T(0.842011f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(T(typename T::value_type(1.5f))), T(0.997495f), T(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(T(typename T::value_type(11.1f))), T(-0.994553f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(T(typename T::value_type(50.0f))), T(-0.262375f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sin(T(typename T::value_type(150.0f))), T(-0.714876f), T(0.00001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunSinVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<T, 8> values
        {
            T(typename T::value_type(-1.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)), T(typename T::value_type(1.001)),
            T(typename T::value_type(1.5)), T(typename T::value_type(11.1)), T(typename T::value_type(50.0)), T(typename T::value_type(150.0))
        };

        for(const T val : values)
        {
            const T s = TRAP::Math::Sin(val);
            for(u32 l = 0; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::sin(val[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunSinVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sin(T(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sin(T(inf)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sin(T(ninf)))));
    }
}

TEST_CASE("TRAP::Math::Sin()", "[math][generic][sin]")
{
    SECTION("Scalar - f64")
    {
        RunSinRunTimeTests<f64>();
        RunSinCompileTimeTests<f64>();
        RunSinEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunSinRunTimeTests<f32>();
        RunSinCompileTimeTests<f32>();
        RunSinEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunSinVecRunTimeTests<TRAP::Math::Vec2d>();
        RunSinVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunSinVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunSinVecRunTimeTests<TRAP::Math::Vec2f>();
        RunSinVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunSinVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunSinVecRunTimeTests<TRAP::Math::Vec3d>();
        RunSinVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunSinVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunSinVecRunTimeTests<TRAP::Math::Vec3f>();
        RunSinVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunSinVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunSinVecRunTimeTests<TRAP::Math::Vec4d>();
        RunSinVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunSinVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunSinVecRunTimeTests<TRAP::Math::Vec4f>();
        RunSinVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunSinVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

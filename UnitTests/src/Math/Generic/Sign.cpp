#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::signed_integral<T> || std::floating_point<T> || TRAP::Math::IsVec<T>
    consteval void RunCompileTimeSignTests()
    {
        if constexpr(std::signed_integral<T> || std::floating_point<T>)
        {
            if constexpr (std::floating_point<T>)
            {
                constexpr T Epsilon = std::numeric_limits<T>::epsilon();

                static_assert(TRAP::Math::Equal(TRAP::Math::Sign(T( 0)), T( 0), Epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Sign(T( 1)), T( 1), Epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Sign(T( 2)), T( 1), Epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Sign(T( 3)), T( 1), Epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Sign(T(-1)), T(-1), Epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Sign(T(-2)), T(-1), Epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Sign(T(-3)), T(-1), Epsilon));
            }
            else
            {
                static_assert(TRAP::Math::Sign(std::numeric_limits<T>::max()) == T(1));
                static_assert(TRAP::Math::Sign(std::numeric_limits<T>::min()) == T(-1));
                static_assert(TRAP::Math::Sign(T( 0)) == T( 0));
                static_assert(TRAP::Math::Sign(T( 1)) == T( 1));
                static_assert(TRAP::Math::Sign(T( 2)) == T( 1));
                static_assert(TRAP::Math::Sign(T( 3)) == T( 1));
                static_assert(TRAP::Math::Sign(T(-1)) == T(-1));
                static_assert(TRAP::Math::Sign(T(-2)) == T(-1));
                static_assert(TRAP::Math::Sign(T(-3)) == T(-1));
            }
        }
        else if constexpr(TRAP::Math::IsVec<T>)
        {
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(T( 1)), T( 1))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(T( 0)), T( 0))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(T( 2)), T( 1))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(T( 3)), T( 1))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(T(-1)), T(-1))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(T(-2)), T(-1))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(T(-3)), T(-1))));
        }
    }

    template<typename T>
    requires std::signed_integral<T> || std::floating_point<T>
    consteval void RunCompileTimeSignEdgeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr T max = std::numeric_limits<T>::max();
        constexpr T min = std::numeric_limits<T>::lowest();
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();

        static_assert(TRAP::Math::Equal(TRAP::Math::Sign(max), T(1.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sign(min), T(-1.0), Epsilon));
        if constexpr(std::floating_point<T>)
        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(nan), T(0.0), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(inf), T(1.0), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Sign(ninf), T(-1.0), Epsilon));
        }
    }
}

TEST_CASE("TRAP::Math::Sign()", "[math][generic][sign]")
{
    SECTION("Scalar - i8")
    {
        RunCompileTimeSignTests<i8>();
        RunCompileTimeSignEdgeTests<i8>();
    }
    SECTION("Scalar - i16")
    {
        RunCompileTimeSignTests<i16>();
        RunCompileTimeSignEdgeTests<i16>();
    }
    SECTION("Scalar - i32")
    {
        RunCompileTimeSignTests<i32>();
        RunCompileTimeSignEdgeTests<i32>();
    }
    SECTION("Scalar - i64")
    {
        RunCompileTimeSignTests<i64>();
        RunCompileTimeSignEdgeTests<i64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeSignTests<f32>();
        RunCompileTimeSignEdgeTests<f32>();
    }
    SECTION("Scalar - f64")
    {
        RunCompileTimeSignTests<f64>();
        RunCompileTimeSignEdgeTests<f64>();
    }

    SECTION("Vec2 - i8")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2i8>();
        RunCompileTimeSignTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - i16")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2i16>();
        RunCompileTimeSignTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - i32")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2i32>();
        RunCompileTimeSignTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - i64")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2i64>();
        RunCompileTimeSignTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - f64")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2d>();
        RunCompileTimeSignTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2f>();
        RunCompileTimeSignTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - i8")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3i8>();
        RunCompileTimeSignTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - i16")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3i16>();
        RunCompileTimeSignTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - i32")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3i32>();
        RunCompileTimeSignTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - i64")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3i64>();
        RunCompileTimeSignTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - f64")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3d>();
        RunCompileTimeSignTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3f>();
        RunCompileTimeSignTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - i8")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4i8>();
        RunCompileTimeSignTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - i16")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4i16>();
        RunCompileTimeSignTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - i32")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4i32>();
        RunCompileTimeSignTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - i64")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4i64>();
        RunCompileTimeSignTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - f64")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4d>();
        RunCompileTimeSignTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4f>();
        RunCompileTimeSignTests<TRAP::Math::Vec4f>();
    }
}
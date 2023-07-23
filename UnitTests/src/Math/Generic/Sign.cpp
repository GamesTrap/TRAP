#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

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

TEST_CASE("TRAP::Math::Sign()", "[math][generic][sign]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeSignTests<int8_t>();
        RunCompileTimeSignEdgeTests<int8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeSignTests<int16_t>();
        RunCompileTimeSignEdgeTests<int16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeSignTests<int32_t>();
        RunCompileTimeSignEdgeTests<int32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeSignTests<int64_t>();
        RunCompileTimeSignEdgeTests<int64_t>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeSignTests<float>();
        RunCompileTimeSignEdgeTests<float>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeSignTests<double>();
        RunCompileTimeSignEdgeTests<double>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2i8>();
        RunCompileTimeSignTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2i16>();
        RunCompileTimeSignTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2i32>();
        RunCompileTimeSignTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2i64>();
        RunCompileTimeSignTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2d>();
        RunCompileTimeSignTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec2f>();
        RunCompileTimeSignTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3i8>();
        RunCompileTimeSignTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3i16>();
        RunCompileTimeSignTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3i32>();
        RunCompileTimeSignTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3i64>();
        RunCompileTimeSignTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3d>();
        RunCompileTimeSignTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec3f>();
        RunCompileTimeSignTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4i8>();
        RunCompileTimeSignTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4i16>();
        RunCompileTimeSignTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4i32>();
        RunCompileTimeSignTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4i64>();
        RunCompileTimeSignTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4d>();
        RunCompileTimeSignTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeSignTests<TRAP::Math::Vec4f>();
        RunCompileTimeSignTests<TRAP::Math::Vec4f>();
    }
}
#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::signed_integral<T> || std::floating_point<T> || TRAP::Math::IsVec<T>
void RunSignTests(const T val, const T expected)
{
    if constexpr (std::integral<T>)
    {
        REQUIRE(TRAP::Math::Sign(val) == expected);
    }
    else if constexpr(std::floating_point<T>)
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();
        REQUIRE_THAT(TRAP::Math::Sign(val), Catch::Matchers::WithinRel(expected, Epsilon));
    }
    else if constexpr(TRAP::Math::IsVec<T>)
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sign(val), expected)));
    }
}

template<typename T>
requires std::signed_integral<T> || std::floating_point<T> || TRAP::Math::IsVec<T>
constexpr void RunConstexprSignTests()
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

TEST_CASE("TRAP::Math::Sign()", "[math][generic][sign]")
{
    const std::pair val = GENERATE(values(
        {
            std::pair{ 0,  0},
            std::pair{ 1,  1},
            std::pair{ 2,  1},
            std::pair{ 3,  1},
            std::pair{-1, -1},
            std::pair{-2, -1},
            std::pair{-3, -1},
        }));

    SECTION("Scalar - int8_t")
    {
        RunSignTests(static_cast<int8_t>(val.first), static_cast<int8_t>(val.second));
        RunConstexprSignTests<int8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunSignTests(static_cast<int16_t>(val.first), static_cast<int16_t>(val.second));
        RunConstexprSignTests<int16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunSignTests(static_cast<int32_t>(val.first), static_cast<int32_t>(val.second));
        RunConstexprSignTests<int32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunSignTests(static_cast<int64_t>(val.first), static_cast<int64_t>(val.second));
        RunConstexprSignTests<int64_t>();
    }
    SECTION("Scalar - float")
    {
        RunSignTests(static_cast<float>(val.first), static_cast<float>(val.second));
        RunConstexprSignTests<float>();
    }
    SECTION("Scalar - double")
    {
        RunSignTests(static_cast<double>(val.first), static_cast<double>(val.second));
        RunConstexprSignTests<double>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunSignTests(TRAP::Math::Vec2i8(static_cast<int8_t>(val.first)), TRAP::Math::Vec2i8(static_cast<int8_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunSignTests(TRAP::Math::Vec2i16(static_cast<int16_t>(val.first)), TRAP::Math::Vec2i16(static_cast<int16_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunSignTests(TRAP::Math::Vec2i32(static_cast<int32_t>(val.first)), TRAP::Math::Vec2i32(static_cast<int32_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunSignTests(TRAP::Math::Vec2i64(static_cast<int64_t>(val.first)), TRAP::Math::Vec2i64(static_cast<int64_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - double")
    {
        RunSignTests(TRAP::Math::Vec2d(static_cast<double>(val.first)), TRAP::Math::Vec2d(static_cast<double>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunSignTests(TRAP::Math::Vec2f(static_cast<float>(val.first)), TRAP::Math::Vec2f(static_cast<float>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunSignTests(TRAP::Math::Vec3i8(static_cast<int8_t>(val.first)), TRAP::Math::Vec3i8(static_cast<int8_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunSignTests(TRAP::Math::Vec3i16(static_cast<int16_t>(val.first)), TRAP::Math::Vec3i16(static_cast<int16_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunSignTests(TRAP::Math::Vec3i32(static_cast<int32_t>(val.first)), TRAP::Math::Vec3i32(static_cast<int32_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunSignTests(TRAP::Math::Vec3i64(static_cast<int64_t>(val.first)), TRAP::Math::Vec3i64(static_cast<int64_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - double")
    {
        RunSignTests(TRAP::Math::Vec3d(static_cast<double>(val.first)), TRAP::Math::Vec3d(static_cast<double>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunSignTests(TRAP::Math::Vec3f(static_cast<float>(val.first)), TRAP::Math::Vec3f(static_cast<float>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunSignTests(TRAP::Math::Vec4i8(static_cast<int8_t>(val.first)), TRAP::Math::Vec4i8(static_cast<int8_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunSignTests(TRAP::Math::Vec4i16(static_cast<int16_t>(val.first)), TRAP::Math::Vec4i16(static_cast<int16_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunSignTests(TRAP::Math::Vec4i32(static_cast<int32_t>(val.first)), TRAP::Math::Vec4i32(static_cast<int32_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunSignTests(TRAP::Math::Vec4i64(static_cast<int64_t>(val.first)), TRAP::Math::Vec4i64(static_cast<int64_t>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - double")
    {
        RunSignTests(TRAP::Math::Vec4d(static_cast<double>(val.first)), TRAP::Math::Vec4d(static_cast<double>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunSignTests(TRAP::Math::Vec4f(static_cast<float>(val.first)), TRAP::Math::Vec4f(static_cast<float>(val.second)));
        RunConstexprSignTests<TRAP::Math::Vec4f>();
    }
}
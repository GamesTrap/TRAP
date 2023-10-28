#include <cstdint>
#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T, typename X = T>
    requires (std::is_arithmetic_v<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)) &&
            (std::is_arithmetic_v<X> || (TRAP::Math::IsVec<X> && std::floating_point<typename X::value_type>))
    consteval void RunModCompileTimeTests()
    {
        if constexpr(std::is_arithmetic_v<T>)
        {
            if constexpr(std::floating_point<T>)
            {
                constexpr T Epsilon = std::numeric_limits<T>::epsilon();
                static_assert(TRAP::Math::Equal(TRAP::Math::Mod(T( 1.5f), X(1.0f)), T( 0.5f), Epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Mod(T(-0.2f), X(1.0f)), T(-0.2f), Epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Mod(T( 3.0f), X(2.0f)), T( 1.0f), Epsilon));
            }
            else if constexpr(std::integral<T>)
            {
                static_assert(TRAP::Math::Mod(T( 1.5f), X(1.0f)) == T( 0.5f));
                static_assert(TRAP::Math::Mod(T(-0.2f), X(1.0f)) == T(-0.2f));
                static_assert(TRAP::Math::Mod(T( 3.0f), X(2.0f)) == T( 1.0f));
            }
        }
        else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type> && TRAP::Math::IsVec<X>)
        {
            constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(T( 1.5f), X(1.0f)), T( 0.5f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(T(-0.2f), X(1.0f)), T(-0.2f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(T( 3.0f), X(2.0f)), T( 1.0f), Epsilon)));
        }
        else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type> && std::floating_point<X>)
        {
            constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(T( 1.5f), X(1.0f)), T( 0.5f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(T(-0.2f), X(1.0f)), T(-0.2f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(T( 3.0f), X(2.0f)), T( 1.0f), Epsilon)));
        }
    }

    template<typename T, typename X = T>
    requires (std::is_arithmetic_v<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)) &&
            (std::is_arithmetic_v<X> || (TRAP::Math::IsVec<X> && std::floating_point<typename X::value_type>))
    void RunModRunTimeTests()
    {
        static constexpr std::array<std::tuple<T, X, T>, 3> values
        {
            std::tuple<T, X, T>{T( 1.5f), X(1.0f), T( 0.5f)},
            std::tuple<T, X, T>{T(-0.2f), X(1.0f), T(-0.2f)},
            std::tuple<T, X, T>{T( 3.0f), X(2.0f), T( 1.0f)},
        };

        for(const auto& [val1, val2, expected] : values)
        {
            if constexpr(std::is_arithmetic_v<T>)
            {
                if constexpr(std::floating_point<T>)
                {
                    static constexpr T Epsilon = std::numeric_limits<T>::epsilon();
                    REQUIRE(TRAP::Math::Equal(TRAP::Math::Mod(val1, val2), expected, Epsilon));
                }
                else if constexpr(std::integral<T>)
                {
                    REQUIRE(TRAP::Math::Mod(val1, val2) == expected);
                }
            }
            else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type> && TRAP::Math::IsVec<X>)
            {
                static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(val1, T(val2)), expected, Epsilon)));
            }
            else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type> && std::floating_point<X>)
            {
                constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mod(val1, val2), expected, Epsilon)));
            }
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunModEdgeRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::lowest();

        if constexpr(std::floating_point<T>)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Mod(max, T(100.0)), static_cast<T>(std::fmod(max, T(100))), Epsilon));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Mod(min, T(-50.0)), static_cast<T>(std::fmod(min, T(-50.0))), Epsilon));

            static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Mod(T(15), T(0))));
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Mod(T(2.0), nan)));
        }
        else if constexpr(std::integral<T>)
        {
            REQUIRE(TRAP::Math::Mod(max, T(100.0)) == static_cast<T>(max % T(100)));

            if constexpr(std::signed_integral<T>)
            {
                REQUIRE(TRAP::Math::Mod(min, T(-50.0)) == static_cast<T>(min % T(-50.0)));
            }
        }
    }
}

TEST_CASE("TRAP::Math::Mod()", "[math][generic][mod]")
{
    SECTION("Scalar - i8")
    {
        RunModRunTimeTests<i8>();
        RunModCompileTimeTests<i8>();
        RunModEdgeRunTimeTests<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunModRunTimeTests<u8>();
        RunModCompileTimeTests<u8>();
        RunModEdgeRunTimeTests<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunModRunTimeTests<i16>();
        RunModCompileTimeTests<i16>();
        RunModEdgeRunTimeTests<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunModRunTimeTests<u16>();
        RunModCompileTimeTests<u16>();
        RunModEdgeRunTimeTests<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunModRunTimeTests<i32>();
        RunModCompileTimeTests<i32>();
        RunModEdgeRunTimeTests<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunModRunTimeTests<u32>();
        RunModCompileTimeTests<u32>();
        RunModEdgeRunTimeTests<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunModRunTimeTests<i64>();
        RunModCompileTimeTests<i64>();
        RunModEdgeRunTimeTests<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunModRunTimeTests<u64>();
        RunModCompileTimeTests<u64>();
        RunModEdgeRunTimeTests<u64>();
    }
    SECTION("Scalar - f64")
    {
        RunModRunTimeTests<f64>();
        RunModCompileTimeTests<f64>();
        RunModEdgeRunTimeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunModRunTimeTests<f32>();
        RunModCompileTimeTests<f32>();
        RunModEdgeRunTimeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunModRunTimeTests<TRAP::Math::Vec2d>();
        RunModCompileTimeTests<TRAP::Math::Vec2d>();
        RunModRunTimeTests<TRAP::Math::Vec2d, f64>();
        RunModCompileTimeTests<TRAP::Math::Vec2d, f64>();
    }
    SECTION("Vec2 - f32")
    {
        RunModRunTimeTests<TRAP::Math::Vec2f>();
        RunModCompileTimeTests<TRAP::Math::Vec2f>();
        RunModRunTimeTests<TRAP::Math::Vec2f, f32>();
        RunModCompileTimeTests<TRAP::Math::Vec2f, f32>();
    }

    SECTION("Vec3 - f64")
    {
        RunModRunTimeTests<TRAP::Math::Vec3d>();
        RunModCompileTimeTests<TRAP::Math::Vec3d>();
        RunModRunTimeTests<TRAP::Math::Vec3d, f64>();
        RunModCompileTimeTests<TRAP::Math::Vec3d, f64>();
    }
    SECTION("Vec3 - f32")
    {
        RunModRunTimeTests<TRAP::Math::Vec3f>();
        RunModCompileTimeTests<TRAP::Math::Vec3f>();
        RunModRunTimeTests<TRAP::Math::Vec3f, f32>();
        RunModCompileTimeTests<TRAP::Math::Vec3f, f32>();
    }

    SECTION("Vec4 - f64")
    {
        RunModRunTimeTests<TRAP::Math::Vec4d>();
        RunModCompileTimeTests<TRAP::Math::Vec4d>();
        RunModRunTimeTests<TRAP::Math::Vec4d, f64>();
        RunModCompileTimeTests<TRAP::Math::Vec4d, f64>();
    }
    SECTION("Vec4 - f32")
    {
        RunModRunTimeTests<TRAP::Math::Vec4f>();
        RunModCompileTimeTests<TRAP::Math::Vec4f>();
        RunModRunTimeTests<TRAP::Math::Vec4f, f32>();
        RunModCompileTimeTests<TRAP::Math::Vec4f, f32>();
    }
}
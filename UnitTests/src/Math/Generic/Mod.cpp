#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

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
                    REQUIRE_THAT(TRAP::Math::Mod(val1, val2), Catch::Matchers::WithinRel(expected, Epsilon));
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
    SECTION("Scalar - int8_t")
    {
        RunModRunTimeTests<int8_t>();
        RunModCompileTimeTests<int8_t>();
        RunModEdgeRunTimeTests<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunModRunTimeTests<uint8_t>();
        RunModCompileTimeTests<uint8_t>();
        RunModEdgeRunTimeTests<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunModRunTimeTests<int16_t>();
        RunModCompileTimeTests<int16_t>();
        RunModEdgeRunTimeTests<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunModRunTimeTests<uint16_t>();
        RunModCompileTimeTests<uint16_t>();
        RunModEdgeRunTimeTests<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunModRunTimeTests<int32_t>();
        RunModCompileTimeTests<int32_t>();
        RunModEdgeRunTimeTests<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunModRunTimeTests<uint32_t>();
        RunModCompileTimeTests<uint32_t>();
        RunModEdgeRunTimeTests<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunModRunTimeTests<int64_t>();
        RunModCompileTimeTests<int64_t>();
        RunModEdgeRunTimeTests<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunModRunTimeTests<uint64_t>();
        RunModCompileTimeTests<uint64_t>();
        RunModEdgeRunTimeTests<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunModRunTimeTests<double>();
        RunModCompileTimeTests<double>();
        RunModEdgeRunTimeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunModRunTimeTests<float>();
        RunModCompileTimeTests<float>();
        RunModEdgeRunTimeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunModRunTimeTests<TRAP::Math::Vec2d>();
        RunModCompileTimeTests<TRAP::Math::Vec2d>();
        RunModRunTimeTests<TRAP::Math::Vec2d, double>();
        RunModCompileTimeTests<TRAP::Math::Vec2d, double>();
    }
    SECTION("Vec2 - float")
    {
        RunModRunTimeTests<TRAP::Math::Vec2f>();
        RunModCompileTimeTests<TRAP::Math::Vec2f>();
        RunModRunTimeTests<TRAP::Math::Vec2f, float>();
        RunModCompileTimeTests<TRAP::Math::Vec2f, float>();
    }

    SECTION("Vec3 - double")
    {
        RunModRunTimeTests<TRAP::Math::Vec3d>();
        RunModCompileTimeTests<TRAP::Math::Vec3d>();
        RunModRunTimeTests<TRAP::Math::Vec3d, double>();
        RunModCompileTimeTests<TRAP::Math::Vec3d, double>();
    }
    SECTION("Vec3 - float")
    {
        RunModRunTimeTests<TRAP::Math::Vec3f>();
        RunModCompileTimeTests<TRAP::Math::Vec3f>();
        RunModRunTimeTests<TRAP::Math::Vec3f, float>();
        RunModCompileTimeTests<TRAP::Math::Vec3f, float>();
    }

    SECTION("Vec4 - double")
    {
        RunModRunTimeTests<TRAP::Math::Vec4d>();
        RunModCompileTimeTests<TRAP::Math::Vec4d>();
        RunModRunTimeTests<TRAP::Math::Vec4d, double>();
        RunModCompileTimeTests<TRAP::Math::Vec4d, double>();
    }
    SECTION("Vec4 - float")
    {
        RunModRunTimeTests<TRAP::Math::Vec4f>();
        RunModCompileTimeTests<TRAP::Math::Vec4f>();
        RunModRunTimeTests<TRAP::Math::Vec4f, float>();
        RunModCompileTimeTests<TRAP::Math::Vec4f, float>();
    }
}
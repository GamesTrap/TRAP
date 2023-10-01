#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    consteval void RunRoundEvenVecCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-1.5f)), T(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 1.5f)), T( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-2.5f)), T(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 2.5f)), T( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-3.5f)), T(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 3.5f)), T( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-4.5f)), T(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 4.5f)), T( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-5.5f)), T(-6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 5.5f)), T( 6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-6.5f)), T(-6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 6.5f)), T( 6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-7.5f)), T(-8.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 7.5f)), T( 8.0f), Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-2.4f)), T(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 2.4f)), T( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-2.6f)), T(-3.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 2.6f)), T( 3.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-2.0f)), T(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 2.0f)), T( 2.0f), Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 0.0f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 0.5f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 1.0f)), T( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 0.1f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 0.9f)), T( 1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 1.5f)), T( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 1.9f)), T( 2.0f), Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-0.0f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-0.5f)), T(-0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-1.0f)), T(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-0.1f)), T( 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-0.9f)), T(-1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-1.5f)), T(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-1.9f)), T(-2.0f), Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 1.5f)), T( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 2.5f)), T( 2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 3.5f)), T( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 4.5f)), T( 4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 5.5f)), T( 6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 6.5f)), T( 6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 7.5f)), T( 8.0f), Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-1.5f)), T(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-2.5f)), T(-2.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-3.5f)), T(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-4.5f)), T(-4.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-5.5f)), T(-6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-6.5f)), T(-6.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-7.5f)), T(-8.0f), Epsilon)));
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunRoundEvenCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-1.5f)), T(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 1.5f)), T( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-2.5f)), T(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 2.5f)), T( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-3.5f)), T(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 3.5f)), T( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-4.5f)), T(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 4.5f)), T( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-5.5f)), T(-6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 5.5f)), T( 6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-6.5f)), T(-6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 6.5f)), T( 6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-7.5f)), T(-8.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 7.5f)), T( 8.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-2.4f)), T(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 2.4f)), T( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-2.6f)), T(-3.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 2.6f)), T( 3.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-2.0f)), T(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 2.0f)), T( 2.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 0.0f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 0.5f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 1.0f)), T( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 0.1f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 0.9f)), T( 1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 1.5f)), T( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 1.9f)), T( 2.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-0.0f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-0.5f)), T(-0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-1.0f)), T(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-0.1f)), T( 0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-0.9f)), T(-1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-1.5f)), T(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-1.9f)), T(-2.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 1.5f)), T( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 2.5f)), T( 2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 3.5f)), T( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 4.5f)), T( 4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 5.5f)), T( 6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 6.5f)), T( 6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T( 7.5f)), T( 8.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-1.5f)), T(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-2.5f)), T(-2.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-3.5f)), T(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-4.5f)), T(-4.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-5.5f)), T(-6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-6.5f)), T(-6.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::RoundEven(T(-7.5f)), T(-8.0f), Epsilon));
    }

    template<typename T>
    requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    void RunRoundEvenRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<std::pair<T, T>, 48> values
        {
            std::pair<T, T>{T(-1.5f), T(-2.0f)}, std::pair<T, T>{T( 1.5f), T( 2.0f)}, std::pair<T, T>{T(-2.5f), T(-2.0f)},
            std::pair<T, T>{T( 2.5f), T( 2.0f)}, std::pair<T, T>{T(-3.5f), T(-4.0f)}, std::pair<T, T>{T( 3.5f), T( 4.0f)},
            std::pair<T, T>{T(-4.5f), T(-4.0f)}, std::pair<T, T>{T( 4.5f), T( 4.0f)}, std::pair<T, T>{T(-5.5f), T(-6.0f)},
            std::pair<T, T>{T( 5.5f), T( 6.0f)}, std::pair<T, T>{T(-6.5f), T(-6.0f)}, std::pair<T, T>{T( 6.5f), T( 6.0f)},
            std::pair<T, T>{T(-7.5f), T(-8.0f)}, std::pair<T, T>{T( 7.5f), T( 8.0f)},

            std::pair<T, T>{T(-2.4f), T(-2.0f)}, std::pair<T, T>{T( 2.4f), T( 2.0f)}, std::pair<T, T>{T(-2.6f), T(-3.0f)},
            std::pair<T, T>{T( 2.6f), T( 3.0f)}, std::pair<T, T>{T(-2.0f), T(-2.0f)}, std::pair<T, T>{T( 2.0f), T( 2.0f)},

            std::pair<T, T>{T( 0.0f), T( 0.0f)}, std::pair<T, T>{T( 0.5f), T( 0.0f)}, std::pair<T, T>{T( 1.0f), T( 1.0f)},
            std::pair<T, T>{T( 0.1f), T( 0.0f)}, std::pair<T, T>{T( 0.9f), T( 1.0f)}, std::pair<T, T>{T( 1.5f), T( 2.0f)},
            std::pair<T, T>{T( 1.9f), T( 2.0f)},

            std::pair<T, T>{T(-0.0f), T( 0.0f)}, std::pair<T, T>{T(-0.5f), T(-0.0f)}, std::pair<T, T>{T(-1.0f), T(-1.0f)},
            std::pair<T, T>{T(-0.1f), T( 0.0f)}, std::pair<T, T>{T(-0.9f), T(-1.0f)}, std::pair<T, T>{T(-1.5f), T(-2.0f)},
            std::pair<T, T>{T(-1.9f), T(-2.0f)},

            std::pair<T, T>{T( 1.5f), T( 2.0f)}, std::pair<T, T>{T( 2.5f), T( 2.0f)}, std::pair<T, T>{T( 3.5f), T( 4.0f)},
            std::pair<T, T>{T( 4.5f), T( 4.0f)}, std::pair<T, T>{T( 5.5f), T( 6.0f)}, std::pair<T, T>{T( 6.5f), T( 6.0f)},
            std::pair<T, T>{T( 7.5f), T( 8.0f)},

            std::pair<T, T>{T(-1.5f), T(-2.0f)}, std::pair<T, T>{T(-2.5f), T(-2.0f)}, std::pair<T, T>{T(-3.5f), T(-4.0f)},
            std::pair<T, T>{T(-4.5f), T(-4.0f)}, std::pair<T, T>{T(-5.5f), T(-6.0f)}, std::pair<T, T>{T(-6.5f), T(-6.0f)},
            std::pair<T, T>{T(-7.5f), T(-8.0f)},
        };

        for(const auto& [val, expected] : values)
        {
            if constexpr(std::floating_point<T>)
            {
                REQUIRE_THAT(TRAP::Math::RoundEven(val), Catch::Matchers::WithinRel(expected, Epsilon));
            }
            else if constexpr(TRAP::Math::IsVec<T>)
            {
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::RoundEven(val), T(expected), Epsilon)));
            }
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunRoundEvenEdgeRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::lowest();

        REQUIRE(TRAP::Math::Equal(TRAP::Math::RoundEven(max), max, Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::RoundEven(min), min, Epsilon));
    }
}

TEST_CASE("TRAP::Math::RoundEven()", "[math][generic][roundeven]")
{
    SECTION("Scalar - double")
    {
        RunRoundEvenRunTimeTests<double>();
        RunRoundEvenCompileTimeTests<double>();
        RunRoundEvenEdgeRunTimeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunRoundEvenRunTimeTests<float>();
        RunRoundEvenCompileTimeTests<float>();
        RunRoundEvenEdgeRunTimeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunRoundEvenRunTimeTests<TRAP::Math::Vec2d>();
        RunRoundEvenVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunRoundEvenRunTimeTests<TRAP::Math::Vec2f>();
        RunRoundEvenVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunRoundEvenRunTimeTests<TRAP::Math::Vec3d>();
        RunRoundEvenVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunRoundEvenRunTimeTests<TRAP::Math::Vec3f>();
        RunRoundEvenVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunRoundEvenRunTimeTests<TRAP::Math::Vec4d>();
        RunRoundEvenVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunRoundEvenRunTimeTests<TRAP::Math::Vec4f>();
        RunRoundEvenVecCompileTimeTests<TRAP::Math::Vec4f>();
    }
}
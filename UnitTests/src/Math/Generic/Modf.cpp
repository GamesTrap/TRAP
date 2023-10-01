#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T> || (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
    void RunModfTests()
    {
        if constexpr(std::floating_point<T>)
        {
            static constexpr T Epsilon = std::numeric_limits<T>::epsilon();
            T i{};

            static constexpr std::array<std::tuple<T, T, T>, 1> values
            {
                std::tuple{T(1.5), T(0.5), T(1.0)}
            };

            for(const auto& [val, expected, expectedI] : values)
            {
                REQUIRE_THAT(TRAP::Math::Modf(val, i), Catch::Matchers::WithinRel(expected, Epsilon));
                REQUIRE_THAT(i, Catch::Matchers::WithinRel(expectedI, Epsilon));
            }
        }
        else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
        {
            static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();
            T i{};

            static constexpr std::array<std::tuple<T, T, T>, 1> values
            {
                std::tuple<T, T, T>{TRAP::Math::Vec4d{1.1, 1.2, 1.5, 1.7}, TRAP::Math::Vec4d{0.1, 0.2, 0.5, 0.7}, TRAP::Math::Vec4d(1.0)}
            };

            for(const auto& [val, expected, expectedI] : values)
            {
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Modf(val, i), expected, Epsilon)));
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(i, expectedI, Epsilon)));
            }
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunModfEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();

        T i{};

        const T resMax = TRAP::Math::Modf(max, i);
        REQUIRE((TRAP::Math::Equal(resMax, T(0), Epsilon) && (static_cast<T>(i) == max)));

        const T resMin = TRAP::Math::Modf(min, i);
        REQUIRE((TRAP::Math::Equal(resMin, T(0), Epsilon) && (static_cast<T>(i) == min)));

        const T resNaN = TRAP::Math::Modf(nan, i);
        REQUIRE((TRAP::Math::IsNaN(resNaN) && TRAP::Math::IsNaN(i)));

        const T resInf = TRAP::Math::Modf(inf, i);
        REQUIRE((TRAP::Math::Equal(resInf, T(0), Epsilon) && (static_cast<T>(i) == inf)));

        const T resNInf = TRAP::Math::Modf(-inf, i);
        REQUIRE((TRAP::Math::Equal(resNInf, T(-0), Epsilon) && (static_cast<T>(i) == -inf)));
    }
}

TEST_CASE("TRAP::Math::Modf()", "[math][generic][modf]")
{
    SECTION("Scalar - double")
    {
        RunModfTests<double>();
        RunModfEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunModfTests<float>();
        RunModfEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunModfTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunModfTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunModfTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunModfTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunModfTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunModfTests<TRAP::Math::Vec4f>();
    }
}
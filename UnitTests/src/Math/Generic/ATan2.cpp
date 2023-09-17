#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunATanTests()
{
    static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static constexpr std::array<std::pair<T, T>, 16> values
    {
        std::pair(T(0.0), T(0.0)), std::pair(T(0.2), T(0.0)), std::pair(T(-0.2), T(0.0)),
        std::pair(T(0.001), T(0.001)), std::pair(T(0.49), T(0.49)), std::pair(T(-0.5), T(-0.5)),
        std::pair(T(0.5), T(-0.5)), std::pair(T(-0.5), T(0.5)), std::pair(T(9.6), T(8.4)),
        std::pair(T(1.0), T(0.0)), std::pair(T(0.0), T(1.0)), std::pair(T(-1.0), T(0.0)),
        std::pair(T(0.0), T(-1.0)), std::pair(T(1.0), T(3.0)), std::pair(T(-5.0), T(2.5)),
        std::pair(T(-1000.0), T(-0.001)),
    };

    for(const auto& [y, x] : values)
    {
        REQUIRE_THAT(TRAP::Math::ATan(y, x), Catch::Matchers::WithinRel(std::atan2(y, x), Epsilon));
    }
}

template<typename T>
requires std::floating_point<T>
void RunATanEdgeTests()
{
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(nan, T(1.0))));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(T(1.0), nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::ATan(nan, nan)));
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunATanVecTests()
{
    static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    static constexpr std::array<std::pair<T, T>, 16> values
    {
        std::pair(T(typename T::value_type(0.0)), T(typename T::value_type(0.0))),
        std::pair(T(typename T::value_type(0.2)), T(typename T::value_type(0.0))),
        std::pair(T(typename T::value_type(-0.2)), T(typename T::value_type(0.0))),
        std::pair(T(typename T::value_type(0.001)), T(typename T::value_type(0.001))),
        std::pair(T(typename T::value_type(0.49)), T(typename T::value_type(0.49))),
        std::pair(T(typename T::value_type(-0.5)), T(typename T::value_type(-0.5))),
        std::pair(T(typename T::value_type(0.5)), T(typename T::value_type(-0.5))),
        std::pair(T(typename T::value_type(-0.5)), T(typename T::value_type(0.5))),
        std::pair(T(typename T::value_type(9.6)), T(typename T::value_type(8.4))),
        std::pair(T(typename T::value_type(1.0)), T(typename T::value_type(0.0))),
        std::pair(T(typename T::value_type(0.0)), T(typename T::value_type(1.0))),
        std::pair(T(typename T::value_type(-1.0)), T(typename T::value_type(0.0))),
        std::pair(T(typename T::value_type(0.0)), T(typename T::value_type(-1.0))),
        std::pair(T(typename T::value_type(1.0)), T(typename T::value_type(3.0))),
        std::pair(T(typename T::value_type(-5.0)), T(typename T::value_type(2.5))),
        std::pair(T(typename T::value_type(-1000.0)), T(typename T::value_type(-0.001))),
    };

    for(const auto& [y, x] : values)
    {
        const T s = TRAP::Math::ATan(y, x);
        for(uint32_t l = 0; l < T::Length(); ++l)
            REQUIRE(TRAP::Math::Equal(s[l], std::atan2(y[l], x[l]), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunATanVecEdgeTests()
{
    static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(T(nan), T(1.0)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(T(1.0), T(nan)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::ATan(T(nan), T(nan)))));
}

TEST_CASE("TRAP::Math::ATan(y, x)", "[math][generic][atan2]")
{
    SECTION("Scalar - double")
    {
        RunATanTests<double>();
        RunATanEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunATanTests<float>();
        RunATanEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunATanVecTests<TRAP::Math::Vec2d>();
        RunATanVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunATanVecTests<TRAP::Math::Vec2f>();
        RunATanVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunATanVecTests<TRAP::Math::Vec3d>();
        RunATanVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunATanVecTests<TRAP::Math::Vec3f>();
        RunATanVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunATanVecTests<TRAP::Math::Vec4d>();
        RunATanVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunATanVecTests<TRAP::Math::Vec4f>();
        RunATanVecEdgeTests<TRAP::Math::Vec4f>();
    }
}

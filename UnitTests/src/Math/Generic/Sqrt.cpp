#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunSqrtTests()
{
    static constexpr std::array<std::tuple<T, T>, 5> values
    {
        std::tuple(T(0.5f), T(0.707107f)),
        std::tuple(T(1.5f), T(1.22474f)),
        std::tuple(T(2.0f), T(1.41421f)),
        std::tuple(T(41.5f), T(6.44205f)),
        std::tuple(T(0.0f), T(0.0f)),
    };

    for(const auto& [x, expected] : values)
    {
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Sqrt(x), expected, T(0.00001f)));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunSqrtVecTests()
{
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(T(4.0f)), T(2.0f), T(0.01f))));
}

template<typename T>
requires TRAP::Math::IsQuat<T>
void RunSqrtQuatTests()
{
    static constexpr typename T::value_type Epsilon = typename T::value_type(0.001f);

    {
        static constexpr T x(typename T::value_type(1.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        static constexpr T res(typename T::value_type(1.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
    }
    {
        static constexpr T x(typename T::value_type(2.0f), typename T::value_type(3.0f), typename T::value_type(4.0f), typename T::value_type(5.0f));
        static constexpr T res(typename T::value_type(2.162f), typename T::value_type(0.693803f), typename T::value_type(0.92507f), typename T::value_type(1.15634f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
    }
    {
        static constexpr T x(typename T::value_type(-2.0f), typename T::value_type(-3.0f), typename T::value_type(-4.0f), typename T::value_type(-5.0f));
        static constexpr T res(typename T::value_type(1.63531f), typename T::value_type(-0.917258f), typename T::value_type(-1.22301f), typename T::value_type(-1.52876f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
    }
    {
        static constexpr T x(typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sqrt(x))));
    }
}

template<typename T>
requires std::floating_point<T>
void RunSqrtEdgeTests()
{
    static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    static constexpr T inf = std::numeric_limits<T>::infinity();
    static constexpr T ninf = -std::numeric_limits<T>::infinity();
    static constexpr T min = std::numeric_limits<T>::lowest();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sqrt(min)));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Sqrt(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sqrt(ninf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sqrt(nan)));
}

TEST_CASE("TRAP::Math::Sqrt()", "[math][generic][sqrt]")
{
    SECTION("Scalar - double")
    {
        RunSqrtTests<double>();
        RunSqrtEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunSqrtTests<float>();
        RunSqrtEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunSqrtVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunSqrtVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunSqrtVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunSqrtVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunSqrtVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunSqrtVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunSqrtQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunSqrtQuatTests<TRAP::Math::Quatf>();
    }
}
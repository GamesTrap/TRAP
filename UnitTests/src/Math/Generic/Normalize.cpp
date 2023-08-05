#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunNormalizeVecTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    const T normalize1 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f)));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize1, T(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f)), T(Epsilon))));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize1), typename T::valueType(1.0f), Epsilon));

    const T normalize2 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::valueType>(2.0f, 0.0f, 0.0f, 0.0f)));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize2, T(TRAP::Math::Vec<4, typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f)), T(Epsilon))));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize2), typename T::valueType(1.0f), Epsilon));

    const T normalize3 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::valueType>(-0.6f, 0.7f, -0.5f, 0.8f)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize3), typename T::valueType(1.0f), Epsilon));

    if constexpr(TRAP::Math::IsVec4<T>)
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize3, T(TRAP::Math::Vec<4, typename T::valueType>(-0.45485884017063427f, 0.53066861674163057f, -0.37904901841347383f, 0.60647843849879091f)), T(0.0000001f))));
    }
    else if constexpr(TRAP::Math::IsVec3<T>)
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize3, T(TRAP::Math::Vec<3, typename T::valueType>(-0.57207757317981289f, 0.66742379748924929f, -0.47673129203957787f)), T(0.0000001f))));
    }
    else if constexpr(TRAP::Math::IsVec2<T>)
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize3, T(TRAP::Math::Vec<2, typename T::valueType>(-0.65079139475254222f, 0.75925658411108998f)), T(0.0000001f))));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunNormalizeVecEdgeTests()
{
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    const T normalize1 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::valueType>(0.0f, 0.0f, 0.0f, 0.0f)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize1)));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize1)));

    const T normalize2 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::valueType>(inf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize2)));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize2)));

    const T normalize3 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::valueType>(ninf)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize3)));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize3)));

    const T normalize4 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::valueType>(nan)));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize4)));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize4)));
}

template<typename T>
requires TRAP::Math::IsQuat<T> && std::floating_point<typename T::valueType>
void RunNormalizeQuatTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();
    constexpr typename T::valueType PI = TRAP::Math::PI<typename T::valueType>();

    {
        constexpr T a(1.0f, 0.0f, 0.0f, 0.0f);
        const T n = TRAP::Math::Normalize(a);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(a, n, Epsilon)));
    }
    {
        constexpr T a(1.0f, TRAP::Math::Vec<3, typename T::valueType>(0.0f));
        const T n = TRAP::Math::Normalize(a);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(a, n, Epsilon)));
    }
    {
        const T q = TRAP::Math::AngleAxis(PI * typename T::valueType(0.25f), TRAP::Math::Vec<3, typename T::valueType>(0.0f, 0.0f, 1.0f));
        const T n = TRAP::Math::Normalize(q);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(n), typename T::valueType(1.0f), Epsilon));
    }
    {
        const T q = TRAP::Math::AngleAxis(PI * typename T::valueType(0.25f), TRAP::Math::Vec<3, typename T::valueType>(0.0f, 0.0f, 2.0f));
        const T n = TRAP::Math::Normalize(q);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(n), typename T::valueType(1.0f), Epsilon));
    }
    {
        const T q = TRAP::Math::AngleAxis(PI * typename T::valueType(0.25f), TRAP::Math::Vec<3, typename T::valueType>(1.0f, 2.0f, 3.0f));
        const T n = TRAP::Math::Normalize(q);
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(n), typename T::valueType(1.0f), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsQuat<T> && std::floating_point<typename T::valueType>
void RunNormalizeQuatEdgeTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();

    const T normalize1 = TRAP::Math::Normalize(T(0.0f, 0.0f, 0.0f, 0.0f));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize1), typename T::valueType(1.0f), Epsilon));
    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(T(1.0f, 0.0f, 0.0f, 0.0f), normalize1, Epsilon)));

    const T normalize2 = TRAP::Math::Normalize(T(inf, inf, inf, inf));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize2)));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize2)));

    const T normalize3 = TRAP::Math::Normalize(T(ninf, ninf, ninf, ninf));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize3)));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize3)));

    const T normalize4 = TRAP::Math::Normalize(T(nan, nan, nan, nan));
    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize4)));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize4)));
}

TEST_CASE("TRAP::Math::Normalize()", "[math][generic][normalize]")
{
    SECTION("Vec4 - double")
    {
        RunNormalizeVecTests<TRAP::Math::Vec4d>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunNormalizeVecTests<TRAP::Math::Vec4f>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Vec3 - double")
    {
        RunNormalizeVecTests<TRAP::Math::Vec3d>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunNormalizeVecTests<TRAP::Math::Vec3f>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec2 - double")
    {
        RunNormalizeVecTests<TRAP::Math::Vec2d>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunNormalizeVecTests<TRAP::Math::Vec2f>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Quat - double")
    {
        RunNormalizeQuatTests<TRAP::Math::Quatd>();
        RunNormalizeQuatEdgeTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunNormalizeQuatTests<TRAP::Math::Quatf>();
        RunNormalizeQuatEdgeTests<TRAP::Math::Quatf>();
    }
}
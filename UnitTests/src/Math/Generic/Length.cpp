#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires TRAP::Math::IsVec2<T> && std::floating_point<typename T::valueType>
void RunLengthVec2Tests()
{
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(2.0f, 3.0f)), typename T::valueType(3.6055512754639891f), typename T::valueType(0.000001f)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(-5.0f, -9.0f)), typename T::valueType(10.295630140987001f), typename T::valueType(0.000001f)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f)), typename T::valueType(0.0f), typename T::valueType(0.000001f)));
}

template<typename T>
requires TRAP::Math::IsVec3<T> && std::floating_point<typename T::valueType>
void RunLengthVec3Tests()
{
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(2.0f, 3.0f, 6.0f)), typename T::valueType(7.0f), typename T::valueType(0.000001f)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(-5.0f, -9.0f, -12.0f)), typename T::valueType(15.811388300841896f), typename T::valueType(0.000001f)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f, 0.0f)), typename T::valueType(0.0f), typename T::valueType(0.000001f)));
}

template<typename T>
requires TRAP::Math::IsVec4<T> && std::floating_point<typename T::valueType>
void RunLengthVec4Tests()
{
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(2.0f, 3.0f, 6.0f, 8.0f)), typename T::valueType(10.63014581273465f), typename T::valueType(0.000001f)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(-5.0f, -9.0f, -12.0f, -15.0f)), typename T::valueType(21.794494717703369f), typename T::valueType(0.000001f)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f, 0.0f, 0.0f)), typename T::valueType(0.0f), typename T::valueType(0.000001f)));
}


template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::valueType>
void RunLengthVecEdgeTests()
{
    constexpr typename T::valueType nan = std::numeric_limits<typename T::valueType>::quiet_NaN();
    constexpr typename T::valueType inf = std::numeric_limits<typename T::valueType>::infinity();
    constexpr typename T::valueType ninf = -std::numeric_limits<typename T::valueType>::infinity();

    REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(T(TRAP::Math::Vec<4, typename T::valueType>(nan, 1.0f, 2.0f, 3.0f)))));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Length(T(TRAP::Math::Vec<4, typename T::valueType>(inf, 4.0f, 6.0f, 8.0f)))));
    REQUIRE(TRAP::Math::IsInf(TRAP::Math::Length(T(TRAP::Math::Vec<4, typename T::valueType>(ninf, 4.0f, 6.0f, 8.0f)))));
}

template<typename T>
requires TRAP::Math::IsQuat<T>
void RunLengthQuatTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(1.0f, 2.0f, 3.0f, 4.0f)), typename T::valueType(5.4772255750516612f), typename T::valueType(0.000001f)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(-2.0f, -3.0f, -4.0f, -5.0f)), typename T::valueType(7.3484692283495345f), typename T::valueType(0.000001f)));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f, 0.0f, 0.0f)), typename T::valueType(0.0f), Epsilon));
}

TEST_CASE("TRAP::Math::Length()", "[math][generic][length]")
{
    SECTION("Vec2 - double")
    {
        RunLengthVec2Tests<TRAP::Math::Vec2d>();
        RunLengthVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunLengthVec2Tests<TRAP::Math::Vec2f>();
        RunLengthVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunLengthVec3Tests<TRAP::Math::Vec3d>();
        RunLengthVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunLengthVec3Tests<TRAP::Math::Vec3f>();
        RunLengthVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunLengthVec4Tests<TRAP::Math::Vec4d>();
        RunLengthVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunLengthVec4Tests<TRAP::Math::Vec4f>();
        RunLengthVecEdgeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunLengthQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunLengthQuatTests<TRAP::Math::Quatf>();
    }
}
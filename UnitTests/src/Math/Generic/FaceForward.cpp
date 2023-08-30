#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
consteval void RunCompileTimeFaceForwardVecTests()
{
    constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    {
        constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(-2.0f, 0.0f, 0.0f, 0.0f));
        constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
    }
    {
        constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
        constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 0.0f, 0.0f));
        constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
    }
    {
        constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
        constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
    }
    {
        constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
        constexpr T incident(normal);
        constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
    }
    {
        constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
        constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 3.0f, 4.0f));
        constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
    }
    {
        constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
        constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
    }
    {
        constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(2.0f, 0.0f, 0.0f, 0.0f));
        constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
void RunFaceForwardVecEdgeTests()
{
    constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
    constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
    constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

    static_assert(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::FaceForward(T(inf), T(ninf), T(inf)))));
    static_assert(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::FaceForward(T(ninf), T(inf), T(ninf)))));
    REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::FaceForward(T(nan), T(nan), T(nan)))));
}

TEST_CASE("TRAP::Math::FaceForward()", "[math][generic][faceforward]")
{
    SECTION("Vec4 - double")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec4d>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec4f>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Vec3 - double")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec3d>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec3f>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec2 - double")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec2d>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec2f>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec2f>();
    }
}
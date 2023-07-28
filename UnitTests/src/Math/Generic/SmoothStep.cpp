#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
consteval void RunCompileTimeSmoothStepTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static_assert(TRAP::Math::Equal(TRAP::Math::SmoothStep(T( 0.0f), T( 1.0f), T( 0.5f)), T(0.5f), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::SmoothStep(T( 2.0f), T( 5.0f), T( 1.0f)), T(0.0f), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::SmoothStep(T( 0.0f), T(10.0f), T(15.0f)), T(1.0f), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::SmoothStep(T(10.0f), T(20.0f), T(10.0f)), T(0.0f), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::SmoothStep(T( 5.0f), T( 8.0f), T( 8.0f)), T(1.0f), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::SmoothStep(T( 0.0f), T( 1.0f), T(-1.0f)), T(0.0f), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::SmoothStep(T( 0.0f), T( 1.0f), T( 2.0f)), T(1.0f), Epsilon));
}

template<typename EDGE, typename VEC>
requires (std::floating_point<EDGE> || (TRAP::Math::IsVec<EDGE> && std::floating_point<typename  EDGE::valueType>)) && TRAP::Math::IsVec<VEC>
consteval void RunCompileTimeSmoothStepVecTests()
{
    if constexpr(std::floating_point<EDGE>)
    {
        constexpr EDGE Epsilon = std::numeric_limits<EDGE>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(EDGE(1.0f), VEC(TRAP::Math::Vec<4, typename VEC::valueType>( 1.0f,  2.0f,  3.0f,  4.0f))), VEC(1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(EDGE(0.0f), VEC(TRAP::Math::Vec<4, typename VEC::valueType>( 1.0f,  2.0f,  3.0f,  4.0f))), VEC(1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(EDGE(0.0f), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(-1.0f, -2.0f, -3.0f, -4.0f))), VEC(0.0f), Epsilon)));
    }
    else if constexpr(TRAP::Math::IsVec<EDGE> && std::floating_point<typename EDGE::valueType>)
    {
        constexpr typename EDGE::valueType Epsilon = std::numeric_limits<typename EDGE::valueType>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(0.0f, 0.0f, 0.0f, 1.0f)), EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(1.0f, 1.0f, 1.0f, 2.0f)), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(0.5f, 0.5f, 0.5f, 1.5f))), VEC(0.5f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(1.0f, 1.0f, 1.0f, 1.0f)), EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(5.0f, 5.0f, 5.0f, 5.0f)), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(0.5f, 2.0f, 3.0f, 4.0f))), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(0.0f, 0.156250f, 0.5f, 0.843750f)), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(0.0f, 0.0f, 0.0f, 0.0f)), EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(10.0f, 10.0f, 10.0f, 10.0f)), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(15.0f, 11.0f, 12.0f, 10.5f))), VEC(1.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(10.0f, 10.0f, 10.0f, 10.0f)), EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(20.0f, 20.0f, 20.0f, 20.0f)), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(10.0f, 20.0f, 15.0f, 10.0f))), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(0.0f, 1.0f, 0.5f, 0.0f)), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(0.0f, 0.0f, 0.0f, 0.0f)), EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(1.0f, 1.0f, 1.0f, 1.0f)), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(-1.0f, 2.0f, 3.0f, 0.5f))), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(0.0f, 1.0f, 1.0f, 0.5f)), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SmoothStep(EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(0.0f, 0.0f, 0.0f, 0.0f)), EDGE(TRAP::Math::Vec<4, typename EDGE::valueType>(1.0f, 1.0f, 1.0f, 1.0f)), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(2.0f, 1.0f, 0.0f, 1.5f))), VEC(TRAP::Math::Vec<4, typename VEC::valueType>(1.0f, 1.0f, 0.0f, 1.0f)), Epsilon)));
    }
}

TEST_CASE("TRAP::Math::SmoothStep()", "[math][generic][smoothstep]")
{
    SECTION("Scalar - double")
    {
        RunCompileTimeSmoothStepTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeSmoothStepTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunCompileTimeSmoothStepVecTests<double, TRAP::Math::Vec2d>();
        RunCompileTimeSmoothStepVecTests<TRAP::Math::Vec2d, TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeSmoothStepVecTests<float, TRAP::Math::Vec2f>();
        RunCompileTimeSmoothStepVecTests<TRAP::Math::Vec2f, TRAP::Math::Vec2f>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeSmoothStepVecTests<double, TRAP::Math::Vec3d>();
        RunCompileTimeSmoothStepVecTests<TRAP::Math::Vec3d, TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeSmoothStepVecTests<float, TRAP::Math::Vec3f>();
        RunCompileTimeSmoothStepVecTests<TRAP::Math::Vec3f, TRAP::Math::Vec3f>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeSmoothStepVecTests<double, TRAP::Math::Vec4d>();
        RunCompileTimeSmoothStepVecTests<TRAP::Math::Vec4d, TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeSmoothStepVecTests<float, TRAP::Math::Vec4f>();
        RunCompileTimeSmoothStepVecTests<TRAP::Math::Vec4f, TRAP::Math::Vec4f>();
    }
}
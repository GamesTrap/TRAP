#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunCompileTimeStepTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr T edge = 2.0f;

        static_assert(TRAP::Math::Equal(TRAP::Math::Step(edge, T(1.0f)), T(0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Step(edge, T(3.0f)), T(1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Step(edge, T(2.0f)), T(1.0f), Epsilon));
    }

    template<typename EDGE, typename VEC>
    requires (std::floating_point<EDGE> || (TRAP::Math::IsVec<EDGE> && std::floating_point<typename  EDGE::value_type>)) && TRAP::Math::IsVec<VEC>
    consteval void RunStepVecTests()
    {
        if constexpr(std::floating_point<EDGE>)
        {
            constexpr EDGE Epsilon = std::numeric_limits<EDGE>::epsilon();

            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(EDGE(1.0f), VEC(TRAP::Math::Vec<4, typename VEC::value_type>( 1.0f,  2.0f,  3.0f,  4.0f))), VEC(1.0f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(EDGE(0.0f), VEC(TRAP::Math::Vec<4, typename VEC::value_type>( 1.0f,  2.0f,  3.0f,  4.0f))), VEC(1.0f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(EDGE(0.0f), VEC(TRAP::Math::Vec<4, typename VEC::value_type>(-1.0f, -2.0f, -3.0f, -4.0f))), VEC(0.0f), Epsilon)));
        }
        else if constexpr(TRAP::Math::IsVec<EDGE> && std::floating_point<typename EDGE::value_type>)
        {
            constexpr typename EDGE::value_type Epsilon = std::numeric_limits<typename EDGE::value_type>::epsilon();

            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(EDGE(TRAP::Math::Vec<4, typename EDGE::value_type>(-1.0f, -2.0f, -3.0f, -4.0f)), VEC(TRAP::Math::Vec<4, typename VEC::value_type>(-2.0f, -3.0f, -4.0f, -5.0f))), VEC(0.0f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(EDGE(TRAP::Math::Vec<4, typename EDGE::value_type>( 0.0f,  1.0f,  2.0f,  3.0f)), VEC(TRAP::Math::Vec<4, typename VEC::value_type>( 1.0f,  2.0f,  3.0f,  4.0f))), VEC(1.0f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(EDGE(TRAP::Math::Vec<4, typename EDGE::value_type>( 2.0f,  3.0f,  4.0f,  5.0f)), VEC(TRAP::Math::Vec<4, typename VEC::value_type>( 1.0f,  2.0f,  3.0f,  4.0f))), VEC(0.0f), Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Step(EDGE(TRAP::Math::Vec<4, typename EDGE::value_type>( 0.0f,  1.0f,  2.0f,  3.0f)), VEC(TRAP::Math::Vec<4, typename VEC::value_type>(-1.0f, -2.0f, -3.0f, -4.0f))), VEC(0.0f), Epsilon)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunCompileTimeStepEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T min = std::numeric_limits<T>::denorm_min();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T nan = -std::numeric_limits<T>::quiet_NaN();

        static_assert(TRAP::Math::Equal(TRAP::Math::Step(T( 0.0f), min), T(1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Step(T(-1.0f), min), T(1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Step(T( 1.0f), min), T(0.0f), Epsilon));

        static_assert(TRAP::Math::Equal(TRAP::Math::Step(T( 0.0f), inf), T(1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Step(T( 0.0f), ninf), T(0.0f), Epsilon));

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(T(-1.0f), nan), T(1.0f), Epsilon));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Step(T( 1.0f), nan), T(1.0f), Epsilon));
    }
}

TEST_CASE("TRAP::Math::Step()", "[math][generic][step]")
{
    SECTION("Scalar - double")
    {
        RunCompileTimeStepTests<double>();
        RunCompileTimeStepEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeStepTests<float>();
        RunCompileTimeStepEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunStepVecTests<double, TRAP::Math::Vec2d>();
        RunStepVecTests<TRAP::Math::Vec2d, TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunStepVecTests<float, TRAP::Math::Vec2f>();
        RunStepVecTests<TRAP::Math::Vec2f, TRAP::Math::Vec2f>();
    }
    SECTION("Vec3 - double")
    {
        RunStepVecTests<double, TRAP::Math::Vec3d>();
        RunStepVecTests<TRAP::Math::Vec3d, TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunStepVecTests<float, TRAP::Math::Vec3f>();
        RunStepVecTests<TRAP::Math::Vec3f, TRAP::Math::Vec3f>();
    }
    SECTION("Vec4 - double")
    {
        RunStepVecTests<double, TRAP::Math::Vec4d>();
        RunStepVecTests<TRAP::Math::Vec4d, TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunStepVecTests<float, TRAP::Math::Vec4f>();
        RunStepVecTests<TRAP::Math::Vec4f, TRAP::Math::Vec4f>();
    }
}
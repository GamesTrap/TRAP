#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunCompileTimeIsFiniteTests()
    {
        static_assert(TRAP::Math::IsFinite(T(1.0)));
        static_assert(TRAP::Math::IsFinite(T(-1.0)));
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunCompileTimeIsEvenEdgeTests()
    {
        constexpr T min = std::numeric_limits<T>::min();
        constexpr T max = std::numeric_limits<T>::max();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();
        constexpr T nan = -std::numeric_limits<T>::quiet_NaN();

        static_assert( TRAP::Math::IsFinite(min));
        static_assert( TRAP::Math::IsFinite(max));
        static_assert(!TRAP::Math::IsFinite(inf));
        static_assert(!TRAP::Math::IsFinite(ninf));
        static_assert(!TRAP::Math::IsFinite(nan));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunCompileTimeIsFiniteVecTests()
    {
        static_assert(TRAP::Math::All(TRAP::Math::IsFinite(T(typename T::value_type(1.0)))));
        static_assert(TRAP::Math::All(TRAP::Math::IsFinite(T(typename T::value_type(-1.0)))));
    }
}

TEST_CASE("TRAP::Math::IsFinite()", "[math][generic][isfinite]")
{
    SECTION("Scalar - float")
    {
        RunCompileTimeIsEvenEdgeTests<float>();
        RunCompileTimeIsFiniteTests<float>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeIsEvenEdgeTests<double>();
        RunCompileTimeIsFiniteTests<double>();
    }

    SECTION("Vec2 - float")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec2f>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec2d>();
    }

    SECTION("Vec3 - float")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec3f>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec3d>();
    }

    SECTION("Vec4 - float")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec4f>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec4d>();
    }
}
#include <limits>

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
    SECTION("Scalar - f32")
    {
        RunCompileTimeIsEvenEdgeTests<f32>();
        RunCompileTimeIsFiniteTests<f32>();
    }
    SECTION("Scalar - f64")
    {
        RunCompileTimeIsEvenEdgeTests<f64>();
        RunCompileTimeIsFiniteTests<f64>();
    }

    SECTION("Vec2 - f32")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec2f>();
    }
    SECTION("Vec2 - f64")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec2d>();
    }

    SECTION("Vec3 - f32")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec3f>();
    }
    SECTION("Vec3 - f64")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec3d>();
    }

    SECTION("Vec4 - f32")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec4f>();
    }
    SECTION("Vec4 - f64")
    {
        RunCompileTimeIsFiniteVecTests<TRAP::Math::Vec4d>();
    }
}
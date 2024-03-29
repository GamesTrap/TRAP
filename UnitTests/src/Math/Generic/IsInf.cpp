#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires (std::floating_point<T> ||
            (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>) ||
            (TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>))
    consteval void RunCompileTimeIsInfTest()
    {
        if constexpr(std::floating_point<T>)
        {
            constexpr T inf = std::numeric_limits<T>::infinity();
            static_assert(TRAP::Math::IsInf(inf));
            static_assert(TRAP::Math::IsInf(-inf));
        }
        else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
        {
            constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
            static_assert(TRAP::Math::Any(TRAP::Math::IsInf(T(inf))));
            static_assert(TRAP::Math::Any(TRAP::Math::IsInf(T(-inf))));
        }
        else if constexpr(TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>)
        {
            constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
            static_assert(TRAP::Math::Any(TRAP::Math::IsInf(T(inf, inf, inf, inf))));
            static_assert(TRAP::Math::Any(TRAP::Math::IsInf(T(-inf, -inf, -inf, -inf))));
        }
    }
}

TEST_CASE("TRAP::Math::IsInf()", "[math][generic][isinf]")
{
    SECTION("Scalar - f64")
    {
        RunCompileTimeIsInfTest<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeIsInfTest<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunCompileTimeIsInfTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeIsInfTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunCompileTimeIsInfTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeIsInfTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunCompileTimeIsInfTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeIsInfTest<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - f64")
    {
        RunCompileTimeIsInfTest<TRAP::Math::Quatd>();
    }
    SECTION("Quat - f32")
    {
        RunCompileTimeIsInfTest<TRAP::Math::Quatf>();
    }
}
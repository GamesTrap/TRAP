#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires (std::floating_point<T> ||
            (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>) ||
            (TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>))
    consteval void RunCompileTimeIsNaNTest()
    {
        if constexpr(std::floating_point<T>)
        {
            constexpr T nan = std::numeric_limits<T>::quiet_NaN();
            static_assert(TRAP::Math::IsNaN(nan));
            static_assert(TRAP::Math::IsNaN(-nan));
        }
        else if constexpr(TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>)
        {
            constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
            static_assert(TRAP::Math::Any(TRAP::Math::IsNaN(T(nan))));
            static_assert(TRAP::Math::Any(TRAP::Math::IsNaN(T(-nan))));
        }
        else if constexpr(TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>)
        {
            constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
            static_assert(TRAP::Math::Any(TRAP::Math::IsNaN(T(nan, nan, nan, nan))));
            static_assert(TRAP::Math::Any(TRAP::Math::IsNaN(T(-nan, -nan, -nan, -nan))));
        }
    }
}

TEST_CASE("TRAP::Math::IsNaN()", "[math][generic][isnan]")
{
    SECTION("Scalar - f64")
    {
        RunCompileTimeIsNaNTest<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeIsNaNTest<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - f64")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Quatd>();
    }
    SECTION("Quat - f32")
    {
        RunCompileTimeIsNaNTest<TRAP::Math::Quatf>();
    }
}
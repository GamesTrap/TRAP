#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunCompileTimeDegreesScalarTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(T(0.0f))), T(0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(T(45.0f))), T(45.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(T(90.0f))), T(90.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(T(180.0f))), T(180.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(T(270.0f))), T(270.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(T(-45.0f))), T(-45.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(T(-90.0f))), T(-90.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(T(-180.0f))), T(-180.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(T(-270.0f))), T(-270.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(T(360.0f))), T(360.0f), Epsilon));
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunCompileTimeDegreesScalarEdgeTests()
    {
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();

        static_assert(TRAP::Math::IsInf(inf));
        static_assert(TRAP::Math::IsInf(ninf));
        static_assert(TRAP::Math::IsNaN(nan));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunCompileTimeDegreesVecTests()
    {
        {
            constexpr T degrees(TRAP::Math::tVec4<typename T::value_type>(0.0f, 45.0f, 90.0f, 180.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(degrees)), degrees, typename T::value_type(0.00001f))));
        }
        {
            constexpr T degrees(TRAP::Math::tVec4<typename T::value_type>(270.0f, -45.0f, -90.0f, -180.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(degrees)), degrees, typename T::value_type(0.00001f))));
        }
        {
            constexpr T degrees(TRAP::Math::tVec4<typename T::value_type>(-270.0f, 360.0f, 85.0f, -99.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Radians(degrees)), degrees, typename T::value_type(0.00001f))));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunCompileTimeDegreesVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

        {
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::Degrees(T(inf)))));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::Degrees(T(ninf)))));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Degrees(T(nan)))));
        }
    }
}

TEST_CASE("TRAP::Math::Degrees()", "[math][generic][degrees]")
{
    SECTION("Scalar - f64")
    {
        RunCompileTimeDegreesScalarTests<f64>();
        RunCompileTimeDegreesScalarEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeDegreesScalarTests<f32>();
        RunCompileTimeDegreesScalarEdgeTests<f32>();
    }

    SECTION("Vec2 - f64")
    {
        RunCompileTimeDegreesVecTests<TRAP::Math::Vec2d>();
        RunCompileTimeDegreesVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeDegreesVecTests<TRAP::Math::Vec2f>();
        RunCompileTimeDegreesVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunCompileTimeDegreesVecTests<TRAP::Math::Vec3d>();
        RunCompileTimeDegreesVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeDegreesVecTests<TRAP::Math::Vec3f>();
        RunCompileTimeDegreesVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunCompileTimeDegreesVecTests<TRAP::Math::Vec4d>();
        RunCompileTimeDegreesVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeDegreesVecTests<TRAP::Math::Vec4f>();
        RunCompileTimeDegreesVecEdgeTests<TRAP::Math::Vec4f>();
    }
}
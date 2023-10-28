#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunRefractVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr typename T::value_type eta = 1.5f;
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(-1.0f, 0.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr typename T::value_type eta = 1.5f;
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 1.0f, 0.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 1.0f, 0.0f, 0.0f));
            constexpr typename T::value_type eta = 1.5f;
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(-4.2838823f, -4.2838823f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, typename T::value_type(0.0000001f))));
        }
        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(-1.0f, 1.0f, 0.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(-1.0f, -1.0f, 0.0f, 0.0f));
            constexpr typename T::value_type eta = 1.5f;
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 1.0f, 0.0f));
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr typename T::value_type eta = 1.5f;
            constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunRefractVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            static constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(-1.0f, 0.0f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            static constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 1.0f, 0.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 1.0f, 0.0f, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            static constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(-4.2838823f, -4.2838823f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, typename T::value_type(0.0000001f))));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(-1.0f, 1.0f, 0.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(-1.0f, -1.0f, 0.0f, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            static constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 1.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            static constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunRefractVecEdgeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
        static constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::lowest();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(min, min, min, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 1.0f, 1.0f, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(max, max, max, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 1.0f, 1.0f, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(inf, inf, inf, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 1.0f, 1.0f, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(ninf, ninf, ninf, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 1.0f, 1.0f, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Refract(incident, normal, eta))));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(nan, nan, nan, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 1.0f, 1.0f, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            static constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
        {
            static constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 3.0f, 0.0f));
            static constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(nan, nan, nan, 0.0f));
            static constexpr typename T::value_type eta = 1.5f;
            static constexpr T expected(TRAP::Math::Vec<4, typename T::value_type>(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Refract(incident, normal, eta), expected, Epsilon)));
        }
    }
}

TEST_CASE("TRAP::Math::Refract()", "[math][generic][refract]")
{
    SECTION("Vec4 - f64")
    {
        RunRefractVecRunTimeTests<TRAP::Math::Vec4d>();
        RunRefractVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunRefractVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunRefractVecRunTimeTests<TRAP::Math::Vec4f>();
        RunRefractVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunRefractVecEdgeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Vec3 - f64")
    {
        RunRefractVecRunTimeTests<TRAP::Math::Vec3d>();
        RunRefractVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunRefractVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunRefractVecRunTimeTests<TRAP::Math::Vec3f>();
        RunRefractVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunRefractVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec2 - f64")
    {
        RunRefractVecRunTimeTests<TRAP::Math::Vec2d>();
        RunRefractVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunRefractVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunRefractVecRunTimeTests<TRAP::Math::Vec2f>();
        RunRefractVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunRefractVecEdgeTests<TRAP::Math::Vec2f>();
    }
}
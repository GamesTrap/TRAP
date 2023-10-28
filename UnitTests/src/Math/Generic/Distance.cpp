#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec2<T> && std::floating_point<typename T::value_type>
    consteval void RunDistanceVec2CompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            constexpr T p0(1.0f, 0.0f);
            constexpr typename T::value_type expected(0.0f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            constexpr T p0(1.0f, 2.0f);
            constexpr T p1(5.0f, 6.0f);
            constexpr typename T::value_type expected(5.6568542494923806f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
        }
        {
            constexpr T p0(0.0f, 0.0f);
            constexpr typename T::value_type expected(0.0f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            constexpr T p0(1.0f, 2.0f);
            constexpr T p1(-1.0f, -2.0f);
            constexpr typename T::value_type expected(4.4721359549995796f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec2<T> && std::floating_point<typename T::value_type>
    void RunDistanceVec2RunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            static constexpr T p0(1.0f, 0.0f);
            static constexpr typename T::value_type expected(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            static constexpr T p0(1.0f, 2.0f);
            static constexpr T p1(5.0f, 6.0f);
            static constexpr typename T::value_type expected(5.6568542494923806f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
        }
        {
            static constexpr T p0(0.0f, 0.0f);
            static constexpr typename T::value_type expected(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            static constexpr T p0(1.0f, 2.0f);
            static constexpr T p1(-1.0f, -2.0f);
            static constexpr typename T::value_type expected(4.4721359549995796f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec3<T> && std::floating_point<typename T::value_type>
    consteval void RunDistanceVec3CompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            constexpr T p0(1.0f, 0.0f, 0.0f);
            constexpr typename T::value_type expected(0.0f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            constexpr T p0(1.0f, 2.0f, 3.0f);
            constexpr T p1(5.0f, 6.0f, 7.0f);
            constexpr typename T::value_type expected(6.9282032302755088f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
        }
        {
            constexpr T p0(0.0f, 0.0f, 0.0f);
            constexpr typename T::value_type expected(0.0f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            constexpr T p0(1.0f, 2.0f, 3.0f);
            constexpr T p1(-1.0f, -2.0f, -3.0f);
            constexpr typename T::value_type expected(7.48331499f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec3<T> && std::floating_point<typename T::value_type>
    void RunDistanceVec3RunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            static constexpr T p0(1.0f, 0.0f, 0.0f);
            static constexpr typename T::value_type expected(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            static constexpr T p0(1.0f, 2.0f, 3.0f);
            static constexpr T p1(5.0f, 6.0f, 7.0f);
            static constexpr typename T::value_type expected(6.9282032302755088f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
        }
        {
            static constexpr T p0(0.0f, 0.0f, 0.0f);
            static constexpr typename T::value_type expected(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            static constexpr T p0(1.0f, 2.0f, 3.0f);
            static constexpr T p1(-1.0f, -2.0f, -3.0f);
            static constexpr typename T::value_type expected(7.48331499f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec4<T> && std::floating_point<typename T::value_type>
    consteval void RunDistanceVec4CompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            constexpr T p0(1.0f, 0.0f, 0.0f, 0.0f);
            constexpr typename T::value_type expected(0.0f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            constexpr T p0(1.0f, 2.0f, 3.0f, 4.0f);
            constexpr T p1(5.0f, 6.0f, 7.0f, 8.0f);
            constexpr typename T::value_type expected(8.0f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Epsilon));
        }
        {
            constexpr T p0(0.0f, 0.0f, 0.0f, 0.0f);
            constexpr typename T::value_type expected(0.0f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            constexpr T p0(1.0f, 2.0f, 3.0f, 4.0f);
            constexpr T p1(-1.0f, -2.0f, -3.0f, -4.0f);
            constexpr typename T::value_type expected(10.954451150103322f);
            static_assert(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec4<T> && std::floating_point<typename T::value_type>
    void RunDistanceVec4RunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            static constexpr T p0(1.0f, 0.0f, 0.0f, 0.0f);
            static constexpr typename T::value_type expected(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            static constexpr T p0(1.0f, 2.0f, 3.0f, 4.0f);
            static constexpr T p1(5.0f, 6.0f, 7.0f, 8.0f);
            static constexpr typename T::value_type expected(8.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Epsilon));
        }
        {
            static constexpr T p0(0.0f, 0.0f, 0.0f, 0.0f);
            static constexpr typename T::value_type expected(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p0), expected, Epsilon));
        }
        {
            static constexpr T p0(1.0f, 2.0f, 3.0f, 4.0f);
            static constexpr T p1(-1.0f, -2.0f, -3.0f, -4.0f);
            static constexpr typename T::value_type expected(10.954451150103322f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, typename T::value_type(0.000001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunDistanceVecEdgeRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
        static constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::min();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

        {
            static constexpr T p0(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr T p1(TRAP::Math::Vec<4, typename T::value_type>(max, 0.0f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Distance(p0, p1)));
        }
        {
            static constexpr T p0(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr T p1(TRAP::Math::Vec<4, typename T::value_type>(min, 0.0f, 0.0f, 0.0f));
            static constexpr typename T::value_type expected(0.0f);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Distance(p0, p1), expected, Epsilon));
        }
        {
            static constexpr T p0(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr T p1(TRAP::Math::Vec<4, typename T::value_type>(nan, 0.0f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Distance(p0, p1)));
        }
        {
            static constexpr T p0(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr T p1(TRAP::Math::Vec<4, typename T::value_type>(inf, 0.0f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Distance(p0, p1)));
        }
        {
            static constexpr T p0(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            static constexpr T p1(TRAP::Math::Vec<4, typename T::value_type>(ninf, 0.0f, 0.0f, 0.0f));
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Distance(p0, p1)));
        }
    }
}

TEST_CASE("TRAP::Math::Distance()", "[math][generic][distance]")
{
    SECTION("Vec2 - f64")
    {
        RunDistanceVec2RunTimeTests<TRAP::Math::Vec2d>();
        RunDistanceVec2CompileTimeTests<TRAP::Math::Vec2d>();
        RunDistanceVecEdgeRunTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunDistanceVec2RunTimeTests<TRAP::Math::Vec2f>();
        RunDistanceVec2CompileTimeTests<TRAP::Math::Vec2f>();
        RunDistanceVecEdgeRunTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunDistanceVec3RunTimeTests<TRAP::Math::Vec3d>();
        RunDistanceVec3CompileTimeTests<TRAP::Math::Vec3d>();
        RunDistanceVecEdgeRunTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunDistanceVec3RunTimeTests<TRAP::Math::Vec3f>();
        RunDistanceVec3CompileTimeTests<TRAP::Math::Vec3f>();
        RunDistanceVecEdgeRunTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunDistanceVec4RunTimeTests<TRAP::Math::Vec4d>();
        RunDistanceVec4CompileTimeTests<TRAP::Math::Vec4d>();
        RunDistanceVecEdgeRunTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunDistanceVec4RunTimeTests<TRAP::Math::Vec4f>();
        RunDistanceVec4CompileTimeTests<TRAP::Math::Vec4f>();
        RunDistanceVecEdgeRunTimeTests<TRAP::Math::Vec4f>();
    }
}
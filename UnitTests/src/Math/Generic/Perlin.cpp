#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunPerlinCompileTimeTests()
    {
        constexpr typename T::value_type epsilon = TRAP::Math::Epsilon<typename T::value_type>();

        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Perlin(T{}), typename T::value_type(), epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Perlin(T{std::numeric_limits<typename T::value_type>::min()}), typename T::value_type(), epsilon));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{TRAP::Math::Vec<4, typename T::value_type>{10.0f, 20.0f, 30.0f, 40.0f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{-TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{-TRAP::Math::Vec<4, typename T::value_type>{10.0f, 20.0f, 30.0f, 40.0f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{TRAP::Math::Vec<4, typename T::value_type>{0.0f, 5.0f, 10.0f, 15.0f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{TRAP::Math::Vec<4, typename T::value_type>{1.0f, 0.0f, 10.0f, 2.0f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{TRAP::Math::Vec<4, typename T::value_type>{1.0f, 5.0f, 0.0f, 2.0f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        if constexpr(T::Length() == 2)
        {
            constexpr auto test = TRAP::Math::Perlin(T{0.2, 0.3});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0.232801), typename T::value_type(0.000001f)));
            constexpr auto test2 = TRAP::Math::Perlin(-T{0.2, 0.3});
            static_assert(TRAP::Math::Equal(test2, typename T::value_type(-0.272289), typename T::value_type(0.000001f)));
        }
        else if constexpr(T::Length() == 3)
        {
            constexpr auto test = TRAP::Math::Perlin(T{0.2, 0.3, 0.4});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(-0.46676), typename T::value_type(0.01f)));
            constexpr auto test2 = TRAP::Math::Perlin(-T{0.2, 0.3, 0.4});
            static_assert(TRAP::Math::Equal(test2, typename T::value_type(0.264326), typename T::value_type(0.01f)));
        }
        else if constexpr(T::Length() == 4)
        {
            constexpr auto test = TRAP::Math::Perlin(T{0.2, 0.3, 0.4, 0.5});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(-0.437573), typename T::value_type(0.000001f)));
            constexpr auto test2 = TRAP::Math::Perlin(-T{0.2, 0.3, 0.4, 0.5});
            static_assert(TRAP::Math::Equal(test2, typename T::value_type(0.59862), typename T::value_type(0.000001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunPerlinPeriodicCompileTimeTests()
    {
        constexpr typename T::value_type epsilon = TRAP::Math::Epsilon<typename T::value_type>();

        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Perlin(T{}, T(1)), typename T::value_type(), epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Perlin(T{std::numeric_limits<typename T::value_type>::min()}), typename T::value_type(), epsilon));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}}, T{TRAP::Math::Vec<4, typename T::value_type>{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{TRAP::Math::Vec<4, typename T::value_type>{10.0f, 20.0f, 30.0f, 40.0f}}, T{TRAP::Math::Vec<4, typename T::value_type>{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{-TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}}, T{TRAP::Math::Vec<4, typename T::value_type>{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{-TRAP::Math::Vec<4, typename T::value_type>{10.0f, 20.0f, 30.0f, 40.0f}}, T{TRAP::Math::Vec<4, typename T::value_type>{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{TRAP::Math::Vec<4, typename T::value_type>{0.0f, 5.0f, 10.0f, 15.0f}}, T{TRAP::Math::Vec<4, typename T::value_type>{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{TRAP::Math::Vec<4, typename T::value_type>{1.0f, 0.0f, 10.0f, 2.0f}}, T{TRAP::Math::Vec<4, typename T::value_type>{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        {
            constexpr auto test = TRAP::Math::Perlin(T{TRAP::Math::Vec<4, typename T::value_type>{1.0f, 5.0f, 0.0f, 2.0f}}, T{TRAP::Math::Vec<4, typename T::value_type>{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0)));
        }
        if constexpr(T::Length() == 2)
        {
            constexpr auto test = TRAP::Math::Perlin(T{0.2, 0.3}, T{0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(0.248691), typename T::value_type(0.000001f)));
            constexpr auto test2 = TRAP::Math::Perlin(-T{0.2, 0.3}, T{0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test2, typename T::value_type(-0.248691), typename T::value_type(0.000001f)));
        }
        else if constexpr(T::Length() == 3)
        {
            constexpr auto test = TRAP::Math::Perlin(T{0.2, 0.3, 0.4}, T{0.5f, 0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(-0.246843), typename T::value_type(0.000001f)));
            constexpr auto test2 = TRAP::Math::Perlin(-T{0.2, 0.3, 0.4}, T{0.5f, 0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test2, typename T::value_type(0.246843), typename T::value_type(0.000001f)));
        }
        else if constexpr(T::Length() == 4)
        {
            constexpr auto test = TRAP::Math::Perlin(T{0.2, 0.3, 0.4, 0.5}, T{0.5f, 0.5f, 0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test, typename T::value_type(-0.0998235), typename T::value_type(0.0000001f)));
            constexpr auto test2 = TRAP::Math::Perlin(-T{0.2, 0.3, 0.4, 0.5}, T{0.5f, 0.5f, 0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test2, typename T::value_type(0.0998234), typename T::value_type(0.0000001f)));
        }
    }
}

TEST_CASE("TRAP::Math::Perlin(Vec)", "[math][generic][perlin]")
{
    SECTION("Vec2 - f32")
    {
        RunPerlinCompileTimeTests<TRAP::Math::Vec2f>();
    }
    SECTION("Vec2 - f64")
    {
        RunPerlinCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec3 - f32")
    {
        RunPerlinCompileTimeTests<TRAP::Math::Vec3f>();
    }
    SECTION("Vec3 - f64")
    {
        RunPerlinCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec4 - f32")
    {
        RunPerlinCompileTimeTests<TRAP::Math::Vec4f>();
    }
    SECTION("Vec4 - f64")
    {
        RunPerlinCompileTimeTests<TRAP::Math::Vec4d>();
    }
}

TEST_CASE("TRAP::Math::Perlin(Vec, Vec)", "[math][generic][perlin]")
{
    SECTION("Vec2 - f32")
    {
        RunPerlinPeriodicCompileTimeTests<TRAP::Math::Vec2f>();
    }
    SECTION("Vec2 - f64")
    {
        RunPerlinPeriodicCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec3 - f32")
    {
        RunPerlinPeriodicCompileTimeTests<TRAP::Math::Vec3f>();
    }
    SECTION("Vec3 - f64")
    {
        RunPerlinPeriodicCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec4 - f32")
    {
        RunPerlinPeriodicCompileTimeTests<TRAP::Math::Vec4f>();
    }
    SECTION("Vec4 - f64")
    {
        RunPerlinPeriodicCompileTimeTests<TRAP::Math::Vec4d>();
    }
}

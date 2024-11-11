#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Perlin(Vec)", "[math][generic][perlin][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar epsilon = TRAP::Math::Epsilon<Scalar>();

        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Perlin(TestType{}), Scalar(), epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Perlin(TestType{std::numeric_limits<Scalar>::min()}), Scalar(), epsilon));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{-Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{0.0f, 5.0f, 10.0f, 15.0f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 0.0f, 10.0f, 2.0f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 5.0f, 0.0f, 2.0f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        if constexpr(TestType::Length() == 2u)
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{0.2, 0.3});
            static_assert(TRAP::Math::Equal(test, Scalar(0.232801), Scalar(0.000001f)));
            static constexpr auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3});
            static_assert(TRAP::Math::Equal(test2, Scalar(-0.272289), Scalar(0.000001f)));
        }
        else if constexpr(TestType::Length() == 3u)
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{0.2, 0.3, 0.4});
            static_assert(TRAP::Math::Equal(test, Scalar(-0.46676), Scalar(0.01f)));
            static constexpr auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3, 0.4});
            static_assert(TRAP::Math::Equal(test2, Scalar(0.264326), Scalar(0.01f)));
        }
        else if constexpr(TestType::Length() == 4u)
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{0.2, 0.3, 0.4, 0.5});
            static_assert(TRAP::Math::Equal(test, Scalar(-0.437573), Scalar(0.000001f)));
            static constexpr auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3, 0.4, 0.5});
            static_assert(TRAP::Math::Equal(test2, Scalar(0.59862), Scalar(0.000001f)));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar epsilon = TRAP::Math::Epsilon<Scalar>();

        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Perlin(TestType{}), Scalar(), epsilon));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Perlin(TestType{std::numeric_limits<Scalar>::min()}), Scalar(), epsilon));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{-Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{0.0f, 5.0f, 10.0f, 15.0f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 0.0f, 10.0f, 2.0f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 5.0f, 0.0f, 2.0f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        if constexpr(TestType::Length() == 2u)
        {
            const auto test = TRAP::Math::Perlin(TestType{0.2, 0.3});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0.232801), Scalar(0.000001f)));
            const auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3});
            REQUIRE(TRAP::Math::Equal(test2, Scalar(-0.272289), Scalar(0.000001f)));
        }
        else if constexpr(TestType::Length() == 3u)
        {
            const auto test = TRAP::Math::Perlin(TestType{0.2, 0.3, 0.4});
            REQUIRE(TRAP::Math::Equal(test, Scalar(-0.46676), Scalar(0.01f)));
            const auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3, 0.4});
            REQUIRE(TRAP::Math::Equal(test2, Scalar(0.264326), Scalar(0.01f)));
        }
        else if constexpr(TestType::Length() == 4u)
        {
            const auto test = TRAP::Math::Perlin(TestType{0.2, 0.3, 0.4, 0.5});
            REQUIRE(TRAP::Math::Equal(test, Scalar(-0.437573), Scalar(0.000001f)));
            const auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3, 0.4, 0.5});
            REQUIRE(TRAP::Math::Equal(test2, Scalar(0.59862), Scalar(0.000001f)));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Perlin(Vec, Vec)", "[math][generic][perlin][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar epsilon = TRAP::Math::Epsilon<Scalar>();

        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Perlin(TestType{}, TestType(1)), Scalar(), epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Perlin(TestType{std::numeric_limits<Scalar>::min()}), Scalar(), epsilon));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{-Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{0.0f, 5.0f, 10.0f, 15.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 0.0f, 10.0f, 2.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 5.0f, 0.0f, 2.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            static_assert(TRAP::Math::Equal(test, Scalar(0)));
        }
        if constexpr(TestType::Length() == 2u)
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{0.2, 0.3}, TestType{0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test, Scalar(0.248691), Scalar(0.000001f)));
            static constexpr auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3}, TestType{0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test2, Scalar(-0.248691), Scalar(0.000001f)));
        }
        else if constexpr(TestType::Length() == 3u)
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{0.2, 0.3, 0.4}, TestType{0.5f, 0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test, Scalar(-0.246843), Scalar(0.000001f)));
            static constexpr auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3, 0.4}, TestType{0.5f, 0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test2, Scalar(0.246843), Scalar(0.000001f)));
        }
        else if constexpr(TestType::Length() == 4u)
        {
            static constexpr auto test = TRAP::Math::Perlin(TestType{0.2, 0.3, 0.4, 0.5}, TestType{0.5f, 0.5f, 0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test, Scalar(-0.0998235), Scalar(0.0000001f)));
            static constexpr auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3, 0.4, 0.5}, TestType{0.5f, 0.5f, 0.5f, 0.5f});
            static_assert(TRAP::Math::Equal(test2, Scalar(0.0998234), Scalar(0.0000001f)));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar epsilon = TRAP::Math::Epsilon<Scalar>();

        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Perlin(TestType{}, TestType(1)), Scalar(), epsilon));
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Perlin(TestType{std::numeric_limits<Scalar>::min()}), Scalar(), epsilon));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{-Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{0.0f, 5.0f, 10.0f, 15.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 0.0f, 10.0f, 2.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        {
            const auto test = TRAP::Math::Perlin(TestType{Vec4Scalar{1.0f, 5.0f, 0.0f, 2.0f}}, TestType{Vec4Scalar{0.5f, 0.5f, 0.5f, 0.5f}});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0)));
        }
        if constexpr(TestType::Length() == 2u)
        {
            const auto test = TRAP::Math::Perlin(TestType{0.2, 0.3}, TestType{0.5f, 0.5f});
            REQUIRE(TRAP::Math::Equal(test, Scalar(0.248691), Scalar(0.000001f)));
            const auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3}, TestType{0.5f, 0.5f});
            REQUIRE(TRAP::Math::Equal(test2, Scalar(-0.248691), Scalar(0.000001f)));
        }
        else if constexpr(TestType::Length() == 3u)
        {
            const auto test = TRAP::Math::Perlin(TestType{0.2, 0.3, 0.4}, TestType{0.5f, 0.5f, 0.5f});
            REQUIRE(TRAP::Math::Equal(test, Scalar(-0.246843), Scalar(0.000001f)));
            const auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3, 0.4}, TestType{0.5f, 0.5f, 0.5f});
            REQUIRE(TRAP::Math::Equal(test2, Scalar(0.246843), Scalar(0.000001f)));
        }
        else if constexpr(TestType::Length() == 4u)
        {
            const auto test = TRAP::Math::Perlin(TestType{0.2, 0.3, 0.4, 0.5}, TestType{0.5f, 0.5f, 0.5f, 0.5f});
            REQUIRE(TRAP::Math::Equal(test, Scalar(-0.0998235), Scalar(0.0000001f)));
            const auto test2 = TRAP::Math::Perlin(-TestType{0.2, 0.3, 0.4, 0.5}, TestType{0.5f, 0.5f, 0.5f, 0.5f});
            REQUIRE(TRAP::Math::Equal(test2, Scalar(0.0998234), Scalar(0.0000001f)));
        }
    }
}

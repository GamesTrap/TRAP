#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Simplex()", "[math][generic][simplex][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar epsilon = TRAP::Math::Epsilon<Scalar>();

        if constexpr(TestType::Length() == 2u)
        {
            {
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{}), Scalar(), epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{std::numeric_limits<Scalar>::min()}), Scalar(), epsilon));
            }
            {
                static constexpr auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                static_assert(TRAP::Math::Equal(test, Scalar(0.368338), Scalar(0.000001f)));
            }
            {
                static constexpr auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
                static_assert(TRAP::Math::Equal(test, Scalar(0.0649866), Scalar(0.00001f)));
            }
            {
                static constexpr auto test = TRAP::Math::Simplex(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                static_assert(TRAP::Math::Equal(test, Scalar(-0.151068), Scalar(0.00001f)));
            }
            {
                static constexpr auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{0.0f, 5.0f, 10.0f, 15.0f}});
                static_assert(TRAP::Math::Equal(test, Scalar(0.558613), Scalar(0.00001f)));
            }
        }
        else if constexpr(TestType::Length() == 3u)
        {
            {
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{}), Scalar(-0.412199), Scalar(0.000001f)));
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{std::numeric_limits<Scalar>::min()}), Scalar(-0.412199), Scalar(0.000001f)));
            }
            {
                static constexpr auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                static_assert(TRAP::Math::Equal(test, Scalar(0.733515), Scalar(0.000001f)));
            }
            {
                static constexpr auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
                static_assert(TRAP::Math::Equal(test, Scalar(0.412199), Scalar(0.000001f)));
            }
            {
                static constexpr auto test = TRAP::Math::Simplex(TestType{-Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
                static_assert(TRAP::Math::Equal(test, Scalar(0.412199), Scalar(0.000001f)));
            }

            if constexpr(std::same_as<Scalar, f32>)
            {
                {
                    static constexpr auto test = TRAP::Math::Simplex(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                    static_assert(TRAP::Math::Equal(test, Scalar(-0.663988), Scalar(0.000001f)));
                }
            }
            else if constexpr(std::same_as<Scalar, f64>)
            {
                {
                    static constexpr auto test = TRAP::Math::Simplex(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                    static_assert(TRAP::Math::Equal(test, Scalar(0.663988), Scalar(0.000001f)));
                }
            }
        }
        else if constexpr(TestType::Length() == 4u)
        {
            {
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{}), Scalar(), epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{std::numeric_limits<Scalar>::min()}), Scalar(), epsilon));
            }
            {
                static constexpr auto test = TRAP::Math::Simplex(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                static_assert(TRAP::Math::Equal(test, Scalar(-0.216971), Scalar(0.00001f)));
            }
            {
                static constexpr auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{0.0f, 5.0f, 10.0f, 15.0f}});
                static_assert(TRAP::Math::Equal(test, Scalar(0.136951), Scalar(0.00001f)));
            }

            if constexpr(std::same_as<Scalar, f32>)
            {
                {
                    static constexpr auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                    static_assert(TRAP::Math::Equal(test, Scalar(-0.374768), Scalar(0.000001f)));
                }
                {
                    static constexpr auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
                    static_assert(TRAP::Math::Equal(test, Scalar(0.355061), Scalar(0.00001f)));
                }
            }
            else if constexpr(std::same_as<Scalar, f64>)
            {
                {
                    static constexpr auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                    static_assert(TRAP::Math::Equal(test, Scalar(0.351581), Scalar(0.000001f)));
                }
                {
                    static constexpr auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
                    static_assert(TRAP::Math::Equal(test, Scalar(-0.317451), Scalar(0.00001f)));
                }
            }
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar epsilon = TRAP::Math::Epsilon<Scalar>();

        if constexpr(TestType::Length() == 2u)
        {
            {
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{}), Scalar(), epsilon));
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{std::numeric_limits<Scalar>::min()}), Scalar(), epsilon));
            }
            {
                const auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                REQUIRE(TRAP::Math::Equal(test, Scalar(0.368338), Scalar(0.000001f)));
            }
            {
                const auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
                REQUIRE(TRAP::Math::Equal(test, Scalar(0.0649866), Scalar(0.00001f)));
            }
            {
                const auto test = TRAP::Math::Simplex(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                REQUIRE(TRAP::Math::Equal(test, Scalar(-0.151068), Scalar(0.00001f)));
            }
            {
                const auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{0.0f, 5.0f, 10.0f, 15.0f}});
                REQUIRE(TRAP::Math::Equal(test, Scalar(0.558613), Scalar(0.00001f)));
            }
        }
        else if constexpr(TestType::Length() == 3u)
        {
            {
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{}), Scalar(-0.412199), Scalar(0.000001f)));
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{std::numeric_limits<Scalar>::min()}), Scalar(-0.412199), Scalar(0.000001f)));
            }
            {
                const auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                REQUIRE(TRAP::Math::Equal(test, Scalar(0.733515), Scalar(0.000001f)));
            }
            {
                const auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
                REQUIRE(TRAP::Math::Equal(test, Scalar(0.412199), Scalar(0.000001f)));
            }
            {
                const auto test = TRAP::Math::Simplex(TestType{-Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
                REQUIRE(TRAP::Math::Equal(test, Scalar(0.412199), Scalar(0.000001f)));
            }

            if constexpr(std::same_as<Scalar, f32>)
            {
                {
                    const auto test = TRAP::Math::Simplex(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                    REQUIRE(TRAP::Math::Equal(test, Scalar(-0.663988), Scalar(0.000001f)));
                }
            }
            else if constexpr(std::same_as<Scalar, f64>)
            {
                {
                    const auto test = TRAP::Math::Simplex(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                    REQUIRE(TRAP::Math::Equal(test, Scalar(0.663988), Scalar(0.000001f)));
                }
            }
        }
        else if constexpr(TestType::Length() == 4u)
        {
            {
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{}), Scalar(), epsilon));
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Simplex(TestType{std::numeric_limits<Scalar>::min()}), Scalar(), epsilon));
            }
            {
                const auto test = TRAP::Math::Simplex(TestType{-Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                REQUIRE(TRAP::Math::Equal(test, Scalar(-0.216971), Scalar(0.00001f)));
            }
            {
                const auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{0.0f, 5.0f, 10.0f, 15.0f}});
                REQUIRE(TRAP::Math::Equal(test, Scalar(0.136951), Scalar(0.00001f)));
            }

            if constexpr(std::same_as<Scalar, f32>)
            {
                {
                    const auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                    REQUIRE(TRAP::Math::Equal(test, Scalar(-0.374768), Scalar(0.000001f)));
                }
                {
                    const auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
                    REQUIRE(TRAP::Math::Equal(test, Scalar(0.355061), Scalar(0.00001f)));
                }
            }
            else if constexpr(std::same_as<Scalar, f64>)
            {
                {
                    const auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{1.0f, 2.0f, 3.0f, 4.0f}});
                    REQUIRE(TRAP::Math::Equal(test, Scalar(0.351581), Scalar(0.000001f)));
                }
                {
                    const auto test = TRAP::Math::Simplex(TestType{Vec4Scalar{10.0f, 20.0f, 30.0f, 40.0f}});
                    REQUIRE(TRAP::Math::Equal(test, Scalar(-0.317451), Scalar(0.00001f)));
                }
            }
        }
    }
}

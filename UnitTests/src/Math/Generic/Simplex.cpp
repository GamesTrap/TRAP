#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunSimplexCompileTimeTests()
    {
        constexpr typename T::value_type epsilon = TRAP::Math::Epsilon<typename T::value_type>();

        if constexpr(T::Length() == 2)
        {
            {
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(T{}), typename T::value_type(), epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(T{std::numeric_limits<typename T::value_type>::min()}), typename T::value_type(), epsilon));
            }
            {
                constexpr auto test = TRAP::Math::Simplex(T{TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}});
                static_assert(TRAP::Math::Equal(test, typename T::value_type(0.368338), typename T::value_type(0.000001f)));
            }
            {
                constexpr auto test = TRAP::Math::Simplex(T{TRAP::Math::Vec<4, typename T::value_type>{10.0f, 20.0f, 30.0f, 40.0f}});
                static_assert(TRAP::Math::Equal(test, typename T::value_type(0.0649866), typename T::value_type(0.00001f)));
            }
            {
                constexpr auto test = TRAP::Math::Simplex(T{-TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}});
                static_assert(TRAP::Math::Equal(test, typename T::value_type(-0.151068), typename T::value_type(0.00001f)));
            }
            {
                constexpr auto test = TRAP::Math::Simplex(T{TRAP::Math::Vec<4, typename T::value_type>{0.0f, 5.0f, 10.0f, 15.0f}});
                static_assert(TRAP::Math::Equal(test, typename T::value_type(0.558613), typename T::value_type(0.00001f)));
            }
        }
        else if constexpr(T::Length() == 3)
        {
            {
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(T{}), typename T::value_type(-0.412199), typename T::value_type(0.000001f)));
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(T{std::numeric_limits<typename T::value_type>::min()}), typename T::value_type(-0.412199), typename T::value_type(0.000001f)));
            }
            {
                constexpr auto test = TRAP::Math::Simplex(T{TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}});
                static_assert(TRAP::Math::Equal(test, typename T::value_type(0.733515), typename T::value_type(0.000001f)));
            }
            {
                constexpr auto test = TRAP::Math::Simplex(T{TRAP::Math::Vec<4, typename T::value_type>{10.0f, 20.0f, 30.0f, 40.0f}});
                static_assert(TRAP::Math::Equal(test, typename T::value_type(0.412199), typename T::value_type(0.000001f)));
            }
            {
                constexpr auto test = TRAP::Math::Simplex(T{-TRAP::Math::Vec<4, typename T::value_type>{10.0f, 20.0f, 30.0f, 40.0f}});
                static_assert(TRAP::Math::Equal(test, typename T::value_type(0.412199), typename T::value_type(0.000001f)));
            }

            if constexpr(std::same_as<typename T::value_type, f32>)
            {
                {
                    constexpr auto test = TRAP::Math::Simplex(T{-TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}});
                    static_assert(TRAP::Math::Equal(test, typename T::value_type(-0.663988), typename T::value_type(0.000001f)));
                }
            }
            else if constexpr(std::same_as<typename T::value_type, f64>)
            {
                {
                    constexpr auto test = TRAP::Math::Simplex(T{-TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}});
                    static_assert(TRAP::Math::Equal(test, typename T::value_type(0.663988), typename T::value_type(0.000001f)));
                }
            }
        }
        else if constexpr(T::Length() == 4)
        {
            {
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(T{}), typename T::value_type(), epsilon));
                static_assert(TRAP::Math::Equal(TRAP::Math::Simplex(T{std::numeric_limits<typename T::value_type>::min()}), typename T::value_type(), epsilon));
            }
            {
                constexpr auto test = TRAP::Math::Simplex(T{-TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}});
                static_assert(TRAP::Math::Equal(test, typename T::value_type(-0.216971), typename T::value_type(0.00001f)));
            }
            {
                constexpr auto test = TRAP::Math::Simplex(T{TRAP::Math::Vec<4, typename T::value_type>{0.0f, 5.0f, 10.0f, 15.0f}});
                static_assert(TRAP::Math::Equal(test, typename T::value_type(0.136951), typename T::value_type(0.00001f)));
            }

            if constexpr(std::same_as<typename T::value_type, f32>)
            {
                {
                    constexpr auto test = TRAP::Math::Simplex(T{TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}});
                    static_assert(TRAP::Math::Equal(test, typename T::value_type(-0.374768), typename T::value_type(0.000001f)));
                }
                {
                    constexpr auto test = TRAP::Math::Simplex(T{TRAP::Math::Vec<4, typename T::value_type>{10.0f, 20.0f, 30.0f, 40.0f}});
                    static_assert(TRAP::Math::Equal(test, typename T::value_type(0.355061), typename T::value_type(0.00001f)));
                }
            }
            else if constexpr(std::same_as<typename T::value_type, f64>)
            {
                {
                    constexpr auto test = TRAP::Math::Simplex(T{TRAP::Math::Vec<4, typename T::value_type>{1.0f, 2.0f, 3.0f, 4.0f}});
                    static_assert(TRAP::Math::Equal(test, typename T::value_type(0.351581), typename T::value_type(0.000001f)));
                }
                {
                    constexpr auto test = TRAP::Math::Simplex(T{TRAP::Math::Vec<4, typename T::value_type>{10.0f, 20.0f, 30.0f, 40.0f}});
                    static_assert(TRAP::Math::Equal(test, typename T::value_type(-0.317451), typename T::value_type(0.00001f)));
                }
            }
        }
    }
}

TEST_CASE("TRAP::Math::Simplex()", "[math][generic][simplex]")
{
    SECTION("Vec2 - f32")
    {
        RunSimplexCompileTimeTests<TRAP::Math::Vec2f>();
    }
    SECTION("Vec2 - f64")
    {
        RunSimplexCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec3 - f32")
    {
        RunSimplexCompileTimeTests<TRAP::Math::Vec3f>();
    }
    SECTION("Vec3 - f64")
    {
        RunSimplexCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec4 - f32")
    {
        RunSimplexCompileTimeTests<TRAP::Math::Vec4f>();
    }
    SECTION("Vec4 - f64")
    {
        RunSimplexCompileTimeTests<TRAP::Math::Vec4d>();
    }
}

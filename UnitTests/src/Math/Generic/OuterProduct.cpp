#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type> && T::Length() > 2)
    consteval void RunCompileTimeOuterProductTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            constexpr T v1(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 3.0f, 4.0f));
            constexpr T v2(TRAP::Math::Vec<4, typename T::value_type>(5.0f, 6.0f, 7.0f, 8.0f));
            constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(5.0f, 10.0f, 15.0f, 20.0f, 6.0f, 12.0f, 18.0f, 24.0f, 7.0f, 14.0f, 21.0f, 28.0f, 8.0f, 16.0f, 24.0f, 32.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
        {
            constexpr T v1(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr T v2(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
        {
            constexpr T v1(TRAP::Math::Vec<4, typename T::value_type>(2.0f));
            constexpr T v2(TRAP::Math::Vec<4, typename T::value_type>(3.0f));
            constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
        {
            constexpr T v1(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, -1.0f, 0.0f));
            constexpr T v2(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, -1.0f));
            constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
        {
            constexpr T v1(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 3.0f, 4.0f));
            constexpr T v2(TRAP::Math::Vec<4, typename T::value_type>(0.0f));
            constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
        {
            constexpr T v1(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 3.0f, 4.0f));
            constexpr T v2(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 3.0f, 4.0f));
            constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(1.0f, 2.0f, 3.0f, 4.0f, 2.0f, 4.0f, 6.0f, 8.0f, 3.0f, 6.0f, 9.0f, 12.0f, 4.0f, 8.0f, 12.0f, 16.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(result, expected, Epsilon)));
        }
    }

    template<typename T>
    requires (TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type> && T::Length() > 2)
    void RunOuterProductEdgeTests()
    {
        static constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::lowest();
        static constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        {
            static constexpr T v1(TRAP::Math::Vec<4, typename T::value_type>{max});
            static constexpr T v2(TRAP::Math::Vec<4, typename T::value_type>{min});
            const auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected)));
        }
        {
            static constexpr T v(TRAP::Math::Vec<4, typename T::value_type>{inf});
            static constexpr auto result = TRAP::Math::OuterProduct(v, v);
            static constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected)));
        }
        {
            static constexpr T v1(TRAP::Math::Vec<4, typename T::value_type>{inf});
            static constexpr T v2(TRAP::Math::Vec<4, typename T::value_type>{ninf});
            static constexpr auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf, ninf));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, expected)));
        }
        {
            static constexpr T v1(TRAP::Math::Vec<4, typename T::value_type>{nan});
            static constexpr T v2(TRAP::Math::Vec<4, typename T::value_type>{1.f});
            const auto result = TRAP::Math::OuterProduct(v1, v2);
            static constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(result, expected))));
        }
        {
            static constexpr T v(TRAP::Math::Vec<4, typename T::value_type>{nan});
            const auto result = TRAP::Math::OuterProduct(v, v);
            static constexpr decltype(result) expected(TRAP::Math::tMat4<typename T::value_type>(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(result, expected))));
        }
    }
}

TEST_CASE("TRAP::Math::OuterProduct()", "[math][generic][outerproduct]")
{
    SECTION("Vec3 - f64")
    {
        RunCompileTimeOuterProductTests<TRAP::Math::Vec3d>();
        RunOuterProductEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeOuterProductTests<TRAP::Math::Vec3f>();
        RunOuterProductEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunCompileTimeOuterProductTests<TRAP::Math::Vec4d>();
        RunOuterProductEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeOuterProductTests<TRAP::Math::Vec4f>();
        RunOuterProductEdgeTests<TRAP::Math::Vec4f>();
    }
}
#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T>) && std::floating_point<typename T::value_type>)
    consteval void RunCompileTimeTransposeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            constexpr T m(TRAP::Math::tMat4<typename T::value_type>(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
            constexpr T t = TRAP::Math::Transpose(m);
            constexpr T expected(TRAP::Math::tMat4<typename T::value_type>(0.0f, 4.0f, 8.0f, 12.0f, 1.0f, 5.0f, 9.0f, 13.0f, 2.0f, 6.0f, 10.0f, 14.0f, 3.0f, 7.0f, 11.0f, 15.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        }
        {
            constexpr T m(TRAP::Math::tMat4<typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f));
            constexpr T t = TRAP::Math::Transpose(m);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, m, Epsilon)));
        }
        {
            constexpr T m(TRAP::Math::tMat4<typename T::value_type>(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f));
            constexpr T t = TRAP::Math::Transpose(m);
            constexpr T t2 = TRAP::Math::Transpose(t);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(m, t2, Epsilon)));
        }
    }

    template<typename T>
    requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T>) && std::floating_point<typename T::value_type>)
    void RunTransposeEdgeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::lowest();
        static constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        {
            static constexpr T m(TRAP::Math::tMat4<typename T::value_type>(max, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr T t = TRAP::Math::Transpose(m);
            static constexpr T expected(TRAP::Math::tMat4<typename T::value_type>(max, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        }
        {
            static constexpr T m(TRAP::Math::tMat4<typename T::value_type>(min, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr T t = TRAP::Math::Transpose(m);
            static constexpr T expected(TRAP::Math::tMat4<typename T::value_type>(min, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected, Epsilon)));
        }
        {
            static constexpr T m(TRAP::Math::tMat4<typename T::value_type>(inf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr T t = TRAP::Math::Transpose(m);
            static constexpr T expected(TRAP::Math::tMat4<typename T::value_type>(inf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected)));
        }
        {
            static constexpr T m(TRAP::Math::tMat4<typename T::value_type>(ninf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static constexpr T t = TRAP::Math::Transpose(m);
            static constexpr T expected(TRAP::Math::tMat4<typename T::value_type>(ninf, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(t, expected)));
        }
        {
            static constexpr T m(TRAP::Math::tMat4<typename T::value_type>(nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan));
            static constexpr T t = TRAP::Math::Transpose(m);
            REQUIRE(TRAP::Math::All(TRAP::Math::Not(TRAP::Math::Equal(t, m))));
        }
    }
}

TEST_CASE("TRAP::Math::Transpose()", "[math][generic][transpose]")
{
    SECTION("Mat3 - double")
    {
        RunCompileTimeTransposeTests<TRAP::Math::Mat3d>();
        RunTransposeEdgeTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunCompileTimeTransposeTests<TRAP::Math::Mat3f>();
        RunTransposeEdgeTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunCompileTimeTransposeTests<TRAP::Math::Mat4d>();
        RunTransposeEdgeTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeTransposeTests<TRAP::Math::Mat4f>();
        RunTransposeEdgeTests<TRAP::Math::Mat4f>();
    }
}
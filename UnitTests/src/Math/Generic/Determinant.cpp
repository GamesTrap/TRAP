#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T>) && std::floating_point<typename T::value_type>)
    consteval void RunCompileTimeDeterminantTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(T(1.0f)), typename T::value_type(1.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(T(0.0f)), typename T::value_type(0.0f), Epsilon));

        constexpr T input1(TRAP::Math::tMat4<typename T::value_type>(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.0f));
        constexpr T input2(TRAP::Math::tMat4<typename T::value_type>(2.0f, 4.0f, 1.0f, 3.0f, 1.0f, 2.0f, 4.0f, 6.0f, 3.0f, 1.0f, 2.0f, 2.0f, 5.0f, 7.0f, 3.0f, 1.0f));
        constexpr T input3(TRAP::Math::tMat4<typename T::value_type>(1.0f, 2.0f, 3.0f, 4.0f, 2.0f, 4.0f, 6.0f, 8.0f, 3.0f, 6.0f, 9.0f, 12.0f, 4.0f, 8.0f, 12.0f, 16.0f));
        constexpr T input4(TRAP::Math::tMat4<typename T::value_type>(-1.0f, 2.0f, -3.0f, 4.0f, -2.0f, 5.0f, -1.0f, 7.0f, -3.0f, 1.0f, -4.0f, 6.0f, -4.0f, 3.0f, -2.0f, 8.0f));

        if constexpr(TRAP::Math::IsMat4<T>)
        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input1), typename T::value_type(120.0f), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input2), typename T::value_type(-184.0f), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input3), typename T::value_type(0.0f), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input4), typename T::value_type(5.0f), Epsilon));
        }
        else if constexpr(TRAP::Math::IsMat3<T>)
        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input1), typename T::value_type(24.0f), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input2), typename T::value_type(35.0f), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input3), typename T::value_type(0.0f), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Determinant(input4), typename T::value_type(-30.0f), Epsilon));
        }
    }

    template<typename T>
    requires ((TRAP::Math::IsMat3<T> || TRAP::Math::IsMat4<T>) && std::floating_point<typename T::value_type>)
    void RunDeterminantEdgeTests()
    {
        static constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::lowest();
        static constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        static constexpr T input1(TRAP::Math::tMat4<typename T::value_type>{min});
        static constexpr T input2(TRAP::Math::tMat4<typename T::value_type>{max});
        static constexpr T input3(TRAP::Math::tMat4<typename T::value_type>{inf});
        static constexpr T input4(TRAP::Math::tMat4<typename T::value_type>{ninf});
        static constexpr T input5(TRAP::Math::tMat4<typename T::value_type>{nan});

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input3)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input4)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input5)));

        if constexpr(TRAP::Math::IsMat4<T>)
        {
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input1)));
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Determinant(input2)));
        }
        else if constexpr(TRAP::Math::IsMat3<T>)
        {
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Determinant(input1)));
            REQUIRE(TRAP::Math::IsInf(TRAP::Math::Determinant(input2)));
        }
    }
}

TEST_CASE("TRAP::Math::Determinant()", "[math][generic][determinant]")
{
    SECTION("Mat3 - f64")
    {
        RunCompileTimeDeterminantTests<TRAP::Math::Mat3d>();
        RunDeterminantEdgeTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - f32")
    {
        RunCompileTimeDeterminantTests<TRAP::Math::Mat3f>();
        RunDeterminantEdgeTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - f64")
    {
        RunCompileTimeDeterminantTests<TRAP::Math::Mat4d>();
        RunDeterminantEdgeTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - f32")
    {
        RunCompileTimeDeterminantTests<TRAP::Math::Mat4f>();
        RunDeterminantEdgeTests<TRAP::Math::Mat4f>();
    }
}
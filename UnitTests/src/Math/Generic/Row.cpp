#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires TRAP::Math::IsMat<T>
void RunRowTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

    //Get
    {
        constexpr T m(1.0f);
        constexpr typename T::rowType expectedA(TRAP::Math::tVec4<typename T::valueType>(1.0f, 0.0f, 0.0f, 0.0f));
        constexpr typename T::rowType expectedB(TRAP::Math::tVec4<typename T::valueType>(0.0f, 1.0f, 0.0f, 0.0f));
        constexpr typename T::rowType expectedC(TRAP::Math::tVec4<typename T::valueType>(0.0f, 0.0f, 1.0f, 0.0f));
        constexpr typename T::rowType expectedD(TRAP::Math::tVec4<typename T::valueType>(0.0f, 0.0f, 0.0f, 1.0f));

        if constexpr(TRAP::Math::IsMat3<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 0), expectedA, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 1), expectedB, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 2), expectedC, Epsilon)));
        }
        else if constexpr(TRAP::Math::IsMat4<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 0), expectedA, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 1), expectedB, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 2), expectedC, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 3), expectedD, Epsilon)));
        }
    }

    //Set
    {
        T m(1.0f);

        constexpr typename T::rowType expectedA(TRAP::Math::tVec4<typename T::valueType>(0.0f, 1.0f, 2.0f, 3.0f));
        constexpr typename T::rowType expectedB(TRAP::Math::tVec4<typename T::valueType>(4.0f, 5.0f, 6.0f, 7.0f));
        constexpr typename T::rowType expectedC(TRAP::Math::tVec4<typename T::valueType>(8.0f, 9.0f, 10.0f, 11.0f));
        constexpr typename T::rowType expectedD(TRAP::Math::tVec4<typename T::valueType>(12.0f, 13.0f, 14.0f, 15.0f));

        if constexpr(TRAP::Math::IsMat3<T>)
        {
            m = TRAP::Math::Row(m, 0, expectedA);
            m = TRAP::Math::Row(m, 1, expectedB);
            m = TRAP::Math::Row(m, 2, expectedC);

            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 0), expectedA, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 1), expectedB, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 2), expectedC, Epsilon)));
        }
        else if constexpr(TRAP::Math::IsMat4<T>)
        {
            m = TRAP::Math::Row(m, 0, expectedA);
            m = TRAP::Math::Row(m, 1, expectedB);
            m = TRAP::Math::Row(m, 2, expectedC);
            m = TRAP::Math::Row(m, 3, expectedD);

            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 0), expectedA, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 1), expectedB, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 2), expectedC, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 3), expectedD, Epsilon)));
        }
    }
}

TEST_CASE("TRAP::Math::Row()", "[math][generic][row]")
{
    SECTION("Mat4 - double")
    {
        RunRowTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunRowTests<TRAP::Math::Mat4f>();
    }

    SECTION("Mat3 - double")
    {
        RunRowTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunRowTests<TRAP::Math::Mat3f>();
    }
}

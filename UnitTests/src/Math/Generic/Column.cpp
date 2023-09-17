#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires TRAP::Math::IsMat<T>
void RunColumnTests()
{
    static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

    //Get
    {
        static constexpr T m(1.0f);
        static constexpr typename T::col_type expectedA(TRAP::Math::tVec4<typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
        static constexpr typename T::col_type expectedB(TRAP::Math::tVec4<typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
        static constexpr typename T::col_type expectedC(TRAP::Math::tVec4<typename T::value_type>(0.0f, 0.0f, 1.0f, 0.0f));
        static constexpr typename T::col_type expectedD(TRAP::Math::tVec4<typename T::value_type>(0.0f, 0.0f, 0.0f, 1.0f));

        if constexpr(TRAP::Math::IsMat3<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 0), expectedA, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 1), expectedB, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 2), expectedC, Epsilon)));
        }
        else if constexpr(TRAP::Math::IsMat4<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 0), expectedA, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 1), expectedB, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 2), expectedC, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 3), expectedD, Epsilon)));
        }
    }

    //Set
    {
        T m(1.0f);

        static constexpr typename T::col_type expectedA(TRAP::Math::tVec4<typename T::value_type>(0.0f, 1.0f, 2.0f, 3.0f));
        static constexpr typename T::col_type expectedB(TRAP::Math::tVec4<typename T::value_type>(4.0f, 5.0f, 6.0f, 7.0f));
        static constexpr typename T::col_type expectedC(TRAP::Math::tVec4<typename T::value_type>(8.0f, 9.0f, 10.0f, 11.0f));
        static constexpr typename T::col_type expectedD(TRAP::Math::tVec4<typename T::value_type>(12.0f, 13.0f, 14.0f, 15.0f));

        if constexpr(TRAP::Math::IsMat3<T>)
        {
            m = TRAP::Math::Column(m, 0, expectedA);
            m = TRAP::Math::Column(m, 1, expectedB);
            m = TRAP::Math::Column(m, 2, expectedC);

            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 0), expectedA, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 1), expectedB, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 2), expectedC, Epsilon)));
        }
        else if constexpr(TRAP::Math::IsMat4<T>)
        {
            m = TRAP::Math::Column(m, 0, expectedA);
            m = TRAP::Math::Column(m, 1, expectedB);
            m = TRAP::Math::Column(m, 2, expectedC);
            m = TRAP::Math::Column(m, 3, expectedD);

            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 0), expectedA, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 1), expectedB, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 2), expectedC, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 3), expectedD, Epsilon)));
        }
    }
}

TEST_CASE("TRAP::Math::Column()", "[math][generic][column]")
{
    SECTION("Mat4 - double")
    {
        RunColumnTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunColumnTests<TRAP::Math::Mat4f>();
    }

    SECTION("Mat3 - double")
    {
        RunColumnTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunColumnTests<TRAP::Math::Mat3f>();
    }
}

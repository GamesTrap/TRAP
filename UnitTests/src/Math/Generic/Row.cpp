#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsMat<T>
    consteval void RunRowCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        //Get
        {
            constexpr T m(1.0f);
            constexpr typename T::row_type expectedA(TRAP::Math::tVec4<typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr typename T::row_type expectedB(TRAP::Math::tVec4<typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            constexpr typename T::row_type expectedC(TRAP::Math::tVec4<typename T::value_type>(0.0f, 0.0f, 1.0f, 0.0f));
            constexpr typename T::row_type expectedD(TRAP::Math::tVec4<typename T::value_type>(0.0f, 0.0f, 0.0f, 1.0f));

            if constexpr(TRAP::Math::IsMat3<T>)
            {
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 0), expectedA, Epsilon)));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 1), expectedB, Epsilon)));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 2), expectedC, Epsilon)));
            }
            else if constexpr(TRAP::Math::IsMat4<T>)
            {
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 0), expectedA, Epsilon)));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 1), expectedB, Epsilon)));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 2), expectedC, Epsilon)));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 3), expectedD, Epsilon)));
            }
        }

        //Set
        {
            constexpr typename T::row_type expectedA(TRAP::Math::tVec4<typename T::value_type>(0.0f, 1.0f, 2.0f, 3.0f));
            constexpr typename T::row_type expectedB(TRAP::Math::tVec4<typename T::value_type>(4.0f, 5.0f, 6.0f, 7.0f));
            constexpr typename T::row_type expectedC(TRAP::Math::tVec4<typename T::value_type>(8.0f, 9.0f, 10.0f, 11.0f));
            constexpr typename T::row_type expectedD(TRAP::Math::tVec4<typename T::value_type>(12.0f, 13.0f, 14.0f, 15.0f));

            constexpr T m(1.0f);

            if constexpr(TRAP::Math::IsMat3<T>)
            {
                constexpr T a = TRAP::Math::Row(m, 0, expectedA);
                constexpr T b = TRAP::Math::Row(a, 1, expectedB);
                constexpr T c = TRAP::Math::Row(b, 2, expectedC);

                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(c, 0), expectedA, Epsilon)));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(c, 1), expectedB, Epsilon)));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(c, 2), expectedC, Epsilon)));
            }
            else if constexpr(TRAP::Math::IsMat4<T>)
            {
                constexpr T a = TRAP::Math::Row(m, 0, expectedA);
                constexpr T b = TRAP::Math::Row(a, 1, expectedB);
                constexpr T c = TRAP::Math::Row(b, 2, expectedC);
                constexpr T d = TRAP::Math::Row(c, 3, expectedD);

                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(d, 0), expectedA, Epsilon)));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(d, 1), expectedB, Epsilon)));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(d, 2), expectedC, Epsilon)));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(d, 3), expectedD, Epsilon)));
            }
        }
    }

    template<typename T>
    requires TRAP::Math::IsMat<T>
    void RunRowRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        //Get
        {
            static constexpr T m(1.0f);
            static constexpr typename T::row_type expectedA(TRAP::Math::tVec4<typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            static constexpr typename T::row_type expectedB(TRAP::Math::tVec4<typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            static constexpr typename T::row_type expectedC(TRAP::Math::tVec4<typename T::value_type>(0.0f, 0.0f, 1.0f, 0.0f));
            static constexpr typename T::row_type expectedD(TRAP::Math::tVec4<typename T::value_type>(0.0f, 0.0f, 0.0f, 1.0f));

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

            static constexpr typename T::row_type expectedA(TRAP::Math::tVec4<typename T::value_type>(0.0f, 1.0f, 2.0f, 3.0f));
            static constexpr typename T::row_type expectedB(TRAP::Math::tVec4<typename T::value_type>(4.0f, 5.0f, 6.0f, 7.0f));
            static constexpr typename T::row_type expectedC(TRAP::Math::tVec4<typename T::value_type>(8.0f, 9.0f, 10.0f, 11.0f));
            static constexpr typename T::row_type expectedD(TRAP::Math::tVec4<typename T::value_type>(12.0f, 13.0f, 14.0f, 15.0f));

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
}

TEST_CASE("TRAP::Math::Row()", "[math][generic][row]")
{
    SECTION("Mat4 - double")
    {
        RunRowRunTimeTests<TRAP::Math::Mat4d>();
        RunRowCompileTimeTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunRowRunTimeTests<TRAP::Math::Mat4f>();
        RunRowCompileTimeTests<TRAP::Math::Mat4f>();
    }

    SECTION("Mat3 - double")
    {
        RunRowRunTimeTests<TRAP::Math::Mat3d>();
        RunRowCompileTimeTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunRowRunTimeTests<TRAP::Math::Mat3f>();
        RunRowCompileTimeTests<TRAP::Math::Mat3f>();
    }
}

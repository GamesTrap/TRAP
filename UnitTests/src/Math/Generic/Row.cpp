#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Row()", "[math][generic][row][mat]",
                   TRAP::Math::Mat3f, TRAP::Math::Mat3d, TRAP::Math::Mat4f, TRAP::Math::Mat4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;
    using Row = TestType::row_type;

    static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

    //Get
    {
        static constexpr TestType m(1.0f);
        static constexpr Row expectedA(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
        static constexpr Row expectedB(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
        static constexpr Row expectedC(Vec4Scalar(0.0f, 0.0f, 1.0f, 0.0f));
        static constexpr Row expectedD(Vec4Scalar(0.0f, 0.0f, 0.0f, 1.0f));

        if constexpr(TRAP::Math::IsMat3<TestType>)
        {
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 0), expectedA, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 1), expectedB, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 2), expectedC, Epsilon)));
        }
        else if constexpr(TRAP::Math::IsMat4<TestType>)
        {
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 0), expectedA, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 1), expectedB, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 2), expectedC, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Row(m, 3), expectedD, Epsilon)));
        }
    }

    //Set
    {
        static constexpr Row expectedA(Vec4Scalar(0.0f, 1.0f, 2.0f, 3.0f));
        static constexpr Row expectedB(Vec4Scalar(4.0f, 5.0f, 6.0f, 7.0f));
        static constexpr Row expectedC(Vec4Scalar(8.0f, 9.0f, 10.0f, 11.0f));
        static constexpr Row expectedD(Vec4Scalar(12.0f, 13.0f, 14.0f, 15.0f));

        static constexpr TestType m(1.0f);

        if constexpr(TRAP::Math::IsMat3<TestType>)
        {
            static constexpr TestType a = TRAP::Math::Row(m, 0, expectedA);
            static constexpr TestType b = TRAP::Math::Row(a, 1, expectedB);
            static constexpr TestType c = TRAP::Math::Row(b, 2, expectedC);

            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(c, 0), expectedA, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(c, 1), expectedB, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(c, 2), expectedC, Epsilon)));
        }
        else if constexpr(TRAP::Math::IsMat4<TestType>)
        {
            static constexpr TestType a = TRAP::Math::Row(m, 0, expectedA);
            static constexpr TestType b = TRAP::Math::Row(a, 1, expectedB);
            static constexpr TestType c = TRAP::Math::Row(b, 2, expectedC);
            static constexpr TestType d = TRAP::Math::Row(c, 3, expectedD);

            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(d, 0), expectedA, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(d, 1), expectedB, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(d, 2), expectedC, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(d, 3), expectedD, Epsilon)));
        }
    }
}

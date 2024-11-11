#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Column()", "[math][generic][column][mat]",
                   TRAP::Math::Mat4d, TRAP::Math::Mat4f, TRAP::Math::Mat3d, TRAP::Math::Mat3f)
{
    using Scalar = TestType::value_type;
    using Column = TestType::col_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

    //Get
    {
        static constexpr TestType m(1.0f);
        static constexpr Column expectedA(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f));
        static constexpr Column expectedB(Vec4Scalar(0.0f, 1.0f, 0.0f, 0.0f));
        static constexpr Column expectedC(Vec4Scalar(0.0f, 0.0f, 1.0f, 0.0f));
        static constexpr Column expectedD(Vec4Scalar(0.0f, 0.0f, 0.0f, 1.0f));

        if constexpr(TRAP::Math::IsMat3<TestType>)
        {
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 0), expectedA, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 1), expectedB, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 2), expectedC, Epsilon)));
        }
        else if constexpr(TRAP::Math::IsMat4<TestType>)
        {
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 0), expectedA, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 1), expectedB, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 2), expectedC, Epsilon)));
            STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP ::Math::Column(m, 3), expectedD, Epsilon)));
        }
    }

    //Set
    {
        TestType m(1.0f);

        static constexpr Column expectedA(Vec4Scalar(0.0f, 1.0f, 2.0f, 3.0f));
        static constexpr Column expectedB(Vec4Scalar(4.0f, 5.0f, 6.0f, 7.0f));
        static constexpr Column expectedC(Vec4Scalar(8.0f, 9.0f, 10.0f, 11.0f));
        static constexpr Column expectedD(Vec4Scalar(12.0f, 13.0f, 14.0f, 15.0f));

        if constexpr(TRAP::Math::IsMat3<TestType>)
        {
            m = TRAP::Math::Column(m, 0, expectedA);
            m = TRAP::Math::Column(m, 1, expectedB);
            m = TRAP::Math::Column(m, 2, expectedC);

            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 0), expectedA, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 1), expectedB, Epsilon)));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 2), expectedC, Epsilon)));
        }
        else if constexpr(TRAP::Math::IsMat4<TestType>)
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

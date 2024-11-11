#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Abs()", "[math][generic][abs][scalar]", i8, i16, i32, i64, f32, f64)
{
    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<TestType>(0.0)), static_cast<TestType>(0.0), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<TestType>(-0.0)), static_cast<TestType>(0.0), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<TestType>(1.0)), static_cast<TestType>(1.0), Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<TestType>(-1.0)), static_cast<TestType>(1.0), Epsilon));
    }

    SECTION("Edge cases")
    {
        static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();
        static constexpr TestType max = std::numeric_limits<TestType>::max();
        static constexpr TestType nmax = -std::numeric_limits<TestType>::max();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Abs(max), max, Epsilon));
        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Abs(nmax), max, Epsilon));

        if constexpr(std::floating_point<TestType>)
        {
            static constexpr TestType min = std::numeric_limits<TestType>::lowest();
            static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
            static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
            static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();
            static constexpr TestType dmin = std::numeric_limits<TestType>::denorm_min();

            STATIC_REQUIRE(TRAP::Math::Abs(min) == max);
            STATIC_REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Abs(nan)));
            STATIC_REQUIRE(TRAP::Math::Abs(inf) == inf);
            STATIC_REQUIRE(TRAP::Math::Abs(ninf) == inf);
            STATIC_REQUIRE(TRAP::Math::Abs(dmin) == dmin);
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Abs()", "[math][generic][abs][vec]",
                   TRAP::Math::Vec2i8, TRAP::Math::Vec2i16, TRAP::Math::Vec2i32, TRAP::Math::Vec2i64, TRAP::Math::Vec2f, TRAP::Math::Vec2d,
                   TRAP::Math::Vec3i8, TRAP::Math::Vec3i16, TRAP::Math::Vec3i32, TRAP::Math::Vec3i64, TRAP::Math::Vec3f, TRAP::Math::Vec3d,
                   TRAP::Math::Vec4i8, TRAP::Math::Vec4i16, TRAP::Math::Vec4i32, TRAP::Math::Vec4i64, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

    using Scalar = TestType::value_type;

    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(TestType(static_cast<Scalar>(0.0))), TestType(static_cast<Scalar>(0.0)), Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(TestType(static_cast<Scalar>(-0.0))), TestType(static_cast<Scalar>(0.0)), Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(TestType(static_cast<Scalar>(1.0))), TestType(static_cast<Scalar>(1.0)), Epsilon)));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(TestType(static_cast<Scalar>(-1.0))), TestType(static_cast<Scalar>(1.0)), Epsilon)));
}

TEMPLATE_TEST_CASE("TRAP::Math::Abs()", "[math][generic][abs][mat]", TRAP::Math::Mat3d, TRAP::Math::Mat3f, TRAP::Math::Mat4d, TRAP::Math::Mat4f)
{
    using Scalar = TestType::value_type;

    if constexpr(std::same_as<TestType, TRAP::Math::tMat3<Scalar>>)
    {
        static constexpr TestType A
        (
            Scalar(3.0), Scalar(1.0), Scalar(5.2),
            Scalar(1.4), Scalar(0.5), Scalar(9.3),
            Scalar(6.8), Scalar(8.4), Scalar(4.3)
        );
        static constexpr TestType B
        (
            Scalar( 1.0), Scalar(-1.0), Scalar( 1.0),
            Scalar(-1.0), Scalar( 1.0), Scalar( 1.0),
            Scalar( 1.0), Scalar(-1.0), Scalar(-1.0)
        );

        static constexpr TestType C = TRAP::Math::MatrixCompMult(A, B);
        static constexpr TestType D = TRAP::Math::Abs(C);

        static constexpr TRAP::Math::Vec3b col1 = TRAP::Math::Equal(std::get<0u>(D), std::get<0u>(A));
        static constexpr TRAP::Math::Vec3b col2 = TRAP::Math::Equal(std::get<1u>(D), std::get<1u>(A));
        static constexpr TRAP::Math::Vec3b col3 = TRAP::Math::Equal(std::get<2u>(D), std::get<2u>(A));

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Vec3b{TRAP::Math::All(col1),
                                                         TRAP::Math::All(col2),
                                                         TRAP::Math::All(col3)}));
    }
    else if constexpr(std::same_as<TestType, TRAP::Math::tMat4<Scalar>>)
    {
        static constexpr TestType A
        (
            Scalar(3.0), Scalar(1.0), Scalar(5.2), Scalar(4.9),
            Scalar(1.4), Scalar(0.5), Scalar(9.3), Scalar(3.7),
            Scalar(6.8), Scalar(8.4), Scalar(4.3), Scalar(3.9),
            Scalar(5.6), Scalar(7.2), Scalar(1.1), Scalar(4.4)
        );
        static constexpr TestType B
        (
            Scalar( 1.0), Scalar(-1.0), Scalar( 1.0), Scalar( 1.0),
            Scalar(-1.0), Scalar( 1.0), Scalar( 1.0), Scalar(-1.0),
            Scalar( 1.0), Scalar(-1.0), Scalar(-1.0), Scalar(-1.0),
            Scalar(-1.0), Scalar(-1.0), Scalar( 1.0), Scalar( 1.0)
        );

        static constexpr TestType C = TRAP::Math::MatrixCompMult(A, B);
        static constexpr TestType D = TRAP::Math::Abs(C);

        static constexpr TRAP::Math::Vec4b col1 = TRAP::Math::Equal(std::get<0u>(D), std::get<0u>(A));
        static constexpr TRAP::Math::Vec4b col2 = TRAP::Math::Equal(std::get<1u>(D), std::get<1u>(A));
        static constexpr TRAP::Math::Vec4b col3 = TRAP::Math::Equal(std::get<2u>(D), std::get<2u>(A));
        static constexpr TRAP::Math::Vec4b col4 = TRAP::Math::Equal(std::get<3u>(D), std::get<3u>(A));

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Vec4b{TRAP::Math::All(col1),
                                                         TRAP::Math::All(col2),
                                                         TRAP::Math::All(col3),
                                                         TRAP::Math::All(col4)}));
    }
}

#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Mix()", "[math][generic][mix][scalar]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{

    SECTION("Normal cases")
    {
        static constexpr TestType Epsilon = TRAP::Math::Epsilon<TestType>();

        {
            using X = bool;

            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(0.0f), TestType(1.0f), X(0.0f)), TestType(0.0f), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(0.0f), TestType(1.0f), X(1.0f)), TestType(1.0f), Epsilon));
            if constexpr(std::floating_point<TestType> || std::signed_integral<TestType>)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(-1.0f), TestType(1.0f), X(0.0f)), TestType(-1.0f), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(-1.0f), TestType(1.0f), X(1.0f)), TestType(1.0f), Epsilon));
            }
        }
        {
            using X = f32;

            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(0.0f), TestType(1.0f), X(0.0f)), TestType(0.0f), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(0.0f), TestType(1.0f), X(1.0f)), TestType(1.0f), Epsilon));
            if constexpr(std::floating_point<TestType> || std::signed_integral<TestType>)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(-1.0f), TestType(1.0f), X(0.0f)), TestType(-1.0f), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(-1.0f), TestType(1.0f), X(1.0f)), TestType(1.0f), Epsilon));
            }
        }
        {
            using X = f64;

            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(0.0f), TestType(1.0f), X(0.0f)), TestType(0.0f), Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(0.0f), TestType(1.0f), X(1.0f)), TestType(1.0f), Epsilon));
            if constexpr(std::floating_point<TestType> || std::signed_integral<TestType>)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(-1.0f), TestType(1.0f), X(0.0f)), TestType(-1.0f), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(TestType(-1.0f), TestType(1.0f), X(1.0f)), TestType(1.0f), Epsilon));
            }
        }
    }

    SECTION("Edge cases")
    {
        if constexpr(std::floating_point<TestType>)
        {
            static constexpr TestType Epsilon = std::numeric_limits<TestType>::epsilon();

            static constexpr TestType max = std::numeric_limits<TestType>::max();
            static constexpr TestType min = std::numeric_limits<TestType>::lowest();
            static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
            static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(min, max, TestType(0.0f)), min, Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(min, max, TestType(1.0f)), max, Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(max, max, TestType(0.5f)), max, Epsilon));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Mix(min, min, TestType(0.5f)), min, Epsilon));
            STATIC_REQUIRE(TRAP::Math::IsInf(TRAP::Math::Mix(min, inf, TestType(1.0f))));
            STATIC_REQUIRE(TRAP::Math::IsInf(TRAP::Math::Mix(min, ninf, TestType(1.0f))));
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Mix()", "[math][generic][mix][vec]",
                   TRAP::Math::Vec2i8, TRAP::Math::Vec2i16, TRAP::Math::Vec2i32, TRAP::Math::Vec2i64,
                   TRAP::Math::Vec2ui8, TRAP::Math::Vec2ui16, TRAP::Math::Vec2ui32, TRAP::Math::Vec2ui64, TRAP::Math::Vec2f, TRAP::Math::Vec2d,
                   TRAP::Math::Vec3i8, TRAP::Math::Vec3i16, TRAP::Math::Vec3i32, TRAP::Math::Vec3i64,
                   TRAP::Math::Vec3ui8, TRAP::Math::Vec3ui16, TRAP::Math::Vec3ui32, TRAP::Math::Vec3ui64, TRAP::Math::Vec3f, TRAP::Math::Vec3d,
                   TRAP::Math::Vec4i8, TRAP::Math::Vec4i16, TRAP::Math::Vec4i32, TRAP::Math::Vec4i64,
                   TRAP::Math::Vec4ui8, TRAP::Math::Vec4ui16, TRAP::Math::Vec4ui32, TRAP::Math::Vec4ui64, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using ScalarT = TestType::value_type;

    static constexpr ScalarT Epsilon = TRAP::Math::Epsilon<ScalarT>();

    {
        using X = bool;

        static constexpr TestType res1 = TRAP::Math::Mix(TestType(0), TestType(1), X(0.0f));
        static constexpr TestType res2 = TRAP::Math::Mix(TestType(0), TestType(1), X(1.0f));

        if constexpr(std::signed_integral<ScalarT> || std::floating_point<ScalarT>)
        {
            static constexpr TestType res3 = TRAP::Math::Mix(TestType(-1), TestType(1), X(0.0f));
            static constexpr TestType res4 = TRAP::Math::Mix(TestType(-1), TestType(1), X(1.0f));

            if constexpr(TestType::Length() == 2u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res3.x(), TestType(-1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.y(), TestType(-1).y(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res4.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.y(), TestType(1).y(), Epsilon));
            }
            else if constexpr(TestType::Length() == 3u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.z(), TestType(0).z(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.z(), TestType(1).z(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res3.x(), TestType(-1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.y(), TestType(-1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.z(), TestType(-1).z(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res4.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.z(), TestType(1).z(), Epsilon));
            }
            else if constexpr(TestType::Length() == 4u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.z(), TestType(0).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.w(), TestType(0).w(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.z(), TestType(1).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.w(), TestType(1).w(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res3.x(), TestType(-1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.y(), TestType(-1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.z(), TestType(-1).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.w(), TestType(-1).w(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res4.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.z(), TestType(1).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.w(), TestType(1).w(), Epsilon));
            }
        }
        else
        {
            if constexpr(TestType::Length() == 2u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));
            }
            else if constexpr(TestType::Length() == 3u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.z(), TestType(0).z(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.z(), TestType(1).z(), Epsilon));
            }
            else if constexpr(TestType::Length() == 4u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.z(), TestType(0).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.w(), TestType(0).w(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.z(), TestType(1).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.w(), TestType(1).w(), Epsilon));
            }
        }
    }
    {
        using X = TRAP::Math::Vec<TestType::Length(), bool>;

        static constexpr TestType res1 = TRAP::Math::Mix(TestType(0), TestType(1), X(0.0f));
        static constexpr TestType res2 = TRAP::Math::Mix(TestType(0), TestType(1), X(1.0f));

        if constexpr(std::signed_integral<ScalarT> || std::floating_point<ScalarT>)
        {
            static constexpr TestType res3 = TRAP::Math::Mix(TestType(-1), TestType(1), X(0.0f));
            static constexpr TestType res4 = TRAP::Math::Mix(TestType(-1), TestType(1), X(1.0f));

            if constexpr(TestType::Length() == 2u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res3.x(), TestType(-1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.y(), TestType(-1).y(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res4.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.y(), TestType(1).y(), Epsilon));
            }
            else if constexpr(TestType::Length() == 3u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.z(), TestType(0).z(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.z(), TestType(1).z(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res3.x(), TestType(-1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.y(), TestType(-1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.z(), TestType(-1).z(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res4.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.z(), TestType(1).z(), Epsilon));
            }
            else if constexpr(TestType::Length() == 4u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.z(), TestType(0).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.w(), TestType(0).w(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.z(), TestType(1).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.w(), TestType(1).w(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res3.x(), TestType(-1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.y(), TestType(-1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.z(), TestType(-1).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res3.w(), TestType(-1).w(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res4.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.z(), TestType(1).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res4.w(), TestType(1).w(), Epsilon));
            }
        }
        else
        {
            if constexpr(TestType::Length() == 2u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));
            }
            else if constexpr(TestType::Length() == 3u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.z(), TestType(0).z(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.z(), TestType(1).z(), Epsilon));
            }
            else if constexpr(TestType::Length() == 4u)
            {
                STATIC_REQUIRE(TRAP::Math::Equal(res1.x(), TestType(0).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.y(), TestType(0).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.z(), TestType(0).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res1.w(), TestType(0).w(), Epsilon));

                STATIC_REQUIRE(TRAP::Math::Equal(res2.x(), TestType(1).x(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.y(), TestType(1).y(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.z(), TestType(1).z(), Epsilon));
                STATIC_REQUIRE(TRAP::Math::Equal(res2.w(), TestType(1).w(), Epsilon));
            }
        }
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Mix()", "[math][generic][mix][mat]",
                   TRAP::Math::Mat3f, TRAP::Math::Mat3d, TRAP::Math::Mat4f, TRAP::Math::Mat4d)
{
    using Scalar = TestType::value_type;

    static constexpr TestType A(static_cast<Scalar>(2));
    static constexpr TestType B(static_cast<Scalar>(4));
    static constexpr TestType C = TRAP::Math::Mix(A, B, 0.5);
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(C, TestType(static_cast<Scalar>(3)), static_cast<Scalar>(1))));
    static constexpr TestType D = TRAP::Math::Mix(A, B, 0.5f);
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Equal(D, TestType(static_cast<Scalar>(3)), static_cast<Scalar>(1))));
}

TEMPLATE_TEST_CASE("TRAP::Math::Mix()", "[math][generic][mix][quat]",
                   TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr TestType Q1(Vec3Scalar(1, 0, 0), Vec3Scalar(1, 0, 0));
        static constexpr TestType Q2(Vec3Scalar(1, 0, 0), Vec3Scalar(0, 1, 0));

        static constexpr TestType Q3 = TRAP::Math::Mix(Q1, Q2, static_cast<Scalar>(0.5f));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Angle(Q3)), static_cast<Scalar>(45.0f), static_cast<Scalar>(0.001f)));

        static constexpr TestType Q4 = TRAP::Math::Mix(Q2, Q1, static_cast<Scalar>(0.5f));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Angle(Q4)), static_cast<Scalar>(45.0f), static_cast<Scalar>(0.001f)));

        static constexpr TestType A = TRAP::Math::AngleAxis(static_cast<Scalar>(0.0f), Vec3Scalar(static_cast<Scalar>(0.0f), static_cast<Scalar>(0.0f), static_cast<Scalar>(1.0f)));
        static constexpr TestType B = TRAP::Math::AngleAxis(TRAP::Math::PI<Scalar>() * static_cast<Scalar>(0.5f), Vec3Scalar(static_cast<Scalar>(0.0f), static_cast<Scalar>(0.0f), static_cast<Scalar>(1.0f)));
        static constexpr TestType C = TRAP::Math::Mix(A, B, static_cast<Scalar>(0.5f));
        static constexpr TestType D = TRAP::Math::AngleAxis(TRAP::Math::PI<Scalar>() * static_cast<Scalar>(0.25f), Vec3Scalar(static_cast<Scalar>(0.0f), static_cast<Scalar>(0.0f), static_cast<Scalar>(1.0f)));

        static_assert(TRAP::Math::Equal(C.x(), D.x(), static_cast<Scalar>(0.01f)));
        static_assert(TRAP::Math::Equal(C.y(), D.y(), static_cast<Scalar>(0.01f)));
        static_assert(TRAP::Math::Equal(C.z(), D.z(), static_cast<Scalar>(0.01f)));
        static_assert(TRAP::Math::Equal(C.w(), D.w(), static_cast<Scalar>(0.01f)));

        static constexpr TestType e(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
        static constexpr TestType f(Scalar(1.0f), Scalar(0.1f), Scalar(0.1f), Scalar(0.1f));
        static constexpr Scalar a = Scalar(0.5f) * (Scalar(1.0f) + TRAP::Math::Epsilon<Scalar>());
        static constexpr TestType expected(Scalar(1.0f), Scalar(0.05f), Scalar(0.05f), Scalar(0.05f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mix(e, f, a), expected, TRAP::Math::Epsilon<Scalar>())));
    }

    SECTION("Normal cases - std")
    {
        static constexpr TestType Q1(Vec3Scalar(1, 0, 0), Vec3Scalar(1, 0, 0));
        static constexpr TestType Q2(Vec3Scalar(1, 0, 0), Vec3Scalar(0, 1, 0));

        const TestType Q3 = TRAP::Math::Mix(Q1, Q2, static_cast<Scalar>(0.5f));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Angle(Q3)), static_cast<Scalar>(45.0f), static_cast<Scalar>(0.001f)));

        const TestType Q4 = TRAP::Math::Mix(Q2, Q1, static_cast<Scalar>(0.5f));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Angle(Q4)), static_cast<Scalar>(45.0f), static_cast<Scalar>(0.001f)));

        static constexpr TestType A = TRAP::Math::AngleAxis(static_cast<Scalar>(0.0f), Vec3Scalar(static_cast<Scalar>(0.0f), static_cast<Scalar>(0.0f), static_cast<Scalar>(1.0f)));
        static constexpr TestType B = TRAP::Math::AngleAxis(TRAP::Math::PI<Scalar>() * static_cast<Scalar>(0.5f), Vec3Scalar(static_cast<Scalar>(0.0f), static_cast<Scalar>(0.0f), static_cast<Scalar>(1.0f)));
        const TestType C = TRAP::Math::Mix(A, B, static_cast<Scalar>(0.5f));
        static constexpr TestType D = TRAP::Math::AngleAxis(TRAP::Math::PI<Scalar>() * static_cast<Scalar>(0.25f), Vec3Scalar(static_cast<Scalar>(0.0f), static_cast<Scalar>(0.0f), static_cast<Scalar>(1.0f)));

        REQUIRE(TRAP::Math::Equal(C.x(), D.x(), static_cast<Scalar>(0.01f)));
        REQUIRE(TRAP::Math::Equal(C.y(), D.y(), static_cast<Scalar>(0.01f)));
        REQUIRE(TRAP::Math::Equal(C.z(), D.z(), static_cast<Scalar>(0.01f)));
        REQUIRE(TRAP::Math::Equal(C.w(), D.w(), static_cast<Scalar>(0.01f)));

        //Linear interpolation
        static constexpr TestType e(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
        static constexpr TestType f(Scalar(1.0f), Scalar(0.1f), Scalar(0.1f), Scalar(0.1f));
        static constexpr Scalar a = Scalar(0.5f) * (Scalar(1.0f) + TRAP::Math::Epsilon<Scalar>());
        static constexpr TestType expected(Scalar(1.0f), Scalar(0.05f), Scalar(0.05f), Scalar(0.05f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Mix(e, f, a), expected, TRAP::Math::Epsilon<Scalar>())));
    }
}

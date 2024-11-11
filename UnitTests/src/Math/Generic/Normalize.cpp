#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Normalize()", "[math][generic][normalize][vec]",
                   TRAP::Math::Vec2f, TRAP::Math::Vec2d, TRAP::Math::Vec3f, TRAP::Math::Vec3d, TRAP::Math::Vec4f, TRAP::Math::Vec4d)
{
    using Scalar = TestType::value_type;
    using Vec2Scalar = TRAP::Math::tVec2<Scalar>;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr TestType normalize1 = TRAP::Math::Normalize(TestType(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(normalize1, TestType(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f)), TestType(Epsilon))));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(normalize1), Scalar(1.0f), Epsilon));

        static constexpr TestType normalize2 = TRAP::Math::Normalize(TestType(Vec4Scalar(2.0f, 0.0f, 0.0f, 0.0f)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(normalize2, TestType(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f)), TestType(Epsilon))));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(normalize2), Scalar(1.0f), Epsilon));

        static constexpr TestType normalize3 = TRAP::Math::Normalize(TestType(Vec4Scalar(-0.6f, 0.7f, -0.5f, 0.8f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(normalize3), Scalar(1.0f), Epsilon));

        if constexpr(TRAP::Math::IsVec4<TestType>)
        {
            static_assert(TRAP::Math::All(TRAP::Math::Equal(normalize3, TestType(Vec4Scalar(-0.45485884017063427f, 0.53066861674163057f, -0.37904901841347383f, 0.60647843849879091f)), TestType(0.0000001f))));
        }
        else if constexpr(TRAP::Math::IsVec3<TestType>)
        {
            static_assert(TRAP::Math::All(TRAP::Math::Equal(normalize3, TestType(Vec3Scalar(-0.57207757317981289f, 0.66742379748924929f, -0.47673129203957787f)), TestType(0.0000001f))));
        }
        else if constexpr(TRAP::Math::IsVec2<TestType>)
        {
            static_assert(TRAP::Math::All(TRAP::Math::Equal(normalize3, TestType(Vec2Scalar(-0.65079139475254222f, 0.75925658411108998f)), TestType(0.0000001f))));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        const TestType normalize1 = TRAP::Math::Normalize(TestType(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize1, TestType(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f)), TestType(Epsilon))));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize1), Scalar(1.0f), Epsilon));

        const TestType normalize2 = TRAP::Math::Normalize(TestType(Vec4Scalar(2.0f, 0.0f, 0.0f, 0.0f)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize2, TestType(Vec4Scalar(1.0f, 0.0f, 0.0f, 0.0f)), TestType(Epsilon))));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize2), Scalar(1.0f), Epsilon));

        const TestType normalize3 = TRAP::Math::Normalize(TestType(TRAP::Math::Vec<4, Scalar>(-0.6f, 0.7f, -0.5f, 0.8f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize3), Scalar(1.0f), Epsilon));

        if constexpr(TRAP::Math::IsVec4<TestType>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize3, TestType(Vec4Scalar(-0.45485884017063427f, 0.53066861674163057f, -0.37904901841347383f, 0.60647843849879091f)), TestType(0.0000001f))));
        }
        else if constexpr(TRAP::Math::IsVec3<TestType>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize3, TestType(Vec3Scalar(-0.57207757317981289f, 0.66742379748924929f, -0.47673129203957787f)), TestType(0.0000001f))));
        }
        else if constexpr(TRAP::Math::IsVec2<TestType>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize3, TestType(Vec2Scalar(-0.65079139475254222f, 0.75925658411108998f)), TestType(0.0000001f))));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        const TestType normalize1 = TRAP::Math::Normalize(TestType(Vec4Scalar(0.0f, 0.0f, 0.0f, 0.0f)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize1)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize1)));

        const TestType normalize2 = TRAP::Math::Normalize(TestType(Vec4Scalar(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize2)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize2)));

        const TestType normalize3 = TRAP::Math::Normalize(TestType(Vec4Scalar(ninf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize3)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize3)));

        const TestType normalize4 = TRAP::Math::Normalize(TestType(Vec4Scalar(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize4)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize4)));
    }
}

TEMPLATE_TEST_CASE("TRAP::Math::Normalize()", "[math][generic][normalize][quat]",
                   TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Normal cases - GCEM")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();
        static constexpr Scalar PI = TRAP::Math::PI<Scalar>();

        {
            static constexpr TestType a(1.0f, 0.0f, 0.0f, 0.0f);
            static constexpr TestType n = TRAP::Math::Normalize(a);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(a, n, Epsilon)));
        }
        {
            static constexpr TestType a(1.0f, Vec3Scalar(0.0f));
            static constexpr TestType n = TRAP::Math::Normalize(a);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(a, n, Epsilon)));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(PI * Scalar(0.25f), Vec3Scalar(0.0f, 0.0f, 1.0f));
            static constexpr TestType n = TRAP::Math::Normalize(q);
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(n), Scalar(1.0f), Epsilon));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(PI * Scalar(0.25f), Vec3Scalar(0.0f, 0.0f, 2.0f));
            static constexpr TestType n = TRAP::Math::Normalize(q);
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(n), Scalar(1.0f), Epsilon));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(PI * Scalar(0.25f), Vec3Scalar(1.0f, 2.0f, 3.0f));
            static constexpr TestType n = TRAP::Math::Normalize(q);
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(n), Scalar(1.0f), Epsilon));
        }
        {
            static constexpr TestType input(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static constexpr TestType expected(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Normalize(input), expected, Epsilon)));
        }
    }

    SECTION("Normal cases - std")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();
        static constexpr Scalar PI = TRAP::Math::PI<Scalar>();

        {
            static constexpr TestType a(1.0f, 0.0f, 0.0f, 0.0f);
            const TestType n = TRAP::Math::Normalize(a);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(a, n, Epsilon)));
        }
        {
            static constexpr TestType a(1.0f, Vec3Scalar(0.0f));
            const TestType n = TRAP::Math::Normalize(a);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(a, n, Epsilon)));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(PI * Scalar(0.25f), Vec3Scalar(0.0f, 0.0f, 1.0f));
            const TestType n = TRAP::Math::Normalize(q);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(n), Scalar(1.0f), Epsilon));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(PI * Scalar(0.25f), Vec3Scalar(0.0f, 0.0f, 2.0f));
            const TestType n = TRAP::Math::Normalize(q);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(n), Scalar(1.0f), Epsilon));
        }
        {
            static constexpr TestType q = TRAP::Math::AngleAxis(PI * Scalar(0.25f), Vec3Scalar(1.0f, 2.0f, 3.0f));
            const TestType n = TRAP::Math::Normalize(q);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(n), Scalar(1.0f), Epsilon));
        }
        {
            static constexpr TestType input(Scalar(0.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            static constexpr TestType expected(Scalar(1.0f), Scalar(0.0f), Scalar(0.0f), Scalar(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Normalize(input), expected, Epsilon)));
        }
    }

    SECTION("Edge cases")
    {
        static constexpr Scalar Epsilon = std::numeric_limits<Scalar>::epsilon();

        static constexpr Scalar inf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar ninf = std::numeric_limits<Scalar>::infinity();
        static constexpr Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

        const TestType normalize1 = TRAP::Math::Normalize(TestType(0.0f, 0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize1), Scalar(1.0f), Epsilon));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TestType(1.0f, 0.0f, 0.0f, 0.0f), normalize1, Epsilon)));

        const TestType normalize2 = TRAP::Math::Normalize(TestType(inf, inf, inf, inf));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize2)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize2)));

        const TestType normalize3 = TRAP::Math::Normalize(TestType(ninf, ninf, ninf, ninf));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize3)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize3)));

        const TestType normalize4 = TRAP::Math::Normalize(TestType(nan, nan, nan, nan));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize4)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize4)));
    }
}

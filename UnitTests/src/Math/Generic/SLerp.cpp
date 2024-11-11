#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::SLerp()", "[math][generic][slerp][scalar]", i8, i16, i32, i64, u8, u16, u32, u64, f32, f64)
{
    SECTION("Normal cases - GCEM")
    {
        if constexpr(std::floating_point<TestType>)
        {
            using QuatScalar = TRAP::Math::tQuat<TestType>;
            using Vec3Scalar = TRAP::Math::tVec3<TestType>;

            static constexpr TestType Epsilon = TestType(0.00001f);

            {
                static constexpr QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                static constexpr QuatScalar q2(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
                static constexpr QuatScalar q3 = TRAP::Math::SLerp(q1, q2, TestType(0.5f));
                static constexpr TestType f3 = TRAP::Math::Degrees(TRAP::Math::Angle(q3));
                static_assert(TRAP::Math::Equal(f3, TestType(45.0f), Epsilon));
            }
            {
                static constexpr QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                static constexpr QuatScalar q2(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
                static constexpr QuatScalar q3 = TRAP::Math::SLerp(q2, q1, TestType(0.5f));
                static constexpr TestType f3 = TRAP::Math::Degrees(TRAP::Math::Angle(q3));
                static_assert(TRAP::Math::Equal(f3, TestType(45.0f), Epsilon));
            }

            static constexpr TestType sqrt2 = TRAP::Math::Sqrt(TestType(2.0f)) / TestType(2.0f);
            static constexpr QuatScalar id(static_cast<TestType>(1.0f), static_cast<TestType>(0.0f), static_cast<TestType>(0.0f), static_cast<f32>(0.0f));
            static constexpr QuatScalar y90rot(sqrt2, 0.0f, sqrt2, 0.0f);
            [[maybe_unused]] static constexpr QuatScalar y180rot(0.0f, 0.0f, 1.0f, 0.0f);

            {
                static constexpr QuatScalar id2 = TRAP::Math::SLerp(id, y90rot, TestType(0.0f));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(id, id2, Epsilon)));
            }
            {
                static constexpr QuatScalar y90rot2 = TRAP::Math::SLerp(id, y90rot, TestType(1.0f));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot2, Epsilon)));
            }
            {
                [[maybe_unused]] static constexpr QuatScalar y45rot1 = TRAP::Math::SLerp(id, y90rot, TestType(0.5f));
            }

            static constexpr QuatScalar ym45rot2 = TRAP::Math::SLerp(y90rot, id, TestType(0.5f));

            {
                static constexpr QuatScalar y45rot3 = TRAP::Math::SLerp(id, -y90rot, TestType(0.5f));
                static constexpr TestType y45angle3 = TRAP::Math::Angle(y45rot3);
                static_assert(TRAP::Math::Equal(y45angle3, TRAP::Math::PI<TestType>() * TestType(0.25f), Epsilon));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, y45rot3, Epsilon)));
            }
            {
                static constexpr QuatScalar y45rot4 = TRAP::Math::SLerp(-y90rot, id, TestType(0.5f));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, -y45rot4, Epsilon)));
            }
            {
                static constexpr QuatScalar y90rot3 = TRAP::Math::SLerp(y90rot, y90rot, TestType(0.5f));
                static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot3, Epsilon)));
            }
            {
                static constexpr QuatScalar xz90rot = TRAP::Math::SLerp(id, -y90rot, TestType(0.5f));
                static constexpr TestType xz90angle = TRAP::Math::Angle(xz90rot);
                static_assert(TRAP::Math::Equal(xz90angle, TRAP::Math::PI<TestType>() * TestType(0.25f), Epsilon));
            }
            {
                [[maybe_unused]] static constexpr QuatScalar almostid = TRAP::Math::SLerp(id, TRAP::Math::AngleAxis(TestType(0.1f), Vec3Scalar(0.0f, 1.0f, 0.0f)), TestType(0.5f));
            }
        }
    }

    SECTION("Normal cases - std")
    {
        if constexpr(std::floating_point<TestType>)
        {
            using QuatScalar = TRAP::Math::tQuat<TestType>;
            using Vec3Scalar = TRAP::Math::tVec3<TestType>;

            static constexpr TestType Epsilon = TestType(0.00001f);

            {
                static constexpr QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                static constexpr QuatScalar q2(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
                const QuatScalar q3 = TRAP::Math::SLerp(q1, q2, TestType(0.5f));
                const TestType f3 = TRAP::Math::Degrees(TRAP::Math::Angle(q3));
                REQUIRE(TRAP::Math::Equal(f3, TestType(45.0f), Epsilon));
            }
            {
                static constexpr QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                static constexpr QuatScalar q2(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(0.0f, 1.0f, 0.0f));
                const QuatScalar q3 = TRAP::Math::SLerp(q2, q1, TestType(0.5f));
                const TestType f3 = TRAP::Math::Degrees(TRAP::Math::Angle(q3));
                REQUIRE(TRAP::Math::Equal(f3, TestType(45.0f), Epsilon));
            }

            static constexpr TestType sqrt2 = TRAP::Math::Sqrt(TestType(2.0f)) / TestType(2.0f);
            static constexpr QuatScalar id(static_cast<TestType>(1.0f), static_cast<TestType>(0.0f), static_cast<TestType>(0.0f), static_cast<f32>(0.0f));
            static constexpr QuatScalar y90rot(sqrt2, 0.0f, sqrt2, 0.0f);
            [[maybe_unused]] static constexpr QuatScalar y180rot(0.0f, 0.0f, 1.0f, 0.0f);

            {
                const QuatScalar id2 = TRAP::Math::SLerp(id, y90rot, TestType(0.0f));
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, id2, Epsilon)));
            }
            {
                const QuatScalar y90rot2 = TRAP::Math::SLerp(id, y90rot, TestType(1.0f));
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot2, Epsilon)));
            }
            {
                [[maybe_unused]] const QuatScalar y45rot1 = TRAP::Math::SLerp(id, y90rot, TestType(0.5f));
            }

            const QuatScalar ym45rot2 = TRAP::Math::SLerp(y90rot, id, TestType(0.5f));

            {
                const QuatScalar y45rot3 = TRAP::Math::SLerp(id, -y90rot, TestType(0.5f));
                const TestType y45angle3 = TRAP::Math::Angle(y45rot3);
                REQUIRE(TRAP::Math::Equal(y45angle3, TRAP::Math::PI<TestType>() * TestType(0.25f), Epsilon));
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, y45rot3, Epsilon)));
            }
            {
                const QuatScalar y45rot4 = TRAP::Math::SLerp(-y90rot, id, TestType(0.5f));
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, -y45rot4, Epsilon)));
            }
            {
                const QuatScalar y90rot3 = TRAP::Math::SLerp(y90rot, y90rot, TestType(0.5f));
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot3, Epsilon)));
            }
            {
                const QuatScalar xz90rot = TRAP::Math::SLerp(id, -y90rot, TestType(0.5f));
                const TestType xz90angle = TRAP::Math::Angle(xz90rot);
                REQUIRE(TRAP::Math::Equal(xz90angle, TRAP::Math::PI<TestType>() * TestType(0.25f), Epsilon));
            }
            {
                [[maybe_unused]] const QuatScalar almostid = TRAP::Math::SLerp(id, TRAP::Math::AngleAxis(TestType(0.1f), Vec3Scalar(0.0f, 1.0f, 0.0f)), TestType(0.5f));
            }
            {
                static constexpr QuatScalar a(-1.0f, 0.0f, 0.0f, 0.0f);
                const QuatScalar result = TRAP::Math::SLerp(a, id, TestType(0.5f));
                REQUIRE(TRAP::Math::Equal(TRAP::Math::Pow(TRAP::Math::Dot(id, result), TestType(2.0f)), TestType(1.0f), TestType(0.01f)));
            }
            {
                const QuatScalar x{1.0f, 0.0f, 0.0f, 0.0f};
                const QuatScalar y{-1.0f, 0.0f, 0.0f, 0.0f};
                const TestType a = TestType(0.5f);

                const QuatScalar result = TRAP::Math::SLerp(x, y, a);
            }
            {
                const QuatScalar x{1.0f, 0.0f, 0.0f, 0.0f};
                const QuatScalar y{0.0f, 1.0f, 0.0f, 0.0f};
                const TestType a = TestType(0.5f);

                const QuatScalar result = TRAP::Math::SLerp(x, y, a);
            }
        }
    }

    SECTION("Edge cases")
    {
        if constexpr(std::floating_point<TestType>)
        {
            using QuatScalar = TRAP::Math::tQuat<TestType>;
            using Vec3Scalar = TRAP::Math::tVec3<TestType>;

            static constexpr TestType min = std::numeric_limits<TestType>::lowest();
            static constexpr TestType max = std::numeric_limits<TestType>::max();
            static constexpr TestType nan = std::numeric_limits<TestType>::quiet_NaN();
            static constexpr TestType inf = std::numeric_limits<TestType>::infinity();
            static constexpr TestType ninf = -std::numeric_limits<TestType>::infinity();

            {
                const QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                const QuatScalar result = TRAP::Math::SLerp(q1, q1, min);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, QuatScalar(0.0f, 0.0f, 0.0f, 0.0f))));
            }
            {
                const QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                const QuatScalar result = TRAP::Math::SLerp(q1, q1, max);
                REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, QuatScalar(0.0f, 0.0f, 0.0f, 0.0f))));
            }
            {
                const QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                const QuatScalar result = TRAP::Math::SLerp(q1, q1, inf);
                REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(result)));
            }
            {
                const QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                const QuatScalar result = TRAP::Math::SLerp(q1, q1, ninf);
                REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(result)));
            }
            {
                const QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                const QuatScalar result = TRAP::Math::SLerp(q1, q1, nan);
                REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(result)));
            }
        }
    }

    SECTION("Spin normal cases - GCEM")
    {
        if constexpr(!std::floating_point<TestType>)
        {
            {
                using T = f32;
                using S = TestType;

                using QuatScalar = TRAP::Math::tQuat<T>;

                static constexpr T Epsilon = T(0.00001f);

                static constexpr T sqrt2 = TRAP::Math::Sqrt(T(2.0f)) / T(2.0f);
                static constexpr QuatScalar id(static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f));
                static constexpr QuatScalar y90rot(sqrt2, 0.0f, sqrt2, 0.0f);
                static constexpr QuatScalar y180rot(0.0f, 0.0f, 1.0f, 0.0f);

                {
                    static constexpr QuatScalar id2 = TRAP::Math::SLerp(id, id, T(1.0f), S(1));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(id, id2, Epsilon)));
                }
                {
                    static constexpr QuatScalar id3 = TRAP::Math::SLerp(id, id, T(1.0f), S(2));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(id, id3, Epsilon)));
                }
                {
                    static constexpr QuatScalar y90rot2 = TRAP::Math::SLerp(id, y90rot, T(1.0f), S(1));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, -y90rot2, Epsilon)));
                }
                {
                    static constexpr QuatScalar y90rot3 = TRAP::Math::SLerp(id, y90rot, T(8.0f) / T(9.0f), S(2));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(id, y90rot3, Epsilon)));
                }
                {
                    static constexpr QuatScalar y90rot4 = TRAP::Math::SLerp(id, y90rot, T(0.2f), S(1));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot4, Epsilon)));
                }

                static constexpr QuatScalar ym45rot3 = TRAP::Math::SLerp(y90rot, id, T(0.5f));
                static constexpr QuatScalar ym45rot2 = TRAP::Math::SLerp(y90rot, id, T(0.9f), S(1));

                {
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(-ym45rot2, ym45rot3, Epsilon)));
                }
                {
                    static constexpr QuatScalar y45rot3 = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(0));
                    static constexpr T y45angle3 = TRAP::Math::Angle(y45rot3);
                    static_assert(TRAP::Math::Equal(y45angle3, TRAP::Math::PI<T>() * T(0.25f), Epsilon));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(ym45rot3, y45rot3, Epsilon)));
                }
                {
                    static constexpr QuatScalar y45rot4 = TRAP::Math::SLerp(-y90rot, id, T(0.5f), S(0));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, y45rot4, Epsilon)));
                }
                if constexpr(std::same_as<T, f32>)
                {
                    static constexpr QuatScalar y90rot5 = TRAP::Math::SLerp(y90rot, y90rot, T(0.5f), S(2));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot5, Epsilon)));
                }
                {
                    static constexpr QuatScalar xz90rot = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(1));
                    static constexpr T xz90angle = TRAP::Math::Angle(xz90rot);
                    static_assert(TRAP::Math::Equal(xz90angle, TRAP::Math::PI<T>() * T(1.25f), Epsilon));
                }

                if constexpr(std::signed_integral<S>)
                {
                    static constexpr QuatScalar neg90rot = TRAP::Math::SLerp(id, y90rot, T(2.0f) / T(3.0f), S(-1));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(y180rot, -neg90rot, Epsilon)));
                }
            }
            {
                using T = f64;
                using S = TestType;

                using QuatScalar = TRAP::Math::tQuat<T>;

                static constexpr T Epsilon = T(0.00001f);

                static constexpr T sqrt2 = TRAP::Math::Sqrt(T(2.0f)) / T(2.0f);
                static constexpr QuatScalar id(static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f));
                static constexpr QuatScalar y90rot(sqrt2, 0.0f, sqrt2, 0.0f);
                static constexpr QuatScalar y180rot(0.0f, 0.0f, 1.0f, 0.0f);

                {
                    static constexpr QuatScalar id2 = TRAP::Math::SLerp(id, id, T(1.0f), S(1));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(id, id2, Epsilon)));
                }
                {
                    static constexpr QuatScalar id3 = TRAP::Math::SLerp(id, id, T(1.0f), S(2));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(id, id3, Epsilon)));
                }
                {
                    static constexpr QuatScalar y90rot2 = TRAP::Math::SLerp(id, y90rot, T(1.0f), S(1));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, -y90rot2, Epsilon)));
                }
                {
                    static constexpr QuatScalar y90rot3 = TRAP::Math::SLerp(id, y90rot, T(8.0f) / T(9.0f), S(2));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(id, y90rot3, Epsilon)));
                }
                {
                    static constexpr QuatScalar y90rot4 = TRAP::Math::SLerp(id, y90rot, T(0.2f), S(1));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot4, Epsilon)));
                }

                static constexpr QuatScalar ym45rot3 = TRAP::Math::SLerp(y90rot, id, T(0.5f));
                static constexpr QuatScalar ym45rot2 = TRAP::Math::SLerp(y90rot, id, T(0.9f), S(1));

                {
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(-ym45rot2, ym45rot3, Epsilon)));
                }
                {
                    static constexpr QuatScalar y45rot3 = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(0));
                    static constexpr T y45angle3 = TRAP::Math::Angle(y45rot3);
                    static_assert(TRAP::Math::Equal(y45angle3, TRAP::Math::PI<T>() * T(0.25f), Epsilon));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(ym45rot3, y45rot3, Epsilon)));
                }
                {
                    static constexpr QuatScalar y45rot4 = TRAP::Math::SLerp(-y90rot, id, T(0.5f), S(0));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, y45rot4, Epsilon)));
                }
                if constexpr(std::same_as<T, f32>)
                {
                    static constexpr QuatScalar y90rot5 = TRAP::Math::SLerp(y90rot, y90rot, T(0.5f), S(2));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot5, Epsilon)));
                }
                {
                    static constexpr QuatScalar xz90rot = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(1));
                    static constexpr T xz90angle = TRAP::Math::Angle(xz90rot);
                    static_assert(TRAP::Math::Equal(xz90angle, TRAP::Math::PI<T>() * T(1.25f), Epsilon));
                }

                if constexpr(std::signed_integral<S>)
                {
                    static constexpr QuatScalar neg90rot = TRAP::Math::SLerp(id, y90rot, T(2.0f) / T(3.0f), S(-1));
                    static_assert(TRAP::Math::All(TRAP::Math::Equal(y180rot, -neg90rot, Epsilon)));
                }
            }
        }
    }

    SECTION("Spin normal cases - std")
    {
        if constexpr(!std::floating_point<TestType>)
        {
            {
                using T = f32;
                using S = TestType;

                using QuatScalar = TRAP::Math::tQuat<T>;

                static constexpr T Epsilon = T(0.00001f);

                static constexpr T sqrt2 = TRAP::Math::Sqrt(T(2.0f)) / T(2.0f);
                static constexpr QuatScalar id(static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f));
                static constexpr QuatScalar y90rot(sqrt2, 0.0f, sqrt2, 0.0f);
                static constexpr QuatScalar y180rot(0.0f, 0.0f, 1.0f, 0.0f);

                {
                    const QuatScalar id2 = TRAP::Math::SLerp(id, id, T(1.0f), S(1));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, id2, Epsilon)));
                }
                {
                    const QuatScalar id3 = TRAP::Math::SLerp(id, id, T(1.0f), S(2));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, id3, Epsilon)));
                }
                {
                    const QuatScalar y90rot2 = TRAP::Math::SLerp(id, y90rot, T(1.0f), S(1));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, -y90rot2, Epsilon)));
                }
                {
                    const QuatScalar y90rot3 = TRAP::Math::SLerp(id, y90rot, T(8.0f) / T(9.0f), S(2));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, y90rot3, Epsilon)));
                }
                {
                    const QuatScalar y90rot4 = TRAP::Math::SLerp(id, y90rot, T(0.2f), S(1));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot4, Epsilon)));
                }

                const QuatScalar ym45rot3 = TRAP::Math::SLerp(y90rot, id, T(0.5f));
                const QuatScalar ym45rot2 = TRAP::Math::SLerp(y90rot, id, T(0.9f), S(1));

                {
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(-ym45rot2, ym45rot3, Epsilon)));
                }
                {
                    const QuatScalar y45rot3 = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(0));
                    const T y45angle3 = TRAP::Math::Angle(y45rot3);
                    REQUIRE(TRAP::Math::Equal(y45angle3, TRAP::Math::PI<T>() * T(0.25f), Epsilon));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ym45rot3, y45rot3, Epsilon)));
                }
                {
                    const QuatScalar y45rot4 = TRAP::Math::SLerp(-y90rot, id, T(0.5f), S(0));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, y45rot4, Epsilon)));
                }
                if constexpr(std::same_as<T, f32>)
                {
                    const QuatScalar y90rot5 = TRAP::Math::SLerp(y90rot, y90rot, T(0.5f), S(2));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot5, Epsilon)));
                }
                {
                    const QuatScalar xz90rot = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(1));
                    const T xz90angle = TRAP::Math::Angle(xz90rot);
                    REQUIRE(TRAP::Math::Equal(xz90angle, TRAP::Math::PI<T>() * T(1.25f), Epsilon));
                }

                if constexpr(std::signed_integral<S>)
                {
                    const QuatScalar neg90rot = TRAP::Math::SLerp(id, y90rot, T(2.0f) / T(3.0f), S(-1));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y180rot, -neg90rot, Epsilon)));
                }
            }
            {
                using T = f64;
                using S = TestType;

                using QuatScalar = TRAP::Math::tQuat<T>;

                static constexpr T Epsilon = T(0.00001f);

                static constexpr T sqrt2 = TRAP::Math::Sqrt(T(2.0f)) / T(2.0f);
                static constexpr QuatScalar id(static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f));
                static constexpr QuatScalar y90rot(sqrt2, 0.0f, sqrt2, 0.0f);
                static constexpr QuatScalar y180rot(0.0f, 0.0f, 1.0f, 0.0f);

                {
                    const QuatScalar id2 = TRAP::Math::SLerp(id, id, T(1.0f), S(1));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, id2, Epsilon)));
                }
                {
                    const QuatScalar id3 = TRAP::Math::SLerp(id, id, T(1.0f), S(2));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, id3, Epsilon)));
                }
                {
                    const QuatScalar y90rot2 = TRAP::Math::SLerp(id, y90rot, T(1.0f), S(1));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, -y90rot2, Epsilon)));
                }
                {
                    const QuatScalar y90rot3 = TRAP::Math::SLerp(id, y90rot, T(8.0f) / T(9.0f), S(2));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(id, y90rot3, Epsilon)));
                }
                {
                    const QuatScalar y90rot4 = TRAP::Math::SLerp(id, y90rot, T(0.2f), S(1));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot4, Epsilon)));
                }

                const QuatScalar ym45rot3 = TRAP::Math::SLerp(y90rot, id, T(0.5f));
                const QuatScalar ym45rot2 = TRAP::Math::SLerp(y90rot, id, T(0.9f), S(1));

                {
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(-ym45rot2, ym45rot3, Epsilon)));
                }
                {
                    const QuatScalar y45rot3 = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(0));
                    const T y45angle3 = TRAP::Math::Angle(y45rot3);
                    REQUIRE(TRAP::Math::Equal(y45angle3, TRAP::Math::PI<T>() * T(0.25f), Epsilon));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ym45rot3, y45rot3, Epsilon)));
                }
                {
                    const QuatScalar y45rot4 = TRAP::Math::SLerp(-y90rot, id, T(0.5f), S(0));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(ym45rot2, y45rot4, Epsilon)));
                }
                if constexpr(std::same_as<T, f32>)
                {
                    const QuatScalar y90rot5 = TRAP::Math::SLerp(y90rot, y90rot, T(0.5f), S(2));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y90rot, y90rot5, Epsilon)));
                }
                {
                    const QuatScalar xz90rot = TRAP::Math::SLerp(id, -y90rot, T(0.5f), S(1));
                    const T xz90angle = TRAP::Math::Angle(xz90rot);
                    REQUIRE(TRAP::Math::Equal(xz90angle, TRAP::Math::PI<T>() * T(1.25f), Epsilon));
                }

                if constexpr(std::signed_integral<S>)
                {
                    const QuatScalar neg90rot = TRAP::Math::SLerp(id, y90rot, T(2.0f) / T(3.0f), S(-1));
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(y180rot, -neg90rot, Epsilon)));
                }

                {
                    const QuatScalar x{1.0f, 0.0f, 0.0f, 0.0f};
                    const QuatScalar y{-1.0f, 0.0f, 0.0f, 0.0f};
                    const T a = T(0.5f);

                    const QuatScalar result = TRAP::Math::SLerp(x, y, a, S(1));
                }
                {
                    const QuatScalar x{1.0f, 0.0f, 0.0f, 0.0f};
                    const QuatScalar y{0.0f, 1.0f, 0.0f, 0.0f};
                    const T a = T(0.5f);

                    const QuatScalar result = TRAP::Math::SLerp(x, y, a, S(1));
                }
            }
        }
    }

    SECTION("Spin edge cases")
    {
        if constexpr(!std::floating_point<TestType>)
        {
            {
                using T = f32;
                using S = TestType;

                using QuatScalar = TRAP::Math::tQuat<T>;
                using Vec3Scalar = TRAP::Math::tVec3<T>;

                static constexpr S min = std::numeric_limits<S>::min();
                static constexpr S max = std::numeric_limits<S>::max();

                {
                    const QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                    const QuatScalar result = TRAP::Math::SLerp(q1, q1, T(1.0f), min);
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, QuatScalar(1.0f, 0.0f, 0.0f, 0.0f))));
                }
                {
                    const QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                    const QuatScalar result = TRAP::Math::SLerp(q1, q1, T(1.0f), max);
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, QuatScalar(1.0f, 0.0f, 0.0f, 0.0f))));
                }
            }
            {
                using T = f64;
                using S = TestType;

                using QuatScalar = TRAP::Math::tQuat<T>;
                using Vec3Scalar = TRAP::Math::tVec3<T>;

                static constexpr S min = std::numeric_limits<S>::min();
                static constexpr S max = std::numeric_limits<S>::max();

                {
                    const QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                    const QuatScalar result = TRAP::Math::SLerp(q1, q1, T(1.0f), min);
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, QuatScalar(1.0f, 0.0f, 0.0f, 0.0f))));
                }
                {
                    const QuatScalar q1(Vec3Scalar(1.0f, 0.0f, 0.0f), Vec3Scalar(1.0f, 0.0f, 0.0f));
                    const QuatScalar result = TRAP::Math::SLerp(q1, q1, T(1.0f), max);
                    REQUIRE(TRAP::Math::All(TRAP::Math::Equal(result, QuatScalar(1.0f, 0.0f, 0.0f, 0.0f))));
                }
            }
        }
    }
}

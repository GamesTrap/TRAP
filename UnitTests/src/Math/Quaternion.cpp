#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Quat", "[math][quat]", TRAP::Math::Quatf, TRAP::Math::Quatd)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;

    SECTION("Class requirements")
    {
        STATIC_REQUIRE_FALSE(std::is_final_v<TestType>);
        STATIC_REQUIRE(std::copyable<TestType>);
        STATIC_REQUIRE(std::movable<TestType>);
    }

    SECTION("Typedefs")
    {
        STATIC_REQUIRE(std::same_as<typename TestType::value_type, Scalar>);
        STATIC_REQUIRE(std::same_as<typename TestType::pointer, Scalar*>);
        STATIC_REQUIRE(std::same_as<typename TestType::const_pointer, const Scalar*>);
        STATIC_REQUIRE(std::same_as<typename TestType::reference, Scalar&>);
        STATIC_REQUIRE(std::same_as<typename TestType::const_reference, const Scalar&>);
        STATIC_REQUIRE(std::same_as<typename TestType::size_type, u32>);
        STATIC_REQUIRE(std::same_as<typename TestType::difference_type, isize>);
    }

    SECTION("Constructors")
    {
        //Default constructor
        {
            static constexpr TestType q{};

            STATIC_REQUIRE(q.x() == Scalar(0));
            STATIC_REQUIRE(q.y() == Scalar(0));
            STATIC_REQUIRE(q.z() == Scalar(0));
            STATIC_REQUIRE(q.w() == Scalar(0));
        }

        //Move constructor
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            TestType q1(std::move(q));

            REQUIRE(q1.x() == Scalar(2));
            REQUIRE(q1.y() == Scalar(3));
            REQUIRE(q1.z() == Scalar(4));
            REQUIRE(q1.w() == Scalar(1));
        }

        //Copy constructor
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1(q);

            STATIC_REQUIRE(q1.x() == Scalar(2));
            STATIC_REQUIRE(q1.y() == Scalar(3));
            STATIC_REQUIRE(q1.z() == Scalar(4));
            STATIC_REQUIRE(q1.w() == Scalar(1));
        }

        {
            static constexpr TestType q(Scalar(5), Vec3Scalar(Scalar(1), Scalar(2), Scalar(3)));

            STATIC_REQUIRE(q.x() == Scalar(1));
            STATIC_REQUIRE(q.y() == Scalar(2));
            STATIC_REQUIRE(q.z() == Scalar(3));
            STATIC_REQUIRE(q.w() == Scalar(5));
        }

        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));

            STATIC_REQUIRE(q.x() == Scalar(2));
            STATIC_REQUIRE(q.y() == Scalar(3));
            STATIC_REQUIRE(q.z() == Scalar(4));
            STATIC_REQUIRE(q.w() == Scalar(1));
        }

        {
            static constexpr TestType q(TRAP::Math::tQuat<f64>(f64(1.0), f64(2.0), f64(3.0), f64(4.0)));

            STATIC_REQUIRE(q.x() == Scalar(2));
            STATIC_REQUIRE(q.y() == Scalar(3));
            STATIC_REQUIRE(q.z() == Scalar(4));
            STATIC_REQUIRE(q.w() == Scalar(1));
        }

        {
            static constexpr TestType q(Vec3Scalar(Scalar(1), Scalar(2), Scalar(3)), Vec3Scalar(Scalar(3), Scalar(2), Scalar(1)));

            STATIC_REQUIRE(TRAP::Math::Equal(q.x(), Scalar(-0.154303), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(q.y(), Scalar(0.308607), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(q.z(), Scalar(-0.154303), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(q.w(), Scalar(0.925820), Scalar(0.000001)));

            static constexpr TestType q1(Vec3Scalar(Scalar(1), Scalar(0), Scalar(0)), Vec3Scalar(Scalar(-1), Scalar(0), Scalar(0)));

            STATIC_REQUIRE(q1.x() == Scalar(0));
            STATIC_REQUIRE(q1.y() == Scalar(1));
            STATIC_REQUIRE(q1.z() == Scalar(0));
            STATIC_REQUIRE(q1.w() == Scalar(0));

            const TestType q2(Vec3Scalar(Scalar(1), Scalar(0), Scalar(0)), Vec3Scalar(Scalar(-1), Scalar(0), Scalar(0)));

            REQUIRE(q2.x() == Scalar(0));
            REQUIRE(q2.y() == Scalar(1));
            REQUIRE(q2.z() == Scalar(0));
            REQUIRE(q2.w() == Scalar(0));
        }

        {
            static constexpr TestType q(Vec3Scalar(Scalar(1), Scalar(2), Scalar(3)));

            STATIC_REQUIRE(TRAP::Math::Equal(q.x(), Scalar(-0.718287), Scalar(0.0000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(q.y(), Scalar(0.310622), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(q.z(), Scalar(0.444435), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(q.w(), Scalar(0.435953), Scalar(0.000001)));
        }

        {
            static constexpr TRAP::Math::tMat3<Scalar> m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType q(m);

            STATIC_REQUIRE(q.x() == Scalar(-0.25));
            STATIC_REQUIRE(q.y() == Scalar(0.5));
            STATIC_REQUIRE(q.z() == Scalar(-0.25));
            STATIC_REQUIRE(q.w() == Scalar(2.0));
        }

        {
            static constexpr TRAP::Math::tMat4<Scalar> m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType q(m);

            STATIC_REQUIRE(TRAP::Math::Equal(q.x(), Scalar(-0.344124), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(q.y(), Scalar(0.688247), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(q.z(), Scalar(-0.344124), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(q.w(), Scalar(2.179450), Scalar(0.000001)));
        }
    }

    SECTION("Assignments")
    {
        //Move assignment
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            TestType qMove{};
            qMove = std::move(q);

            REQUIRE(qMove.x() == Scalar(2));
            REQUIRE(qMove.y() == Scalar(3));
            REQUIRE(qMove.z() == Scalar(4));
            REQUIRE(qMove.w() == Scalar(1));
        }

        //Copy assignment
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            TestType qCopy{};
            qCopy = q;

            REQUIRE(qCopy.x() == Scalar(2));
            REQUIRE(qCopy.y() == Scalar(3));
            REQUIRE(qCopy.z() == Scalar(4));
            REQUIRE(qCopy.w() == Scalar(1));
        }

        //Conversion assignment
        {
            static constexpr TRAP::Math::tQuat<f64> q(f64(1), f64(2), f64(3), f64(4));
            TestType qConversion{};
            qConversion = q;

            REQUIRE(qConversion.x() == Scalar(2));
            REQUIRE(qConversion.y() == Scalar(3));
            REQUIRE(qConversion.z() == Scalar(4));
            REQUIRE(qConversion.w() == Scalar(1));
        }
    }

    SECTION("Accessors")
    {
        //Length
        {
            STATIC_REQUIRE(TestType::Length() == 4u);
        }

        //Component access
        {
            static constexpr TestType v(Scalar(1), Scalar(2), Scalar(3), Scalar(4));

            STATIC_REQUIRE(v.x() == Scalar(2));
            STATIC_REQUIRE(v.y() == Scalar(3));
            STATIC_REQUIRE(v.z() == Scalar(4));
            STATIC_REQUIRE(v.w() == Scalar(1));
            //Check that v.x() returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>));

            TestType v2(Scalar(1), Scalar(2), Scalar(3), Scalar(4));

            REQUIRE(v2.x() == Scalar(2));
            REQUIRE(v2.y() == Scalar(3));
            REQUIRE(v2.z() == Scalar(4));
            REQUIRE(v2.w() == Scalar(1));
            //Check that v2.x() returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2.x())> && !std::is_const_v<std::remove_reference_t<decltype(v2.x())>>));
        }

        //operator[]
        {
            static constexpr TestType v(Scalar(1), Scalar(2), Scalar(3), Scalar(4));

            STATIC_REQUIRE(v[0] == Scalar(1));
            STATIC_REQUIRE(v[1] == Scalar(2));
            STATIC_REQUIRE(v[2] == Scalar(3));
            STATIC_REQUIRE(v[3] == Scalar(4));
            //Check that v[0] returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>));

            TestType v2(Scalar(1), Scalar(2), Scalar(3), Scalar(4));

            REQUIRE(v2[0] == Scalar(1));
            REQUIRE(v2[1] == Scalar(2));
            REQUIRE(v2[2] == Scalar(3));
            REQUIRE(v2[3] == Scalar(4));
            //Check that v2[0] returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2[0])> && !std::is_const_v<std::remove_reference_t<decltype(v2[0])>>));
        }

        //at()
        {
            static constexpr TestType v(Scalar(1), Scalar(2), Scalar(3), Scalar(4));

            STATIC_REQUIRE(v.at(0) == Scalar(1));
            STATIC_REQUIRE(v.at(1) == Scalar(2));
            STATIC_REQUIRE(v.at(2) == Scalar(3));
            STATIC_REQUIRE(v.at(3) == Scalar(4));
            REQUIRE_THROWS_AS(v.at(10), std::out_of_range);
            //Check that v.at(0) returns const T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v.at(0))> && std::is_const_v<std::remove_reference_t<decltype(v.at(0))>>));

            TestType v2(Scalar(1), Scalar(2), Scalar(3), Scalar(4));

            REQUIRE(v2.at(0) == Scalar(1));
            REQUIRE(v2.at(1) == Scalar(2));
            REQUIRE(v2.at(2) == Scalar(3));
            REQUIRE(v2.at(3) == Scalar(4));
            REQUIRE_THROWS_AS(v2.at(10), std::out_of_range);
            //Check that v2.at(0) returns T&
            STATIC_REQUIRE((std::is_reference_v<decltype(v2.at(0))> && !std::is_const_v<std::remove_reference_t<decltype(v2.at(0))>>));
        }
    }

    SECTION("Operators")
    {
        //Unary operator+
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1 = +q;

            STATIC_REQUIRE(q1.x() == Scalar(2));
            STATIC_REQUIRE(q1.y() == Scalar(3));
            STATIC_REQUIRE(q1.z() == Scalar(4));
            STATIC_REQUIRE(q1.w() == Scalar(1));
        }

        //Unary operator-
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1 = -q;

            STATIC_REQUIRE(q1.x() == Scalar(-2));
            STATIC_REQUIRE(q1.y() == Scalar(-3));
            STATIC_REQUIRE(q1.z() == Scalar(-4));
            STATIC_REQUIRE(q1.w() == Scalar(-1));
        }

        //Binary operator+
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1 = q + q;

            STATIC_REQUIRE(q1.x() == Scalar(4));
            STATIC_REQUIRE(q1.y() == Scalar(6));
            STATIC_REQUIRE(q1.z() == Scalar(8));
            STATIC_REQUIRE(q1.w() == Scalar(2));
        }

        //Binary operator-
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1 = q - q;

            STATIC_REQUIRE(q1.x() == Scalar(0));
            STATIC_REQUIRE(q1.y() == Scalar(0));
            STATIC_REQUIRE(q1.z() == Scalar(0));
            STATIC_REQUIRE(q1.w() == Scalar(0));
        }

        //Binary operator*
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1 = q * q;

            STATIC_REQUIRE(q1.x() == Scalar(4));
            STATIC_REQUIRE(q1.y() == Scalar(6));
            STATIC_REQUIRE(q1.z() == Scalar(8));
            STATIC_REQUIRE(q1.w() == Scalar(-28));
        }

        //Binary operator*
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr Vec3Scalar v = q * Vec3Scalar(Scalar(1), Scalar(2), Scalar(3));

            STATIC_REQUIRE(v.x() == Scalar(25));
            STATIC_REQUIRE(v.y() == Scalar(2));
            STATIC_REQUIRE(v.z() == Scalar(-9));

            const TestType q1(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            const Vec3Scalar v1 = q1 * Vec3Scalar(Scalar(1), Scalar(2), Scalar(3));

            REQUIRE(v1.x() == Scalar(25));
            REQUIRE(v1.y() == Scalar(2));
            REQUIRE(v1.z() == Scalar(-9));
        }

        //Binary operator*
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr Vec3Scalar v = Vec3Scalar(Scalar(1), Scalar(2), Scalar(3)) * q;

            STATIC_REQUIRE(TRAP::Math::Equal(v.x(), Scalar(1.022222), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(v.y(), Scalar(2.008889), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(v.z(), Scalar(2.982222), Scalar(0.000001)));
        }

        //Binary operator*
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TRAP::Math::tVec4<Scalar> v = q * TRAP::Math::tVec4<Scalar>(Scalar(1), Scalar(2), Scalar(3), Scalar(4));

            STATIC_REQUIRE(v.x() == Scalar(25));
            STATIC_REQUIRE(v.y() == Scalar(2));
            STATIC_REQUIRE(v.z() == Scalar(-9));
            STATIC_REQUIRE(v.w() == Scalar(4));
        }

        //Binary operator*
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TRAP::Math::tVec4<Scalar> v = TRAP::Math::tVec4<Scalar>(Scalar(1), Scalar(2), Scalar(3), Scalar(4)) * q;

            STATIC_REQUIRE(TRAP::Math::Equal(v.x(), Scalar(1.022222), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(v.y(), Scalar(2.008889), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(v.z(), Scalar(2.982222), Scalar(0.000001)));
            STATIC_REQUIRE(TRAP::Math::Equal(v.w(), Scalar(4.0), Scalar(0.000001)));
        }

        //Binary operator*
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1 = q * Scalar(2);

            STATIC_REQUIRE(q1.x() == Scalar(4));
            STATIC_REQUIRE(q1.y() == Scalar(6));
            STATIC_REQUIRE(q1.z() == Scalar(8));
            STATIC_REQUIRE(q1.w() == Scalar(2));
        }

        //Binary operator*
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1 = Scalar(2) * q;

            STATIC_REQUIRE(q1.x() == Scalar(4));
            STATIC_REQUIRE(q1.y() == Scalar(6));
            STATIC_REQUIRE(q1.z() == Scalar(8));
            STATIC_REQUIRE(q1.w() == Scalar(2));
        }

        //Binary operator/
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1 = q / Scalar(2);

            STATIC_REQUIRE(q1.x() == Scalar(1));
            STATIC_REQUIRE(q1.y() == Scalar(1.5));
            STATIC_REQUIRE(q1.z() == Scalar(2));
            STATIC_REQUIRE(q1.w() == Scalar(0.5));
        }

        //operator==
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1(Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            STATIC_REQUIRE(!(q == q1));
            STATIC_REQUIRE(q == q);
        }

        //operator!=
        {
            static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            static constexpr TestType q1(Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            STATIC_REQUIRE(q != q1);
            STATIC_REQUIRE(!(q != q));
        }

        //operator +=
        {
            TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            q += TestType(Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            REQUIRE(q.x() == Scalar(5));
            REQUIRE(q.y() == Scalar(5));
            REQUIRE(q.z() == Scalar(5));
            REQUIRE(q.w() == Scalar(5));
        }

        //operator -=
        {
            TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            q -= TestType(Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            REQUIRE(q.x() == Scalar(-1));
            REQUIRE(q.y() == Scalar(1));
            REQUIRE(q.z() == Scalar(3));
            REQUIRE(q.w() == Scalar(-3));
        }

        //operator *=
        {
            TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            q *= TestType(Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            REQUIRE(q.x() == Scalar(6));
            REQUIRE(q.y() == Scalar(24));
            REQUIRE(q.z() == Scalar(12));
            REQUIRE(q.w() == Scalar(-12));
        }

        //operator *=
        {
            TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            q *= 2.0;

            REQUIRE(q.x() == Scalar(4));
            REQUIRE(q.y() == Scalar(6));
            REQUIRE(q.z() == Scalar(8));
            REQUIRE(q.w() == Scalar(2));
        }

        //operator /=
        {
            TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            q /= 2.0;

            REQUIRE(q.x() == Scalar(1));
            REQUIRE(q.y() == Scalar(1.5));
            REQUIRE(q.z() == Scalar(2));
            REQUIRE(q.w() == Scalar(0.5));
        }
    }

    SECTION("std::hash")
    {
        usize hash = std::hash<TestType>()(TestType(Scalar(1), Scalar(2), Scalar(3), Scalar(4)));
    }

    SECTION("fmt::format")
    {
        static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
        std::string str = fmt::format("{}", q);

		if constexpr(std::same_as<Scalar, f32>)
        {
            REQUIRE(str == fmt::format("Quatf({}, {{{}, {}, {}}})", q.w(), q.x(), q.y(), q.z()));
        }
		else if constexpr(std::same_as<Scalar, f64>)
        {
            REQUIRE(str == fmt::format("Quatd({}, {{{}, {}, {}}})", q.w(), q.x(), q.y(), q.z()));
        }
    }

    SECTION("std::get")
    {
        static constexpr TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));

        STATIC_REQUIRE(std::get<0u>(q) == Scalar(1));
        STATIC_REQUIRE(std::get<1u>(q) == Scalar(2));
        STATIC_REQUIRE(std::get<2u>(q) == Scalar(3));
        STATIC_REQUIRE(std::get<3u>(q) == Scalar(4));
    }

    SECTION("std::swap")
    {
        {
            TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            TestType q1(Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            q.Swap(q1);

            REQUIRE(q.w() == Scalar(4));
            REQUIRE(q.x() == Scalar(3));
            REQUIRE(q.y() == Scalar(2));
            REQUIRE(q.z() == Scalar(1));
            REQUIRE(q1.w() == Scalar(1));
            REQUIRE(q1.x() == Scalar(2));
            REQUIRE(q1.y() == Scalar(3));
            REQUIRE(q1.z() == Scalar(4));
        }
        {
            TestType q(Scalar(1), Scalar(2), Scalar(3), Scalar(4));
            TestType q1(Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            std::swap(q, q1);

            REQUIRE(q.w() == Scalar(4));
            REQUIRE(q.x() == Scalar(3));
            REQUIRE(q.y() == Scalar(2));
            REQUIRE(q.z() == Scalar(1));
            REQUIRE(q1.w() == Scalar(1));
            REQUIRE(q1.x() == Scalar(2));
            REQUIRE(q1.y() == Scalar(3));
            REQUIRE(q1.z() == Scalar(4));
        }
    }
}

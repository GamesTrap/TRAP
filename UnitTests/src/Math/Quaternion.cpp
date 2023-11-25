#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunQuatTypedefsCompileTimeTests()
    {
        using Quat = TRAP::Math::tQuat<T>;

        static_assert(std::same_as<typename Quat::value_type, T>);
        static_assert(std::same_as<typename Quat::pointer, T*>);
        static_assert(std::same_as<typename Quat::const_pointer, const T*>);
        static_assert(std::same_as<typename Quat::reference, T&>);
        static_assert(std::same_as<typename Quat::const_reference, const T&>);
        static_assert(std::same_as<typename Quat::size_type, u32>);
        static_assert(std::same_as<typename Quat::difference_type, isize>);
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunQuatConstructorCompileTimeTests()
    {
        using Quat = TRAP::Math::tQuat<T>;

        //Default constructor
        {
            constexpr Quat q{};

            static_assert(q.x() == T(0));
            static_assert(q.y() == T(0));
            static_assert(q.z() == T(0));
            static_assert(q.w() == T(0));
        }

        //Move constructor
        {
        }

        //Copy constructor
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1(q);

            static_assert(q1.x() == T(2));
            static_assert(q1.y() == T(3));
            static_assert(q1.z() == T(4));
            static_assert(q1.w() == T(1));
        }

        {
            constexpr Quat q(T(5), TRAP::Math::tVec3<T>(T(1), T(2), T(3)));

            static_assert(q.x() == T(1));
            static_assert(q.y() == T(2));
            static_assert(q.z() == T(3));
            static_assert(q.w() == T(5));
        }

        {
            constexpr Quat q(T(1), T(2), T(3), T(4));

            static_assert(q.x() == T(2));
            static_assert(q.y() == T(3));
            static_assert(q.z() == T(4));
            static_assert(q.w() == T(1));
        }

        {
            constexpr Quat q(TRAP::Math::tQuat<f64>(f64(1.0), f64(2.0), f64(3.0), f64(4.0)));

            static_assert(q.x() == T(2));
            static_assert(q.y() == T(3));
            static_assert(q.z() == T(4));
            static_assert(q.w() == T(1));
        }

        {
            constexpr Quat q(TRAP::Math::tVec3<T>(T(1), T(2), T(3)), TRAP::Math::tVec3<T>(T(3), T(2), T(1)));

            static_assert(TRAP::Math::Equal(q.x(), T(-0.154303), T(0.000001)));
            static_assert(TRAP::Math::Equal(q.y(), T(0.308607), T(0.000001)));
            static_assert(TRAP::Math::Equal(q.z(), T(-0.154303), T(0.000001)));
            static_assert(TRAP::Math::Equal(q.w(), T(0.925820), T(0.000001)));

            constexpr Quat q1(TRAP::Math::tVec3<T>(T(1), T(0), T(0)), TRAP::Math::tVec3<T>(T(-1), T(0), T(0)));

            static_assert(q1.x() == T(0));
            static_assert(q1.y() == T(1));
            static_assert(q1.z() == T(0));
            static_assert(q1.w() == T(0));
        }

        {
            constexpr Quat q(TRAP::Math::tVec3<T>(T(1), T(2), T(3)));

            static_assert(TRAP::Math::Equal(q.x(), T(-0.718287), T(0.0000001)));
            static_assert(TRAP::Math::Equal(q.y(), T(0.310622), T(0.000001)));
            static_assert(TRAP::Math::Equal(q.z(), T(0.444435), T(0.000001)));
            static_assert(TRAP::Math::Equal(q.w(), T(0.435953), T(0.000001)));
        }

        {
            constexpr TRAP::Math::tMat3<T> m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Quat q(m);

            static_assert(q.x() == T(-0.25));
            static_assert(q.y() == T(0.5));
            static_assert(q.z() == T(-0.25));
            static_assert(q.w() == T(2.0));
        }

        {
            constexpr TRAP::Math::tMat4<T> m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Quat q(m);

            static_assert(TRAP::Math::Equal(q.x(), T(-0.344124), T(0.000001)));
            static_assert(TRAP::Math::Equal(q.y(), T(0.688247), T(0.000001)));
            static_assert(TRAP::Math::Equal(q.z(), T(-0.344124), T(0.000001)));
            static_assert(TRAP::Math::Equal(q.w(), T(2.179450), T(0.000001)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunQuatConstructorRunTimeTests()
    {
        using Quat = TRAP::Math::tQuat<T>;

        //Default constructor
        {
            Quat q{};

            REQUIRE(q.x() == T(0));
            REQUIRE(q.y() == T(0));
            REQUIRE(q.z() == T(0));
            REQUIRE(q.w() == T(0));
        }

        //Move constructor
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1(std::move(q));

            REQUIRE(q1.x() == T(2));
            REQUIRE(q1.y() == T(3));
            REQUIRE(q1.z() == T(4));
            REQUIRE(q1.w() == T(1));
        }

        //Copy constructor
        {
            const Quat q(T(1), T(2), T(3), T(4));
            Quat q1(q);

            REQUIRE(q1.x() == T(2));
            REQUIRE(q1.y() == T(3));
            REQUIRE(q1.z() == T(4));
            REQUIRE(q1.w() == T(1));
        }

        //Copy conversion constructor
        {
            const TRAP::Math::tQuat<f64> q(T(1), T(2), T(3), T(4));
            Quat q1(q);

            REQUIRE(q1.x() == T(2));
            REQUIRE(q1.y() == T(3));
            REQUIRE(q1.z() == T(4));
            REQUIRE(q1.w() == T(1));
        }

        {
            Quat q(T(5), TRAP::Math::tVec3<T>(T(1), T(2), T(3)));

            REQUIRE(q.x() == T(1));
            REQUIRE(q.y() == T(2));
            REQUIRE(q.z() == T(3));
            REQUIRE(q.w() == T(5));
        }

        {
            Quat q(T(1), T(2), T(3), T(4));

            REQUIRE(q.x() == T(2));
            REQUIRE(q.y() == T(3));
            REQUIRE(q.z() == T(4));
            REQUIRE(q.w() == T(1));
        }

        {
            Quat q(TRAP::Math::tQuat<f64>(f64(1.0), f64(2.0), f64(3.0), f64(4.0)));

            REQUIRE(q.x() == T(2));
            REQUIRE(q.y() == T(3));
            REQUIRE(q.z() == T(4));
            REQUIRE(q.w() == T(1));
        }

        {
            Quat q(TRAP::Math::tVec3<T>(T(1), T(2), T(3)), TRAP::Math::tVec3<T>(T(3), T(2), T(1)));

            REQUIRE(TRAP::Math::Equal(q.x(), T(-0.154303), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(q.y(), T(0.308607), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(q.z(), T(-0.154303), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(q.w(), T(0.925820), T(0.000001)));

            Quat q1(TRAP::Math::tVec3<T>(T(1), T(0), T(0)), TRAP::Math::tVec3<T>(T(-1), T(0), T(0)));

            REQUIRE(q1.x() == T(0));
            REQUIRE(q1.y() == T(1));
            REQUIRE(q1.z() == T(0));
            REQUIRE(q1.w() == T(0));
        }

        {
            Quat q(TRAP::Math::tVec3<T>(T(1), T(2), T(3)));

            REQUIRE(TRAP::Math::Equal(q.x(), T(-0.718287), T(0.0000001)));
            REQUIRE(TRAP::Math::Equal(q.y(), T(0.310622), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(q.z(), T(0.444435), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(q.w(), T(0.435953), T(0.000001)));
        }

        {
            TRAP::Math::tMat3<T> m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Quat q(m);

            REQUIRE(q.x() == T(-0.25));
            REQUIRE(q.y() == T(0.5));
            REQUIRE(q.z() == T(-0.25));
            REQUIRE(q.w() == T(2.0));
        }

        {
            TRAP::Math::tMat4<T> m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Quat q(m);

            REQUIRE(TRAP::Math::Equal(q.x(), T(-0.344124), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(q.y(), T(0.688247), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(q.z(), T(-0.344124), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(q.w(), T(2.179450), T(0.000001)));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunQuatAssignmentsRunTimeTests()
    {
        using Quat = TRAP::Math::tQuat<T>;

        //Move assignment
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat qMove{};
            qMove = std::move(q);

            REQUIRE(qMove.x() == T(2));
            REQUIRE(qMove.y() == T(3));
            REQUIRE(qMove.z() == T(4));
            REQUIRE(qMove.w() == T(1));
        }

        //Copy assignment
        {
            const Quat q(T(1), T(2), T(3), T(4));
            Quat qCopy{};
            qCopy = q;

            REQUIRE(qCopy.x() == T(2));
            REQUIRE(qCopy.y() == T(3));
            REQUIRE(qCopy.z() == T(4));
            REQUIRE(qCopy.w() == T(1));
        }

        //Conversion assignment
        {
            TRAP::Math::tQuat<f64> q(f64(1), f64(2), f64(3), f64(4));
            Quat qConversion{};
            qConversion = q;

            REQUIRE(qConversion.x() == T(2));
            REQUIRE(qConversion.y() == T(3));
            REQUIRE(qConversion.z() == T(4));
            REQUIRE(qConversion.w() == T(1));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunQuatAccessorCompileTimeTests()
    {
        using Quat = TRAP::Math::tQuat<T>;

        //Length
        {
            static_assert(Quat::Length() == 4);
        }

        //Component access
        {
            constexpr Quat v(T(1), T(2), T(3), T(4));

            static_assert(v.x() == T(2));
            static_assert(v.y() == T(3));
            static_assert(v.z() == T(4));
            static_assert(v.w() == T(1));
            //Check that v.x() returns const T&
            static_assert(std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>);
        }

        //operator[]
        {
            constexpr Quat v(T(1), T(2), T(3), T(4));

            static_assert(v[0] == T(1));
            static_assert(v[1] == T(2));
            static_assert(v[2] == T(3));
            static_assert(v[3] == T(4));
            //Check that v[0] returns const T&
            static_assert(std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>);
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunQuatAccessorRunTimeTests()
    {
        using Quat = TRAP::Math::tQuat<T>;

        //Length
        {
            REQUIRE(Quat::Length() == 4);
        }

        //Component access
        {
            const Quat v(T(1), T(2), T(3), T(4));

            REQUIRE(v.x() == T(2));
            REQUIRE(v.y() == T(3));
            REQUIRE(v.z() == T(4));
            REQUIRE(v.w() == T(1));
            //Check that v.x() returns const T&
            static_assert(std::is_reference_v<decltype(v.x())> && std::is_const_v<std::remove_reference_t<decltype(v.x())>>);

            Quat v2(T(1), T(2), T(3), T(4));

            REQUIRE(v2.x() == T(2));
            REQUIRE(v2.y() == T(3));
            REQUIRE(v2.z() == T(4));
            REQUIRE(v2.w() == T(1));
            //Check that v2.x() returns T&
            static_assert(std::is_reference_v<decltype(v2.x())> && !std::is_const_v<std::remove_reference_t<decltype(v2.x())>>);
        }

        //operator[]
        {
            const Quat v(T(1), T(2), T(3), T(4));

            REQUIRE(v[0] == T(1));
            REQUIRE(v[1] == T(2));
            REQUIRE(v[2] == T(3));
            REQUIRE(v[3] == T(4));
            //Check that v[0] returns const T&
            static_assert(std::is_reference_v<decltype(v[0])> && std::is_const_v<std::remove_reference_t<decltype(v[0])>>);

            Quat v2(T(1), T(2), T(3), T(4));

            REQUIRE(v2[0] == T(1));
            REQUIRE(v2[1] == T(2));
            REQUIRE(v2[2] == T(3));
            REQUIRE(v2[3] == T(4));
            //Check that v2[0] returns T&
            static_assert(std::is_reference_v<decltype(v2[0])> && !std::is_const_v<std::remove_reference_t<decltype(v2[0])>>);
        }

        //at()
        {
            const Quat v(T(1), T(2), T(3), T(4));

            REQUIRE(v.at(0) == T(1));
            REQUIRE(v.at(1) == T(2));
            REQUIRE(v.at(2) == T(3));
            REQUIRE(v.at(3) == T(4));
            //Check that v.at(0) returns const T&
            static_assert(std::is_reference_v<decltype(v.at(0))> && std::is_const_v<std::remove_reference_t<decltype(v.at(0))>>);

            Quat v2(T(1), T(2), T(3), T(4));

            REQUIRE(v2.at(0) == T(1));
            REQUIRE(v2.at(1) == T(2));
            REQUIRE(v2.at(2) == T(3));
            REQUIRE(v2.at(3) == T(4));
            //Check that v2.at(0) returns T&
            static_assert(std::is_reference_v<decltype(v2.at(0))> && !std::is_const_v<std::remove_reference_t<decltype(v2.at(0))>>);
        }
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunQuatOperatorCompileTimeTests()
    {
        using Quat = TRAP::Math::tQuat<T>;

        //Unary operator+
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1 = +q;

            static_assert(q1.x() == T(2));
            static_assert(q1.y() == T(3));
            static_assert(q1.z() == T(4));
            static_assert(q1.w() == T(1));
        }

        //Unary operator-
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1 = -q;

            static_assert(q1.x() == T(-2));
            static_assert(q1.y() == T(-3));
            static_assert(q1.z() == T(-4));
            static_assert(q1.w() == T(-1));
        }

        //Binary operator+
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1 = q + q;

            static_assert(q1.x() == T(4));
            static_assert(q1.y() == T(6));
            static_assert(q1.z() == T(8));
            static_assert(q1.w() == T(2));
        }

        //Binary operator-
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1 = q - q;

            static_assert(q1.x() == T(0));
            static_assert(q1.y() == T(0));
            static_assert(q1.z() == T(0));
            static_assert(q1.w() == T(0));
        }

        //Binary operator*
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1 = q * q;

            static_assert(q1.x() == T(4));
            static_assert(q1.y() == T(6));
            static_assert(q1.z() == T(8));
            static_assert(q1.w() == T(-28));
        }

        //Binary operator*
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr TRAP::Math::tVec3<T> v = q * TRAP::Math::tVec3<T>(T(1), T(2), T(3));

            static_assert(v.x() == T(25));
            static_assert(v.y() == T(2));
            static_assert(v.z() == T(-9));
        }

        //Binary operator*
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr TRAP::Math::tVec3<T> v = TRAP::Math::tVec3<T>(T(1), T(2), T(3)) * q;

            static_assert(TRAP::Math::Equal(v.x(), T(1.022222), T(0.000001)));
            static_assert(TRAP::Math::Equal(v.y(), T(2.008889), T(0.000001)));
            static_assert(TRAP::Math::Equal(v.z(), T(2.982222), T(0.000001)));
        }

        //Binary operator*
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr TRAP::Math::tVec4<T> v = q * TRAP::Math::tVec4<T>(T(1), T(2), T(3), T(4));

            static_assert(v.x() == T(25));
            static_assert(v.y() == T(2));
            static_assert(v.z() == T(-9));
            static_assert(v.w() == T(4));
        }

        //Binary operator*
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr TRAP::Math::tVec4<T> v = TRAP::Math::tVec4<T>(T(1), T(2), T(3), T(4)) * q;

            static_assert(TRAP::Math::Equal(v.x(), T(1.022222), T(0.000001)));
            static_assert(TRAP::Math::Equal(v.y(), T(2.008889), T(0.000001)));
            static_assert(TRAP::Math::Equal(v.z(), T(2.982222), T(0.000001)));
            static_assert(TRAP::Math::Equal(v.w(), T(4.0), T(0.000001)));
        }

        //Binary operator*
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1 = q * T(2);

            static_assert(q1.x() == T(4));
            static_assert(q1.y() == T(6));
            static_assert(q1.z() == T(8));
            static_assert(q1.w() == T(2));
        }

        //Binary operator*
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1 = T(2) * q;

            static_assert(q1.x() == T(4));
            static_assert(q1.y() == T(6));
            static_assert(q1.z() == T(8));
            static_assert(q1.w() == T(2));
        }

        //Binary operator/
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1 = q / T(2);

            static_assert(q1.x() == T(1));
            static_assert(q1.y() == T(1.5));
            static_assert(q1.z() == T(2));
            static_assert(q1.w() == T(0.5));
        }

        //operator==
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1(T(4), T(3), T(2), T(1));

            static_assert(!(q == q1));
            static_assert(q == q);
        }

        //operator!=
        {
            constexpr Quat q(T(1), T(2), T(3), T(4));
            constexpr Quat q1(T(4), T(3), T(2), T(1));

            static_assert(q != q1);
            static_assert(!(q != q));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunQuatOperatorRunTimeTests()
    {
        using Quat = TRAP::Math::tQuat<T>;

        //operator +=
        {
            Quat q(T(1), T(2), T(3), T(4));
            q += Quat(T(4), T(3), T(2), T(1));

            REQUIRE(q.x() == T(5));
            REQUIRE(q.y() == T(5));
            REQUIRE(q.z() == T(5));
            REQUIRE(q.w() == T(5));
        }

        //operator -=
        {
            Quat q(T(1), T(2), T(3), T(4));
            q -= Quat(T(4), T(3), T(2), T(1));

            REQUIRE(q.x() == T(-1));
            REQUIRE(q.y() == T(1));
            REQUIRE(q.z() == T(3));
            REQUIRE(q.w() == T(-3));
        }

        //operator *=
        {
            Quat q(T(1), T(2), T(3), T(4));
            q *= Quat(T(4), T(3), T(2), T(1));

            REQUIRE(q.x() == T(6));
            REQUIRE(q.y() == T(24));
            REQUIRE(q.z() == T(12));
            REQUIRE(q.w() == T(-12));
        }

        //operator *=
        {
            Quat q(T(1), T(2), T(3), T(4));
            q *= 2.0;

            REQUIRE(q.x() == T(4));
            REQUIRE(q.y() == T(6));
            REQUIRE(q.z() == T(8));
            REQUIRE(q.w() == T(2));
        }

        //operator /=
        {
            Quat q(T(1), T(2), T(3), T(4));
            q /= 2.0;

            REQUIRE(q.x() == T(1));
            REQUIRE(q.y() == T(1.5));
            REQUIRE(q.z() == T(2));
            REQUIRE(q.w() == T(0.5));
        }

        //Unary operator+
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1 = +q;

            REQUIRE(q1.x() == T(2));
            REQUIRE(q1.y() == T(3));
            REQUIRE(q1.z() == T(4));
            REQUIRE(q1.w() == T(1));
        }

        //Unary operator-
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1 = -q;

            REQUIRE(q1.x() == T(-2));
            REQUIRE(q1.y() == T(-3));
            REQUIRE(q1.z() == T(-4));
            REQUIRE(q1.w() == T(-1));
        }

        //Binary operator+
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1 = q + q;

            REQUIRE(q1.x() == T(4));
            REQUIRE(q1.y() == T(6));
            REQUIRE(q1.z() == T(8));
            REQUIRE(q1.w() == T(2));
        }

        //Binary operator-
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1 = q - q;

            REQUIRE(q1.x() == T(0));
            REQUIRE(q1.y() == T(0));
            REQUIRE(q1.z() == T(0));
            REQUIRE(q1.w() == T(0));
        }

        //Binary operator*
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1 = q * q;

            REQUIRE(q1.x() == T(4));
            REQUIRE(q1.y() == T(6));
            REQUIRE(q1.z() == T(8));
            REQUIRE(q1.w() == T(-28));
        }

        //Binary operator*
        {
            Quat q(T(1), T(2), T(3), T(4));
            TRAP::Math::tVec3<T> v = q * TRAP::Math::tVec3<T>(T(1), T(2), T(3));

            REQUIRE(v.x() == T(25));
            REQUIRE(v.y() == T(2));
            REQUIRE(v.z() == T(-9));
        }

        //Binary operator*
        {
            Quat q(T(1), T(2), T(3), T(4));
            TRAP::Math::tVec3<T> v = TRAP::Math::tVec3<T>(T(1), T(2), T(3)) * q;

            REQUIRE(TRAP::Math::Equal(v.x(), T(1.022222), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(v.y(), T(2.008889), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(v.z(), T(2.982222), T(0.000001)));
        }

        //Binary operator*
        {
            Quat q(T(1), T(2), T(3), T(4));
            TRAP::Math::tVec4<T> v = q * TRAP::Math::tVec4<T>(T(1), T(2), T(3), T(4));

            REQUIRE(v.x() == T(25));
            REQUIRE(v.y() == T(2));
            REQUIRE(v.z() == T(-9));
            REQUIRE(v.w() == T(4));
        }

        //Binary operator*
        {
            Quat q(T(1), T(2), T(3), T(4));
            TRAP::Math::tVec4<T> v = TRAP::Math::tVec4<T>(T(1), T(2), T(3), T(4)) * q;

            REQUIRE(TRAP::Math::Equal(v.x(), T(1.022222), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(v.y(), T(2.008889), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(v.z(), T(2.982222), T(0.000001)));
            REQUIRE(TRAP::Math::Equal(v.w(), T(4.0), T(0.000001)));
        }

        //Binary operator*
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1 = q * T(2);

            REQUIRE(q1.x() == T(4));
            REQUIRE(q1.y() == T(6));
            REQUIRE(q1.z() == T(8));
            REQUIRE(q1.w() == T(2));
        }

        //Binary operator*
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1 = T(2) * q;

            REQUIRE(q1.x() == T(4));
            REQUIRE(q1.y() == T(6));
            REQUIRE(q1.z() == T(8));
            REQUIRE(q1.w() == T(2));
        }

        //Binary operator/
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1 = q / T(2);

            REQUIRE(q1.x() == T(1));
            REQUIRE(q1.y() == T(1.5));
            REQUIRE(q1.z() == T(2));
            REQUIRE(q1.w() == T(0.5));
        }

        //operator==
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1(T(4), T(3), T(2), T(1));

            REQUIRE(!(q == q1));
            REQUIRE(q == q);
        }

        //operator!=
        {
            Quat q(T(1), T(2), T(3), T(4));
            Quat q1(T(4), T(3), T(2), T(1));

            REQUIRE(q != q1);
            REQUIRE(!(q != q));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunQuatHashRunTimeTests()
    {
        using Quat = TRAP::Math::tQuat<T>;

        usize hash = std::hash<Quat>()(Quat(T(1), T(2), T(3), T(4)));
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunQuatFormatRunTimeTests()
    {
        using Quat = TRAP::Math::tQuat<T>;

        Quat q(T(1), T(2), T(3), T(4));
        std::string str = fmt::format("{}", q);

		if constexpr(std::same_as<T, f32>)
        {
            REQUIRE(str == fmt::format("Quatf({}, {{{}, {}, {}}})", q.w(), q.x(), q.y(), q.z()));
        }
		else if constexpr(std::same_as<T, f64>)
        {
            REQUIRE(str == fmt::format("Quatd({}, {{{}, {}, {}}})", q.w(), q.x(), q.y(), q.z()));
        }
    }
}

TEST_CASE("TRAP::Math::Quat", "[math][quat]")
{
    SECTION("Typedefs")
    {
        RunQuatTypedefsCompileTimeTests<f32>();
        RunQuatTypedefsCompileTimeTests<f64>();
    }

    SECTION("Constructors")
    {
        RunQuatConstructorCompileTimeTests<f32>();
        RunQuatConstructorCompileTimeTests<f64>();

        RunQuatConstructorRunTimeTests<f32>();
        RunQuatConstructorRunTimeTests<f64>();
    }

    SECTION("Assignments")
    {
        RunQuatAssignmentsRunTimeTests<f32>();
        RunQuatAssignmentsRunTimeTests<f64>();
    }

    SECTION("Accessors")
    {
        RunQuatAccessorCompileTimeTests<f32>();
        RunQuatAccessorCompileTimeTests<f64>();

        RunQuatAccessorRunTimeTests<f32>();
        RunQuatAccessorRunTimeTests<f64>();
    }

    SECTION("Operators")
    {
        RunQuatOperatorCompileTimeTests<f32>();
        RunQuatOperatorCompileTimeTests<f64>();

        RunQuatOperatorRunTimeTests<f32>();
        RunQuatOperatorRunTimeTests<f64>();
    }

    SECTION("std::hash")
    {
        RunQuatHashRunTimeTests<f32>();
        RunQuatHashRunTimeTests<f64>();
    }

    SECTION("fmt::format")
    {
        RunQuatFormatRunTimeTests<f32>();
        RunQuatFormatRunTimeTests<f64>();
    }
}

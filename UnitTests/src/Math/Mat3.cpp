#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Mat3", "[math][mat][mat3]", TRAP::Math::Mat3f, TRAP::Math::Mat3d)
{
    using Scalar = TestType::value_type;
    using Vec3Scalar = TRAP::Math::tVec3<Scalar>;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;
    using Col = TestType::col_type;

    SECTION("Typedefs")
    {
        STATIC_REQUIRE(std::same_as<typename TestType::col_type, Vec3Scalar>);
        STATIC_REQUIRE(std::same_as<typename TestType::row_type, Vec3Scalar>);
        STATIC_REQUIRE(std::same_as<typename TestType::value_type, Scalar>);
        STATIC_REQUIRE(std::same_as<typename TestType::size_type, u32>);
        STATIC_REQUIRE(std::same_as<typename TestType::difference_type, isize>);
        STATIC_REQUIRE(std::same_as<typename TestType::iterator, typename std::array<Vec3Scalar, 3u>::iterator>);
        STATIC_REQUIRE(std::same_as<typename TestType::const_iterator, typename std::array<Vec3Scalar, 3u>::const_iterator>);
        STATIC_REQUIRE(std::same_as<typename TestType::reverse_iterator, typename std::array<Vec3Scalar, 3u>::reverse_iterator>);
        STATIC_REQUIRE(std::same_as<typename TestType::const_reverse_iterator, typename std::array<Vec3Scalar, 3u>::const_reverse_iterator>);
    }

    SECTION("Constructors")
    {
        //Default constructor
        {
            static constexpr TestType m{};

            STATIC_REQUIRE(std::get<0u>(m) == Col{});
            STATIC_REQUIRE(std::get<1u>(m) == Col{});
            STATIC_REQUIRE(std::get<2u>(m) == Col{});
        }

        //Move constructor
        {
            Col c1{Scalar(1), Scalar(2), Scalar(3)};
            Col c2{Scalar(4), Scalar(5), Scalar(6)};
            Col c3{Scalar(7), Scalar(8), Scalar(9)};
            TestType m{c1, c2, c3};
            const TestType mMove(std::move(m));

            REQUIRE(std::get<0u>(mMove) == c1);
            REQUIRE(std::get<1u>(mMove) == c2);
            REQUIRE(std::get<2u>(mMove) == c3);
        }

        //Copy constructor
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3)};
            static constexpr Col c2{Scalar(4), Scalar(5), Scalar(6)};
            static constexpr Col c3{Scalar(7), Scalar(8), Scalar(9)};
            static constexpr TestType m{c1, c2, c3};
            static constexpr TestType mCopy(m);

            STATIC_REQUIRE(std::get<0u>(mCopy) == c1);
            STATIC_REQUIRE(std::get<1u>(mCopy) == c2);
            STATIC_REQUIRE(std::get<2u>(mCopy) == c3);
        }

        //Scalar constructor
        {
            static constexpr TestType m(Scalar(1));

            STATIC_REQUIRE(std::get<0u>(m) == Col{Scalar(1), Scalar(0), Scalar(0)});
            STATIC_REQUIRE(std::get<1u>(m) == Col{Scalar(0), Scalar(1), Scalar(0)});
            STATIC_REQUIRE(std::get<2u>(m) == Col{Scalar(0), Scalar(0), Scalar(1)});
        }

        //Value constructor
        {
            static constexpr TestType mCopy(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));

            STATIC_REQUIRE(std::get<0u>(mCopy) == Col{Scalar(1), Scalar(2), Scalar(3)});
            STATIC_REQUIRE(std::get<1u>(mCopy) == Col{Scalar(4), Scalar(5), Scalar(6)});
            STATIC_REQUIRE(std::get<2u>(mCopy) == Col{Scalar(7), Scalar(8), Scalar(9)});
        }

        //Column constructor
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3)};
            static constexpr Col c2{Scalar(4), Scalar(5), Scalar(6)};
            static constexpr Col c3{Scalar(7), Scalar(8), Scalar(9)};
            static constexpr TestType m{c1, c2, c3};

            STATIC_REQUIRE(std::get<0u>(m) == c1);
            STATIC_REQUIRE(std::get<1u>(m) == c2);
            STATIC_REQUIRE(std::get<2u>(m) == c3);
        }

        //Value conversion constructor
        {
            static constexpr TestType mCopy(f64(1), f64(2), f64(3), f64(4), f64(5), f64(6), f64(7), f64(8), f64(9));

            STATIC_REQUIRE(std::get<0u>(mCopy) == Col{Scalar(1), Scalar(2), Scalar(3)});
            STATIC_REQUIRE(std::get<1u>(mCopy) == Col{Scalar(4), Scalar(5), Scalar(6)});
            STATIC_REQUIRE(std::get<2u>(mCopy) == Col{Scalar(7), Scalar(8), Scalar(9)});
        }

        //Column conversion constructor
        {
            static constexpr Vec3Scalar v1{f64(1), f64(2), f64(3)};
            static constexpr Vec3Scalar v2{f64(4), f64(5), f64(6)};
            static constexpr Vec3Scalar v3{f64(7), f64(8), f64(9)};
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3)};
            static constexpr Col c2{Scalar(4), Scalar(5), Scalar(6)};
            static constexpr Col c3{Scalar(7), Scalar(8), Scalar(9)};
            static constexpr TestType m{v1, v2, v3};

            STATIC_REQUIRE(std::get<0u>(m) == c1);
            STATIC_REQUIRE(std::get<1u>(m) == c2);
            STATIC_REQUIRE(std::get<2u>(m) == c3);
        }

        //Copy conversion constructor
        {
            //Mat3
            {
                static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3)};
                static constexpr Col c2{Scalar(4), Scalar(5), Scalar(6)};
                static constexpr Col c3{Scalar(7), Scalar(8), Scalar(9)};
                static constexpr Vec3Scalar v1{f64(1), f64(2), f64(3)};
                static constexpr Vec3Scalar v2{f64(4), f64(5), f64(6)};
                static constexpr Vec3Scalar v3{f64(7), f64(8), f64(9)};
                static constexpr TRAP::Math::tMat3<f64> m{v1, v2, v3};
                static constexpr TestType mCopy(m);

                STATIC_REQUIRE(std::get<0u>(mCopy) == c1);
                STATIC_REQUIRE(std::get<1u>(mCopy) == c2);
                STATIC_REQUIRE(std::get<2u>(mCopy) == c3);
            }

            //Mat4
            {
                static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3)};
                static constexpr Col c2{Scalar(5), Scalar(6), Scalar(7)};
                static constexpr Col c3{Scalar(9), Scalar(10), Scalar(11)};
                static constexpr Vec4Scalar v1{f64(1), f64(2), f64(3), f64(4)};
                static constexpr Vec4Scalar v2{f64(5), f64(6), f64(7), f64(8)};
                static constexpr Vec4Scalar v3{f64(9), f64(10), f64(11), f64(12)};
                static constexpr Vec4Scalar v4{f64(13), f64(14), f64(15), f64(16)};
                static constexpr TRAP::Math::tMat4<f64> m{v1, v2, v3, v4};
                static constexpr TestType mCopy(m);

                STATIC_REQUIRE(std::get<0u>(mCopy) == c1);
                STATIC_REQUIRE(std::get<1u>(mCopy) == c2);
                STATIC_REQUIRE(std::get<2u>(mCopy) == c3);
            }
        }
    }

    SECTION("Assignments")
    {
        //Move assignment
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3)};
            static constexpr Col c2{Scalar(4), Scalar(5), Scalar(6)};
            static constexpr Col c3{Scalar(7), Scalar(8), Scalar(9)};
            TestType m{c1, c2, c3};
            TestType mMove{};
            mMove = std::move(m);

            REQUIRE(std::get<0u>(mMove) == c1);
            REQUIRE(std::get<1u>(mMove) == c2);
            REQUIRE(std::get<2u>(mMove) == c3);
        }

        //Copy assignment
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3)};
            static constexpr Col c2{Scalar(4), Scalar(5), Scalar(6)};
            static constexpr Col c3{Scalar(7), Scalar(8), Scalar(9)};
            static constexpr TestType m{c1, c2, c3};
            TestType mCopy{};
            mCopy = m;

            REQUIRE(std::get<0u>(mCopy) == c1);
            REQUIRE(std::get<1u>(mCopy) == c2);
            REQUIRE(std::get<2u>(mCopy) == c3);
        }

        //Copy conversion assignment
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3)};
            static constexpr Col c2{Scalar(4), Scalar(5), Scalar(6)};
            static constexpr Col c3{Scalar(7), Scalar(8), Scalar(9)};
            static constexpr Vec3Scalar v1{f64(1), f64(2), f64(3)};
            static constexpr Vec3Scalar v2{f64(4), f64(5), f64(6)};
            static constexpr Vec3Scalar v3{f64(7), f64(8), f64(9)};
            static constexpr TestType m{v1, v2, v3};
            TestType mCopy{};
            mCopy = m;

            REQUIRE(std::get<0u>(mCopy) == c1);
            REQUIRE(std::get<1u>(mCopy) == c2);
            REQUIRE(std::get<2u>(mCopy) == c3);
        }
    }

    SECTION("Accessors")
    {
        //Length
        {
            STATIC_REQUIRE(TestType::Length() == 3);
        }

        //operator[]
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));

            STATIC_REQUIRE(m[0] == Col(Scalar(1), Scalar(2), Scalar(3)));
            STATIC_REQUIRE(m[1] == Col(Scalar(4), Scalar(5), Scalar(6)));
            STATIC_REQUIRE(m[2] == Col(Scalar(7), Scalar(8), Scalar(9)));
            //Check that m[0] returns const Col&
            STATIC_REQUIRE(std::same_as<decltype(m[0]), const Col&>);

            TestType m1(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));

            REQUIRE(m1[0] == Col(Scalar(1), Scalar(2), Scalar(3)));
            REQUIRE(m1[1] == Col(Scalar(4), Scalar(5), Scalar(6)));
            REQUIRE(m1[2] == Col(Scalar(7), Scalar(8), Scalar(9)));
            //Check that m[0] returns Col&
            STATIC_REQUIRE(std::same_as<decltype(m1[0]), Col&>);
        }

        //at()
        {
            static constexpr TestType m1(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));

            STATIC_REQUIRE(m1.at(0) == Col(Scalar(1), Scalar(2), Scalar(3)));
            STATIC_REQUIRE(m1.at(1) == Col(Scalar(4), Scalar(5), Scalar(6)));
            STATIC_REQUIRE(m1.at(2) == Col(Scalar(7), Scalar(8), Scalar(9)));
            REQUIRE_THROWS_AS(m1.at(10), std::out_of_range);
            //Check that m1[0] returns const Col&
            STATIC_REQUIRE(std::same_as<decltype(m1[0]), const Col&>);

            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));

            REQUIRE(m.at(0) == Col(Scalar(1), Scalar(2), Scalar(3)));
            REQUIRE(m.at(1) == Col(Scalar(4), Scalar(5), Scalar(6)));
            REQUIRE(m.at(2) == Col(Scalar(7), Scalar(8), Scalar(9)));
            REQUIRE_THROWS_AS(m.at(10), std::out_of_range);
            //Check that m[0] returns Col&
            STATIC_REQUIRE(std::same_as<decltype(m[0]), Col&>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));

            STATIC_REQUIRE(m.begin() == &m[0]);
            STATIC_REQUIRE(m.cbegin() == &m[0]);
            STATIC_REQUIRE(m.rbegin() == std::reverse_iterator((&m[2]) + 1));
            STATIC_REQUIRE(m.crbegin() == std::reverse_iterator((&m[2]) + 1));
            STATIC_REQUIRE(m.end() == ((&m[2]) + 1));
            STATIC_REQUIRE(m.cend() == ((&m[2]) + 1));
            STATIC_REQUIRE(m.rend() == std::reverse_iterator(&m[0]));
            STATIC_REQUIRE(m.crend() == std::reverse_iterator(&m[0]));
        }
#endif
    }

    SECTION("Operators")
    {
        //Unary operator+
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = +m;

            STATIC_REQUIRE(m[0] == Col{1, 2, 3});
            STATIC_REQUIRE(m[1] == Col{4, 5, 6});
            STATIC_REQUIRE(m[2] == Col{7, 8, 9});
        }

        //Unary operator-
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = -m;

            STATIC_REQUIRE(m1[0] == Col{-1, -2, -3});
            STATIC_REQUIRE(m1[1] == Col{-4, -5, -6});
            STATIC_REQUIRE(m1[2] == Col{-7, -8, -9});
        }

        //Binary operator+
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = m + Scalar(1);

            STATIC_REQUIRE(m1[0] == Col{2, 3, 4});
            STATIC_REQUIRE(m1[1] == Col{5, 6, 7});
            STATIC_REQUIRE(m1[2] == Col{8, 9, 10});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = Scalar(1) + m;

            STATIC_REQUIRE(m1[0] == Col{2, 3, 4});
            STATIC_REQUIRE(m1[1] == Col{5, 6, 7});
            STATIC_REQUIRE(m1[2] == Col{8, 9, 10});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = m + m;

            STATIC_REQUIRE(m1[0] == Col{2, 4, 6});
            STATIC_REQUIRE(m1[1] == Col{8, 10, 12});
            STATIC_REQUIRE(m1[2] == Col{14, 16 ,18});
        }

        //Binary operator-
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = m - Scalar(1);

            STATIC_REQUIRE(m1[0] == Col{0, 1, 2});
            STATIC_REQUIRE(m1[1] == Col{3, 4, 5});
            STATIC_REQUIRE(m1[2] == Col{6, 7, 8});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = Scalar(1) - m;

            STATIC_REQUIRE(m1[0] == Col{0, -1, -2});
            STATIC_REQUIRE(m1[1] == Col{-3, -4, -5});
            STATIC_REQUIRE(m1[2] == Col{-6, -7, -8});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = m - m;

            STATIC_REQUIRE(m1[0] == Col{});
            STATIC_REQUIRE(m1[1] == Col{});
            STATIC_REQUIRE(m1[2] == Col{});
        }

        //Binary operator*
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = m * Scalar(2);

            STATIC_REQUIRE(m1[0] == Col{2, 4, 6});
            STATIC_REQUIRE(m1[1] == Col{8, 10, 12});
            STATIC_REQUIRE(m1[2] == Col{14, 16, 18});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = Scalar(2) * m;

            STATIC_REQUIRE(m1[0] == Col{2, 4, 6});
            STATIC_REQUIRE(m1[1] == Col{8, 10, 12});
            STATIC_REQUIRE(m1[2] == Col{14, 16, 18});
        }
        {
            static constexpr Col v{Scalar(1), Scalar(2), Scalar(3)};
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr Col c = m * v;

            STATIC_REQUIRE(c == Col{Scalar(30), Scalar(36), Scalar(42)});
        }
        {
            static constexpr Col v{Scalar(1), Scalar(2), Scalar(3)};
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr Col c = v * m;

            STATIC_REQUIRE(c == Col{Scalar(14), Scalar(32), Scalar(50)});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = m * m;

            STATIC_REQUIRE(m1[0] == Col{30, 36, 42});
            STATIC_REQUIRE(m1[1] == Col{66, 81, 96});
            STATIC_REQUIRE(m1[2] == Col{102, 126, 150});
        }

        //Binary operator/
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1 = m / Scalar(2);

            STATIC_REQUIRE(m1[0] == Col{0.5, 1, 1.5});
            STATIC_REQUIRE(m1[1] == Col{2, 2.5, 3});
            STATIC_REQUIRE(m1[2] == Col{3.5, 4, 4.5});
        }
        {
            static constexpr TestType m(Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2));
            static constexpr TestType m1 = Scalar(10) / m;

            STATIC_REQUIRE(m1[0] == Col{5, 5, 5});
            STATIC_REQUIRE(m1[1] == Col{5, 5, 5});
            STATIC_REQUIRE(m1[2] == Col{5, 5, 5});
        }
        {
            static constexpr TestType m(Scalar(1));
            static constexpr Col v(Scalar(2));
            static constexpr Col m1 = m / v;

            STATIC_REQUIRE(m1 == Col{2, 2, 2});
        }
        {
            static constexpr TestType m(Scalar(1));
            static constexpr Col v(Scalar(2));
            static constexpr Col m1 = v / m;

            STATIC_REQUIRE(m1 == Col{2, 2, 2});
        }
        {
            static constexpr TestType m(Scalar(1));
            static constexpr TestType m1(Scalar(2));
            static constexpr TestType m2 = m / m1;

            STATIC_REQUIRE(m2[0] == Col{0.5, 0, 0});
            STATIC_REQUIRE(m2[1] == Col{0, 0.5, 0});
            STATIC_REQUIRE(m2[2] == Col{0, 0, 0.5});
        }

        //Binary operator==
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1(Scalar(9), Scalar(8), Scalar(7), Scalar(6), Scalar(5), Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            STATIC_REQUIRE(m == m);
            STATIC_REQUIRE(!(m == m1));
        }

        //Binary operator!=
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            static constexpr TestType m1(Scalar(9), Scalar(8), Scalar(7), Scalar(6), Scalar(5), Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            STATIC_REQUIRE(m != m1);
            STATIC_REQUIRE(!(m != m));
        }

        //Scalar operator+=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            m += Scalar(10);

            REQUIRE(m[0] == Col{11, 12, 13});
            REQUIRE(m[1] == Col{14, 15, 16});
            REQUIRE(m[2] == Col{17, 18, 19});
        }

        //operator+=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            m += m;

            REQUIRE(m[0] == Col{2, 4, 6});
            REQUIRE(m[1] == Col{8, 10, 12});
            REQUIRE(m[2] == Col{14, 16, 18});
        }

        //Scalar opeartor-=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            m -= Scalar(10);

            REQUIRE(m[0] == Col{-9, -8, -7});
            REQUIRE(m[1] == Col{-6, -5, -4});
            REQUIRE(m[2] == Col{-3, -2, -1});
        }

        //operator-=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            m -= m;

            REQUIRE(m[0] == Col{});
            REQUIRE(m[1] == Col{});
            REQUIRE(m[2] == Col{});
        }

        //Scalar operator*=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            m *= Scalar(10);

            REQUIRE(m[0] == Col{10, 20, 30});
            REQUIRE(m[1] == Col{40, 50, 60});
            REQUIRE(m[2] == Col{70, 80, 90});
        }

        //operator*=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            m *= m;

            REQUIRE(m[0] == Col{1, 4, 9});
            REQUIRE(m[1] == Col{16, 25, 36});
            REQUIRE(m[2] == Col{49, 64, 81});
        }

        //Scalar operator/=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            m /= Scalar(0.5);

            REQUIRE(m[0] == Col{2, 4, 6});
            REQUIRE(m[1] == Col{8, 10, 12});
            REQUIRE(m[2] == Col{14, 16, 18});
        }

        //operator/=
        {
            TestType m(Scalar(1));
            static constexpr TestType m1(Scalar(2));
            m /= m1;

            REQUIRE(m[0] == Col{0.5, 0, 0});
            REQUIRE(m[1] == Col{0, 0.5, 0});
            REQUIRE(m[2] == Col{0, 0, 0.5});
        }

        //operator++
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            m++;

            REQUIRE(m[0] == Col{2, 3, 4});
            REQUIRE(m[1] == Col{5, 6, 7});
            REQUIRE(m[2] == Col{8, 9, 10});
        }
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            ++m;

            REQUIRE(m[0] == Col{2, 3, 4});
            REQUIRE(m[1] == Col{5, 6, 7});
            REQUIRE(m[2] == Col{8, 9, 10});
        }

        //operator--
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            m--;

            REQUIRE(m[0] == Col{0, 1, 2});
            REQUIRE(m[1] == Col{3, 4, 5});
            REQUIRE(m[2] == Col{6, 7, 8});
        }
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            --m;

            REQUIRE(m[0] == Col{0, 1, 2});
            REQUIRE(m[1] == Col{3, 4, 5});
            REQUIRE(m[2] == Col{6, 7, 8});
        }
    }

    SECTION("std::hash")
    {
        static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
        usize hash = std::hash<TestType>()(m);
    }

    SECTION("fmt::format")
    {
        static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));

        if constexpr(std::same_as<Scalar, f32>)
        {
            REQUIRE(fmt::format("{}", m) == fmt::format("Mat3f(({},{},{}), ({},{},{}), ({},{},{}))",
                                                        std::get<0u>(std::get<0u>(m)), std::get<1u>(std::get<0u>(m)),
							                            std::get<2u>(std::get<0u>(m)), std::get<0u>(std::get<1u>(m)),
						                                std::get<1u>(std::get<1u>(m)), std::get<2u>(std::get<1u>(m)),
							                            std::get<0u>(std::get<2u>(m)), std::get<1u>(std::get<2u>(m)),
							                            std::get<2u>(std::get<2u>(m))));
        }
        else if constexpr(std::same_as<Scalar, f64>)
        {
            REQUIRE(fmt::format("{}", m) == fmt::format("Mat3d(({},{},{}), ({},{},{}), ({},{},{}))",
                                                        std::get<0u>(std::get<0u>(m)), std::get<1u>(std::get<0u>(m)),
							                            std::get<2u>(std::get<0u>(m)), std::get<0u>(std::get<1u>(m)),
						                                std::get<1u>(std::get<1u>(m)), std::get<2u>(std::get<1u>(m)),
							                            std::get<0u>(std::get<2u>(m)), std::get<1u>(std::get<2u>(m)),
							                            std::get<2u>(std::get<2u>(m))));
        }
    }

    SECTION("std::get")
    {
        static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));

        STATIC_REQUIRE(std::get<0u>(m) == Col{1, 2, 3});
        STATIC_REQUIRE(std::get<1u>(m) == Col{4, 5, 6});
        STATIC_REQUIRE(std::get<2u>(m) == Col{7, 8, 9});
    }

    SECTION("std::swap")
    {
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            TestType m1(Scalar(9), Scalar(8), Scalar(7), Scalar(6), Scalar(5), Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            m.Swap(m1);

            REQUIRE(std::get<0u>(m) == Col{9, 8, 7});
            REQUIRE(std::get<1u>(m) == Col{6, 5, 4});
            REQUIRE(std::get<2u>(m) == Col{3, 2, 1});
            REQUIRE(std::get<0u>(m1) == Col{1, 2, 3});
            REQUIRE(std::get<1u>(m1) == Col{4, 5, 6});
            REQUIRE(std::get<2u>(m1) == Col{7, 8, 9});
        }
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9));
            TestType m1(Scalar(9), Scalar(8), Scalar(7), Scalar(6), Scalar(5), Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            std::swap(m, m1);

            REQUIRE(std::get<0u>(m) == Col{9, 8, 7});
            REQUIRE(std::get<1u>(m) == Col{6, 5, 4});
            REQUIRE(std::get<2u>(m) == Col{3, 2, 1});
            REQUIRE(std::get<0u>(m1) == Col{1, 2, 3});
            REQUIRE(std::get<1u>(m1) == Col{4, 5, 6});
            REQUIRE(std::get<2u>(m1) == Col{7, 8, 9});
        }
    }
}

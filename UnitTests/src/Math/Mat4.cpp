#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Mat4", "[math][mat][mat4]", TRAP::Math::Mat4f, TRAP::Math::Mat4d)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;
    using Col = TestType::col_type;
    using Row = TestType::row_type;

    SECTION("Class requirements")
    {
        STATIC_REQUIRE_FALSE(std::is_final_v<TestType>);
        STATIC_REQUIRE(std::copyable<TestType>);
        STATIC_REQUIRE(std::movable<TestType>);
    }

    SECTION("Typedefs")
    {
        STATIC_REQUIRE(std::same_as<typename TestType::col_type, Vec4Scalar>);
        STATIC_REQUIRE(std::same_as<typename TestType::row_type, Vec4Scalar>);
        STATIC_REQUIRE(std::same_as<typename TestType::value_type, Scalar>);
        STATIC_REQUIRE(std::same_as<typename TestType::size_type, u32>);
        STATIC_REQUIRE(std::same_as<typename TestType::difference_type, isize>);
        STATIC_REQUIRE(std::same_as<typename TestType::iterator, typename std::array<Vec4Scalar, 4u>::iterator>);
        STATIC_REQUIRE(std::same_as<typename TestType::const_iterator, typename std::array<Vec4Scalar, 4u>::const_iterator>);
        STATIC_REQUIRE(std::same_as<typename TestType::reverse_iterator, typename std::array<Vec4Scalar, 4u>::reverse_iterator>);
        STATIC_REQUIRE(std::same_as<typename TestType::const_reverse_iterator, typename std::array<Vec4Scalar, 4u>::const_reverse_iterator>);
    }

    SECTION("Constructors")
    {
        //Default constructor
        {
            static constexpr TestType m{};
            TestType m1{};

            STATIC_REQUIRE(std::get<0u>(m) == Col{});
            STATIC_REQUIRE(std::get<1u>(m) == Col{});
            STATIC_REQUIRE(std::get<2u>(m) == Col{});
            STATIC_REQUIRE(std::get<3u>(m) == Col{});
        }

        //Move constructor
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
            static constexpr Col c2{Scalar(5), Scalar(6), Scalar(7), Scalar(8)};
            static constexpr Col c3{Scalar(9), Scalar(10), Scalar(11), Scalar(12)};
            static constexpr Col c4{Scalar(13), Scalar(14), Scalar(15), Scalar(16)};
            TestType m{c1, c2, c3, c4};
            const TestType mMove(std::move(m));

            REQUIRE(std::get<0u>(mMove) == c1);
            REQUIRE(std::get<1u>(mMove) == c2);
            REQUIRE(std::get<2u>(mMove) == c3);
            REQUIRE(std::get<3u>(mMove) == c4);
        }

        //Copy constructor
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
            static constexpr Col c2{Scalar(5), Scalar(6), Scalar(7), Scalar(8)};
            static constexpr Col c3{Scalar(9), Scalar(10), Scalar(11), Scalar(12)};
            static constexpr Col c4{Scalar(13), Scalar(14), Scalar(15), Scalar(16)};
            static constexpr TestType m{c1, c2, c3, c4};
            static constexpr TestType mCopy(m);

            STATIC_REQUIRE(std::get<0u>(mCopy) == c1);
            STATIC_REQUIRE(std::get<1u>(mCopy) == c2);
            STATIC_REQUIRE(std::get<2u>(mCopy) == c3);
            STATIC_REQUIRE(std::get<3u>(mCopy) == c4);
        }

        //Scalar constructor
        {
            static constexpr TestType m(Scalar(1));

            STATIC_REQUIRE(std::get<0u>(m) == Col{Scalar(1), Scalar(0), Scalar(0), Scalar(0)});
            STATIC_REQUIRE(std::get<1u>(m) == Col{Scalar(0), Scalar(1), Scalar(0), Scalar(0)});
            STATIC_REQUIRE(std::get<2u>(m) == Col{Scalar(0), Scalar(0), Scalar(1), Scalar(0)});
            STATIC_REQUIRE(std::get<3u>(m) == Col{Scalar(0), Scalar(0), Scalar(0), Scalar(1)});
        }

        //Value constructor
        {
            static constexpr TestType mCopy(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));

            STATIC_REQUIRE(std::get<0u>(mCopy) == Col{Scalar(1), Scalar(2), Scalar(3), Scalar(4)});
            STATIC_REQUIRE(std::get<1u>(mCopy) == Col{Scalar(5), Scalar(6), Scalar(7), Scalar(8)});
            STATIC_REQUIRE(std::get<2u>(mCopy) == Col{Scalar(9), Scalar(10), Scalar(11), Scalar(12)});
            STATIC_REQUIRE(std::get<3u>(mCopy) == Col{Scalar(13), Scalar(14), Scalar(15), Scalar(16)});
        }

        //Column constructor
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
            static constexpr Col c2{Scalar(5), Scalar(6), Scalar(7), Scalar(8)};
            static constexpr Col c3{Scalar(9), Scalar(10), Scalar(11), Scalar(12)};
            static constexpr Col c4{Scalar(13), Scalar(14), Scalar(15), Scalar(16)};
            static constexpr TestType m{c1, c2, c3, c4};

            STATIC_REQUIRE(std::get<0u>(m) == c1);
            STATIC_REQUIRE(std::get<1u>(m) == c2);
            STATIC_REQUIRE(std::get<2u>(m) == c3);
            STATIC_REQUIRE(std::get<3u>(m) == c4);
        }

        //Value conversion constructor
        {
            static constexpr TestType mCopy(f64(1), f64(2), f64(3), f64(4), f64(5), f64(6), f64(7), f64(8), f64(9), f64(10), f64(11), f64(12), f64(13), f64(14), f64(15), f64(16));

            STATIC_REQUIRE(std::get<0u>(mCopy) == Col{Scalar(1), Scalar(2), Scalar(3), Scalar(4)});
            STATIC_REQUIRE(std::get<1u>(mCopy) == Col{Scalar(5), Scalar(6), Scalar(7), Scalar(8)});
            STATIC_REQUIRE(std::get<2u>(mCopy) == Col{Scalar(9), Scalar(10), Scalar(11), Scalar(12)});
            STATIC_REQUIRE(std::get<3u>(mCopy) == Col{Scalar(13), Scalar(14), Scalar(15), Scalar(16)});
        }

        //Column conversion constructor
        {
            static constexpr TRAP::Math::tVec4<f64> v1{f64(1), f64(2), f64(3), f64(4)};
            static constexpr TRAP::Math::tVec4<f64> v2{f64(5), f64(6), f64(7), f64(8)};
            static constexpr TRAP::Math::tVec4<f64> v3{f64(9), f64(10), f64(11), f64(12)};
            static constexpr TRAP::Math::tVec4<f64> v4{f64(13), f64(14), f64(15), f64(16)};
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
            static constexpr Col c2{Scalar(5), Scalar(6), Scalar(7), Scalar(8)};
            static constexpr Col c3{Scalar(9), Scalar(10), Scalar(11), Scalar(12)};
            static constexpr Col c4{Scalar(13), Scalar(14), Scalar(15), Scalar(16)};
            static constexpr TestType m{v1, v2, v3, v4};

            STATIC_REQUIRE(std::get<0u>(m) == c1);
            STATIC_REQUIRE(std::get<1u>(m) == c2);
            STATIC_REQUIRE(std::get<2u>(m) == c3);
            STATIC_REQUIRE(std::get<3u>(m) == c4);
        }

        //Copy conversion constructor
        {
            //Mat3
            {
                static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3), Scalar(0)};
                static constexpr Col c2{Scalar(4), Scalar(5), Scalar(6), Scalar(0)};
                static constexpr Col c3{Scalar(7), Scalar(8), Scalar(9), Scalar(0)};
                static constexpr Col c4{Scalar(0), Scalar(0), Scalar(0), Scalar(1)};
                static constexpr TRAP::Math::tVec3<f64> v1{f64(1), f64(2), f64(3)};
                static constexpr TRAP::Math::tVec3<f64> v2{f64(4), f64(5), f64(6)};
                static constexpr TRAP::Math::tVec3<f64> v3{f64(7), f64(8), f64(9)};
                static constexpr TRAP::Math::tMat3<f64> m{v1, v2, v3};
                static constexpr TestType mCopy(m);

                STATIC_REQUIRE(std::get<0u>(mCopy) == c1);
                STATIC_REQUIRE(std::get<1u>(mCopy) == c2);
                STATIC_REQUIRE(std::get<2u>(mCopy) == c3);
                STATIC_REQUIRE(std::get<3u>(mCopy) == c4);
            }

            //Mat4
            {
                static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
                static constexpr Col c2{Scalar(5), Scalar(6), Scalar(7), Scalar(8)};
                static constexpr Col c3{Scalar(9), Scalar(10), Scalar(11), Scalar(12)};
                static constexpr Col c4{Scalar(13), Scalar(14), Scalar(15), Scalar(16)};
                static constexpr TRAP::Math::tVec4<f64> v1{f64(1), f64(2), f64(3), f64(4)};
                static constexpr TRAP::Math::tVec4<f64> v2{f64(5), f64(6), f64(7), f64(8)};
                static constexpr TRAP::Math::tVec4<f64> v3{f64(9), f64(10), f64(11), f64(12)};
                static constexpr TRAP::Math::tVec4<f64> v4{f64(13), f64(14), f64(15), f64(16)};
                static constexpr TRAP::Math::tMat4<f64> m{v1, v2, v3, v4};
                static constexpr TestType mCopy(m);

                STATIC_REQUIRE(std::get<0u>(mCopy) == c1);
                STATIC_REQUIRE(std::get<1u>(mCopy) == c2);
                STATIC_REQUIRE(std::get<2u>(mCopy) == c3);
                STATIC_REQUIRE(std::get<3u>(mCopy) == c4);
            }
        }
    }

    SECTION("Assignments")
    {
        //Move assignment
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
            static constexpr Col c2{Scalar(5), Scalar(6), Scalar(7), Scalar(8)};
            static constexpr Col c3{Scalar(9), Scalar(10), Scalar(11), Scalar(12)};
            static constexpr Col c4{Scalar(13), Scalar(14), Scalar(15), Scalar(16)};
            TestType m{c1, c2, c3, c4};
            TestType mMove{};
            mMove = std::move(m);

            REQUIRE(std::get<0u>(mMove) == c1);
            REQUIRE(std::get<1u>(mMove) == c2);
            REQUIRE(std::get<2u>(mMove) == c3);
            REQUIRE(std::get<3u>(mMove) == c4);
        }

        //Copy assignment
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
            static constexpr Col c2{Scalar(5), Scalar(6), Scalar(7), Scalar(8)};
            static constexpr Col c3{Scalar(9), Scalar(10), Scalar(11), Scalar(12)};
            static constexpr Col c4{Scalar(13), Scalar(14), Scalar(15), Scalar(16)};
            static constexpr TestType m{c1, c2, c3, c4};
            TestType mCopy{};
            mCopy = m;

            REQUIRE(std::get<0u>(mCopy) == c1);
            REQUIRE(std::get<1u>(mCopy) == c2);
            REQUIRE(std::get<2u>(mCopy) == c3);
            REQUIRE(std::get<3u>(mCopy) == c4);
        }

        //Copy conversion assignment
        {
            static constexpr Col c1{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
            static constexpr Col c2{Scalar(5), Scalar(6), Scalar(7), Scalar(8)};
            static constexpr Col c3{Scalar(9), Scalar(10), Scalar(11), Scalar(12)};
            static constexpr Col c4{Scalar(13), Scalar(14), Scalar(15), Scalar(16)};
            static constexpr TRAP::Math::tVec4<f64> v1{f64(1), f64(2), f64(3), f64(4)};
            static constexpr TRAP::Math::tVec4<f64> v2{f64(5), f64(6), f64(7), f64(8)};
            static constexpr TRAP::Math::tVec4<f64> v3{f64(9), f64(10), f64(11), f64(12)};
            static constexpr TRAP::Math::tVec4<f64> v4{f64(13), f64(14), f64(15), f64(16)};
            static constexpr TRAP::Math::tMat4<f64> m{v1, v2, v3, v4};
            TestType mCopy{};
            mCopy = m;

            REQUIRE(std::get<0u>(mCopy) == c1);
            REQUIRE(std::get<1u>(mCopy) == c2);
            REQUIRE(std::get<2u>(mCopy) == c3);
            REQUIRE(std::get<3u>(mCopy) == c4);
        }
    }

    SECTION("Accessors")
    {
        //Length
        {
            STATIC_REQUIRE(TestType::Length() == 4u);
        }

        //operator[]
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));

            STATIC_REQUIRE(m[0] == Col(Scalar(1), Scalar(2), Scalar(3), Scalar(4)));
            STATIC_REQUIRE(m[1] == Col(Scalar(5), Scalar(6), Scalar(7), Scalar(8)));
            STATIC_REQUIRE(m[2] == Col(Scalar(9), Scalar(10), Scalar(11), Scalar(12)));
            STATIC_REQUIRE(m[3] == Col(Scalar(13), Scalar(14), Scalar(15), Scalar(16)));
            //Check that m[0] returns const Col&
            STATIC_REQUIRE(std::same_as<decltype(m[0]), const Col&>);

            TestType m1(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));

            REQUIRE(m1[0] == Col(Scalar(1), Scalar(2), Scalar(3), Scalar(4)));
            REQUIRE(m1[1] == Col(Scalar(5), Scalar(6), Scalar(7), Scalar(8)));
            REQUIRE(m1[2] == Col(Scalar(9), Scalar(10), Scalar(11), Scalar(12)));
            REQUIRE(m1[3] == Col(Scalar(13), Scalar(14), Scalar(15), Scalar(16)));
            //Check that m[0] returns Col&
            STATIC_REQUIRE(std::same_as<decltype(m1[0]), Col&>);
        }

        //at()
        {
            static constexpr TestType m1(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));

            STATIC_REQUIRE(m1.at(0) == Col(Scalar(1), Scalar(2), Scalar(3), Scalar(4)));
            STATIC_REQUIRE(m1.at(1) == Col(Scalar(5), Scalar(6), Scalar(7), Scalar(8)));
            STATIC_REQUIRE(m1.at(2) == Col(Scalar(9), Scalar(10), Scalar(11), Scalar(12)));
            STATIC_REQUIRE(m1.at(3) == Col(Scalar(13), Scalar(14), Scalar(15), Scalar(16)));
            REQUIRE_THROWS_AS(m1.at(10), std::out_of_range);
            //Check that m1[0] returns const Col&
            STATIC_REQUIRE(std::same_as<decltype(m1[0]), const Col&>);

            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));

            REQUIRE(m.at(0) == Col(Scalar(1), Scalar(2), Scalar(3), Scalar(4)));
            REQUIRE(m.at(1) == Col(Scalar(5), Scalar(6), Scalar(7), Scalar(8)));
            REQUIRE(m.at(2) == Col(Scalar(9), Scalar(10), Scalar(11), Scalar(12)));
            REQUIRE(m.at(3) == Col(Scalar(13), Scalar(14), Scalar(15), Scalar(16)));
            REQUIRE_THROWS_AS(m.at(10), std::out_of_range);
            //Check that m[0] returns Col&
            STATIC_REQUIRE(std::same_as<decltype(m[0]), Col&>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));

            STATIC_REQUIRE(m.begin() == &m[0]);
            STATIC_REQUIRE(m.cbegin() == &m[0]);
            STATIC_REQUIRE(m.rbegin() == std::reverse_iterator((&m[3]) + 1));
            STATIC_REQUIRE(m.crbegin() == std::reverse_iterator((&m[3]) + 1));
            STATIC_REQUIRE(m.end() == ((&m[3]) + 1));
            STATIC_REQUIRE(m.cend() == ((&m[3]) + 1));
            STATIC_REQUIRE(m.rend() == std::reverse_iterator(&m[0]));
            STATIC_REQUIRE(m.crend() == std::reverse_iterator(&m[0]));

            TestType m1(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));

            REQUIRE(m1.begin() == &m1[0]);
            REQUIRE(m1.cbegin() == &m1[0]);
            REQUIRE(m1.rbegin() == std::reverse_iterator((&m1[3]) + 1));
            REQUIRE(m1.crbegin() == std::reverse_iterator((&m1[3]) + 1));
            REQUIRE(m1.end() == ((&m1[3]) + 1));
            REQUIRE(m1.cend() == ((&m1[3]) + 1));
            REQUIRE(m1.rend() == std::reverse_iterator(&m1[0]));
            REQUIRE(m1.crend() == std::reverse_iterator(&m1[0]));
        }
#endif
    }

    SECTION("Operators")
    {
        //Unary operator+
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = +m;

            STATIC_REQUIRE(m[0] == Col{1, 2, 3, 4});
            STATIC_REQUIRE(m[1] == Col{5, 6, 7, 8});
            STATIC_REQUIRE(m[2] == Col{9, 10, 11, 12});
            STATIC_REQUIRE(m[3] == Col{13, 14, 15, 16});
        }

        //Unary operator-
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = -m;

            STATIC_REQUIRE(m1[0] == Col{-1, -2, -3, -4});
            STATIC_REQUIRE(m1[1] == Col{-5, -6, -7, -8});
            STATIC_REQUIRE(m1[2] == Col{-9, -10, -11, -12});
            STATIC_REQUIRE(m1[3] == Col{-13, -14, -15, -16});
        }

        //Binary operator+
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = m + Scalar(1);

            STATIC_REQUIRE(m1[0] == Col{2, 3, 4, 5});
            STATIC_REQUIRE(m1[1] == Col{6, 7, 8, 9});
            STATIC_REQUIRE(m1[2] == Col{10, 11, 12, 13});
            STATIC_REQUIRE(m1[3] == Col{14, 15, 16, 17});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = Scalar(1) + m;

            STATIC_REQUIRE(m1[0] == Col{2, 3, 4, 5});
            STATIC_REQUIRE(m1[1] == Col{6, 7, 8, 9});
            STATIC_REQUIRE(m1[2] == Col{10, 11, 12, 13});
            STATIC_REQUIRE(m1[3] == Col{14, 15, 16, 17});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = m + m;

            STATIC_REQUIRE(m1[0] == Col{2, 4, 6, 8});
            STATIC_REQUIRE(m1[1] == Col{10, 12, 14, 16});
            STATIC_REQUIRE(m1[2] == Col{18, 20, 22, 24});
            STATIC_REQUIRE(m1[3] == Col{26, 28, 30, 32});
        }

        //Binary operator-
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = m - Scalar(1);

            STATIC_REQUIRE(m1[0] == Col{0, 1, 2, 3});
            STATIC_REQUIRE(m1[1] == Col{4, 5, 6, 7});
            STATIC_REQUIRE(m1[2] == Col{8, 9, 10, 11});
            STATIC_REQUIRE(m1[3] == Col{12, 13, 14, 15});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = Scalar(1) - m;

            STATIC_REQUIRE(m1[0] == Col{0, -1, -2, -3});
            STATIC_REQUIRE(m1[1] == Col{-4, -5, -6, -7});
            STATIC_REQUIRE(m1[2] == Col{-8, -9, -10, -11});
            STATIC_REQUIRE(m1[3] == Col{-12, -13, -14, -15});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = m - m;

            STATIC_REQUIRE(m1[0] == Col{});
            STATIC_REQUIRE(m1[1] == Col{});
            STATIC_REQUIRE(m1[2] == Col{});
            STATIC_REQUIRE(m1[3] == Col{});
        }

        //Binary operator*
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = m * Scalar(2);

            STATIC_REQUIRE(m1[0] == Col{2, 4, 6, 8});
            STATIC_REQUIRE(m1[1] == Col{10, 12, 14, 16});
            STATIC_REQUIRE(m1[2] == Col{18, 20, 22, 24});
            STATIC_REQUIRE(m1[3] == Col{26, 28, 30, 32});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = Scalar(2) * m;

            STATIC_REQUIRE(m1[0] == Col{2, 4, 6, 8});
            STATIC_REQUIRE(m1[1] == Col{10, 12, 14, 16});
            STATIC_REQUIRE(m1[2] == Col{18, 20, 22, 24});
            STATIC_REQUIRE(m1[3] == Col{26, 28, 30, 32});
        }
        {
            static constexpr Col v{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr Col c = m * v;

            STATIC_REQUIRE(c == Col{Scalar(90), Scalar(100), Scalar(110), Scalar(120)});
        }
        {
            static constexpr Col v{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr Col c = v * m;

            STATIC_REQUIRE(c == Col{Scalar(30), Scalar(70), Scalar(110), Scalar(150)});
        }
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = m * m;

            STATIC_REQUIRE(m1[0] == Col{90, 100, 110, 120});
            STATIC_REQUIRE(m1[1] == Col{202, 228, 254, 280});
            STATIC_REQUIRE(m1[2] == Col{314, 356, 398, 440});
            STATIC_REQUIRE(m1[3] == Col{426, 484, 542, 600});
        }
        {
            static constexpr Row r{Scalar(1), Scalar(2), Scalar(3), Scalar(4)};
            TestType m(Scalar(1));
            Col c1 = m * r;
        }

        //Binary operator/
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1 = m / Scalar(2);

            STATIC_REQUIRE(m1[0] == Col{0.5, 1, 1.5, 2});
            STATIC_REQUIRE(m1[1] == Col{2.5, 3, 3.5, 4});
            STATIC_REQUIRE(m1[2] == Col{4.5, 5, 5.5, 6});
            STATIC_REQUIRE(m1[3] == Col{6.5, 7, 7.5, 8});
        }
        {
            static constexpr TestType m(Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2), Scalar(2));
            static constexpr TestType m1 = Scalar(10) / m;

            STATIC_REQUIRE(m1[0] == Col{5, 5, 5, 5});
            STATIC_REQUIRE(m1[1] == Col{5, 5, 5, 5});
            STATIC_REQUIRE(m1[2] == Col{5, 5, 5, 5});
            STATIC_REQUIRE(m1[3] == Col{5, 5, 5, 5});
        }
        {
            static constexpr TestType m(Scalar(1));
            static constexpr Col v(Scalar(2));
            static constexpr Col m1 = m / v;

            STATIC_REQUIRE(m1 == Col{2, 2, 2, 2});
        }
        {
            static constexpr TestType m(Scalar(1));
            static constexpr Col v(Scalar(2));
            static constexpr Col m1 = v / m;

            STATIC_REQUIRE(m1 == Col{2, 2, 2, 2});
        }
        {
            static constexpr TestType m(Scalar(1));
            static constexpr TestType m1(Scalar(2));
            static constexpr TestType m2 = m / m1;

            STATIC_REQUIRE(m2[0] == Col{0.5, 0, 0, 0});
            STATIC_REQUIRE(m2[1] == Col{0, 0.5, 0, 0});
            STATIC_REQUIRE(m2[2] == Col{0, 0, 0.5, 0});
            STATIC_REQUIRE(m2[3] == Col{0, 0, 0, 0.5});
        }

        //Binary operator==
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1(Scalar(16), Scalar(15), Scalar(14), Scalar(13), Scalar(12), Scalar(11), Scalar(10), Scalar(9), Scalar(8), Scalar(7), Scalar(6), Scalar(5), Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            STATIC_REQUIRE(m == m);
            STATIC_REQUIRE(!(m == m1));
        }

        //Binary operator!=
        {
            static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            static constexpr TestType m1(Scalar(16), Scalar(15), Scalar(14), Scalar(13), Scalar(12), Scalar(11), Scalar(10), Scalar(9), Scalar(8), Scalar(7), Scalar(6), Scalar(5), Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            STATIC_REQUIRE(m != m1);
            STATIC_REQUIRE(!(m != m));
        }

        //Scalar operator+=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            m += Scalar(10);

            REQUIRE(m[0] == Col{11, 12, 13, 14});
            REQUIRE(m[1] == Col{15, 16, 17, 18});
            REQUIRE(m[2] == Col{19, 20, 21, 22});
            REQUIRE(m[3] == Col{23, 24, 25, 26});
        }

        //operator+=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            m += m;

            REQUIRE(m[0] == Col{2, 4, 6, 8});
            REQUIRE(m[1] == Col{10, 12, 14, 16});
            REQUIRE(m[2] == Col{18, 20, 22, 24});
            REQUIRE(m[3] == Col{26, 28, 30, 32});
        }

        //Scalar opeartor-=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            m -= Scalar(10);

            REQUIRE(m[0] == Col{-9, -8, -7, -6});
            REQUIRE(m[1] == Col{-5, -4, -3, -2});
            REQUIRE(m[2] == Col{-1, 0, 1, 2});
            REQUIRE(m[3] == Col{3, 4, 5, 6});
        }

        //operator-=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            m -= m;

            REQUIRE(m[0] == Col{});
            REQUIRE(m[1] == Col{});
            REQUIRE(m[2] == Col{});
            REQUIRE(m[3] == Col{});
        }

        //Scalar operator*=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            m *= Scalar(10);

            REQUIRE(m[0] == Col{10, 20, 30, 40});
            REQUIRE(m[1] == Col{50, 60, 70, 80});
            REQUIRE(m[2] == Col{90, 100, 110, 120});
            REQUIRE(m[3] == Col{130, 140, 150, 160});
        }

        //operator*=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            m *= m;

            REQUIRE(m[0] == Col{1, 4, 9, 16});
            REQUIRE(m[1] == Col{25, 36, 49, 64});
            REQUIRE(m[2] == Col{81, 100, 121, 144});
            REQUIRE(m[3] == Col{169, 196, 225, 256});
        }

        //Scalar operator/=
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            m /= Scalar(0.5);

            REQUIRE(m[0] == Col{2, 4, 6, 8});
            REQUIRE(m[1] == Col{10, 12, 14, 16});
            REQUIRE(m[2] == Col{18, 20, 22, 24});
            REQUIRE(m[3] == Col{26, 28, 30, 32});
        }

        //operator/=
        {
            TestType m(Scalar(1));
            static constexpr TestType m1(Scalar(2));
            m /= m1;

            REQUIRE(m[0] == Col{0.5, 0, 0, 0});
            REQUIRE(m[1] == Col{0, 0.5, 0, 0});
            REQUIRE(m[2] == Col{0, 0, 0.5, 0});
            REQUIRE(m[3] == Col{0, 0, 0, 0.5});
        }

        //operator++
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            m++;

            REQUIRE(m[0] == Col{2, 3, 4, 5});
            REQUIRE(m[1] == Col{6, 7, 8, 9});
            REQUIRE(m[2] == Col{10, 11, 12, 13});
            REQUIRE(m[3] == Col{14, 15, 16, 17});
        }
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            ++m;

            REQUIRE(m[0] == Col{2, 3, 4, 5});
            REQUIRE(m[1] == Col{6, 7, 8, 9});
            REQUIRE(m[2] == Col{10, 11, 12, 13});
            REQUIRE(m[3] == Col{14, 15, 16, 17});
        }

        //operator--
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            m--;

            REQUIRE(m[0] == Col{0, 1, 2, 3});
            REQUIRE(m[1] == Col{4, 5, 6, 7});
            REQUIRE(m[2] == Col{8, 9, 10, 11});
            REQUIRE(m[3] == Col{12, 13, 14, 15});
        }
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            --m;

            REQUIRE(m[0] == Col{0, 1, 2, 3});
            REQUIRE(m[1] == Col{4, 5, 6, 7});
            REQUIRE(m[2] == Col{8, 9, 10, 11});
            REQUIRE(m[3] == Col{12, 13, 14, 15});
        }
    }

    SECTION("std::hash")
    {
        static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
        usize hash = std::hash<TestType>()(m);
    }

    SECTION("fmt::format")
    {
        static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));

        if constexpr(std::same_as<Scalar, f32>)
        {
            REQUIRE(fmt::format("{}", m) == fmt::format("Mat4f(({},{},{},{}), ({},{},{},{}), ({},{},{},{}), ({},{},{},{}))",
                                                        std::get<0>(std::get<0>(m)), std::get<1>(std::get<0>(m)),
						                                std::get<2>(std::get<0>(m)), std::get<3>(std::get<0>(m)),
						                                std::get<0>(std::get<1>(m)), std::get<1>(std::get<1>(m)),
						                                std::get<2>(std::get<1>(m)), std::get<3>(std::get<1>(m)),
						                                std::get<0>(std::get<2>(m)), std::get<1>(std::get<2>(m)),
						                                std::get<2>(std::get<2>(m)), std::get<3>(std::get<2>(m)),
						                                std::get<0>(std::get<3>(m)), std::get<1>(std::get<3>(m)),
						                                std::get<2>(std::get<3>(m)), std::get<3>(std::get<3>(m))));
        }
        else if constexpr(std::same_as<Scalar, f64>)
        {
            REQUIRE(fmt::format("{}", m) == fmt::format("Mat4d(({},{},{},{}), ({},{},{},{}), ({},{},{},{}), ({},{},{},{}))",
                                                        std::get<0>(std::get<0>(m)), std::get<1>(std::get<0>(m)),
						                                std::get<2>(std::get<0>(m)), std::get<3>(std::get<0>(m)),
						                                std::get<0>(std::get<1>(m)), std::get<1>(std::get<1>(m)),
						                                std::get<2>(std::get<1>(m)), std::get<3>(std::get<1>(m)),
						                                std::get<0>(std::get<2>(m)), std::get<1>(std::get<2>(m)),
						                                std::get<2>(std::get<2>(m)), std::get<3>(std::get<2>(m)),
						                                std::get<0>(std::get<3>(m)), std::get<1>(std::get<3>(m)),
						                                std::get<2>(std::get<3>(m)), std::get<3>(std::get<3>(m))));
        }
    }

    SECTION("std::get")
    {
        static constexpr TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));

        STATIC_REQUIRE(std::get<0u>(m) == Col{1, 2, 3, 4});
        STATIC_REQUIRE(std::get<1u>(m) == Col{5, 6, 7, 8});
        STATIC_REQUIRE(std::get<2u>(m) == Col{9, 10, 11, 12});
        STATIC_REQUIRE(std::get<3u>(m) == Col{13, 14, 15, 16});
    }

    SECTION("std::swap")
    {
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            TestType m1(Scalar(16), Scalar(15), Scalar(14), Scalar(13), Scalar(12), Scalar(11), Scalar(10), Scalar(9), Scalar(8), Scalar(7), Scalar(6), Scalar(5), Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            m.Swap(m1);

            REQUIRE(std::get<0u>(m) == Col{16, 15, 14, 13});
            REQUIRE(std::get<1u>(m) == Col{12, 11, 10, 9});
            REQUIRE(std::get<2u>(m) == Col{8, 7, 6, 5});
            REQUIRE(std::get<3u>(m) == Col{4, 3, 2, 1});
            REQUIRE(std::get<0u>(m1) == Col{1, 2, 3, 4});
            REQUIRE(std::get<1u>(m1) == Col{5, 6, 7, 8});
            REQUIRE(std::get<2u>(m1) == Col{9, 10, 11, 12});
            REQUIRE(std::get<3u>(m1) == Col{13, 14, 15, 16});
        }
        {
            TestType m(Scalar(1), Scalar(2), Scalar(3), Scalar(4), Scalar(5), Scalar(6), Scalar(7), Scalar(8), Scalar(9), Scalar(10), Scalar(11), Scalar(12), Scalar(13), Scalar(14), Scalar(15), Scalar(16));
            TestType m1(Scalar(16), Scalar(15), Scalar(14), Scalar(13), Scalar(12), Scalar(11), Scalar(10), Scalar(9), Scalar(8), Scalar(7), Scalar(6), Scalar(5), Scalar(4), Scalar(3), Scalar(2), Scalar(1));

            std::swap(m, m1);

            REQUIRE(std::get<0u>(m) == Col{16, 15, 14, 13});
            REQUIRE(std::get<1u>(m) == Col{12, 11, 10, 9});
            REQUIRE(std::get<2u>(m) == Col{8, 7, 6, 5});
            REQUIRE(std::get<3u>(m) == Col{4, 3, 2, 1});
            REQUIRE(std::get<0u>(m1) == Col{1, 2, 3, 4});
            REQUIRE(std::get<1u>(m1) == Col{5, 6, 7, 8});
            REQUIRE(std::get<2u>(m1) == Col{9, 10, 11, 12});
            REQUIRE(std::get<3u>(m1) == Col{13, 14, 15, 16});
        }
    }
}

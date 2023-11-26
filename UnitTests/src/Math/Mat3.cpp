#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunMat3TypedefsCompileTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;

        static_assert(std::same_as<typename Mat::col_type, TRAP::Math::Vec<3, T>>);
        static_assert(std::same_as<typename Mat::row_type, TRAP::Math::Vec<3, T>>);
        static_assert(std::same_as<typename Mat::value_type, T>);
        static_assert(std::same_as<typename Mat::size_type, u32>);
        static_assert(std::same_as<typename Mat::difference_type, isize>);
        static_assert(std::same_as<typename Mat::iterator, typename std::array<TRAP::Math::Vec<3, T>, 3>::iterator>);
        static_assert(std::same_as<typename Mat::const_iterator, typename std::array<TRAP::Math::Vec<3, T>, 3>::const_iterator>);
        static_assert(std::same_as<typename Mat::reverse_iterator, typename std::array<TRAP::Math::Vec<3, T>, 3>::reverse_iterator>);
        static_assert(std::same_as<typename Mat::const_reverse_iterator, typename std::array<TRAP::Math::Vec<3, T>, 3>::const_reverse_iterator>);
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunMat3ConstructorCompileTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;
        using Col = Mat::col_type;

        //Default constructor
        {
            constexpr Mat m{};

            static_assert(std::get<0>(m) == Col{});
            static_assert(std::get<1>(m) == Col{});
            static_assert(std::get<2>(m) == Col{});
        }

        //Move constructor
        {
        }

        //Copy constructor
        {
            constexpr Col c1{T(1), T(2), T(3)};
            constexpr Col c2{T(4), T(5), T(6)};
            constexpr Col c3{T(7), T(8), T(9)};
            constexpr Mat m{c1, c2, c3};
            constexpr Mat mCopy(m);

            static_assert(std::get<0>(mCopy) == c1);
            static_assert(std::get<1>(mCopy) == c2);
            static_assert(std::get<2>(mCopy) == c3);
        }

        //Scalar constructor
        {
            constexpr Mat m(T(1));

            static_assert(std::get<0>(m) == Col{T(1), T(0), T(0)});
            static_assert(std::get<1>(m) == Col{T(0), T(1), T(0)});
            static_assert(std::get<2>(m) == Col{T(0), T(0), T(1)});
        }

        //Value constructor
        {
            constexpr Mat mCopy(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

            static_assert(std::get<0>(mCopy) == Col{T(1), T(2), T(3)});
            static_assert(std::get<1>(mCopy) == Col{T(4), T(5), T(6)});
            static_assert(std::get<2>(mCopy) == Col{T(7), T(8), T(9)});
        }

        //Column constructor
        {
            constexpr Col c1{T(1), T(2), T(3)};
            constexpr Col c2{T(4), T(5), T(6)};
            constexpr Col c3{T(7), T(8), T(9)};
            constexpr Mat m{c1, c2, c3};

            static_assert(std::get<0>(m) == c1);
            static_assert(std::get<1>(m) == c2);
            static_assert(std::get<2>(m) == c3);
        }

        //Value conversion constructor
        {
            constexpr Mat mCopy(f64(1), f64(2), f64(3), f64(4), f64(5), f64(6), f64(7), f64(8), f64(9));

            static_assert(std::get<0>(mCopy) == Col{T(1), T(2), T(3)});
            static_assert(std::get<1>(mCopy) == Col{T(4), T(5), T(6)});
            static_assert(std::get<2>(mCopy) == Col{T(7), T(8), T(9)});
        }

        //Column conversion constructor
        {
            constexpr TRAP::Math::tVec3<f64> v1{f64(1), f64(2), f64(3)};
            constexpr TRAP::Math::tVec3<f64> v2{f64(4), f64(5), f64(6)};
            constexpr TRAP::Math::tVec3<f64> v3{f64(7), f64(8), f64(9)};
            constexpr Col c1{T(1), T(2), T(3)};
            constexpr Col c2{T(4), T(5), T(6)};
            constexpr Col c3{T(7), T(8), T(9)};
            constexpr Mat m{v1, v2, v3};

            static_assert(std::get<0>(m) == c1);
            static_assert(std::get<1>(m) == c2);
            static_assert(std::get<2>(m) == c3);
        }

        //Copy conversion constructor
        {
            //Mat3
            {
                constexpr Col c1{T(1), T(2), T(3)};
                constexpr Col c2{T(4), T(5), T(6)};
                constexpr Col c3{T(7), T(8), T(9)};
                constexpr TRAP::Math::tVec3<f64> v1{f64(1), f64(2), f64(3)};
                constexpr TRAP::Math::tVec3<f64> v2{f64(4), f64(5), f64(6)};
                constexpr TRAP::Math::tVec3<f64> v3{f64(7), f64(8), f64(9)};
                constexpr TRAP::Math::tMat3<f64> m{v1, v2, v3};
                constexpr Mat mCopy(m);

                static_assert(std::get<0>(mCopy) == c1);
                static_assert(std::get<1>(mCopy) == c2);
                static_assert(std::get<2>(mCopy) == c3);
            }

            //Mat4
            {
                constexpr Col c1{T(1), T(2), T(3)};
                constexpr Col c2{T(5), T(6), T(7)};
                constexpr Col c3{T(9), T(10), T(11)};
                constexpr TRAP::Math::tVec4<f64> v1{f64(1), f64(2), f64(3), f64(4)};
                constexpr TRAP::Math::tVec4<f64> v2{f64(5), f64(6), f64(7), f64(8)};
                constexpr TRAP::Math::tVec4<f64> v3{f64(9), f64(10), f64(11), f64(12)};
                constexpr TRAP::Math::tVec4<f64> v4{f64(13), f64(14), f64(15), f64(16)};
                constexpr TRAP::Math::tMat4<f64> m{v1, v2, v3, v4};
                constexpr Mat mCopy(m);

                static_assert(std::get<0>(mCopy) == c1);
                static_assert(std::get<1>(mCopy) == c2);
                static_assert(std::get<2>(mCopy) == c3);
            }
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat3ConstructorRunTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;
        using Col = Mat::col_type;

        //Default constructor
        {
            Mat m{};

            REQUIRE(std::get<0>(m) == Col{});
            REQUIRE(std::get<1>(m) == Col{});
            REQUIRE(std::get<2>(m) == Col{});
        }

        //Move constructor
        {
            Col c1{T(1), T(2), T(3)};
            Col c2{T(4), T(5), T(6)};
            Col c3{T(7), T(8), T(9)};
            Mat m{c1, c2, c3};
            const Mat mMove(std::move(m));

            REQUIRE(std::get<0>(mMove) == c1);
            REQUIRE(std::get<1>(mMove) == c2);
            REQUIRE(std::get<2>(mMove) == c3);
        }

        //Copy constructor
        {
            Col c1{T(1), T(2), T(3)};
            Col c2{T(4), T(5), T(6)};
            Col c3{T(7), T(8), T(9)};
            const Mat m{c1, c2, c3};
            Mat mCopy(m);

            REQUIRE(std::get<0>(mCopy) == c1);
            REQUIRE(std::get<1>(mCopy) == c2);
            REQUIRE(std::get<2>(mCopy) == c3);
        }

        //Scalar constructor
        {
            T scalar(1);
            Mat m(scalar);

            REQUIRE(std::get<0>(m) == Col{T(1), T(0), T(0)});
            REQUIRE(std::get<1>(m) == Col{T(0), T(1), T(0)});
            REQUIRE(std::get<2>(m) == Col{T(0), T(0), T(1)});
        }

        //Value constructor
        {
            Mat mCopy(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

            REQUIRE(std::get<0>(mCopy) == Col{T(1), T(2), T(3)});
            REQUIRE(std::get<1>(mCopy) == Col{T(4), T(5), T(6)});
            REQUIRE(std::get<2>(mCopy) == Col{T(7), T(8), T(9)});
        }

        //Column constructor
        {
            Col c1{T(1), T(2), T(3)};
            Col c2{T(4), T(5), T(6)};
            Col c3{T(7), T(8), T(9)};
            Mat m{c1, c2, c3};

            REQUIRE(std::get<0>(m) == c1);
            REQUIRE(std::get<1>(m) == c2);
            REQUIRE(std::get<2>(m) == c3);
        }

        //Value conversion constructor
        {
            f64 x = 1.0f;
            f64 y = 2.0f;
            f64 z = 3.0f;
            Mat m(x, y, z, x, y, z, x, y, z);

            REQUIRE(std::get<0>(m) == Col{T(1), T(2), T(3)});
            REQUIRE(std::get<1>(m) == Col{T(1), T(2), T(3)});
            REQUIRE(std::get<2>(m) == Col{T(1), T(2), T(3)});
        }

        //Column conversion constructor
        {
            TRAP::Math::tVec3<f64> v1{f64(1), f64(2), f64(3)};
            TRAP::Math::tVec3<f64> v2{f64(4), f64(5), f64(6)};
            TRAP::Math::tVec3<f64> v3{f64(7), f64(8), f64(9)};
            Col c1{T(1), T(2), T(3)};
            Col c2{T(4), T(5), T(6)};
            Col c3{T(7), T(8), T(9)};
            Mat m{v1, v2, v3};

            REQUIRE(std::get<0>(m) == c1);
            REQUIRE(std::get<1>(m) == c2);
            REQUIRE(std::get<2>(m) == c3);
        }

        //Copy conversion constructor
        {
            //Mat3
            {
                Col c1{T(1), T(2), T(3)};
                Col c2{T(4), T(5), T(6)};
                Col c3{T(7), T(8), T(9)};
                TRAP::Math::tVec3<f64> v1{f64(1), f64(2), f64(3)};
                TRAP::Math::tVec3<f64> v2{f64(4), f64(5), f64(6)};
                TRAP::Math::tVec3<f64> v3{f64(7), f64(8), f64(9)};
                const TRAP::Math::tMat3<f64> m{v1, v2, v3};
                Mat mCopy(m);

                REQUIRE(std::get<0>(mCopy) == c1);
                REQUIRE(std::get<1>(mCopy) == c2);
                REQUIRE(std::get<2>(mCopy) == c3);
            }

            //Mat4
            {
                Col c1{T(1), T(2), T(3)};
                Col c2{T(5), T(6), T(7)};
                Col c3{T(9), T(10), T(11)};
                TRAP::Math::tVec4<f64> v1{f64(1), f64(2), f64(3), f64(4)};
                TRAP::Math::tVec4<f64> v2{f64(5), f64(6), f64(7), f64(8)};
                TRAP::Math::tVec4<f64> v3{f64(9), f64(10), f64(11), f64(12)};
                TRAP::Math::tVec4<f64> v4{f64(13), f64(14), f64(15), f64(16)};
                const TRAP::Math::tMat4<f64> m{v1, v2, v3, v4};
                Mat mCopy(m);

                REQUIRE(std::get<0>(mCopy) == c1);
                REQUIRE(std::get<1>(mCopy) == c2);
                REQUIRE(std::get<2>(mCopy) == c3);
            }
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat3AssignmentRunTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;
        using Col = Mat::col_type;

        //Move assignment
        {
            Col c1{T(1), T(2), T(3)};
            Col c2{T(4), T(5), T(6)};
            Col c3{T(7), T(8), T(9)};
            Mat m{c1, c2, c3};
            Mat mMove{};
            mMove = std::move(m);

            REQUIRE(std::get<0>(mMove) == c1);
            REQUIRE(std::get<1>(mMove) == c2);
            REQUIRE(std::get<2>(mMove) == c3);
        }

        //Copy assignment
        {
            Col c1{T(1), T(2), T(3)};
            Col c2{T(4), T(5), T(6)};
            Col c3{T(7), T(8), T(9)};
            const Mat m{c1, c2, c3};
            Mat mCopy{};
            mCopy = m;

            REQUIRE(std::get<0>(mCopy) == c1);
            REQUIRE(std::get<1>(mCopy) == c2);
            REQUIRE(std::get<2>(mCopy) == c3);
        }

        //Copy conversion assignment
        {
            Col c1{T(1), T(2), T(3)};
            Col c2{T(4), T(5), T(6)};
            Col c3{T(7), T(8), T(9)};
            TRAP::Math::tVec3<f64> v1{f64(1), f64(2), f64(3)};
            TRAP::Math::tVec3<f64> v2{f64(4), f64(5), f64(6)};
            TRAP::Math::tVec3<f64> v3{f64(7), f64(8), f64(9)};
            TRAP::Math::tMat3<f64> m{v1, v2, v3};
            Mat mCopy{};
            mCopy = m;

            REQUIRE(std::get<0>(mCopy) == c1);
            REQUIRE(std::get<1>(mCopy) == c2);
            REQUIRE(std::get<2>(mCopy) == c3);
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunMat3AccessorCompileTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;
        using Col = Mat::col_type;

        //Length
        {
            static_assert(Mat::Length() == 3);
        }

        //operator[]
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

            static_assert(m[0] == Col(T(1), T(2), T(3)));
            static_assert(m[1] == Col(T(4), T(5), T(6)));
            static_assert(m[2] == Col(T(7), T(8), T(9)));
            //Check that m[0] returns const Col&
            static_assert(std::same_as<decltype(m[0]), const Col&>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

            static_assert(m.begin() == &m[0]);
            static_assert(m.cbegin() == &m[0]);
            static_assert(m.rbegin() == std::reverse_iterator((&m[2]) + 1));
            static_assert(m.crbegin() == std::reverse_iterator((&m[2]) + 1));
            static_assert(m.end() == ((&m[2]) + 1));
            static_assert(m.cend() == ((&m[2]) + 1));
            static_assert(m.rend() == std::reverse_iterator(&m[0]));
            static_assert(m.crend() == std::reverse_iterator(&m[0]));
        }
#endif
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat3AccessorRunTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;
        using Col = Mat::col_type;

        //Length
        {
            REQUIRE(Mat::Length() == 3);
        }

        //operator[]
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

            REQUIRE(m[0] == Col(T(1), T(2), T(3)));
            REQUIRE(m[1] == Col(T(4), T(5), T(6)));
            REQUIRE(m[2] == Col(T(7), T(8), T(9)));
            //Check that m[0] returns Col&
            REQUIRE(std::same_as<decltype(m[0]), Col&>);

            const Mat m1(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

            REQUIRE(m1[0] == Col(T(1), T(2), T(3)));
            REQUIRE(m1[1] == Col(T(4), T(5), T(6)));
            REQUIRE(m1[2] == Col(T(7), T(8), T(9)));
            //Check that m1[0] returns const Col&
            REQUIRE(std::same_as<decltype(m1[0]), const Col&>);
        }

        //at()
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

            REQUIRE(m.at(0) == Col(T(1), T(2), T(3)));
            REQUIRE(m.at(1) == Col(T(4), T(5), T(6)));
            REQUIRE(m.at(2) == Col(T(7), T(8), T(9)));
            //Check that m[0] returns Col&
            REQUIRE(std::same_as<decltype(m[0]), Col&>);

            const Mat m1(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

            REQUIRE(m1.at(0) == Col(T(1), T(2), T(3)));
            REQUIRE(m1.at(1) == Col(T(4), T(5), T(6)));
            REQUIRE(m1.at(2) == Col(T(7), T(8), T(9)));
            //Check that m1[0] returns const Col&
            REQUIRE(std::same_as<decltype(m1[0]), const Col&>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

            REQUIRE(m.begin() == &m[0]);
            REQUIRE(m.cbegin() == &m[0]);
            REQUIRE(m.rbegin() == std::reverse_iterator((&m[2]) + 1));
            REQUIRE(m.crbegin() == std::reverse_iterator((&m[2]) + 1));
            REQUIRE(m.end() == ((&m[2]) + 1));
            REQUIRE(m.cend() == ((&m[2]) + 1));
            REQUIRE(m.rend() == std::reverse_iterator(&m[0]));
            REQUIRE(m.crend() == std::reverse_iterator(&m[0]));

            constexpr Mat m2(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

            REQUIRE(m2.begin() == &m2[0]);
            REQUIRE(m2.cbegin() == &m2[0]);
            REQUIRE(m2.rbegin() == std::reverse_iterator((&m2[2]) + 1));
            REQUIRE(m2.crbegin() == std::reverse_iterator((&m2[2]) + 1));
            REQUIRE(m2.end() == ((&m2[2]) + 1));
            REQUIRE(m2.cend() == ((&m2[2]) + 1));
            REQUIRE(m2.rend() == std::reverse_iterator(&m2[0]));
            REQUIRE(m2.crend() == std::reverse_iterator(&m2[0]));
        }
#endif
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunMat3OperatorCompileTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;
        using Col = Mat::col_type;

        //Unary operator+
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = +m;

            static_assert(m[0] == Col{1, 2, 3});
            static_assert(m[1] == Col{4, 5, 6});
            static_assert(m[2] == Col{7, 8, 9});
        }

        //Unary operator-
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = -m;

            static_assert(m1[0] == Col{-1, -2, -3});
            static_assert(m1[1] == Col{-4, -5, -6});
            static_assert(m1[2] == Col{-7, -8, -9});
        }

        //Binary operator+
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = m + T(1);

            static_assert(m1[0] == Col{2, 3, 4});
            static_assert(m1[1] == Col{5, 6, 7});
            static_assert(m1[2] == Col{8, 9, 10});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = T(1) + m;

            static_assert(m1[0] == Col{2, 3, 4});
            static_assert(m1[1] == Col{5, 6, 7});
            static_assert(m1[2] == Col{8, 9, 10});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = m + m;

            static_assert(m1[0] == Col{2, 4, 6});
            static_assert(m1[1] == Col{8, 10, 12});
            static_assert(m1[2] == Col{14, 16 ,18});
        }

        //Binary operator-
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = m - T(1);

            static_assert(m1[0] == Col{0, 1, 2});
            static_assert(m1[1] == Col{3, 4, 5});
            static_assert(m1[2] == Col{6, 7, 8});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = T(1) - m;

            static_assert(m1[0] == Col{0, -1, -2});
            static_assert(m1[1] == Col{-3, -4, -5});
            static_assert(m1[2] == Col{-6, -7, -8});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = m - m;

            static_assert(m1[0] == Col{});
            static_assert(m1[1] == Col{});
            static_assert(m1[2] == Col{});
        }

        //Binary operator*
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = m * T(2);

            static_assert(m1[0] == Col{2, 4, 6});
            static_assert(m1[1] == Col{8, 10, 12});
            static_assert(m1[2] == Col{14, 16, 18});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = T(2) * m;

            static_assert(m1[0] == Col{2, 4, 6});
            static_assert(m1[1] == Col{8, 10, 12});
            static_assert(m1[2] == Col{14, 16, 18});
        }
        {
            constexpr Col v{T(1), T(2), T(3)};
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Col c = m * v;

            static_assert(c == Col{T(30), T(36), T(42)});
        }
        {
            constexpr Col v{T(1), T(2), T(3)};
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Col c = v * m;

            static_assert(c == Col{T(14), T(32), T(50)});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = m * m;

            static_assert(m1[0] == Col{30, 36, 42});
            static_assert(m1[1] == Col{66, 81, 96});
            static_assert(m1[2] == Col{102, 126, 150});
        }

        //Binary operator/
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1 = m / T(2);

            static_assert(m1[0] == Col{0.5, 1, 1.5});
            static_assert(m1[1] == Col{2, 2.5, 3});
            static_assert(m1[2] == Col{3.5, 4, 4.5});
        }
        {
            constexpr Mat m(T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2));
            constexpr Mat m1 = T(10) / m;

            static_assert(m1[0] == Col{5, 5, 5});
            static_assert(m1[1] == Col{5, 5, 5});
            static_assert(m1[2] == Col{5, 5, 5});
        }
        {
            constexpr Mat m(T(1));
            constexpr Col v(T(2));
            constexpr Col m1 = m / v;

            static_assert(m1 == Col{2, 2, 2});
        }
        {
            constexpr Mat m(T(1));
            constexpr Col v(T(2));
            constexpr Col m1 = v / m;

            static_assert(m1 == Col{2, 2, 2});
        }
        {
            constexpr Mat m(T(1));
            constexpr Mat m1(T(2));
            constexpr Mat m2 = m / m1;

            static_assert(m2[0] == Col{0.5, 0, 0});
            static_assert(m2[1] == Col{0, 0.5, 0});
            static_assert(m2[2] == Col{0, 0, 0.5});
        }

        //Binary operator==
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1(T(9), T(8), T(7), T(6), T(5), T(4), T(3), T(2), T(1));

            static_assert(m == m);
            static_assert(!(m == m1));
        }

        //Binary operator!=
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            constexpr Mat m1(T(9), T(8), T(7), T(6), T(5), T(4), T(3), T(2), T(1));

            static_assert(m != m1);
            static_assert(!(m != m));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat3OperatorRunTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;
        using Col = Mat::col_type;

        //Scalar operator+=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            m += T(10);

            REQUIRE(m[0] == Col{11, 12, 13});
            REQUIRE(m[1] == Col{14, 15, 16});
            REQUIRE(m[2] == Col{17, 18, 19});
        }

        //operator+=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            m += m;

            REQUIRE(m[0] == Col{2, 4, 6});
            REQUIRE(m[1] == Col{8, 10, 12});
            REQUIRE(m[2] == Col{14, 16, 18});
        }

        //Scalar opeartor-=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            m -= T(10);

            REQUIRE(m[0] == Col{-9, -8, -7});
            REQUIRE(m[1] == Col{-6, -5, -4});
            REQUIRE(m[2] == Col{-3, -2, -1});
        }

        //operator-=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            m -= m;

            REQUIRE(m[0] == Col{});
            REQUIRE(m[1] == Col{});
            REQUIRE(m[2] == Col{});
        }

        //Scalar operator*=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            m *= T(10);

            REQUIRE(m[0] == Col{10, 20, 30});
            REQUIRE(m[1] == Col{40, 50, 60});
            REQUIRE(m[2] == Col{70, 80, 90});
        }

        //operator*=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            m *= m;

            REQUIRE(m[0] == Col{1, 4, 9});
            REQUIRE(m[1] == Col{16, 25, 36});
            REQUIRE(m[2] == Col{49, 64, 81});
        }

        //Scalar operator/=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            m /= T(0.5);

            REQUIRE(m[0] == Col{2, 4, 6});
            REQUIRE(m[1] == Col{8, 10, 12});
            REQUIRE(m[2] == Col{14, 16, 18});
        }

        //operator/=
        {
            Mat m(T(1));
            Mat m1(T(2));
            m /= m1;

            REQUIRE(m[0] == Col{0.5, 0, 0});
            REQUIRE(m[1] == Col{0, 0.5, 0});
            REQUIRE(m[2] == Col{0, 0, 0.5});
        }

        //operator++
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            m++;

            REQUIRE(m[0] == Col{2, 3, 4});
            REQUIRE(m[1] == Col{5, 6, 7});
            REQUIRE(m[2] == Col{8, 9, 10});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            ++m;

            REQUIRE(m[0] == Col{2, 3, 4});
            REQUIRE(m[1] == Col{5, 6, 7});
            REQUIRE(m[2] == Col{8, 9, 10});
        }

        //operator--
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            m--;

            REQUIRE(m[0] == Col{0, 1, 2});
            REQUIRE(m[1] == Col{3, 4, 5});
            REQUIRE(m[2] == Col{6, 7, 8});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            --m;

            REQUIRE(m[0] == Col{0, 1, 2});
            REQUIRE(m[1] == Col{3, 4, 5});
            REQUIRE(m[2] == Col{6, 7, 8});
        }

        //Unary operator+
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = +m;

            REQUIRE(m[0] == Col{1, 2, 3});
            REQUIRE(m[1] == Col{4, 5, 6});
            REQUIRE(m[2] == Col{7, 8, 9});
        }

        //Unary operator-
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = -m;

            REQUIRE(m1[0] == Col{-1, -2, -3});
            REQUIRE(m1[1] == Col{-4, -5, -6});
            REQUIRE(m1[2] == Col{-7, -8, -9});
        }

        //Binary operator+
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = m + T(1);

            REQUIRE(m1[0] == Col{2, 3, 4});
            REQUIRE(m1[1] == Col{5, 6, 7});
            REQUIRE(m1[2] == Col{8, 9, 10});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = T(1) + m;

            REQUIRE(m1[0] == Col{2, 3, 4});
            REQUIRE(m1[1] == Col{5, 6, 7});
            REQUIRE(m1[2] == Col{8, 9, 10});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = m + m;

            REQUIRE(m1[0] == Col{2, 4, 6});
            REQUIRE(m1[1] == Col{8, 10, 12});
            REQUIRE(m1[2] == Col{14, 16 ,18});
        }

        //Binary operator-
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = m - T(1);

            REQUIRE(m1[0] == Col{0, 1, 2});
            REQUIRE(m1[1] == Col{3, 4, 5});
            REQUIRE(m1[2] == Col{6, 7, 8});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = T(1) - m;

            REQUIRE(m1[0] == Col{0, -1, -2});
            REQUIRE(m1[1] == Col{-3, -4, -5});
            REQUIRE(m1[2] == Col{-6, -7, -8});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = m - m;

            REQUIRE(m1[0] == Col{});
            REQUIRE(m1[1] == Col{});
            REQUIRE(m1[2] == Col{});
        }

        //Binary operator*
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = m * T(2);

            REQUIRE(m1[0] == Col{2, 4, 6});
            REQUIRE(m1[1] == Col{8, 10, 12});
            REQUIRE(m1[2] == Col{14, 16, 18});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = T(2) * m;

            REQUIRE(m1[0] == Col{2, 4, 6});
            REQUIRE(m1[1] == Col{8, 10, 12});
            REQUIRE(m1[2] == Col{14, 16, 18});
        }
        {
            Col v{T(1), T(2), T(3)};
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Col c = m * v;

            REQUIRE(c == Col{T(30), T(36), T(42)});
        }
        {
            Col v{T(1), T(2), T(3)};
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Col c = v * m;

            REQUIRE(c == Col{T(14), T(32), T(50)});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = m * m;

            REQUIRE(m1[0] == Col{30, 36, 42});
            REQUIRE(m1[1] == Col{66, 81, 96});
            REQUIRE(m1[2] == Col{102, 126, 150});
        }

        //Binary operator/
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1 = m / T(2);

            REQUIRE(m1[0] == Col{0.5, 1, 1.5});
            REQUIRE(m1[1] == Col{2, 2.5, 3});
            REQUIRE(m1[2] == Col{3.5, 4, 4.5});
        }
        {
            Mat m(T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2));
            Mat m1 = T(10) / m;

            REQUIRE(m1[0] == Col{5, 5, 5});
            REQUIRE(m1[1] == Col{5, 5, 5});
            REQUIRE(m1[2] == Col{5, 5, 5});
        }
        {
            Mat m(T(1));
            Col v(T(2));
            Col m1 = m / v;

            REQUIRE(m1 == Col{2, 2, 2});
        }
        {
            Mat m(T(1));
            Col v(T(2));
            Col m1 = v / m;

            REQUIRE(m1 == Col{2, 2, 2});
        }
        {
            Mat m(T(1));
            Mat m1(T(2));
            Mat m2 = m / m1;

            REQUIRE(m2[0] == Col{0.5, 0, 0});
            REQUIRE(m2[1] == Col{0, 0.5, 0});
            REQUIRE(m2[2] == Col{0, 0, 0.5});
        }

        //Binary operator==
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1(T(9), T(8), T(7), T(6), T(5), T(4), T(3), T(2), T(1));

            REQUIRE(m == m);
            REQUIRE(!(m == m1));
        }

        //Binary operator!=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
            Mat m1(T(9), T(8), T(7), T(6), T(5), T(4), T(3), T(2), T(1));

            REQUIRE(m != m1);
            REQUIRE(!(m != m));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat3HashRunTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;

        const Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));
        usize hash = std::hash<Mat>()(m);
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat3FormatRunTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;

        const Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

        if constexpr(std::same_as<T, f32>)
        {
            REQUIRE(fmt::format("{}", m) == fmt::format("Mat3f(({},{},{}), ({},{},{}), ({},{},{}))",
                                                        std::get<0>(std::get<0>(m)), std::get<1>(std::get<0>(m)),
							                            std::get<2>(std::get<0>(m)), std::get<0>(std::get<1>(m)),
						                                std::get<1>(std::get<1>(m)), std::get<2>(std::get<1>(m)),
							                            std::get<0>(std::get<2>(m)), std::get<1>(std::get<2>(m)),
							                            std::get<2>(std::get<2>(m))));
        }
        else if constexpr(std::same_as<T, f64>)
        {
            REQUIRE(fmt::format("{}", m) == fmt::format("Mat3d(({},{},{}), ({},{},{}), ({},{},{}))",
                                                        std::get<0>(std::get<0>(m)), std::get<1>(std::get<0>(m)),
							                            std::get<2>(std::get<0>(m)), std::get<0>(std::get<1>(m)),
						                                std::get<1>(std::get<1>(m)), std::get<2>(std::get<1>(m)),
							                            std::get<0>(std::get<2>(m)), std::get<1>(std::get<2>(m)),
							                            std::get<2>(std::get<2>(m))));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat3GetRunTimeTests()
    {
        using Mat = TRAP::Math::tMat3<T>;
        using Col = Mat::col_type;

        const Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

        REQUIRE(std::get<0>(m) == Col{1, 2, 3});
        REQUIRE(std::get<1>(m) == Col{4, 5, 6});
        REQUIRE(std::get<2>(m) == Col{7, 8, 9});

        REQUIRE(std::get<0>(std::move(m)) == Col{1, 2, 3});

        Mat m1(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9));

        REQUIRE(std::get<0>(m1) == Col{1, 2, 3});
        REQUIRE(std::get<1>(m1) == Col{4, 5, 6});
        REQUIRE(std::get<2>(m1) == Col{7, 8, 9});

        REQUIRE(std::get<0>(std::move(m1)) == Col{1, 2, 3});
    }
}

TEST_CASE("TRAP::Math::Mat3", "[math][mat][mat3]")
{
    SECTION("Typedefs")
    {
        RunMat3TypedefsCompileTimeTests<f32>();
        RunMat3TypedefsCompileTimeTests<f64>();
    }

    SECTION("Constructors")
    {
        RunMat3ConstructorCompileTimeTests<f32>();
        RunMat3ConstructorCompileTimeTests<f64>();

        RunMat3ConstructorRunTimeTests<f32>();
        RunMat3ConstructorRunTimeTests<f64>();
    }

    SECTION("Assignments")
    {
        RunMat3AssignmentRunTimeTests<f32>();
        RunMat3AssignmentRunTimeTests<f64>();
    }

    SECTION("Accessors")
    {
        RunMat3AccessorCompileTimeTests<f32>();
        RunMat3AccessorCompileTimeTests<f64>();

        RunMat3AccessorRunTimeTests<f32>();
        RunMat3AccessorRunTimeTests<f64>();
    }

    SECTION("Operators")
    {
        RunMat3OperatorCompileTimeTests<f32>();
        RunMat3OperatorCompileTimeTests<f64>();

        RunMat3OperatorRunTimeTests<f32>();
        RunMat3OperatorRunTimeTests<f64>();
    }

    SECTION("std::hash")
    {
        RunMat3HashRunTimeTests<f32>();
        RunMat3HashRunTimeTests<f64>();
    }

    SECTION("fmt::format")
    {
        RunMat3FormatRunTimeTests<f32>();
        RunMat3FormatRunTimeTests<f64>();
    }

    SECTION("std::get")
    {
        RunMat3GetRunTimeTests<f32>();
        RunMat3GetRunTimeTests<f64>();
    }
}

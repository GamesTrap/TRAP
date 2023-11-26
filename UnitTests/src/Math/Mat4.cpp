#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunMat4TypedefsCompileTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;

        static_assert(std::same_as<typename Mat::col_type, TRAP::Math::Vec<4, T>>);
        static_assert(std::same_as<typename Mat::row_type, TRAP::Math::Vec<4, T>>);
        static_assert(std::same_as<typename Mat::value_type, T>);
        static_assert(std::same_as<typename Mat::size_type, u32>);
        static_assert(std::same_as<typename Mat::difference_type, isize>);
        static_assert(std::same_as<typename Mat::iterator, typename std::array<TRAP::Math::Vec<4, T>, 4>::iterator>);
        static_assert(std::same_as<typename Mat::const_iterator, typename std::array<TRAP::Math::Vec<4, T>, 4>::const_iterator>);
        static_assert(std::same_as<typename Mat::reverse_iterator, typename std::array<TRAP::Math::Vec<4, T>, 4>::reverse_iterator>);
        static_assert(std::same_as<typename Mat::const_reverse_iterator, typename std::array<TRAP::Math::Vec<4, T>, 4>::const_reverse_iterator>);
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunMat4ConstructorCompileTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;
        using Col = Mat::col_type;

        //Default constructor
        {
            constexpr Mat m{};

            static_assert(std::get<0>(m) == Col{});
            static_assert(std::get<1>(m) == Col{});
            static_assert(std::get<2>(m) == Col{});
            static_assert(std::get<3>(m) == Col{});
        }

        //Move constructor
        {
        }

        //Copy constructor
        {
            constexpr Col c1{T(1), T(2), T(3), T(4)};
            constexpr Col c2{T(5), T(6), T(7), T(8)};
            constexpr Col c3{T(9), T(10), T(11), T(12)};
            constexpr Col c4{T(13), T(14), T(15), T(16)};
            constexpr Mat m{c1, c2, c3, c4};
            constexpr Mat mCopy(m);

            static_assert(std::get<0>(mCopy) == c1);
            static_assert(std::get<1>(mCopy) == c2);
            static_assert(std::get<2>(mCopy) == c3);
            static_assert(std::get<3>(mCopy) == c4);
        }

        //Scalar constructor
        {
            constexpr Mat m(T(1));

            static_assert(std::get<0>(m) == Col{T(1), T(0), T(0), T(0)});
            static_assert(std::get<1>(m) == Col{T(0), T(1), T(0), T(0)});
            static_assert(std::get<2>(m) == Col{T(0), T(0), T(1), T(0)});
            static_assert(std::get<3>(m) == Col{T(0), T(0), T(0), T(1)});
        }

        //Value constructor
        {
            constexpr Mat mCopy(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

            static_assert(std::get<0>(mCopy) == Col{T(1), T(2), T(3), T(4)});
            static_assert(std::get<1>(mCopy) == Col{T(5), T(6), T(7), T(8)});
            static_assert(std::get<2>(mCopy) == Col{T(9), T(10), T(11), T(12)});
            static_assert(std::get<3>(mCopy) == Col{T(13), T(14), T(15), T(16)});
        }

        //Column constructor
        {
            constexpr Col c1{T(1), T(2), T(3), T(4)};
            constexpr Col c2{T(5), T(6), T(7), T(8)};
            constexpr Col c3{T(9), T(10), T(11), T(12)};
            constexpr Col c4{T(13), T(14), T(15), T(16)};
            constexpr Mat m{c1, c2, c3, c4};

            static_assert(std::get<0>(m) == c1);
            static_assert(std::get<1>(m) == c2);
            static_assert(std::get<2>(m) == c3);
            static_assert(std::get<3>(m) == c4);
        }

        //Value conversion constructor
        {
            constexpr Mat mCopy(f64(1), f64(2), f64(3), f64(4), f64(5), f64(6), f64(7), f64(8), f64(9), f64(10), f64(11), f64(12), f64(13), f64(14), f64(15), f64(16));

            static_assert(std::get<0>(mCopy) == Col{T(1), T(2), T(3), T(4)});
            static_assert(std::get<1>(mCopy) == Col{T(5), T(6), T(7), T(8)});
            static_assert(std::get<2>(mCopy) == Col{T(9), T(10), T(11), T(12)});
            static_assert(std::get<3>(mCopy) == Col{T(13), T(14), T(15), T(16)});
        }

        //Column conversion constructor
        {
            constexpr TRAP::Math::tVec4<f64> v1{f64(1), f64(2), f64(3), f64(4)};
            constexpr TRAP::Math::tVec4<f64> v2{f64(5), f64(6), f64(7), f64(8)};
            constexpr TRAP::Math::tVec4<f64> v3{f64(9), f64(10), f64(11), f64(12)};
            constexpr TRAP::Math::tVec4<f64> v4{f64(13), f64(14), f64(15), f64(16)};
            constexpr Col c1{T(1), T(2), T(3), T(4)};
            constexpr Col c2{T(5), T(6), T(7), T(8)};
            constexpr Col c3{T(9), T(10), T(11), T(12)};
            constexpr Col c4{T(13), T(14), T(15), T(16)};
            constexpr Mat m{v1, v2, v3, v4};

            static_assert(std::get<0>(m) == c1);
            static_assert(std::get<1>(m) == c2);
            static_assert(std::get<2>(m) == c3);
            static_assert(std::get<3>(m) == c4);
        }

        //Copy conversion constructor
        {
            //Mat3
            {
                constexpr Col c1{T(1), T(2), T(3), T(0)};
                constexpr Col c2{T(4), T(5), T(6), T(0)};
                constexpr Col c3{T(7), T(8), T(9), T(0)};
                constexpr Col c4{T(0), T(0), T(0), T(1)};
                constexpr TRAP::Math::tVec3<f64> v1{f64(1), f64(2), f64(3)};
                constexpr TRAP::Math::tVec3<f64> v2{f64(4), f64(5), f64(6)};
                constexpr TRAP::Math::tVec3<f64> v3{f64(7), f64(8), f64(9)};
                constexpr TRAP::Math::tMat3<f64> m{v1, v2, v3};
                constexpr Mat mCopy(m);

                static_assert(std::get<0>(mCopy) == c1);
                static_assert(std::get<1>(mCopy) == c2);
                static_assert(std::get<2>(mCopy) == c3);
                static_assert(std::get<3>(mCopy) == c4);
            }

            //Mat4
            {
                constexpr Col c1{T(1), T(2), T(3), T(4)};
                constexpr Col c2{T(5), T(6), T(7), T(8)};
                constexpr Col c3{T(9), T(10), T(11), T(12)};
                constexpr Col c4{T(13), T(14), T(15), T(16)};
                constexpr TRAP::Math::tVec4<f64> v1{f64(1), f64(2), f64(3), f64(4)};
                constexpr TRAP::Math::tVec4<f64> v2{f64(5), f64(6), f64(7), f64(8)};
                constexpr TRAP::Math::tVec4<f64> v3{f64(9), f64(10), f64(11), f64(12)};
                constexpr TRAP::Math::tVec4<f64> v4{f64(13), f64(14), f64(15), f64(16)};
                constexpr TRAP::Math::tMat4<f64> m{v1, v2, v3, v4};
                constexpr Mat mCopy(m);

                static_assert(std::get<0>(mCopy) == c1);
                static_assert(std::get<1>(mCopy) == c2);
                static_assert(std::get<2>(mCopy) == c3);
                static_assert(std::get<3>(mCopy) == c4);
            }
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat4ConstructorRunTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;
        using Col = Mat::col_type;

        //Default constructor
        {
            Mat m{};

            REQUIRE(std::get<0>(m) == Col{});
            REQUIRE(std::get<1>(m) == Col{});
            REQUIRE(std::get<2>(m) == Col{});
            REQUIRE(std::get<3>(m) == Col{});
        }

        //Move constructor
        {
            Col c1{T(1), T(2), T(3), T(4)};
            Col c2{T(5), T(6), T(7), T(8)};
            Col c3{T(9), T(10), T(11), T(12)};
            Col c4{T(13), T(14), T(15), T(16)};
            Mat m{c1, c2, c3, c4};
            const Mat mMove(std::move(m));

            REQUIRE(std::get<0>(mMove) == c1);
            REQUIRE(std::get<1>(mMove) == c2);
            REQUIRE(std::get<2>(mMove) == c3);
            REQUIRE(std::get<3>(mMove) == c4);
        }

        //Copy constructor
        {
            Col c1{T(1), T(2), T(3), T(4)};
            Col c2{T(5), T(6), T(7), T(8)};
            Col c3{T(9), T(10), T(11), T(12)};
            Col c4{T(13), T(14), T(15), T(16)};
            const Mat m{c1, c2, c3, c4};
            Mat mCopy(m);

            REQUIRE(std::get<0>(mCopy) == c1);
            REQUIRE(std::get<1>(mCopy) == c2);
            REQUIRE(std::get<2>(mCopy) == c3);
            REQUIRE(std::get<3>(mCopy) == c4);
        }

        //Scalar constructor
        {
            T scalar(1);
            Mat m(scalar);

            REQUIRE(std::get<0>(m) == Col{T(1), T(0), T(0), T(0)});
            REQUIRE(std::get<1>(m) == Col{T(0), T(1), T(0), T(0)});
            REQUIRE(std::get<2>(m) == Col{T(0), T(0), T(1), T(0)});
            REQUIRE(std::get<3>(m) == Col{T(0), T(0), T(0), T(1)});
        }

        //Value constructor
        {
            Mat mCopy(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

            REQUIRE(std::get<0>(mCopy) == Col{T(1), T(2), T(3), T(4)});
            REQUIRE(std::get<1>(mCopy) == Col{T(5), T(6), T(7), T(8)});
            REQUIRE(std::get<2>(mCopy) == Col{T(9), T(10), T(11), T(12)});
            REQUIRE(std::get<3>(mCopy) == Col{T(13), T(14), T(15), T(16)});
        }

        //Column constructor
        {
            Col c1{T(1), T(2), T(3), T(4)};
            Col c2{T(5), T(6), T(7), T(8)};
            Col c3{T(9), T(10), T(11), T(12)};
            Col c4{T(13), T(14), T(15), T(16)};
            Mat m{c1, c2, c3, c4};

            REQUIRE(std::get<0>(m) == c1);
            REQUIRE(std::get<1>(m) == c2);
            REQUIRE(std::get<2>(m) == c3);
            REQUIRE(std::get<3>(m) == c4);
        }

        //Value conversion constructor
        {
            f64 x = 1.0f;
            f64 y = 2.0f;
            f64 z = 3.0f;
            f64 w = 4.0f;
            Mat m(x, y, z, w, x, y, z, w, x, y, z, w, x, y, z, w);

            REQUIRE(std::get<0>(m) == Col{T(1), T(2), T(3), T(4)});
            REQUIRE(std::get<1>(m) == Col{T(1), T(2), T(3), T(4)});
            REQUIRE(std::get<2>(m) == Col{T(1), T(2), T(3), T(4)});
            REQUIRE(std::get<3>(m) == Col{T(1), T(2), T(3), T(4)});
        }

        //Column conversion constructor
        {
            TRAP::Math::tVec4<f64> v1{f64(1), f64(2), f64(3), f64(4)};
            TRAP::Math::tVec4<f64> v2{f64(5), f64(6), f64(7), f64(8)};
            TRAP::Math::tVec4<f64> v3{f64(9), f64(10), f64(11), f64(12)};
            TRAP::Math::tVec4<f64> v4{f64(13), f64(14), f64(15), f64(16)};
            Col c1{T(1), T(2), T(3), T(4)};
            Col c2{T(5), T(6), T(7), T(8)};
            Col c3{T(9), T(10), T(11), T(12)};
            Col c4{T(13), T(14), T(15), T(16)};
            Mat m{v1, v2, v3, v4};

            REQUIRE(std::get<0>(m) == c1);
            REQUIRE(std::get<1>(m) == c2);
            REQUIRE(std::get<2>(m) == c3);
            REQUIRE(std::get<3>(m) == c4);
        }

        //Copy conversion constructor
        {
            //Mat3
            {
                Col c1{T(1), T(2), T(3), T(0)};
                Col c2{T(4), T(5), T(6), T(0)};
                Col c3{T(7), T(8), T(9), T(0)};
                Col c4{T(0), T(0), T(0), T(1)};
                TRAP::Math::tVec3<f64> v1{f64(1), f64(2), f64(3)};
                TRAP::Math::tVec3<f64> v2{f64(4), f64(5), f64(6)};
                TRAP::Math::tVec3<f64> v3{f64(7), f64(8), f64(9)};
                const TRAP::Math::tMat3<f64> m{v1, v2, v3};
                Mat mCopy(m);

                REQUIRE(std::get<0>(mCopy) == c1);
                REQUIRE(std::get<1>(mCopy) == c2);
                REQUIRE(std::get<2>(mCopy) == c3);
                REQUIRE(std::get<3>(mCopy) == c4);
            }

            //Mat4
            {
                Col c1{T(1), T(2), T(3), T(4)};
                Col c2{T(5), T(6), T(7), T(8)};
                Col c3{T(9), T(10), T(11), T(12)};
                Col c4{T(13), T(14), T(15), T(16)};
                TRAP::Math::tVec4<f64> v1{f64(1), f64(2), f64(3), f64(4)};
                TRAP::Math::tVec4<f64> v2{f64(5), f64(6), f64(7), f64(8)};
                TRAP::Math::tVec4<f64> v3{f64(9), f64(10), f64(11), f64(12)};
                TRAP::Math::tVec4<f64> v4{f64(13), f64(14), f64(15), f64(16)};
                const TRAP::Math::tMat4<f64> m{v1, v2, v3, v4};
                Mat mCopy(m);

                REQUIRE(std::get<0>(mCopy) == c1);
                REQUIRE(std::get<1>(mCopy) == c2);
                REQUIRE(std::get<2>(mCopy) == c3);
                REQUIRE(std::get<3>(mCopy) == c4);
            }
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat4AssignmentRunTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;
        using Col = Mat::col_type;

        //Move assignment
        {
            Col c1{T(1), T(2), T(3), T(4)};
            Col c2{T(5), T(6), T(7), T(8)};
            Col c3{T(9), T(10), T(11), T(12)};
            Col c4{T(13), T(14), T(15), T(16)};
            Mat m{c1, c2, c3, c4};
            Mat mMove{};
            mMove = std::move(m);

            REQUIRE(std::get<0>(mMove) == c1);
            REQUIRE(std::get<1>(mMove) == c2);
            REQUIRE(std::get<2>(mMove) == c3);
            REQUIRE(std::get<3>(mMove) == c4);
        }

        //Copy assignment
        {
            Col c1{T(1), T(2), T(3), T(4)};
            Col c2{T(5), T(6), T(7), T(8)};
            Col c3{T(9), T(10), T(11), T(12)};
            Col c4{T(13), T(14), T(15), T(16)};
            const Mat m{c1, c2, c3, c4};
            Mat mCopy{};
            mCopy = m;

            REQUIRE(std::get<0>(mCopy) == c1);
            REQUIRE(std::get<1>(mCopy) == c2);
            REQUIRE(std::get<2>(mCopy) == c3);
            REQUIRE(std::get<3>(mCopy) == c4);
        }

        //Copy conversion assignment
        {
            Col c1{T(1), T(2), T(3), T(4)};
            Col c2{T(5), T(6), T(7), T(8)};
            Col c3{T(9), T(10), T(11), T(12)};
            Col c4{T(13), T(14), T(15), T(16)};
            TRAP::Math::tVec4<f64> v1{f64(1), f64(2), f64(3), f64(4)};
            TRAP::Math::tVec4<f64> v2{f64(5), f64(6), f64(7), f64(8)};
            TRAP::Math::tVec4<f64> v3{f64(9), f64(10), f64(11), f64(12)};
            TRAP::Math::tVec4<f64> v4{f64(13), f64(14), f64(15), f64(16)};
            TRAP::Math::tMat4<f64> m{v1, v2, v3, v4};
            Mat mCopy{};
            mCopy = m;

            REQUIRE(std::get<0>(mCopy) == c1);
            REQUIRE(std::get<1>(mCopy) == c2);
            REQUIRE(std::get<2>(mCopy) == c3);
            REQUIRE(std::get<3>(mCopy) == c4);
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunMat4AccessorCompileTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;
        using Col = Mat::col_type;

        //Length
        {
            static_assert(Mat::Length() == 4);
        }

        //operator[]
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

            static_assert(m[0] == Col(T(1), T(2), T(3), T(4)));
            static_assert(m[1] == Col(T(5), T(6), T(7), T(8)));
            static_assert(m[2] == Col(T(9), T(10), T(11), T(12)));
            static_assert(m[3] == Col(T(13), T(14), T(15), T(16)));
            //Check that m[0] returns const Col&
            static_assert(std::same_as<decltype(m[0]), const Col&>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

            static_assert(m.begin() == &m[0]);
            static_assert(m.cbegin() == &m[0]);
            static_assert(m.rbegin() == std::reverse_iterator((&m[3]) + 1));
            static_assert(m.crbegin() == std::reverse_iterator((&m[3]) + 1));
            static_assert(m.end() == ((&m[3]) + 1));
            static_assert(m.cend() == ((&m[3]) + 1));
            static_assert(m.rend() == std::reverse_iterator(&m[0]));
            static_assert(m.crend() == std::reverse_iterator(&m[0]));
        }
#endif
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat4AccessorRunTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;
        using Col = Mat::col_type;

        //Length
        {
            REQUIRE(Mat::Length() == 4);
        }

        //operator[]
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

            REQUIRE(m[0] == Col(T(1), T(2), T(3), T(4)));
            REQUIRE(m[1] == Col(T(5), T(6), T(7), T(8)));
            REQUIRE(m[2] == Col(T(9), T(10), T(11), T(12)));
            REQUIRE(m[3] == Col(T(13), T(14), T(15), T(16)));
            //Check that m[0] returns Col&
            REQUIRE(std::same_as<decltype(m[0]), Col&>);

            const Mat m1(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

            REQUIRE(m1[0] == Col(T(1), T(2), T(3), T(4)));
            REQUIRE(m1[1] == Col(T(5), T(6), T(7), T(8)));
            REQUIRE(m1[2] == Col(T(9), T(10), T(11), T(12)));
            REQUIRE(m1[3] == Col(T(13), T(14), T(15), T(16)));
            //Check that m1[0] returns const Col&
            REQUIRE(std::same_as<decltype(m1[0]), const Col&>);
        }

        //at()
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

            REQUIRE(m.at(0) == Col(T(1), T(2), T(3), T(4)));
            REQUIRE(m.at(1) == Col(T(5), T(6), T(7), T(8)));
            REQUIRE(m.at(2) == Col(T(9), T(10), T(11), T(12)));
            REQUIRE(m.at(3) == Col(T(13), T(14), T(15), T(16)));
            //Check that m[0] returns Col&
            REQUIRE(std::same_as<decltype(m[0]), Col&>);

            const Mat m1(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

            REQUIRE(m1.at(0) == Col(T(1), T(2), T(3), T(4)));
            REQUIRE(m1.at(1) == Col(T(5), T(6), T(7), T(8)));
            REQUIRE(m1.at(2) == Col(T(9), T(10), T(11), T(12)));
            REQUIRE(m1.at(3) == Col(T(13), T(14), T(15), T(16)));
            //Check that m1[0] returns const Col&
            REQUIRE(std::same_as<decltype(m1[0]), const Col&>);
        }

#ifndef TRAP_PLATFORM_WINDOWS
        //iterators
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

            REQUIRE(m.begin() == &m[0]);
            REQUIRE(m.cbegin() == &m[0]);
            REQUIRE(m.rbegin() == std::reverse_iterator((&m[3]) + 1));
            REQUIRE(m.crbegin() == std::reverse_iterator((&m[3]) + 1));
            REQUIRE(m.end() == ((&m[3]) + 1));
            REQUIRE(m.cend() == ((&m[3]) + 1));
            REQUIRE(m.rend() == std::reverse_iterator(&m[0]));
            REQUIRE(m.crend() == std::reverse_iterator(&m[0]));

            constexpr Mat m2(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

            REQUIRE(m2.begin() == &m2[0]);
            REQUIRE(m2.cbegin() == &m2[0]);
            REQUIRE(m2.rbegin() == std::reverse_iterator((&m2[3]) + 1));
            REQUIRE(m2.crbegin() == std::reverse_iterator((&m2[3]) + 1));
            REQUIRE(m2.end() == ((&m2[3]) + 1));
            REQUIRE(m2.cend() == ((&m2[3]) + 1));
            REQUIRE(m2.rend() == std::reverse_iterator(&m2[0]));
            REQUIRE(m2.crend() == std::reverse_iterator(&m2[0]));
        }
#endif
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunMat4OperatorCompileTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;
        using Col = Mat::col_type;

        //Unary operator+
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = +m;

            static_assert(m[0] == Col{1, 2, 3, 4});
            static_assert(m[1] == Col{5, 6, 7, 8});
            static_assert(m[2] == Col{9, 10, 11, 12});
            static_assert(m[3] == Col{13, 14, 15, 16});
        }

        //Unary operator-
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = -m;

            static_assert(m1[0] == Col{-1, -2, -3, -4});
            static_assert(m1[1] == Col{-5, -6, -7, -8});
            static_assert(m1[2] == Col{-9, -10, -11, -12});
            static_assert(m1[3] == Col{-13, -14, -15, -16});
        }

        //Binary operator+
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = m + T(1);

            static_assert(m1[0] == Col{2, 3, 4, 5});
            static_assert(m1[1] == Col{6, 7, 8, 9});
            static_assert(m1[2] == Col{10, 11, 12, 13});
            static_assert(m1[3] == Col{14, 15, 16, 17});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = T(1) + m;

            static_assert(m1[0] == Col{2, 3, 4, 5});
            static_assert(m1[1] == Col{6, 7, 8, 9});
            static_assert(m1[2] == Col{10, 11, 12, 13});
            static_assert(m1[3] == Col{14, 15, 16, 17});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = m + m;

            static_assert(m1[0] == Col{2, 4, 6, 8});
            static_assert(m1[1] == Col{10, 12, 14, 16});
            static_assert(m1[2] == Col{18, 20, 22, 24});
            static_assert(m1[3] == Col{26, 28, 30, 32});
        }

        //Binary operator-
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = m - T(1);

            static_assert(m1[0] == Col{0, 1, 2, 3});
            static_assert(m1[1] == Col{4, 5, 6, 7});
            static_assert(m1[2] == Col{8, 9, 10, 11});
            static_assert(m1[3] == Col{12, 13, 14, 15});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = T(1) - m;

            static_assert(m1[0] == Col{0, -1, -2, -3});
            static_assert(m1[1] == Col{-4, -5, -6, -7});
            static_assert(m1[2] == Col{-8, -9, -10, -11});
            static_assert(m1[3] == Col{-12, -13, -14, -15});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = m - m;

            static_assert(m1[0] == Col{});
            static_assert(m1[1] == Col{});
            static_assert(m1[2] == Col{});
            static_assert(m1[3] == Col{});
        }

        //Binary operator*
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = m * T(2);

            static_assert(m1[0] == Col{2, 4, 6, 8});
            static_assert(m1[1] == Col{10, 12, 14, 16});
            static_assert(m1[2] == Col{18, 20, 22, 24});
            static_assert(m1[3] == Col{26, 28, 30, 32});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = T(2) * m;

            static_assert(m1[0] == Col{2, 4, 6, 8});
            static_assert(m1[1] == Col{10, 12, 14, 16});
            static_assert(m1[2] == Col{18, 20, 22, 24});
            static_assert(m1[3] == Col{26, 28, 30, 32});
        }
        {
            constexpr Col v{T(1), T(2), T(3), T(4)};
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Col c = m * v;

            static_assert(c == Col{T(90), T(100), T(110), T(120)});
        }
        {
            constexpr Col v{T(1), T(2), T(3), T(4)};
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Col c = v * m;

            static_assert(c == Col{T(30), T(70), T(110), T(150)});
        }
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = m * m;

            static_assert(m1[0] == Col{90, 100, 110, 120});
            static_assert(m1[1] == Col{202, 228, 254, 280});
            static_assert(m1[2] == Col{314, 356, 398, 440});
            static_assert(m1[3] == Col{426, 484, 542, 600});
        }

        //Binary operator/
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1 = m / T(2);

            static_assert(m1[0] == Col{0.5, 1, 1.5, 2});
            static_assert(m1[1] == Col{2.5, 3, 3.5, 4});
            static_assert(m1[2] == Col{4.5, 5, 5.5, 6});
            static_assert(m1[3] == Col{6.5, 7, 7.5, 8});
        }
        {
            constexpr Mat m(T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2));
            constexpr Mat m1 = T(10) / m;

            static_assert(m1[0] == Col{5, 5, 5, 5});
            static_assert(m1[1] == Col{5, 5, 5, 5});
            static_assert(m1[2] == Col{5, 5, 5, 5});
            static_assert(m1[3] == Col{5, 5, 5, 5});
        }
        {
            constexpr Mat m(T(1));
            constexpr Col v(T(2));
            constexpr Col m1 = m / v;

            static_assert(m1 == Col{2, 2, 2, 2});
        }
        {
            constexpr Mat m(T(1));
            constexpr Col v(T(2));
            constexpr Col m1 = v / m;

            static_assert(m1 == Col{2, 2, 2, 2});
        }
        {
            constexpr Mat m(T(1));
            constexpr Mat m1(T(2));
            constexpr Mat m2 = m / m1;

            static_assert(m2[0] == Col{0.5, 0, 0, 0});
            static_assert(m2[1] == Col{0, 0.5, 0, 0});
            static_assert(m2[2] == Col{0, 0, 0.5, 0});
            static_assert(m2[3] == Col{0, 0, 0, 0.5});
        }

        //Binary operator==
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1(T(16), T(15), T(14), T(13), T(12), T(11), T(10), T(9), T(8), T(7), T(6), T(5), T(4), T(3), T(2), T(1));

            static_assert(m == m);
            static_assert(!(m == m1));
        }

        //Binary operator!=
        {
            constexpr Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            constexpr Mat m1(T(16), T(15), T(14), T(13), T(12), T(11), T(10), T(9), T(8), T(7), T(6), T(5), T(4), T(3), T(2), T(1));

            static_assert(m != m1);
            static_assert(!(m != m));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat4OperatorRunTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;
        using Col = Mat::col_type;

        //Scalar operator+=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            m += T(10);

            REQUIRE(m[0] == Col{11, 12, 13, 14});
            REQUIRE(m[1] == Col{15, 16, 17, 18});
            REQUIRE(m[2] == Col{19, 20, 21, 22});
            REQUIRE(m[3] == Col{23, 24, 25, 26});
        }

        //operator+=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            m += m;

            REQUIRE(m[0] == Col{2, 4, 6, 8});
            REQUIRE(m[1] == Col{10, 12, 14, 16});
            REQUIRE(m[2] == Col{18, 20, 22, 24});
            REQUIRE(m[3] == Col{26, 28, 30, 32});
        }

        //Scalar opeartor-=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            m -= T(10);

            REQUIRE(m[0] == Col{-9, -8, -7, -6});
            REQUIRE(m[1] == Col{-5, -4, -3, -2});
            REQUIRE(m[2] == Col{-1, 0, 1, 2});
            REQUIRE(m[3] == Col{3, 4, 5, 6});
        }

        //operator-=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            m -= m;

            REQUIRE(m[0] == Col{});
            REQUIRE(m[1] == Col{});
            REQUIRE(m[2] == Col{});
            REQUIRE(m[3] == Col{});
        }

        //Scalar operator*=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            m *= T(10);

            REQUIRE(m[0] == Col{10, 20, 30, 40});
            REQUIRE(m[1] == Col{50, 60, 70, 80});
            REQUIRE(m[2] == Col{90, 100, 110, 120});
            REQUIRE(m[3] == Col{130, 140, 150, 160});
        }

        //operator*=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            m *= m;

            REQUIRE(m[0] == Col{1, 4, 9, 16});
            REQUIRE(m[1] == Col{25, 36, 49, 64});
            REQUIRE(m[2] == Col{81, 100, 121, 144});
            REQUIRE(m[3] == Col{169, 196, 225, 256});
        }

        //Scalar operator/=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            m /= T(0.5);

            REQUIRE(m[0] == Col{2, 4, 6, 8});
            REQUIRE(m[1] == Col{10, 12, 14, 16});
            REQUIRE(m[2] == Col{18, 20, 22, 24});
            REQUIRE(m[3] == Col{26, 28, 30, 32});
        }

        //operator/=
        {
            Mat m(T(1));
            Mat m1(T(2));
            m /= m1;

            REQUIRE(m[0] == Col{0.5, 0, 0, 0});
            REQUIRE(m[1] == Col{0, 0.5, 0, 0});
            REQUIRE(m[2] == Col{0, 0, 0.5, 0});
            REQUIRE(m[3] == Col{0, 0, 0, 0.5});
        }

        //operator++
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            m++;

            REQUIRE(m[0] == Col{2, 3, 4, 5});
            REQUIRE(m[1] == Col{6, 7, 8, 9});
            REQUIRE(m[2] == Col{10, 11, 12, 13});
            REQUIRE(m[3] == Col{14, 15, 16, 17});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            ++m;

            REQUIRE(m[0] == Col{2, 3, 4, 5});
            REQUIRE(m[1] == Col{6, 7, 8, 9});
            REQUIRE(m[2] == Col{10, 11, 12, 13});
            REQUIRE(m[3] == Col{14, 15, 16, 17});
        }

        //operator--
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            m--;

            REQUIRE(m[0] == Col{0, 1, 2, 3});
            REQUIRE(m[1] == Col{4, 5, 6, 7});
            REQUIRE(m[2] == Col{8, 9, 10, 11});
            REQUIRE(m[3] == Col{12, 13, 14, 15});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            --m;

            REQUIRE(m[0] == Col{0, 1, 2, 3});
            REQUIRE(m[1] == Col{4, 5, 6, 7});
            REQUIRE(m[2] == Col{8, 9, 10, 11});
            REQUIRE(m[3] == Col{12, 13, 14, 15});
        }

        //Unary operator+
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = +m;

            REQUIRE(m[0] == Col{1, 2, 3, 4});
            REQUIRE(m[1] == Col{5, 6, 7, 8});
            REQUIRE(m[2] == Col{9, 10, 11, 12});
            REQUIRE(m[3] == Col{13, 14, 15, 16});
        }

        //Unary operator-
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = -m;

            REQUIRE(m1[0] == Col{-1, -2, -3, -4});
            REQUIRE(m1[1] == Col{-5, -6, -7, -8});
            REQUIRE(m1[2] == Col{-9, -10, -11, -12});
            REQUIRE(m1[3] == Col{-13, -14, -15, -16});
        }

        //Binary operator+
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = m + T(1);

            REQUIRE(m1[0] == Col{2, 3, 4, 5});
            REQUIRE(m1[1] == Col{6, 7, 8, 9});
            REQUIRE(m1[2] == Col{10, 11, 12, 13});
            REQUIRE(m1[3] == Col{14, 15, 16, 17});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = T(1) + m;

            REQUIRE(m1[0] == Col{2, 3, 4, 5});
            REQUIRE(m1[1] == Col{6, 7, 8, 9});
            REQUIRE(m1[2] == Col{10, 11, 12, 13});
            REQUIRE(m1[3] == Col{14, 15, 16, 17});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = m + m;

            REQUIRE(m1[0] == Col{2, 4, 6, 8});
            REQUIRE(m1[1] == Col{10, 12, 14, 16});
            REQUIRE(m1[2] == Col{18, 20, 22, 24});
            REQUIRE(m1[3] == Col{26, 28, 30, 32});
        }

        //Binary operator-
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = m - T(1);

            REQUIRE(m1[0] == Col{0, 1, 2, 3});
            REQUIRE(m1[1] == Col{4, 5, 6, 7});
            REQUIRE(m1[2] == Col{8, 9, 10, 11});
            REQUIRE(m1[3] == Col{12, 13, 14, 15});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = T(1) - m;

            REQUIRE(m1[0] == Col{0, -1, -2, -3});
            REQUIRE(m1[1] == Col{-4, -5, -6, -7});
            REQUIRE(m1[2] == Col{-8, -9, -10, -11});
            REQUIRE(m1[3] == Col{-12, -13, -14, -15});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = m - m;

            REQUIRE(m1[0] == Col{});
            REQUIRE(m1[1] == Col{});
            REQUIRE(m1[2] == Col{});
            REQUIRE(m1[3] == Col{});
        }

        //Binary operator*
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = m * T(2);

            REQUIRE(m1[0] == Col{2, 4, 6, 8});
            REQUIRE(m1[1] == Col{10, 12, 14, 16});
            REQUIRE(m1[2] == Col{18, 20, 22, 24});
            REQUIRE(m1[3] == Col{26, 28, 30, 32});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = T(2) * m;

            REQUIRE(m1[0] == Col{2, 4, 6, 8});
            REQUIRE(m1[1] == Col{10, 12, 14, 16});
            REQUIRE(m1[2] == Col{18, 20, 22, 24});
            REQUIRE(m1[3] == Col{26, 28, 30, 32});
        }
        {
            Col v{T(1), T(2), T(3), T(4)};
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Col c = m * v;

            REQUIRE(c == Col{T(90), T(100), T(110), T(120)});
        }
        {
            Col v{T(1), T(2), T(3), T(4)};
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Col c = v * m;

            REQUIRE(c == Col{T(30), T(70), T(110), T(150)});
        }
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = m * m;

            REQUIRE(m1[0] == Col{90, 100, 110, 120});
            REQUIRE(m1[1] == Col{202, 228, 254, 280});
            REQUIRE(m1[2] == Col{314, 356, 398, 440});
            REQUIRE(m1[3] == Col{426, 484, 542, 600});
        }

        //Binary operator/
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1 = m / T(2);

            REQUIRE(m1[0] == Col{0.5, 1, 1.5, 2});
            REQUIRE(m1[1] == Col{2.5, 3, 3.5, 4});
            REQUIRE(m1[2] == Col{4.5, 5, 5.5, 6});
            REQUIRE(m1[3] == Col{6.5, 7, 7.5, 8});
        }
        {
            Mat m(T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2), T(2));
            Mat m1 = T(10) / m;

            REQUIRE(m1[0] == Col{5, 5, 5, 5});
            REQUIRE(m1[1] == Col{5, 5, 5, 5});
            REQUIRE(m1[2] == Col{5, 5, 5, 5});
            REQUIRE(m1[3] == Col{5, 5, 5, 5});
        }
        {
            Mat m(T(1));
            Col v(T(2));
            Col m1 = m / v;

            REQUIRE(m1 == Col{2, 2, 2, 2});
        }
        {
            Mat m(T(1));
            Col v(T(2));
            Col m1 = v / m;

            REQUIRE(m1 == Col{2, 2, 2, 2});
        }
        {
            Mat m(T(1));
            Mat m1(T(2));
            Mat m2 = m / m1;

            REQUIRE(m2[0] == Col{0.5, 0, 0, 0});
            REQUIRE(m2[1] == Col{0, 0.5, 0, 0});
            REQUIRE(m2[2] == Col{0, 0, 0.5, 0});
            REQUIRE(m2[3] == Col{0, 0, 0, 0.5});
        }

        //Binary operator==
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1(T(16), T(15), T(14), T(13), T(12), T(11), T(10), T(9), T(8), T(7), T(6), T(5), T(4), T(3), T(2), T(1));

            REQUIRE(m == m);
            REQUIRE(!(m == m1));
        }

        //Binary operator!=
        {
            Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
            Mat m1(T(16), T(15), T(14), T(13), T(12), T(11), T(10), T(9), T(8), T(7), T(6), T(5), T(4), T(3), T(2), T(1));

            REQUIRE(m != m1);
            REQUIRE(!(m != m));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat4HashRunTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;

        const Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));
        usize hash = std::hash<Mat>()(m);
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat4FormatRunTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;

        const Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

        if constexpr(std::same_as<T, f32>)
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
        else if constexpr(std::same_as<T, f64>)
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

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunMat4GetRunTimeTests()
    {
        using Mat = TRAP::Math::tMat4<T>;
        using Col = Mat::col_type;

        const Mat m(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

        REQUIRE(std::get<0>(m) == Col{1, 2, 3, 4});
        REQUIRE(std::get<1>(m) == Col{5, 6, 7, 8});
        REQUIRE(std::get<2>(m) == Col{9, 10, 11, 12});
        REQUIRE(std::get<3>(m) == Col{13, 14, 15, 16});

        REQUIRE(std::get<0>(std::move(m)) == Col{1, 2, 3, 4});

        Mat m1(T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8), T(9), T(10), T(11), T(12), T(13), T(14), T(15), T(16));

        REQUIRE(std::get<0>(m1) == Col{1, 2, 3, 4});
        REQUIRE(std::get<1>(m1) == Col{5, 6, 7, 8});
        REQUIRE(std::get<2>(m1) == Col{9, 10, 11, 12});
        REQUIRE(std::get<3>(m1) == Col{13, 14, 15, 16});

        REQUIRE(std::get<0>(std::move(m1)) == Col{1, 2, 3, 4});
    }
}

TEST_CASE("TRAP::Math::Mat4", "[math][mat][mat4]")
{
    SECTION("Typedefs")
    {
        RunMat4TypedefsCompileTimeTests<f32>();
        RunMat4TypedefsCompileTimeTests<f64>();
    }

    SECTION("Constructors")
    {
        RunMat4ConstructorCompileTimeTests<f32>();
        RunMat4ConstructorCompileTimeTests<f64>();

        RunMat4ConstructorRunTimeTests<f32>();
        RunMat4ConstructorRunTimeTests<f64>();
    }

    SECTION("Assignments")
    {
        RunMat4AssignmentRunTimeTests<f32>();
        RunMat4AssignmentRunTimeTests<f64>();
    }

    SECTION("Accessors")
    {
        RunMat4AccessorCompileTimeTests<f32>();
        RunMat4AccessorCompileTimeTests<f64>();

        RunMat4AccessorRunTimeTests<f32>();
        RunMat4AccessorRunTimeTests<f64>();
    }

    SECTION("Operators")
    {
        RunMat4OperatorCompileTimeTests<f32>();
        RunMat4OperatorCompileTimeTests<f64>();

        RunMat4OperatorRunTimeTests<f32>();
        RunMat4OperatorRunTimeTests<f64>();
    }

    SECTION("std::hash")
    {
        RunMat4HashRunTimeTests<f32>();
        RunMat4HashRunTimeTests<f64>();
    }

    SECTION("fmt::format")
    {
        RunMat4FormatRunTimeTests<f32>();
        RunMat4FormatRunTimeTests<f64>();
    }

    SECTION("std::get")
    {
        RunMat4GetRunTimeTests<f32>();
        RunMat4GetRunTimeTests<f64>();
    }
}

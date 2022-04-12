/*
================================================================================
The Happy Bunny License (Modified MIT License)
--------------------------------------------------------------------------------
Copyright (c) 2005 - G-Truc Creation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Restrictions:
 By making use of the Software for military purposes, you choose to make a
 Bunny unhappy.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Modified by: Jan "GamesTrap" Schuerkamp
*/

#ifndef TRAP_MAT4_H
#define TRAP_MAT4_H

#include "Vec4.h"
#include "Core/PlatformDetection.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"

namespace TRAP::Math
{
	/// <summary>
	/// Struct for a 4x4 matrix.
	/// </summary>
	template<typename T>
	struct Mat<4, 4, T>
	{
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Mat(Mat&&) = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~Mat() = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr Mat<4, 4, T>& operator=(Mat&&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr Mat<4, 4, T>& operator=(const Mat&) = default;

		typedef Vec<4, T> colType;
		typedef Vec<4, T> rowType;
		typedef Mat<4, 4, T> type;
		typedef Mat<4, 4, T> transposeType;
		typedef T valueType;

	private:
		std::array<colType, 4> value{};

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		constexpr Mat() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Mat(const Mat<4, 4, T> & m) = default;

		/// <summary>
		/// Scalar constructor.
		/// </summary>
		explicit constexpr Mat(const T & scalar);
		/// <summary>
		/// Value constructor.
		/// </summary>
		constexpr Mat(const T & x0, const T & y0, const T & z0, const T & w0,
			          const T & x1, const T & y1, const T & z1, const T & w1,
			          const T & x2, const T & y2, const T & z2, const T & w2,
			          const T & x3, const T & y3, const T & z3, const T & w3);
		/// <summary>
		/// Column constructor.
		/// </summary>
		constexpr Mat(const colType & v0, const colType & v1, const colType & v2, const colType & v3);

		/// <summary>
		/// Value conversion constructor.
		/// </summary>
		template<typename X1, typename Y1, typename Z1, typename W1,
				 typename X2, typename Y2, typename Z2, typename W2,
				 typename X3, typename Y3, typename Z3, typename W3,
				 typename X4, typename Y4, typename Z4, typename W4>
		constexpr Mat(X1 x1, Y1 y1, Z1 z1, W1 w1,
			          X2 x2, Y2 y2, Z2 z2, W2 w2,
			          X3 x3, Y3 y3, Z3 z3, W3 w3,
			          X4 x4, Y4 y4, Z4 z4, W4 w4);

		/// <summary>
		/// Column conversion constructor.
		/// </summary>
		template<typename V1, typename V2, typename V3, typename V4>
		constexpr Mat(const Vec<4, V1> & v1, const Vec<4, V2> & v2, const Vec<4, V3> & v3, const Vec<4, V4> & v4);

		/// <summary>
		/// Copy conversion constructor.
		/// </summary>
		template<typename U>
		explicit constexpr Mat(const Mat<4, 4, U> & m);

		/// <summary>
		/// Copy conversion constructor.
		/// </summary>
		explicit constexpr Mat(const Mat<3, 3, T> & x);

		/// <summary>
		/// Retrieve the length of the matrix.
		/// </summary>
		/// <returns>Length.</returns>
		static constexpr int32_t Length();

		/// <summary>
		/// Retrieve a column of the matrix.
		/// </summary>
		/// <param name="i">Column to retrieve.</param>
		colType& operator[](int32_t i);
		/// <summary>
		/// Retrieve a column of the matrix.
		/// </summary>
		/// <param name="i">Column to retrieve.</param>
		constexpr const colType& operator[](int32_t i) const;

		//Unary arithmetic operators
		template<typename U>
		Mat<4, 4, T>& operator=(const Mat<4, 4, U>& m);
		template<typename U>
		Mat<4, 4, T>& operator+=(U s);
		template<typename U>
		Mat<4, 4, T>& operator+=(const Mat<4, 4, U>& m);
		template<typename U>
		Mat<4, 4, T>& operator-=(U s);
		template<typename U>
		Mat<4, 4, T>& operator-=(const Mat<4, 4, U>& m);
		template<typename U>
		Mat<4, 4, T>& operator*=(U s);
		template<typename U>
		Mat<4, 4, T>& operator*=(const Mat<4, 4, U>& m);
		template<typename U>
		Mat<4, 4, T>& operator/=(U s);
		template<typename U>
		Mat<4, 4, T>& operator/=(const Mat<4, 4, U>& m);

		//Increment and decrement operators
		Mat<4, 4, T>& operator++();
		Mat<4, 4, T>& operator--();
		Mat<4, 4, T> operator++(int);
		Mat<4, 4, T> operator--(int);
	};

	//Unary operators
	template<typename T>
	Mat<4, 4, T> operator+(const Mat<4, 4, T>& m);

	template<typename T>
	Mat<4, 4, T> operator-(const Mat<4, 4, T>& m);

	//Binary operators
	template<typename T>
	Mat<4, 4, T> operator+(const Mat<4, 4, T>& m, const T& scalar);

	template<typename T>
	Mat<4, 4, T> operator+(const T& scalar, const Mat<4, 4, T>& m);

	template<typename T>
	Mat<4, 4, T> operator+(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2);

	template<typename T>
	Mat<4, 4, T> operator-(const Mat<4, 4, T>& m, const T& scalar);

	template<typename T>
	Mat<4, 4, T> operator-(const T& scalar, const Mat<4, 4, T>& m);

	template<typename T>
	Mat<4, 4, T> operator-(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2);

	template<typename T>
	Mat<4, 4, T> operator*(const Mat<4, 4, T>& m, const T& scalar);

	template<typename T>
	Mat<4, 4, T> operator*(const T& scalar, const Mat<4, 4, T>& m);

	template<typename T>
	typename Mat<4, 4, T>::colType operator*(const Mat<4, 4, T>& m, const typename Mat<4, 4, T>::rowType& v);

	template<typename T>
	typename Mat<4, 4, T>::rowType operator*(const typename Mat<4, 4, T>::colType& v, const Mat<4, 4, T>& m);

	template<typename T>
	Mat<4, 4, T> operator*(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2);

	template<typename T>
	Mat<4, 4, T> operator/(const Mat<4, 4, T>& m, const T& scalar);

	template<typename T>
	Mat<4, 4, T> operator/(const T& scalar, const Mat<4, 4, T>& m);

	template<typename T>
	typename Mat<4, 4, T>::colType operator/(const Mat<4, 4, T>& m, const typename Mat<4, 4, T>::rowType& v);

	template<typename T>
	typename Mat<4, 4, T>::rowType operator/(const typename Mat<4, 4, T>::colType& v, const Mat<4, 4, T>& m);

	template<typename T>
	Mat<4, 4, T> operator/(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2);

	//Boolean operators
	template<typename T>
	bool operator==(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2);

	template<typename T>
	bool operator!=(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2);
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	struct hash<TRAP::Math::Mat<4, 4, T>>
	{
		constexpr std::size_t operator()(const TRAP::Math::Mat<4, 4, T>& m) const
		{
			std::size_t seed = 0;
			hash<TRAP::Math::Vec<4, T>> hasher;
			TRAP::Utils::HashCombine(seed, hasher(m[0]), hasher(m[1]), hasher(m[2]), hasher(m[3]));
			return seed;
		}
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------//
//Constructors

template<typename T>
constexpr TRAP::Math::Mat<4, 4, T>::Mat(const T& scalar)
	: value{ colType(scalar, 0, 0, 0), colType(0, scalar, 0, 0), colType(0, 0, scalar, 0), colType(0, 0, 0, scalar) }
{}

template<typename T>
constexpr TRAP::Math::Mat<4, 4, T>::Mat(const T& x0, const T& y0, const T& z0, const T& w0,
	                                    const T& x1, const T& y1, const T& z1, const T& w1,
	                                    const T& x2, const T& y2, const T& z2, const T& w2,
	                                    const T& x3, const T& y3, const T& z3, const T& w3)
	: value{ colType(x0, y0, z0, w0), colType(x1, y1, z1, w1), colType(x2, y2, z2, w2), colType(x3, y3, z3, w3) }
{}

template<typename T>
constexpr TRAP::Math::Mat<4, 4, T>::Mat(const colType& v0, const colType& v1, const colType& v2, const colType& v3)
	: value{ colType(v0), colType(v1), colType(v2), colType(v3) }
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion constructors

template<typename T>
template<typename X1, typename Y1, typename Z1, typename W1,
	     typename X2, typename Y2, typename Z2, typename W2,
	     typename X3, typename Y3, typename Z3, typename W3,
	     typename X4, typename Y4, typename Z4, typename W4>
constexpr TRAP::Math::Mat<4, 4, T>::Mat(X1 x1, Y1 y1, Z1 z1, W1 w1,
	                                    X2 x2, Y2 y2, Z2 z2, W2 w2,
	                                    X3 x3, Y3 y3, Z3 z3, W3 w3,
	                                    X4 x4, Y4 y4, Z4 z4, W4 w4)
	: value{ colType(x1, y1, z1, w1), colType(x2, y2, z2, w2), colType(x3, y3, z3, w3), colType(x4, y4, z4, w4) }
{
	static_assert(std::numeric_limits<X1>::is_iec559 || std::numeric_limits<X1>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 1st parameter type invalid.");
	static_assert(std::numeric_limits<Y1>::is_iec559 || std::numeric_limits<Y1>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 2nd parameter type invalid.");
	static_assert(std::numeric_limits<Z1>::is_iec559 || std::numeric_limits<Z1>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 3rd parameter type invalid.");
	static_assert(std::numeric_limits<W1>::is_iec559 || std::numeric_limits<W1>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 4th parameter type invalid.");

	static_assert(std::numeric_limits<X2>::is_iec559 || std::numeric_limits<X2>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 5th parameter type invalid.");
	static_assert(std::numeric_limits<Y2>::is_iec559 || std::numeric_limits<Y2>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 6th parameter type invalid.");
	static_assert(std::numeric_limits<Z2>::is_iec559 || std::numeric_limits<Z2>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 7th parameter type invalid.");
	static_assert(std::numeric_limits<W2>::is_iec559 || std::numeric_limits<W2>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 8th parameter type invalid.");

	static_assert(std::numeric_limits<X3>::is_iec559 || std::numeric_limits<X3>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 9th parameter type invalid.");
	static_assert(std::numeric_limits<Y3>::is_iec559 || std::numeric_limits<Y3>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 10th parameter type invalid.");
	static_assert(std::numeric_limits<Z3>::is_iec559 || std::numeric_limits<Z3>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 11th parameter type invalid.");
	static_assert(std::numeric_limits<W3>::is_iec559 || std::numeric_limits<W3>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 12th parameter type invalid.");

	static_assert(std::numeric_limits<X4>::is_iec559 || std::numeric_limits<X4>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 13th parameter type invalid.");
	static_assert(std::numeric_limits<Y4>::is_iec559 || std::numeric_limits<Y4>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 14th parameter type invalid.");
	static_assert(std::numeric_limits<Z4>::is_iec559 || std::numeric_limits<Z4>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 15th parameter type invalid.");
	static_assert(std::numeric_limits<W4>::is_iec559 || std::numeric_limits<W4>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 16th parameter type invalid.");
}

template<typename T>
template<typename V1, typename V2, typename V3, typename V4>
constexpr TRAP::Math::Mat<4, 4, T>::Mat(const Vec<4, V1>& v1, const Vec<4, V2>& v2, const Vec<4, V3>& v3,
                                        const Vec<4, V4>& v4)
	: value{ colType(v1), colType(v2), colType(v3), colType(v4) }
{
	static_assert(std::numeric_limits<V1>::is_iec559 || std::numeric_limits<V1>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 1st parameter type invalid.");
	static_assert(std::numeric_limits<V2>::is_iec559 || std::numeric_limits<V2>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 2nd parameter type invalid.");
	static_assert(std::numeric_limits<V3>::is_iec559 || std::numeric_limits<V3>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 3rd parameter type invalid.");
	static_assert(std::numeric_limits<V4>::is_iec559 || std::numeric_limits<V4>::is_integer,
	              "*Mat4x4 constructor only takes float and integer types, 4th parameter type invalid.");
}

//-------------------------------------------------------------------------------------------------------------------//
//Matrix conversions
template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<4, 4, T>::Mat(const Mat<4, 4, U>& m)
	: value{ colType(m[0]), colType(m[1]), colType(m[2]), colType(m[3]) }
{}

template<typename T>
constexpr TRAP::Math::Mat<4, 4, T>::Mat(const Mat<3, 3, T>& x)
	: value{ colType(x[0], 0), colType(x[1], 0), colType(x[2], 0), colType(0, 0, 0, 1) }
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr int TRAP::Math::Mat<4, 4, T>::Length()
{
	return 4 * 4;
}

//-------------------------------------------------------------------------------------------------------------------//
//Accesses

template<typename T>
typename TRAP::Math::Mat<4, 4, T>::colType& TRAP::Math::Mat<4, 4, T>::operator[](const int32_t i)
{
	TRAP_ASSERT(i < this->Length());

	return this->value[i];
}

template<typename T>
constexpr const typename TRAP::Math::Mat<4, 4, T>::colType& TRAP::Math::Mat<4, 4, T>::operator[](const int32_t i) const
{
	TRAP_ASSERT(i < this->Length());

	return this->value[i];
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary updatable operators

template<typename T>
template<typename U>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator=(const Mat<4, 4, U>& m)
{
	this->value[0] = m[0];
	this->value[1] = m[1];
	this->value[2] = m[2];
	this->value[3] = m[3];

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator+=(U s)
{
	this->value[0] += s;
	this->value[1] += s;
	this->value[2] += s;
	this->value[3] += s;

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator+=(const Mat<4, 4, U>& m)
{
	this->value[0] += m[0];
	this->value[1] += m[1];
	this->value[2] += m[2];
	this->value[3] += m[3];

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator-=(U s)
{
	this->value[0] -= s;
	this->value[1] -= s;
	this->value[2] -= s;
	this->value[3] -= s;

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator-=(const Mat<4, 4, U>& m)
{
	this->value[0] -= m[0];
	this->value[1] -= m[1];
	this->value[2] -= m[2];
	this->value[3] -= m[3];

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator*=(U s)
{
	this->value[0] *= s;
	this->value[1] *= s;
	this->value[2] *= s;
	this->value[3] *= s;

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator*=(const Mat<4, 4, U>& m)
{
	return (*this = *this * m);
}

template<typename T>
template<typename U>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator/=(U s)
{
	this->value[0] /= s;
	this->value[1] /= s;
	this->value[2] /= s;
	this->value[3] /= s;

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator/=(const Mat<4, 4, U>& m)
{
	return *this *= Inverse(m);
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator++()
{
	++this->value[0];
	++this->value[1];
	++this->value[2];
	++this->value[3];

	return *this;
}

template<typename T>
TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator--()
{
	--this->value[0];
	--this->value[1];
	--this->value[2];
	--this->value[3];

	return *this;
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Mat<4, 4, T>::operator++(int)
{
	Mat<4, 4, T> result(*this);
	++*this;

	return result;
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Mat<4, 4, T>::operator--(int)
{
	Mat<4, 4, T> result(*this);
	--*this;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator+(const Mat<4, 4, T>& m)
{
	return m;
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator-(const Mat<4, 4, T>& m)
{
	return Mat<4, 4, T>(-m[0], -m[1], -m[2], -m[3]);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator+(const Mat<4, 4, T>& m, const T& scalar)
{
	return Mat<4, 4, T>(m[0] + scalar, m[1] + scalar, m[2] + scalar, m[3] + scalar);
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator+(const T& scalar, const Mat<4, 4, T>& m)
{
	return Mat<4, 4, T>(m[0] + scalar, m[1] + scalar, m[2] + scalar, m[3] + scalar);
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator+(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2)
{
	return Mat<4, 4, T>(m1[0] + m2[0], m1[1] + m2[1], m1[2] + m2[2], m1[3] + m2[2]);
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator-(const Mat<4, 4, T>& m, const T& scalar)
{
	return Mat<4, 4, T>(m[0] - scalar, m[1] - scalar, m[2] - scalar, m[3] - scalar);
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator-(const T& scalar, const Mat<4, 4, T>& m)
{
	return Mat<4, 4, T>(scalar - m[0], scalar - m[1], scalar - m[2], scalar - m[3]);
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator-(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2)
{
	return Mat<4, 4, T>(m1[0] - m2[0], m1[1] - m2[1], m1[2] - m2[2], m1[3] - m2[3]);
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator*(const Mat<4, 4, T>& m, const T& scalar)
{
	return Mat<4, 4, T>(m[0] * scalar, m[1] * scalar, m[2] * scalar, m[3] * scalar);
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator*(const T& scalar, const Mat<4, 4, T>& m)
{
	return Mat<4, 4, T>(m[0] * scalar, m[1] * scalar, m[2] * scalar, m[3] * scalar);
}

template<typename T>
typename TRAP::Math::Mat<4, 4, T>::colType TRAP::Math::operator*(const Mat<4, 4, T>& m,
                                                                 const typename Mat<4, 4, T>::rowType& v)
{
	typename Mat<4, 4, T>::colType const mov0(v[0]);
	typename Mat<4, 4, T>::colType const mov1(v[1]);
	typename Mat<4, 4, T>::colType const mul0 = m[0] * mov0;
	typename Mat<4, 4, T>::colType const mul1 = m[1] * mov1;
	typename Mat<4, 4, T>::colType const add0 = mul0 + mul1;
	typename Mat<4, 4, T>::colType const mov2(v[2]);
	typename Mat<4, 4, T>::colType const mov3(v[3]);
	typename Mat<4, 4, T>::colType const mul2 = m[2] * mov2;
	typename Mat<4, 4, T>::colType const mul3 = m[3] * mov3;
	typename Mat<4, 4, T>::colType const add1 = mul2 + mul3;
	typename Mat<4, 4, T>::colType const add2 = add0 + add1;

	return add2;
}

template<typename T>
typename TRAP::Math::Mat<4, 4, T>::rowType TRAP::Math::operator*(const typename Mat<4, 4, T>::colType& v,
                                                                 const Mat<4, 4, T>& m)
{
	return typename Mat<4, 4, T>::rowType(m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3],
		                                  m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3],
		                                  m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3],
		                                  m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3]);
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator*(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2)
{
	typename Mat<4, 4, T>::colType const srcA0 = m1[0];
	typename Mat<4, 4, T>::colType const srcA1 = m1[1];
	typename Mat<4, 4, T>::colType const srcA2 = m1[2];
	typename Mat<4, 4, T>::colType const srcA3 = m1[3];

	typename Mat<4, 4, T>::colType const srcB0 = m2[0];
	typename Mat<4, 4, T>::colType const srcB1 = m2[1];
	typename Mat<4, 4, T>::colType const srcB2 = m2[2];
	typename Mat<4, 4, T>::colType const srcB3 = m2[3];

	Mat<4, 4, T> result;
	result[0] = srcA0 * srcB0[0] + srcA1 * srcB0[1] + srcA2 * srcB0[2] + srcA3 * srcB0[3];
	result[1] = srcA0 * srcB1[0] + srcA1 * srcB1[1] + srcA2 * srcB1[2] + srcA3 * srcB1[3];
	result[2] = srcA0 * srcB2[0] + srcA1 * srcB2[1] + srcA2 * srcB2[2] + srcA3 * srcB2[3];
	result[3] = srcA0 * srcB3[0] + srcA1 * srcB3[1] + srcA2 * srcB3[2] + srcA3 * srcB3[3];

	return result;
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator/(const Mat<4, 4, T>& m, const T& scalar)
{
	return Mat<4, 4, T>(m[0] / scalar, m[1] / scalar, m[2] / scalar, m[3] / scalar);
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator/(const T& scalar, const Mat<4, 4, T>& m)
{
	return Mat<4, 4, T>(scalar / m[0], scalar / m[1], scalar / m[2], scalar / m[3]);
}

template<typename T>
typename TRAP::Math::Mat<4, 4, T>::colType TRAP::Math::operator/(const Mat<4, 4, T>& m,
                                                                 const typename Mat<4, 4, T>::rowType& v)
{
	return Inverse(m) * v;
}

template<typename T>
typename TRAP::Math::Mat<4, 4, T>::rowType TRAP::Math::operator/(const typename Mat<4, 4, T>::colType& v,
                                                                 const Mat<4, 4, T>& m)
{
	return v * Inverse(m);
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::operator/(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2)
{
	Mat<4, 4, T> m1Copy(m1);

	return m1Copy /= m2;
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
bool TRAP::Math::operator==(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2)
{
	return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]);
}

template<typename T>
bool TRAP::Math::operator!=(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2)
{
	return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]);
}

#endif /*TRAP_MAT4_H*/
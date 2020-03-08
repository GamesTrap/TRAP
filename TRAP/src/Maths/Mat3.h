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

#ifndef _TRAP_MAT3_H_
#define _TRAP_MAT3_H_

#include "Vec3.h"

namespace TRAP::Math
{
	template<typename T>
	struct Mat<3, 3, T>
	{
		constexpr Mat(Mat&&) = default;
		~Mat() = default;
		constexpr Mat<3, 3, T>& operator=(Mat&&) = default;
		constexpr Mat<3, 3, T>& operator=(const Mat&) = default;

		typedef Vec<3, T> colType;
		typedef Vec<3, T> rowType;
		typedef Mat<3, 3, T> type;
		typedef Mat<3, 3, T> transposeType;
		typedef T valueType;

	private:
		std::array<colType, 3> value{};

	public:
		//Constructors
		constexpr Mat() = default;
		constexpr Mat(const Mat<3, 3, T> & m) = default;

		explicit constexpr Mat(T scalar);
		constexpr Mat(T x0, T y0, T z0,
			T x1, T y1, T z1,
			T x2, T y2, T z2);
		constexpr Mat(const colType & v0, const colType & v1, const colType & v2);

		//Conversions
		template<typename X1, typename Y1, typename Z1,
			typename X2, typename Y2, typename Z2,
			typename X3, typename Y3, typename Z3>
			constexpr Mat(X1 x1, Y1 y1, Z1 z1,
				X2 x2, Y2 y2, Z2 z2,
				X3 x3, Y3 y3, Z3 z3);

		template<typename V1, typename V2, typename V3>
		constexpr Mat(const Vec<3, V1> & v1, const Vec<3, V2> & v2, const Vec<3, V3> & v3);

		//Matrix conversions
		template<typename U>
		explicit constexpr Mat(const Mat<3, 3, U> & m);

		explicit constexpr Mat(const Mat<4, 4, T> & x);

		static constexpr int Length();

		//Accesses
		colType& operator[](int i);
		constexpr const colType& operator[](int i) const;

		//Unary arithmetic operators
		template<typename U>
		Mat<3, 3, T>& operator=(const Mat<3, 3, U>& m);
		template<typename U>
		Mat<3, 3, T>& operator+=(U s);
		template<typename U>
		Mat<3, 3, T>& operator+=(const Mat<3, 3, U>& m);
		template<typename U>
		Mat<3, 3, T>& operator-=(U s);
		template<typename U>
		Mat<3, 3, T>& operator-=(const Mat<3, 3, U>& m);
		template<typename U>
		Mat<3, 3, T>& operator*=(U s);
		template<typename U>
		Mat<3, 3, T>& operator*=(const Mat<3, 3, U>& m);
		template<typename U>
		Mat<3, 3, T>& operator/=(U s);
		template<typename U>
		Mat<3, 3, T>& operator/=(const Mat<3, 3, U>& m);

		//Increment and decrement operators
		Mat<3, 3, T>& operator++();
		Mat<3, 3, T>& operator--();
		Mat<3, 3, T> operator++(int);
		Mat<3, 3, T> operator--(int);
	};
	
	//Unary operators
	template<typename T>
	Mat<3, 3, T> operator+(const Mat<3, 3, T>& m);

	template<typename T>
	Mat<3, 3, T> operator-(const Mat<3, 3, T>& m);

	//Binary operators
	template<typename T>
	Mat<3, 3, T> operator+(const Mat<3, 3, T>& m, T scalar);

	template<typename T>
	Mat<3, 3, T> operator+(T scalar, const Mat<3, 3, T>& m);

	template<typename T>
	Mat<3, 3, T> operator+(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2);

	template<typename T>
	Mat<3, 3, T> operator-(const Mat<3, 3, T>& m, T scalar);

	template<typename T>
	Mat<3, 3, T> operator-(T scalar, const Mat<3, 3, T>& m);

	template<typename T>
	Mat<3, 3, T> operator-(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2);

	template<typename T>
	Mat<3, 3, T> operator*(const Mat<3, 3, T>& m, T scalar);

	template<typename T>
	Mat<3, 3, T> operator*(T scalar, const Mat<3, 3, T>& m);

	template<typename T>
	typename Mat<3, 3, T>::colType operator*(const Mat<3, 3, T>& m, const typename Mat<3, 3, T>::rowType& v);

	template<typename T>
	typename Mat<3, 3, T>::rowType operator*(const typename Mat<3, 3, T>::colType& v, const Mat<3, 3, T>& m);

	template<typename T>
	Mat<3, 3, T> operator*(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2);

	template<typename T>
	Mat<3, 3, T> operator/(const Mat<3, 3, T>& m, T scalar);

	template<typename T>
	Mat<3, 3, T> operator/(T scalar, const Mat<3, 3, T>& m);

	template<typename T>
	typename Mat<3, 3, T>::colType operator/(const Mat<3, 3, T>& m, const typename Mat<3, 3, T>::rowType& v);

	template<typename T>
	typename Mat<3, 3, T>::rowType operator/(const typename Mat<3, 3, T>::colType& v, const Mat<3, 3, T>& m);

	template<typename T>
	Mat<3, 3, T> operator/(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2);

	//Boolean operators
	template<typename T>
	constexpr bool operator==(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2);

	template<typename T>
	bool operator!=(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2);	
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//Constructors

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(T scalar)
	: value{ colType(scalar, 0, 0), colType(0, scalar, 0), colType(0, 0, scalar) }
{}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(T x0, T y0, T z0,
	T x1, T y1, T z1,
	T x2, T y2, T z2)
	: value{ colType(x0, y0, z0), colType(x1, y1, z1), colType(x2, y2, z2) }
{}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const colType& v0, const colType& v1, const colType& v2)
	: value{ colType(v0), colType(v1), colType(v2) }
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion constructors

template<typename T>
template<typename X1, typename Y1, typename Z1,
	typename X2, typename Y2, typename Z2,
	typename X3, typename Y3, typename Z3>
	constexpr TRAP::Math::Mat<3, 3, T>::Mat(X1 x1, Y1 y1, Z1 z1,
		X2 x2, Y2 y2, Z2 z2,
		X3 x3, Y3 y3, Z3 z3)
	: value{ colType(x1, y1, z1), colType(x2, y2, z2), colType(x3, y3, z3) }
{}

template<typename T>
template<typename V1, typename V2, typename V3>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const Vec<3, V1>& v1, const Vec<3, V2>& v2, const Vec<3, V3>& v3)
	: value{ colType(v1), colType(v2), colType(v3) }
{}

//-------------------------------------------------------------------------------------------------------------------//
//Matrix conversions
template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const Mat<3, 3, U>& m)
	: value{ colType(m[0]), colType(m[1]), colType(m[2]) }
{}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const Mat<4, 4, T>& x)
	: value{ colType(x[0]), colType(x[1]), colType(x[2]) }
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr int TRAP::Math::Mat<3, 3, T>::Length()
{
	return 3 * 3;
}

//-------------------------------------------------------------------------------------------------------------------//
//Accesses

template<typename T>
typename TRAP::Math::Mat<3, 3, T>::colType& TRAP::Math::Mat<3, 3, T>::operator[](int i)
{
	assert(i < this->Length());

	return this->value[i];
}

template<typename T>
constexpr const typename TRAP::Math::Mat<3, 3, T>::colType& TRAP::Math::Mat<3, 3, T>::operator[](int i) const
{
	assert(i < this->Length());

	return this->value[i];
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary updatable operators

template<typename T>
template<typename U>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator=(const Mat<3, 3, U>& m)
{
	this->value[0] = m[0];
	this->value[1] = m[1];
	this->value[2] = m[2];

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator+=(U s)
{
	this->value[0] += s;
	this->value[1] += s;
	this->value[2] += s;

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator+=(const Mat<3, 3, U>& m)
{
	this->value[0] += m[0];
	this->value[1] += m[1];
	this->value[2] += m[2];

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator-=(U s)
{
	this->value[0] -= s;
	this->value[1] -= s;
	this->value[2] -= s;

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator-=(const Mat<3, 3, U>& m)
{
	this->value[0] -= m[0];
	this->value[1] -= m[1];
	this->value[2] -= m[2];

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator*=(U s)
{
	this->value[0] *= s;
	this->value[1] *= s;
	this->value[2] *= s;

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator*=(const Mat<3, 3, U>& m)
{
	return (*this = *this * m);
}

template<typename T>
template<typename U>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator/=(U s)
{
	this->value[0] /= s;
	this->value[1] /= s;
	this->value[2] /= s;

	return *this;
}

template<typename T>
template<typename U>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator/=(const Mat<3, 3, U>& m)
{
	return *this *= Inverse(m);
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator++()
{
	++this->value[0];
	++this->value[1];
	++this->value[2];

	return *this;
}

template<typename T>
TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator--()
{
	--this->value[0];
	--this->value[1];
	--this->value[2];

	return *this;
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::Mat<3, 3, T>::operator++(int)
{
	Mat<3, 3, T> result(*this);
	++* this;

	return result;
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::Mat<3, 3, T>::operator--(int)
{
	Mat<3, 3, T> result(*this);
	--* this;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const Mat<3, 3, T>& m)
{
	return m;
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const Mat<3, 3, T>& m)
{
	return Mat<3, 3, T>(-m[0], -m[1], -m[2]);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const Mat<3, 3, T>& m, T scalar)
{
	return Mat<3, 3, T>(m[0] + scalar, m[1] + scalar, m[2] + scalar);
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(T scalar, const Mat<3, 3, T>& m)
{
	return Mat<3, 3, T>(m[0] + scalar, m[1] + scalar, m[2] + scalar);
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2)
{
	return Mat<3, 3, T>(m1[0] + m2[0], m1[1] + m2[1], m1[2] + m2[2]);
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const Mat<3, 3, T>& m, T scalar)
{
	return Mat<3, 3, T>(m[0] - scalar, m[1] - scalar, m[2] - scalar);
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(T scalar, const Mat<3, 3, T>& m)
{
	return Mat<3, 3, T>(scalar - m[0], scalar - m[1], scalar - m[2]);
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2)
{
	return Mat<3, 3, T>(m1[0] - m2[0], m1[1] - m2[1], m1[2] - m2[2]);
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator*(const Mat<3, 3, T>& m, T scalar)
{
	return Mat<3, 3, T>(m[0] * scalar, m[1] * scalar, m[2] * scalar);
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator*(T scalar, const Mat<3, 3, T>& m)
{
	return Mat<3, 3, T>(m[0] * scalar, m[1] * scalar, m[2] * scalar);
}

template<typename T>
typename TRAP::Math::Mat<3, 3, T>::colType TRAP::Math::operator*(const Mat<3, 3, T>& m, const typename Mat<3, 3, T>::rowType& v)
{
	return typename Mat<3, 3, T>::colType(m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
		m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
		m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
}

template<typename T>
typename TRAP::Math::Mat<3, 3, T>::rowType TRAP::Math::operator*(const typename Mat<3, 3, T>::colType& v, const Mat<3, 3, T>& m)
{
	return typename Mat<3, 3, T>::rowType(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
		m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
		m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator*(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2)
{
	T const srcA00 = m1[0][0];
	T const srcA01 = m1[0][1];
	T const srcA02 = m1[0][2];
	T const srcA10 = m1[1][0];
	T const srcA11 = m1[1][1];
	T const srcA12 = m1[1][2];
	T const srcA20 = m1[2][0];
	T const srcA21 = m1[2][1];
	T const srcA22 = m1[2][2];

	T const srcB00 = m2[0][0];
	T const srcB01 = m2[0][1];
	T const srcB02 = m2[0][2];
	T const srcB10 = m2[1][0];
	T const srcB11 = m2[1][1];
	T const srcB12 = m2[1][2];
	T const srcB20 = m2[2][0];
	T const srcB21 = m2[2][1];
	T const srcB22 = m2[2][2];

	Mat<3, 3, T> result;
	result[0][0] = srcA00 * srcB00 + srcA10 * srcB01 + srcA20 * srcB02;
	result[0][1] = srcA01 * srcB00 + srcA11 * srcB01 + srcA21 * srcB02;
	result[0][2] = srcA02 * srcB00 + srcA12 * srcB01 + srcA22 * srcB02;
	result[1][0] = srcA00 * srcB10 + srcA10 * srcB11 + srcA20 * srcB12;
	result[1][1] = srcA01 * srcB10 + srcA11 * srcB11 + srcA21 * srcB12;
	result[1][2] = srcA02 * srcB10 + srcA12 * srcB11 + srcA22 * srcB12;
	result[2][0] = srcA00 * srcB20 + srcA10 * srcB21 + srcA20 * srcB22;
	result[2][1] = srcA01 * srcB20 + srcA11 * srcB21 + srcA21 * srcB22;
	result[2][2] = srcA02 * srcB20 + srcA12 * srcB21 + srcA22 * srcB22;

	return result;
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator/(const Mat<3, 3, T>& m, T scalar)
{
	return Mat<3, 3, T>(m[0] / scalar, m[1] / scalar, m[2] / scalar);
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator/(T scalar, const Mat<3, 3, T>& m)
{
	return Mat<3, 3, T>(scalar / m[0], scalar / m[1], scalar / m[2]);
}

template<typename T>
typename TRAP::Math::Mat<3, 3, T>::colType TRAP::Math::operator/(const Mat<3, 3, T>& m, const typename Mat<3, 3, T>::rowType& v)
{
	return Inverse(m) * v;
}

template<typename T>
typename TRAP::Math::Mat<3, 3, T>::rowType TRAP::Math::operator/(const typename Mat<3, 3, T>::colType& v, const Mat<3, 3, T>& m)
{
	return v * Inverse(m);
}

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::operator/(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2)
{
	Mat<3, 3, T> m1Copy(m1);

	return m1Copy /= m2;
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
constexpr bool TRAP::Math::operator==(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2)
{
	return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]);
}

template<typename T>
bool TRAP::Math::operator!=(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2)
{
	return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]);
}

#endif /*_TRAP_MAT3_H_*/
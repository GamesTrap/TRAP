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

#ifndef TRAP_MAT3_H
#define TRAP_MAT3_H

#include <array>

#include "Vec3.h"
#include "Core/PlatformDetection.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"

namespace TRAP::Math
{
	/// <summary>
	/// Struct for a 3x3 matrix.
	/// </summary>
	template<typename T>
	struct Mat<3, 3, T>
	{
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Mat(Mat&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~Mat() = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr Mat<3, 3, T>& operator=(Mat&&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr Mat<3, 3, T>& operator=(const Mat&) noexcept = default;

		using colType = Vec<3, T>;
		using rowType = Vec<3, T>;
		using type = Mat<3, 3, T>;
		using transposeType = Mat<3, 3, T>;
		using valueType = T;

	private:
		std::array<colType, 3> value{};

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		constexpr Mat() noexcept = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Mat(const Mat<3, 3, T> & m) noexcept = default;

		/// <summary>
		/// Scalar constructor.
		/// </summary>
		explicit constexpr Mat(T scalar) noexcept;
		/// <summary>
		/// Value constructor.
		/// </summary>
		constexpr Mat(T x0, T y0, T z0,
			          T x1, T y1, T z1,
			          T x2, T y2, T z2) noexcept;
		/// <summary>
		/// Column constructor.
		/// </summary>
		constexpr Mat(const colType & v0, const colType & v1, const colType & v2) noexcept;

		/// <summary>
		/// Value conversion constructor.
		/// </summary>
		template<typename X1, typename Y1, typename Z1,
			     typename X2, typename Y2, typename Z2,
			     typename X3, typename Y3, typename Z3>
		constexpr Mat(X1 x1, Y1 y1, Z1 z1,
			          X2 x2, Y2 y2, Z2 z2,
			          X3 x3, Y3 y3, Z3 z3) noexcept;

		/// <summary>
		/// Column conversion constructor.
		/// </summary>
		template<typename V1, typename V2, typename V3>
		constexpr Mat(const Vec<3, V1> & v1, const Vec<3, V2> & v2, const Vec<3, V3> & v3) noexcept;

		/// <summary>
		/// Copy conversion constructor.
		/// </summary>
		template<typename U>
		constexpr Mat(const Mat<3, 3, U> & m) noexcept;

		/// <summary>
		/// Copy conversion constructor.
		/// </summary>
		constexpr Mat(const Mat<4, 4, T> & x) noexcept;

		/// <summary>
		/// Retrieve the length of the matrix.
		/// </summary>
		/// <returns>Length.</returns>
		[[nodiscard]] static constexpr int32_t Length() noexcept;

		/// <summary>
		/// Retrieve a column of the matrix.
		/// </summary>
		/// <param name="i">Column to retrieve.</param>
		[[nodiscard]] constexpr colType& operator[](int32_t i);
		/// <summary>
		/// Retrieve a column of the matrix.
		/// </summary>
		/// <param name="i">Column to retrieve.</param>
		[[nodiscard]] constexpr const colType& operator[](int32_t i) const;

		//Unary arithmetic operators
		template<typename U>
		constexpr Mat<3, 3, T>& operator=(const Mat<3, 3, U>& m) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator+=(U s) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator+=(const Mat<3, 3, U>& m) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator-=(U s) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator-=(const Mat<3, 3, U>& m) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator*=(U s) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator*=(const Mat<3, 3, U>& m) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator/=(U s) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator/=(const Mat<3, 3, U>& m) ;

		//Increment and decrement operators
		constexpr Mat<3, 3, T>& operator++() noexcept;
		constexpr Mat<3, 3, T>& operator--() noexcept;
		constexpr Mat<3, 3, T> operator++(int32_t) noexcept;
		constexpr Mat<3, 3, T> operator--(int32_t) noexcept;

		[[nodiscard]] std::string ToString() const;
	};

	//Unary operators
	template<typename T>
	constexpr Mat<3, 3, T> operator+(const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator-(const Mat<3, 3, T>& m) noexcept;

	//Binary operators
	template<typename T>
	constexpr Mat<3, 3, T> operator+(const Mat<3, 3, T>& m, T scalar) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator+(T scalar, const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator+(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator-(const Mat<3, 3, T>& m, T scalar) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator-(T scalar, const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator-(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator*(const Mat<3, 3, T>& m, T scalar) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator*(T scalar, const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	constexpr typename Mat<3, 3, T>::colType operator*(const Mat<3, 3, T>& m, const typename Mat<3, 3, T>::rowType& v) noexcept;

	template<typename T>
	constexpr typename Mat<3, 3, T>::rowType operator*(const typename Mat<3, 3, T>::colType& v, const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator*(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator/(const Mat<3, 3, T>& m, T scalar) noexcept;

	template<typename T>
	constexpr Mat<3, 3, T> operator/(T scalar, const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	constexpr typename Mat<3, 3, T>::colType operator/(const Mat<3, 3, T>& m, const typename Mat<3, 3, T>::rowType& v);

	template<typename T>
	constexpr typename Mat<3, 3, T>::rowType operator/(const typename Mat<3, 3, T>::colType& v, const Mat<3, 3, T>& m);

	template<typename T>
	constexpr Mat<3, 3, T> operator/(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;

	//Boolean operators
	template<typename T>
	constexpr bool operator==(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;

	template<typename T>
	constexpr bool operator!=(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	struct hash<TRAP::Math::Mat<3, 3, T>>
	{
		constexpr std::size_t operator()(const TRAP::Math::Mat<3, 3, T>& m) const noexcept
		{
			std::size_t seed = 0;
			hash<TRAP::Math::Vec<3, T>> hasher;
			TRAP::Utils::HashCombine(seed, hasher(m[0]), hasher(m[1]), hasher(m[2]));
			return seed;
		}
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//Constructors

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const T scalar) noexcept
	: value{ colType(scalar, 0, 0), colType(0, scalar, 0), colType(0, 0, scalar) }
{}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const T x0, const T y0, const T z0,
	                                    const T x1, const T y1, const T z1,
	                                    const T x2, const T y2, const T z2) noexcept
	: value{ colType(x0, y0, z0), colType(x1, y1, z1), colType(x2, y2, z2) }
{}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const colType& v0, const colType& v1, const colType& v2) noexcept
	: value{ colType(v0), colType(v1), colType(v2) }
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion constructors

template<typename T>
template<typename X1, typename Y1, typename Z1,
	     typename X2, typename Y2, typename Z2,
	     typename X3, typename Y3, typename Z3>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const X1 x1, const Y1 y1, const Z1 z1,
	                                    const X2 x2, const Y2 y2, const Z2 z2,
	                                    const X3 x3, const Y3 y3, const Z3 z3) noexcept
	: value{ colType(x1, y1, z1), colType(x2, y2, z2), colType(x3, y3, z3) }
{}

template<typename T>
template<typename V1, typename V2, typename V3>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const Vec<3, V1>& v1, const Vec<3, V2>& v2, const Vec<3, V3>& v3) noexcept
	: value{ colType(v1), colType(v2), colType(v3) }
{}

//-------------------------------------------------------------------------------------------------------------------//
//Matrix conversions
template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const Mat<3, 3, U>& m) noexcept
	: value{ colType(m[0]), colType(m[1]), colType(m[2]) }
{}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const Mat<4, 4, T>& x) noexcept
	: value{ colType(x[0]), colType(x[1]), colType(x[2]) }
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
[[nodiscard]] constexpr int TRAP::Math::Mat<3, 3, T>::Length() noexcept
{
	return 3;
}

//-------------------------------------------------------------------------------------------------------------------//
//Accesses

template<typename T>
[[nodiscard]] constexpr typename TRAP::Math::Mat<3, 3, T>::colType& TRAP::Math::Mat<3, 3, T>::operator[](const int32_t i)
{
	TRAP_ASSERT(i < this->Length(), "Math::Mat<3, 3, T>::operator[]: index out of range");

	return this->value[i];
}

template<typename T>
[[nodiscard]] constexpr const typename TRAP::Math::Mat<3, 3, T>::colType& TRAP::Math::Mat<3, 3, T>::operator[](const int32_t i) const
{
	TRAP_ASSERT(i < this->Length(), "Math::Mat<3, 3, T>::operator[]: index out of range");

	return this->value[i];
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary updatable operators

template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator=(const Mat<3, 3, U>& m) noexcept
{
	this->value[0] = m[0];
	this->value[1] = m[1];
	this->value[2] = m[2];

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator+=(const U s) noexcept
{
	this->value[0] += s;
	this->value[1] += s;
	this->value[2] += s;

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator+=(const Mat<3, 3, U>& m) noexcept
{
	this->value[0] += m[0];
	this->value[1] += m[1];
	this->value[2] += m[2];

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator-=(const U s) noexcept
{
	this->value[0] -= s;
	this->value[1] -= s;
	this->value[2] -= s;

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator-=(const Mat<3, 3, U>& m) noexcept
{
	this->value[0] -= m[0];
	this->value[1] -= m[1];
	this->value[2] -= m[2];

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator*=(const U s) noexcept
{
	this->value[0] *= s;
	this->value[1] *= s;
	this->value[2] *= s;

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator*=(const Mat<3, 3, U>& m) noexcept
{
	return (*this = *this * m);
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator/=(const U s) noexcept
{
	this->value[0] /= s;
	this->value[1] /= s;
	this->value[2] /= s;

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator/=(const Mat<3, 3, U>& m)
{
	return *this *= Inverse(m);
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator++() noexcept
{
	++this->value[0];
	++this->value[1];
	++this->value[2];

	return *this;
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator--() noexcept
{
	--this->value[0];
	--this->value[1];
	--this->value[2];

	return *this;
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::Mat<3, 3, T>::operator++(int) noexcept
{
	Mat<3, 3, T> result(*this);
	++*this;

	return result;
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::Mat<3, 3, T>::operator--(int) noexcept
{
	Mat<3, 3, T> result(*this);
	--*this;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] std::string TRAP::Math::Mat<3, 3, T>::ToString() const
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	std::string postfix = "";
	if constexpr(std::is_same_v<T, float>)
		postfix = "f";
	else if constexpr(std::is_same_v<T, double>)
		postfix = "d";
	else
		return "Unknown type";

	return "Mat3" + postfix + "((" + std::to_string(value[0][0]) + ", " + std::to_string(value[0][1]) + ", " +
	       std::to_string(value[0][2]) + "), (" + std::to_string(value[1][0]) + ", " + std::to_string(value[1][1]) +
		   ", " + std::to_string(value[1][2]) + "), (" + std::to_string(value[2][0]) + ", " +
		   std::to_string(value[2][1]) + ", " + std::to_string(value[2][2]) + "))";
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const Mat<3, 3, T>& m) noexcept
{
	return m;
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const Mat<3, 3, T>& m) noexcept
{
	return Mat<3, 3, T>(-m[0], -m[1], -m[2]);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const Mat<3, 3, T>& m, const T scalar) noexcept
{
	return Mat<3, 3, T>(m[0] + scalar, m[1] + scalar, m[2] + scalar);
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const T scalar, const Mat<3, 3, T>& m) noexcept
{
	return Mat<3, 3, T>(m[0] + scalar, m[1] + scalar, m[2] + scalar);
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	return Mat<3, 3, T>(m1[0] + m2[0], m1[1] + m2[1], m1[2] + m2[2]);
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const Mat<3, 3, T>& m, const T scalar) noexcept
{
	return Mat<3, 3, T>(m[0] - scalar, m[1] - scalar, m[2] - scalar);
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const T scalar, const Mat<3, 3, T>& m) noexcept
{
	return Mat<3, 3, T>(scalar - m[0], scalar - m[1], scalar - m[2]);
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	return Mat<3, 3, T>(m1[0] - m2[0], m1[1] - m2[1], m1[2] - m2[2]);
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator*(const Mat<3, 3, T>& m, const T scalar) noexcept
{
	return Mat<3, 3, T>(m[0] * scalar, m[1] * scalar, m[2] * scalar);
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator*(const T scalar, const Mat<3, 3, T>& m) noexcept
{
	return Mat<3, 3, T>(m[0] * scalar, m[1] * scalar, m[2] * scalar);
}

template<typename T>
constexpr typename TRAP::Math::Mat<3, 3, T>::colType TRAP::Math::operator*(const Mat<3, 3, T>& m,
                                                                           const typename Mat<3, 3, T>::rowType& v) noexcept
{
	return typename Mat<3, 3, T>::colType(m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
		                                  m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
		                                  m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
}

template<typename T>
constexpr typename TRAP::Math::Mat<3, 3, T>::rowType TRAP::Math::operator*(const typename Mat<3, 3, T>::colType& v,
                                                                           const Mat<3, 3, T>& m) noexcept
{
	return typename Mat<3, 3, T>::rowType(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
		                                  m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
		                                  m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator*(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	const T srcA00 = m1[0][0];
	const T srcA01 = m1[0][1];
	const T srcA02 = m1[0][2];
	const T srcA10 = m1[1][0];
	const T srcA11 = m1[1][1];
	const T srcA12 = m1[1][2];
	const T srcA20 = m1[2][0];
	const T srcA21 = m1[2][1];
	const T srcA22 = m1[2][2];

	const T srcB00 = m2[0][0];
	const T srcB01 = m2[0][1];
	const T srcB02 = m2[0][2];
	const T srcB10 = m2[1][0];
	const T srcB11 = m2[1][1];
	const T srcB12 = m2[1][2];
	const T srcB20 = m2[2][0];
	const T srcB21 = m2[2][1];
	const T srcB22 = m2[2][2];

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
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator/(const Mat<3, 3, T>& m, const T scalar) noexcept
{
	return Mat<3, 3, T>(m[0] / scalar, m[1] / scalar, m[2] / scalar);
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator/(const T scalar, const Mat<3, 3, T>& m) noexcept
{
	return Mat<3, 3, T>(scalar / m[0], scalar / m[1], scalar / m[2]);
}

template<typename T>
constexpr typename TRAP::Math::Mat<3, 3, T>::colType TRAP::Math::operator/(const Mat<3, 3, T>& m,
                                                                           const typename Mat<3, 3, T>::rowType& v)
{
	return Inverse(m) * v;
}

template<typename T>
constexpr typename TRAP::Math::Mat<3, 3, T>::rowType TRAP::Math::operator/(const typename Mat<3, 3, T>::colType& v,
                                                                           const Mat<3, 3, T>& m)
{
	return v * Inverse(m);
}

template<typename T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator/(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	Mat<3, 3, T> m1Copy(m1);

	return m1Copy /= m2;
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
constexpr bool TRAP::Math::operator==(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]);
}

template<typename T>
constexpr bool TRAP::Math::operator!=(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]);
}

#endif /*TRAP_MAT3_H*/

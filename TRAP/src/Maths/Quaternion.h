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

#ifndef _TRAP_QUATERNION_H_
#define _TRAP_QUATERNION_H_

#include "Types.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"

namespace TRAP::Math
{
	template<typename T>
	struct tQuaternion
	{
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		constexpr tQuaternion(tQuaternion&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~tQuaternion() = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		constexpr tQuaternion<T>& operator=(tQuaternion&&) = default;

		//Implementation detail
		typedef T valueType;
		typedef tQuaternion<T> type;

		//Data
		T x, y, z, w;

		//Implicit basic constructors

		/// <summary>
		/// Default Constructor.
		/// </summary>
		constexpr tQuaternion() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		constexpr tQuaternion(const tQuaternion& q) = default;

		/// <summary>
		/// Explicit basic constructor.
		/// </summary>
		constexpr tQuaternion(T s, const Vec<3, T>& v);
		/// <summary>
		/// Explicit basic constructor.
		/// </summary>
		constexpr tQuaternion(T w, T x, T y, T z);

		//Conversion constructors

		template<typename U>
		constexpr explicit tQuaternion(const tQuaternion<T>& q);

		//Explicit conversion operators
		//explicit operator Mat<3, 3, T>() const;
		//explicit operator Mat<4, 4, T>() const;

		/// <summary>
		/// Create a quaternion from two normalized axis.
		/// </summary>
		tQuaternion(const Vec<3, T>& u, const Vec<3, T>& v);

		/// <summary>
		/// Build a quaternion from euler angles (pitch, yaw, roll).
		/// </summary>
		/// <param name="eulerAnglesInRadians">Euler angles (pitch, yaw, roll).</param>
		constexpr explicit tQuaternion(const Vec<3, T>& eulerAnglesInRadians);
		explicit tQuaternion(const Mat<3, 3, T>& m);
		explicit tQuaternion(const Mat<4, 4, T>& m);

		//Unary arithmetic operators
		constexpr tQuaternion<T>& operator=(const tQuaternion<T>& q) = default;

		template<typename U>
		constexpr tQuaternion<T>& operator=(const tQuaternion<U>& q);
		template<typename U>
		constexpr tQuaternion<T>& operator+=(const tQuaternion<U>& q);
		template<typename U>
		constexpr tQuaternion<T>& operator-=(const tQuaternion<U>& q);
		template<typename U>
		constexpr tQuaternion<T>& operator*=(const tQuaternion<U>& r);
		template<typename U>
		constexpr tQuaternion<T>& operator*=(U s);
		template<typename U>
		constexpr tQuaternion<T>& operator/=(U s);

		/// <summary>
		/// Retrieve the count of components of a quaternion.
		/// </summary>
		/// <returns>Count.</returns>
		static constexpr int Length();

		//Component Access
		constexpr T& operator[](int i);
		constexpr const T& operator[](int i) const;
	};

	//-------------------------------------------------------------------------------------------------------------------//
	//Unary bit operators

	template<typename T>
	constexpr tQuaternion<T> operator+(const tQuaternion<T>& q);

	template<typename T>
	constexpr tQuaternion<T> operator-(const tQuaternion<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//
	//Binary operators

	template<typename T>
	constexpr tQuaternion<T> operator+(const tQuaternion<T>& q, const tQuaternion<T>& p);

	template<typename T>
	constexpr tQuaternion<T> operator-(const tQuaternion<T>& q, const tQuaternion<T>& p);

	template<typename T>
	constexpr tQuaternion<T> operator*(const tQuaternion<T>& q, const tQuaternion<T>& p);

	template<typename T>
	constexpr Vec<3, T> operator*(const tQuaternion<T>& q, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator*(const Vec<3, T>& v, const tQuaternion<T>& q);

	template<typename T>
	constexpr Vec<4, T> operator*(const tQuaternion<T>& q, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v, const tQuaternion<T>& q);

	template<typename T>
	constexpr tQuaternion<T> operator*(const tQuaternion<T>& q, const T& s);

	template<typename T>
	constexpr tQuaternion<T> operator*(const T& s, const tQuaternion<T>& q);

	template<typename T>
	constexpr tQuaternion<T> operator/(const tQuaternion<T>& q, const T& s);

	//-------------------------------------------------------------------------------------------------------------------//
	//Boolean operators

	template<typename T>
	constexpr bool operator==(const tQuaternion<T>& q1, const tQuaternion<T>& q2);

	template<typename T>
	constexpr bool operator!=(const tQuaternion<T>& q1, const tQuaternion<T>& q2);
}

//-------------------------------------------------------------------------------------------------------------------//
//Explicit basic constructors
template <typename T>
constexpr TRAP::Math::tQuaternion<T>::tQuaternion(T s, const Vec<3, T>& v)
	: x(v.x), y(v.y), z(v.z), w(s)
{
}

template <typename T>
constexpr TRAP::Math::tQuaternion<T>::tQuaternion(T w, T x, T y, T z)
	: x(x), y(y), z(z), w(w)
{
}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion constructors

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuaternion<T>::tQuaternion(const tQuaternion<T>& q)
	: x(static_cast<T>(q.x)), y(static_cast<T>(q.y)), z(static_cast<T>(q.z)), w(static_cast<T>(q.w))
{
}

//-------------------------------------------------------------------------------------------------------------------//
//Explicit conversion operators

/*template <typename T>
TRAP::Math::tQuaternion<T>::operator Mat<3, 3, T>() const
{
	return Mat3Cast(*this);
}

template <typename T>
TRAP::Math::tQuaternion<T>::operator Mat<4, 4, T>() const
{
	return Mat4Cast(*this);
}*/

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::tQuaternion<T>::tQuaternion(const Vec<3, T>& u, const Vec<3, T>& v)
{
	T normUNormV = Sqrt(Dot(u, u) * Dot(v, v));
	T realPart = normUNormV + Dot(u, v);
	Vec<3, T> t;

	if(realPart < static_cast<T>(1.e-6f) * normUNormV)
	{
		//If u and v are exactly opposite, rotate 180 degrees around an arbitrary orthogonal axis.
		//Axis normalization can happen later, when we normalize the quaternion.
		realPart = static_cast<T>(0);
		t = Abs(u.x) > Abs(u.z) ? Vec<3, T>(-u.y, u.x, static_cast<T>(0)) : Vec<3, T>(static_cast<T>(0), -u.z, u.y);
	}
	else
	{
		//Otherwise, build quaternion the standard way.
		t = Cross(u, v);
	}

	*this = Normalize(tQuaternion<T>(realPart, t.x, t.y, t.z));
}

//-------------------------------------------------------------------------------------------------------------------//
//Build quaternion from euler angles

template <typename T>
constexpr TRAP::Math::tQuaternion<T>::tQuaternion(const Vec<3, T>& eulerAnglesInRadians)
{
	Vec<3, T> c = Cos(eulerAnglesInRadians * T(0.5));
	Vec<3, T> s = Sin(eulerAnglesInRadians * T(0.5));

	this->w = c.x * c.y * c.z + s.x * s.y * s.z;
	this->x = s.x * c.y * c.z - c.x * s.y * s.z;
	this->y = c.x * s.y * c.z + s.x * c.y * s.z;
	this->z = c.x * c.y * s.z - s.x * s.y * c.z;
}

template <typename T>
TRAP::Math::tQuaternion<T>::tQuaternion(const Mat<3, 3, T>& m)
{
	*this = QuaternionCast(m);
}

template <typename T>
TRAP::Math::tQuaternion<T>::tQuaternion(const Mat<4, 4, T>& m)
{
	*this = QuaternionCast(m);
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuaternion<T>& TRAP::Math::tQuaternion<T>::operator=(const tQuaternion<U>& q)
{
	this->w = static_cast<T>(q.w);
	this->x = static_cast<T>(q.x);
	this->y = static_cast<T>(q.y);
	this->z = static_cast<T>(q.z);

	return *this;
}

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuaternion<T>& TRAP::Math::tQuaternion<T>::operator+=(const tQuaternion<U>& q)
{
	const tQuaternion<T> p(q);

	return (*this = tQuaternion<T>(this->w + p.w, this->x + p.x, this->y + p.y, this->z + p.z));
}

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuaternion<T>& TRAP::Math::tQuaternion<T>::operator-=(const tQuaternion<U>& q)
{
	const tQuaternion<T> p(q);

	return (*this = tQuaternion<T>(this->w - p.w, this->x - p.x, this->y - p.y, this->z - p.z));
}

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuaternion<T>& TRAP::Math::tQuaternion<T>::operator*=(const tQuaternion<U>& r)
{
	const tQuaternion<T> p(*this);
	const tQuaternion<T> q(r);

	this->w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
	this->x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
	this->y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
	this->z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;

	return *this;
}

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuaternion<T>& TRAP::Math::tQuaternion<T>::operator*=(U s)
{
	return (*this = tQuaternion<T>(this->w * s, this->x * s, this->y * s, this->z * s));
}

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuaternion<T>& TRAP::Math::tQuaternion<T>::operator/=(U s)
{
	return (*this = tQuaternion<T>(this->w / s, this->x / s, this->y / s, this->z / s));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr int TRAP::Math::tQuaternion<T>::Length()
{
	return 4;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component Access

template <typename T>
constexpr T& TRAP::Math::tQuaternion<T>::operator[](int i)
{
	TRAP_ASSERT(i >= 0 && i < this->Length());

	return (&x)[i];
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr const T& TRAP::Math::tQuaternion<T>::operator[](int i) const
{
	TRAP_ASSERT(i >= 0 && i < this->Length());

	return (&x)[i];
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------//
//Unary bit operators

template <typename T>
constexpr TRAP::Math::tQuaternion<T> TRAP::Math::operator+(const tQuaternion<T>& q)
{
	return q;
}

template <typename T>
constexpr TRAP::Math::tQuaternion<T> TRAP::Math::operator-(const tQuaternion<T>& q)
{
	return tQuaternion<T>(-q.w, -q.x, -q.y, -q.z);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary operators

template <typename T>
constexpr TRAP::Math::tQuaternion<T> TRAP::Math::operator+(const tQuaternion<T>& q, const tQuaternion<T>& p)
{
	return tQuaternion<T>(q) += p;
}

template <typename T>
constexpr TRAP::Math::tQuaternion<T> TRAP::Math::operator-(const tQuaternion<T>& q, const tQuaternion<T>& p)
{
	return tQuaternion<T>(q) -= p;
}

template <typename T>
constexpr TRAP::Math::tQuaternion<T> TRAP::Math::operator*(const tQuaternion<T>& q, const tQuaternion<T>& p)
{
	return tQuaternion<T>(q) *= p;
}

template <typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const tQuaternion<T>& q, const Vec<3, T>& v)
{
	const Vec<3, T> quaternionVector(q.x, q.y, q.z);
	const Vec<3, T> uv(Cross(quaternionVector, v));
	const Vec<3, T> uuv(Cross(quaternionVector, uv));

	return v + ((uv * q.w) + uuv) * static_cast<T>(2);
}

template <typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const Vec<3, T>& v, const tQuaternion<T>& q)
{
	return Inverse(q) * v;
}

template <typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const tQuaternion<T>& q, const Vec<4, T>& v)
{
	return Vec<4, T>(q * Vec<3, T>(v), v.w);
}

template <typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<4, T>& v, const tQuaternion<T>& q)
{
	return Inverse(q) * v;
}

template <typename T>
constexpr TRAP::Math::tQuaternion<T> TRAP::Math::operator*(const tQuaternion<T>& q, const T& s)
{
	return tQuaternion<T>(q.w * s, q.x * s, q.y * s, q.z * s);
}

template <typename T>
constexpr TRAP::Math::tQuaternion<T> TRAP::Math::operator*(const T& s, const tQuaternion<T>& q)
{
	return q * s;
}

template <typename T>
constexpr TRAP::Math::tQuaternion<T> TRAP::Math::operator/(const tQuaternion<T>& q, const T& s)
{
	return tQuaternion<T>(q.w / s, q.x / s, q.y / s, q.z / s);
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template <typename T>
constexpr bool TRAP::Math::operator==(const tQuaternion<T>& q1, const tQuaternion<T>& q2)
{
	return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
}

template <typename T>
constexpr bool TRAP::Math::operator!=(const tQuaternion<T>& q1, const tQuaternion<T>& q2)
{
	return q1.x != q2.x && q1.y != q2.y && q1.z != q2.z && q1.w != q2.w;
}

#endif /*_TRAP_QUATERNION_H_*/
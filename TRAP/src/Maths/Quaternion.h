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

#ifndef TRAP_QUATERNION_H
#define TRAP_QUATERNION_H

#include <cmath>

#include "Vec3.h"
#include "Core/PlatformDetection.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"

namespace TRAP::Math
{
	template<typename T>
	struct tQuat
	{
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr tQuat(tQuat&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~tQuat() = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr tQuat<T>& operator=(tQuat&&) noexcept = default;

		//Implementation detail
		using valueType = T;
		using type = tQuat<T>;

		//Data
		T x, y, z, w;

		//Implicit basic constructors

		/// <summary>
		/// Constructor.
		/// </summary>
		constexpr tQuat() noexcept = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr tQuat(const tQuat& q) noexcept = default;

		/// <summary>
		/// Explicit basic constructor.
		/// </summary>
		constexpr tQuat(T s, const Vec<3, T>& v) noexcept;
		/// <summary>
		/// Explicit basic constructor.
		/// </summary>
		constexpr tQuat(T w_, T x_, T y_, T z_) noexcept;

		//Conversion constructors

		template<typename U>
		explicit constexpr tQuat(const tQuat<T>& q) noexcept;

		//Explicit conversion operators
		//explicit operator Mat<3, 3, T>() const;
		//explicit operator Mat<4, 4, T>() const;

		/// <summary>
		/// Create a quaternion from two normalized axis.
		/// </summary>
		tQuat(const Vec<3, T>& u, const Vec<3, T>& v);

		/// <summary>
		/// Build a quaternion from euler angles (pitch, yaw, roll).
		/// </summary>
		/// <param name="eulerAnglesInRadians">Euler angles (pitch, yaw, roll).</param>
		explicit constexpr tQuat(const Vec<3, T>& eulerAnglesInRadians);
		explicit tQuat(const Mat<3, 3, T>& m);
		explicit tQuat(const Mat<4, 4, T>& m);

		//Unary arithmetic operators
		constexpr tQuat<T>& operator=(const tQuat<T>& q) noexcept = default;

		template<typename U>
		constexpr tQuat<T>& operator=(const tQuat<U>& q) noexcept;
		template<typename U>
		constexpr tQuat<T>& operator+=(const tQuat<U>& q) noexcept;
		template<typename U>
		constexpr tQuat<T>& operator-=(const tQuat<U>& q) noexcept;
		template<typename U>
		constexpr tQuat<T>& operator*=(const tQuat<U>& r) noexcept;
		template<typename U>
		constexpr tQuat<T>& operator*=(U s) noexcept;
		template<typename U>
		constexpr tQuat<T>& operator/=(U s) noexcept;

		/// <summary>
		/// Retrieve the count of components of a quaternion.
		/// </summary>
		/// <returns>Count.</returns>
		static constexpr int Length() noexcept;

		//Component Access
		constexpr T& operator[](int i);
		constexpr const T& operator[](int i) const;

		std::string ToString() const;
	};

	//-------------------------------------------------------------------------------------------------------------------//
	//Unary bit operators

	template<typename T>
	constexpr tQuat<T> operator+(const tQuat<T>& q) noexcept;

	template<typename T>
	constexpr tQuat<T> operator-(const tQuat<T>& q) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//
	//Binary operators

	template<typename T>
	constexpr tQuat<T> operator+(const tQuat<T>& q, const tQuat<T>& p) noexcept;

	template<typename T>
	constexpr tQuat<T> operator-(const tQuat<T>& q, const tQuat<T>& p) noexcept;

	template<typename T>
	constexpr tQuat<T> operator*(const tQuat<T>& q, const tQuat<T>& p) noexcept;

	template<typename T>
	constexpr Vec<3, T> operator*(const tQuat<T>& q, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator*(const Vec<3, T>& v, const tQuat<T>& q);

	template<typename T>
	constexpr Vec<4, T> operator*(const tQuat<T>& q, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v, const tQuat<T>& q);

	template<typename T>
	constexpr tQuat<T> operator*(const tQuat<T>& q, const T& s) noexcept;

	template<typename T>
	constexpr tQuat<T> operator*(const T& s, const tQuat<T>& q) noexcept;

	template<typename T>
	constexpr tQuat<T> operator/(const tQuat<T>& q, const T& s) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//
	//Boolean operators

	template<typename T>
	constexpr bool operator==(const tQuat<T>& q1, const tQuat<T>& q2) noexcept;

	template<typename T>
	constexpr bool operator!=(const tQuat<T>& q1, const tQuat<T>& q2) noexcept;
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	struct hash<TRAP::Math::tQuat<T>>
	{
		constexpr std::size_t operator()(const TRAP::Math::tQuat<T>& q) const noexcept
		{
			std::size_t seed = 0;
			hash<T> hasher;
			TRAP::Utils::HashCombine(seed, hasher(q.x), hasher(q.y), hasher(q.z), hasher(q.w));
			return seed;
		}
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//Explicit basic constructors
template <typename T>
constexpr TRAP::Math::tQuat<T>::tQuat(const T s, const Vec<3, T>& v) noexcept
	: x(v.x), y(v.y), z(v.z), w(s)
{
}

template <typename T>
constexpr TRAP::Math::tQuat<T>::tQuat(const T w_, const T x_, const T y_, const T z_) noexcept
	: x(x_), y(y_), z(z_), w(w_)
{
}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion constructors

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuat<T>::tQuat(const tQuat<T>& q) noexcept
	: x(static_cast<T>(q.x)), y(static_cast<T>(q.y)), z(static_cast<T>(q.z)), w(static_cast<T>(q.w))
{
}

//-------------------------------------------------------------------------------------------------------------------//
//Explicit conversion operators

/*template <typename T>
TRAP::Math::tQuat<T>::operator Mat<3, 3, T>() const
{
	return Mat3Cast(*this);
}

template <typename T>
TRAP::Math::tQuat<T>::operator Mat<4, 4, T>() const
{
	return Mat4Cast(*this);
}*/

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::tQuat<T>::tQuat(const Vec<3, T>& u, const Vec<3, T>& v)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	//TODO Can't use TRAP::Math::Sqrt here
	const T normUNormV = std::sqrt(Dot(u, u) * Dot(v, v));
	T realPart = normUNormV + Dot(u, v);
	Vec<3, T> t;

	if(realPart < static_cast<T>(1.e-6f) * normUNormV)
	{
		//If u and v are exactly opposite, rotate 180 degrees around an arbitrary orthogonal axis.
		//Axis normalization can happen later, when we normalize the quaternion.
		realPart = static_cast<T>(0);
		//TODO Can't use TRAP::Math::Abs here
		t = std::abs(u.x) > std::abs(u.z) ? Vec<3, T>(-u.y, u.x, static_cast<T>(0)) : Vec<3, T>(static_cast<T>(0), -u.z, u.y);
	}
	else
	{
		//Otherwise, build quaternion the standard way.
		t = Cross(u, v);
	}

	*this = Normalize(tQuat<T>(realPart, t.x, t.y, t.z));
}

//-------------------------------------------------------------------------------------------------------------------//
//Build quaternion from euler angles

template <typename T>
constexpr TRAP::Math::tQuat<T>::tQuat(const Vec<3, T>& eulerAnglesInRadians)
{
	const Vec<3, T> c = Cos(eulerAnglesInRadians * T(0.5));
	const Vec<3, T> s = Sin(eulerAnglesInRadians * T(0.5));

	this->w = c.x * c.y * c.z + s.x * s.y * s.z;
	this->x = s.x * c.y * c.z - c.x * s.y * s.z;
	this->y = c.x * s.y * c.z + s.x * c.y * s.z;
	this->z = c.x * c.y * s.z - s.x * s.y * c.z;
}

template <typename T>
TRAP::Math::tQuat<T>::tQuat(const Mat<3, 3, T>& m)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	*this = QuaternionCast(m);
}

template <typename T>
TRAP::Math::tQuat<T>::tQuat(const Mat<4, 4, T>& m)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	*this = QuaternionCast(m);
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator=(const tQuat<U>& q) noexcept
{
	this->w = static_cast<T>(q.w);
	this->x = static_cast<T>(q.x);
	this->y = static_cast<T>(q.y);
	this->z = static_cast<T>(q.z);

	return *this;
}

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator+=(const tQuat<U>& q) noexcept
{
	const tQuat<T> p(q);

	return (*this = tQuat<T>(this->w + p.w, this->x + p.x, this->y + p.y, this->z + p.z));
}

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator-=(const tQuat<U>& q) noexcept
{
	const tQuat<T> p(q);

	return (*this = tQuat<T>(this->w - p.w, this->x - p.x, this->y - p.y, this->z - p.z));
}

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator*=(const tQuat<U>& r) noexcept
{
	const tQuat<T> p(*this);
	const tQuat<T> q(r);

	this->w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
	this->x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
	this->y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
	this->z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;

	return *this;
}

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator*=(const U s) noexcept
{
	return (*this = tQuat<T>(this->w * s, this->x * s, this->y * s, this->z * s));
}

template <typename T>
template <typename U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator/=(const U s) noexcept
{
	return (*this = tQuat<T>(this->w / s, this->x / s, this->y / s, this->z / s));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr int TRAP::Math::tQuat<T>::Length() noexcept
{
	return 4;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component Access

template <typename T>
constexpr T& TRAP::Math::tQuat<T>::operator[](const int i)
{
	TRAP_ASSERT(i >= 0 && i < this->Length(), "Math::tQuat<T>::operator[]: Index out of range!");

	return (&x)[i];
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr const T& TRAP::Math::tQuat<T>::operator[](const int i) const
{
	TRAP_ASSERT(i >= 0 && i < this->Length(), "Math::tQuat<T>::operator[]: Index out of range!");

	return (&x)[i];
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
std::string TRAP::Math::tQuat<T>::ToString() const
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	std::string postfix = "";
	if constexpr(std::is_same_v<T, float>)
		postfix = "f";
	else if constexpr(std::is_same_v<T, double>)
		postfix = "d";
	else
		return "Unknown type";

	return "Quat" + postfix + "(" + std::to_string(w) + ", {" + std::to_string(x) + ", " +
	       std::to_string(y) + ", " + std::to_string(z) + "})";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------//
//Unary bit operators

template <typename T>
constexpr TRAP::Math::tQuat<T> TRAP::Math::operator+(const tQuat<T>& q) noexcept
{
	return q;
}

template <typename T>
constexpr TRAP::Math::tQuat<T> TRAP::Math::operator-(const tQuat<T>& q) noexcept
{
	return tQuat<T>(-q.w, -q.x, -q.y, -q.z);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary operators

template <typename T>
constexpr TRAP::Math::tQuat<T> TRAP::Math::operator+(const tQuat<T>& q, const tQuat<T>& p) noexcept
{
	return tQuat<T>(q) += p;
}

template <typename T>
constexpr TRAP::Math::tQuat<T> TRAP::Math::operator-(const tQuat<T>& q, const tQuat<T>& p) noexcept
{
	return tQuat<T>(q) -= p;
}

template <typename T>
constexpr TRAP::Math::tQuat<T> TRAP::Math::operator*(const tQuat<T>& q, const tQuat<T>& p) noexcept
{
	return tQuat<T>(q) *= p;
}

template <typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const tQuat<T>& q, const Vec<3, T>& v)
{
	const Vec<3, T> quaternionVector(q.x, q.y, q.z);
	const Vec<3, T> uv(Cross(quaternionVector, v));
	const Vec<3, T> uuv(Cross(quaternionVector, uv));

	return v + ((uv * q.w) + uuv) * static_cast<T>(2);
}

template <typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const Vec<3, T>& v, const tQuat<T>& q)
{
	return Inverse(q) * v;
}

template <typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const tQuat<T>& q, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(q * Vec<3, T>(v), v.w);
}

template <typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<4, T>& v, const tQuat<T>& q)
{
	return Inverse(q) * v;
}

template <typename T>
constexpr TRAP::Math::tQuat<T> TRAP::Math::operator*(const tQuat<T>& q, const T& s) noexcept
{
	return tQuat<T>(q.w * s, q.x * s, q.y * s, q.z * s);
}

template <typename T>
constexpr TRAP::Math::tQuat<T> TRAP::Math::operator*(const T& s, const tQuat<T>& q) noexcept
{
	return q * s;
}

template <typename T>
constexpr TRAP::Math::tQuat<T> TRAP::Math::operator/(const tQuat<T>& q, const T& s) noexcept
{
	return tQuat<T>(q.w / s, q.x / s, q.y / s, q.z / s);
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template <typename T>
constexpr bool TRAP::Math::operator==(const tQuat<T>& q1, const tQuat<T>& q2) noexcept
{
	return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
}

template <typename T>
constexpr bool TRAP::Math::operator!=(const tQuat<T>& q1, const tQuat<T>& q2) noexcept
{
	return q1.x != q2.x && q1.y != q2.y && q1.z != q2.z && q1.w != q2.w;
}

#endif /*TRAP_QUATERNION_H*/
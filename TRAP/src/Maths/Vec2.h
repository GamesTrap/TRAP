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

#ifndef TRAP_VEC2_H
#define TRAP_VEC2_H

#include "Types.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"

namespace TRAP::Math
{
	/// <summary>
	/// Struct for a 2-dimensional vector.
	/// </summary>
	template<class T>
	struct Vec<2, T>
	{
		//Implementation detail
		typedef T valueType;
		typedef Vec<2, T> type;
		typedef Vec<2, bool> boolType;

		//Data
		T x, y;

		//Implicit basic constructors
		constexpr Vec() = default;
		constexpr Vec(const Vec & v) = default;

		//Explicit basic constructors
		constexpr explicit Vec(T scalar);
		constexpr Vec(T x, T y);

		//Conversion constructors
		template<typename U>
		constexpr explicit Vec(const Vec<1, U> & v);

		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(A x, B y);
		template<typename A, typename B>
		constexpr Vec(const Vec<1, A> & x, B y);
		template<typename A, typename B>
		constexpr Vec(A x, const Vec<1, B> & y);
		template<typename A, typename B>
		constexpr Vec(const Vec<1, A> & x, const Vec<1, B> & y);

		//Conversion vector constructors

		//Explicit conversions
		template<typename U>
		constexpr explicit Vec(const Vec<3, U> & v);
		//Explicit conversions
		template<typename U>
		constexpr explicit Vec(const Vec<4, U> & v);

		//Explicit conversions
		template<typename U>
		constexpr explicit Vec(const Vec<2, U> & v);

		constexpr Vec(Vec&&) = default;
		~Vec() = default;
		constexpr Vec<2, T>& operator=(Vec&&) = default;

		/// <summary>
		/// Retrieve the count of components of the vector.
		/// </summary>
		/// <returns>Count of components.</returns>
		static constexpr int Length();

		//Component Access
		constexpr T& operator[](int i);
		constexpr const T& operator[](int i) const;

		//Unary arithmetic operators
		constexpr Vec<2, T>& operator=(const Vec& v) = default;

		template<typename U>
		constexpr Vec<2, T>& operator=(const Vec<2, U> & v);
		template<typename U>
		constexpr Vec<2, T>& operator+=(U scalar);
		template<typename U>
		constexpr Vec<2, T>& operator+=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<2, T>& operator+=(const Vec<2, U> & v);
		template<typename U>
		constexpr Vec<2, T>& operator-=(U scalar);
		template<typename U>
		constexpr Vec<2, T>& operator-=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<2, T>& operator-=(const Vec<2, U> & v);
		template<typename U>
		constexpr Vec<2, T>& operator*=(U scalar);
		template<typename U>
		constexpr Vec<2, T>& operator*=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<2, T>& operator*=(const Vec<2, U> & v);
		template<typename U>
		constexpr Vec<2, T>& operator/=(U scalar);
		template<typename U>
		constexpr Vec<2, T>& operator/=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<2, T>& operator/=(const Vec<2, U> & v);

		//Increment and decrement operators
		constexpr Vec<2, T>& operator++();
		constexpr Vec<2, T>& operator--();
		constexpr Vec<2, T> operator++(int);
		constexpr Vec<2, T> operator--(int);

		//Unary bit operators
		template<typename U>
		constexpr Vec<2, T>& operator%=(U scalar);
		template<typename U>
		constexpr Vec<2, T>& operator%=(const Vec<1, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator%=(const Vec<2, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator&=(U scalar);
		template<typename U>
		constexpr Vec<2, T>& operator&=(const Vec<1, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator&=(const Vec<2, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator|=(U scalar);
		template<typename U>
		constexpr Vec<2, T>& operator|=(const Vec<1, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator|=(const Vec<2, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator^=(U scalar);
		template<typename U>
		constexpr Vec<2, T>& operator^=(const Vec<1, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator^=(const Vec<2, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator<<=(U scalar);
		template<typename U>
		constexpr Vec<2, T>& operator<<=(const Vec<1, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator<<=(const Vec<2, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator>>=(U scalar);
		template<typename U>
		constexpr Vec<2, T>& operator>>=(const Vec<1, U>& v);
		template<typename U>
		constexpr Vec<2, T>& operator>>=(const Vec<2, U>& v);
	};

	//Unary operators
	template<typename T>
	constexpr Vec<2, T> operator+(const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator-(const Vec<2, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	//Binary operators
	template<typename T>
	constexpr Vec<2, T> operator+(const Vec<2, T>& v, T scalar);

	template<typename T>
	constexpr Vec<2, T> operator+(const Vec<2, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator+(T scalar, const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator+(const Vec<1, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator+(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator-(const Vec<2, T>& v, T scalar);

	template<typename T>
	constexpr Vec<2, T> operator-(const Vec<2, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator-(T scalar, const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator-(const Vec<1, T>& v1, const  Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator-(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator*(const Vec<2, T>& v, T scalar);

	template<typename T>
	constexpr Vec<2, T> operator*(const Vec<2, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator*(T scalar, const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator*(const Vec<1, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator*(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator/(const Vec<2, T>& v, T scalar);

	template<typename T>
	constexpr Vec<2, T> operator/(const Vec<2, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator/(T scalar, const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator/(const Vec<1, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator/(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator%(const Vec<2, T>& v, T scalar);

	template<typename T>
	constexpr Vec<2, T> operator%(const Vec<2, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator%(T scalar, const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator%(const Vec<1, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator%(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator&(const Vec<2, T>& v, T scalar);

	template<typename T>
	constexpr Vec<2, T> operator&(const Vec<2, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator&(T scalar, const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator&(const Vec<1, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator&(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator|(const Vec<2, T>& v, T scalar);

	template<typename T>
	constexpr Vec<2, T> operator|(const Vec<2, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator|(T scalar, const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator|(const Vec<1, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator|(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator^(const Vec<2, T>& v, T scalar);

	template<typename T>
	constexpr Vec<2, T> operator^(const Vec<2, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator^(T scalar, const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator^(const Vec<1, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator^(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator<<(const Vec<2, T>& v, T scalar);

	template<typename T>
	constexpr Vec<2, T> operator<<(const Vec<2, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator<<(T scalar, const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator<<(const Vec<1, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator<<(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator>>(const Vec<2, T>& v, T scalar);

	template<typename T>
	constexpr Vec<2, T> operator>>(const Vec<2, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator>>(T scalar, const Vec<2, T>& v);

	template<typename T>
	constexpr Vec<2, T> operator>>(const Vec<1, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator>>(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr Vec<2, T> operator~(const Vec<2, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	//Boolean operators
	template<typename T>
	constexpr bool operator==(const Vec<2, T>& v1, const Vec<2, T>& v2);

	template<typename T>
	constexpr bool operator!=(const Vec<2, T>& v1, const Vec<2, T>& v2);

	constexpr Vec<2, bool> operator&&(const Vec<2, bool>& v1, const Vec<2, bool>& v2);

	constexpr Vec<2, bool> operator||(const Vec<2, bool>& v1, const Vec<2, bool>& v2);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------//

//Explicit basic constructors
//
template<typename T>
constexpr TRAP::Math::Vec<2, T>::Vec(T scalar)
	: x(scalar), y(scalar)
{}

template<typename T>
constexpr TRAP::Math::Vec<2, T>::Vec(T x, T y)
	: x(x), y(y)
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion scalar constructors

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>::Vec(const Vec<1, U>& v)
	: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<2, T>::Vec(A x, B y)
	: x(static_cast<T>(x)), y(static_cast<T>(y))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<2, T>::Vec(const Vec<1, A>& x, B y)
	: x(static_cast<T>(x.x)), y(static_cast<T>(y))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<2, T>::Vec(A x, const Vec<1, B>& y)
	: x(static_cast<T>(x)), y(static_cast<T>(y.x))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<2, T>::Vec(const Vec<1, A>& x, const Vec<1, B>& y)
	: x(static_cast<T>(x.x)), y(static_cast<T>(y.x))
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion vector constructors

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>::Vec(const Vec<2, U>& v)
	: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
{}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>::Vec(const Vec<3, U>& v)
	: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
{}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>::Vec(const Vec<4, U>& v)
	: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
{}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
constexpr int TRAP::Math::Vec<2, T>::Length()
{
	return 2;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component accesses

template<typename T>
constexpr T& TRAP::Math::Vec<2, T>::operator[](int i)
{
	TRAP_ASSERT(i >= 0 && i < this->Length());

	return i == 0 ? x : y;
}

template<typename T>
constexpr const T& TRAP::Math::Vec<2, T>::operator[](int i) const
{
	TRAP_ASSERT(i >= 0 && i < this->Length());

	return i == 0 ? x : y;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator=(const Vec<2, U>& v)
{
	this->x = static_cast<T>(v.x);
	this->y = static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator+=(U scalar)
{
	this->x += static_cast<T>(scalar);
	this->y += static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator+=(const Vec<1, U>& v)
{
	this->x += static_cast<T>(v.x);
	this->y += static_cast<T>(v.x);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator+=(const Vec<2, U>& v)
{
	this->x += static_cast<T>(v.x);
	this->y += static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator-=(U scalar)
{
	this->x -= static_cast<T>(scalar);
	this->y -= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator-=(const Vec<1, U>& v)
{
	this->x -= static_cast<T>(v.x);
	this->y -= static_cast<T>(v.x);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator-=(const Vec<2, U>& v)
{
	this->x -= static_cast<T>(v.x);
	this->y -= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator*=(U scalar)
{
	this->x *= static_cast<T>(scalar);
	this->y *= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator*=(const Vec<1, U>& v)
{
	this->x *= static_cast<T>(v.x);
	this->y *= static_cast<T>(v.x);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator*=(const Vec<2, U>& v)
{
	this->x *= static_cast<T>(v.x);
	this->y *= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator/=(U scalar)
{
	this->x /= static_cast<T>(scalar);
	this->y /= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator/=(const Vec<1, U>& v)
{
	this->x /= static_cast<T>(v.x);
	this->y /= static_cast<T>(v.x);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator/=(const Vec<2, U>& v)
{
	this->x /= static_cast<T>(v.x);
	this->y /= static_cast<T>(v.y);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator++()
{
	++this->x;
	++this->y;

	return *this;
}

template<typename T>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator--()
{
	--this->x;
	--this->y;

	return *this;
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Vec<2, T>::operator++(int)
{
	Vec<2, T> result(*this);
	++*this;

	return result;
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Vec<2, T>::operator--(int)
{
	Vec<2, T> result(*this);
	--*this;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary bit operators

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator%=(U scalar)
{
	this->x %= static_cast<T>(scalar);
	this->y %= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator%=(const Vec<1, U>& v)
{
	this->x %= static_cast<T>(v.x);
	this->y %= static_cast<T>(v.x);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator%=(const Vec<2, U>& v)
{
	this->x %= static_cast<T>(v.x);
	this->y %= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator&=(U scalar)
{
	this->x &= static_cast<T>(scalar);
	this->y &= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator&=(const Vec<1, U>& v)
{
	this->x &= static_cast<T>(v.x);
	this->y &= static_cast<T>(v.x);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator&=(const Vec<2, U>& v)
{
	this->x &= static_cast<T>(v.x);
	this->y &= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator|=(U scalar)
{
	this->x |= static_cast<T>(scalar);
	this->y |= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator|=(const Vec<1, U>& v)
{
	this->x |= static_cast<T>(v.x);
	this->y |= static_cast<T>(v.x);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator|=(const Vec<2, U>& v)
{
	this->x |= static_cast<T>(v.x);
	this->y |= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator^=(U scalar)
{
	this->x ^= static_cast<T>(scalar);
	this->y ^= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator^=(const Vec<1, U>& v)
{
	this->x ^= static_cast<T>(v.x);
	this->y ^= static_cast<T>(v.x);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator^=(const Vec<2, U>& v)
{
	this->x ^= static_cast<T>(v.x);
	this->y ^= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator<<=(U scalar)
{
	this->x <<= static_cast<T>(scalar);
	this->y <<= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator<<=(const Vec<1, U>& v)
{
	this->x <<= static_cast<T>(v.x);
	this->y <<= static_cast<T>(v.x);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator<<=(const Vec<2, U>& v)
{
	this->x <<= static_cast<T>(v.x);
	this->y <<= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator>>=(U scalar)
{
	this->x >>= static_cast<T>(scalar);
	this->y >>= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator>>=(const Vec<1, U>& v)
{
	this->x >>= static_cast<T>(v.x);
	this->y >>= static_cast<T>(v.x);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator>>=(const Vec<2, U>& v)
{
	this->x >>= static_cast<T>(v.x);
	this->y >>= static_cast<T>(v.y);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<2, T>& v)
{
	return v;
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<2, T>& v)
{
	return Vec<2, T>(-v.x, -v.y);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<2, T>& v, T scalar)
{
	return Vec<2, T>(v.x + scalar, v.y + scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<2, T>& v1, const Vec<1, T>& v2)
{
	return Vec<2, T>(v1.x + v2.x, v1.y + v2.x);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(T scalar, const Vec<2, T>& v)
{
	return Vec<2, T>(scalar + v.x, scalar + v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<1, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x + v2.x, v1.x + v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x + v2.x, v1.y + v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<2, T>& v, T scalar)
{
	return Vec<2, T>(v.x - scalar, v.y - scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<2, T>& v1, const Vec<1, T>& v2)
{
	return Vec<2, T>(v1.x - v2.x, v1.y - v2.x);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(T scalar, const Vec<2, T>& v)
{
	return Vec<2, T>(scalar - v.x, scalar - v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<1, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x - v2.x, v1.x - v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x - v2.x, v1.y - v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(const Vec<2, T>& v, T scalar)
{
	return Vec<2, T>(v.x * scalar, v.y * scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(const Vec<2, T>& v1, const Vec<1, T>& v2)
{
	return Vec<2, T>(v1.x * v2.x, v1.y * v2.x);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(T scalar, const Vec<2, T>& v)
{
	return Vec<2, T>(scalar * v.x, scalar * v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(const Vec<1, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x * v2.x, v1.x * v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x * v2.x, v1.y * v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(const Vec<2, T>& v, T scalar)
{
	return Vec<2, T>(v.x / scalar, v.y / scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(const Vec<2, T>& v1, const Vec<1, T>& v2)
{
	return Vec<2, T>(v1.x / v2.x, v1.y / v2.x);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(T scalar, const Vec<2, T>& v)
{
	return Vec<2, T>(scalar / v.x, scalar / v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(const Vec<1, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x / v2.x, v1.x / v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x / v2.x, v1.y / v2.y);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary bit operators

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(const Vec<2, T>& v, T scalar)
{
	return Vec<2, T>(v.x % scalar, v.y % scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(const Vec<2, T>& v1, const Vec<1, T>& v2)
{
	return Vec<2, T>(v1.x % v2.x, v1.y % v2.x);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(T scalar, const Vec<2, T>& v)
{
	return Vec<2, T>(scalar % v.x, scalar % v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(const Vec<1, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x % v2.x, v1.x % v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x % v2.x, v1.y % v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(const Vec<2, T>& v, T scalar)
{
	return Vec<2, T>(v.x & scalar, v.y & scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(const Vec<2, T>& v1, const Vec<1, T>& v2)
{
	return Vec<2, T>(v1.x & v2.x, v1.y & v2.x);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(T scalar, const Vec<2, T>& v)
{
	return Vec<2, T>(scalar & v.x, scalar & v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(const Vec<1, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x & v2.x, v1.x & v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x & v2.x, v1.y & v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(const Vec<2, T>& v, T scalar)
{
	return Vec<2, T>(v.x | scalar, v.y | scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(const Vec<2, T>& v1, const Vec<1, T>& v2)
{
	return Vec<2, T>(v1.x | v2.x, v1.y | v2.x);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(T scalar, const Vec<2, T>& v)
{
	return Vec<2, T>(scalar | v.x, scalar | v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(const Vec<1, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x | v2.x, v1.x | v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x | v2.x, v1.y | v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(const Vec<2, T>& v, T scalar)
{
	return Vec<2, T>(v.x ^ scalar, v.y ^ scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(const Vec<2, T>& v1, const Vec<1, T>& v2)
{
	return Vec<2, T>(v1.x ^ v2.x, v1.y ^ v2.x);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(T scalar, const Vec<2, T>& v)
{
	return Vec<2, T>(scalar ^ v.x, scalar ^ v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(const Vec<1, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x ^ v2.x, v1.x ^ v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x ^ v2.x, v1.y ^ v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(const Vec<2, T>& v, T scalar)
{
	return Vec<2, T>(v.x << scalar, v.y << scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(const Vec<2, T>& v1, const Vec<1, T>& v2)
{
	return Vec<2, T>(v1.x << v2.x, v1.y << v2.x);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(T scalar, const Vec<2, T>& v)
{
	return Vec<2, T>(scalar << v.x, scalar << v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(const Vec<1, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x << v2.x, v1.x << v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x << v2.x, v1.y << v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(const Vec<2, T>& v, T scalar)
{
	return Vec<2, T>(v.x >> scalar, v.y >> scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(const Vec<2, T>& v1, const Vec<1, T>& v2)
{
	return Vec<2, T>(v1.x >> v2.x, v1.y >> v2.x);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(T scalar, const Vec<2, T>& v)
{
	return Vec<2, T>(scalar >> v.x, scalar >> v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(const Vec<1, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x >> v2.x, v1.x >> v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return Vec<2, T>(v1.x >> v2.x, v1.y >> v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator~(const Vec<2, T>& v)
{
	return Vec<2, T>(~v.x, ~v.y);
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
constexpr bool TRAP::Math::operator==(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return v1.x == v2.x && v1.y == v2.y;
}

template<typename T>
constexpr bool TRAP::Math::operator!=(const Vec<2, T>& v1, const Vec<2, T>& v2)
{
	return !(v1 == v2);
}

constexpr TRAP::Math::Vec<2, bool> TRAP::Math::operator&&(const Vec<2, bool>& v1, const Vec<2, bool>& v2)
{
	return Vec<2, bool>(v1.x && v2.x, v1.y && v2.y);
}

constexpr TRAP::Math::Vec<2, bool> TRAP::Math::operator||(const Vec<2, bool>& v1, const Vec<2, bool>& v2)
{
	return Vec<2, bool>(v1.x || v2.x, v1.y || v2.y);
}

#endif /*TRAP_VEC2_H*/
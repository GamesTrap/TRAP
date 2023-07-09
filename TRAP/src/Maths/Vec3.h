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

#ifndef TRAP_VEC3_H
#define TRAP_VEC3_H

#include "Types.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Utils/Utils.h"

namespace TRAP::Math
{
	/// <summary>
	/// Struct for a 3-dimensional vector.
	/// </summary>
	template<typename T>
	requires std::is_arithmetic_v<T>
	struct Vec<3, T>
	{
		//Implementation details
		using valueType = T;
		using type = Vec<3, T>;
		using boolType = Vec<3, bool>;

		//Data
		T x, y, z;

		//Implicit basic constructors
		constexpr Vec() noexcept = default;
		constexpr Vec(const Vec & v) noexcept = default;

		//Explicit basic constructors
		explicit constexpr Vec(T scalar) noexcept;
		constexpr Vec(T x_, T y_, T z_) noexcept;

		//Explicit conversions
		template<typename X, typename Y, typename Z>
		constexpr Vec(X x_, Y y_, Z z_) noexcept;

		//Conversion vector constructors

		//Explicit conversions
		template<typename A, typename B>
		requires std::is_arithmetic_v<A>
		constexpr Vec(const Vec<2, A> & xy, B z_) noexcept;
		//Explicit conversions
		template<typename A, typename B>
		requires std::is_arithmetic_v<B>
		constexpr Vec(A x_, const Vec<2, B> & yz) noexcept;
		//Explicit conversions
		template<typename U>
		requires std::is_arithmetic_v<U>
		explicit constexpr Vec(const Vec<4, U> & v) noexcept;

		//Explicit conversions
		template<typename U>
		requires std::is_arithmetic_v<U>
		explicit constexpr Vec(const Vec<3, U> & v) noexcept;

		constexpr Vec(Vec&&) noexcept = default;
		constexpr ~Vec() = default;
		constexpr Vec<3, T>& operator=(Vec&&) noexcept = default;

		/// <summary>
		/// Retrieve the count of components of the vector.
		/// </summary>
		/// <returns>Count of components.</returns>
		[[nodiscard]] static constexpr std::size_t Length() noexcept;

		//Component accesses
		[[nodiscard]] constexpr T& operator[](std::size_t i);
		[[nodiscard]] constexpr const T& operator[](std::size_t i) const;

		[[nodiscard]] T& at(std::size_t i);
		[[nodiscard]] const T& at(std::size_t i) const;

		//Unary arithmetic operators
		constexpr Vec<3, T>& operator=(const Vec<3, T>& v) noexcept = default;

		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator=(const Vec<3, U> & v) noexcept;
		template<typename U>
		constexpr Vec<3, T>& operator+=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator+=(const Vec<3, U> & v) noexcept;
		template<typename U>
		constexpr Vec<3, T>& operator-=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator-=(const Vec<3, U> & v) noexcept;
		template<typename U>
		constexpr Vec<3, T>& operator*=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator*=(const Vec<3, U> & v) noexcept;
		template<typename U>
		constexpr Vec<3, T>& operator/=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator/=(const Vec<3, U> & v) noexcept;

		//Increment and decrement operators
		constexpr Vec<3, T>& operator++() noexcept;
		constexpr Vec<3, T>& operator--() noexcept;
		constexpr const Vec<3, T> operator++(int) noexcept;
		constexpr const Vec<3, T> operator--(int) noexcept;

		//Unary bit operators
		template<typename U>
		constexpr Vec<3, T>& operator%=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator%=(const Vec<3, U> & v) noexcept;
		template<typename U>
		constexpr Vec<3, T>& operator&=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator&=(const Vec<3, U> & v) noexcept;
		template<typename U>
		constexpr Vec<3, T>& operator|=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator|=(const Vec<3, U> & v) noexcept;
		template<typename U>
		constexpr Vec<3, T>& operator^=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator^=(const Vec<3, U> & v) noexcept;
		template<typename U>
		constexpr Vec<3, T>& operator<<=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator<<=(const Vec<3, U> & v) noexcept;
		template<typename U>
		constexpr Vec<3, T>& operator>>=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<3, T>& operator>>=(const Vec<3, U> & v) noexcept;

		[[nodiscard]] std::string ToString() const;
	};

	//Unary operators
	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator+(const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator-(const Vec<3, T>& v) noexcept;

	//Binary operators
	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator+(const Vec<3, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator+(T scalar, const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator+(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator-(const Vec<3, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator-(T scalar, const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator-(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator*(const Vec<3, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator*(T scalar, const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator*(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator/(const Vec<3, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator/(T scalar, const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator/(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator%(const Vec<3, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator%(T scalar, const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator%(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator&(const Vec<3, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator&(T scalar, const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator&(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator|(const Vec<3, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator|(T scalar, const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator|(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator^(const Vec<3, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator^(T scalar, const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator^(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator<<(const Vec<3, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator<<(T scalar, const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator<<(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator>>(const Vec<3, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator>>(T scalar, const Vec<3, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator>>(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<3, T> operator~(const Vec<3, T>& v) noexcept;

	//Boolean operators
	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr bool operator==(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr bool operator!=(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept;

	constexpr Vec<3, bool> operator&&(const Vec<3, bool>& v1, const Vec<3, bool>& v2) noexcept;

	constexpr Vec<3, bool> operator||(const Vec<3, bool>& v1, const Vec<3, bool>& v2) noexcept;
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	requires std::is_arithmetic_v<T>
	struct hash<TRAP::Math::Vec<3, T>>
	{
		[[nodiscard]] constexpr std::size_t operator()(const TRAP::Math::Vec<3, T>& v) const noexcept
		{
			std::size_t seed = 0;
			hash<T> hasher;
			TRAP::Utils::HashCombine(seed, hasher(v.x), hasher(v.y), hasher(v.z));
			return seed;
		}
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//Explicit basic constructors
template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T>::Vec(const T scalar) noexcept
	: x(scalar), y(scalar), z(scalar)
{}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T>::Vec(T x_, T y_, T z_) noexcept
	: x(x_), y(y_), z(z_)
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion scalar constructors

template<typename T>
requires std::is_arithmetic_v<T>
template<typename X, typename Y, typename Z>
constexpr TRAP::Math::Vec<3, T>::Vec(const X x_, const Y y_, const Z z_) noexcept
	: x(static_cast<T>(x_)),
	  y(static_cast<T>(y_)),
	  z(static_cast<T>(z_))
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion vector constructors

template<typename T>
requires std::is_arithmetic_v<T>
template<typename A, typename B>
requires std::is_arithmetic_v<A>
constexpr TRAP::Math::Vec<3, T>::Vec(const Vec<2, A>& xy, const B z_) noexcept
	: x(static_cast<T>(xy.x)),
	  y(static_cast<T>(xy.y)),
	  z(static_cast<T>(z_))
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename A, typename B>
requires std::is_arithmetic_v<B>
constexpr TRAP::Math::Vec<3, T>::Vec(const A x_, const Vec<2, B>& yz) noexcept
	: x(static_cast<T>(x_)),
	  y(static_cast<T>(yz.x)),
	  z(static_cast<T>(yz.y))
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>::Vec(const Vec<3, U>& v) noexcept
	: x(static_cast<T>(v.x)),
	  y(static_cast<T>(v.y)),
	  z(static_cast<T>(v.z))
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>::Vec(const Vec<4, U>& v) noexcept
	: x(static_cast<T>(v.x)),
	  y(static_cast<T>(v.y)),
	  z(static_cast<T>(v.z))
{}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr std::size_t TRAP::Math::Vec<3, T>::Length() noexcept
{
	return 3;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component accesses

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<3, T>::operator[](const std::size_t i)
{
	switch (i)
	{
	default:
		[[fallthrough]];
	case 0:
		return x;

	case 1:
		return y;

	case 2:
		return z;
	}
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<3, T>::operator[](const std::size_t i) const
{
	switch (i)
	{
	default:
		[[fallthrough]];
	case 0:
		return x;

	case 1:
		return y;

	case 2:
		return z;
	}
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] T& TRAP::Math::Vec<3, T>::at(const std::size_t i)
{
	TRAP_ASSERT(i < this->Length(), "Math::Vec<3, T>::operator[]: Index out of range!");

	switch (i)
	{
	default:
		[[fallthrough]];
	case 0:
		return x;

	case 1:
		return y;

	case 2:
		return z;
	}
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] const T& TRAP::Math::Vec<3, T>::at(const std::size_t i) const
{
	TRAP_ASSERT(i < this->Length(), "Math::Vec<3, T>::operator[]: Index out of range!");

	switch (i)
	{
	default:
		[[fallthrough]];
	case 0:
		return x;

	case 1:
		return y;

	case 2:
		return z;
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator=(const Vec<3, U>& v) noexcept
{
	this->x = static_cast<T>(v.x);
	this->y = static_cast<T>(v.y);
	this->z = static_cast<T>(v.z);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator+=(const U scalar) noexcept
{
	this->x += static_cast<T>(scalar);
	this->y += static_cast<T>(scalar);
	this->z += static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator+=(const Vec<3, U>& v) noexcept
{
	this->x += static_cast<T>(v.x);
	this->y += static_cast<T>(v.y);
	this->z += static_cast<T>(v.z);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator-=(const U scalar) noexcept
{
	this->x -= static_cast<T>(scalar);
	this->y -= static_cast<T>(scalar);
	this->z -= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator-=(const Vec<3, U>& v) noexcept
{
	this->x -= static_cast<T>(v.x);
	this->y -= static_cast<T>(v.y);
	this->z -= static_cast<T>(v.z);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator*=(const U scalar) noexcept
{
	this->x *= static_cast<T>(scalar);
	this->y *= static_cast<T>(scalar);
	this->z *= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator*=(const Vec<3, U>& v) noexcept
{
	this->x *= static_cast<T>(v.x);
	this->y *= static_cast<T>(v.y);
	this->z *= static_cast<T>(v.z);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator/=(const U scalar) noexcept
{
	this->x /= static_cast<T>(scalar);
	this->y /= static_cast<T>(scalar);
	this->z /= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator/=(const Vec<3, U>& v) noexcept
{
	this->x /= static_cast<T>(v.x);
	this->y /= static_cast<T>(v.y);
	this->z /= static_cast<T>(v.z);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator++() noexcept
{
	++this->x;
	++this->y;
	++this->z;

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator--() noexcept
{
	--this->x;
	--this->y;
	--this->z;

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr const TRAP::Math::Vec<3, T> TRAP::Math::Vec<3, T>::operator++(const int) noexcept
{
	Vec<3, T> result(*this);
	++*this;

	return result;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr const TRAP::Math::Vec<3, T> TRAP::Math::Vec<3, T>::operator--(const int) noexcept
{
	Vec<3, T> result(*this);
	--*this;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary bit operators

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator%=(const U scalar) noexcept
{
	this->x %= static_cast<T>(scalar);
	this->y %= static_cast<T>(scalar);
	this->z %= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator%=(const Vec<3, U>& v) noexcept
{
	this->x %= static_cast<T>(v.x);
	this->y %= static_cast<T>(v.y);
	this->z %= static_cast<T>(v.z);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator&=(const U scalar) noexcept
{
	this->x &= static_cast<T>(scalar);
	this->y &= static_cast<T>(scalar);
	this->z &= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator&=(const Vec<3, U>& v) noexcept
{
	this->x &= static_cast<T>(v.x);
	this->y &= static_cast<T>(v.y);
	this->z &= static_cast<T>(v.z);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator|=(const U scalar) noexcept
{
	this->x |= static_cast<T>(scalar);
	this->y |= static_cast<T>(scalar);
	this->z |= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator|=(const Vec<3, U>& v) noexcept
{
	this->x |= static_cast<T>(v.x);
	this->y |= static_cast<T>(v.y);
	this->z |= static_cast<T>(v.z);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator^=(const U scalar) noexcept
{
	this->x ^= static_cast<T>(scalar);
	this->y ^= static_cast<T>(scalar);
	this->z ^= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator^=(const Vec<3, U>& v) noexcept
{
	this->x ^= static_cast<T>(v.x);
	this->y ^= static_cast<T>(v.y);
	this->z ^= static_cast<T>(v.z);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator<<=(const U scalar) noexcept
{
	this->x <<= static_cast<T>(scalar);
	this->y <<= static_cast<T>(scalar);
	this->z <<= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator<<=(const Vec<3, U>& v) noexcept
{
	this->x <<= static_cast<T>(v.x);
	this->y <<= static_cast<T>(v.y);
	this->z <<= static_cast<T>(v.z);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator>>=(const U scalar) noexcept
{
	this->x >>= static_cast<T>(scalar);
	this->y >>= static_cast<T>(scalar);
	this->z >>= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator>>=(const Vec<3, U>& v) noexcept
{
	this->x >>= static_cast<T>(v.x);
	this->y >>= static_cast<T>(v.y);
	this->z >>= static_cast<T>(v.z);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] std::string TRAP::Math::Vec<3, T>::ToString() const
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if constexpr(std::same_as<T, float>)
		return fmt::format("Vec3f({}, {}, {})", x, y, z);
	else if constexpr(std::same_as<T, double>)
		return fmt::format("Vec3d({}, {}, {})", x, y, z);
	else if constexpr(std::same_as<T, bool>)
		return fmt::format("Vec3b({}, {}, {})", (x ? "true" : "false"), (y ? "true" : "false"), (z ? "true" : "false"));
	else if constexpr(std::same_as<T, int8_t>)
		return fmt::format("Vec3i8({}, {}, {})", x, y, z);
	else if constexpr(std::same_as<T, int16_t>)
		return fmt::format("Vec3i16({}, {}, {})", x, y, z);
	else if constexpr(std::same_as<T, int32_t>)
		return fmt::format("Vec3i32({}, {}, {})", x, y, z);
	else if constexpr(std::same_as<T, int64_t>)
		return fmt::format("Vec3i64({}, {}, {})", x, y, z);
	else if constexpr(std::same_as<T, uint8_t>)
		return fmt::format("Vec3ui8({}, {}, {})", x, y, z);
	else if constexpr(std::same_as<T, uint16_t>)
		return fmt::format("Vec3ui16({}, {}, {})", x, y, z);
	else if constexpr(std::same_as<T, uint32_t>)
		return fmt::format("Vec3ui32({}, {}, {})", x, y, z);
	else if constexpr(std::same_as<T, uint64_t>)
		return fmt::format("Vec3ui64({}, {}, {})", x, y, z);
	else
		return "Unknown type";
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(const Vec<3, T>& v) noexcept
{
	return v;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(-v.x, -v.y, -v.z);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x + scalar, v.y + scalar, v.z + scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar + v.x, scalar + v.y, scalar + v.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x - scalar, v.y - scalar, v.z - scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar - v.x, scalar - v.y, scalar - v.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x * scalar, v.y * scalar, v.z * scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar * v.x, scalar * v.y, scalar * v.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator/(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x / scalar, v.y / scalar, v.z / scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator/(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar / v.x, scalar / v.y, scalar / v.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator/(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary bit operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator%(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x % scalar, v.y % scalar, v.z % scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator%(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar % v.x, scalar % v.y, scalar % v.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator%(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator&(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x & scalar, v.y & scalar, v.z & scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator&(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar & v.x, scalar & v.y, scalar & v.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator&(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator|(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x | scalar, v.y | scalar, v.z | scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator|(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar | v.x, scalar | v.y, scalar | v.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator|(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator^(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x ^ scalar, v.y ^ scalar, v.z ^ scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator^(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar ^ v.x, scalar ^ v.y, scalar ^ v.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator^(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator<<(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x << scalar, v.y << scalar, v.z << scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator<<(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar << v.x, scalar << v.y, scalar << v.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator<<(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator>>(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x >> scalar, v.y >> scalar, v.z >> scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator>>(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar >> v.x, scalar >> v.y, scalar >> v.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator>>(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator~(const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(~v.x, ~v.y, ~v.z);
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr bool TRAP::Math::operator==(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr bool TRAP::Math::operator!=(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return !(v1 == v2);
}

constexpr TRAP::Math::Vec<3, bool> TRAP::Math::operator&&(const Vec<3, bool>& v1, const Vec<3, bool>& v2) noexcept
{
	return Vec<3, bool>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z);
}

constexpr TRAP::Math::Vec<3, bool> TRAP::Math::operator||(const Vec<3, bool>& v1, const Vec<3, bool>& v2) noexcept
{
	return Vec<3, bool>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z);
}

//-------------------------------------------------------------------------------------------------------------------//
//std::get support

namespace std
{
	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 3).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr T& get(TRAP::Math::Vec<3, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<3, T>::Length());

		switch (I)
		{
		default:
			[[fallthrough]];
		case 0:
			return v.x;

		case 1:
			return v.y;

		case 2:
			return v.z;
		}
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 3).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr T&& get(TRAP::Math::Vec<3, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<3, T>::Length());

		switch (I)
		{
		default:
			[[fallthrough]];
		case 0:
			return v.x;

		case 1:
			return v.y;

		case 2:
			return v.z;
		}
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 3).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const T& get(const TRAP::Math::Vec<3, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<3, T>::Length());

		switch (I)
		{
		default:
			[[fallthrough]];
		case 0:
			return v.x;

		case 1:
			return v.y;

		case 2:
			return v.z;
		}
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 3).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const T&& get(const TRAP::Math::Vec<3, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<3, T>::Length());

		switch (I)
		{
		default:
			[[fallthrough]];
		case 0:
			return v.x;

		case 1:
			return v.y;

		case 2:
			return v.z;
		}
	}
}

#endif /*TRAP_VEC3_H*/
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
	struct Vec<3, T>
	{
		//Implementation details
		using valueType = T;
		using type = Vec<3, T>;
		using boolType = Vec<3, bool>;

		//Data
		T x, y, z;

		//Implicit basic constructors
		constexpr Vec() = default;
		constexpr Vec(const Vec & v) = default;

		//Explicit basic constructors
		constexpr explicit Vec(T scalar);
		constexpr Vec(T x, T y, T z);

		//Explicit conversions
		template<typename X, typename Y, typename Z>
		constexpr Vec(X x, Y y, Z z);

		//Conversion vector constructors

		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(const Vec<2, A> & xy, B z);
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(A x, const Vec<2, B> & yz);
		//Explicit conversions
		template<typename U>
		constexpr explicit Vec(const Vec<4, U> & v);

		//Explicit conversions
		template<typename U>
		constexpr explicit Vec(const Vec<3, U> & v);

		constexpr Vec(Vec&&) = default;
		~Vec() = default;
		constexpr Vec<3, T>& operator=(Vec&&) = default;

		/// <summary>
		/// Retrieve the count of components of the vector.
		/// </summary>
		/// <returns>Count of components.</returns>
		static constexpr int Length();

		//Comoponent accesses
		constexpr T& operator[](int i);
		constexpr const T& operator[](int i) const;

		//Unary arithmetic operators
		constexpr Vec<3, T>& operator=(const Vec<3, T>& v) = default;

		template<typename U>
		constexpr Vec<3, T>& operator=(const Vec<3, U> & v);
		template<typename U>
		constexpr Vec<3, T>& operator+=(U scalar);
		template<typename U>
		constexpr Vec<3, T>& operator+=(const Vec<3, U> & v);
		template<typename U>
		constexpr Vec<3, T>& operator-=(U scalar);
		template<typename U>
		constexpr Vec<3, T>& operator-=(const Vec<3, U> & v);
		template<typename U>
		constexpr Vec<3, T>& operator*=(U scalar);
		template<typename U>
		constexpr Vec<3, T>& operator*=(const Vec<3, U> & v);
		template<typename U>
		constexpr Vec<3, T>& operator/=(U scalar);
		template<typename U>
		constexpr Vec<3, T>& operator/=(const Vec<3, U> & v);

		//Increment and decrement operators
		constexpr Vec<3, T>& operator++();
		constexpr Vec<3, T>& operator--();
		constexpr Vec<3, T> operator++(int);
		constexpr Vec<3, T> operator--(int);

		//Unary bit operators
		template<typename U>
		constexpr Vec<3, T>& operator%=(U scalar);
		template<typename U>
		constexpr Vec<3, T>& operator%=(const Vec<3, U> & v);
		template<typename U>
		constexpr Vec<3, T>& operator&=(U scalar);
		template<typename U>
		constexpr Vec<3, T>& operator&=(const Vec<3, U> & v);
		template<typename U>
		constexpr Vec<3, T>& operator|=(U scalar);
		template<typename U>
		constexpr Vec<3, T>& operator|=(const Vec<3, U> & v);
		template<typename U>
		constexpr Vec<3, T>& operator^=(U scalar);
		template<typename U>
		constexpr Vec<3, T>& operator^=(const Vec<3, U> & v);
		template<typename U>
		constexpr Vec<3, T>& operator<<=(U scalar);
		template<typename U>
		constexpr Vec<3, T>& operator<<=(const Vec<3, U> & v);
		template<typename U>
		constexpr Vec<3, T>& operator>>=(U scalar);
		template<typename U>
		constexpr Vec<3, T>& operator>>=(const Vec<3, U> & v);

		std::string ToString() const;
	};

	//Unary operators
	template<typename T>
	constexpr Vec<3, T> operator+(const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator-(const Vec<3, T>& v);

	//Binary operators
	template<typename T>
	constexpr Vec<3, T> operator+(const Vec<3, T>& v, T scalar);

	template<typename T>
	constexpr Vec<3, T> operator+(T scalar, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator+(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr Vec<3, T> operator-(const Vec<3, T>& v, T scalar);

	template<typename T>
	constexpr Vec<3, T> operator-(T scalar, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator-(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr Vec<3, T> operator*(const Vec<3, T>& v, T scalar);

	template<typename T>
	constexpr Vec<3, T> operator*(T scalar, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator*(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr Vec<3, T> operator/(const Vec<3, T>& v, T scalar);

	template<typename T>
	constexpr Vec<3, T> operator/(T scalar, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator/(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr Vec<3, T> operator%(const Vec<3, T>& v, T scalar);

	template<typename T>
	constexpr Vec<3, T> operator%(T scalar, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator%(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr Vec<3, T> operator&(const Vec<3, T>& v, T scalar);

	template<typename T>
	constexpr Vec<3, T> operator&(T scalar, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator&(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr Vec<3, T> operator|(const Vec<3, T>& v, T scalar);

	template<typename T>
	constexpr Vec<3, T> operator|(T scalar, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator|(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr Vec<3, T> operator^(const Vec<3, T>& v, T scalar);

	template<typename T>
	constexpr Vec<3, T> operator^(T scalar, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator^(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr Vec<3, T> operator<<(const Vec<3, T>& v, T scalar);

	template<typename T>
	constexpr Vec<3, T> operator<<(T scalar, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator<<(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr Vec<3, T> operator>>(const Vec<3, T>& v, T scalar);

	template<typename T>
	constexpr Vec<3, T> operator>>(T scalar, const Vec<3, T>& v);

	template<typename T>
	constexpr Vec<3, T> operator>>(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr Vec<3, T> operator~(const Vec<3, T>& v);

	//Boolean operators
	template<typename T>
	constexpr bool operator==(const Vec<3, T>& v1, const Vec<3, T>& v2);

	template<typename T>
	constexpr bool operator!=(const Vec<3, T>& v1, const Vec<3, T>& v2);

	constexpr Vec<3, bool> operator&&(const Vec<3, bool>& v1, const Vec<3, bool>& v2);

	constexpr Vec<3, bool> operator||(const Vec<3, bool>& v1, const Vec<3, bool>& v2);
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	struct hash<TRAP::Math::Vec<3, T>>
	{
		constexpr std::size_t operator()(const TRAP::Math::Vec<3, T>& v) const
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
constexpr TRAP::Math::Vec<3, T>::Vec(T scalar)
	: x(scalar), y(scalar), z(scalar)
{}

template<typename T>
constexpr TRAP::Math::Vec<3, T>::Vec(T x, T y, T z)
	: x(x), y(y), z(z)
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion scalar constructors

template<typename T>
template<typename X, typename Y, typename Z>
constexpr TRAP::Math::Vec<3, T>::Vec(X x, Y y, Z z)
	: x(static_cast<T>(x)),
	  y(static_cast<T>(y)),
	  z(static_cast<T>(z))
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion vector constructors

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<3, T>::Vec(const Vec<2, A>& xy, B z)
	: x(static_cast<T>(xy.x)),
	  y(static_cast<T>(xy.y)),
	  z(static_cast<T>(z))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<3, T>::Vec(A x, const Vec<2, B>& yz)
	: x(static_cast<T>(x)),
	  y(static_cast<T>(yz.x)),
	  z(static_cast<T>(yz.y))
{}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>::Vec(const Vec<3, U>& v)
	: x(static_cast<T>(v.x)),
	  y(static_cast<T>(v.y)),
	  z(static_cast<T>(v.z))
{}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>::Vec(const Vec<4, U>& v)
	: x(static_cast<T>(v.x)),
	  y(static_cast<T>(v.y)),
	  z(static_cast<T>(v.z))
{}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
constexpr int TRAP::Math::Vec<3, T>::Length()
{
	return 3;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component accesses

template<typename T>
constexpr T& TRAP::Math::Vec<3, T>::operator[](int i)
{
	TRAP_ASSERT(i >= 0 && i < this->Length());

	switch (i)
	{
	default:
	case 0:
		return x;

	case 1:
		return y;

	case 2:
		return z;
	}
}

template<typename T>
constexpr const T& TRAP::Math::Vec<3, T>::operator[](int i) const
{
	TRAP_ASSERT(i >= 0 && i < this->Length());

	switch (i)
	{
	default:
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
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator=(const Vec<3, U>& v)
{
	this->x = static_cast<T>(v.x);
	this->y = static_cast<T>(v.y);
	this->z = static_cast<T>(v.z);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator+=(U scalar)
{
	this->x += static_cast<T>(scalar);
	this->y += static_cast<T>(scalar);
	this->z += static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator+=(const Vec<3, U>& v)
{
	this->x += static_cast<T>(v.x);
	this->y += static_cast<T>(v.y);
	this->z += static_cast<T>(v.z);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator-=(U scalar)
{
	this->x -= static_cast<T>(scalar);
	this->y -= static_cast<T>(scalar);
	this->z -= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator-=(const Vec<3, U>& v)
{
	this->x -= static_cast<T>(v.x);
	this->y -= static_cast<T>(v.y);
	this->z -= static_cast<T>(v.z);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator*=(U scalar)
{
	this->x *= static_cast<T>(scalar);
	this->y *= static_cast<T>(scalar);
	this->z *= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator*=(const Vec<3, U>& v)
{
	this->x *= static_cast<T>(v.x);
	this->y *= static_cast<T>(v.y);
	this->z *= static_cast<T>(v.z);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator/=(U scalar)
{
	this->x /= static_cast<T>(scalar);
	this->y /= static_cast<T>(scalar);
	this->z /= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator/=(const Vec<3, U>& v)
{
	this->x /= static_cast<T>(v.x);
	this->y /= static_cast<T>(v.y);
	this->z /= static_cast<T>(v.z);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator++()
{
	++this->x;
	++this->y;
	++this->z;

	return *this;
}

template<typename T>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator--()
{
	--this->x;
	--this->y;
	--this->z;

	return *this;
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Vec<3, T>::operator++(int)
{
	Vec<3, T> result(*this);
	++*this;

	return result;
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Vec<3, T>::operator--(int)
{
	Vec<3, T> result(*this);
	--*this;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary bit operators

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator%=(U scalar)
{
	this->x %= static_cast<T>(scalar);
	this->y %= static_cast<T>(scalar);
	this->z %= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator%=(const Vec<3, U>& v)
{
	this->x %= static_cast<T>(v.x);
	this->y %= static_cast<T>(v.y);
	this->z %= static_cast<T>(v.z);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator&=(U scalar)
{
	this->x &= static_cast<T>(scalar);
	this->y &= static_cast<T>(scalar);
	this->z &= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator&=(const Vec<3, U>& v)
{
	this->x &= static_cast<T>(v.x);
	this->y &= static_cast<T>(v.y);
	this->z &= static_cast<T>(v.z);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator|=(U scalar)
{
	this->x |= static_cast<T>(scalar);
	this->y |= static_cast<T>(scalar);
	this->z |= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator|=(const Vec<3, U>& v)
{
	this->x |= static_cast<T>(v.x);
	this->y |= static_cast<T>(v.y);
	this->z |= static_cast<T>(v.z);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator^=(U scalar)
{
	this->x ^= static_cast<T>(scalar);
	this->y ^= static_cast<T>(scalar);
	this->z ^= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator^=(const Vec<3, U>& v)
{
	this->x ^= static_cast<T>(v.x);
	this->y ^= static_cast<T>(v.y);
	this->z ^= static_cast<T>(v.z);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator<<=(U scalar)
{
	this->x <<= static_cast<T>(scalar);
	this->y <<= static_cast<T>(scalar);
	this->z <<= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator<<=(const Vec<3, U>& v)
{
	this->x <<= static_cast<T>(v.x);
	this->y <<= static_cast<T>(v.y);
	this->z <<= static_cast<T>(v.z);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator>>=(U scalar)
{
	this->x >>= static_cast<T>(scalar);
	this->y >>= static_cast<T>(scalar);
	this->z >>= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator>>=(const Vec<3, U>& v)
{
	this->x >>= static_cast<T>(v.x);
	this->y >>= static_cast<T>(v.y);
	this->z >>= static_cast<T>(v.z);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
std::string TRAP::Math::Vec<3, T>::ToString() const
{
	if constexpr(std::is_same_v<T, float>)
		return "Vec3f(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	else if constexpr(std::is_same_v<T, double>)
		return "Vec3d(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	else if constexpr(std::is_same_v<T, bool>)
		return std::string("Vec3b(") + (x ? "true" : "false") + ", " + (y ? "true" : "false") + ", " + (z ? "true" : "false") + ")";
	else if constexpr(std::is_same_v<T, int8_t>)
		return "Vec3i8(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	else if constexpr(std::is_same_v<T, int16_t>)
		return "Vec3i16(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	else if constexpr(std::is_same_v<T, int32_t>)
		return "Vec3i32(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	else if constexpr(std::is_same_v<T, int64_t>)
		return "Vec3i64(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	else if constexpr(std::is_same_v<T, uint8_t>)
		return "Vec3ui8(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	else if constexpr(std::is_same_v<T, uint16_t>)
		return "Vec3ui16(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	else if constexpr(std::is_same_v<T, uint32_t>)
		return "Vec3ui32(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	else if constexpr(std::is_same_v<T, uint64_t>)
		return "Vec3ui64(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	else
		return "Unknown type";
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(const Vec<3, T>& v)
{
	return v;
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(const Vec<3, T>& v)
{
	return Vec<3, T>(-v.x, -v.y, -v.z);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(const Vec<3, T>& v, T scalar)
{
	return Vec<3, T>(v.x + scalar, v.y + scalar, v.z + scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(T scalar, const Vec<3, T>& v)
{
	return Vec<3, T>(scalar + v.x, scalar + v.y, scalar + v.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return Vec<3, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(const Vec<3, T>& v, T scalar)
{
	return Vec<3, T>(v.x - scalar, v.y - scalar, v.z - scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(T scalar, const Vec<3, T>& v)
{
	return Vec<3, T>(scalar - v.x, scalar - v.y, scalar - v.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return Vec<3, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const Vec<3, T>& v, T scalar)
{
	return Vec<3, T>(v.x * scalar, v.y * scalar, v.z * scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(T scalar, const Vec<3, T>& v)
{
	return Vec<3, T>(scalar * v.x, scalar * v.y, scalar * v.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return Vec<3, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator/(const Vec<3, T>& v, T scalar)
{
	return Vec<3, T>(v.x / scalar, v.y / scalar, v.z / scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator/(T scalar, const Vec<3, T>& v)
{
	return Vec<3, T>(scalar / v.x, scalar / v.y, scalar / v.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator/(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return Vec<3, T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary bit operators

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator%(const Vec<3, T>& v, T scalar)
{
	return Vec<3, T>(v.x % scalar, v.y % scalar, v.z % scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator%(T scalar, const Vec<3, T>& v)
{
	return Vec<3, T>(scalar % v.x, scalar % v.y, scalar % v.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator%(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return Vec<3, T>(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator&(const Vec<3, T>& v, T scalar)
{
	return Vec<3, T>(v.x & scalar, v.y & scalar, v.z & scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator&(T scalar, const Vec<3, T>& v)
{
	return Vec<3, T>(scalar & v.x, scalar & v.y, scalar & v.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator&(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return Vec<3, T>(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator|(const Vec<3, T>& v, T scalar)
{
	return Vec<3, T>(v.x | scalar, v.y | scalar, v.z | scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator|(T scalar, const Vec<3, T>& v)
{
	return Vec<3, T>(scalar | v.x, scalar | v.y, scalar | v.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator|(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return Vec<3, T>(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator^(const Vec<3, T>& v, T scalar)
{
	return Vec<3, T>(v.x ^ scalar, v.y ^ scalar, v.z ^ scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator^(T scalar, const Vec<3, T>& v)
{
	return Vec<3, T>(scalar ^ v.x, scalar ^ v.y, scalar ^ v.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator^(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return Vec<3, T>(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator<<(const Vec<3, T>& v, T scalar)
{
	return Vec<3, T>(v.x << scalar, v.y << scalar, v.z << scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator<<(T scalar, const Vec<3, T>& v)
{
	return Vec<3, T>(scalar << v.x, scalar << v.y, scalar << v.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator<<(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return Vec<3, T>(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator>>(const Vec<3, T>& v, T scalar)
{
	return Vec<3, T>(v.x >> scalar, v.y >> scalar, v.z >> scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator>>(T scalar, const Vec<3, T>& v)
{
	return Vec<3, T>(scalar >> v.x, scalar >> v.y, scalar >> v.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator>>(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return Vec<3, T>(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z);
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator~(const Vec<3, T>& v)
{
	return Vec<3, T>(~v.x, ~v.y, ~v.z);
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
constexpr bool TRAP::Math::operator==(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

template<typename T>
constexpr bool TRAP::Math::operator!=(const Vec<3, T>& v1, const Vec<3, T>& v2)
{
	return !(v1 == v2);
}

constexpr TRAP::Math::Vec<3, bool> TRAP::Math::operator&&(const Vec<3, bool>& v1, const Vec<3, bool>& v2)
{
	return Vec<3, bool>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z);
}

constexpr TRAP::Math::Vec<3, bool> TRAP::Math::operator||(const Vec<3, bool>& v1, const Vec<3, bool>& v2)
{
	return Vec<3, bool>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z);
}

#endif /*TRAP_VEC3_H*/
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

#ifndef TRAP_VEC4_H
#define TRAP_VEC4_H

#include "Types.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Utils/Utils.h"

namespace TRAP::Math
{
	/// <summary>
	/// Struct for a 4-dimensional vector.
	/// </summary>
	template<typename T>
	struct Vec<4, T>
	{
		//Implementation details
		using valueType = T;
		using type = Vec<4, T>;
		using boolType = Vec<4, bool>;

		//Data
		T x, y, z, w;

		//Implicit basic constructors
		constexpr Vec() = default;
		constexpr Vec(const Vec<4, T> & v) = default;

		//Explicit basic constructors
		explicit constexpr Vec(T scalar);
		constexpr Vec(T x, T y, T z, T w);

		//Explicit conversions
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(X x, Y y, Z z, W w);

		//Conversion vector constructors

		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(const Vec<2, A> & xy, B z, C w);
		//Explicit conversions
		template<typename A, typename B, typename C>
		explicit constexpr Vec(A x, const Vec<2, B> & yz, C w);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(A x, B y, const Vec<2, C> & zw);
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(const Vec<3, A> & xyz, B w);
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(A x, const Vec<3, B> & yzw);
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(const Vec<2, A> & xy, const Vec<2, B> & zw);

		//Explicit conversions
		template<typename U>
		explicit constexpr Vec(const Vec<4, U> & v);

		constexpr Vec(Vec&&) = default;
		~Vec() = default;
		constexpr Vec<4, T>& operator=(Vec&&) = default;

		/// <summary>
		/// Retrieve the count of components of the vector.
		/// </summary>
		/// <returns>Count of components.</returns>
		static constexpr int Length();

		//Comoponent accesses
		constexpr T& operator[](int i);
		constexpr const T& operator[](int i) const;

		//Unary arithmetic operators
		constexpr Vec<4, T>& operator=(const Vec<4, T> & v) = default;

		template<typename U>
		constexpr Vec<4, T>& operator=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator+=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator+=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator-=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator-=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator*=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator*=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator/=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator/=(const Vec<4, U> & v);

		//Increment and decrement operators
		constexpr Vec<4, T>& operator++();
		constexpr Vec<4, T>& operator--();
		constexpr Vec<4, T> operator++(int);
		constexpr Vec<4, T> operator--(int);

		//Unary bit operators
		template<typename U>
		constexpr Vec<4, T>& operator%=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator%=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator&=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator&=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator|=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator|=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator^=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator^=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator<<=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator<<=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator>>=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator>>=(const Vec<4, U> & v);

		std::string ToString() const;
	};

	//Unary operators
	template<typename T>
	constexpr Vec<4, T> operator+(const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v);

	//Binary operators
	template<typename T>
	constexpr Vec<4, T> operator+(const Vec<4, T>& v, const T& scalar);

	template<typename T>
	constexpr Vec<4, T> operator+(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator+(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v, const T& scalar);

	template<typename T>
	constexpr Vec<4, T> operator-(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v, const T& scalar);

	template<typename T>
	constexpr Vec<4, T> operator*(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator/(const Vec<4, T>& v, const T& scalar);

	template<typename T>
	constexpr Vec<4, T> operator/(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator/(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator%(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator%(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator%(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator&(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator&(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator&(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator|(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator|(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator|(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator^(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator^(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator^(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator<<(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator<<(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator<<(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator>>(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator>>(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator>>(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator~(const Vec<4, T>& v);

	//Boolean operators
	template<typename T>
	constexpr bool operator==(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr bool operator!=(const Vec<4, T>& v1, const Vec<4, T>& v2);

	constexpr Vec<4, bool> operator&&(const Vec<4, bool>& v1, const Vec<4, bool>& v2);

	constexpr Vec<4, bool> operator||(const Vec<4, bool>& v1, const Vec<4, bool>& v2);
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	struct hash<TRAP::Math::Vec<4, T>>
	{
		constexpr std::size_t operator()(const TRAP::Math::Vec<4, T>& v) const
		{
			std::size_t seed = 0;
			hash<T> hasher;
			TRAP::Utils::HashCombine(seed, hasher(v.x), hasher(v.y), hasher(v.z), hasher(v.w));
			return seed;
		}
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//Explicit basic constructors
template<typename T>
constexpr TRAP::Math::Vec<4, T>::Vec(T scalar)
	: x(scalar), y(scalar), z(scalar), w(scalar)
{}

template<typename T>
constexpr TRAP::Math::Vec<4, T>::Vec(T x, T y, T z, T w)
	: x(x), y(y), z(z), w(w)
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion scalar constructors

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(X x, Y y, Z z, W w)
	: x(static_cast<T>(x)),
	  y(static_cast<T>(y)),
	  z(static_cast<T>(z)),
	  w(static_cast<T>(w))
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion vector constructors

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<2, A>& xy, B z, C w)
	: x(static_cast<T>(xy.x)),
	  y(static_cast<T>(xy.y)),
	  z(static_cast<T>(z)),
	  w(static_cast<T>(w))
{}

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(A x, const Vec<2, B>& yz, C w)
	: x(static_cast<T>(x)),
	  y(static_cast<T>(yz.x)),
	  z(static_cast<T>(yz.y)),
	  w(static_cast<T>(w))
{}

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(A x, B y, const Vec<2, C>& zw)
	: x(static_cast<T>(x)),
	  y(static_cast<T>(y)),
	  z(static_cast<T>(zw.x)),
	  w(static_cast<T>(zw.y))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<3, A>& xyz, B w)
	: x(static_cast<T>(xyz.x)),
	  y(static_cast<T>(xyz.y)),
	  z(static_cast<T>(xyz.z)),
	  w(static_cast<T>(w))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<4, T>::Vec(A x, const Vec<3, B>& yzw)
	: x(static_cast<T>(x)),
	  y(static_cast<T>(yzw.x)),
	  z(static_cast<T>(yzw.y)),
	  w(static_cast<T>(yzw.z))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<2, A>& xy, const Vec<2, B>& zw)
	: x(static_cast<T>(xy.x)),
	  y(static_cast<T>(xy.y)),
	  z(static_cast<T>(zw.x)),
	  w(static_cast<T>(zw.y))
{}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<4, U>& v)
	: x(static_cast<T>(v.x)),
	  y(static_cast<T>(v.y)),
	  z(static_cast<T>(v.z)),
	  w(static_cast<T>(v.w))
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr int TRAP::Math::Vec<4, T>::Length()
{
	return 4;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component accesses
template<typename T>
constexpr T& TRAP::Math::Vec<4, T>::operator[](int i)
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

	case 3:
		return w;
	}
}

template<typename T>
constexpr const T& TRAP::Math::Vec<4, T>::operator[](int i) const
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

	case 3:
		return w;
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator=(const Vec<4, U>& v)
{
	this->x = static_cast<T>(v.x);
	this->y = static_cast<T>(v.y);
	this->z = static_cast<T>(v.z);
	this->w = static_cast<T>(v.w);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator+=(U scalar)
{
	return (*this = Vec<4, T>(this->x + scalar, this->y + scalar, this->z + scalar, this->w + scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator+=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator-=(U scalar)
{
	return (*this = Vec<4, T>(this->x - scalar, this->y - scalar, this->z - scalar, this->w - scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator-=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator*=(U scalar)
{
	return (*this = Vec<4, T>(this->x * scalar, this->y * scalar, this->z * scalar, this->w * scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator*=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator/=(U scalar)
{
	return (*this = Vec<4, T>(this->x / scalar, this->y / scalar, this->z / scalar, this->w / scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator/=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x / v.x, this->y / v.y, this->z / v.z, this->w / v.w));
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator++()
{
	++this->x;
	++this->y;
	++this->z;
	++this->w;

	return *this;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator--()
{
	--this->x;
	--this->y;
	--this->z;
	--this->w;

	return *this;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Vec<4, T>::operator++(int)
{
	Vec<4, T> Result(*this);
	++*this;

	return Result;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Vec<4, T>::operator--(int)
{
	Vec<4, T> Result(*this);
	--*this;

	return Result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary bit operators

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator%=(U scalar)
{
	return (*this = Vec<4, T>(this->x % scalar, this->y % scalar, this->z % scalar, this->w % scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator%=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x % v.x, this->y % v.y, this->z % v.z, this->w % v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator&=(U scalar)
{
	return (*this = Vec<4, T>(this->x & scalar, this->y & scalar, this->z & scalar, this->w & scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator&=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x & v.x, this->y & v.y, this->z & v.z, this->w & v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator|=(U scalar)
{
	return (*this = Vec<4, T>(this->x | scalar, this->y | scalar, this->z | scalar, this->w | scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator|=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x | v.x, this->y | v.y, this->z | v.z, this->w | v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator^=(U scalar)
{
	return (*this = Vec<4, T>(this->x ^ scalar, this->y ^ scalar, this->z ^ scalar, this->w ^ scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator^=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x ^ v.x, this->y ^ v.y, this->z ^ v.z, this->w ^ v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator<<=(U scalar)
{
	return (*this = Vec<4, T>(this->x << scalar, this->y << scalar, this->z << scalar, this->w << scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator<<=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x << v.x, this->y << v.y, this->z << v.z, this->w << v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator>>=(U scalar)
{
	return (*this = Vec<4, T>(this->x >> scalar, this->y >> scalar, this->z >> scalar, this->w >> scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator>>=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x >> v.x, this->y >> v.y, this->z >> v.z, this->w >> v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
std::string TRAP::Math::Vec<4, T>::ToString() const
{
	if constexpr(std::is_same_v<T, float>)
		return "Vec4f(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +
			", " + std::to_string(w) + ")";
	else if constexpr(std::is_same_v<T, double>)
		return "Vec4d(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +
			", " + std::to_string(w) + ")";
	else if constexpr(std::is_same_v<T, bool>)
		return std::string("Vec4b(") + (x ? "true" : "false") + ", " + (y ? "true" : "false") + ", " + (z ? "true" : "false") + ", " + (w ? "true" : "false") + ")";
	else if constexpr(std::is_same_v<T, int8_t>)
		return "Vec4i8(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +
			", " + std::to_string(w) + ")";
	else if constexpr(std::is_same_v<T, int16_t>)
		return "Vec4i16(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +
			", " + std::to_string(w) + ")";
	else if constexpr(std::is_same_v<T, int32_t>)
		return "Vec4i32(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +
			", " + std::to_string(w) + ")";
	else if constexpr(std::is_same_v<T, int64_t>)
		return "Vec4i64(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +
			", " + std::to_string(w) + ")";
	else if constexpr(std::is_same_v<T, uint8_t>)
		return "Vec4ui8(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +
			", " + std::to_string(w) + ")";
	else if constexpr(std::is_same_v<T, uint16_t>)
		return "Vec4ui16(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +
			", " + std::to_string(w) + ")";
	else if constexpr(std::is_same_v<T, uint32_t>)
		return "Vec4ui32(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +
			", " + std::to_string(w) + ")";
	else if constexpr(std::is_same_v<T, uint64_t>)
		return "Vec4ui64(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +
			", " + std::to_string(w) + ")";
	else
		return "Unknown type";
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary constant operators

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<4, T>& v)
{
	return v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<4, T>& v)
{
	return Vec<4, T>(0) -= v;
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<4, T>& v, const T& scalar)
{
	return Vec<4, T>(v) += scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(v) += scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1) += v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<4, T>& v, const T& scalar)
{
	return Vec<4, T>(v) -= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) -= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1) -= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<4, T>& v, const T& scalar)
{
	return Vec<4, T>(v) *= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(v) *= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1) *= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(const Vec<4, T>& v, const T& scalar)
{
	return Vec<4, T>(v) /= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) /= v;
}


template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1) /= v2;
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary bit operators

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(const Vec<4, T>& v, T scalar)
{
	return Vec<4, T>(v) %= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) %= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1) %= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(const Vec<4, T>& v, T scalar)
{
	return Vec<4, T>(v) &= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) &= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1) &= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(const Vec<4, T>& v, T scalar)
{
	return Vec<4, T>(v) |= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) |= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1) |= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(const Vec<4, T>& v, T scalar)
{
	return Vec<4, T>(v) ^= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) ^= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1) ^= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(const Vec<4, T>& v, T scalar)
{
	return Vec<4, T>(v) <<= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) <<= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1) <<= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(const Vec<4, T>& v, T scalar)
{
	return Vec<4, T>(v) >>= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) >>= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1) >>= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator~(const Vec<4, T>& v)
{
	return Vec<4, T>(~v.x, ~v.y, ~v.z, ~v.w);
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
constexpr bool TRAP::Math::operator==(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

template<typename T>
constexpr bool TRAP::Math::operator!=(const Vec<4, T>& v1, const Vec<4, T>& v2)
{
	return !(v1 == v2);
}

constexpr TRAP::Math::Vec<4, bool> TRAP::Math::operator&&(const Vec<4, bool>& v1, const Vec<4, bool>& v2)
{
	return Vec<4, bool>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
}

constexpr TRAP::Math::Vec<4, bool> TRAP::Math::operator||(const Vec<4, bool>& v1, const Vec<4, bool>& v2)
{
	return Vec<4, bool>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
}

#endif /*TRAP_VEC4_H*/
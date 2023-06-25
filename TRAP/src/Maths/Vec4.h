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
		constexpr Vec() noexcept = default;
		constexpr Vec(const Vec<4, T> & v) noexcept = default;

		//Explicit basic constructors
		explicit constexpr Vec(T scalar) noexcept;
		constexpr Vec(T x_, T y_, T z_, T w_) noexcept;

		//Explicit conversions
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(X x_, Y y_, Z z_, W w_) noexcept;

		//Conversion vector constructors

		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(const Vec<2, A> & xy, B z_, C w_) noexcept;
		//Explicit conversions
		template<typename A, typename B, typename C>
		explicit constexpr Vec(A x_, const Vec<2, B> & yz, C w_) noexcept;
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(A x_, B y_, const Vec<2, C> & zw) noexcept;
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(const Vec<3, A> & xyz, B w_) noexcept;
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(A x_, const Vec<3, B> & yzw) noexcept;
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(const Vec<2, A> & xy, const Vec<2, B> & zw) noexcept;

		//Explicit conversions
		template<typename U>
		explicit constexpr Vec(const Vec<4, U> & v) noexcept;

		constexpr Vec(Vec&&) noexcept = default;
		constexpr ~Vec() = default;
		constexpr Vec<4, T>& operator=(Vec&&) noexcept = default;

		/// <summary>
		/// Retrieve the count of components of the vector.
		/// </summary>
		/// <returns>Count of components.</returns>
		[[nodiscard]] static constexpr std::size_t Length() noexcept;

		//Component accesses
		[[nodiscard]] constexpr T& operator[](std::size_t i);
		[[nodiscard]] constexpr const T& operator[](std::size_t i) const;

		//Unary arithmetic operators
		constexpr Vec<4, T>& operator=(const Vec<4, T> & v) noexcept = default;

		template<typename U>
		constexpr Vec<4, T>& operator=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator+=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator+=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator-=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator-=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator*=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator*=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator/=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator/=(const Vec<4, U> & v) noexcept;

		//Increment and decrement operators
		constexpr Vec<4, T>& operator++() noexcept;
		constexpr Vec<4, T>& operator--() noexcept;
		constexpr const Vec<4, T> operator++(int) noexcept;
		constexpr const Vec<4, T> operator--(int) noexcept;

		//Unary bit operators
		template<typename U>
		constexpr Vec<4, T>& operator%=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator%=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator&=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator&=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator|=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator|=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator^=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator^=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator<<=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator<<=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator>>=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator>>=(const Vec<4, U> & v) noexcept;

		[[nodiscard]] std::string ToString() const;
	};

	//Unary operators
	template<typename T>
	constexpr Vec<4, T> operator+(const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v) noexcept;

	//Binary operators
	template<typename T>
	constexpr Vec<4, T> operator+(const Vec<4, T>& v, const T& scalar) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator+(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator+(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v, const T& scalar) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator-(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v, const T& scalar) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator*(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator/(const Vec<4, T>& v, const T& scalar) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator/(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator/(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator%(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator%(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator%(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator&(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator&(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator&(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator|(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator|(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator|(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator^(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator^(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator^(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator<<(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator<<(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator<<(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator>>(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator>>(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator>>(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<4, T> operator~(const Vec<4, T>& v) noexcept;

	//Boolean operators
	template<typename T>
	constexpr bool operator==(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	constexpr bool operator!=(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	constexpr Vec<4, bool> operator&&(const Vec<4, bool>& v1, const Vec<4, bool>& v2) noexcept;

	constexpr Vec<4, bool> operator||(const Vec<4, bool>& v1, const Vec<4, bool>& v2) noexcept;
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	struct hash<TRAP::Math::Vec<4, T>>
	{
		constexpr std::size_t operator()(const TRAP::Math::Vec<4, T>& v) const noexcept
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
constexpr TRAP::Math::Vec<4, T>::Vec(const T scalar) noexcept
	: x(scalar), y(scalar), z(scalar), w(scalar)
{}

template<typename T>
constexpr TRAP::Math::Vec<4, T>::Vec(T x_, T y_, T z_, T w_) noexcept
	: x(x_), y(y_), z(z_), w(w_)
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion scalar constructors

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(const X x_, const Y y_, const Z z_, const W w_) noexcept
	: x(static_cast<T>(x_)),
	  y(static_cast<T>(y_)),
	  z(static_cast<T>(z_)),
	  w(static_cast<T>(w_))
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion vector constructors

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<2, A>& xy, const B z_, const C w_) noexcept
	: x(static_cast<T>(xy.x)),
	  y(static_cast<T>(xy.y)),
	  z(static_cast<T>(z_)),
	  w(static_cast<T>(w_))
{}

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(const A x_, const Vec<2, B>& yz, const C w_) noexcept
	: x(static_cast<T>(x_)),
	  y(static_cast<T>(yz.x)),
	  z(static_cast<T>(yz.y)),
	  w(static_cast<T>(w_))
{}

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(const A x_, const B y_, const Vec<2, C>& zw) noexcept
	: x(static_cast<T>(x_)),
	  y(static_cast<T>(y_)),
	  z(static_cast<T>(zw.x)),
	  w(static_cast<T>(zw.y))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<3, A>& xyz, const B w_) noexcept
	: x(static_cast<T>(xyz.x)),
	  y(static_cast<T>(xyz.y)),
	  z(static_cast<T>(xyz.z)),
	  w(static_cast<T>(w_))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<4, T>::Vec(const A x_, const Vec<3, B>& yzw) noexcept
	: x(static_cast<T>(x_)),
	  y(static_cast<T>(yzw.x)),
	  z(static_cast<T>(yzw.y)),
	  w(static_cast<T>(yzw.z))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<2, A>& xy, const Vec<2, B>& zw) noexcept
	: x(static_cast<T>(xy.x)),
	  y(static_cast<T>(xy.y)),
	  z(static_cast<T>(zw.x)),
	  w(static_cast<T>(zw.y))
{}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<4, U>& v) noexcept
	: x(static_cast<T>(v.x)),
	  y(static_cast<T>(v.y)),
	  z(static_cast<T>(v.z)),
	  w(static_cast<T>(v.w))
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
[[nodiscard]] constexpr std::size_t TRAP::Math::Vec<4, T>::Length() noexcept
{
	return 4;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component accesses
template<typename T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<4, T>::operator[](const std::size_t i)
{
	TRAP_ASSERT(i < this->Length(), "Math::Vec<4, T>::operator[]: index out of range!");

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

	case 3:
		return w;
	}
}

template<typename T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<4, T>::operator[](const std::size_t i) const
{
	TRAP_ASSERT(i < this->Length(), "Math::Vec<4, T>::operator[]: index out of range!");

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

	case 3:
		return w;
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator=(const Vec<4, U>& v) noexcept
{
	this->x = static_cast<T>(v.x);
	this->y = static_cast<T>(v.y);
	this->z = static_cast<T>(v.z);
	this->w = static_cast<T>(v.w);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator+=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x + scalar, this->y + scalar, this->z + scalar, this->w + scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator+=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator-=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x - scalar, this->y - scalar, this->z - scalar, this->w - scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator-=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator*=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x * scalar, this->y * scalar, this->z * scalar, this->w * scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator*=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator/=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x / scalar, this->y / scalar, this->z / scalar, this->w / scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator/=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x / v.x, this->y / v.y, this->z / v.z, this->w / v.w));
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator++() noexcept
{
	++this->x;
	++this->y;
	++this->z;
	++this->w;

	return *this;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator--() noexcept
{
	--this->x;
	--this->y;
	--this->z;
	--this->w;

	return *this;
}

template<typename T>
constexpr const TRAP::Math::Vec<4, T> TRAP::Math::Vec<4, T>::operator++(const int) noexcept
{
	Vec<4, T> Result(*this);
	++*this;

	return Result;
}

template<typename T>
constexpr const TRAP::Math::Vec<4, T> TRAP::Math::Vec<4, T>::operator--(const int) noexcept
{
	Vec<4, T> Result(*this);
	--*this;

	return Result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary bit operators

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator%=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x % scalar, this->y % scalar, this->z % scalar, this->w % scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator%=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x % v.x, this->y % v.y, this->z % v.z, this->w % v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator&=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x & scalar, this->y & scalar, this->z & scalar, this->w & scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator&=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x & v.x, this->y & v.y, this->z & v.z, this->w & v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator|=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x | scalar, this->y | scalar, this->z | scalar, this->w | scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator|=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x | v.x, this->y | v.y, this->z | v.z, this->w | v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator^=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x ^ scalar, this->y ^ scalar, this->z ^ scalar, this->w ^ scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator^=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x ^ v.x, this->y ^ v.y, this->z ^ v.z, this->w ^ v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator<<=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x << scalar, this->y << scalar, this->z << scalar, this->w << scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator<<=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x << v.x, this->y << v.y, this->z << v.z, this->w << v.w));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator>>=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x >> scalar, this->y >> scalar, this->z >> scalar, this->w >> scalar));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator>>=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x >> v.x, this->y >> v.y, this->z >> v.z, this->w >> v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] std::string TRAP::Math::Vec<4, T>::ToString() const
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if constexpr(std::is_same_v<T, float>)
		return fmt::format("Vec4f({}, {}, {}, {})", x, y, z, w);
	else if constexpr(std::is_same_v<T, double>)
		return fmt::format("Vec4d({}, {}, {}, {})", x, y, z, w);
	else if constexpr(std::is_same_v<T, bool>)
		return fmt::format("Vec4b({}, {}, {}, {})", (x ? "true" : "false"), (y ? "true" : "false"), (z ? "true" : "false"), (w ? "true" : "false"));
	else if constexpr(std::is_same_v<T, int8_t>)
		return fmt::format("Vec4i8({}, {}, {}, {})", x, y, z, w);
	else if constexpr(std::is_same_v<T, int16_t>)
		return fmt::format("Vec4i16({}, {}, {}, {})", x, y, z, w);
	else if constexpr(std::is_same_v<T, int32_t>)
		return fmt::format("Vec4i32({}, {}, {}, {})", x, y, z, w);
	else if constexpr(std::is_same_v<T, int64_t>)
		return fmt::format("Vec4i64({}, {}, {}, {})", x, y, z, w);
	else if constexpr(std::is_same_v<T, uint8_t>)
		return fmt::format("Vec4ui8({}, {}, {}, {})", x, y, z, w);
	else if constexpr(std::is_same_v<T, uint16_t>)
		return fmt::format("Vec4ui16({}, {}, {}, {})", x, y, z, w);
	else if constexpr(std::is_same_v<T, uint32_t>)
		return fmt::format("Vec4ui32({}, {}, {}, {})", x, y, z, w);
	else if constexpr(std::is_same_v<T, uint64_t>)
		return fmt::format("Vec4ui64({}, {}, {}, {})", x, y, z, w);
	else
		return "Unknown type";
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary constant operators

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<4, T>& v) noexcept
{
	return v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(0) -= v;
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<4, T>& v, const T& scalar) noexcept
{
	return Vec<4, T>(v) += scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(v) += scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1) += v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<4, T>& v, const T& scalar) noexcept
{
	return Vec<4, T>(v) -= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar) -= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1) -= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<4, T>& v, const T& scalar) noexcept
{
	return Vec<4, T>(v) *= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(v) *= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1) *= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(const Vec<4, T>& v, const T& scalar) noexcept
{
	return Vec<4, T>(v) /= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar) /= v;
}


template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1) /= v2;
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary bit operators

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v) %= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar) %= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1) %= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v) &= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar) &= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1) &= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v) |= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar) |= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1) |= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v) ^= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar) ^= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1) ^= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v) <<= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar) <<= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1) <<= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v) >>= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar) >>= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1) >>= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator~(const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(~v.x, ~v.y, ~v.z, ~v.w);
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
constexpr bool TRAP::Math::operator==(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

template<typename T>
constexpr bool TRAP::Math::operator!=(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return !(v1 == v2);
}

constexpr TRAP::Math::Vec<4, bool> TRAP::Math::operator&&(const Vec<4, bool>& v1, const Vec<4, bool>& v2) noexcept
{
	return Vec<4, bool>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
}

constexpr TRAP::Math::Vec<4, bool> TRAP::Math::operator||(const Vec<4, bool>& v1, const Vec<4, bool>& v2) noexcept
{
	return Vec<4, bool>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
}

//-------------------------------------------------------------------------------------------------------------------//
//std::get support

namespace std
{
	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	constexpr T& get(TRAP::Math::Vec<4, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<4, T>::Length());

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

		case 3:
			return v.w;
		}
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	constexpr T&& get(TRAP::Math::Vec<4, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<4, T>::Length());

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

		case 3:
			return v.w;
		}
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	constexpr const T& get(const TRAP::Math::Vec<4, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<4, T>::Length());

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

		case 3:
			return v.w;
		}
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	constexpr const T&& get(const TRAP::Math::Vec<4, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<4, T>::Length());

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

		case 3:
			return v.w;
		}
	}
}

#endif /*TRAP_VEC4_H*/
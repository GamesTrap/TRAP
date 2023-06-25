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
#include "Utils/Utils.h"

namespace TRAP::Math
{
	/// <summary>
	/// Struct for a 2-dimensional vector.
	/// </summary>
	template<class T>
	struct Vec<2, T>
	{
		//Implementation detail
		using valueType = T;
		using type = Vec<2, T>;
		using boolType = Vec<2, bool>;

		//Data
		T x, y;

		//Implicit basic constructors
		constexpr Vec() noexcept = default;
		constexpr Vec(const Vec & v) noexcept = default;

		//Explicit basic constructors
		explicit constexpr Vec(T scalar) noexcept;
		constexpr Vec(T x_, T y_) noexcept;

		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(A x_, B y_) noexcept;

		//Conversion vector constructors

		//Explicit conversions
		template<typename U>
		explicit constexpr Vec(const Vec<3, U> & v) noexcept;
		//Explicit conversions
		template<typename U>
		explicit constexpr Vec(const Vec<4, U> & v) noexcept;

		//Explicit conversions
		template<typename U>
		explicit constexpr Vec(const Vec<2, U> & v) noexcept;

		constexpr Vec(Vec&&) noexcept = default;
		constexpr ~Vec() = default;
		constexpr Vec<2, T>& operator=(Vec&&) noexcept = default;

		/// <summary>
		/// Retrieve the count of components of the vector.
		/// </summary>
		/// <returns>Count of components.</returns>
		[[nodiscard]] static constexpr std::size_t Length() noexcept;

		//Component Access
		[[nodiscard]] constexpr T& operator[](std::size_t i);
		[[nodiscard]] constexpr const T& operator[](std::size_t i) const;

		//Unary arithmetic operators
		constexpr Vec<2, T>& operator=(const Vec& v) noexcept = default;

		template<typename U>
		constexpr Vec<2, T>& operator=(const Vec<2, U> & v) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator+=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator+=(const Vec<2, U> & v) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator-=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator-=(const Vec<2, U> & v) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator*=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator*=(const Vec<2, U> & v) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator/=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator/=(const Vec<2, U> & v) noexcept;

		//Increment and decrement operators
		constexpr Vec<2, T>& operator++() noexcept;
		constexpr Vec<2, T>& operator--() noexcept;
		constexpr const Vec<2, T> operator++(int) noexcept;
		constexpr const Vec<2, T> operator--(int) noexcept;

		//Unary bit operators
		template<typename U>
		constexpr Vec<2, T>& operator%=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator%=(const Vec<2, U>& v) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator&=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator&=(const Vec<2, U>& v) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator|=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator|=(const Vec<2, U>& v) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator^=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator^=(const Vec<2, U>& v) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator<<=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator<<=(const Vec<2, U>& v) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator>>=(U scalar) noexcept;
		template<typename U>
		constexpr Vec<2, T>& operator>>=(const Vec<2, U>& v) noexcept;

		[[nodiscard]] std::string ToString() const;
	};

	//Unary operators
	template<typename T>
	constexpr Vec<2, T> operator+(const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator-(const Vec<2, T>& v) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	//Binary operators
	template<typename T>
	constexpr Vec<2, T> operator+(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator+(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator+(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator-(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator-(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator-(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator*(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator*(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator*(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator/(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator/(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator/(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator%(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator%(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator%(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator&(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator&(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator&(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator|(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator|(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator|(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator^(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator^(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator^(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator<<(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator<<(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator<<(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator>>(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator>>(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator>>(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr Vec<2, T> operator~(const Vec<2, T>& v) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	//Boolean operators
	template<typename T>
	constexpr bool operator==(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	constexpr bool operator!=(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	constexpr Vec<2, bool> operator&&(const Vec<2, bool>& v1, const Vec<2, bool>& v2) noexcept;

	constexpr Vec<2, bool> operator||(const Vec<2, bool>& v1, const Vec<2, bool>& v2) noexcept;
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	struct hash<TRAP::Math::Vec<2, T>>
	{
		constexpr std::size_t operator()(const TRAP::Math::Vec<2, T>& v) const noexcept
		{
			std::size_t seed = 0;
			hash<T> hasher;
			TRAP::Utils::HashCombine(seed, hasher(v.x), hasher(v.y));
			return seed;
		}
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------//

//Explicit basic constructors
//
template<typename T>
constexpr TRAP::Math::Vec<2, T>::Vec(const T scalar) noexcept
	: x(scalar), y(scalar)
{}

template<typename T>
constexpr TRAP::Math::Vec<2, T>::Vec(const T x_, const T y_) noexcept
	: x(x_), y(y_)
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion scalar constructors

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<2, T>::Vec(const A x_, const B y_) noexcept
	: x(static_cast<T>(x_)), y(static_cast<T>(y_))
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion vector constructors

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>::Vec(const Vec<2, U>& v) noexcept
	: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
{}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>::Vec(const Vec<3, U>& v) noexcept
	: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
{}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>::Vec(const Vec<4, U>& v) noexcept
	: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
[[nodiscard]] constexpr std::size_t TRAP::Math::Vec<2, T>::Length() noexcept
{
	return 2;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component accesses

template<typename T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<2, T>::operator[](const std::size_t i)
{
	TRAP_ASSERT(i < this->Length(), "Math::Vec<2, T>::operator[]: Index out of range!");

	return i == 0 ? x : y;
}

template<typename T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<2, T>::operator[](const std::size_t i) const
{
	TRAP_ASSERT(i < this->Length(), "Math::Vec<2, T>::operator[]: Index out of range!");

	return i == 0 ? x : y;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator=(const Vec<2, U>& v) noexcept
{
	this->x = static_cast<T>(v.x);
	this->y = static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator+=(const U scalar) noexcept
{
	this->x += static_cast<T>(scalar);
	this->y += static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator+=(const Vec<2, U>& v) noexcept
{
	this->x += static_cast<T>(v.x);
	this->y += static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator-=(const U scalar) noexcept
{
	this->x -= static_cast<T>(scalar);
	this->y -= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator-=(const Vec<2, U>& v) noexcept
{
	this->x -= static_cast<T>(v.x);
	this->y -= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator*=(const U scalar) noexcept
{
	this->x *= static_cast<T>(scalar);
	this->y *= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator*=(const Vec<2, U>& v) noexcept
{
	this->x *= static_cast<T>(v.x);
	this->y *= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator/=(const U scalar) noexcept
{
	this->x /= static_cast<T>(scalar);
	this->y /= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator/=(const Vec<2, U>& v) noexcept
{
	this->x /= static_cast<T>(v.x);
	this->y /= static_cast<T>(v.y);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator++() noexcept
{
	++this->x;
	++this->y;

	return *this;
}

template<typename T>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator--() noexcept
{
	--this->x;
	--this->y;

	return *this;
}

template<typename T>
constexpr const TRAP::Math::Vec<2, T> TRAP::Math::Vec<2, T>::operator++(int) noexcept
{
	Vec<2, T> result(*this);
	++*this;

	return result;
}

template<typename T>
constexpr const TRAP::Math::Vec<2, T> TRAP::Math::Vec<2, T>::operator--(int) noexcept
{
	Vec<2, T> result(*this);
	--*this;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary bit operators

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator%=(const U scalar) noexcept
{
	this->x %= static_cast<T>(scalar);
	this->y %= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator%=(const Vec<2, U>& v) noexcept
{
	this->x %= static_cast<T>(v.x);
	this->y %= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator&=(const U scalar) noexcept
{
	this->x &= static_cast<T>(scalar);
	this->y &= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator&=(const Vec<2, U>& v) noexcept
{
	this->x &= static_cast<T>(v.x);
	this->y &= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator|=(const U scalar) noexcept
{
	this->x |= static_cast<T>(scalar);
	this->y |= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator|=(const Vec<2, U>& v) noexcept
{
	this->x |= static_cast<T>(v.x);
	this->y |= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator^=(const U scalar) noexcept
{
	this->x ^= static_cast<T>(scalar);
	this->y ^= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator^=(const Vec<2, U>& v) noexcept
{
	this->x ^= static_cast<T>(v.x);
	this->y ^= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator<<=(const U scalar) noexcept
{
	this->x <<= static_cast<T>(scalar);
	this->y <<= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator<<=(const Vec<2, U>& v) noexcept
{
	this->x <<= static_cast<T>(v.x);
	this->y <<= static_cast<T>(v.y);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator>>=(const U scalar) noexcept
{
	this->x >>= static_cast<T>(scalar);
	this->y >>= static_cast<T>(scalar);

	return *this;
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator>>=(const Vec<2, U>& v) noexcept
{
	this->x >>= static_cast<T>(v.x);
	this->y >>= static_cast<T>(v.y);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] std::string TRAP::Math::Vec<2, T>::ToString() const
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if constexpr(std::is_same_v<T, float>)
		return fmt::format("Vec2f({}, {})", x, y);
	else if constexpr(std::is_same_v<T, double>)
		return fmt::format("Vec2d({}, {})", x, y);
	else if constexpr(std::is_same_v<T, bool>)
		return fmt::format("Vec2b({}, {})", (x ? "true" : "false"), (y ? "true" : "false"));
	else if constexpr(std::is_same_v<T, int8_t>)
		return fmt::format("Vec2i8({}, {})", x, y);
	else if constexpr(std::is_same_v<T, int16_t>)
		return fmt::format("Vec2i16({}, {})", x, y);
	else if constexpr(std::is_same_v<T, int32_t>)
		return fmt::format("Vec2i32({}, {})", x, y);
	else if constexpr(std::is_same_v<T, int64_t>)
		return fmt::format("Vec2i64({}, {})", x, y);
	else if constexpr(std::is_same_v<T, uint8_t>)
		return fmt::format("Vec2ui8({}, {})", x, y);
	else if constexpr(std::is_same_v<T, uint16_t>)
		return fmt::format("Vec2ui16({}, {})", x, y);
	else if constexpr(std::is_same_v<T, uint32_t>)
		return fmt::format("Vec2ui32({}, {})", x, y);
	else if constexpr(std::is_same_v<T, uint64_t>)
		return fmt::format("Vec2ui64({}, {})", x, y);
	else
		return "Unknown type";
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<2, T>& v) noexcept
{
	return v;
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(-v.x, -v.y);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x + scalar, v.y + scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar + v.x, scalar + v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x + v2.x, v1.y + v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x - scalar, v.y - scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar - v.x, scalar - v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x - v2.x, v1.y - v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x * scalar, v.y * scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar * v.x, scalar * v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x * v2.x, v1.y * v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x / scalar, v.y / scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar / v.x, scalar / v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x / v2.x, v1.y / v2.y);
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary bit operators

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x % scalar, v.y % scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar % v.x, scalar % v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x % v2.x, v1.y % v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x & scalar, v.y & scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar & v.x, scalar & v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x & v2.x, v1.y & v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x | scalar, v.y | scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar | v.x, scalar | v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x | v2.x, v1.y | v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x ^ scalar, v.y ^ scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar ^ v.x, scalar ^ v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x ^ v2.x, v1.y ^ v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x << scalar, v.y << scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar << v.x, scalar << v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x << v2.x, v1.y << v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x >> scalar, v.y >> scalar);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar >> v.x, scalar >> v.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x >> v2.x, v1.y >> v2.y);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator~(const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(~v.x, ~v.y);
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
constexpr bool TRAP::Math::operator==(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return v1.x == v2.x && v1.y == v2.y;
}

template<typename T>
constexpr bool TRAP::Math::operator!=(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return !(v1 == v2);
}

constexpr TRAP::Math::Vec<2, bool> TRAP::Math::operator&&(const Vec<2, bool>& v1, const Vec<2, bool>& v2) noexcept
{
	return Vec<2, bool>(v1.x && v2.x, v1.y && v2.y);
}

constexpr TRAP::Math::Vec<2, bool> TRAP::Math::operator||(const Vec<2, bool>& v1, const Vec<2, bool>& v2) noexcept
{
	return Vec<2, bool>(v1.x || v2.x, v1.y || v2.y);
}

//-------------------------------------------------------------------------------------------------------------------//
//std::get support

namespace std
{
	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 2).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	constexpr T& get(TRAP::Math::Vec<2, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<2, T>::Length());

		return I == 0 ? v.x : v.y;
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 2).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	constexpr T&& get(TRAP::Math::Vec<2, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<2, T>::Length());

		return I == 0 ? v.x : v.y;
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 2).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	constexpr const T& get(const TRAP::Math::Vec<2, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<2, T>::Length());

		return I == 0 ? v.x : v.y;
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 2).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	constexpr const T&& get(const TRAP::Math::Vec<2, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<2, T>::Length());

		return I == 0 ? v.x : v.y;
	}
}

#endif /*TRAP_VEC2_H*/
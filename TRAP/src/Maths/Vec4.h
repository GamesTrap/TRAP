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

#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Types.h"
#include "Utils/Utils.h"

namespace TRAP::Math
{
	/// <summary>
	/// Struct for a 4-dimensional vector.
	/// </summary>
	template<typename T>
	requires std::is_arithmetic_v<T>
	struct Vec<4, T>
	{
		//Implementation details
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using iterator = std::array<T, 4>::iterator;
		using const_iterator = std::array<T, 4>::const_iterator;
		using size_type = uint32_t;
		using difference_type = std::ptrdiff_t;
		using reverse_iterator = std::array<T, 4>::reverse_iterator;
		using const_reverse_iterator = std::array<T, 4>::const_reverse_iterator;

private:
		//Data
		std::array<T, 4> data;

public:
		//Implicit basic constructors
		constexpr Vec() noexcept = default;
		constexpr Vec(const Vec<4, T> & v) noexcept = default;

		//Explicit basic constructors
		explicit constexpr Vec(T scalar) noexcept;
		constexpr Vec(T x_, T y_, T z_, T w_) noexcept;

		//Explicit conversions
		template<typename X, typename Y, typename Z, typename W>
		requires std::is_arithmetic_v<X> && std::is_arithmetic_v<Y> && std::is_arithmetic_v<Z> && std::is_arithmetic_v<W>
		constexpr Vec(X x_, Y y_, Z z_, W w_) noexcept;

		//Conversion vector constructors

		//Explicit conversions
		template<typename A, typename B, typename C>
		requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B> && std::is_arithmetic_v<C>
		constexpr Vec(const Vec<2, A> & xy, B z_, C w_) noexcept;
		//Explicit conversions
		template<typename A, typename B, typename C>
		requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B> && std::is_arithmetic_v<C>
		explicit constexpr Vec(A x_, const Vec<2, B> & yz, C w_) noexcept;
		//Explicit conversions
		template<typename A, typename B, typename C>
		requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B> && std::is_arithmetic_v<C>
		constexpr Vec(A x_, B y_, const Vec<2, C> & zw) noexcept;
		//Explicit conversions
		template<typename A, typename B>
		requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
		constexpr Vec(const Vec<3, A> & xyz, B w_) noexcept;
		//Explicit conversions
		template<typename A, typename B>
		requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
		constexpr Vec(A x_, const Vec<3, B> & yzw) noexcept;
		//Explicit conversions
		template<typename A, typename B>
		requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
		constexpr Vec(const Vec<2, A> & xy, const Vec<2, B> & zw) noexcept;

		//Explicit conversions
		template<typename U>
		requires std::is_arithmetic_v<U>
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
		[[nodiscard]] constexpr T& x() noexcept;
		[[nodiscard]] constexpr const T& x() const noexcept;
		[[nodiscard]] constexpr T& y() noexcept;
		[[nodiscard]] constexpr const T& y() const noexcept;
		[[nodiscard]] constexpr T& z() noexcept;
		[[nodiscard]] constexpr const T& z() const noexcept;
		[[nodiscard]] constexpr T& w() noexcept;
		[[nodiscard]] constexpr const T& w() const noexcept;

		[[nodiscard]] constexpr T& operator[](std::size_t i) noexcept;
		[[nodiscard]] constexpr const T& operator[](std::size_t i) const noexcept;

		[[nodiscard]] T& at(std::size_t i);
		[[nodiscard]] const T& at(std::size_t i) const;

		//Iterator
		[[nodiscard]] constexpr const_iterator begin() const noexcept;
		[[nodiscard]] constexpr iterator begin() noexcept;
		[[nodiscard]] constexpr const_iterator cbegin() const noexcept;
		[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept;
		[[nodiscard]] constexpr reverse_iterator rbegin() noexcept;
		[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept;
		[[nodiscard]] constexpr const_iterator end() const noexcept;
		[[nodiscard]] constexpr iterator end() noexcept;
		[[nodiscard]] constexpr const_iterator cend() const noexcept;
		[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept;
		[[nodiscard]] constexpr reverse_iterator rend() noexcept;
		[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept;

		//Unary arithmetic operators
		constexpr Vec<4, T>& operator=(const Vec<4, T> & v) noexcept = default;

		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<4, T>& operator=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator+=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<4, T>& operator+=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator-=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<4, T>& operator-=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator*=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<4, T>& operator*=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator/=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
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
		requires std::is_arithmetic_v<U>
		constexpr Vec<4, T>& operator%=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator&=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<4, T>& operator&=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator|=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<4, T>& operator|=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator^=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<4, T>& operator^=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator<<=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<4, T>& operator<<=(const Vec<4, U> & v) noexcept;
		template<typename U>
		constexpr Vec<4, T>& operator>>=(U scalar) noexcept;
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<4, T>& operator>>=(const Vec<4, U> & v) noexcept;

		[[nodiscard]] constexpr auto operator<=>(const Vec<4, T>& rhs) const = default;
		[[nodiscard]] constexpr bool operator==(const Vec<4, T>& rhs) const = default;
		[[nodiscard]] constexpr bool operator!=(const Vec<4, T>& rhs) const = default;

		[[nodiscard]] std::string ToString() const;
	};

	//Unary operators
	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator+(const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v) noexcept;

	//Binary operators
	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator+(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator+(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator+(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator-(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator*(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator/(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator/(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator/(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator%(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator%(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator%(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator&(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator&(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator&(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator|(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator|(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator|(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator^(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator^(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator^(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator<<(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator<<(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator<<(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator>>(const Vec<4, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator>>(T scalar, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator>>(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Vec<4, T> operator~(const Vec<4, T>& v) noexcept;

	//Boolean operators
	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr bool operator==(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
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
	requires std::is_arithmetic_v<T>
	struct hash<TRAP::Math::Vec<4, T>>
	{
		[[nodiscard]] constexpr std::size_t operator()(const TRAP::Math::Vec<4, T>& v) const noexcept
		{
			std::size_t seed = 0;
			hash<T> hasher;
			TRAP::Utils::HashCombine(seed, hasher(v.x()), hasher(v.y()), hasher(v.z()), hasher(v.w()));
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
constexpr TRAP::Math::Vec<4, T>::Vec(const T scalar) noexcept
	: data{scalar, scalar, scalar, scalar}
{}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T>::Vec(T x_, T y_, T z_, T w_) noexcept
	: data{x_, y_, z_, w_}
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion scalar constructors

template<typename T>
requires std::is_arithmetic_v<T>
template<typename X, typename Y, typename Z, typename W>
requires std::is_arithmetic_v<X> && std::is_arithmetic_v<Y> && std::is_arithmetic_v<Z> && std::is_arithmetic_v<W>
constexpr TRAP::Math::Vec<4, T>::Vec(const X x_, const Y y_, const Z z_, const W w_) noexcept
	: data{static_cast<T>(x_), static_cast<T>(y_), static_cast<T>(z_), static_cast<T>(w_)}
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion vector constructors

template<typename T>
requires std::is_arithmetic_v<T>
template<typename A, typename B, typename C>
requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B> && std::is_arithmetic_v<C>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<2, A>& xy, const B z_, const C w_) noexcept
	: data{static_cast<T>(xy.x()), static_cast<T>(xy.y()), static_cast<T>(z_), static_cast<T>(w_)}
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename A, typename B, typename C>
requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B> && std::is_arithmetic_v<C>
constexpr TRAP::Math::Vec<4, T>::Vec(const A x_, const Vec<2, B>& yz, const C w_) noexcept
	: data{static_cast<T>(x_), static_cast<T>(yz.x()), static_cast<T>(yz.y()), static_cast<T>(w_)}
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename A, typename B, typename C>
requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B> && std::is_arithmetic_v<C>
constexpr TRAP::Math::Vec<4, T>::Vec(const A x_, const B y_, const Vec<2, C>& zw) noexcept
	: data{static_cast<T>(x_), static_cast<T>(y_), static_cast<T>(zw.x()), static_cast<T>(zw.y())}
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename A, typename B>
requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<3, A>& xyz, const B w_) noexcept
	: data{static_cast<T>(xyz.x()), static_cast<T>(xyz.y()), static_cast<T>(xyz.z()), static_cast<T>(w_)}
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename A, typename B>
requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
constexpr TRAP::Math::Vec<4, T>::Vec(const A x_, const Vec<3, B>& yzw) noexcept
	: data{static_cast<T>(x_), static_cast<T>(yzw.x()), static_cast<T>(yzw.y()), static_cast<T>(yzw.z())}
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename A, typename B>
requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<2, A>& xy, const Vec<2, B>& zw) noexcept
	: data{static_cast<T>(xy.x()), static_cast<T>(xy.y()), static_cast<T>(zw.x()), static_cast<T>(zw.y())}
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<4, U>& v) noexcept
	: data{static_cast<T>(v.x()), static_cast<T>(v.y()), static_cast<T>(v.z()), static_cast<T>(v.w())}
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr std::size_t TRAP::Math::Vec<4, T>::Length() noexcept
{
	return 4;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component accesses

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<4, T>::x() noexcept
{
	return data[0];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<4, T>::x() const noexcept
{
	return data[0];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<4, T>::y() noexcept
{
	return data[1];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<4, T>::y() const noexcept
{
	return data[1];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<4, T>::z() noexcept
{
	return data[2];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<4, T>::z() const noexcept
{
	return data[2];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<4, T>::w() noexcept
{
	return data[3];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<4, T>::w() const noexcept
{
	return data[3];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<4, T>::operator[](const std::size_t i) noexcept
{
	return data[i];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<4, T>::operator[](const std::size_t i) const noexcept
{
	return data[i];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] T& TRAP::Math::Vec<4, T>::at(const std::size_t i)
{
	TRAP_ASSERT(i < this->Length(), "Math::Vec<4, T>::at(): index out of range!");

	return data[i];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] const T& TRAP::Math::Vec<4, T>::at(const std::size_t i) const
{
	TRAP_ASSERT(i < this->Length(), "Math::Vec<4, T>::at(): index out of range!");

	return data[i];
}

//-------------------------------------------------------------------------------------------------------------------//

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::const_iterator TRAP::Math::Vec<4, T>::begin() const noexcept
{
	return data.begin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::iterator TRAP::Math::Vec<4, T>::begin() noexcept
{
	return data.begin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::const_iterator TRAP::Math::Vec<4, T>::cbegin() const noexcept
{
	return data.cbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::const_reverse_iterator TRAP::Math::Vec<4, T>::rbegin() const noexcept
{
	return data.rbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::reverse_iterator TRAP::Math::Vec<4, T>::rbegin() noexcept
{
	return data.rbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::const_reverse_iterator TRAP::Math::Vec<4, T>::crbegin() const noexcept
{
	return data.crbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::const_iterator TRAP::Math::Vec<4, T>::end() const noexcept
{
	return data.end();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::iterator TRAP::Math::Vec<4, T>::end() noexcept
{
	return data.end();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::const_iterator TRAP::Math::Vec<4, T>::cend() const noexcept
{
	return data.cend();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::const_reverse_iterator TRAP::Math::Vec<4, T>::rend() const noexcept
{
	return data.rend();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::reverse_iterator TRAP::Math::Vec<4, T>::rend() noexcept
{
	return data.rend();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T>::const_reverse_iterator TRAP::Math::Vec<4, T>::crend() const noexcept
{
	return data.crend();
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator=(const Vec<4, U>& v) noexcept
{
	this->x() = static_cast<T>(v.x());
	this->y() = static_cast<T>(v.y());
	this->z() = static_cast<T>(v.z());
	this->w() = static_cast<T>(v.w());

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator+=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x() + static_cast<T>(scalar), this->y() + static_cast<T>(scalar),
	                          this->z() + static_cast<T>(scalar), this->w() + static_cast<T>(scalar)));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator+=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x() + static_cast<T>(v.x()), this->y() + static_cast<T>(v.y()),
	                          this->z() + static_cast<T>(v.z()), this->w() + static_cast<T>(v.w())));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator-=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x() - static_cast<T>(scalar), this->y() - static_cast<T>(scalar),
	                          this->z() - static_cast<T>(scalar), this->w() - static_cast<T>(scalar)));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator-=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x() - static_cast<T>(v.x()), this->y() - static_cast<T>(v.y()),
	                          this->z() - static_cast<T>(v.z()), this->w() - static_cast<T>(v.w())));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator*=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x() * static_cast<T>(scalar), this->y() * static_cast<T>(scalar),
	                          this->z() * static_cast<T>(scalar), this->w() * static_cast<T>(scalar)));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator*=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x() * static_cast<T>(v.x()), this->y() * static_cast<T>(v.y()),
	                          this->z() * static_cast<T>(v.z()), this->w() * static_cast<T>(v.w())));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator/=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x() / static_cast<T>(scalar), this->y() / static_cast<T>(scalar),
	                          this->z() / static_cast<T>(scalar), this->w() / static_cast<T>(scalar)));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator/=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x() / static_cast<T>(v.x()), this->y() / static_cast<T>(v.y()),
	                          this->z() / static_cast<T>(v.z()), this->w() / static_cast<T>(v.w())));
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator++() noexcept
{
	++this->x();
	++this->y();
	++this->z();
	++this->w();

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator--() noexcept
{
	--this->x();
	--this->y();
	--this->z();
	--this->w();

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr const TRAP::Math::Vec<4, T> TRAP::Math::Vec<4, T>::operator++(const int32_t) noexcept
{
	const Vec<4, T> Result(*this);
	++*this;

	return Result;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr const TRAP::Math::Vec<4, T> TRAP::Math::Vec<4, T>::operator--(const int32_t) noexcept
{
	const Vec<4, T> Result(*this);
	--*this;

	return Result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary bit operators

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator%=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x() % static_cast<T>(scalar), this->y() % static_cast<T>(scalar),
	                          this->z() % static_cast<T>(scalar), this->w() % static_cast<T>(scalar)));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator%=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x() % static_cast<T>(v.x()), this->y() % static_cast<T>(v.y()),
	                          this->z() % static_cast<T>(v.z()), this->w() % static_cast<T>(v.w())));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator&=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x() & static_cast<T>(scalar), this->y() & static_cast<T>(scalar),
	                          this->z() & static_cast<T>(scalar), this->w() & static_cast<T>(scalar)));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator&=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x() & static_cast<T>(v.x()), this->y() & static_cast<T>(v.y()),
	                          this->z() & static_cast<T>(v.z()), this->w() & static_cast<T>(v.w())));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator|=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x() | static_cast<T>(scalar), this->y() | static_cast<T>(scalar),
	                          this->z() | static_cast<T>(scalar), this->w() | static_cast<T>(scalar)));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator|=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x() | static_cast<T>(v.x()), this->y() | static_cast<T>(v.y()),
	                          this->z() | static_cast<T>(v.z()), this->w() | static_cast<T>(v.w())));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator^=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x() ^ static_cast<T>(scalar), this->y() ^ static_cast<T>(scalar),
	                          this->z() ^ static_cast<T>(scalar), this->w() ^ static_cast<T>(scalar)));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator^=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x() ^ static_cast<T>(v.x()), this->y() ^ static_cast<T>(v.y()),
	                          this->z() ^ static_cast<T>(v.z()), this->w() ^ static_cast<T>(v.w())));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator<<=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x() << static_cast<T>(scalar), this->y() << static_cast<T>(scalar),
	                          this->z() << static_cast<T>(scalar), this->w() << static_cast<T>(scalar)));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator<<=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x() << static_cast<T>(v.x()), this->y() << static_cast<T>(v.y()),
	                          this->z() << static_cast<T>(v.z()), this->w() << static_cast<T>(v.w())));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator>>=(const U scalar) noexcept
{
	return (*this = Vec<4, T>(this->x() >> static_cast<T>(scalar), this->y() >> static_cast<T>(scalar),
	                          this->z() >> static_cast<T>(scalar), this->w() >> static_cast<T>(scalar)));
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator>>=(const Vec<4, U>& v) noexcept
{
	return (*this = Vec<4, T>(this->x() >> static_cast<T>(v.x()), this->y() >> static_cast<T>(v.y()),
	                          this->z() >> static_cast<T>(v.z()), this->w() >> static_cast<T>(v.w())));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] std::string TRAP::Math::Vec<4, T>::ToString() const
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if constexpr(std::same_as<T, float>)
		return fmt::format("Vec4f({}, {}, {}, {})", x(), y(), z(), w());
	else if constexpr(std::same_as<T, double>)
		return fmt::format("Vec4d({}, {}, {}, {})", x(), y(), z(), w());
	else if constexpr(std::same_as<T, bool>)
		return fmt::format("Vec4b({}, {}, {}, {})", (x() ? "true" : "false"), (y() ? "true" : "false"), (z() ? "true" : "false"), (w() ? "true" : "false"));
	else if constexpr(std::same_as<T, int8_t>)
		return fmt::format("Vec4i8({}, {}, {}, {})", x(), y(), z(), w());
	else if constexpr(std::same_as<T, int16_t>)
		return fmt::format("Vec4i16({}, {}, {}, {})", x(), y(), z(), w());
	else if constexpr(std::same_as<T, int32_t>)
		return fmt::format("Vec4i32({}, {}, {}, {})", x(), y(), z(), w());
	else if constexpr(std::same_as<T, int64_t>)
		return fmt::format("Vec4i64({}, {}, {}, {})", x(), y(), z(), w());
	else if constexpr(std::same_as<T, uint8_t>)
		return fmt::format("Vec4ui8({}, {}, {}, {})", x(), y(), z(), w());
	else if constexpr(std::same_as<T, uint16_t>)
		return fmt::format("Vec4ui16({}, {}, {}, {})", x(), y(), z(), w());
	else if constexpr(std::same_as<T, uint32_t>)
		return fmt::format("Vec4ui32({}, {}, {}, {})", x(), y(), z(), w());
	else if constexpr(std::same_as<T, uint64_t>)
		return fmt::format("Vec4ui64({}, {}, {}, {})", x(), y(), z(), w());
	else
		return "Unknown type";
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary constant operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<4, T>& v) noexcept
{
	return v;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(-v.x(), -v.y(), -v.z(), -v.w());
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v.x() + scalar, v.y() + scalar, v.z() + scalar, v.w() + scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar + v.x(), scalar + v.y(), scalar + v.z(), scalar + v.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z(), v1.w() + v2.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v.x() - scalar, v.y() - scalar, v.z() - scalar, v.w() - scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar - v.x(), scalar - v.y(), scalar - v.z(), scalar - v.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z(), v1.w() - v2.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v.x() * scalar, v.y() * scalar, v.z() * scalar, v.w() * scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar * v.x(), scalar * v.y(), scalar * v.z(), scalar * v.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1.x() * v2.x(), v1.y() * v2.y(), v1.z() * v2.z(), v1.w() * v2.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v.x() / scalar, v.y() / scalar, v.z() / scalar, v.w() / scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar / v.x(), scalar / v.y(), scalar / v.z(), scalar / v.w());
}


template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1.x() / v2.x(), v1.y() / v2.y(), v1.z() / v2.z(), v1.w() / v2.w());
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary bit operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v.x() % scalar, v.y() % scalar, v.z() % scalar, v.w() % scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar % v.x(), scalar % v.y(), scalar % v.z(), scalar % v.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1.x() % v2.x(), v1.y() % v2.y(), v1.z() % v2.z(), v1.w() % v2.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v.x() & scalar, v.y() & scalar, v.z() & scalar, v.w() & scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar & v.x(), scalar & v.y(), scalar & v.z(), scalar & v.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1.x() & v2.x(), v1.y() & v2.y(), v1.z() & v2.z(), v1.w() & v2.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v.x() | scalar, v.y() | scalar, v.z() | scalar, v.w() | scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar | v.x(), scalar | v.y(), scalar | v.z(), scalar | v.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1.x() | v2.x(), v1.y() | v2.y(), v1.z() | v2.z(), v1.w() | v2.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v.x() ^ scalar, v.y() ^ scalar, v.z() ^ scalar, v.w() ^ scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar ^ v.x(), scalar ^ v.y(), scalar ^ v.z(), scalar ^ v.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1.x() ^ v2.x(), v1.y() ^ v2.y(), v1.z() ^ v2.z(), v1.w() ^ v2.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v.x() << scalar, v.y() << scalar, v.z() << scalar, v.w() << scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar << v.x(), scalar << v.y(), scalar << v.z(), scalar << v.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1.x() << v2.x(), v1.y() << v2.y(), v1.z() << v2.z(), v1.w() << v2.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(const Vec<4, T>& v, const T scalar) noexcept
{
	return Vec<4, T>(v.x() >> scalar, v.y() >> scalar, v.z() >> scalar, v.w() >> scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(const T scalar, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(scalar >> v.x(), scalar >> v.y(), scalar >> v.z(), scalar >> v.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return Vec<4, T>(v1.x() >> v2.x(), v1.y() >> v2.y(), v1.z() >> v2.z(), v1.w() >> v2.w());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator~(const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(~v.x(), ~v.y(), ~v.z(), ~v.w());
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr bool TRAP::Math::operator==(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return v1.x() == v2.x() && v1.y() == v2.y() && v1.z() == v2.z() && v1.w() == v2.w();
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr bool TRAP::Math::operator!=(const Vec<4, T>& v1, const Vec<4, T>& v2) noexcept
{
	return !(v1 == v2);
}

constexpr TRAP::Math::Vec<4, bool> TRAP::Math::operator&&(const Vec<4, bool>& v1, const Vec<4, bool>& v2) noexcept
{
	return Vec<4, bool>(v1.x() && v2.x(), v1.y() && v2.y(), v1.z() && v2.z(), v1.w() && v2.w());
}

constexpr TRAP::Math::Vec<4, bool> TRAP::Math::operator||(const Vec<4, bool>& v1, const Vec<4, bool>& v2) noexcept
{
	return Vec<4, bool>(v1.x() || v2.x(), v1.y() || v2.y(), v1.z() || v2.z(), v1.w() || v2.w());
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
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr T& get(TRAP::Math::Vec<4, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<4, T>::Length());

		if constexpr(I == 0)
		{
			return v.x();
		}
		else if constexpr(I == 1)
		{
			return v.y();
		}
		else if constexpr(I == 2)
		{
			return v.z();
		}
		else if constexpr(I == 3)
		{
			return v.w();
		}
#if __cpp_lib_unreachable >= 202202L
		else
		{
			std::unreachable();
		}
#endif /*__cpp_lib_unreachable >= 202202L*/
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr T&& get(TRAP::Math::Vec<4, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<4, T>::Length());

		if constexpr(I == 0)
		{
			return v.x();
		}
		else if constexpr(I == 1)
		{
			return v.y();
		}
		else if constexpr(I == 2)
		{
			return v.z();
		}
		else if constexpr(I == 3)
		{
			return v.w();
		}
#if __cpp_lib_unreachable >= 202202L
		else
		{
			std::unreachable();
		}
#endif /*__cpp_lib_unreachable >= 202202L*/
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const T& get(const TRAP::Math::Vec<4, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<4, T>::Length());

		if constexpr(I == 0)
		{
			return v.x();
		}
		else if constexpr(I == 1)
		{
			return v.y();
		}
		else if constexpr(I == 2)
		{
			return v.z();
		}
		else if constexpr(I == 3)
		{
			return v.w();
		}
#if __cpp_lib_unreachable >= 202202L
		else
		{
			std::unreachable();
		}
#endif /*__cpp_lib_unreachable >= 202202L*/
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const T&& get(const TRAP::Math::Vec<4, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<4, T>::Length());

		if constexpr(I == 0)
		{
			return v.x();
		}
		else if constexpr(I == 1)
		{
			return v.y();
		}
		else if constexpr(I == 2)
		{
			return v.z();
		}
		else if constexpr(I == 3)
		{
			return v.w();
		}
#if __cpp_lib_unreachable >= 202202L
		else
		{
			std::unreachable();
		}
#endif /*__cpp_lib_unreachable >= 202202L*/
	}
}

#endif /*TRAP_VEC4_H*/
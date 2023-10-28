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

#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Types.h"
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
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using iterator = std::array<T, 3>::iterator;
		using const_iterator = std::array<T, 3>::const_iterator;
		using size_type = u32;
		using difference_type = isize;
		using reverse_iterator = std::array<T, 3>::reverse_iterator;
		using const_reverse_iterator = std::array<T, 3>::const_reverse_iterator;

private:
		//Data
		std::array<T, 3> data;

public:
		//Implicit basic constructors
		constexpr Vec() noexcept = default;
		constexpr Vec(const Vec & v) noexcept = default;

		//Explicit basic constructors
		explicit constexpr Vec(T scalar) noexcept;
		constexpr Vec(T x_, T y_, T z_) noexcept;

		//Explicit conversions
		template<typename X, typename Y, typename Z>
		requires std::is_arithmetic_v<X> && std::is_arithmetic_v<Y> && std::is_arithmetic_v<Z>
		constexpr Vec(X x_, Y y_, Z z_) noexcept;

		//Conversion vector constructors

		//Explicit conversions
		template<typename A, typename B>
		requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
		constexpr Vec(const Vec<2, A> & xy, B z_) noexcept;
		//Explicit conversions
		template<typename A, typename B>
		requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
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
		[[nodiscard]] static constexpr usize Length() noexcept;

		//Component accesses
		[[nodiscard]] constexpr T& x() noexcept;
		[[nodiscard]] constexpr const T& x() const noexcept;
		[[nodiscard]] constexpr T& y() noexcept;
		[[nodiscard]] constexpr const T& y() const noexcept;
		[[nodiscard]] constexpr T& z() noexcept;
		[[nodiscard]] constexpr const T& z() const noexcept;

		[[nodiscard]] constexpr T& operator[](usize i) noexcept;
		[[nodiscard]] constexpr const T& operator[](usize i) const noexcept;

		[[nodiscard]] T& at(usize i);
		[[nodiscard]] const T& at(usize i) const;

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
		constexpr const Vec<3, T> operator++(i32) noexcept;
		constexpr const Vec<3, T> operator--(i32) noexcept;

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

		[[nodiscard]] constexpr auto operator<=>(const Vec<3, T>& rhs) const noexcept = default;
		[[nodiscard]] constexpr bool operator==(const Vec<3, T>& rhs) const noexcept = default;
		[[nodiscard]] constexpr bool operator!=(const Vec<3, T>& rhs) const noexcept = default;

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
		[[nodiscard]] constexpr usize operator()(const TRAP::Math::Vec<3, T>& v) const noexcept
		{
			usize seed = 0;
			hash<T> hasher;
			TRAP::Utils::HashCombine(seed, hasher(v.x()), hasher(v.y()), hasher(v.z()));
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
	: data{scalar, scalar, scalar}
{}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T>::Vec(T x_, T y_, T z_) noexcept
	: data{x_, y_, z_}
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion scalar constructors

template<typename T>
requires std::is_arithmetic_v<T>
template<typename X, typename Y, typename Z>
requires std::is_arithmetic_v<X> && std::is_arithmetic_v<Y> && std::is_arithmetic_v<Z>
constexpr TRAP::Math::Vec<3, T>::Vec(const X x_, const Y y_, const Z z_) noexcept
	: data{static_cast<T>(x_), static_cast<T>(y_), static_cast<T>(z_)}
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion vector constructors

template<typename T>
requires std::is_arithmetic_v<T>
template<typename A, typename B>
requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
constexpr TRAP::Math::Vec<3, T>::Vec(const Vec<2, A>& xy, const B z_) noexcept
	: data{static_cast<T>(xy.x()), static_cast<T>(xy.y()), static_cast<T>(z_)}
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename A, typename B>
requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
constexpr TRAP::Math::Vec<3, T>::Vec(const A x_, const Vec<2, B>& yz) noexcept
	: data{static_cast<T>(x_), static_cast<T>(yz.x()), static_cast<T>(yz.y())}
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>::Vec(const Vec<3, U>& v) noexcept
	: data{static_cast<T>(v.x()), static_cast<T>(v.y()), static_cast<T>(v.z())}
{}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>::Vec(const Vec<4, U>& v) noexcept
	: data{static_cast<T>(v.x()), static_cast<T>(v.y()), static_cast<T>(v.z())}
{}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr usize TRAP::Math::Vec<3, T>::Length() noexcept
{
	return 3;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component accesses

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<3, T>::x() noexcept
{
	return data[0];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<3, T>::x() const noexcept
{
	return data[0];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<3, T>::y() noexcept
{
	return data[1];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<3, T>::y() const noexcept
{
	return data[1];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<3, T>::z() noexcept
{
	return data[2];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<3, T>::z() const noexcept
{
	return data[2];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<3, T>::operator[](const usize i) noexcept
{
	return data[i];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<3, T>::operator[](const usize i) const noexcept
{
	return data[i];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] T& TRAP::Math::Vec<3, T>::at(const usize i)
{
	TRAP_ASSERT(i < this->Length(), "Math::Vec<3, T>::at(): Index out of range!");

	return data[i];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] const T& TRAP::Math::Vec<3, T>::at(const usize i) const
{
	TRAP_ASSERT(i < this->Length(), "Math::Vec<3, T>::at(): Index out of range!");

	return data[i];
}

//-------------------------------------------------------------------------------------------------------------------//

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::const_iterator TRAP::Math::Vec<3, T>::begin() const noexcept
{
	return data.begin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::iterator TRAP::Math::Vec<3, T>::begin() noexcept
{
	return data.begin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::const_iterator TRAP::Math::Vec<3, T>::cbegin() const noexcept
{
	return data.cbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::const_reverse_iterator TRAP::Math::Vec<3, T>::rbegin() const noexcept
{
	return data.rbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::reverse_iterator TRAP::Math::Vec<3, T>::rbegin() noexcept
{
	return data.rbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::const_reverse_iterator TRAP::Math::Vec<3, T>::crbegin() const noexcept
{
	return data.crbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::const_iterator TRAP::Math::Vec<3, T>::end() const noexcept
{
	return data.end();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::iterator TRAP::Math::Vec<3, T>::end() noexcept
{
	return data.end();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::const_iterator TRAP::Math::Vec<3, T>::cend() const noexcept
{
	return data.cend();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::const_reverse_iterator TRAP::Math::Vec<3, T>::rend() const noexcept
{
	return data.rend();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::reverse_iterator TRAP::Math::Vec<3, T>::rend() noexcept
{
	return data.rend();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T>::const_reverse_iterator TRAP::Math::Vec<3, T>::crend() const noexcept
{
	return data.crend();
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator=(const Vec<3, U>& v) noexcept
{
	this->x() = static_cast<T>(v.x());
	this->y() = static_cast<T>(v.y());
	this->z() = static_cast<T>(v.z());

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator+=(const U scalar) noexcept
{
	this->x() += static_cast<T>(scalar);
	this->y() += static_cast<T>(scalar);
	this->z() += static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator+=(const Vec<3, U>& v) noexcept
{
	this->x() += static_cast<T>(v.x());
	this->y() += static_cast<T>(v.y());
	this->z() += static_cast<T>(v.z());

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator-=(const U scalar) noexcept
{
	this->x() -= static_cast<T>(scalar);
	this->y() -= static_cast<T>(scalar);
	this->z() -= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator-=(const Vec<3, U>& v) noexcept
{
	this->x() -= static_cast<T>(v.x());
	this->y() -= static_cast<T>(v.y());
	this->z() -= static_cast<T>(v.z());

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator*=(const U scalar) noexcept
{
	this->x() *= static_cast<T>(scalar);
	this->y() *= static_cast<T>(scalar);
	this->z() *= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator*=(const Vec<3, U>& v) noexcept
{
	this->x() *= static_cast<T>(v.x());
	this->y() *= static_cast<T>(v.y());
	this->z() *= static_cast<T>(v.z());

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator/=(const U scalar) noexcept
{
	this->x() /= static_cast<T>(scalar);
	this->y() /= static_cast<T>(scalar);
	this->z() /= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator/=(const Vec<3, U>& v) noexcept
{
	this->x() /= static_cast<T>(v.x());
	this->y() /= static_cast<T>(v.y());
	this->z() /= static_cast<T>(v.z());

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator++() noexcept
{
	++this->x();
	++this->y();
	++this->z();

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator--() noexcept
{
	--this->x();
	--this->y();
	--this->z();

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr const TRAP::Math::Vec<3, T> TRAP::Math::Vec<3, T>::operator++(const i32) noexcept
{
	const Vec<3, T> result(*this);
	++*this;

	return result;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr const TRAP::Math::Vec<3, T> TRAP::Math::Vec<3, T>::operator--(const i32) noexcept
{
	const Vec<3, T> result(*this);
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
	this->x() %= static_cast<T>(scalar);
	this->y() %= static_cast<T>(scalar);
	this->z() %= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator%=(const Vec<3, U>& v) noexcept
{
	this->x() %= static_cast<T>(v.x());
	this->y() %= static_cast<T>(v.y());
	this->z() %= static_cast<T>(v.z());

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator&=(const U scalar) noexcept
{
	this->x() &= static_cast<T>(scalar);
	this->y() &= static_cast<T>(scalar);
	this->z() &= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator&=(const Vec<3, U>& v) noexcept
{
	this->x() &= static_cast<T>(v.x());
	this->y() &= static_cast<T>(v.y());
	this->z() &= static_cast<T>(v.z());

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator|=(const U scalar) noexcept
{
	this->x() |= static_cast<T>(scalar);
	this->y() |= static_cast<T>(scalar);
	this->z() |= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator|=(const Vec<3, U>& v) noexcept
{
	this->x() |= static_cast<T>(v.x());
	this->y() |= static_cast<T>(v.y());
	this->z() |= static_cast<T>(v.z());

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator^=(const U scalar) noexcept
{
	this->x() ^= static_cast<T>(scalar);
	this->y() ^= static_cast<T>(scalar);
	this->z() ^= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator^=(const Vec<3, U>& v) noexcept
{
	this->x() ^= static_cast<T>(v.x());
	this->y() ^= static_cast<T>(v.y());
	this->z() ^= static_cast<T>(v.z());

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator<<=(const U scalar) noexcept
{
	this->x() <<= static_cast<T>(scalar);
	this->y() <<= static_cast<T>(scalar);
	this->z() <<= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator<<=(const Vec<3, U>& v) noexcept
{
	this->x() <<= static_cast<T>(v.x());
	this->y() <<= static_cast<T>(v.y());
	this->z() <<= static_cast<T>(v.z());

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator>>=(const U scalar) noexcept
{
	this->x() >>= static_cast<T>(scalar);
	this->y() >>= static_cast<T>(scalar);
	this->z() >>= static_cast<T>(scalar);

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
template<typename U>
requires std::is_arithmetic_v<U>
constexpr TRAP::Math::Vec<3, T>& TRAP::Math::Vec<3, T>::operator>>=(const Vec<3, U>& v) noexcept
{
	this->x() >>= static_cast<T>(v.x());
	this->y() >>= static_cast<T>(v.y());
	this->z() >>= static_cast<T>(v.z());

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] std::string TRAP::Math::Vec<3, T>::ToString() const
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if constexpr(std::same_as<T, f32>)
		return fmt::format("Vec3f({}, {}, {})", x(), y(), z());
	else if constexpr(std::same_as<T, f64>)
		return fmt::format("Vec3d({}, {}, {})", x(), y(), z());
	else if constexpr(std::same_as<T, bool>)
		return fmt::format("Vec3b({}, {}, {})", (x() ? "true" : "false"), (y() ? "true" : "false"), (z() ? "true" : "false"));
	else if constexpr(std::same_as<T, i8>)
		return fmt::format("Vec3i8({}, {}, {})", x(), y(), z());
	else if constexpr(std::same_as<T, i16>)
		return fmt::format("Vec3i16({}, {}, {})", x(), y(), z());
	else if constexpr(std::same_as<T, i32>)
		return fmt::format("Vec3i32({}, {}, {})", x(), y(), z());
	else if constexpr(std::same_as<T, i64>)
		return fmt::format("Vec3i64({}, {}, {})", x(), y(), z());
	else if constexpr(std::same_as<T, u8>)
		return fmt::format("Vec3ui8({}, {}, {})", x(), y(), z());
	else if constexpr(std::same_as<T, u16>)
		return fmt::format("Vec3ui16({}, {}, {})", x(), y(), z());
	else if constexpr(std::same_as<T, u32>)
		return fmt::format("Vec3ui32({}, {}, {})", x(), y(), z());
	else if constexpr(std::same_as<T, u64>)
		return fmt::format("Vec3ui64({}, {}, {})", x(), y(), z());
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
	return Vec<3, T>(-v.x(), -v.y(), -v.z());
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x() + scalar, v.y() + scalar, v.z() + scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar + v.x(), scalar + v.y(), scalar + v.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator+(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x() - scalar, v.y() - scalar, v.z() - scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar - v.x(), scalar - v.y(), scalar - v.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator-(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x() * scalar, v.y() * scalar, v.z() * scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar * v.x(), scalar * v.y(), scalar * v.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x() * v2.x(), v1.y() * v2.y(), v1.z() * v2.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator/(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x() / scalar, v.y() / scalar, v.z() / scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator/(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar / v.x(), scalar / v.y(), scalar / v.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator/(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x() / v2.x(), v1.y() / v2.y(), v1.z() / v2.z());
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary bit operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator%(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x() % scalar, v.y() % scalar, v.z() % scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator%(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar % v.x(), scalar % v.y(), scalar % v.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator%(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x() % v2.x(), v1.y() % v2.y(), v1.z() % v2.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator&(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x() & scalar, v.y() & scalar, v.z() & scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator&(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar & v.x(), scalar & v.y(), scalar & v.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator&(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x() & v2.x(), v1.y() & v2.y(), v1.z() & v2.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator|(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x() | scalar, v.y() | scalar, v.z() | scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator|(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar | v.x(), scalar | v.y(), scalar | v.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator|(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x() | v2.x(), v1.y() | v2.y(), v1.z() | v2.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator^(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x() ^ scalar, v.y() ^ scalar, v.z() ^ scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator^(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar ^ v.x(), scalar ^ v.y(), scalar ^ v.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator^(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x() ^ v2.x(), v1.y() ^ v2.y(), v1.z() ^ v2.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator<<(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x() << scalar, v.y() << scalar, v.z() << scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator<<(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar << v.x(), scalar << v.y(), scalar << v.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator<<(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x() << v2.x(), v1.y() << v2.y(), v1.z() << v2.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator>>(const Vec<3, T>& v, const T scalar) noexcept
{
	return Vec<3, T>(v.x() >> scalar, v.y() >> scalar, v.z() >> scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator>>(const T scalar, const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(scalar >> v.x(), scalar >> v.y(), scalar >> v.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator>>(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return Vec<3, T>(v1.x() >> v2.x(), v1.y() >> v2.y(), v1.z() >> v2.z());
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator~(const Vec<3, T>& v) noexcept
{
	return Vec<3, T>(~v.x(), ~v.y(), ~v.z());
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr bool TRAP::Math::operator==(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return v1.x() == v2.x() && v1.y() == v2.y() && v1.z() == v2.z();
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr bool TRAP::Math::operator!=(const Vec<3, T>& v1, const Vec<3, T>& v2) noexcept
{
	return !(v1 == v2);
}

constexpr TRAP::Math::Vec<3, bool> TRAP::Math::operator&&(const Vec<3, bool>& v1, const Vec<3, bool>& v2) noexcept
{
	return Vec<3, bool>(v1.x() && v2.x(), v1.y() && v2.y(), v1.z() && v2.z());
}

constexpr TRAP::Math::Vec<3, bool> TRAP::Math::operator||(const Vec<3, bool>& v1, const Vec<3, bool>& v2) noexcept
{
	return Vec<3, bool>(v1.x() || v2.x(), v1.y() || v2.y(), v1.z() || v2.z());
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
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr T& get(TRAP::Math::Vec<3, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<3, T>::Length());

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
#if __cpp_lib_unreachable >= 202202L
		else
		{
			std::unreachable();
		}
#endif /*__cpp_lib_unreachable >= 202202L*/
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 3).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr T&& get(TRAP::Math::Vec<3, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<3, T>::Length());

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
#if __cpp_lib_unreachable >= 202202L
		else
		{
			std::unreachable();
		}
#endif /*__cpp_lib_unreachable >= 202202L*/
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 3).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const T& get(const TRAP::Math::Vec<3, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<3, T>::Length());

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
#if __cpp_lib_unreachable >= 202202L
		else
		{
			std::unreachable();
		}
#endif /*__cpp_lib_unreachable >= 202202L*/
	}

	/// <summary>
	/// Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 3).
	/// This is enforced at compile time!
	/// </summary>
	/// <param name="v">Vector whose contents to extract.</param>
	/// <returns>A reference to the Ith element of v.</returns>
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const T&& get(const TRAP::Math::Vec<3, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<3, T>::Length());

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
#if __cpp_lib_unreachable >= 202202L
		else
		{
			std::unreachable();
		}
#endif /*__cpp_lib_unreachable >= 202202L*/
	}
}

#endif /*TRAP_VEC3_H*/
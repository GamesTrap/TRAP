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

#ifndef TRAP_MAT3_H
#define TRAP_MAT3_H

#include <array>

#include "Vec3.h"
#include "Core/PlatformDetection.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"

template<typename T>
requires std::floating_point<T>
struct TRAP::Math::Mat<3, 3, T>;

namespace std
{
	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr static typename TRAP::Math::Mat<3, 3, T>::col_type& get(TRAP::Math::Mat<3, 3, T>& m) noexcept;

	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr static typename TRAP::Math::Mat<3, 3, T>::col_type&& get(TRAP::Math::Mat<3, 3, T>&& m) noexcept;

	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr static const typename TRAP::Math::Mat<3, 3, T>::col_type& get(const TRAP::Math::Mat<3, 3, T>& m) noexcept;

	template<std::size_t I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr static const typename TRAP::Math::Mat<3, 3, T>::col_type&& get(const TRAP::Math::Mat<3, 3, T>&& m) noexcept;
}

namespace TRAP::Math
{
	/// <summary>
	/// Struct for a 3x3 matrix.
	/// </summary>
	template<typename T>
	requires std::floating_point<T>
	struct Mat<3, 3, T>
	{
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Mat(Mat&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~Mat() = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr Mat<3, 3, T>& operator=(Mat&&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr Mat<3, 3, T>& operator=(const Mat&) noexcept = default;

		using col_type = Vec<3, T>;
		using row_type = Vec<3, T>;
		using value_type = T;
		using size_type = uint32_t;
		using difference_type = std::ptrdiff_t;
		using iterator = std::array<col_type, 3>::iterator;
		using const_iterator = std::array<col_type, 3>::const_iterator;
		using reverse_iterator = std::array<col_type, 3>::reverse_iterator;
		using const_reverse_iterator = std::array<col_type, 3>::const_reverse_iterator;

	private:
		std::array<col_type, 3> value{};

	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		constexpr Mat() noexcept = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Mat(const Mat<3, 3, T> & m) noexcept = default;

		/// <summary>
		/// Scalar constructor.
		/// </summary>
		explicit constexpr Mat(T scalar) noexcept;
		/// <summary>
		/// Value constructor.
		/// </summary>
		constexpr Mat(T x0, T y0, T z0,
			          T x1, T y1, T z1,
			          T x2, T y2, T z2) noexcept;
		/// <summary>
		/// Column constructor.
		/// </summary>
		constexpr Mat(const col_type & v0, const col_type & v1, const col_type & v2) noexcept;

		/// <summary>
		/// Value conversion constructor.
		/// </summary>
		template<typename X1, typename Y1, typename Z1,
			     typename X2, typename Y2, typename Z2,
			     typename X3, typename Y3, typename Z3>
		requires std::floating_point<X1> && std::floating_point<Y1> && std::floating_point<Z1> &&
				 std::floating_point<X2> && std::floating_point<Y2> && std::floating_point<Z2> &&
				 std::floating_point<X3> && std::floating_point<Y3> && std::floating_point<Z3>
		constexpr Mat(X1 x1, Y1 y1, Z1 z1,
			          X2 x2, Y2 y2, Z2 z2,
			          X3 x3, Y3 y3, Z3 z3) noexcept;

		/// <summary>
		/// Column conversion constructor.
		/// </summary>
		template<typename V1, typename V2, typename V3>
		requires std::floating_point<V1> && std::floating_point<V2> && std::floating_point<V3>
		constexpr Mat(const Vec<3, V1> & v1, const Vec<3, V2> & v2, const Vec<3, V3> & v3) noexcept;

		/// <summary>
		/// Copy conversion constructor.
		/// </summary>
		template<typename U>
		requires std::floating_point<U>
		constexpr Mat(const Mat<3, 3, U> & m) noexcept;

		/// <summary>
		/// Copy conversion constructor.
		/// </summary>
		constexpr Mat(const Mat<4, 4, T> & x) noexcept;

		/// <summary>
		/// Retrieve the length of the matrix.
		/// </summary>
		/// <returns>Length.</returns>
		[[nodiscard]] static constexpr std::size_t Length() noexcept;

		/// <summary>
		/// Retrieve a column of the matrix.
		/// </summary>
		/// <param name="i">Column to retrieve.</param>
		[[nodiscard]] constexpr col_type& operator[](std::size_t i);
		/// <summary>
		/// Retrieve a column of the matrix.
		/// </summary>
		/// <param name="i">Column to retrieve.</param>
		[[nodiscard]] constexpr const col_type& operator[](std::size_t i) const;


		/// <summary>
		/// Retrieve a column of the matrix.
		/// </summary>
		/// <param name="i">Column to retrieve.</param>
		[[nodiscard]] col_type& at(std::size_t i);
		/// <summary>
		/// Retrieve a column of the matrix.
		/// </summary>
		/// <param name="i">Column to retrieve.</param>
		[[nodiscard]] const col_type& at(std::size_t i) const;

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
		template<typename U>
		requires std::floating_point<U>
		constexpr Mat<3, 3, T>& operator=(const Mat<3, 3, U>& m) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator+=(U s) noexcept;
		template<typename U>
		requires std::floating_point<U>
		constexpr Mat<3, 3, T>& operator+=(const Mat<3, 3, U>& m) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator-=(U s) noexcept;
		template<typename U>
		requires std::floating_point<U>
		constexpr Mat<3, 3, T>& operator-=(const Mat<3, 3, U>& m) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator*=(U s) noexcept;
		template<typename U>
		requires std::floating_point<U>
		constexpr Mat<3, 3, T>& operator*=(const Mat<3, 3, U>& m) noexcept;
		template<typename U>
		constexpr Mat<3, 3, T>& operator/=(U s) noexcept;
		template<typename U>
		requires std::floating_point<U>
		constexpr Mat<3, 3, T>& operator/=(const Mat<3, 3, U>& m) ;

		//Increment and decrement operators
		constexpr Mat<3, 3, T>& operator++() noexcept;
		constexpr Mat<3, 3, T>& operator--() noexcept;
		constexpr const Mat<3, 3, T> operator++(int32_t) noexcept;
		constexpr const Mat<3, 3, T> operator--(int32_t) noexcept;

		[[nodiscard]] constexpr auto operator<=>(const Mat<3, 3, T>& rhs) const = default;
		[[nodiscard]] constexpr bool operator==(const Mat<3, 3, T>& rhs) const = default;
		[[nodiscard]] constexpr bool operator!=(const Mat<3, 3, T>& rhs) const = default;

		[[nodiscard]] std::string ToString() const;
	};

	//Unary operators
	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator+(const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator-(const Mat<3, 3, T>& m) noexcept;

	//Binary operators
	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator+(const Mat<3, 3, T>& m, T scalar) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator+(T scalar, const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator+(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator-(const Mat<3, 3, T>& m, T scalar) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator-(T scalar, const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator-(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator*(const Mat<3, 3, T>& m, T scalar) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator*(T scalar, const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr typename Mat<3, 3, T>::col_type operator*(const Mat<3, 3, T>& m, const typename Mat<3, 3, T>::row_type& v) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr typename Mat<3, 3, T>::row_type operator*(const typename Mat<3, 3, T>::col_type& v, const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator*(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator/(const Mat<3, 3, T>& m, T scalar) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator/(T scalar, const Mat<3, 3, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr typename Mat<3, 3, T>::col_type operator/(const Mat<3, 3, T>& m, const typename Mat<3, 3, T>::row_type& v);

	template<typename T>
	requires std::floating_point<T>
	constexpr typename Mat<3, 3, T>::row_type operator/(const typename Mat<3, 3, T>::col_type& v, const Mat<3, 3, T>& m);

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<3, 3, T> operator/(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;

	//Boolean operators
	template<typename T>
	requires std::floating_point<T>
	constexpr bool operator==(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr bool operator!=(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept;
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	requires std::floating_point<T>
	struct hash<TRAP::Math::Mat<3, 3, T>>
	{
		[[nodiscard]] constexpr std::size_t operator()(const TRAP::Math::Mat<3, 3, T>& m) const noexcept
		{
			std::size_t seed = 0;
			hash<TRAP::Math::Vec<3, T>> hasher;
			TRAP::Utils::HashCombine(seed, hasher(std::get<0>(m)), hasher(std::get<1>(m)), hasher(std::get<2>(m)));
			return seed;
		}
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//Constructors

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const T scalar) noexcept
	: value{ col_type(scalar, 0, 0), col_type(0, scalar, 0), col_type(0, 0, scalar) }
{}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const T x0, const T y0, const T z0,
	                                    const T x1, const T y1, const T z1,
	                                    const T x2, const T y2, const T z2) noexcept
	: value{ col_type(x0, y0, z0), col_type(x1, y1, z1), col_type(x2, y2, z2) }
{}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const col_type& v0, const col_type& v1, const col_type& v2) noexcept
	: value{ col_type(v0), col_type(v1), col_type(v2) }
{}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion constructors

template<typename T>
requires std::floating_point<T>
template<typename X1, typename Y1, typename Z1,
	     typename X2, typename Y2, typename Z2,
	     typename X3, typename Y3, typename Z3>
requires std::floating_point<X1> && std::floating_point<Y1> && std::floating_point<Z1> &&
         std::floating_point<X2> && std::floating_point<Y2> && std::floating_point<Z2> &&
         std::floating_point<X3> && std::floating_point<Y3> && std::floating_point<Z3>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const X1 x1, const Y1 y1, const Z1 z1,
	                                    const X2 x2, const Y2 y2, const Z2 z2,
	                                    const X3 x3, const Y3 y3, const Z3 z3) noexcept
	: value{ col_type(x1, y1, z1), col_type(x2, y2, z2), col_type(x3, y3, z3) }
{}

template<typename T>
requires std::floating_point<T>
template<typename V1, typename V2, typename V3>
requires std::floating_point<V1> && std::floating_point<V2> && std::floating_point<V3>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const Vec<3, V1>& v1, const Vec<3, V2>& v2, const Vec<3, V3>& v3) noexcept
	: value{ col_type(v1), col_type(v2), col_type(v3) }
{}

//-------------------------------------------------------------------------------------------------------------------//
//Matrix conversions
template<typename T>
requires std::floating_point<T>
template<typename U>
requires std::floating_point<U>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const Mat<3, 3, U>& m) noexcept
	: value{ col_type(std::get<0>(m)), col_type(std::get<1>(m)), col_type(std::get<2>(m)) }
{}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T>::Mat(const Mat<4, 4, T>& x) noexcept
	: value{ col_type(x[0]), col_type(x[1]), col_type(x[2]) }
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr std::size_t TRAP::Math::Mat<3, 3, T>::Length() noexcept
{
	return 3;
}

//-------------------------------------------------------------------------------------------------------------------//
//Accesses

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr typename TRAP::Math::Mat<3, 3, T>::col_type& TRAP::Math::Mat<3, 3, T>::operator[](const std::size_t i)
{
	return this->value[i];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr const typename TRAP::Math::Mat<3, 3, T>::col_type& TRAP::Math::Mat<3, 3, T>::operator[](const std::size_t i) const
{
	return this->value[i];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] typename TRAP::Math::Mat<3, 3, T>::col_type& TRAP::Math::Mat<3, 3, T>::at(const std::size_t i)
{
	TRAP_ASSERT(i < this->Length(), "Math::Mat<3, 3, T>::at(): index out of range");

	return this->value[i];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] const typename TRAP::Math::Mat<3, 3, T>::col_type& TRAP::Math::Mat<3, 3, T>::at(const std::size_t i) const
{
	TRAP_ASSERT(i < this->Length(), "Math::Mat<3, 3, T>::at(): index out of range");

	return this->value[i];
}

//-------------------------------------------------------------------------------------------------------------------//

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::const_iterator TRAP::Math::Mat<3, 3, T>::begin() const noexcept
{
	return value.begin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::iterator TRAP::Math::Mat<3, 3, T>::begin() noexcept
{
	return value.begin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::const_iterator TRAP::Math::Mat<3, 3, T>::cbegin() const noexcept
{
	return value.cbegin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::const_reverse_iterator TRAP::Math::Mat<3, 3, T>::rbegin() const noexcept
{
	return value.rbegin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::reverse_iterator TRAP::Math::Mat<3, 3, T>::rbegin() noexcept
{
	return value.rbegin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::const_reverse_iterator TRAP::Math::Mat<3, 3, T>::crbegin() const noexcept
{
	return value.crbegin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::const_iterator TRAP::Math::Mat<3, 3, T>::end() const noexcept
{
	return value.end();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::iterator TRAP::Math::Mat<3, 3, T>::end() noexcept
{
	return value.end();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::const_iterator TRAP::Math::Mat<3, 3, T>::cend() const noexcept
{
	return value.cend();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::const_reverse_iterator TRAP::Math::Mat<3, 3, T>::rend() const noexcept
{
	return value.rend();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::reverse_iterator TRAP::Math::Mat<3, 3, T>::rend() noexcept
{
	return value.rend();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T>::const_reverse_iterator TRAP::Math::Mat<3, 3, T>::crend() const noexcept
{
	return value.crend();
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary updatable operators

template<typename T>
requires std::floating_point<T>
template<typename U>
requires std::floating_point<U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator=(const Mat<3, 3, U>& m) noexcept
{
	std::get<0>(this->value) = static_cast<T>(std::get<0>(m.value));
	std::get<1>(this->value) = static_cast<T>(std::get<1>(m.value));
	std::get<2>(this->value) = static_cast<T>(std::get<2>(m.value));

	return *this;
}

template<typename T>
requires std::floating_point<T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator+=(const U s) noexcept
{
	std::get<0>(this->value) += static_cast<T>(s);
	std::get<1>(this->value) += static_cast<T>(s);
	std::get<2>(this->value) += static_cast<T>(s);

	return *this;
}

template<typename T>
requires std::floating_point<T>
template<typename U>
requires std::floating_point<U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator+=(const Mat<3, 3, U>& m) noexcept
{
	std::get<0>(this->value) += static_cast<T>(std::get<0>(m.value));
	std::get<1>(this->value) += static_cast<T>(std::get<1>(m.value));
	std::get<2>(this->value) += static_cast<T>(std::get<2>(m.value));

	return *this;
}

template<typename T>
requires std::floating_point<T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator-=(const U s) noexcept
{
	std::get<0>(this->value) -= static_cast<T>(s);
	std::get<1>(this->value) -= static_cast<T>(s);
	std::get<2>(this->value) -= static_cast<T>(s);

	return *this;
}

template<typename T>
requires std::floating_point<T>
template<typename U>
requires std::floating_point<U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator-=(const Mat<3, 3, U>& m) noexcept
{
	std::get<0>(this->value) -= static_cast<T>(std::get<0>(m.value));
	std::get<1>(this->value) -= static_cast<T>(std::get<1>(m.value));
	std::get<2>(this->value) -= static_cast<T>(std::get<2>(m.value));

	return *this;
}

template<typename T>
requires std::floating_point<T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator*=(const U s) noexcept
{
	std::get<0>(this->value) *= static_cast<T>(s);
	std::get<1>(this->value) *= static_cast<T>(s);
	std::get<2>(this->value) *= static_cast<T>(s);

	return *this;
}

template<typename T>
requires std::floating_point<T>
template<typename U>
requires std::floating_point<U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator*=(const Mat<3, 3, U>& m) noexcept
{
	return (*this = *this * m);
}

template<typename T>
requires std::floating_point<T>
template<typename U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator/=(const U s) noexcept
{
	std::get<0>(this->value) /= static_cast<T>(s);
	std::get<1>(this->value) /= static_cast<T>(s);
	std::get<2>(this->value) /= static_cast<T>(s);

	return *this;
}

template<typename T>
requires std::floating_point<T>
template<typename U>
requires std::floating_point<U>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator/=(const Mat<3, 3, U>& m)
{
	return *this *= Inverse(m);
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator++() noexcept
{
	++std::get<0>(this->value);
	++std::get<1>(this->value);
	++std::get<2>(this->value);

	return *this;
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T>& TRAP::Math::Mat<3, 3, T>::operator--() noexcept
{
	--std::get<0>(this->value);
	--std::get<1>(this->value);
	--std::get<2>(this->value);

	return *this;
}

template<typename T>
requires std::floating_point<T>
constexpr const TRAP::Math::Mat<3, 3, T> TRAP::Math::Mat<3, 3, T>::operator++(const int32_t) noexcept
{
	const Mat<3, 3, T> result(*this);
	++*this;

	return result;
}

template<typename T>
requires std::floating_point<T>
constexpr const TRAP::Math::Mat<3, 3, T> TRAP::Math::Mat<3, 3, T>::operator--(const int32_t) noexcept
{
	const Mat<3, 3, T> result(*this);
	--*this;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] std::string TRAP::Math::Mat<3, 3, T>::ToString() const
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	std::string postfix;
	if constexpr(std::same_as<T, float>)
		postfix = "f";
	else if constexpr(std::same_as<T, double>)
		postfix = "d";
	else
		return "Unknown type";

	return fmt::format("Mat3{}(({},{},{}), ({},{},{}), ({},{},{}))", postfix, std::get<0>(std::get<0>(value)),
	                   std::get<1>(std::get<0>(value)), std::get<2>(std::get<0>(value)), std::get<0>(std::get<1>(value)),
					   std::get<1>(std::get<1>(value)), std::get<2>(std::get<1>(value)), std::get<0>(std::get<2>(value)),
					   std::get<1>(std::get<2>(value)), std::get<2>(std::get<2>(value)));
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const Mat<3, 3, T>& m) noexcept
{
	return m;
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const Mat<3, 3, T>& m) noexcept
{
	return Mat<3, 3, T>(-std::get<0>(m), -std::get<1>(m), -std::get<2>(m));
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const Mat<3, 3, T>& m, const T scalar) noexcept
{
	return Mat<3, 3, T>(std::get<0>(m) + scalar, std::get<1>(m) + scalar, std::get<2>(m) + scalar);
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const T scalar, const Mat<3, 3, T>& m) noexcept
{
	return Mat<3, 3, T>(std::get<0>(m) + scalar, std::get<1>(m) + scalar, std::get<2>(m) + scalar);
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator+(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	return Mat<3, 3, T>(std::get<0>(m1) + std::get<0>(m2), std::get<1>(m1) + std::get<1>(m2), std::get<2>(m1) + std::get<2>(m2));
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const Mat<3, 3, T>& m, const T scalar) noexcept
{
	return Mat<3, 3, T>(std::get<0>(m) - scalar, std::get<1>(m) - scalar, std::get<2>(m) - scalar);
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const T scalar, const Mat<3, 3, T>& m) noexcept
{
	return Mat<3, 3, T>(scalar - std::get<0>(m), scalar - std::get<1>(m), scalar - std::get<2>(m));
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator-(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	return Mat<3, 3, T>(std::get<0>(m1) - std::get<0>(m2), std::get<1>(m1) - std::get<1>(m2), std::get<2>(m1) - std::get<2>(m2));
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator*(const Mat<3, 3, T>& m, const T scalar) noexcept
{
	return Mat<3, 3, T>(std::get<0>(m) * scalar, std::get<1>(m) * scalar, std::get<2>(m) * scalar);
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator*(const T scalar, const Mat<3, 3, T>& m) noexcept
{
	return Mat<3, 3, T>(std::get<0>(m) * scalar, std::get<1>(m) * scalar, std::get<2>(m) * scalar);
}

template<typename T>
requires std::floating_point<T>
constexpr typename TRAP::Math::Mat<3, 3, T>::col_type TRAP::Math::operator*(const Mat<3, 3, T>& m,
                                                                           const typename Mat<3, 3, T>::row_type& v) noexcept
{
	return typename Mat<3, 3, T>::col_type(std::get<0>(std::get<0>(m)) * v.x() + std::get<0>(std::get<1>(m)) * v.y() + std::get<0>(std::get<2>(m)) * v.z(),
		                                  std::get<1>(std::get<0>(m)) * v.x() + std::get<1>(std::get<1>(m)) * v.y() + std::get<1>(std::get<2>(m)) * v.z(),
		                                  std::get<2>(std::get<0>(m)) * v.x() + std::get<2>(std::get<1>(m)) * v.y() + std::get<2>(std::get<2>(m)) * v.z());
}

template<typename T>
requires std::floating_point<T>
constexpr typename TRAP::Math::Mat<3, 3, T>::row_type TRAP::Math::operator*(const typename Mat<3, 3, T>::col_type& v,
                                                                           const Mat<3, 3, T>& m) noexcept
{
	return typename Mat<3, 3, T>::row_type(std::get<0>(std::get<0>(m)) * v.x() + std::get<1>(std::get<0>(m)) * v.y() + std::get<2>(std::get<0>(m)) * v.z(),
		                                  std::get<0>(std::get<1>(m)) * v.x() + std::get<1>(std::get<1>(m)) * v.y() + std::get<2>(std::get<1>(m)) * v.z(),
		                                  std::get<0>(std::get<2>(m)) * v.x() + std::get<1>(std::get<2>(m)) * v.y() + std::get<2>(std::get<2>(m)) * v.z());
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator*(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	const T srcA00 = std::get<0>(std::get<0>(m1));
	const T srcA01 = std::get<1>(std::get<0>(m1));
	const T srcA02 = std::get<2>(std::get<0>(m1));
	const T srcA10 = std::get<0>(std::get<1>(m1));
	const T srcA11 = std::get<1>(std::get<1>(m1));
	const T srcA12 = std::get<2>(std::get<1>(m1));
	const T srcA20 = std::get<0>(std::get<2>(m1));
	const T srcA21 = std::get<1>(std::get<2>(m1));
	const T srcA22 = std::get<2>(std::get<2>(m1));

	const T srcB00 = std::get<0>(std::get<0>(m2));
	const T srcB01 = std::get<1>(std::get<0>(m2));
	const T srcB02 = std::get<2>(std::get<0>(m2));
	const T srcB10 = std::get<0>(std::get<1>(m2));
	const T srcB11 = std::get<1>(std::get<1>(m2));
	const T srcB12 = std::get<2>(std::get<1>(m2));
	const T srcB20 = std::get<0>(std::get<2>(m2));
	const T srcB21 = std::get<1>(std::get<2>(m2));
	const T srcB22 = std::get<2>(std::get<2>(m2));

	Mat<3, 3, T> result;
	std::get<0>(std::get<0>(result)) = srcA00 * srcB00 + srcA10 * srcB01 + srcA20 * srcB02;
	std::get<1>(std::get<0>(result)) = srcA01 * srcB00 + srcA11 * srcB01 + srcA21 * srcB02;
	std::get<2>(std::get<0>(result)) = srcA02 * srcB00 + srcA12 * srcB01 + srcA22 * srcB02;
	std::get<0>(std::get<1>(result)) = srcA00 * srcB10 + srcA10 * srcB11 + srcA20 * srcB12;
	std::get<1>(std::get<1>(result)) = srcA01 * srcB10 + srcA11 * srcB11 + srcA21 * srcB12;
	std::get<2>(std::get<1>(result)) = srcA02 * srcB10 + srcA12 * srcB11 + srcA22 * srcB12;
	std::get<0>(std::get<2>(result)) = srcA00 * srcB20 + srcA10 * srcB21 + srcA20 * srcB22;
	std::get<1>(std::get<2>(result)) = srcA01 * srcB20 + srcA11 * srcB21 + srcA21 * srcB22;
	std::get<2>(std::get<2>(result)) = srcA02 * srcB20 + srcA12 * srcB21 + srcA22 * srcB22;

	return result;
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator/(const Mat<3, 3, T>& m, const T scalar) noexcept
{
	return Mat<3, 3, T>(std::get<0>(m) / scalar, std::get<1>(m) / scalar, std::get<2>(m) / scalar);
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator/(const T scalar, const Mat<3, 3, T>& m) noexcept
{
	return Mat<3, 3, T>(scalar / std::get<0>(m), scalar / std::get<1>(m), scalar / std::get<2>(m));
}

template<typename T>
requires std::floating_point<T>
constexpr typename TRAP::Math::Mat<3, 3, T>::col_type TRAP::Math::operator/(const Mat<3, 3, T>& m,
                                                                           const typename Mat<3, 3, T>::row_type& v)
{
	return Inverse(m) * v;
}

template<typename T>
requires std::floating_point<T>
constexpr typename TRAP::Math::Mat<3, 3, T>::row_type TRAP::Math::operator/(const typename Mat<3, 3, T>::col_type& v,
                                                                           const Mat<3, 3, T>& m)
{
	return v * Inverse(m);
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::operator/(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	Mat<3, 3, T> m1Copy(m1);

	return m1Copy /= m2;
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
requires std::floating_point<T>
constexpr bool TRAP::Math::operator==(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	return (std::get<0>(m1) == std::get<0>(m2)) && (std::get<1>(m1) == std::get<1>(m2)) && (std::get<2>(m1) == std::get<2>(m2));
}

template<typename T>
requires std::floating_point<T>
constexpr bool TRAP::Math::operator!=(const Mat<3, 3, T>& m1, const Mat<3, 3, T>& m2) noexcept
{
	return (std::get<0>(m1) != std::get<0>(m2)) || (std::get<1>(m1) != std::get<1>(m2)) || (std::get<2>(m1) != std::get<2>(m2));
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
	[[nodiscard]] constexpr typename TRAP::Math::Mat<3, 3, T>::col_type& get(TRAP::Math::Mat<3, 3, T>& m) noexcept
	{
		static_assert(I < TRAP::Math::Mat<3, 3, T>::Length());

		return m[I];
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
	[[nodiscard]] constexpr typename TRAP::Math::Mat<3, 3, T>::col_type&& get(TRAP::Math::Mat<3, 3, T>&& m) noexcept
	{
		static_assert(I < TRAP::Math::Mat<3, 3, T>::Length());

		return std::move(m[I]);
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
	[[nodiscard]] constexpr const typename TRAP::Math::Mat<3, 3, T>::col_type& get(const TRAP::Math::Mat<3, 3, T>& m) noexcept
	{
		static_assert(I < TRAP::Math::Mat<3, 3, T>::Length());

		return m[I];
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
	[[nodiscard]] constexpr const typename TRAP::Math::Mat<3, 3, T>::col_type&& get(const TRAP::Math::Mat<3, 3, T>&& m) noexcept
	{
		static_assert(I < TRAP::Math::Mat<3, 3, T>::Length());

		return std::move(m[I]);
	}
}

#endif /*TRAP_MAT3_H*/

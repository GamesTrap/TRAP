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

#ifndef TRAP_MAT4_H
#define TRAP_MAT4_H

#include <array>

#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Utils/Utils.h"
#include "Vec4.h"

template<typename T>
requires std::floating_point<T>
struct TRAP::Math::Mat<4, 4, T>;

namespace std
{
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr typename TRAP::Math::Mat<4, 4, T>::col_type& get(TRAP::Math::Mat<4, 4, T>& m) noexcept;

	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr typename TRAP::Math::Mat<4, 4, T>::col_type&& get(TRAP::Math::Mat<4, 4, T>&& m) noexcept;

	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const typename TRAP::Math::Mat<4, 4, T>::col_type& get(const TRAP::Math::Mat<4, 4, T>& m) noexcept;

	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const typename TRAP::Math::Mat<4, 4, T>::col_type&& get(const TRAP::Math::Mat<4, 4, T>&& m) noexcept;
}

namespace TRAP::Math
{
	/// @brief Struct for a 4x4 matrix.
	template<typename T>
	requires std::floating_point<T>
	struct Mat<4, 4, T>
	{
		/// @brief Move constructor.
		constexpr Mat(Mat&&) noexcept = default;
		/// @brief Destructor.
		constexpr ~Mat() = default;
		/// @brief Move assignment operator.
		constexpr Mat<4, 4, T>& operator=(Mat&&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr Mat<4, 4, T>& operator=(const Mat&) noexcept = default;

		using col_type = Vec<4, T>;
		using row_type = Vec<4, T>;
		using value_type = T;
		using size_type = u32;
		using difference_type = isize;
		using iterator = std::array<col_type, 4>::iterator;
		using const_iterator = std::array<col_type, 4>::const_iterator;
		using reverse_iterator = std::array<col_type, 4>::reverse_iterator;
		using const_reverse_iterator = std::array<col_type, 4>::const_reverse_iterator;

	private:
		std::array<col_type, 4> value{};

	public:
		/// @brief Constructor.
		constexpr Mat() noexcept = default;
		/// @brief Copy constructor.
		constexpr Mat(const Mat<4, 4, T> & m) noexcept = default;

		/// @brief Scalar constructor.
		explicit constexpr Mat(T scalar) noexcept;
		/// @brief Value constructor.
		constexpr Mat(T x0, T y0, T z0, T w0,
			          T x1, T y1, T z1, T w1,
			          T x2, T y2, T z2, T w2,
			          T x3, T y3, T z3, T w3) noexcept;
		/// @brief Column constructor.
		constexpr Mat(const col_type & v0, const col_type & v1, const col_type & v2, const col_type & v3) noexcept;

		/// @brief Value conversion constructor.
		template<typename X1, typename Y1, typename Z1, typename W1,
				 typename X2, typename Y2, typename Z2, typename W2,
				 typename X3, typename Y3, typename Z3, typename W3,
				 typename X4, typename Y4, typename Z4, typename W4>
		requires std::floating_point<X1> && std::floating_point<Y1> && std::floating_point<Z1> && std::floating_point<W1> &&
		         std::floating_point<X2> && std::floating_point<Y2> && std::floating_point<Z2> && std::floating_point<W2> &&
		         std::floating_point<X3> && std::floating_point<Y3> && std::floating_point<Z3> && std::floating_point<W3> &&
		         std::floating_point<X4> && std::floating_point<Y4> && std::floating_point<Z4> && std::floating_point<W4>
		inline constexpr Mat(X1 x1, Y1 y1, Z1 z1, W1 w1,
			                 X2 x2, Y2 y2, Z2 z2, W2 w2,
			                 X3 x3, Y3 y3, Z3 z3, W3 w3,
			                 X4 x4, Y4 y4, Z4 z4, W4 w4)
			: value{ col_type(x1, y1, z1, w1), col_type(x2, y2, z2, w2), col_type(x3, y3, z3, w3), col_type(x4, y4, z4, w4) }
		{}

		/// @brief Column conversion constructor.
		template<typename V1, typename V2, typename V3, typename V4>
		requires std::floating_point<V1> && std::floating_point<V2> && std::floating_point<V3> && std::floating_point<V4>
		inline constexpr Mat(const Vec<4, V1> & v1, const Vec<4, V2> & v2, const Vec<4, V3> & v3, const Vec<4, V4> & v4)
			: value{ col_type(v1), col_type(v2), col_type(v3), col_type(v4) }
		{}

		/// @brief Copy conversion constructor.
		template<typename U>
		requires std::floating_point<U>
		inline constexpr Mat(const Mat<4, 4, U> & m) noexcept
			: value{ col_type(std::get<0>(m)), col_type(std::get<1>(m)), col_type(std::get<2>(m)), col_type(std::get<3>(m)) }
		{}

		/// @brief Copy conversion constructor.
		inline constexpr Mat(const Mat<3, 3, T> & x) noexcept
			: value{ col_type(x[0], 0), col_type(x[1], 0), col_type(x[2], 0), col_type(0, 0, 0, 1) }
		{}

		/// @brief Retrieve the length of the matrix.
		/// @return Length.
		[[nodiscard]] static constexpr usize Length() noexcept;

		/// @brief Retrieve a column of the matrix.
		/// @param i Column to retrieve.
		[[nodiscard]] constexpr col_type& operator[](usize i);
		/// @brief Retrieve a column of the matrix.
		/// @param i Column to retrieve.
		[[nodiscard]] constexpr const col_type& operator[](usize i) const;


		/// @brief Retrieve a column of the matrix.
		/// @param i Column to retrieve.
		[[nodiscard]] col_type& at(usize i);
		/// @brief Retrieve a column of the matrix.
		/// @param i Column to retrieve.
		[[nodiscard]] const col_type& at(usize i) const;

		//Iterator

		/// @brief Returns an iterator to the first element of the matrix.
		/// @return Iterator to the first element.
		[[nodiscard]] constexpr const_iterator begin() const noexcept;
		/// @brief Returns an iterator to the first element of the matrix.
		/// @return Iterator to the first element.
		[[nodiscard]] constexpr iterator begin() noexcept;
		/// @brief Returns an iterator to the first element of the matrix.
		/// @return Iterator to the first element.
		[[nodiscard]] constexpr const_iterator cbegin() const noexcept;
		/// @brief Returns a reverse iterator to the first element of the reversed matrix.
		///        It corresponds to the last element of the non-reversed matrix.
		/// @return Reverse iterator to the first element.
		[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept;
		/// @brief Returns a reverse iterator to the first element of the reversed matrix.
		///        It corresponds to the last element of the non-reversed matrix.
		/// @return Reverse iterator to the first element.
		[[nodiscard]] constexpr reverse_iterator rbegin() noexcept;
		/// @brief Returns a reverse iterator to the first element of the reversed matrix.
		///        It corresponds to the last element of the non-reversed matrix.
		/// @return Reverse iterator to the first element.
		[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept;
		/// @brief Returns an iterator to the element following the last element of the matrix.
		/// @return Iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr const_iterator end() const noexcept;
		/// @brief Returns an iterator to the element following the last element of the matrix.
		/// @return Iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr iterator end() noexcept;
		/// @brief Returns an iterator to the element following the last element of the matrix.
		/// @return Iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr const_iterator cend() const noexcept;
		/// @brief Returns a reverse iterator to the element following the last element of the reversed matrix.
		///        It corresponds to the element preceding the first element of the non-reversed matrix.
		/// @return Reverse iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept;
		/// @brief Returns a reverse iterator to the element following the last element of the reversed matrix.
		///        It corresponds to the element preceding the first element of the non-reversed matrix.
		/// @return Reverse iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr reverse_iterator rend() noexcept;
		/// @brief Returns a reverse iterator to the element following the last element of the reversed matrix.
		///        It corresponds to the element preceding the first element of the non-reversed matrix.
		/// @return Reverse iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept;

		//Unary arithmetic operators
		template<typename U>
		requires std::floating_point<U>
		inline constexpr Mat<4, 4, T>& operator=(const Mat<4, 4, U>& m) noexcept
		{
			std::get<0>(this->value) = static_cast<col_type>(std::get<0>(m));
			std::get<1>(this->value) = static_cast<col_type>(std::get<1>(m));
			std::get<2>(this->value) = static_cast<col_type>(std::get<2>(m));
			std::get<3>(this->value) = static_cast<col_type>(std::get<3>(m));

			return *this;
		}
		template<typename U>
		inline constexpr Mat<4, 4, T>& operator+=(U s) noexcept
		{
			std::get<0>(this->value) += static_cast<T>(s);
			std::get<1>(this->value) += static_cast<T>(s);
			std::get<2>(this->value) += static_cast<T>(s);
			std::get<3>(this->value) += static_cast<T>(s);

			return *this;
		}
		template<typename U>
		requires std::floating_point<U>
		inline constexpr Mat<4, 4, T>& operator+=(const Mat<4, 4, U>& m) noexcept
		{
			std::get<0>(this->value) += static_cast<col_type>(std::get<0>(m));
			std::get<1>(this->value) += static_cast<col_type>(std::get<1>(m));
			std::get<2>(this->value) += static_cast<col_type>(std::get<2>(m));
			std::get<3>(this->value) += static_cast<col_type>(std::get<3>(m));

			return *this;
		}
		template<typename U>
		inline constexpr Mat<4, 4, T>& operator-=(U s) noexcept
		{
			std::get<0>(this->value) -= static_cast<T>(s);
			std::get<1>(this->value) -= static_cast<T>(s);
			std::get<2>(this->value) -= static_cast<T>(s);
			std::get<3>(this->value) -= static_cast<T>(s);

			return *this;
		}
		template<typename U>
		requires std::floating_point<U>
		inline constexpr Mat<4, 4, T>& operator-=(const Mat<4, 4, U>& m) noexcept
		{
			std::get<0>(this->value) -= static_cast<col_type>(std::get<0>(m));
			std::get<1>(this->value) -= static_cast<col_type>(std::get<1>(m));
			std::get<2>(this->value) -= static_cast<col_type>(std::get<2>(m));
			std::get<3>(this->value) -= static_cast<col_type>(std::get<3>(m));

			return *this;
		}
		template<typename U>
		inline constexpr Mat<4, 4, T>& operator*=(U s) noexcept
		{
			std::get<0>(this->value) *= static_cast<T>(s);
			std::get<1>(this->value) *= static_cast<T>(s);
			std::get<2>(this->value) *= static_cast<T>(s);
			std::get<3>(this->value) *= static_cast<T>(s);

			return *this;
		}
		template<typename U>
		requires std::floating_point<U>
		inline constexpr Mat<4, 4, T>& operator*=(const Mat<4, 4, U>& m) noexcept
		{
			std::get<0>(this->value) *= static_cast<col_type>(std::get<0>(m));
			std::get<1>(this->value) *= static_cast<col_type>(std::get<1>(m));
			std::get<2>(this->value) *= static_cast<col_type>(std::get<2>(m));
			std::get<3>(this->value) *= static_cast<col_type>(std::get<3>(m));

			return *this;
		}
		template<typename U>
		inline constexpr Mat<4, 4, T>& operator/=(U s) noexcept
		{
			std::get<0>(this->value) /= static_cast<T>(s);
			std::get<1>(this->value) /= static_cast<T>(s);
			std::get<2>(this->value) /= static_cast<T>(s);
			std::get<3>(this->value) /= static_cast<T>(s);

			return *this;
		}
		template<typename U>
		requires std::floating_point<U>
		inline constexpr Mat<4, 4, T>& operator/=(const Mat<4, 4, U>& m)
		{
			return *this *= Inverse(m);
		}

		//Increment and decrement operators
		constexpr Mat<4, 4, T>& operator++() noexcept;
		constexpr Mat<4, 4, T>& operator--() noexcept;
		constexpr const Mat<4, 4, T> operator++(i32) noexcept;
		constexpr const Mat<4, 4, T> operator--(i32) noexcept;

		[[nodiscard]] consteval auto operator<=>(const Mat<4, 4, T>& rhs) const noexcept = delete;

		inline constexpr void Swap(Mat<4, 4, T>& other) noexcept(std::is_nothrow_move_constructible_v<Mat<4, 4, T>> &&
		                                                         std::is_nothrow_move_assignable_v<Mat<4, 4, T>>)
		{
			std::swap(value, other.value);
		}
	};

	//Unary operators
	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<4, 4, T> operator+(const Mat<4, 4, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	constexpr Mat<4, 4, T> operator-(const Mat<4, 4, T>& m) noexcept;

	//Binary operators
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator+(const Mat<4, 4, T>& m, T scalar) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator+(T scalar, const Mat<4, 4, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator+(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator-(const Mat<4, 4, T>& m, T scalar) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator-(T scalar, const Mat<4, 4, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator-(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator*(const Mat<4, 4, T>& m, T scalar) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator*(T scalar, const Mat<4, 4, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr typename Mat<4, 4, T>::col_type operator*(const Mat<4, 4, T>& m, const typename Mat<4, 4, T>::row_type& v) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr typename Mat<4, 4, T>::row_type operator*(const typename Mat<4, 4, T>::col_type& v, const Mat<4, 4, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator*(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator/(const Mat<4, 4, T>& m, T scalar) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator/(T scalar, const Mat<4, 4, T>& m) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr typename Mat<4, 4, T>::col_type operator/(const Mat<4, 4, T>& m, const typename Mat<4, 4, T>::row_type& v);

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr typename Mat<4, 4, T>::row_type operator/(const typename Mat<4, 4, T>::col_type& v, const Mat<4, 4, T>& m);

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> operator/(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept;

	//Boolean operators
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr bool operator==(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr bool operator!=(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept;
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	requires std::floating_point<T>
	struct hash<TRAP::Math::Mat<4, 4, T>>
	{
		[[nodiscard]] constexpr usize operator()(const TRAP::Math::Mat<4, 4, T>& m) const noexcept
		{
			usize seed = 0;
			constexpr hash<TRAP::Math::Vec<4, T>> hasher;
			TRAP::Utils::HashCombine(seed, hasher(std::get<0>(m)), hasher(std::get<1>(m)), hasher(std::get<2>(m)), hasher(std::get<3>(m)));
			return seed;
		}
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------//
//Constructors

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<4, 4, T>::Mat(const T scalar) noexcept
	: value{ col_type(scalar, 0, 0, 0), col_type(0, scalar, 0, 0), col_type(0, 0, scalar, 0), col_type(0, 0, 0, scalar) }
{}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<4, 4, T>::Mat(const T x0, const T y0, const T z0, const T w0,
	                                    const T x1, const T y1, const T z1, const T w1,
	                                    const T x2, const T y2, const T z2, const T w2,
	                                    const T x3, const T y3, const T z3, const T w3) noexcept
	: value{ col_type(x0, y0, z0, w0), col_type(x1, y1, z1, w1), col_type(x2, y2, z2, w2), col_type(x3, y3, z3, w3) }
{}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<4, 4, T>::Mat(const col_type& v0, const col_type& v1, const col_type& v2, const col_type& v3) noexcept
	: value{ col_type(v0), col_type(v1), col_type(v2), col_type(v3) }
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr usize TRAP::Math::Mat<4, 4, T>::Length() noexcept
{
	return 4;
}

//-------------------------------------------------------------------------------------------------------------------//
//Accesses

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr typename TRAP::Math::Mat<4, 4, T>::col_type& TRAP::Math::Mat<4, 4, T>::operator[](const usize i)
{
	return this->value[i];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr const typename TRAP::Math::Mat<4, 4, T>::col_type& TRAP::Math::Mat<4, 4, T>::operator[](const usize i) const
{
	return this->value[i];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] typename TRAP::Math::Mat<4, 4, T>::col_type& TRAP::Math::Mat<4, 4, T>::at(const usize i)
{
	TRAP_ASSERT(i < this->Length(), "Math::Mat<4, 4, T>::at(): Index out of range!");

	return this->value[i];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] const typename TRAP::Math::Mat<4, 4, T>::col_type& TRAP::Math::Mat<4, 4, T>::at(const usize i) const
{
	TRAP_ASSERT(i < this->Length(), "Math::Mat<4, 4, T>::at(): Index out of range!");

	return this->value[i];
}


//-------------------------------------------------------------------------------------------------------------------//

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::const_iterator TRAP::Math::Mat<4, 4, T>::begin() const noexcept
{
	return value.begin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::iterator TRAP::Math::Mat<4, 4, T>::begin() noexcept
{
	return value.begin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::const_iterator TRAP::Math::Mat<4, 4, T>::cbegin() const noexcept
{
	return value.cbegin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::const_reverse_iterator TRAP::Math::Mat<4, 4, T>::rbegin() const noexcept
{
	return value.rbegin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::reverse_iterator TRAP::Math::Mat<4, 4, T>::rbegin() noexcept
{
	return value.rbegin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::const_reverse_iterator TRAP::Math::Mat<4, 4, T>::crbegin() const noexcept
{
	return value.crbegin();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::const_iterator TRAP::Math::Mat<4, 4, T>::end() const noexcept
{
	return value.end();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::iterator TRAP::Math::Mat<4, 4, T>::end() noexcept
{
	return value.end();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::const_iterator TRAP::Math::Mat<4, 4, T>::cend() const noexcept
{
	return value.cend();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::const_reverse_iterator TRAP::Math::Mat<4, 4, T>::rend() const noexcept
{
	return value.rend();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::reverse_iterator TRAP::Math::Mat<4, 4, T>::rend() noexcept
{
	return value.rend();
}

template<class T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T>::const_reverse_iterator TRAP::Math::Mat<4, 4, T>::crend() const noexcept
{
	return value.crend();
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator++() noexcept
{
	++std::get<0>(this->value);
	++std::get<1>(this->value);
	++std::get<2>(this->value);
	++std::get<3>(this->value);

	return *this;
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<4, 4, T>& TRAP::Math::Mat<4, 4, T>::operator--() noexcept
{
	--std::get<0>(this->value);
	--std::get<1>(this->value);
	--std::get<2>(this->value);
	--std::get<3>(this->value);

	return *this;
}

template<typename T>
requires std::floating_point<T>
constexpr const TRAP::Math::Mat<4, 4, T> TRAP::Math::Mat<4, 4, T>::operator++(const i32) noexcept
{
	const Mat<4, 4, T> result(*this);
	++*this;

	return result;
}

template<typename T>
requires std::floating_point<T>
constexpr const TRAP::Math::Mat<4, 4, T> TRAP::Math::Mat<4, 4, T>::operator--(const i32) noexcept
{
	const Mat<4, 4, T> result(*this);
	--*this;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
struct fmt::formatter<TRAP::Math::Mat<4, 4, T>>
{
    [[nodiscard]] static constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    [[nodiscard]] static fmt::format_context::iterator format(const TRAP::Math::Mat<4, 4, T>& mat, fmt::format_context& ctx)
    {
		char postfix = '\0';
		if constexpr(std::same_as<T, f32>)
			postfix = 'f';
		else if constexpr(std::same_as<T, f64>)
			postfix = 'd';


		return fmt::format_to(ctx.out(), "Mat4{}(({},{},{},{}), ({},{},{},{}), ({},{},{},{}), ({},{},{},{}))", postfix,
						      std::get<0>(std::get<0>(mat)), std::get<1>(std::get<0>(mat)),
						      std::get<2>(std::get<0>(mat)), std::get<3>(std::get<0>(mat)),
						      std::get<0>(std::get<1>(mat)), std::get<1>(std::get<1>(mat)),
						      std::get<2>(std::get<1>(mat)), std::get<3>(std::get<1>(mat)),
						      std::get<0>(std::get<2>(mat)), std::get<1>(std::get<2>(mat)),
						      std::get<2>(std::get<2>(mat)), std::get<3>(std::get<2>(mat)),
						      std::get<0>(std::get<3>(mat)), std::get<1>(std::get<3>(mat)),
						      std::get<2>(std::get<3>(mat)), std::get<3>(std::get<3>(mat)));
    }
};

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator+(const Mat<4, 4, T>& m) noexcept
{
	return m;
}

template<typename T>
requires std::floating_point<T>
constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator-(const Mat<4, 4, T>& m) noexcept
{
	return Mat<4, 4, T>(-std::get<0>(m), -std::get<1>(m), -std::get<2>(m), -std::get<3>(m));
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator+(const Mat<4, 4, T>& m, const T scalar) noexcept
{
	return Mat<4, 4, T>(std::get<0>(m) + scalar, std::get<1>(m) + scalar, std::get<2>(m) + scalar, std::get<3>(m) + scalar);
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator+(const T scalar, const Mat<4, 4, T>& m) noexcept
{
	return Mat<4, 4, T>(std::get<0>(m) + scalar, std::get<1>(m) + scalar, std::get<2>(m) + scalar, std::get<3>(m) + scalar);
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator+(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept
{
	return Mat<4, 4, T>(std::get<0>(m1) + std::get<0>(m2), std::get<1>(m1) + std::get<1>(m2), std::get<2>(m1) + std::get<2>(m2), std::get<3>(m1) + std::get<3>(m2));
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator-(const Mat<4, 4, T>& m, const T scalar) noexcept
{
	return Mat<4, 4, T>(std::get<0>(m) - scalar, std::get<1>(m) - scalar, std::get<2>(m) - scalar, std::get<3>(m) - scalar);
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator-(const T scalar, const Mat<4, 4, T>& m) noexcept
{
	return Mat<4, 4, T>(scalar - std::get<0>(m), scalar - std::get<1>(m), scalar - std::get<2>(m), scalar - std::get<3>(m));
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator-(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept
{
	return Mat<4, 4, T>(std::get<0>(m1) - std::get<0>(m2), std::get<1>(m1) - std::get<1>(m2), std::get<2>(m1) - std::get<2>(m2), std::get<3>(m1) - std::get<3>(m2));
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator*(const Mat<4, 4, T>& m, const T scalar) noexcept
{
	return Mat<4, 4, T>(std::get<0>(m) * scalar, std::get<1>(m) * scalar, std::get<2>(m) * scalar, std::get<3>(m) * scalar);
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator*(const T scalar, const Mat<4, 4, T>& m) noexcept
{
	return Mat<4, 4, T>(std::get<0>(m) * scalar, std::get<1>(m) * scalar, std::get<2>(m) * scalar, std::get<3>(m) * scalar);
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr typename TRAP::Math::Mat<4, 4, T>::col_type TRAP::Math::operator*(const Mat<4, 4, T>& m,
                                                                                          const typename Mat<4, 4, T>::row_type& v) noexcept
{
	typename Mat<4, 4, T>::col_type const mov0(std::get<0>(v));
	typename Mat<4, 4, T>::col_type const mov1(std::get<1>(v));
	typename Mat<4, 4, T>::col_type const mul0 = std::get<0>(m) * mov0;
	typename Mat<4, 4, T>::col_type const mul1 = std::get<1>(m) * mov1;
	typename Mat<4, 4, T>::col_type const add0 = mul0 + mul1;
	typename Mat<4, 4, T>::col_type const mov2(std::get<2>(v));
	typename Mat<4, 4, T>::col_type const mov3(std::get<3>(v));
	typename Mat<4, 4, T>::col_type const mul2 = std::get<2>(m) * mov2;
	typename Mat<4, 4, T>::col_type const mul3 = std::get<3>(m) * mov3;
	typename Mat<4, 4, T>::col_type const add1 = mul2 + mul3;
	typename Mat<4, 4, T>::col_type const add2 = add0 + add1;

	return add2;
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr typename TRAP::Math::Mat<4, 4, T>::row_type TRAP::Math::operator*(const typename Mat<4, 4, T>::col_type& v,
                                                                                          const Mat<4, 4, T>& m) noexcept
{
	return typename Mat<4, 4, T>::row_type(std::get<0>(std::get<0>(m)) * std::get<0>(v) + std::get<1>(std::get<0>(m)) * std::get<1>(v) + std::get<2>(std::get<0>(m)) * std::get<2>(v) + std::get<3>(std::get<0>(m)) * std::get<3>(v),
		                                  std::get<0>(std::get<1>(m)) * std::get<0>(v) + std::get<1>(std::get<1>(m)) * std::get<1>(v) + std::get<2>(std::get<1>(m)) * std::get<2>(v) + std::get<3>(std::get<1>(m)) * std::get<3>(v),
		                                  std::get<0>(std::get<2>(m)) * std::get<0>(v) + std::get<1>(std::get<2>(m)) * std::get<1>(v) + std::get<2>(std::get<2>(m)) * std::get<2>(v) + std::get<3>(std::get<2>(m)) * std::get<3>(v),
		                                  std::get<0>(std::get<3>(m)) * std::get<0>(v) + std::get<1>(std::get<3>(m)) * std::get<1>(v) + std::get<2>(std::get<3>(m)) * std::get<2>(v) + std::get<3>(std::get<3>(m)) * std::get<3>(v));
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator*(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept
{
	typename Mat<4, 4, T>::col_type const srcA0 = std::get<0>(m1);
	typename Mat<4, 4, T>::col_type const srcA1 = std::get<1>(m1);
	typename Mat<4, 4, T>::col_type const srcA2 = std::get<2>(m1);
	typename Mat<4, 4, T>::col_type const srcA3 = std::get<3>(m1);

	typename Mat<4, 4, T>::col_type const srcB0 = std::get<0>(m2);
	typename Mat<4, 4, T>::col_type const srcB1 = std::get<1>(m2);
	typename Mat<4, 4, T>::col_type const srcB2 = std::get<2>(m2);
	typename Mat<4, 4, T>::col_type const srcB3 = std::get<3>(m2);

	Mat<4, 4, T> result;
	std::get<0>(result) = srcA0 * std::get<0>(srcB0) + srcA1 * std::get<1>(srcB0) + srcA2 * std::get<2>(srcB0) + srcA3 * std::get<3>(srcB0);
	std::get<1>(result) = srcA0 * std::get<0>(srcB1) + srcA1 * std::get<1>(srcB1) + srcA2 * std::get<2>(srcB1) + srcA3 * std::get<3>(srcB1);
	std::get<2>(result) = srcA0 * std::get<0>(srcB2) + srcA1 * std::get<1>(srcB2) + srcA2 * std::get<2>(srcB2) + srcA3 * std::get<3>(srcB2);
	std::get<3>(result) = srcA0 * std::get<0>(srcB3) + srcA1 * std::get<1>(srcB3) + srcA2 * std::get<2>(srcB3) + srcA3 * std::get<3>(srcB3);

	return result;
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator/(const Mat<4, 4, T>& m, const T scalar) noexcept
{
	return Mat<4, 4, T>(std::get<0>(m) / scalar, std::get<1>(m) / scalar, std::get<2>(m) / scalar, std::get<3>(m) / scalar);
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator/(const T scalar, const Mat<4, 4, T>& m) noexcept
{
	return Mat<4, 4, T>(scalar / std::get<0>(m), scalar / std::get<1>(m), scalar / std::get<2>(m), scalar / std::get<3>(m));
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr typename TRAP::Math::Mat<4, 4, T>::col_type TRAP::Math::operator/(const Mat<4, 4, T>& m,
                                                                                          const typename Mat<4, 4, T>::row_type& v)
{
	return Inverse(m) * v;
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr typename TRAP::Math::Mat<4, 4, T>::row_type TRAP::Math::operator/(const typename Mat<4, 4, T>::col_type& v,
                                                                                          const Mat<4, 4, T>& m)
{
	return v * Inverse(m);
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::operator/(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept
{
	Mat<4, 4, T> m1Copy(m1);

	return m1Copy /= m2;
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr bool TRAP::Math::operator==(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept
{
	return (std::get<0>(m1) == std::get<0>(m2)) && (std::get<1>(m1) == std::get<1>(m2)) && (std::get<2>(m1) == std::get<2>(m2)) && (std::get<3>(m1) == std::get<3>(m2));
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr bool TRAP::Math::operator!=(const Mat<4, 4, T>& m1, const Mat<4, 4, T>& m2) noexcept
{
	return (std::get<0>(m1) != std::get<0>(m2)) || (std::get<1>(m1) != std::get<1>(m2)) || (std::get<2>(m1) != std::get<2>(m2)) || (std::get<3>(m1) != std::get<3>(m2));
}

//-------------------------------------------------------------------------------------------------------------------//
//std::get support
namespace std
{
	/// @brief Extracts the Ith element from the matrix.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// @param m Matrix whose contents to extract.
	/// @return A reference to the Ith element of v.
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr typename TRAP::Math::Mat<4, 4, T>::col_type& get(TRAP::Math::Mat<4, 4, T>& m) noexcept
	{
		static_assert(I < TRAP::Math::Mat<4, 4, T>::Length());

		return m[I];
	}

	/// @brief Extracts the Ith element from the matrix.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// @param m Matrix whose contents to extract.
	/// @return A reference to the Ith element of v.
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr typename TRAP::Math::Mat<4, 4, T>::col_type&& get(TRAP::Math::Mat<4, 4, T>&& m) noexcept
	{
		static_assert(I < TRAP::Math::Mat<4, 4, T>::Length());

		return std::move(m[I]);
	}

	/// @brief Extracts the Ith element from the matrix.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// @param m Matrix whose contents to extract.
	/// @return A reference to the Ith element of v.
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const typename TRAP::Math::Mat<4, 4, T>::col_type& get(const TRAP::Math::Mat<4, 4, T>& m) noexcept
	{
		static_assert(I < TRAP::Math::Mat<4, 4, T>::Length());

		return m[I];
	}

	/// @brief Extracts the Ith element from the matrix.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// @param m Matrix whose contents to extract.
	/// @return A reference to the Ith element of v.
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const typename TRAP::Math::Mat<4, 4, T>::col_type&& get(const TRAP::Math::Mat<4, 4, T>&& m) noexcept
	{
		static_assert(I < TRAP::Math::Mat<4, 4, T>::Length());

		return std::move(m[I]);
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//std::swap support

namespace std
{
	template<typename T>
	requires std::floating_point<T>
	inline constexpr void swap(TRAP::Math::Mat<4, 4, T>& lhs, TRAP::Math::Mat<4, 4, T>& rhs) noexcept(std::is_nothrow_move_constructible_v<TRAP::Math::Mat<4, 4, T>> &&
																                                      std::is_nothrow_move_assignable_v<TRAP::Math::Mat<4, 4, T>>)
	{
		lhs.Swap(rhs);
	}
}

#endif /*TRAP_MAT4_H*/
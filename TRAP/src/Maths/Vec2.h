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

#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Types.h"
#include "Utils/Utils.h"

namespace TRAP::Math
{
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T FMod(T x, T y);

	/// @brief Struct for a 2-dimensional vector.
	template<class T>
	requires std::is_arithmetic_v<T>
	struct Vec<2, T>
	{
		//Implementation detail
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using iterator = std::array<T, 2>::iterator;
		using const_iterator = std::array<T, 2>::const_iterator;
		using size_type = u32;
		using difference_type = isize;
		using reverse_iterator = std::array<T, 2>::reverse_iterator;
		using const_reverse_iterator = std::array<T, 2>::const_reverse_iterator;

private:
		//Data
		std::array<T, 2> data;

public:
		//Implicit basic constructors
		/// @brief Constructor.
		constexpr Vec() noexcept = default;
		/// @brief Copy constructor.
		constexpr Vec(const Vec & v) noexcept = default;

		//Explicit basic constructors
		explicit constexpr Vec(T scalar) noexcept;
		constexpr Vec(T x_, T y_) noexcept;

		//Explicit conversions
		template<typename A, typename B>
		requires std::is_arithmetic_v<A> && std::is_arithmetic_v<B>
		constexpr Vec(A x_, B y_) noexcept
			: data{ static_cast<T>(x_), static_cast<T>(y_) }
		{}

		//Conversion vector constructors

		//Explicit conversions
		template<typename U>
		requires std::is_arithmetic_v<U>
		explicit constexpr Vec(const Vec<3, U>& v) noexcept
			: data{ static_cast<T>(v.x()), static_cast<T>(v.y()) }
		{}
		//Explicit conversions
		template<typename U>
		requires std::is_arithmetic_v<U>
		explicit constexpr Vec(const Vec<4, U>& v) noexcept
			: data{ static_cast<T>(v.x()), static_cast<T>(v.y()) }
		{}

		//Explicit conversions
		template<typename U>
		requires std::is_arithmetic_v<U>
		explicit constexpr Vec(const Vec<2, U> & v) noexcept
			: data{ static_cast<T>(v.x()), static_cast<T>(v.y()) }
		{}

		/// @brief Move constructor.
		constexpr Vec(Vec&&) noexcept = default;
		/// @brief Destructor.
		constexpr ~Vec() = default;
		/// @brief Move assignment operator.
		constexpr Vec<2, T>& operator=(Vec&&) noexcept = default;

		/// @brief Retrieve the count of components of the vector.
		/// @return Count of components.
		[[nodiscard]] static constexpr usize Length() noexcept;

		//Component Access

		/// @brief Retrieve the x component of the vector.
		/// @return x component.
		[[nodiscard]] constexpr T& x() noexcept;
		/// @brief Retrieve the x component of the vector.
		/// @return x component.
		[[nodiscard]] constexpr const T& x() const noexcept;
		/// @brief Retrieve the y component of the vector.
		/// @return y component.
		[[nodiscard]] constexpr T& y() noexcept;
		/// @brief Retrieve the y component of the vector.
		/// @return y component.
		[[nodiscard]] constexpr const T& y() const noexcept;

		/// @brief Returns a reference to the element at specified location i.
		/// @param i Position of the element to return.
		/// @return Reference to the requested element.
		/// @note No bounds checking is performed.
		[[nodiscard]] constexpr T& operator[](usize i) noexcept;
		/// @brief Returns a constant reference to the element at specified location i.
		/// @param i Position of the element to return.
		/// @return Constant reference to the requested element.
		/// @note No bounds checking is performed.
		[[nodiscard]] constexpr const T& operator[](usize i) const noexcept;

		/// @brief Returns a reference to the element at specified location i, with bounds checking via asserts.
		/// @param i Position of the element to return.
		/// @return Reference to the requested element.
		[[nodiscard]] constexpr T& at(usize i);
		/// @brief Returns a constant reference to the element at specified location i, with bounds checking via asserts.
		/// @param i Position of the element to return.
		/// @return Constant reference to the requested element.
		[[nodiscard]] constexpr const T& at(usize i) const;

		//Iterator

		/// @brief Returns an iterator to the first element of the vector.
		/// @return Iterator to the first element.
		[[nodiscard]] constexpr const_iterator begin() const noexcept;
		/// @brief Returns an iterator to the first element of the vector.
		/// @return Iterator to the first element.
		[[nodiscard]] constexpr iterator begin() noexcept;
		/// @brief Returns an iterator to the first element of the vector.
		/// @return Iterator to the first element.
		[[nodiscard]] constexpr const_iterator cbegin() const noexcept;
		/// @brief Returns a reverse iterator to the first element of the reversed vector.
		///        It corresponds to the last element of the non-reversed vector.
		/// @return Reverse iterator to the first element.
		[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept;
		/// @brief Returns a reverse iterator to the first element of the reversed vector.
		///        It corresponds to the last element of the non-reversed vector.
		/// @return Reverse iterator to the first element.
		[[nodiscard]] constexpr reverse_iterator rbegin() noexcept;
		/// @brief Returns a reverse iterator to the first element of the reversed vector.
		///        It corresponds to the last element of the non-reversed vector.
		/// @return Reverse iterator to the first element.
		[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept;
		/// @brief Returns an iterator to the element following the last element of the vector.
		/// @return Iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr const_iterator end() const noexcept;
		/// @brief Returns an iterator to the element following the last element of the vector.
		/// @return Iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr iterator end() noexcept;
		/// @brief Returns an iterator to the element following the last element of the vector.
		/// @return Iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr const_iterator cend() const noexcept;
		/// @brief Returns a reverse iterator to the element following the last element of the reversed vector.
		///        It corresponds to the element preceding the first element of the non-reversed vector.
		/// @return Reverse iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept;
		/// @brief Returns a reverse iterator to the element following the last element of the reversed vector.
		///        It corresponds to the element preceding the first element of the non-reversed vector.
		/// @return Reverse iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr reverse_iterator rend() noexcept;
		/// @brief Returns a reverse iterator to the element following the last element of the reversed vector.
		///        It corresponds to the element preceding the first element of the non-reversed vector.
		/// @return Reverse iterator to the element following the last element.
		/// @note This element acts as a placeholder; attempting to access it results in undefined behaviour.
		[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept;

		//Unary arithmetic operators

		/// @brief Copy assignment operator
		constexpr Vec<2, T>& operator=(const Vec& v) noexcept = default;

		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator=(const Vec<2, U>& v) noexcept
		{
			this->x() = static_cast<T>(v.x());
			this->y() = static_cast<T>(v.y());

			return *this;
		}
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator+=(U scalar) noexcept
		{
			this->x() += static_cast<T>(scalar);
			this->y() += static_cast<T>(scalar);

			return *this;
		}
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator+=(const Vec<2, U>& v) noexcept
		{
			this->x() += static_cast<T>(v.x());
			this->y() += static_cast<T>(v.y());

			return *this;
		}
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator-=(U scalar) noexcept
		{
			this->x() -= static_cast<T>(scalar);
			this->y() -= static_cast<T>(scalar);

			return *this;
		}
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator-=(const Vec<2, U>& v) noexcept
		{
			this->x() -= static_cast<T>(v.x());
			this->y() -= static_cast<T>(v.y());

			return *this;
		}
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator*=(U scalar) noexcept
		{
			this->x() *= static_cast<T>(scalar);
			this->y() *= static_cast<T>(scalar);

			return *this;
		}
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator*=(const Vec<2, U>& v) noexcept
		{
			this->x() *= static_cast<T>(v.x());
			this->y() *= static_cast<T>(v.y());

			return *this;
		}
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator/=(U scalar) noexcept
		{
			this->x() /= static_cast<T>(scalar);
			this->y() /= static_cast<T>(scalar);

			return *this;
		}
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator/=(const Vec<2, U>& v) noexcept
		{
			this->x() /= static_cast<T>(v.x());
			this->y() /= static_cast<T>(v.y());

			return *this;
		}

		//Increment and decrement operators
		constexpr Vec<2, T>& operator++() noexcept;
		constexpr Vec<2, T>& operator--() noexcept;
		constexpr const Vec<2, T> operator++(i32) noexcept;
		constexpr const Vec<2, T> operator--(i32) noexcept;

		//Unary bit operators
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator%=(U scalar) noexcept
		{
			if constexpr(std::floating_point<T>)
			{
				this->x() = TRAP::Math::FMod(this->x(), static_cast<T>(scalar));
				this->y() = TRAP::Math::FMod(this->y(), static_cast<T>(scalar));
			}
			else
			{
				this->x() %= static_cast<T>(scalar);
				this->y() %= static_cast<T>(scalar);
			}

			return *this;
		}
		template<typename U>
		requires std::is_arithmetic_v<U>
		constexpr Vec<2, T>& operator%=(const Vec<2, U>& v) noexcept
		{
			if constexpr(std::floating_point<T>)
			{
				this->x() = TRAP::Math::FMod(this->x(), static_cast<T>(v.x()));
				this->y() = TRAP::Math::FMod(this->y(), static_cast<T>(v.y()));
			}
			else
			{
				this->x() %= static_cast<T>(v.x());
				this->y() %= static_cast<T>(v.y());
			}

			return *this;
		}
		template<typename U>
		requires std::integral<U>
		constexpr Vec<2, T>& operator&=(U scalar) noexcept
		{
			this->x() &= static_cast<T>(scalar);
			this->y() &= static_cast<T>(scalar);

			return *this;
		}
		template<typename U>
		requires std::integral<U>
		constexpr Vec<2, T>& operator&=(const Vec<2, U>& v) noexcept
		{
			this->x() &= static_cast<T>(v.x());
			this->y() &= static_cast<T>(v.y());

			return *this;
		}
		template<typename U>
		requires std::integral<U>
		constexpr Vec<2, T>& operator|=(U scalar) noexcept
		{
			this->x() |= static_cast<T>(scalar);
			this->y() |= static_cast<T>(scalar);

			return *this;
		}
		template<typename U>
		requires std::integral<U>
		constexpr Vec<2, T>& operator|=(const Vec<2, U>& v) noexcept
		{
			this->x() |= static_cast<T>(v.x());
			this->y() |= static_cast<T>(v.y());

			return *this;
		}
		template<typename U>
		requires std::integral<U>
		constexpr Vec<2, T>& operator^=(U scalar) noexcept
		{
			this->x() ^= static_cast<T>(scalar);
			this->y() ^= static_cast<T>(scalar);

			return *this;
		}
		template<typename U>
		requires std::integral<U>
		constexpr Vec<2, T>& operator^=(const Vec<2, U>& v) noexcept
		{
			this->x() ^= static_cast<T>(v.x());
			this->y() ^= static_cast<T>(v.y());

			return *this;
		}
		template<typename U>
		requires std::integral<U>
		constexpr Vec<2, T>& operator<<=(U scalar) noexcept
		{
			this->x() <<= static_cast<T>(scalar);
			this->y() <<= static_cast<T>(scalar);

			return *this;
		}
		template<typename U>
		requires std::integral<U>
		constexpr Vec<2, T>& operator<<=(const Vec<2, U>& v) noexcept
		{
			this->x() <<= static_cast<T>(v.x());
			this->y() <<= static_cast<T>(v.y());

			return *this;
		}
		template<typename U>
		requires std::integral<U>
		constexpr Vec<2, T>& operator>>=(U scalar) noexcept
		{
			this->x() >>= static_cast<T>(scalar);
			this->y() >>= static_cast<T>(scalar);

			return *this;
		}
		template<typename U>
		requires std::integral<U>
		constexpr Vec<2, T>& operator>>=(const Vec<2, U>& v) noexcept
		{
			this->x() >>= static_cast<T>(v.x());
			this->y() >>= static_cast<T>(v.y());

			return *this;
		}

		[[nodiscard]] consteval auto operator<=>(const Vec<2, T>& rhs) const noexcept = delete;

		constexpr void Swap(Vec<2, T>& other) noexcept(std::is_nothrow_move_constructible_v<Vec<2, T>> &&
		                                                      std::is_nothrow_move_assignable_v<Vec<2, T>>)
		{
			std::swap(data, other.data);
		}
	};

	//Unary operators
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator+(const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::signed_integral<T> || std::floating_point<T>
	[[nodiscard]] constexpr Vec<2, T> operator-(const Vec<2, T>& v) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	//Binary operators
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator+(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator+(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator+(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator-(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator-(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator-(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator*(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator*(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator*(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator/(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator/(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator/(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator%(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator%(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<2, T> operator%(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator&(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator&(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator&(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator|(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator|(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator|(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator^(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator^(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator^(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator<<(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator<<(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator<<(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator>>(const Vec<2, T>& v, T scalar) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator>>(T scalar, const Vec<2, T>& v) noexcept;

	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<2, T> operator>>(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::integral<T>
	constexpr Vec<2, T> operator~(const Vec<2, T>& v) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	//Boolean operators
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr bool operator==(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr bool operator!=(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept;

	[[nodiscard]] constexpr Vec<2, bool> operator&&(const Vec<2, bool>& v1, const Vec<2, bool>& v2) noexcept;

	[[nodiscard]] constexpr Vec<2, bool> operator||(const Vec<2, bool>& v1, const Vec<2, bool>& v2) noexcept;
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	requires std::is_arithmetic_v<T>
	struct hash<TRAP::Math::Vec<2, T>>
	{
		[[nodiscard]] constexpr usize operator()(const TRAP::Math::Vec<2, T>& v) const noexcept
		{
			usize seed = 0;
			constexpr hash<T> hasher;
			TRAP::Utils::HashCombine(seed, hasher(v.x()), hasher(v.y()));
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
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<2, T>::Vec(const T scalar) noexcept
	: data{scalar, scalar}
{}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<2, T>::Vec(const T x_, const T y_) noexcept
	: data{x_, y_}
{}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr usize TRAP::Math::Vec<2, T>::Length() noexcept
{
	return 2;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component accesses

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<2, T>::x() noexcept
{
	return data[0];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<2, T>::x() const noexcept
{
	return data[0];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<2, T>::y() noexcept
{
	return data[1];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<2, T>::y() const noexcept
{
	return data[1];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<2, T>::operator[](const usize i) noexcept
{
	return data[i];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<2, T>::operator[](const usize i) const noexcept
{
	return data[i];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T& TRAP::Math::Vec<2, T>::at(const usize i)
{
	if(i >= this->Length())
		throw std::out_of_range(fmt::format("Math::Vec<2, T>::at(): Index {} is out of range!", i));

	return data[i];
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr const T& TRAP::Math::Vec<2, T>::at(const usize i) const
{
	if(i >= this->Length())
		throw std::out_of_range(fmt::format("Math::Vec<2, T>::at(): Index {} is out of range!", i));

	return data[i];
}

//-------------------------------------------------------------------------------------------------------------------//

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::const_iterator TRAP::Math::Vec<2, T>::begin() const noexcept
{
	return data.begin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::iterator TRAP::Math::Vec<2, T>::begin() noexcept
{
	return data.begin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::const_iterator TRAP::Math::Vec<2, T>::cbegin() const noexcept
{
	return data.cbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::const_reverse_iterator TRAP::Math::Vec<2, T>::rbegin() const noexcept
{
	return data.rbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::reverse_iterator TRAP::Math::Vec<2, T>::rbegin() noexcept
{
	return data.rbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::const_reverse_iterator TRAP::Math::Vec<2, T>::crbegin() const noexcept
{
	return data.crbegin();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::const_iterator TRAP::Math::Vec<2, T>::end() const noexcept
{
	return data.end();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::iterator TRAP::Math::Vec<2, T>::end() noexcept
{
	return data.end();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::const_iterator TRAP::Math::Vec<2, T>::cend() const noexcept
{
	return data.cend();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::const_reverse_iterator TRAP::Math::Vec<2, T>::rend() const noexcept
{
	return data.rend();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::reverse_iterator TRAP::Math::Vec<2, T>::rend() noexcept
{
	return data.rend();
}

template<class T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T>::const_reverse_iterator TRAP::Math::Vec<2, T>::crend() const noexcept
{
	return data.crend();
}

//-------------------------------------------------------------------------------------------------------------------//
//Increment and decrement operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator++() noexcept
{
	++this->x();
	++this->y();

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<2, T>& TRAP::Math::Vec<2, T>::operator--() noexcept
{
	--this->x();
	--this->y();

	return *this;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr const TRAP::Math::Vec<2, T> TRAP::Math::Vec<2, T>::operator++(const i32) noexcept
{
	const Vec<2, T> result(*this);
	++*this;

	return result;
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr const TRAP::Math::Vec<2, T> TRAP::Math::Vec<2, T>::operator--(const i32) noexcept
{
	const Vec<2, T> result(*this);
	--*this;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
struct fmt::formatter<TRAP::Math::Vec<2, T>>
{
    [[nodiscard]] static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    [[nodiscard]] static fmt::format_context::iterator format(const TRAP::Math::Vec<2, T>& vec, fmt::format_context& ctx)
    {
		if constexpr(std::same_as<T, f32>)
			return fmt::format_to(ctx.out(), "Vec2f({}, {})", vec.x(), vec.y());
		else if constexpr(std::same_as<T, f64>)
			return fmt::format_to(ctx.out(), "Vec2d({}, {})", vec.x(), vec.y());
		else if constexpr(std::same_as<T, bool>)
			return fmt::format_to(ctx.out(), "Vec2b({}, {})", vec.x(), vec.y());
		else if constexpr(std::same_as<T, i8>)
			return fmt::format_to(ctx.out(), "Vec2i8({}, {})", vec.x(), vec.y());
		else if constexpr(std::same_as<T, i16>)
			return fmt::format_to(ctx.out(), "Vec2i16({}, {})", vec.x(), vec.y());
		else if constexpr(std::same_as<T, i32>)
			return fmt::format_to(ctx.out(), "Vec2i32({}, {})", vec.x(), vec.y());
		else if constexpr(std::same_as<T, i64>)
			return fmt::format_to(ctx.out(), "Vec2i64({}, {})", vec.x(), vec.y());
		else if constexpr(std::same_as<T, u8>)
			return fmt::format_to(ctx.out(), "Vec2u8({}, {})", vec.x(), vec.y());
		else if constexpr(std::same_as<T, u16>)
			return fmt::format_to(ctx.out(), "Vec2u16({}, {})", vec.x(), vec.y());
		else if constexpr(std::same_as<T, u32>)
			return fmt::format_to(ctx.out(), "Vec2u32({}, {})", vec.x(), vec.y());
		else if constexpr(std::same_as<T, u64>)
			return fmt::format_to(ctx.out(), "Vec2u64({}, {})", vec.x(), vec.y());
		else
			return fmt::format_to(ctx.out(), "Unknown type");
    }
};

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template<typename T>
requires std::is_arithmetic_v<T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<2, T>& v) noexcept
{
	return v;
}

template<typename T>
requires std::signed_integral<T> || std::floating_point<T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(-v.x(), -v.y());
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary arithmetic operators

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x() + scalar, v.y() + scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar + v.x(), scalar + v.y());
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator+(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x() + v2.x(), v1.y() + v2.y());
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x() - scalar, v.y() - scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar - v.x(), scalar - v.y());
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator-(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x() - v2.x(), v1.y() - v2.y());
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x() * scalar, v.y() * scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar * v.x(), scalar * v.y());
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator*(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x() * v2.x(), v1.y() * v2.y());
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x() / scalar, v.y() / scalar);
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar / v.x(), scalar / v.y());
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator/(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x() / v2.x(), v1.y() / v2.y());
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary bit operators

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(const Vec<2, T>& v, const T scalar) noexcept
{
	if constexpr(std::floating_point<T>)
	{
		return Vec<2, T>(TRAP::Math::FMod(v.x(), scalar), TRAP::Math::FMod(v.y(), scalar));
	}
	else
	{
		return Vec<2, T>(v.x() % scalar, v.y() % scalar);
	}
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(const T scalar, const Vec<2, T>& v) noexcept
{
	if constexpr(std::floating_point<T>)
	{
		return Vec<2, T>(TRAP::Math::FMod(scalar, v.x()), TRAP::Math::FMod(scalar, v.y()));
	}
	else
	{
		return Vec<2, T>(scalar % v.x(), scalar % v.y());
	}
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator%(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	if constexpr(std::floating_point<T>)
	{
		return Vec<2, T>(TRAP::Math::FMod(v1.x(), v2.x()), TRAP::Math::FMod(v1.y(), v2.y()));
	}
	else
	{
		return Vec<2, T>(v1.x() % v2.x(), v1.y() % v2.y());
	}
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x() & scalar, v.y() & scalar);
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar & v.x(), scalar & v.y());
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator&(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x() & v2.x(), v1.y() & v2.y());
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x() | scalar, v.y() | scalar);
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar | v.x(), scalar | v.y());
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator|(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x() | v2.x(), v1.y() | v2.y());
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x() ^ scalar, v.y() ^ scalar);
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar ^ v.x(), scalar ^ v.y());
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator^(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x() ^ v2.x(), v1.y() ^ v2.y());
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x() << scalar, v.y() << scalar);
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar << v.x(), scalar << v.y());
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator<<(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x() << v2.x(), v1.y() << v2.y());
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(const Vec<2, T>& v, const T scalar) noexcept
{
	return Vec<2, T>(v.x() >> scalar, v.y() >> scalar);
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(const T scalar, const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(scalar >> v.x(), scalar >> v.y());
}

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator>>(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return Vec<2, T>(v1.x() >> v2.x(), v1.y() >> v2.y());
}

template<typename T>
requires std::integral<T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::operator~(const Vec<2, T>& v) noexcept
{
	return Vec<2, T>(~v.x(), ~v.y());
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool TRAP::Math::operator==(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return v1.x() == v2.x() && v1.y() == v2.y();
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool TRAP::Math::operator!=(const Vec<2, T>& v1, const Vec<2, T>& v2) noexcept
{
	return !(v1 == v2);
}

[[nodiscard]] constexpr TRAP::Math::Vec<2, bool> TRAP::Math::operator&&(const Vec<2, bool>& v1, const Vec<2, bool>& v2) noexcept
{
	return Vec<2, bool>(v1.x() && v2.x(), v1.y() && v2.y());
}

[[nodiscard]] constexpr TRAP::Math::Vec<2, bool> TRAP::Math::operator||(const Vec<2, bool>& v1, const Vec<2, bool>& v2) noexcept
{
	return Vec<2, bool>(v1.x() || v2.x(), v1.y() || v2.y());
}

//-------------------------------------------------------------------------------------------------------------------//
//std::get support

namespace std
{
	/// @brief Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 2).
	/// This is enforced at compile time!
	/// @param v Vector whose contents to extract.
	/// @return A reference to the Ith element of v.
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr T& get(TRAP::Math::Vec<2, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<2, T>::Length());

		if constexpr(I == 0)
		{
			return v.x();
		}
		else if constexpr(I == 1)
		{
			return v.y();
		}
		else
		{
			std::unreachable();
		}
	}

	/// @brief Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 2).
	/// This is enforced at compile time!
	/// @param v Vector whose contents to extract.
	/// @return A reference to the Ith element of v.
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr T&& get(TRAP::Math::Vec<2, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<2, T>::Length());

		if constexpr(I == 0)
		{
			return std::move(v.x());
		}
		else if constexpr(I == 1)
		{
			return std::move(v.y());
		}
		else
		{
			std::unreachable();
		}
	}

	/// @brief Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 2).
	/// This is enforced at compile time!
	/// @param v Vector whose contents to extract.
	/// @return A reference to the Ith element of v.
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const T& get(const TRAP::Math::Vec<2, T>& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<2, T>::Length());

		if constexpr(I == 0)
		{
			return v.x();
		}
		else if constexpr(I == 1)
		{
			return v.y();
		}
		else
		{
			std::unreachable();
		}
	}

	/// @brief Extracts the Ith element from the vector.
	/// I must be an integer value in range [0, 2).
	/// This is enforced at compile time!
	/// @param v Vector whose contents to extract.
	/// @return A reference to the Ith element of v.
	template<usize I, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr const T&& get(const TRAP::Math::Vec<2, T>&& v) noexcept
	{
		static_assert(I < TRAP::Math::Vec<2, T>::Length());

		if constexpr(I == 0)
		{
			return std::move(v.x());
		}
		else if constexpr(I == 1)
		{
			return std::move(v.y());
		}
		else
		{
			std::unreachable();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//std::swap support

namespace std
{
	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr void swap(TRAP::Math::Vec<2, T>& lhs, TRAP::Math::Vec<2, T>& rhs) noexcept(std::is_nothrow_move_constructible_v<TRAP::Math::Vec<2, T>> &&
																                                std::is_nothrow_move_assignable_v<TRAP::Math::Vec<2, T>>)
	{
		lhs.Swap(rhs);
	}
}

#endif /*TRAP_VEC2_H*/
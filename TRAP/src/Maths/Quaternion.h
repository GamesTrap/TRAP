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

#ifndef TRAP_QUATERNION_H
#define TRAP_QUATERNION_H

#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Utils/Utils.h"

namespace TRAP::Math
{
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Sqrt(T x);

	template<typename T>
	requires std::signed_integral<T> || std::floating_point<T>
	[[nodiscard]] constexpr T Abs( T x);

	template<typename T>
	requires std::floating_point<T>
	struct tQuat
	{
		/// @brief Move constructor.
		constexpr tQuat(tQuat&&) noexcept = default;
		/// @brief Destructor.
		constexpr ~tQuat() = default;
		/// @brief Move assignment operator.
		constexpr tQuat<T>& operator=(tQuat&&) noexcept = default;

		//Implementation detail
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = u32;
		using difference_type = isize;

		//Data
private:
		std::array<T, 4> data;

public:
		//Implicit basic constructors

		/// @brief Constructor.
		constexpr tQuat() noexcept = default;
		/// @brief Copy constructor.
		constexpr tQuat(const tQuat& q) noexcept = default;

		/// @brief Explicit basic constructor.
		constexpr tQuat(T s, const Vec<3, T>& v) noexcept;
		/// @brief Explicit basic constructor.
		constexpr tQuat(T w_, T x_, T y_, T z_) noexcept;

		//Conversion constructors

		/// @brief Explicit conversion constructor
		template<typename U>
		requires std::floating_point<U>
		explicit constexpr tQuat(const tQuat<U>& q) noexcept;

		//Explicit conversion operators
		//explicit operator Mat<3, 3, T>() const;
		//explicit operator Mat<4, 4, T>() const;

		/// @brief Create a quaternion from two normalized axis.
		constexpr tQuat(const Vec<3, T>& u, const Vec<3, T>& v);

		/// @brief Build a quaternion from euler angles (pitch, yaw, roll).
		/// @param eulerAnglesInRadians Euler angles (pitch, yaw, roll).
		explicit constexpr tQuat(const Vec<3, T>& eulerAnglesInRadians);
		/// @brief Build a quaternion from a 3x3 matrix.
		explicit constexpr tQuat(const Mat<3, 3, T>& m);
		/// @brief Build a quaternion from a 4x4 matrix.
		explicit constexpr tQuat(const Mat<4, 4, T>& m);

		//Unary arithmetic operators
		constexpr tQuat<T>& operator=(const tQuat<T>& q) noexcept = default;

		template<typename U>
		requires std::floating_point<U>
		constexpr tQuat<T>& operator=(const tQuat<U>& q) noexcept;
		template<typename U>
		requires std::floating_point<U>
		constexpr tQuat<T>& operator+=(const tQuat<U>& q) noexcept;
		template<typename U>
		requires std::floating_point<U>
		constexpr tQuat<T>& operator-=(const tQuat<U>& q) noexcept;
		template<typename U>
		requires std::floating_point<U>
		constexpr tQuat<T>& operator*=(const tQuat<U>& r) noexcept;
		template<typename U>
		constexpr tQuat<T>& operator*=(U s) noexcept;
		template<typename U>
		constexpr tQuat<T>& operator/=(U s) noexcept;

		/// @brief Retrieve the count of components of a quaternion.
		/// @return Count.
		[[nodiscard]] static constexpr usize Length() noexcept;

		//Component Access

		/// @brief Retrieve the w component of the quaternion.
		/// @return w component.
		[[nodiscard]] constexpr T& w() noexcept;
		/// @brief Retrieve the w component of the quaternion.
		/// @return w component.
		[[nodiscard]] constexpr const T& w() const noexcept;
		/// @brief Retrieve the x component of the quaternion.
		/// @return x component.
		[[nodiscard]] constexpr T& x() noexcept;
		/// @brief Retrieve the x component of the quaternion.
		/// @return x component.
		[[nodiscard]] constexpr const T& x() const noexcept;
		/// @brief Retrieve the y component of the quaternion.
		/// @return y component.
		[[nodiscard]] constexpr T& y() noexcept;
		/// @brief Retrieve the y component of the quaternion.
		/// @return y component.
		[[nodiscard]] constexpr const T& y() const noexcept;
		/// @brief Retrieve the z component of the quaternion.
		/// @return z component.
		[[nodiscard]] constexpr T& z() noexcept;
		/// @brief Retrieve the z component of the quaternion.
		/// @return z component.
		[[nodiscard]] constexpr const T& z() const noexcept;

		/// @brief Returns a reference to the element at specified location i.
		/// @param i Position of the element to return.
		/// @return Reference to the requested element.
		/// @note No bounds checking is performed.
		[[nodiscard]] constexpr T& operator[](usize i);
		/// @brief Returns a constant reference to the element at specified location i.
		/// @param i Position of the element to return.
		/// @return Constant reference to the requested element.
		/// @note No bounds checking is performed.
		[[nodiscard]] constexpr const T& operator[](usize i) const;

		/// @brief Returns a reference to the element at specified location i, with bounds checking via asserts.
		/// @param i Position of the element to return.
		/// @return Reference to the requested element.
		[[nodiscard]] constexpr T& at(usize i);
		/// @brief Returns a constant reference to the element at specified location i, with bounds checking via asserts.
		/// @param i Position of the element to return.
		/// @return Constant reference to the requested element.
		[[nodiscard]] constexpr const T& at(usize i) const;

		[[nodiscard]] consteval auto operator<=>(const tQuat<T>& rhs) const noexcept = delete;

		constexpr void Swap(tQuat<T>& other) noexcept(std::is_nothrow_move_constructible_v<tQuat<T>> &&
		                                                     std::is_nothrow_move_assignable_v<tQuat<T>>)
		{
			std::swap(data, other.data);
		}
	};

	//-------------------------------------------------------------------------------------------------------------------//
	//Unary bit operators

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> operator+(const tQuat<T>& q) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> operator-(const tQuat<T>& q) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//
	//Binary operators

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> operator+(const tQuat<T>& q, const tQuat<T>& p) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> operator-(const tQuat<T>& q, const tQuat<T>& p) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> operator*(const tQuat<T>& q, const tQuat<T>& p) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<3, T> operator*(const tQuat<T>& q, const Vec<3, T>& v);

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<3, T> operator*(const Vec<3, T>& v, const tQuat<T>& q);

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, T> operator*(const tQuat<T>& q, const Vec<4, T>& v) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, T> operator*(const Vec<4, T>& v, const tQuat<T>& q);

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> operator*(const tQuat<T>& q, const T& s) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> operator*(const T& s, const tQuat<T>& q) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> operator/(const tQuat<T>& q, const T& s) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//
	//Boolean operators

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr bool operator==(const tQuat<T>& q1, const tQuat<T>& q2) noexcept;

	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr bool operator!=(const tQuat<T>& q1, const tQuat<T>& q2) noexcept;
}

//-------------------------------------------------------------------------------------------------------------------//
//Hash---------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template<typename T>
	requires std::floating_point<T>
	struct hash<TRAP::Math::tQuat<T>>
	{
		[[nodiscard]] constexpr usize operator()(const TRAP::Math::tQuat<T>& q) const noexcept
		{
			usize seed = 0;
			hash<T> hasher;
			TRAP::Utils::HashCombine(seed, hasher(q.x()), hasher(q.y()), hasher(q.z()), hasher(q.w()));
			return seed;
		}
	};
}

//-------------------------------------------------------------------------------------------------------------------//
//Explicit basic constructors
template <typename T>
requires std::floating_point<T>
constexpr TRAP::Math::tQuat<T>::tQuat(const T s, const Vec<3, T>& v) noexcept
	: data{s, v.x(), v.y(), v.z()}
{
}

template <typename T>
requires std::floating_point<T>
constexpr TRAP::Math::tQuat<T>::tQuat(const T w_, const T x_, const T y_, const T z_) noexcept
	: data{w_, x_, y_, z_}
{
}

//-------------------------------------------------------------------------------------------------------------------//
//Conversion constructors

template <typename T>
requires std::floating_point<T>
template <typename U>
requires std::floating_point<U>
constexpr TRAP::Math::tQuat<T>::tQuat(const tQuat<U>& q) noexcept
	: data{static_cast<T>(q.w()), static_cast<T>(q.x()), static_cast<T>(q.y()), static_cast<T>(q.z())}
{
}

//-------------------------------------------------------------------------------------------------------------------//
//Explicit conversion operators

/*template <typename T>
requires std::floating_point<T>
TRAP::Math::tQuat<T>::operator Mat<3, 3, T>() const
{
	return Mat3Cast(*this);
}

template <typename T>
requires std::floating_point<T>
TRAP::Math::tQuat<T>::operator Mat<4, 4, T>() const
{
	return Mat4Cast(*this);
}*/

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
constexpr TRAP::Math::tQuat<T>::tQuat(const Vec<3, T>& u, const Vec<3, T>& v)
{
	const T normUNormV = Sqrt(Dot(u, u) * Dot(v, v));
	T realPart = normUNormV + Dot(u, v);
	Vec<3, T> t;

	if(realPart < static_cast<T>(1.e-6f) * normUNormV)
	{
		//If u and v are exactly opposite, rotate 180 degrees around an arbitrary orthogonal axis.
		//Axis normalization can happen later, when we normalize the quaternion.
		realPart = static_cast<T>(0);
		t = Abs(u.x()) > Abs(u.z()) ? Vec<3, T>(-u.y(), u.x(), static_cast<T>(0)) : Vec<3, T>(static_cast<T>(0), -u.z(), u.y());
	}
	else
	{
		//Otherwise, build quaternion the standard way.
		t = Cross(u, v);
	}

	*this = Normalize(tQuat<T>(realPart, t));
}

//-------------------------------------------------------------------------------------------------------------------//
//Build quaternion from euler angles

template <typename T>
requires std::floating_point<T>
constexpr TRAP::Math::tQuat<T>::tQuat(const Vec<3, T>& eulerAnglesInRadians)
{
	const Vec<3, T> c = Cos(eulerAnglesInRadians * static_cast<T>(0.5));
	const Vec<3, T> s = Sin(eulerAnglesInRadians * static_cast<T>(0.5));

	this->w() = c.x() * c.y() * c.z() + s.x() * s.y() * s.z();
	this->x() = s.x() * c.y() * c.z() - c.x() * s.y() * s.z();
	this->y() = c.x() * s.y() * c.z() + s.x() * c.y() * s.z();
	this->z() = c.x() * c.y() * s.z() - s.x() * s.y() * c.z();
}

template <typename T>
requires std::floating_point<T>
constexpr TRAP::Math::tQuat<T>::tQuat(const Mat<3, 3, T>& m)
{
	*this = QuaternionCast(m);
}

template <typename T>
requires std::floating_point<T>
constexpr TRAP::Math::tQuat<T>::tQuat(const Mat<4, 4, T>& m)
{
	*this = QuaternionCast(m);
}

//-------------------------------------------------------------------------------------------------------------------//
//Unary arithmetic operators

template <typename T>
requires std::floating_point<T>
template <typename U>
requires std::floating_point<U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator=(const tQuat<U>& q) noexcept
{
	this->w() = static_cast<T>(q.w());
	this->x() = static_cast<T>(q.x());
	this->y() = static_cast<T>(q.y());
	this->z() = static_cast<T>(q.z());

	return *this;
}

template <typename T>
requires std::floating_point<T>
template <typename U>
requires std::floating_point<U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator+=(const tQuat<U>& q) noexcept
{
	this->w() += static_cast<T>(q.w());
	this->x() += static_cast<T>(q.x());
	this->y() += static_cast<T>(q.y());
	this->z() += static_cast<T>(q.z());

	return *this;
}

template <typename T>
requires std::floating_point<T>
template <typename U>
requires std::floating_point<U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator-=(const tQuat<U>& q) noexcept
{
	this->w() -= static_cast<T>(q.w());
	this->x() -= static_cast<T>(q.x());
	this->y() -= static_cast<T>(q.y());
	this->z() -= static_cast<T>(q.z());

	return *this;
}

template <typename T>
requires std::floating_point<T>
template <typename U>
requires std::floating_point<U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator*=(const tQuat<U>& r) noexcept
{
	const tQuat<T> p(*this);

	this->w() = p.w() * static_cast<T>(r.w()) - p.x() * static_cast<T>(r.x()) - p.y() * static_cast<T>(r.y()) - p.z() * static_cast<T>(r.z());
	this->x() = p.w() * static_cast<T>(r.x()) + p.x() * static_cast<T>(r.w()) + p.y() * static_cast<T>(r.z()) - p.z() * static_cast<T>(r.y());
	this->y() = p.w() * static_cast<T>(r.y()) + p.y() * static_cast<T>(r.w()) + p.z() * static_cast<T>(r.x()) - p.x() * static_cast<T>(r.z());
	this->z() = p.w() * static_cast<T>(r.z()) + p.z() * static_cast<T>(r.w()) + p.x() * static_cast<T>(r.y()) - p.y() * static_cast<T>(r.x());

	return *this;
}

template <typename T>
requires std::floating_point<T>
template <typename U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator*=(const U s) noexcept
{
	this->w() *= static_cast<T>(s);
	this->x() *= static_cast<T>(s);
	this->y() *= static_cast<T>(s);
	this->z() *= static_cast<T>(s);

	return *this;
}

template <typename T>
requires std::floating_point<T>
template <typename U>
constexpr TRAP::Math::tQuat<T>& TRAP::Math::tQuat<T>::operator/=(const U s) noexcept
{
	this->w() /= static_cast<T>(s);
	this->x() /= static_cast<T>(s);
	this->y() /= static_cast<T>(s);
	this->z() /= static_cast<T>(s);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr usize TRAP::Math::tQuat<T>::Length() noexcept
{
	return 4;
}

//-------------------------------------------------------------------------------------------------------------------//
//Component Access

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T& TRAP::Math::tQuat<T>::w() noexcept
{
	return data[0];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr const T& TRAP::Math::tQuat<T>::w() const noexcept
{
	return data[0];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T& TRAP::Math::tQuat<T>::x() noexcept
{
	return data[1];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr const T& TRAP::Math::tQuat<T>::x() const noexcept
{
	return data[1];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T& TRAP::Math::tQuat<T>::y() noexcept
{
	return data[2];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr const T& TRAP::Math::tQuat<T>::y() const noexcept
{
	return data[2];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T& TRAP::Math::tQuat<T>::z() noexcept
{
	return data[3];
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr const T& TRAP::Math::tQuat<T>::z() const noexcept
{
	return data[3];
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T& TRAP::Math::tQuat<T>::operator[](const usize i)
{
	return data[i];
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr const T& TRAP::Math::tQuat<T>::operator[](const usize i) const
{
	return data[i];
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T& TRAP::Math::tQuat<T>::at(const usize i)
{
	if(i >= this->Length())
		throw std::out_of_range(fmt::format("Math::tQuat<T>::at(): Index {} is out of range!", i));

	return data[i];
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr const T& TRAP::Math::tQuat<T>::at(const usize i) const
{
	if(i >= this->Length())
		throw std::out_of_range(fmt::format("Math::tQuat<T>::at(): Index {} is out of range!", i));

	return data[i];
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
struct fmt::formatter<TRAP::Math::tQuat<T>>
{
    [[nodiscard]] static constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    [[nodiscard]] static fmt::format_context::iterator format(const TRAP::Math::tQuat<T>& quat, fmt::format_context& ctx)
    {
		char postfix = '\0';
		if constexpr(std::same_as<T, f32>)
			postfix = 'f';
		else if constexpr(std::same_as<T, f64>)
			postfix = 'd';

		return fmt::format_to(ctx.out(), "Quat{}({}, {{{}, {}, {}}})", postfix, quat.w(), quat.x(), quat.y(),
		                      quat.z());
    }
};

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------//
//Unary bit operators

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::operator+(const tQuat<T>& q) noexcept
{
	return q;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::operator-(const tQuat<T>& q) noexcept
{
	return tQuat<T>(-q.w(), -q.x(), -q.y(), -q.z());
}

//-------------------------------------------------------------------------------------------------------------------//
//Binary operators

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::operator+(const tQuat<T>& q, const tQuat<T>& p) noexcept
{
	return tQuat<T>(q.w() + p.w(), q.x() + p.x(), q.y() + p.y(), q.z() + p.z());
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::operator-(const tQuat<T>& q, const tQuat<T>& p) noexcept
{
	return tQuat<T>(q.w() - p.w(), q.x() - p.x(), q.y() - p.y(), q.z() - p.z());
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::operator*(const tQuat<T>& q, const tQuat<T>& p) noexcept
{
	return tQuat<T>
	{
		q.w() * p.w() - q.x() * p.x() - q.y() * p.y() - q.z() * p.z(),
		q.w() * p.x() + q.x() * p.w() + q.y() * p.z() - q.z() * p.y(),
		q.w() * p.y() + q.y() * p.w() + q.z() * p.x() - q.x() * p.z(),
		q.w() * p.z() + q.z() * p.w() + q.x() * p.y() - q.y() * p.x(),
	};
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const tQuat<T>& q, const Vec<3, T>& v)
{
	const Vec<3, T> quaternionVector(q.x(), q.y(), q.z());
	const Vec<3, T> uv(Cross(quaternionVector, v));
	const Vec<3, T> uuv(Cross(quaternionVector, uv));

	return v + ((uv * q.w()) + uuv) * static_cast<T>(2);
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T> TRAP::Math::operator*(const Vec<3, T>& v, const tQuat<T>& q)
{
	return Inverse(q) * v;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const tQuat<T>& q, const Vec<4, T>& v) noexcept
{
	return Vec<4, T>(q * Vec<3, T>(v), v.w());
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<4, T>& v, const tQuat<T>& q)
{
	return Inverse(q) * v;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::operator*(const tQuat<T>& q, const T& s) noexcept
{
	return tQuat<T>(q.w() * s, q.x() * s, q.y() * s, q.z() * s);
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::operator*(const T& s, const tQuat<T>& q) noexcept
{
	return q * s;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::operator/(const tQuat<T>& q, const T& s) noexcept
{
	return tQuat<T>(q.w() / s, q.x() / s, q.y() / s, q.z() / s);
}

//-------------------------------------------------------------------------------------------------------------------//
//Boolean operators

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr bool TRAP::Math::operator==(const tQuat<T>& q1, const tQuat<T>& q2) noexcept
{
	return q1.w() == q2.w() && q1.x() == q2.x() && q1.y() == q2.y() && q1.z() == q2.z();
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr bool TRAP::Math::operator!=(const tQuat<T>& q1, const tQuat<T>& q2) noexcept
{
	return q1.w() != q2.w() && q1.x() != q2.x() && q1.y() != q2.y() && q1.z() != q2.z();
}

//-------------------------------------------------------------------------------------------------------------------//
//std::get support
namespace std
{
	/// @brief Extracts the Ith element from the quaternion.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// @param q Quaternion whose contents to extract.
	/// @return A reference to the Ith element of q.
	template<usize I, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T& get(TRAP::Math::tQuat<T>& q) noexcept
	{
		static_assert(I < TRAP::Math::tQuat<T>::Length());

		return q[I];
	}

	/// @brief Extracts the Ith element from the quaternion.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// @param q Quaternion whose contents to extract.
	/// @return A reference to the Ith element of q.
	template<usize I, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T&& get(TRAP::Math::tQuat<T>&& q) noexcept
	{
		static_assert(I < TRAP::Math::tQuat<T>::Length());

		return std::move(q[I]);
	}

	/// @brief Extracts the Ith element from the quaternion.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// @param q Quaternion whose contents to extract.
	/// @return A reference to the Ith element of q.
	template<usize I, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr const T& get(const TRAP::Math::tQuat<T>& q) noexcept
	{
		static_assert(I < TRAP::Math::tQuat<T>::Length());

		return q[I];
	}

	/// @brief Extracts the Ith element from the quaternion.
	/// I must be an integer value in range [0, 4).
	/// This is enforced at compile time!
	/// @param q Quaternion whose contents to extract.
	/// @return A reference to the Ith element of q.
	template<usize I, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr const T&& get(const TRAP::Math::tQuat<T>&& q) noexcept
	{
		static_assert(I < TRAP::Math::tQuat<T>::Length());

		return std::move(q[I]);
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//std::swap support

namespace std
{
	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr void swap(TRAP::Math::tQuat<T>& lhs, TRAP::Math::tQuat<T>& rhs) noexcept(std::is_nothrow_move_constructible_v<TRAP::Math::tQuat<T>> &&
																                              std::is_nothrow_move_assignable_v<TRAP::Math::tQuat<T>>)
	{
		lhs.Swap(rhs);
	}
}

#endif /*TRAP_QUATERNION_H*/
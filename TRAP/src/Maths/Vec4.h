#ifndef _TRAP_VEC4_H_
#define _TRAP_VEC4_H_

#include "Types.h"

namespace TRAP::Math
{
	template<typename T>
	struct Vec<4, T>
	{
		//Implementation details
		typedef T valueType;
		typedef Vec<4, T> type;
		typedef Vec<4, bool> boolType;

		//Data
		T x, y, z, w;

		//Implicit basic constructors
		constexpr Vec() = default;
		constexpr Vec(const Vec<4, T> & v) = default;

		//Explicit basic constructors
		constexpr explicit Vec(T scalar);
		constexpr Vec(T x, T y, T z, T w);

		//Conversion scalar constructors
		template<typename U>
		constexpr explicit Vec(const Vec<1, U> & v);

		//Explicit conversions
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(X x, Y y, Z z, W w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(const Vec<1, X>&, Y y, Z z, W w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(X x, const Vec<1, Y>&, Z z, W w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(const Vec<1, X>&, const Vec<1, Y> & y, Z z, W w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(X x, Y y, const Vec<1, Z> & z, W w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(const Vec<1, X>&, Y y, const Vec<1, Z> & z, W w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(X x, const Vec<1, Y> & y, const Vec<1, Z> & z, W w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(const Vec<1, X> & x, const Vec<1, Y>&, const Vec<1, Z> & z, W w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(const Vec<1, X> & x, Y y, Z z, const Vec<1, W> & w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(X x, const Vec<1, Y> & y, Z z, const Vec<1, W> & w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(const Vec<1, X> & x, const Vec<1, Y> & y, Z z, const Vec<1, W> & w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(X x, Y y, const Vec<1, Z> & z, const Vec<1, W> & w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(const Vec<1, X> & x, Y y, const Vec<1, Z> & z, const Vec<1, W> & w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(X x, const Vec<1, Y> & y, const Vec<1, Z> & z, const Vec<1, W> & w);
		template<typename X, typename Y, typename Z, typename W>
		constexpr Vec(const Vec<1, X> & x, const Vec<1, Y> & y, const Vec<1, Z> & z, const Vec<1, W> & w);

		//Conversion vector constructors

		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(const Vec<2, A> & xy, B z, C w);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(const Vec<2, A> & xy, const Vec<1, B> & z, C w);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(const Vec<2, A> & xy, B z, const Vec<1, C> & w);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(const Vec<2, A> & xy, const Vec<1, B> & z, const Vec<1, C> & w);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr explicit Vec(A x, const Vec<2, B> & yz, C w);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(const Vec<1, A> & x, const Vec<2, B> & yz, C w);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(A x, const Vec<2, B> & yz, const Vec<1, C> & w);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(const Vec<1, A> & x, const Vec<2, B> & yz, const Vec<1, C> & w);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(A x, B y, const Vec<2, C> & zw);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(const Vec<1, A> & x, B y, const Vec<2, C> & zw);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(A x, const Vec<1, B> & y, const Vec<2, C> & zw);
		//Explicit conversions
		template<typename A, typename B, typename C>
		constexpr Vec(const Vec<1, A> & x, const Vec<1, B> & y, const Vec<2, C> & zw);
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(const Vec<3, A> & xyz, B w);
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(const Vec<3, A> & xyz, const Vec<1, B> & w);
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(A x, const Vec<3, B> & yzw);
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(const Vec<1, A> & x, const Vec<3, B> & yzw);
		//Explicit conversions
		template<typename A, typename B>
		constexpr Vec(const Vec<2, A> & xy, const Vec<2, B> & zw);

		//Explicit conversions
		template<typename U>
		constexpr explicit Vec(const Vec<4, U> & v);
		
		constexpr Vec(Vec&&) = default;
		~Vec() = default;
		constexpr Vec<4, T>& operator=(Vec&&) = default;

		//Return the count of components of the vector
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
		constexpr Vec<4, T>& operator+=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator+=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator-=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator-=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator-=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator*=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator*=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator*=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator/=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator/=(const Vec<1, U> & v);
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
		constexpr Vec<4, T>& operator%=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator%=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator&=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator&=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator&=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator|=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator|=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator|=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator^=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator^=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator^=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator<<=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator<<=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator<<=(const Vec<4, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator>>=(U scalar);
		template<typename U>
		constexpr Vec<4, T>& operator>>=(const Vec<1, U> & v);
		template<typename U>
		constexpr Vec<4, T>& operator>>=(const Vec<4, U> & v);
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
	constexpr Vec<4, T> operator+(const Vec<4, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator+(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator+(const Vec<1, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator+(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v, const T& scalar);

	template<typename T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator-(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator-(const Vec<1, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator-(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v, const T& scalar);

	template<typename T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator*(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator*(const Vec<1, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator*(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator/(const Vec<4, T>& v, const T& scalar);

	template<typename T>
	constexpr Vec<4, T> operator/(const Vec<4, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator/(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator/(const Vec<1, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator/(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator%(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator%(const Vec<4, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator%(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator%(const Vec<1, T>& scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator%(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator&(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator&(const Vec<4, T>& v, const Vec<1, T>& scalar);

	template<typename T>
	constexpr Vec<4, T> operator&(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator&(const Vec<1, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator&(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator|(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator|(const Vec<4, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator|(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator|(const Vec<1, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator|(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator^(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator^(const Vec<4, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator^(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator^(const Vec<1, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator^(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator<<(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator<<(const Vec<4, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator<<(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator<<(const Vec<1, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator<<(const Vec<4, T>& v1, const Vec<4, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator>>(const Vec<4, T>& v, T scalar);

	template<typename T>
	constexpr Vec<4, T> operator>>(const Vec<4, T>& v1, const Vec<1, T>& v2);

	template<typename T>
	constexpr Vec<4, T> operator>>(T scalar, const Vec<4, T>& v);

	template<typename T>
	constexpr Vec<4, T> operator>>(const Vec<1, T>& v1, const Vec<4, T>& v2);

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
template<typename U>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, U>& v)
	: x(static_cast<T>(v.x)),
	y(static_cast<T>(v.y)),
	z(static_cast<T>(v.z)),
	w(static_cast<T>(v.w))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(X x, Y y, Z z, W w)
	: x(static_cast<T>(x)),
	y(static_cast<T>(y)),
	z(static_cast<T>(z)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, X>& x, Y y, Z z, W w)
	: x(static_cast<T>(x)),
	y(static_cast<T>(y)),
	z(static_cast<T>(z)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(X x, const Vec<1, Y>& y, Z z, W w)
	: x(static_cast<T>(x)),
	y(static_cast<T>(y.x)),
	z(static_cast<T>(z)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, X>& x, const Vec<1, Y>& y, Z z, W w)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(y.x)),
	z(static_cast<T>(z)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(X x, Y y, const Vec<1, Z>& z, W w)
	: x(static_cast<T>(x)),
	y(static_cast<T>(y)),
	z(static_cast<T>(z.x)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, X>& x, Y y, const Vec<1, Z>& z, W w)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(y)),
	z(static_cast<T>(z.x)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(X x, const Vec<1, Y>& y, const Vec<1, Z>& z, W w)
	: x(static_cast<T>(x)),
	y(static_cast<T>(y.x)),
	z(static_cast<T>(z.x)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, X>& x, const Vec<1, Y>& y, const Vec<1, Z>& z, W w)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(y.x)),
	z(static_cast<T>(z.x)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, X>& x, Y y, Z z, const Vec<1, W>& w)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(y)),
	z(static_cast<T>(z)),
	w(static_cast<T>(w.x))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(X x, const Vec<1, Y>& y, Z z, const Vec<1, W>& w)
	: x(static_cast<T>(x)),
	y(static_cast<T>(y.x)),
	z(static_cast<T>(z)),
	w(static_cast<T>(w.x))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, X>& x, const Vec<1, Y>& y, Z z, const Vec<1, W>& w)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(y.x)),
	z(static_cast<T>(z)),
	w(static_cast<T>(w.x))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(X x, Y y, const Vec<1, Z>& z, const Vec<1, W>& w)
	: x(static_cast<T>(x)),
	y(static_cast<T>(y)),
	z(static_cast<T>(z.x)),
	w(static_cast<T>(w.x))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, X>& x, Y y, const Vec<1, Z>& z, const Vec<1, W>& w)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(y)),
	z(static_cast<T>(z.x)),
	w(static_cast<T>(w.x))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(X x, const Vec<1, Y>& y, const Vec<1, Z>& z, const Vec<1, W>& w)
	: x(static_cast<T>(x)),
	y(static_cast<T>(y.x)),
	z(static_cast<T>(z.x)),
	w(static_cast<T>(w.x))
{}

template<typename T>
template<typename X, typename Y, typename Z, typename W>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, X>& x, const Vec<1, Y>& y, const Vec<1, Z>& z, const Vec<1, W>& w)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(y.x)),
	z(static_cast<T>(z.x)),
	w(static_cast<T>(w.x))
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
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<2, A>& xy, const Vec<1, B>& z, C w)
	: x(static_cast<T>(xy.x)),
	y(static_cast<T>(xy.y)),
	z(static_cast<T>(z.x)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<2, A>& xy, B z, const Vec<1, C>& w)
	: x(static_cast<T>(xy.x)),
	y(static_cast<T>(xy.y)),
	z(static_cast<T>(z)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<2, A>& xy, const Vec<1, B>& z, const Vec<1, C>& w)
	: x(static_cast<T>(xy.x)),
	y(static_cast<T>(xy.y)),
	z(static_cast<T>(z.x)),
	w(static_cast<T>(w.x))
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
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, A>& x, const Vec<2, B>& yz, C w)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(yz.x)),
	z(static_cast<T>(yz.y)),
	w(static_cast<T>(w))
{}

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(A x, const Vec<2, B>& yz, const Vec<1, C>& w)
	: x(static_cast<T>(x)),
	y(static_cast<T>(yz.x)),
	z(static_cast<T>(yz.z)),
	w(static_cast<T>(w.x))
{}

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, A>& x, const Vec<2, B>& yz, const Vec<1, C>& w)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(yz.x)),
	z(static_cast<T>(yz.z)),
	w(static_cast<T>(w.x))
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
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, A>& x, B y, const Vec<2, C>& zw)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(y)),
	z(static_cast<T>(zw.x)),
	w(static_cast<T>(zw.y))
{}

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(A x, const Vec<1, B>& y, const Vec<2, C>& zw)
	: x(static_cast<T>(x)),
	y(static_cast<T>(y.x)),
	z(static_cast<T>(zw.x)),
	w(static_cast<T>(zw.y))
{}

template<typename T>
template<typename A, typename B, typename C>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, A>& x, const Vec<1, B>& y, const Vec<2, C>& zw)
	: x(static_cast<T>(x.x)),
	y(static_cast<T>(y.x)),
	z(static_cast<T>(zw.x)),
	w(static_cast<T>(zw.y))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<3, A>& xyz, B w)
	: x(static_cast<T>(xyz.x)),
	y(static_cast<T>(xyz.y)),
	z(static_cast<T>(xyz.z)),
	w(w)
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<3, A>& xyz, const Vec<1, B>& w)
	: x(static_cast<T>(xyz.x)),
	y(static_cast<T>(xyz.y)),
	z(static_cast<T>(xyz.z)),
	w(w)
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<4, T>::Vec(A x, const Vec<3, B>& yzw)
	: x(static_cast<T>(x)),
	y(static_cast<T>(yzw.x)),
	z(static_cast<T>(yzw.y)),
	w(static_cast<T>(yzw.w))
{}

template<typename T>
template<typename A, typename B>
constexpr TRAP::Math::Vec<4, T>::Vec(const Vec<1, A>& x, const Vec<3, B>& yzw)
	: x(static_cast<T>(x.x)),
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
	assert(i >= 0 && i < this->Length());

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
	assert(i >= 0 && i < this->Length());

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
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator+=(const Vec<1, U>& v)
{
	return (*this = Vec<4, T>(this->x + v.x, this->y + v.x, this->z + v.x, this->w + v.x));
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
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator-=(const Vec<1, U>& v)
{
	return (*this = Vec<4, T>(this->x - v.x, this->y - v.x, this->z - v.x, this->w - v.x));
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
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator*=(const Vec<1, U>& v)
{
	return (*this = Vec<4, T>(this->x * v.x, this->y * v.x, this->z * v.x, this->w * v.x));
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
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator/=(const Vec<1, U>& v)
{
	return (*this = Vec<4, T>(this->x / v.x, this->y / v.x, this->z / v.x, this->w / v.x));
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
	++* this;

	return Result;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Vec<4, T>::operator--(int)
{
	Vec<4, T> Result(*this);
	--* this;

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
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator%=(const Vec<1, U>& v)
{
	return (*this = Vec<4, T>(this->x % v.x, this->y % v.x, this->z % v.x, this->w % v.x));
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
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator&=(const Vec<1, U>& v)
{
	return (*this = Vec<4, T>(this->x & v.x, this->y & v.x, this->z & v.x, this->w & v.x));
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
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator|=(const Vec<1, U>& v)
{
	return (*this = Vec<4, T>(this->x | v.x, this->y | v.x, this->z | v.x, this->w | v.x));
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
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator^=(const Vec<1, U>& v)
{
	return (*this = Vec<4, T>(this->x ^ v.x, this->y ^ v.x, this->z ^ v.x, this->w ^ v.x));
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
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator<<=(const Vec<1, U>& v)
{
	return (*this = Vec<4, T>(this->x << v.x, this->y << v.x, this->z << v.x, this->w << v.x));
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
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator>>=(const Vec<1, U>& v)
{
	return (*this = Vec<4, T>(this->x >> v.x, this->y >> v.x, this->z >> v.x, this->w >> v.x));
}

template<typename T>
template<typename U>
constexpr TRAP::Math::Vec<4, T>& TRAP::Math::Vec<4, T>::operator>>=(const Vec<4, U>& v)
{
	return (*this = Vec<4, T>(this->x >> v.x, this->y >> v.y, this->z >> v.z, this->w >> v.w));
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
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<4, T>& v1, const Vec<1, T>& v2)
{
	return Vec<4, T>(v1) += v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(v) += scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator+(const Vec<1, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v2) += v1;
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
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<4, T>& v1, const Vec<1, T>& v2)
{
	return Vec<4, T>(v1) -= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) -= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator-(const Vec<1, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1.x) -= v2;
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
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<4, T>& v1, const Vec<1, T>& v2)
{
	return Vec<4, T>(v1) *= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(v) *= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator*(const Vec<1, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v2) *= v1;
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
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(const Vec<4, T>& v1, const Vec<1, T>& v2)
{
	return Vec<4, T>(v1) /= v2;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) /= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator/(const Vec<1, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1.x) /= v2;
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
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(const Vec<4, T>& v1, const Vec<1, T>& v2)
{
	return Vec<4, T>(v1) %= v2.x;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) %= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator%(const Vec<1, T>& scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar.x) %= v;
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
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(const Vec<4, T>& v, const Vec<1, T>& scalar)
{
	return Vec<4, T>(v) &= scalar;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) &= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator&(const Vec<1, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1.x) &= v2;
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
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(const Vec<4, T>& v1, const Vec<1, T>& v2)
{
	return Vec<4, T>(v1) |= v2.x;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) |= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator|(const Vec<1, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1.x) |= v2;
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
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(const Vec<4, T>& v1, const Vec<1, T>& v2)
{
	return Vec<4, T>(v1) ^= v2.x;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) ^= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator^(const Vec<1, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1.x) ^= v2;
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
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(const Vec<4, T>& v1, const Vec<1, T>& v2)
{
	return Vec<4, T>(v1) <<= v2.x;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) <<= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator<<(const Vec<1, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1.x) <<= v2;
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
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(const Vec<4, T>& v1, const Vec<1, T>& v2)
{
	return Vec<4, T>(v1) >>= v2.x;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(T scalar, const Vec<4, T>& v)
{
	return Vec<4, T>(scalar) >>= v;
}

template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::operator>>(const Vec<1, T>& v1, const Vec<4, T>& v2)
{
	return Vec<4, T>(v1.x) >>= v2;
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
	return v1.x != v2.x && v1.y != v2.y && v1.z != v2.z && v1.w != v2.w;
}

constexpr TRAP::Math::Vec<4, bool> TRAP::Math::operator&&(const Vec<4, bool>& v1, const Vec<4, bool>& v2)
{
	return Vec<4, bool>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
}

constexpr TRAP::Math::Vec<4, bool> TRAP::Math::operator||(const Vec<4, bool>& v1, const Vec<4, bool>& v2)
{
	return Vec<4, bool>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
}

#endif /*_TRAP_VEC4_H_*/
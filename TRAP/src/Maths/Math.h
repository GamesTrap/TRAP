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

#ifndef _TRAP_MATH_H_
#define _TRAP_MATH_H_

#include "TRAPPCH.h"

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat3.h"
#include "Mat4.h"
#include "Quaternion.h"

namespace TRAP::Math
{
	//Constants
	template<typename T>
	constexpr T Epsilon()
	{
		static_assert(std::numeric_limits<T>::is_iec559, "'epsilon' only accepts floating-point inputs");
		
		return std::numeric_limits<T>::epsilon();
	}
	template<typename T>
	constexpr T CosOneOverTwo()
	{
		return T(0.877582561890372716130286068203503191);
	}
	template<typename T>
	constexpr T PI()
	{
		return T(3.1415926535897932384626433832795);
	}
	template<typename T>
	constexpr T TAU()
	{
		return T(6.2831853071795864769252867665581);
	}
	template<typename T>
	constexpr T TwoPI()
	{
		return T(6.28318530717958647692528676655900576);
	}
	template<typename T>
	constexpr T RootPI()
	{
		return T(1.772453850905516027);
	}
	template<typename T>
	constexpr T HalfPI()
	{
		return T(1.57079632679489661923132169163975144);
	}
	template<typename T>
	constexpr T ThreeOverTwoPI()
	{
		return T(4.71238898038468985769396507491925432);
	}
	template<typename T>
	constexpr T QuarterPI()
	{
		return T(0.785398163397448309615660845819875721);
	}
	template<typename T>
	constexpr T OneOverPI()
	{
		return T(0.318309886183790671537767526745028724);
	}
	template<typename T>
	constexpr T OneOverTwoPI()
	{
		return T(0.159154943091895335768883763372514362);
	}
	template<typename T>
	constexpr T TwoOverPI()
	{
		return T(0.636619772367581343075535053490057448);
	}
	template<typename T>
	constexpr T FourOverPI()
	{
		return T(1.273239544735162686151070106980114898);
	}
	template<typename T>
	constexpr T TwoOverRootPI()
	{
		return T(1.12837916709551257389615890312154517);
	}
	
	template<typename T>
	constexpr T OneOverRootTwo()
	{
		return T(0.707106781186547524400844362104849039);
	}
	template<typename T>
	constexpr T RootHalfPI()
	{
		return T(1.253314137315500251);
	}
	template<typename T>
	constexpr T RootTwoPI()
	{
		return T(2.506628274631000502);
	}
	template<typename T>
	constexpr T RootLnFour()
	{
		return T(1.17741002251547469);
	}
	template<typename T>
	constexpr T e()
	{
		return T(2.71828182845904523536);
	}
	template<typename T>
	constexpr T Euler()
	{
		return T(0.577215664901532860606);
	}
	template<typename T>
	constexpr T RootTwo()
	{
		return T(1.41421356237309504880168872420969808);
	}
	template<typename T>
	constexpr T RootThree()
	{
		return T(1.73205080756887729352744634150587236);
	}
	template<typename T>
	constexpr T RootFive()
	{
		return T(2.23606797749978969640917366873127623);
	}
	template<typename T>
	constexpr T LnTwo()
	{
		return T(0.693147180559945309417232121458176568);
	}
	template<typename T>
	constexpr T LnTen()
	{
		return T(2.30258509299404568401799145468436421);
	}
	template<typename T>
	constexpr T LnLnTwo()
	{
		return T(-0.3665129205816643);
	}
	template<typename T>
	constexpr T Third()
	{
		return T(0.3333333333333333333333333333333333333333);
	}
	template<typename T>
	constexpr T TwoThirds()
	{
		return T(0.666666666666666666666666666666666666667);
	}
	template<typename T>
	constexpr T GoldenRatio()
	{
		return T(1.61803398874989484820458683436563811);
	}

	//-------------------------------------------------------------------------------------------------------------------//
	//Common-------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <typeparam name="genType">Floating-point or integer; scalar or vector types.</typeparam>
	/// <returns>y if y < x; otherwise it returns x.</returns>
	template<typename genType>
	constexpr genType Min(genType x, genType y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="genType">Floating-point or integer; scalar or vector types.</typeparam>
	/// <returns>y if x < y; otherwise it returns x.</returns>
	template<typename genType>
	constexpr genType Max(genType x, genType y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// The fraction 0.5 will round in a direction chosen by the implementation, presumably the direction that is fastest.
	/// This includes the posibillity that Round(x) returns the same value as RoundEven(x) for all values of x.
	/// </summary>
	/// <typeparam name="genType">Floating-point types.</typeparam>
	/// <returns>Value equal to the nearest integer to x.</returns>
	template<typename genType>
	genType Round(genType x);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <typeparam name="genType">Floating-point types.</typeparam>
	/// <returns>Value equal to the nearest integer to x whose absolute value is not larger than the absolute value of x.</returns>
	template<typename genType>
	genType Trunc(genType x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="genFIType">Floating-point or signed integer; scalar or vector types.</typeparam>
	/// <returns>x if x >= 0; otherwise it returns -x.</returns>
	template<typename genFIType>
	constexpr genFIType Abs(genFIType x);

	/// <returns>x if x >= 0; otherwise it returns -x.</returns>
	template<>
	constexpr int Abs(int x);

	/// <typeparam name="genFIType">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>x if x >= 0; otherwise it returns -x.</returns>
	template<typename T>
	constexpr Vec<2, T> Abs(const Vec<2, T>& x);
	/// <typeparam name="genFIType">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>x if x >= 0; otherwise it returns -x.</returns>
	template<typename T>
	constexpr Vec<3, T> Abs(const Vec<3, T>& x);
	/// <typeparam name="genFIType">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>x if x >= 0; otherwise it returns -x.</returns>
	template<typename T>
	constexpr Vec<4, T> Abs(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//
	
	//Fast and works for any type

	/// <typeparam name="genFIType">Floating-point scalar or vector types.</typeparam>
	/// <returns>1.0f if x > 0, 0.0 if x == 0, or -1.0 if x < 0.</returns>
	template<typename genFIType>
	genFIType Sign(genFIType x);

	/// <typeparam name="genFIType">Floating-point scalar types.</typeparam>
	/// <returns>1.0f if x > 0, 0.0 if x == 0, or -1.0 if x < 0.</returns>
	template<typename T>
	Vec<2, T> Sign(const Vec<2, T>& x);
	/// <typeparam name="genFIType">Floating-point scalar types.</typeparam>
	/// <returns>1.0f if x > 0, 0.0 if x == 0, or -1.0 if x < 0.</returns>
	template<typename T>
	Vec<3, T> Sign(const Vec<3, T>& x);
	/// <typeparam name="genFIType">Floating-point scalar types.</typeparam>
	/// <returns>1.0f if x > 0, 0.0 if x == 0, or -1.0 if x < 0.</returns>
	template<typename T>
	Vec<4, T> Sign(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar or vector types.</typeparam>
	/// <returns>Value equal to the nearest integer that is less then or equal to x.</returns>
	template<typename T>
	T Floor(T x);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer that is less then or equal to x.</returns>
	template<typename T>
	Vec<2, T> Floor(const Vec<2, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer that is less then or equal to x.</returns>
	template<typename T>
	Vec<3, T> Floor(const Vec<3, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer that is less then or equal to x.</returns>
	template<typename T>
	Vec<4, T> Floor(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer to x whose absolute value is not larger than the absolute vlaue of x.</returns>
	template<typename T>
	Vec<2, T> Trunc(const Vec<2, T>& x);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer to x whose absolute value is not larger than the absolute vlaue of x.</returns>
	template<typename T>
	Vec<3, T> Trunc(const Vec<3, T>& x);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer to x whose absolute value is not larger than the absolute vlaue of x.</returns>
	template<typename T>
	Vec<4, T> Trunc(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// The fraction 0.5 will round in a direction chosen by the implementation, presumably the direction that is fastest.
	/// This includes the posibillity that Round(x) returns the same value as RoundEven(x) for all values of x.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer to x.</returns>
	template<typename T>
	Vec<2, T> Round(const Vec<2, T>& x);
	/// <summary>
	/// The fraction 0.5 will round in a direction chosen by the implementation, presumably the direction that is fastest.
	/// This includes the posibillity that Round(x) returns the same value as RoundEven(x) for all values of x.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer to x.</returns>
	template<typename T>
	Vec<3, T> Round(const Vec<3, T>& x);
	/// <summary>
	/// The fraction 0.5 will round in a direction chosen by the implementation, presumably the direction that is fastest.
	/// This includes the posibillity that Round(x) returns the same value as RoundEven(x) for all values of x.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer to x.</returns>
	template<typename T>
	Vec<4, T> Round(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// A fractional part of 0.5 will round toward the nearest even integer. (Both 3.5 and 4.5 for x will return 4.0).
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>Value equal to the nearest integer to x.</returns>
	template<typename genType>
	genType RoundEven(genType x);

	/// <summary>
	/// A fractional part of 0.5 will round toward the nearest even integer. (Both 3.5 and 4.5 for x will return 4.0).
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer to x.</returns>
	template<typename T>
	Vec<2, T> RoundEven(const Vec<2, T>& x);
	/// <summary>
	/// A fractional part of 0.5 will round toward the nearest even integer. (Both 3.5 and 4.5 for x will return 4.0).
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer to x.</returns>
	template<typename T>
	Vec<3, T> RoundEven(const Vec<3, T>& x);
	/// <summary>
	/// A fractional part of 0.5 will round toward the nearest even integer. (Both 3.5 and 4.5 for x will return 4.0).
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer to x.</returns>
	template<typename T>
	Vec<4, T> RoundEven(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar or vector types.</typeparam>
	/// <returns>Value equal to the nearest integer that is greater than or equal to x.</returns>
	template<typename T>
	T Ceil(T x);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer that is greater than or equal to x.</returns>
	template<typename T>
	Vec<2, T> Ceil(const Vec<2, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer that is greater than or equal to x.</returns>
	template<typename T>
	Vec<3, T> Ceil(const Vec<3, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Value equal to the nearest integer that is greater than or equal to x.</returns>
	template<typename T>
	Vec<4, T> Ceil(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>x - Floor(x).</returns>
	template<typename genType>
	genType Fract(genType x);

	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>x - Floor(x).</returns>
	template<typename T>
	Vec<2, T> Fract(const Vec<2, T>& x);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>x - Floor(x).</returns>
	template<typename T>
	Vec<3, T> Fract(const Vec<3, T>& x);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>x - Floor(x).</returns>
	template<typename T>
	Vec<4, T> Fract(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <summary>
	/// Modulus.
	/// </summary>
	/// <returns>x - y * Floor(x / y) for each component in x using the floating point value y.</returns>
	template<typename genType>
	genType Mod(genType x, genType y);

	/// <summary>
	/// Modulus.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>x - y * Floor(x / y) for each component in x using the floating point value y.</returns>
	template<typename T>
	Vec<2, T> Mod(const Vec<2, T>& x, T y);
	/// <summary>
	/// Modulus.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>x - y * Floor(x / y) for each component in x using the floating point value y.</returns>
	template<typename T>
	Vec<3, T> Mod(const Vec<3, T>& x, T y);
	/// <summary>
	/// Modulus.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>x - y * Floor(x / y) for each component in x using the floating point value y.</returns>
	template<typename T>
	Vec<4, T> Mod(const Vec<4, T>& x, T y);

	/// <summary>
	/// Modulus.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>x - y * Floor(x / y) for each component in x using the floating point value y.</returns>
	template<typename T>
	Vec<2, T> Mod(const Vec<2, T>& x, const Vec<2, T>& y);
	/// <summary>
	/// Modulus.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>x - y * Floor(x / y) for each component in x using the floating point value y.</returns>
	template<typename T>
	Vec<3, T> Mod(const Vec<3, T>& x, const Vec<3, T>& y);
	/// <summary>
	/// Modulus.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>x - y * Floor(x / y) for each component in x using the floating point value y.</returns>
	template<typename T>
	Vec<4, T> Mod(const Vec<4, T>& x, const Vec<4, T>& y);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <summary>
	/// Both the return value and the output parameter will have the same sign as x.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>Fractional part of x and sets i to the integer part (as a whole number floating point value).</returns>
	template<typename genType>
	genType Modf(genType x, genType& i);

	/// <summary>
	/// Both the return value and the output parameter will have the same sign as x.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Fractional part of x and sets i to the integer part (as a whole number floating point value).</returns>
	template<typename T>
	Vec<1, T> Modf(const Vec<1, T>& x, Vec<1, T>& i);
	/// <summary>
	/// Both the return value and the output parameter will have the same sign as x.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Fractional part of x and sets i to the integer part (as a whole number floating point value).</returns>
	template<typename T>
	Vec<2, T> Modf(const Vec<2, T>& x, Vec<2, T>& i);
	/// <summary>
	/// Both the return value and the output parameter will have the same sign as x.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Fractional part of x and sets i to the integer part (as a whole number floating point value).</returns>
	template<typename T>
	Vec<3, T> Modf(const Vec<3, T>& x, Vec<3, T>& i);
	/// <summary>
	/// Both the return value and the output parameter will have the same sign as x.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Fractional part of x and sets i to the integer part (as a whole number floating point value).</returns>
	template<typename T>
	Vec<4, T> Modf(const Vec<4, T>& x, Vec<4, T>& i);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if y < x; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<2, T> Min(const Vec<2, T>& a, T b);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if y < x; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<3, T> Min(const Vec<3, T>& a, T b);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if y < x; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<4, T> Min(const Vec<4, T>& a, T b);

	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if y < x; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<2, T> Min(const Vec<2, T>& a, const Vec<2, T>& b);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if y < x; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<3, T> Min(const Vec<3, T>& a, const Vec<3, T>& b);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if y < x; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<4, T> Min(const Vec<4, T>& a, const Vec<4, T>& b);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if x < y; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<2, T> Max(const Vec<2, T>& a, T b);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if x < y; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<3, T> Max(const Vec<3, T>& a, T b);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if x < y; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<4, T> Max(const Vec<4, T>& a, T b);

	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if x < y; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<2, T> Max(const Vec<2, T>& a, const Vec<2, T>& b);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if x < y; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<3, T> Max(const Vec<3, T>& a, const Vec<3, T>& b);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>y if x < y; otherwise it returns x.</returns>
	template<typename T>
	constexpr Vec<4, T> Max(const Vec<4, T>& a, const Vec<4, T>& b);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <typeparam name="genType">Floating-point or integer; scalar or vector types.</typeparam>
	/// <returns>Min(Max(x, minval), maxVal) for each component in x using the floating-point values minval and maxVal.</returns>
	template<typename genType>
	constexpr genType Clamp(genType x, genType minVal, genType maxVal);

	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>Min(Max(x, minval), maxVal) for each component in x using the floating-point values minval and maxVal.</returns>
	template<typename T>
	constexpr Vec<2, T> Clamp(const Vec<2, T>& x, T minVal, T maxVal);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>Min(Max(x, minval), maxVal) for each component in x using the floating-point values minval and maxVal.</returns>
	template<typename T>
	constexpr Vec<3, T> Clamp(const Vec<3, T>& x, T minVal, T maxVal);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>Min(Max(x, minval), maxVal) for each component in x using the floating-point values minval and maxVal.</returns>
	template<typename T>
	constexpr Vec<4, T> Clamp(const Vec<4, T>& x, T minVal, T maxVal);

	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>Min(Max(x, minval), maxVal) for each component in x using the floating-point values minval and maxVal.</returns>
	template<typename T>
	constexpr Vec<2, T> Clamp(const Vec<2, T>& x, const Vec<2, T>& minVal, const Vec<2, T>& maxVal);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>Min(Max(x, minval), maxVal) for each component in x using the floating-point values minval and maxVal.</returns>
	template<typename T>
	constexpr Vec<3, T> Clamp(const Vec<3, T>& x, const Vec<3, T>& minVal, const Vec<3, T>& maxVal);
	/// <typeparam name="genType">Floating-point or integer scalar types.</typeparam>
	/// <returns>Min(Max(x, minval), maxVal) for each component in x using the floating-point values minval and maxVal.</returns>
	template<typename T>
	constexpr Vec<4, T> Clamp(const Vec<4, T>& x, const Vec<4, T>& minVal, const Vec<4, T>& maxVal);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <summary>
	/// If genTypeU is a floating scalar or vector:<br>
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.<br>
	/// The value for a is not restricted to the range [0, 1].<br>
	/// <br>
	/// If genTypeU is a boolean scalar or vector:<br>
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a compoennt of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point values and will have no effect on the results.
	/// </summary>
	/// <typeparam name="genTypeT">Floating-point scalar or vector.</typeparam>
	/// <typeparam name="genTypeU">Floating-point or boolean scalar or vector. It can't be a vector if it is the length of genTypeT!</typeparam>
	/// <param name="x">Value to interpolate.</param>
	/// <param name="y">Value to interpolate.</param>
	/// <param name="a">Interpolant.</param>
	/// <returns>See description.</returns>
	template<typename genTypeT, typename genTypeU>
	genTypeT Mix(genTypeT x, genTypeT y, genTypeU a);

	/// <summary>
	/// If genTypeU is a floating scalar or vector:<br>
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.<br>
	/// The value for a is not restricted to the range [0, 1].<br>
	/// <br>
	/// If genTypeU is a boolean scalar or vector:<br>
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a compoennt of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point values and will have no effect on the results.
	/// </summary>
	/// <typeparam name="genTypeT">Floating-point scalar or vector.</typeparam>
	/// <typeparam name="genTypeU">Floating-point or boolean scalar or vector. It can't be a vector if it is the length of genTypeT!</typeparam>
	/// <param name="x">Value to interpolate.</param>
	/// <param name="y">Value to interpolate.</param>
	/// <param name="a">Interpolant.</param>
	/// <returns>See description.</returns>
	template<typename T, typename U>
	Vec<2, T> Mix(const Vec<2, T>& x, const Vec<2, T>& y, U a);
	/// <summary>
	/// If genTypeU is a floating scalar or vector:<br>
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.<br>
	/// The value for a is not restricted to the range [0, 1].<br>
	/// <br>
	/// If genTypeU is a boolean scalar or vector:<br>
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a compoennt of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point values and will have no effect on the results.
	/// </summary>
	/// <typeparam name="genTypeT">Floating-point scalar or vector.</typeparam>
	/// <typeparam name="genTypeU">Floating-point or boolean scalar or vector. It can't be a vector if it is the length of genTypeT!</typeparam>
	/// <param name="x">Value to interpolate.</param>
	/// <param name="y">Value to interpolate.</param>
	/// <param name="a">Interpolant.</param>
	/// <returns>See description.</returns>
	template<typename T, typename U>
	Vec<3, T> Mix(const Vec<3, T>& x, const Vec<3, T>& y, U a);
	/// <summary>
	/// If genTypeU is a floating scalar or vector:<br>
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.<br>
	/// The value for a is not restricted to the range [0, 1].<br>
	/// <br>
	/// If genTypeU is a boolean scalar or vector:<br>
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a compoennt of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point values and will have no effect on the results.
	/// </summary>
	/// <typeparam name="genTypeT">Floating-point scalar or vector.</typeparam>
	/// <typeparam name="genTypeU">Floating-point or boolean scalar or vector. It can't be a vector if it is the length of genTypeT!</typeparam>
	/// <param name="x">Value to interpolate.</param>
	/// <param name="y">Value to interpolate.</param>
	/// <param name="a">Interpolant.</param>
	/// <returns>See description.</returns>
	template<typename T, typename U>
	Vec<4, T> Mix(const Vec<4, T>& x, const Vec<4, T>& y, U a);

	template<typename T, typename U>
	Vec<2, T> Mix(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, U>& a);
	/// <summary>
	/// If genTypeU is a floating scalar or vector:<br>
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.<br>
	/// The value for a is not restricted to the range [0, 1].<br>
	/// <br>
	/// If genTypeU is a boolean scalar or vector:<br>
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a compoennt of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point values and will have no effect on the results.
	/// </summary>
	/// <typeparam name="genTypeT">Floating-point scalar or vector.</typeparam>
	/// <typeparam name="genTypeU">Floating-point or boolean scalar or vector. It can't be a vector if it is the length of genTypeT!</typeparam>
	/// <param name="x">Value to interpolate.</param>
	/// <param name="y">Value to interpolate.</param>
	/// <param name="a">Interpolant.</param>
	/// <returns>See description.</returns>
	template<typename T, typename U>
	Vec<3, T> Mix(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, U>& a);
	/// <summary>
	/// If genTypeU is a floating scalar or vector:<br>
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.<br>
	/// The value for a is not restricted to the range [0, 1].<br>
	/// <br>
	/// If genTypeU is a boolean scalar or vector:<br>
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a compoennt of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point values and will have no effect on the results.
	/// </summary>
	/// <typeparam name="genTypeT">Floating-point scalar or vector.</typeparam>
	/// <typeparam name="genTypeU">Floating-point or boolean scalar or vector. It can't be a vector if it is the length of genTypeT!</typeparam>
	/// <param name="x">Value to interpolate.</param>
	/// <param name="y">Value to interpolate.</param>
	/// <param name="a">Interpolant.</param>
	/// <returns>See description.</returns>
	template<typename T, typename U>
	Vec<4, T> Mix(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, U>& a);

	/// <summary>
	/// Spherical linear interpolation of two quaternions.<br>
	/// The interpolation is oriented and the rotation is performed at constant speed.
	/// For short path spherical linear interpolation, use the Slerp function.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="x">A quaternion.</param>
	/// <param name="y">A quaternion.</param>
	/// <param name="a">Interpolation factor. The interpolation is defined beyond the range [0, 1].</param>
	/// <returns>Spherical linear interpolation of two quaternions.</returns>
	template<typename T>
	tQuaternion<T> Mix(const tQuaternion<T>& x, const tQuaternion<T>& y, T a);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <returns>0.0 if x < edge, otherwise it returns 1.0 for each component of a genType.</returns>
	template<typename genType>
	genType Step(genType edge, genType x);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>0.0 if x < edge, otherwise it returns 1.0.</returns>
	template<typename T>
	Vec<2, T> Step(T edge, const Vec<2, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>0.0 if x < edge, otherwise it returns 1.0.</returns>
	template<typename T>
	Vec<3, T> Step(T edge, const Vec<3, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>0.0 if x < edge, otherwise it returns 1.0.</returns>
	template<typename T>
	Vec<4, T> Step(T edge, const Vec<4, T>& x);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>0.0 if x < edge, otherwise it returns 1.0.</returns>
	template<typename T>
	Vec<2, T> Step(const Vec<2, T>& edge, const Vec<2, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>0.0 if x < edge, otherwise it returns 1.0.</returns>
	template<typename T>
	Vec<3, T> Step(const Vec<3, T>& edge, const Vec<3, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>0.0 if x < edge, otherwise it returns 1.0.</returns>
	template<typename T>
	Vec<4, T> Step(const Vec<4, T>& edge, const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <summary>
	/// This is useful in cases where you would want a threshold function with a smooth transition.<br>
	/// This is equivalent to: getnType t; t = Clamp((x - edge0) / (edge1 - edge0), 0, 1); return t * t * (3 - 2 * t);<br>
	/// Note: Results are undefined if edge0 >= edge1!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.</returns>
	template<typename genType>
	genType SmoothStep(genType edge0, genType edge1, genType x);	

	/// <summary>
	/// This is useful in cases where you would want a threshold function with a smooth transition.<br>
	/// This is equivalent to: getnType t; t = Clamp((x - edge0) / (edge1 - edge0), 0, 1); return t * t * (3 - 2 * t);<br>
	/// Note: Results are undefined if edge0 >= edge1!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.</returns>
	template<typename T>
	Vec<2, T> SmoothStep(T edge0, T edge1, const Vec<2, T>& x);
	/// <summary>
	/// This is useful in cases where you would want a threshold function with a smooth transition.<br>
	/// This is equivalent to: getnType t; t = Clamp((x - edge0) / (edge1 - edge0), 0, 1); return t * t * (3 - 2 * t);<br>
	/// Note: Results are undefined if edge0 >= edge1!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.</returns>
	template<typename T>
	Vec<3, T> SmoothStep(T edge0, T edge1, const Vec<3, T>& x);
	/// <summary>
	/// This is useful in cases where you would want a threshold function with a smooth transition.<br>
	/// This is equivalent to: getnType t; t = Clamp((x - edge0) / (edge1 - edge0), 0, 1); return t * t * (3 - 2 * t);<br>
	/// Note: Results are undefined if edge0 >= edge1!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.</returns>
	template<typename T>
	Vec<4, T> SmoothStep(T edge0, T edge1, const Vec<4, T>& x);

	/// <summary>
	/// This is useful in cases where you would want a threshold function with a smooth transition.<br>
	/// This is equivalent to: getnType t; t = Clamp((x - edge0) / (edge1 - edge0), 0, 1); return t * t * (3 - 2 * t);<br>
	/// Note: Results are undefined if edge0 >= edge1!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.</returns>
	template<typename T>
	Vec<2, T> SmoothStep(const Vec<2, T>& edge0, const Vec<2, T>& edge1, const Vec<2, T>& x);
	/// <summary>
	/// This is useful in cases where you would want a threshold function with a smooth transition.<br>
	/// This is equivalent to: getnType t; t = Clamp((x - edge0) / (edge1 - edge0), 0, 1); return t * t * (3 - 2 * t);<br>
	/// Note: Results are undefined if edge0 >= edge1!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.</returns>
	template<typename T>
	Vec<3, T> SmoothStep(const Vec<3, T>& edge0, const Vec<3, T>& edge1, const Vec<3, T>& x);
	/// <summary>
	/// This is useful in cases where you would want a threshold function with a smooth transition.<br>
	/// This is equivalent to: getnType t; t = Clamp((x - edge0) / (edge1 - edge0), 0, 1); return t * t * (3 - 2 * t);<br>
	/// Note: Results are undefined if edge0 >= edge1!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.</returns>
	template<typename T>
	Vec<4, T> SmoothStep(const Vec<4, T>& edge0, const Vec<4, T>& edge1, const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <summary>
	/// Note: When using compiler fast math, this function may fail!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// True if x hold a NaN (not a number) representation in the underlying implementation's set of floating-point representations.<br>
	/// False otherwise, including for implementations with no NaN representations.
	/// </returns>
	template<typename genType>
	bool IsNaN(genType x);

	/// <summary>
	/// Note: When using compiler fast math, this function may fail!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// True if x hold a NaN (not a number) representation in the underlying implementation's set of floating-point representations.<br>
	/// False otherwise, including for implementations with no NaN representations.
	template<typename T>
	Vec<2, bool> IsNaN(const Vec<2, T>& v);
	/// <summary>
	/// Note: When using compiler fast math, this function may fail!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// True if x hold a NaN (not a number) representation in the underlying implementation's set of floating-point representations.<br>
	/// False otherwise, including for implementations with no NaN representations.
	template<typename T>
	Vec<3, bool> IsNaN(const Vec<3, T>& v);
	/// <summary>
	/// Note: When using compiler fast math, this function may fail!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// True if x hold a NaN (not a number) representation in the underlying implementation's set of floating-point representations.<br>
	/// False otherwise, including for implementations with no NaN representations.
	template<typename T>
	Vec<4, bool> IsNaN(const Vec<4, T>& v);

	/// <summary>
	/// Note: When using compiler fast math, this function may fail!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar type.</typeparam>
	/// <returns>
	/// True if x hold a NaN (not a number) representation in the underlying implementation's set of floating-point representations.<br>
	/// False otherwise, including for implementations with no NaN representations.
	template<typename T>
	Vec<4, bool> IsNaN(const tQuaternion<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// True if x holds a positive infinity or negative infinity representation in the underlying implementation's set of floating-point representations.<br>
	/// False otherwise, including for implementations with no infinity representations.
	/// </returns>
	template<typename genType>
	bool IsInf(genType x);

	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// True if x holds a positive infinity or negative infinity representation in the underlying implementation's set of floating-point representations.<br>
	/// False otherwise, including for implementations with no infinity representations.
	/// </returns>
	template<typename T>
	Vec<2, bool> IsInf(const Vec<2, T>& v);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// True if x holds a positive infinity or negative infinity representation in the underlying implementation's set of floating-point representations.<br>
	/// False otherwise, including for implementations with no infinity representations.
	/// </returns>
	template<typename T>
	Vec<3, bool> IsInf(const Vec<3, T>& v);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// True if x holds a positive infinity or negative infinity representation in the underlying implementation's set of floating-point representations.<br>
	/// False otherwise, including for implementations with no infinity representations.
	/// </returns>
	template<typename T>
	Vec<4, bool> IsInf(const Vec<4, T>& v);

	/// <typeparam name="genType">Floating-point scalar type.</typeparam>
	/// <returns>
	/// True if x holds a positive infinity or negative infinity representation in the underlying implementation's set of floating-point representations.<br>
	/// False otherwise, including for implementations with no infinity representations.
	/// </returns>
	template<typename T>
	Vec<4, bool> IsInf(const tQuaternion<T>& q);
	
	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <summary>
	/// Computes and returns a * b + c.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>Computes and returns a * b + c.</returns>
	template<typename genType>
	genType FMA(const genType& a, const genType& b, const genType& c);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <summary>
	/// Splits x into a floating-point significant in the range [0.5, 1.0) and an integral exponent of two,
	/// such that x = significant * exp(2, exponent).<br>
	/// This significant is returned by the function and exponent is returned in the parameter exp.<br>
	/// For a floating-point value of zero, the significant and exponent are both zero.<br>
	/// Note: For a floating-point value that is an infinity or is not a number, the results are undefined!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>See description.</returns>
	template<typename genType>
	genType FrExp(genType x, int& exp);

	/// <summary>
	/// Splits x into a floating-point significant in the range [0.5, 1.0) and an integral exponent of two,
	/// such that x = significant * exp(2, exponent).<br>
	/// This significant is returned by the function and exponent is returned in the parameter exp.<br>
	/// For a floating-point value of zero, the significant and exponent are both zero.<br>
	/// Note: For a floating-point value that is an infinity or is not a number, the results are undefined!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>See description.</returns>
	template<typename T>
	Vec<2, T> FrExp(const Vec<2, T>& v, const Vec<2, int>& exp);
	/// <summary>
	/// Splits x into a floating-point significant in the range [0.5, 1.0) and an integral exponent of two,
	/// such that x = significant * exp(2, exponent).<br>
	/// This significant is returned by the function and exponent is returned in the parameter exp.<br>
	/// For a floating-point value of zero, the significant and exponent are both zero.<br>
	/// Note: For a floating-point value that is an infinity or is not a number, the results are undefined!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>See description.</returns>
	template<typename T>
	Vec<3, T> FrExp(const Vec<3, T>& v, const Vec<3, int>& exp);
	/// <summary>
	/// Splits x into a floating-point significant in the range [0.5, 1.0) and an integral exponent of two,
	/// such that x = significant * Exp(2, exponent).<br>
	/// This significant is returned by the function and exponent is returned in the parameter exp.<br>
	/// For a floating-point value of zero, the significant and exponent are both zero.<br>
	/// Note: For a floating-point value that is an infinity or is not a number, the results are undefined!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>See description.</returns>
	template<typename T>
	Vec<4, T> FrExp(const Vec<4, T>& v, const Vec<4, int>& exp);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <summary>
	/// Builds a floating-point number from x and the corresponding integral exponent of two in exp, returning:<br>
	/// significant * Exp(2, exponent).<br>
	/// Note: If this product is too large to be represented in the floating-point type, the result is undefined!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>See description.</returns>
	template<typename genType>
	genType LdExp(const genType& x, const int& exp);

	/// <summary>
	/// Builds a floating-point number from x and the corresponding integral exponent of two in exp, returning:<br>
	/// significant * Exp(2, exponent).<br>
	/// Note: If this product is too large to be represented in the floating-point type, the result is undefined!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>See description.</returns>
	template<typename T>
	Vec<2, T> LdExp(const Vec<2, T>& v, const Vec<2, int>& exp);
	/// <summary>
	/// Builds a floating-point number from x and the corresponding integral exponent of two in exp, returning:<br>
	/// significant * Exp(2, exponent).<br>
	/// Note: If this product is too large to be represented in the floating-point type, the result is undefined!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>See description.</returns>
	template<typename T>
	Vec<3, T> LdExp(const Vec<3, T>& v, const Vec<3, int>& exp);
	/// <summary>
	/// Builds a floating-point number from x and the corresponding integral exponent of two in exp, returning:<br>
	/// significant * Exp(2, exponent).<br>
	/// Note: If this product is too large to be represented in the floating-point type, the result is undefined!
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar or vector types.</typeparam>
	/// <returns>See description.</returns>
	template<typename T>
	Vec<4, T> LdExp(const Vec<4, T>& v, const Vec<4, int>& exp);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <returns>True if the value is a power of two number.</returns>
	template<typename genType>
	bool IsPowerOfTwo(genType value);

	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>True if the value is a power of two number.</returns>
	template<typename T>
	Vec<2, bool> IsPowerOfTwo(const Vec<2, T>& v);
	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>True if the value is a power of two number.</returns>
	template<typename T>
	Vec<3, bool> IsPowerOfTwo(const Vec<3, T>& v);
	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>True if the value is a power of two number.</returns>
	template<typename T>
	Vec<4, bool> IsPowerOfTwo(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Similar to 'Mod' but with a different rounding and integer support.
	/// </summary>
	/// <returns>'x - y * Trunc(x / y)' instead of 'x - y * Floor(x / y)'.</returns>
	template<typename genType>
	genType FMod(genType x, genType y);

	/// <summary>
	/// Similar to 'Mod' but with a different rounding and integer support.
	/// </summary>
	/// <returns>'x - y * Trunc(x / y)' instead of 'x - y * Floor(x / y)'.</returns>
	template<typename T>
	Vec<2, T> FMod(const Vec<2, T>& x, T y);
	/// <summary>
	/// Similar to 'Mod' but with a different rounding and integer support.
	/// </summary>
	/// <returns>'x - y * Trunc(x / y)' instead of 'x - y * Floor(x / y)'.</returns>
	template<typename T>
	Vec<3, T> FMod(const Vec<3, T>& x, T y);
	/// <summary>
	/// Similar to 'Mod' but with a different rounding and integer support.
	/// </summary>
	/// <returns>'x - y * Trunc(x / y)' instead of 'x - y * Floor(x / y)'.</returns>
	template<typename T>
	Vec<4, T> FMod(const Vec<4, T>& x, T y);

	/// <summary>
	/// Similar to 'Mod' but with a different rounding and integer support.
	/// </summary>
	/// <returns>'x - y * Trunc(x / y)' instead of 'x - y * Floor(x / y)'.</returns>
	template<typename T>
	Vec<2, T> FMod(const Vec<2, T>& x, const Vec<2, T>& y);
	/// <summary>
	/// Similar to 'Mod' but with a different rounding and integer support.
	/// </summary>
	/// <returns>'x - y * Trunc(x / y)' instead of 'x - y * Floor(x / y)'.</returns>
	template<typename T>
	Vec<3, T> FMod(const Vec<3, T>& x, const Vec<3, T>& y);
	/// <summary>
	/// Similar to 'Mod' but with a different rounding and integer support.
	/// </summary>
	/// <returns>'x - y * Trunc(x / y)' instead of 'x - y * Floor(x / y)'.</returns>
	template<typename T>
	Vec<4, T> FMod(const Vec<4, T>& x, const Vec<4, T>& y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Linear interpolation.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar or vector types.</typeparam>
	/// <returns>Linear interpolation.</returns>
	template<typename T>
	T Lerp(T x, T y, T a);

	/// <summary>
	/// Linear interpolation.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Linear interpolation.</returns>
	template<typename T>
	Vec<2, T> Lerp(const Vec<2, T>& x, const Vec<2, T>& y, T a);
	/// <summary>
	/// Linear interpolation.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Linear interpolation.</returns>
	template<typename T>
	Vec<3, T> Lerp(const Vec<3, T>& x, const Vec<3, T>& y, T a);
	/// <summary>
	/// Linear interpolation.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Linear interpolation.</returns>
	template<typename T>
	Vec<4, T> Lerp(const Vec<4, T>& x, const Vec<4, T>& y, T a);

	/// <summary>
	/// Linear interpolation.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Linear interpolation.</returns>
	template<typename T>
	Vec<2, T> Lerp(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, T>& a);
	/// <summary>
	/// Linear interpolation.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Linear interpolation.</returns>
	template<typename T>
	Vec<3, T> Lerp(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, T>& a);
	/// <summary>
	/// Linear interpolation.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Linear interpolation.</returns>
	template<typename T>
	Vec<4, T> Lerp(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, T>& a);

	/// <summary>
	/// Linear interpolation of two quaternions.<br>
	/// The interpolation is oriented.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="x">A quaternion.</param>
	/// <param name="y">A quaternion.</param>
	/// <param name="a">Interpolation factor. The interpolation is defined in the range [0, 1].</param>
	/// <returns>Linear interpolation of two quaternions.</returns>
	template<typename T>
	tQuaternion<T> Lerp(const tQuaternion<T>& x, const tQuaternion<T>& y, T a);
	
	//-------------------------------------------------------------------------------------------------------------------//
	//Exponential--------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point value.</typeparam>
	/// <param name="base">Pow function is defined for input values of 'base' defined in the range (inf-, inf+) in the limit of the type qualifier.</param>
	/// <param name="exponent">Floating-point value representing the 'exponent'.</param>
	/// <returns>'base' raised to the power 'exponent'.</returns>
	template<typename T>
	T Pow(T base, T exponent);
	
	/// <param name="base">
	/// Floating-point value.<br>
	/// Pow function is defined for input values of 'base' defined in the range (inf-, inf+) in the limit of the type qualifier.
	/// </param>
	/// <param name="exponent">Floating-point value representing the 'exponent'.</param>
	/// <returns>'base' raised to the power 'exponent'.</returns>
	template<typename T>
	Vec<2, T> Pow(const Vec<2, T>& base, const Vec<2, T>& exponent);
	/// <param name="base">
	/// Floating-point value.<br>
	/// Pow function is defined for input values of 'base' defined in the range (inf-, inf+) in the limit of the type qualifier.
	/// </param>
	/// <param name="exponent">Floating-point value representing the 'exponent'.</param>
	/// <returns>'base' raised to the power 'exponent'.</returns>
	template<typename T>
	Vec<3, T> Pow(const Vec<3, T>& base, const Vec<3, T>& exponent);
	/// <param name="base">
	/// Floating-point value.<br>
	/// Pow function is defined for input values of 'base' defined in the range (inf-, inf+) in the limit of the type qualifier.
	/// </param>
	/// <param name="exponent">Floating-point value representing the 'exponent'.</param>
	/// <returns>'base' raised to the power 'exponent'.</returns>
	template<typename T>
	Vec<4, T> Pow(const Vec<4, T>& base, const Vec<4, T>& exponent);

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Quaternion raised to a power.</returns>
	template<typename T>
	tQuaternion<T> Pow(const tQuaternion<T>& x, T y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar or vector type.</typeparam>
	/// <param name="x">Exp function is defined for input values of x defined in the range (inf-, inf+) in the limit of the type qualifier.</param>
	/// <returns>Natural exponentiation of x. i.e., e^x.</returns>
	template<typename T>
	T Exp(T x);
	
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Exp function is defined for input values of x defined in the range (inf-, inf+) in the limit of the type qualifier.</param>
	/// <returns>Natural exponentiation of x. i.e., e^x.</returns>
	template<typename T>
	Vec<2, T> Exp(const Vec<2, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Exp function is defined for input values of x defined in the range (inf-, inf+) in the limit of the type qualifier.</param>
	/// <returns>Natural exponentiation of x. i.e., e^x.</returns>
	template<typename T>
	Vec<3, T> Exp(const Vec<3, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Exp function is defined for input values of x defined in the range (inf-, inf+) in the limit of the type qualifier.</param>
	/// <returns>Natural exponentiation of x. i.e., e^x.</returns>
	template<typename T>
	Vec<4, T> Exp(const Vec<4, T>& x);

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Exponential of a quaternion.</returns>
	template<typename T>
	tQuaternion<T> Exp(const tQuaternion<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar or vector type.</typeparam>
	/// <param name="x">Log function is defined for input values of x defined in the reange (0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Natural logarithm of x, i.e., returns the value y which satisfies the equation x = e^y.</returns>
	template<typename T>
	T Log(T x);
	
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Log function is defined for input values of x defined in the reange (0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Natural logarithm of x, i.e., returns the value y which satisfies the equation x = e^y.<br>Note: Results are undefined if x <= 0!</returns>
	template<typename T>
	Vec<2, T> Log(const Vec<2, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Log function is defined for input values of x defined in the reange (0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Natural logarithm of x, i.e., returns the value y which satisfies the equation x = e^y.<br>Note: Results are undefined if x <= 0!</returns>
	template<typename T>
	Vec<3, T> Log(const Vec<3, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Log function is defined for input values of x defined in the reange (0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Natural logarithm of x, i.e., returns the value y which satisfies the equation x = e^y.<br>Note: Results are undefined if x <= 0!</returns>
	template<typename T>
	Vec<4, T> Log(const Vec<4, T>& x);

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Logarithm of a quaternion.</returns>
	template<typename T>
	tQuaternion<T> Log(const tQuaternion<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="genType">Floating-point scalar or vector type.</typeparam>
	/// <param name="x">Exp2 function is defined for input values of x defined in the range (inf-, inf+) in the limit of the type qualifier.</param>
	/// <returns>2 raised to the x power.</returns>
	template<typename genType>
	genType Exp2(genType x);

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="x">Exp2 function is defined for input values of x defined in the range (inf-, inf+) in the limit of the type qualifier.</param>
	/// <returns>2 raised to the x power.</returns>
	template<typename T>
	Vec<2, T> Exp2(const Vec<2, T>& x);
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="x">Exp2 function is defined for input values of x defined in the range (inf-, inf+) in the limit of the type qualifier.</param>
	/// <returns>2 raised to the x power.</returns>
	template<typename T>
	Vec<3, T> Exp2(const Vec<3, T>& x);
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="x">Exp2 function is defined for input values of x defined in the range (inf-, inf+) in the limit of the type qualifier.</param>
	/// <returns>2 raised to the x power.</returns>
	template<typename T>
	Vec<4, T> Exp2(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="genType">Floating point scalar or vector type.</typeparam>
	/// <param name="x">Log2 function is defined for input values of x defined in the range (0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Base 2 log of x, i.e., returns the value y, which satisfies the equation x = 2^y.</returns>
	template<typename genType>
	genType Log2(genType x);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Log2 function is defined for input values of x defined in the range (0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Base 2 log of x, i.e., returns the value y, which satisfies the equation x = 2^y.</returns>
	template<typename T>
	Vec<2, T> Log2(const Vec<2, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Log2 function is defined for input values of x defined in the range (0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Base 2 log of x, i.e., returns the value y, which satisfies the equation x = 2^y.</returns>
	template<typename T>
	Vec<3, T> Log2(const Vec<3, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Log2 function is defined for input values of x defined in the range (0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Base 2 log of x, i.e., returns the value y, which satisfies the equation x = 2^y.</returns>
	template<typename T>
	Vec<4, T> Log2(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar or vector type.</typeparam>
	/// <returns>Positive square root of x.</returns>
	template<typename T>
	T Sqrt(T x);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Sqrt function is defined for input values of x defined in the range [0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Positive square root of x.</returns>
	template<typename T>
	Vec<2, T> Sqrt(const Vec<2, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Sqrt function is defined for input values of x defined in the range [0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Positive square root of x.</returns>
	template<typename T>
	Vec<3, T> Sqrt(const Vec<3, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="x">Sqrt function is defined for input values of x defined in the range [0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Positive square root of x.</returns>
	template<typename T>
	Vec<4, T> Sqrt(const Vec<4, T>& x);

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Square root of a quaternion.</returns>
	template<typename T>
	tQuaternion<T> Sqrt(const tQuaternion<T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="genType">Floating-point scalar or vector type.</typeparam>
	/// <param name="x">InverseSqrt function is defined for input values of x defined in the range [0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Reciprocal of the positive square root of x.</returns>
	template<typename genType>
	genType InverseSqrt(genType x);

	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <param name="x">InverseSqrt function is defined for input values of x defined in the range [0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Reciprocal of the positive square root of x.</returns>
	template<typename T>
	Vec<2, T> InverseSqrt(const Vec<2, T>& x);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <param name="x">InverseSqrt function is defined for input values of x defined in the range [0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Reciprocal of the positive square root of x.</returns>
	template<typename T>
	Vec<3, T> InverseSqrt(const Vec<3, T>& x);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <param name="x">InverseSqrt function is defined for input values of x defined in the range [0, inf+) in the limit of the type qualifier.</param>
	/// <returns>Reciprocal of the positive square root of x.</returns>
	template<typename T>
	Vec<4, T> InverseSqrt(const Vec<4, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//
	//Geometric----------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <typeparam name="T">Floating-point scalar or vector type.</typeparam>
	/// <returns>Length of x, i.e., Sqrt(x * x).</returns>
	template<typename genType>
	genType Length(genType x);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Length of x, i.e., Sqrt(x * x).</returns>
	template<typename T>
	T Length(const Vec<2, T>& v);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Length of x, i.e., Sqrt(x * x).</returns>
	template<typename T>
	T Length(const Vec<3, T>& v);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Length of x, i.e., Sqrt(x * x).</returns>
	template<typename T>
	T Length(const Vec<4, T>& v);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Norm of a quaternion.</returns>
	template<typename T>
	T Length(const tQuaternion<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar or vector type.</typeparam>
	/// <returns>Distance between p0 and p1, i.e., Length(p0 - p1).</returns>
	template<typename genType>
	genType Distance(const genType& p0, const genType& p1);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Distance between p0 and p1, i.e., Length(p0 - p1).</returns>
	template<typename T>
	T Distance(const Vec<2, T>& p0, const Vec<2, T>& p1);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Distance between p0 and p1, i.e., Length(p0 - p1).</returns>
	template<typename T>
	T Distance(const Vec<3, T>& p0, const Vec<3, T>& p1);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Distance between p0 and p1, i.e., Length(p0 - p1).</returns>
	template<typename T>
	T Distance(const Vec<4, T>& p0, const Vec<4, T>& p1);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar or vector type.</typeparam>
	/// <returns>Dot product of x and y, i.e., result = x * y.</returns>
	template<typename T>
	T Dot(T x, T y);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Dot product of x and y, i.e., result = x * y.</returns>
	template<typename T>
	T Dot(const Vec<2, T>& x, const Vec<2, T>& y);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Dot product of x and y, i.e., result = x * y.</returns>
	template<typename T>
	T Dot(const Vec<3, T>& x, const Vec<3, T>& y);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Dot product of x and y, i.e., result = x * y.</returns>
	template<typename T>
	T Dot(const Vec<4, T>& x, const Vec<4, T>& y);

	/// <summary>
	/// Compute a cross product.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Compute a cross product.</returns>
	template<typename T>
	T Dot(const tQuaternion<T>& x, const tQuaternion<T>& y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Compute a cross product.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Cross product of x and y.</returns>
	template<typename T>
	Vec<3, T> Cross(const Vec<3, T>& x, const Vec<3, T>& y);

	/// <summary>
	/// Compute a cross product.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Compute a cross product.</returns>
	template<typename T>
	tQuaternion<T> Cross(const tQuaternion<T>& q1, const tQuaternion<T>& q2);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Vector in the same direction as x but with a length of 1.</returns>
	template<typename T>
	Vec<2, T> Normalize(const Vec<2, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Vector in the same direction as x but with a length of 1.</returns>
	template<typename T>
	Vec<3, T> Normalize(const Vec<3, T>& x);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Vector in the same direction as x but with a length of 1.</returns>
	template<typename T>
	Vec<4, T> Normalize(const Vec<4, T>& x);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Normalized quaternion.</returns>
	template<typename T>
	tQuaternion<T> Normalize(const tQuaternion<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>If Dot(NRef, I) < 0.0, returns N, otherwise; returns -N.</returns>
	template<typename genType>
	genType FaceForward(const genType& N, const genType& I, const genType& NRef);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>If Dot(NRef, I) < 0.0, returns N, otherwise; returns -N.</returns>
	template<typename T>
	Vec<2, T> FaceForward(const Vec<2, T>& N, const Vec<2, T>& I, const Vec<2, T>& NRef);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>If Dot(NRef, I) < 0.0, returns N, otherwise; returns -N.</returns>
	template<typename T>
	Vec<3, T> FaceForward(const Vec<3, T>& N, const Vec<3, T>& I, const Vec<3, T>& NRef);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>If Dot(NRef, I) < 0.0, returns N, otherwise; returns -N.</returns>
	template<typename T>
	Vec<4, T> FaceForward(const Vec<4, T>& N, const Vec<4, T>& I, const Vec<4, T>& NRef);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>For the incident vector I and surface orientation N, returns the reflection direction: result = I - 2.0 * Dot(N, I) * N.</returns>
	template<typename genType>
	genType Reflect(const genType& I, const genType& N);

	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>For the incident vector I and surface orientation N, returns the reflection direction: result = I - 2.0 * Dot(N, I) * N.</returns>
	template<typename T>
	Vec<2, T> Reflect(const Vec<2, T>& I, const Vec<2, T>& N);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>For the incident vector I and surface orientation N, returns the reflection direction: result = I - 2.0 * Dot(N, I) * N.</returns>
	template<typename T>
	Vec<3, T> Reflect(const Vec<3, T>& I, const Vec<3, T>& N);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>For the incident vector I and surface orientation N, returns the reflection direction: result = I - 2.0 * Dot(N, I) * N.</returns>
	template<typename T>
	Vec<4, T> Reflect(const Vec<4, T>& I, const Vec<4, T>& N);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>For the incident vector I and surface normal N, and the ratio of indices of refraction eta, returns the refraction vector.</returns>
	template<typename genType>
	genType Refract(const genType& I, const genType& N, genType eta);

	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>For the incident vector I and surface normal N, and the ratio of indices of refraction eta, returns the refraction vector.</returns>
	template<typename T>
	Vec<2, T> Refract(const Vec<2, T>& I, const Vec<2, T>& N, T eta);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>For the incident vector I and surface normal N, and the ratio of indices of refraction eta, returns the refraction vector.</returns>
	template<typename T>
	Vec<3, T> Refract(const Vec<3, T>& I, const Vec<3, T>& N, T eta);
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>For the incident vector I and surface normal N, and the ratio of indices of refraction eta, returns the refraction vector.</returns>
	template<typename T>
	Vec<4, T> Refract(const Vec<4, T>& I, const Vec<4, T>& N, T eta);
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Perform a component-wise equal-to comparison of two matrices.
	/// </summary>
	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Boolean vector which components value is true if this expression is satisfied per column of the matrices.</returns>
	template<typename T>
	constexpr Vec<3, bool> Equal(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y);
	/// <summary>
	/// Perform a component-wise equal-to comparison of two matrices.
	/// </summary>
	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Boolean vector which components value is true if this expression is satisfied per column of the matrices.</returns>
	template<typename T>
	constexpr Vec<4, bool> Equal(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y);

	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<3, bool> Equal(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, T epsilon);
	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<4, bool> Equal(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, T epsilon);

	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<3, bool> Equal(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, const Vec<3, T>& epsilon);
	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<4, bool> Equal(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, const Vec<4, T>& epsilon);

	/// <typeparam name="T">Floating-point.</typeparam>
	/// <returns>Component-wise comparison between two vectors in term of ULPs. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<3, bool> Equal(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, int ULPs);
	/// <typeparam name="T">Floating-point.</typeparam>
	/// <returns>Component-wise comparison between two vectors in term of ULPs. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<4, bool> Equal(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, int ULPs);

	/// <typeparam name="T">Floating-point.</typeparam>
	/// <returns>Component-wise comparison between two vectors in term of ULPs. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<3, bool> Equal(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, const Vec<3, int>& ULPs);
	/// <typeparam name="T">Floating-point.</typeparam>
	/// <returns>Component-wise comparison between two vectors in term of ULPs. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<4, bool> Equal(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, const Vec<3, int>& ULPs);

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename T>
	constexpr Vec<3, bool> NotEqual(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y);
	template<typename T>
	constexpr Vec<4, bool> NotEqual(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y);

	template<typename T>
	constexpr Vec<3, bool> NotEqual(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, T epsilon);
	template<typename T>
	constexpr Vec<4, bool> NotEqual(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, T epsilon);

	template<typename T>
	constexpr Vec<3, bool> NotEqual(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, const Vec<3, T>& epsilon);
	template<typename T>
	constexpr Vec<4, bool> NotEqual(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, const Vec<4, T>& epsilon);

	template<typename T>
	constexpr Vec<3, bool> NotEqual(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, int ULPs);
	template<typename T>
	constexpr Vec<4, bool> NotEqual(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, int ULPs);

	template<typename T>
	constexpr Vec<3, bool> NotEqual(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, const Vec<3, int>& ULPs);
	template<typename T>
	constexpr Vec<4, bool> NotEqual(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, const Vec<3, int>& ULPs);

	//-------------------------------------------------------------------------------------------------------------------//
	//Matrix-------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <summary>
	/// Multiply matrix x by matrix y component-wise, i.e., result[i][j] is the scalar product of x[i][j] and y[i][j].
	/// </summary>
	/// <typeparam name="T">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>Multiplied matrix.</returns>
	template<typename T>
	Mat<3, 3, T> MatrixCompMult(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y);
	/// <summary>
	/// Multiply matrix x by matrix y component-wise, i.e., result[i][j] is the scalar product of x[i][j] and y[i][j].
	/// </summary>
	/// <typeparam name="T">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>Multiplied matrix.</returns>
	template<typename T>
	Mat<4, 4, T> MatrixCompMult(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Treats the first parameter c as a column vector and the second paramter r as a row
	/// vector and does a linear algebraic matrix multiply c * r.
	/// </summary>
	/// <typeparam name="T">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>Multiplied matrix.</returns>
	template<typename T>
	Mat<3, 3, T> OuterProduct(const Vec<3, T>& c, const Vec<3, T>& r);
	/// <summary>
	/// Treats the first parameter c as a column vector and the second paramter r as a row
	/// vector and does a linear algebraic matrix multiply c * r.
	/// </summary>
	/// <typeparam name="T">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>Multiplied matrix.</returns>
	template<typename T>
	Mat<4, 4, T> OuterProduct(const Vec<4, T>& c, const Vec<4, T>& r);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>Transpoed matrix of m.</returns>
	template<typename T>
	typename Mat<3, 3, T>::transposeType Transpose(const Mat<3, 3, T>& m);
	/// <typeparam name="T">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>Transpoed matrix of m.</returns>
	template<typename T>
	typename Mat<4, 4, T>::transposeType Transpose(const Mat<4, 4, T>& m);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>Determinant of a sqaured matrix.</returns>
	template<typename T>
	T Determinant(const Mat<3, 3, T>& m);
	/// <typeparam name="T">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>Determinant of a sqaured matrix.</returns>
	template<typename T>
	T Determinant(const Mat<4, 4, T>& m);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>Inverse of a sqaured matrix.</returns>
	template<typename T>
	Mat<3, 3, T> Inverse(const Mat<3, 3, T>& m);
	/// <typeparam name="T">Floating-point or signed integer scalar types.</typeparam>
	/// <returns>Inverse of a sqaured matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Inverse(const Mat<4, 4, T>& m);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a matrix for projecting two-dimensional coordinates onto the screen.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Projection matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Orthographic(T left, T right, T bottom, T top);
	/// <summary>
	/// Create a matrix for an orthographic parallel viewing volume.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Projection matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Orthographic(T left, T right, T bottom, T top, T zNear, T zFar);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Creates a frustum matrix.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Frustum matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Frustum(T left, T right, T bottom, T top, T nearVal, T farVal);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Creates a matrix for a symetric perspective-view frustum.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="fovY">Specifies the field of view angle in the y direction. Expressed in radians.</param>
	/// <param name="aspect">Specified the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x(width) to y(height).</param>
	/// <param name="zNear">Specifies the distance from the viewer to the near clipping plange (always positive).</param>
	/// <param name="zFar">Specifies the distance from the viewer to the far clipping plane (always positive).</param>
	/// <returns>Perspective-view matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Perspective(T fovY, T aspect, T zNear, T zFar);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Builds a perspective projection matrix based on a field of view.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="fov">Expressed in radians.</param>
	/// <param name="width">Width of the viewport.</param>
	/// <param name="height">Height of the viewport.</param>
	/// <param name="zNear">Specifies the distance from the viewer to the near clipping plane (always positive).</param>
	/// <param name="zFar">Specifies the distance from the viewer to the far clipping plange (always positive).</param>
	/// <returns>Field of view based perspective projection matrix.</returns>
	template<typename T>
	Mat<4, 4, T> PerspectiveFoV(T fov, T width, T height, T zNear, T zFar);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Creates a matrix for a symmetric perspective-view frustum with far plane at infinite.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="fovY">Specifies the field of view angle, in degrees, in the y direction. Expressed in radians.</param>
	/// <param name="aspect"></param>
	/// <param name="zNear"></param>
	/// <returns>Symmetric perspective matrix.</returns>
	template<typename T>
	Mat<4, 4, T> InfinitePerspective(T fovY, T aspect, T zNear);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// If genTypeU is a floating scalar or vector:<br>
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.<br>
	/// The value for a is not restricted to the range [0, 1].<br>
	/// <br>
	/// If genTypeU is a boolean scalar or vector:<br>
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a compoennt of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point values and will have no effect on the results.
	/// </summary>
	/// <typeparam name="genTypeT">Floating-point scalar or vector.</typeparam>
	/// <typeparam name="genTypeU">Floating-point or boolean scalar or vector. It can't be a vector if it is the length of genTypeT!</typeparam>
	/// <param name="x">Value to interpolate.</param>
	/// <param name="y">Value to interpolate.</param>
	/// <param name="a">Interpolant.</param>
	/// <returns>See description.</returns>
	template<typename T, typename U>
	Mat<3, 3, T> Mix(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, const Mat<3, 3, U>& a);
	/// <summary>
	/// If genTypeU is a floating scalar or vector:<br>
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.<br>
	/// The value for a is not restricted to the range [0, 1].<br>
	/// <br>
	/// If genTypeU is a boolean scalar or vector:<br>
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a compoennt of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point values and will have no effect on the results.
	/// </summary>
	/// <typeparam name="genTypeT">Floating-point scalar or vector.</typeparam>
	/// <typeparam name="genTypeU">Floating-point or boolean scalar or vector. It can't be a vector if it is the length of genTypeT!</typeparam>
	/// <param name="x">Value to interpolate.</param>
	/// <param name="y">Value to interpolate.</param>
	/// <param name="a">Interpolant.</param>
	/// <returns>See description.</returns>
	template<typename T, typename U>
	Mat<4, 4, T> Mix(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, const Mat<4, 4, U>& a);

	/// <summary>
	/// If genTypeU is a floating scalar or vector:<br>
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.<br>
	/// The value for a is not restricted to the range [0, 1].<br>
	/// <br>
	/// If genTypeU is a boolean scalar or vector:<br>
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a compoennt of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point values and will have no effect on the results.
	/// </summary>
	/// <typeparam name="genTypeT">Floating-point scalar or vector.</typeparam>
	/// <typeparam name="genTypeU">Floating-point or boolean scalar or vector. It can't be a vector if it is the length of genTypeT!</typeparam>
	/// <param name="x">Value to interpolate.</param>
	/// <param name="y">Value to interpolate.</param>
	/// <param name="a">Interpolant.</param>
	/// <returns>See description.</returns>
	template<typename T, typename U>
	Mat<3, 3, T> Mix(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, U a);
	/// <summary>
	/// If genTypeU is a floating scalar or vector:<br>
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.<br>
	/// The value for a is not restricted to the range [0, 1].<br>
	/// <br>
	/// If genTypeU is a boolean scalar or vector:<br>
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a compoennt of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point values and will have no effect on the results.
	/// </summary>
	/// <typeparam name="genTypeT">Floating-point scalar or vector.</typeparam>
	/// <typeparam name="genTypeU">Floating-point or boolean scalar or vector. It can't be a vector if it is the length of genTypeT!</typeparam>
	/// <param name="x">Value to interpolate.</param>
	/// <param name="y">Value to interpolate.</param>
	/// <param name="a">Interpolant.</param>
	/// <returns>See description.</returns>
	template<typename T, typename U>
	Mat<4, 4, T> Mix(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, U a);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Builds a translation 4 * 4 matrix created from a vector of 3 components.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="m">Input matrix multiplied by this translation matrix.</param>
	/// <param name="v">Coordinates of a translation vector.</param>
	/// <returns>Translation matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Translate(const Mat<4, 4, T>& m, const Vec<3, T>& v);
	
	/// <summary>
	/// Transforms a matrix with a translation 4 * 4 matrix created from 3 scalars.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="v">Coordinate of a translation vector.</param>
	/// <returns>Transformation matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Translate(const Vec<3, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Builds a rotation 4 * 4 matrix created from an axis vector and an angle.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="m">Input matrix multiplied by this rotation matrix.</param>
	/// <param name="angleInRadians">Rotation angle expressed in radians.</param>
	/// <param name="v">Rotation axis, recommended to be normalized.</param>
	/// <returns>Rotation matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Rotate(const Mat<4, 4, T>& m, T angleInRadians, const Vec<3, T>& v);

	/// <summary>
	/// Builds a rotation 4 * 4 matrix created from an axis of 3 scalars and an angle expressed in radians.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="angleInRadians">Rotation angle expressed in radians.</param>
	/// <param name="v">Scalar</param>
	/// <returns>Rotation matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Rotate(T angleInRadians, const Vec<3, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Builds a scale 4 * 4 matrix created from 3 scalars.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="m">Input matrix multiplied by this scale matrix.</param>
	/// <param name="v">Ratio of scaling for each axis.</param>
	/// <returns>Scaled matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Scale(const Mat<4, 4, T>& m, const Vec<3, T>& v);

	/// <summary>
	/// Transforms a matrix with a scale 4 * 4 matrix created from a vector of 3 components.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="v">3 Component vector.</param>
	/// <returns>Scaled matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Scale(const Vec<3, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Build a look at view matrix.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="eye">Position of the camera.</param>
	/// <param name="center">Position where the camera is looking at.</param>
	/// <param name="up">Normalized up vector, how the camera is oriented. Typically (0, 0, 1).</param>
	/// <returns>Look at view matrix.</returns>
	template<typename T>
	Mat<4, 4, T> LookAt(const Vec<3, T>& eye, const Vec<3, T>& center, const Vec<3, T>& up);

	//-------------------------------------------------------------------------------------------------------------------//
	//Quaternion---------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Spherical linear interpolation of two quaternions.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="x">A quaternion.</param>
	/// <param name="y">A quaternion.</param>
	/// <param name="a">Interpolation factor. The interpolation is defined beyond the range [0, 1].</param>
	/// <returns>Interpolated quaternions.</returns>
	template<typename T>
	tQuaternion<T> SLerp(const tQuaternion<T>& x, const tQuaternion<T>& y, T a);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Conjugated quaternion.</returns>
	template<typename T>
	tQuaternion<T> Conjugate(const tQuaternion<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Inverted quaternion.</returns>
	template<typename T>
	tQuaternion<T> Inverse(const tQuaternion<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Euler angles, pitch as x, yaw as y, roll as z. The result is expressed in radians.</returns>
	template<typename T>
	Vec<3, T> EulerAngles(const tQuaternion<T>& x);	
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Roll value of euler angles expressed in radians.</returns>
	template<typename T>
	T Roll(const tQuaternion<T>& q);	
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Pitch value of euler angles expressed in radians.</returns>
	template<typename T>
	T Pitch(const tQuaternion<T>& q);
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Yaw value of euler angles expressed in radians.</returns>
	template<typename T>
	T Yaw(const tQuaternion<T>& x);
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Converts a quaternion to a 3 * 3 matrix.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Converted matrix.</returns>
	template<typename T>
	Mat<3, 3, T> Mat3Cast(const tQuaternion<T>& q);
	/// <summary>
	/// Converts a quaternion to a 4 * 4 matrix.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Converted matrix.</returns>
	template<typename T>
	Mat<4, 4, T> Mat4Cast(const tQuaternion<T>& q);
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Converts a pure rotation 3 * 3 matrix to a quaternion.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Converted quaternion.</returns>
	template<typename T>
	tQuaternion<T> QuaternionCast(const Mat<3, 3, T>& m);
	/// <summary>
	/// Converts a pure rotation 4 * 4 matrix to a quaternion.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Converted quaternion.</returns>
	template<typename T>
	tQuaternion<T> QuaternionCast(const Mat<4, 4, T>& m);
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Component-wise comparison result of x < y.</returns>
	template<typename T>
	Vec<4, bool> LessThan(const tQuaternion<T>& x, const tQuaternion<T>& y);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Component-wise comparison result of x <= y.</returns>
	template<typename T>
	Vec<4, bool> LessThanEqual(const tQuaternion<T>& x, const tQuaternion<T>& y);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Component-wise comparison result of x > y.</returns>
	template<typename T>
	Vec<4, bool> GreaterThan(const tQuaternion<T>& x, const tQuaternion<T>& y);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Component-wise comparison result of x >= y.</returns>
	template<typename T>
	Vec<4, bool> GreaterThanEqual(const tQuaternion<T>& x, const tQuaternion<T>& y);
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Build a look at quaternion.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="direction">Desired forward direction. Needs to be normalized.</param>
	/// <param name="up">Up vector, how the camera is oriented. Typically (0, 1, 0).</param>
	/// <returns>Look at quaternion.</returns>
	template<typename T>
	tQuaternion<T> QuaternionLookAt(const Vec<3, T>& direction, const Vec<3, T>& up);
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Component-wise comparison of result x == y.</returns>
	template<typename T>
	Vec<4, bool> Equal(const tQuaternion<T>& x, const tQuaternion<T>& y);

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon.</returns>
	template<typename T>
	Vec<4, bool> Equal(const tQuaternion<T>& x, const tQuaternion<T>& y, T epsilon);	
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Component-wise comparison of result x != y.</returns>
	template<typename T>
	Vec<4, bool> NotEqual(const tQuaternion<T>& x, const tQuaternion<T>& y);
	
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| >= epsilon.</returns>
	template<typename T>
	Vec<4, bool> NotEqual(const tQuaternion<T>& x, const tQuaternion<T>& y, T epsilon);	
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Quaternion rotation angle.</returns>
	template<typename T>
	T Angle(const tQuaternion<T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>Rotation axis.</returns>
	template<typename T>
	Vec<3, T> Axis(const tQuaternion<T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Build a quaternion from an angle and a normalized axis.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <param name="angle">Angle expressed in radians.</param>
	/// <param name="v">Axis of the quaternion, must be normalized.</param>
	/// <returns>Quaternion.</returns>
	template<typename T>
	tQuaternion<T> AngleAxis(const T& angle, const Vec<3, T>& v);
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Rotates a quaternion from a vector of 3 components axis and an angle.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <param name="q">Sourcec orientation.</param>
	/// <param name="angle">Angle expressed in radians.</param>
	/// <param name="v">Axis of the rotation.</param>
	/// <returns>Rotated quaternion.</returns>
	template<typename T>
	tQuaternion<T> Rotate(const tQuaternion<T>& q, const T& angle, const Vec<3, T>& v);
	
	//-------------------------------------------------------------------------------------------------------------------//
	//Vector-------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// <returns>Normalized rotation vector for the x-axis.</returns>
	template<typename T>
	constexpr Vec<3, T> XAxis();

	/// <returns>Normalized rotation vector for the x-axis.</returns>
	template<typename T>
	constexpr Vec<3, T> YAxis();

	/// <returns>Normalized rotation vector for the x-axis.</returns>
	template<typename T>
	constexpr Vec<3, T> ZAxis();
	
	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x < y.</returns>
	template<typename T>
	constexpr Vec<2, bool> LessThan(const Vec<2, T>& x, const Vec<2, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x < y.</returns>
	template<typename T>
	constexpr Vec<3, bool> LessThan(const Vec<3, T>& x, const Vec<3, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x < y.</returns>
	template<typename T>
	constexpr Vec<4, bool> LessThan(const Vec<4, T>& x, const Vec<4, T>& y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x <= y.</returns>
	template<typename T>
	constexpr Vec<2, bool> LessThanEqual(const Vec<2, T>& x, const Vec<2, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x <= y.</returns>
	template<typename T>
	constexpr Vec<3, bool> LessThanEqual(const Vec<3, T>& x, const Vec<3, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x <= y.</returns>
	template<typename T>
	constexpr Vec<4, bool> LessThanEqual(const Vec<4, T>& x, const Vec<4, T>& y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x > y.</returns>
	template<typename T>
	constexpr Vec<2, bool> GreaterThan(const Vec<2, T>& x, const Vec<2, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x > y.</returns>
	template<typename T>
	constexpr Vec<3, bool> GreaterThan(const Vec<3, T>& x, const Vec<3, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x > y.</returns>
	template<typename T>
	constexpr Vec<4, bool> GreaterThan(const Vec<4, T>& x, const Vec<4, T>& y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x >= y.</returns>
	template<typename T>
	constexpr Vec<2, bool> GreaterThanEqual(const Vec<2, T>& x, const Vec<2, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x >= y.</returns>
	template<typename T>
	constexpr Vec<3, bool> GreaterThanEqual(const Vec<3, T>& x, const Vec<3, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x >= y.</returns>
	template<typename T>
	constexpr Vec<4, bool> GreaterThanEqual(const Vec<4, T>& x, const Vec<4, T>& y);
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x == y.</returns>
	template<typename T>
	constexpr Vec<2, bool> Equal(const Vec<2, T>& x, const Vec<2, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x == y.</returns>
	template<typename T>
	constexpr Vec<3, bool> Equal(const Vec<3, T>& x, const Vec<3, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x == y.</returns>
	template<typename T>
	constexpr Vec<4, bool> Equal(const Vec<4, T>& x, const Vec<4, T>& y);

	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<2, bool> Equal(const Vec<2, T>& x, const Vec<2, T>& y, T epsilon);
	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<3, bool> Equal(const Vec<3, T>& x, const Vec<3, T>& y, T epsilon);
	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<4, bool> Equal(const Vec<4, T>& x, const Vec<4, T>& y, T epsilon);

	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<2, bool> Equal(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, T>& epsilon);
	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<3, bool> Equal(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, T>& epsilon);
	/// <typeparam name="T">Floating-point or integer scalar types.</typeparam>
	/// <returns>Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.</returns>
	template<typename T>
	constexpr Vec<4, bool> Equal(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, T>& epsilon);

	/// <typeparam name="T">Floating-point.</typeparam>
	/// <returns>Component-wise comparison between two vectors in term of ULPs. True if this is satisfied.</returns>
	template<typename T>
	constexpr Vec<2, bool> Equal(const Vec<2, T>& x, const Vec<2, T>& y, int ULPs);
	/// <typeparam name="T">Floating-point.</typeparam>
	/// <returns>Component-wise comparison between two vectors in term of ULPs. True if this is satisfied.</returns>
	template<typename T>
	constexpr Vec<3, bool> Equal(const Vec<3, T>& x, const Vec<3, T>& y, int ULPs);
	/// <typeparam name="T">Floating-point.</typeparam>
	/// <returns>Component-wise comparison between two vectors in term of ULPs. True if this is satisfied.</returns>
	template<typename T>
	constexpr Vec<4, bool> Equal(const Vec<4, T>& x, const Vec<4, T>& y, int ULPs);

	/// <typeparam name="T">Floating-point.</typeparam>
	/// <returns>Component-wise comparison between two vectors in term of ULPs. True if this is satisfied.</returns>
	template<typename T>
	constexpr Vec<2, bool> Equal(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, int>& ULPs);
	/// <typeparam name="T">Floating-point.</typeparam>
	/// <returns>Component-wise comparison between two vectors in term of ULPs. True if this is satisfied.</returns>
	template<typename T>
	constexpr Vec<3, bool> Equal(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, int>& ULPs);
	/// <typeparam name="T">Floating-point.</typeparam>
	/// <returns>Component-wise comparison between two vectors in term of ULPs. True if this is satisfied.</returns>
	template<typename T>
	constexpr Vec<4, bool> Equal(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, int>& ULPs);
	
	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x != y.</returns>
	template<typename T>
	constexpr Vec<2, bool> NotEqual(const Vec<2, T>& x, const Vec<2, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x != y.</returns>
	template<typename T>
	constexpr Vec<3, bool> NotEqual(const Vec<3, T>& x, const Vec<3, T>& y);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x != y.</returns>
	template<typename T>
	constexpr Vec<4, bool> NotEqual(const Vec<4, T>& x, const Vec<4, T>& y);

	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x != y.</returns>
	template<typename T>
	constexpr Vec<2, bool> NotEqual(const Vec<2, T>& x, const Vec<2, T>& y, T epsilon);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x != y.</returns>
	template<typename T>
	constexpr Vec<3, bool> NotEqual(const Vec<3, T>& x, const Vec<3, T>& y, T epsilon);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x != y.</returns>
	template<typename T>
	constexpr Vec<4, bool> NotEqual(const Vec<4, T>& x, const Vec<4, T>& y, T epsilon);

	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x != y.</returns>
	template<typename T>
	constexpr Vec<2, bool> NotEqual(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, T>& epsilon);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x != y.</returns>
	template<typename T>
	constexpr Vec<3, bool> NotEqual(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, T>& epsilon);
	/// <typeparam name="T">Floating-point or integer scalar type.</typeparam>
	/// <returns>Component-wise comparison result of x != y.</returns>
	template<typename T>
	constexpr Vec<4, bool> NotEqual(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, T>& epsilon);

	/// <typeparam name="T">Floating-point</typeparam>
	/// <returns>Component-wise comparison between the two vectors in term of ULPs.</returns>
	template<typename T>
	constexpr Vec<2, bool> NotEqual(const Vec<2, T>& x, const Vec<2, T>& y, int ULPs);
	/// <typeparam name="T">Floating-point</typeparam>
	/// <returns>Component-wise comparison between the two vectors in term of ULPs.</returns>
	template<typename T>
	constexpr Vec<3, bool> NotEqual(const Vec<3, T>& x, const Vec<3, T>& y, int ULPs);
	/// <typeparam name="T">Floating-point</typeparam>
	/// <returns>Component-wise comparison between the two vectors in term of ULPs.</returns>
	template<typename T>
	constexpr Vec<4, bool> NotEqual(const Vec<4, T>& x, const Vec<4, T>& y, int ULPs);

	/// <typeparam name="T">Floating-point</typeparam>
	/// <returns>Component-wise comparison between the two vectors in term of ULPs.</returns>
	template<typename T>
	constexpr Vec<2, bool> NotEqual(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, int>& ULPs);
	/// <typeparam name="T">Floating-point</typeparam>
	/// <returns>Component-wise comparison between the two vectors in term of ULPs.</returns>
	template<typename T>
	constexpr Vec<3, bool> NotEqual(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, int>& ULPs);
	/// <typeparam name="T">Floating-point</typeparam>
	/// <returns>Component-wise comparison between the two vectors in term of ULPs.</returns>
	template<typename T>
	constexpr Vec<4, bool> NotEqual(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, int>& ULPs);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <returns>True if any component of v is true.</returns>
	constexpr bool Any(const Vec<2, bool>& v);
	/// <returns>True if any component of v is true.</returns>
	constexpr bool Any(const Vec<3, bool>& v);
	/// <returns>True if any component of v is true.</returns>
	constexpr bool Any(const Vec<4, bool>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <returns>True if all component of v are true.</returns>
	constexpr bool All(const Vec<2, bool>& v);
	/// <returns>True if all component of v are true.</returns>
	constexpr bool All(const Vec<3, bool>& v);
	/// <returns>True if all component of v are true.</returns>
	constexpr bool All(const Vec<4, bool>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <returns>Component-wise logical complement of x.</returns>
	constexpr Vec<2, bool> Not(const Vec<2, bool>& v);
	/// <returns>Component-wise logical complement of x.</returns>
	constexpr Vec<3, bool> Not(const Vec<3, bool>& v);
	/// <returns>Component-wise logical complement of x.</returns>
	constexpr Vec<4, bool> Not(const Vec<4, bool>& v);
	
	//-------------------------------------------------------------------------------------------------------------------//
	//Trigonometric------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Converts degrees to radians and returns the result.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Radians.</returns>
	template<typename genType>
	constexpr genType Radians(genType degrees);

	/// <summary>
	/// Converts degrees to radians and returns the result.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Radians.</returns>
	template<typename T>
	constexpr Vec<2, T> Radians(const Vec<2, T>& v);
	/// <summary>
	/// Converts degrees to radians and returns the result.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Radians.</returns>
	template<typename T>
	constexpr Vec<3, T> Radians(const Vec<3, T>& v);
	/// <summary>
	/// Converts degrees to radians and returns the result.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Radians.</returns>
	template<typename T>
	constexpr Vec<4, T> Radians(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Converts radians to degrees and returns the result.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Degrees.</returns>
	template<typename genType>
	constexpr genType Degrees(genType radians);

	/// <summary>
	/// Converts radians to degrees and returns the result.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Degrees.</returns>
	template<typename T>
	constexpr Vec<2, T> Degrees(const Vec<2, T>& v);
	/// <summary>
	/// Converts radians to degrees and returns the result.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Degrees.</returns>
	template<typename T>
	constexpr Vec<3, T> Degrees(const Vec<3, T>& v);
	/// <summary>
	/// Converts radians to degrees and returns the result.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Degrees.</returns>
	template<typename T>
	constexpr Vec<4, T> Degrees(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Standard trigonometric sine functon.<br>
	/// Values returned by this function will range from [-1, 1].
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Sine.</returns>
	template<typename T>
	T Sin(T x);
	
	/// <summary>
	/// Standard trigonometric sine functon.<br>
	/// Values returned by this function will range from [-1, 1].
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Sine.</returns>
	template<typename T>
	Vec<2, T> Sin(const Vec<2, T>& v);
	/// <summary>
	/// Standard trigonometric sine functon.<br>
	/// Values returned by this function will range from [-1, 1].
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Sine.</returns>
	template<typename T>
	Vec<3, T> Sin(const Vec<3, T>& v);
	/// <summary>
	/// Standard trigonometric sine functon.<br>
	/// Values returned by this function will range from [-1, 1].
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Sine.</returns>
	template<typename T>
	Vec<4, T> Sin(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Standard trigonometric cosine functon.<br>
	/// Values returned by this function will range from [-1, 1].
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Cosine.</returns>
	template<typename T>
	T Cos(T x);

	/// <summary>
	/// Standard trigonometric cosine functon.<br>
	/// Values returned by this function will range from [-1, 1].
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Cosine.</returns>
	template<typename T>
	Vec<2, T> Cos(const Vec<2, T>& v);
	/// <summary>
	/// Standard trigonometric cosine functon.<br>
	/// Values returned by this function will range from [-1, 1].
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Cosine.</returns>
	template<typename T>
	Vec<3, T> Cos(const Vec<3, T>& v);
	/// <summary>
	/// Standard trigonometric cosine functon.<br>
	/// Values returned by this function will range from [-1, 1].
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Cosine.</returns>
	template<typename T>
	Vec<4, T> Cos(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Standard trigonometric tangent functon.<br>
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Tangent.</returns>
	template<typename T>
	T Tan(T x);
	
	/// <summary>
	/// Standard trigonometric tangent functon.<br>
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Tangent.</returns>
	template<typename T>
	Vec<2, T> Tan(const Vec<2, T>& v);
	/// <summary>
	/// Standard trigonometric tangent functon.<br>
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Tangent.</returns>
	template<typename T>
	Vec<3, T> Tan(const Vec<3, T>& v);
	/// <summary>
	/// Standard trigonometric tangent functon.<br>
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Tangent.</returns>
	template<typename T>
	Vec<4, T> Tan(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Arc sine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>
	/// Angle whose sine is x.<br>
	/// The range of values returned by this function is [-PI / 2, PI / 2].<br>
	/// Note: Results are undefined if |x| > 1!
	/// </returns>
	template<typename T>
	T ASin(T x);
	
	/// <summary>
	/// Arc sine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>
	/// Angle whose sine is x.<br>
	/// The range of values returned by this function is [-PI / 2, PI / 2].<br>
	/// Note: Results are undefined if |x| > 1!
	/// </returns>
	template<typename T>
	Vec<2, T> ASin(const Vec<2, T>& v);
	/// <summary>
	/// Arc sine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>
	/// Angle whose sine is x.<br>
	/// The range of values returned by this function is [-PI / 2, PI / 2].<br>
	/// Note: Results are undefined if |x| > 1!
	/// </returns>
	template<typename T>
	Vec<3, T> ASin(const Vec<3, T>& v);
	/// <summary>
	/// Arc sine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>
	/// Angle whose sine is x.<br>
	/// The range of values returned by this function is [-PI / 2, PI / 2].<br>
	/// Note: Results are undefined if |x| > 1!
	/// </returns>
	template<typename T>
	Vec<4, T> ASin(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Arc cosine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>
	/// Angle whose cosine is x.
	/// The range of values returned by this function is [0, PI].<br>
	/// Note: Results are undefined if |x| > 1!
	/// </returns>
	template<typename T>
	T ACos(T x);
	
	/// <summary>
	/// Arc cosine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>
	/// Angle whose cosine is x.
	/// The range of values returned by this function is [0, PI].<br>
	/// Note: Results are undefined if |x| > 1!
	/// </returns>
	template<typename T>
	Vec<2, T> ACos(const Vec<2, T>& v);
	/// <summary>
	/// Arc cosine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>
	/// Angle whose cosine is x.
	/// The range of values returned by this function is [0, PI].<br>
	/// Note: Results are undefined if |x| > 1!
	/// </returns>
	template<typename T>
	Vec<3, T> ACos(const Vec<3, T>& v);
	/// <summary>
	/// Arc cosine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar type.</typeparam>
	/// <returns>
	/// Angle whose cosine is x.
	/// The range of values returned by this function is [0, PI].<br>
	/// Note: Results are undefined if |x| > 1!
	/// </returns>
	template<typename T>
	Vec<4, T> ACos(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Arc tangent.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// Angle whose tangent is y / x.<br>
	/// The signs of x and y are used to determine what quadrant the angle is in.<br>
	/// The range of values returned by this function is [-PI, PI].<br>
	/// Note: Results are undefined if x and y are both 0!
	/// </returns>
	template<typename genType>
	genType ATan(genType y, genType x);

	/// <summary>
	/// Arc tangent.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// Angle whose tangent is y / x.<br>
	/// The signs of x and y are used to determine what quadrant the angle is in.<br>
	/// The range of values returned by this function is [-PI, PI].<br>
	/// Note: Results are undefined if x and y are both 0!
	/// </returns>
	template<typename T>
	Vec<2, T> ATan(const Vec<2, T>& a, const Vec<2, T>& b);
	/// <summary>
	/// Arc tangent.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// Angle whose tangent is y / x.<br>
	/// The signs of x and y are used to determine what quadrant the angle is in.<br>
	/// The range of values returned by this function is [-PI, PI].<br>
	/// Note: Results are undefined if x and y are both 0!
	/// </returns>
	template<typename T>
	Vec<3, T> ATan(const Vec<3, T>& a, const Vec<3, T>& b);
	/// <summary>
	/// Arc tangent.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>
	/// Angle whose tangent is y / x.<br>
	/// The signs of x and y are used to determine what quadrant the angle is in.<br>
	/// The range of values returned by this function is [-PI, PI].<br>
	/// Note: Results are undefined if x and y are both 0!
	/// </returns>
	template<typename T>
	Vec<4, T> ATan(const Vec<4, T>& a, const Vec<4, T>& b);

	/// <summary>
	/// Arc tangent.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Angle whose tangent is y over x. The range of values returned by this function is [-PI / 2, PI / 2].</returns>
	template<typename T>
	T ATan(T x);
	
	/// <summary>
	/// Arc tangent.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Angle whose tangent is y over x. The range of values returned by this function is [-PI / 2, PI / 2].</returns>
	template<typename T>
	Vec<2, T> ATan(const Vec<2, T>& v);
	/// <summary>
	/// Arc tangent.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Angle whose tangent is y over x. The range of values returned by this function is [-PI / 2, PI / 2].</returns>
	template<typename T>
	Vec<3, T> ATan(const Vec<3, T>& v);
	/// <summary>
	/// Arc tangent.
	/// </summary>
	/// <typeparam name="genType">Floating-point scalar types.</typeparam>
	/// <returns>Angle whose tangent is y over x. The range of values returned by this function is [-PI / 2, PI / 2].</returns>
	template<typename T>
	Vec<4, T> ATan(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic sine function, (Exp(x) - Exp(-x)) / 2.</returns>
	template<typename T>
	T SinH(T x);
	
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic sine function, (Exp(x) - Exp(-x)) / 2.</returns>
	template<typename T>
	Vec<2, T> SinH(const Vec<2, T>& v);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic sine function, (Exp(x) - Exp(-x)) / 2.</returns>
	template<typename T>
	Vec<3, T> SinH(const Vec<3, T>& v);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic sine function, (Exp(x) - Exp(-x)) / 2.</returns>
	template<typename T>
	Vec<4, T> SinH(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic cosine function, (Exp(x) + Exp(-x)) / 2.</returns>
	template<typename T>
	T CosH(T x);
	
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic cosine function, (Exp(x) + Exp(-x)) / 2.</returns>
	template<typename T>
	Vec<2, T> CosH(const Vec<2, T>& v);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic cosine function, (Exp(x) + Exp(-x)) / 2.</returns>
	template<typename T>
	Vec<3, T> CosH(const Vec<3, T>& v);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic cosine function, (Exp(x) + Exp(-x)) / 2.</returns>
	template<typename T>
	Vec<4, T> CosH(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic tangent function, SinH(angle) / CosH(angle).</returns>
	template<typename T>
	T TanH(T x);
	
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic tangent function, SinH(angle) / CosH(angle).</returns>
	template<typename T>
	Vec<2, T> TanH(const Vec<2, T>& v);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic tangent function, SinH(angle) / CosH(angle).</returns>
	template<typename T>
	Vec<3, T> TanH(const Vec<3, T>& v);
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Hyperbolic tangent function, SinH(angle) / CosH(angle).</returns>
	template<typename T>
	Vec<4, T> TanH(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Ary hyperbolic sine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Inverse of SinH.</returns>
	template<typename T>
	T ASinH(T x);
	
	/// <summary>
	/// Ary hyperbolic sine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Inverse of SinH.</returns>
	template<typename T>
	Vec<2, T> ASinH(const Vec<2, T>& v);
	/// <summary>
	/// Ary hyperbolic sine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Inverse of SinH.</returns>
	template<typename T>
	Vec<3, T> ASinH(const Vec<3, T>& v);
	/// <summary>
	/// Ary hyperbolic sine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Inverse of SinH.</returns>
	template<typename T>
	Vec<4, T> ASinH(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Arc hyperbolic cosine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Non-negative inverse of CosH.</returns>
	template<typename T>
	T ACosH(T x);
	
	/// <summary>
	/// Arc hyperbolic cosine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Non-negative inverse of CosH.</returns>
	template<typename T>
	Vec<2, T> ACosH(const Vec<2, T>& v);
	/// <summary>
	/// Arc hyperbolic cosine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Non-negative inverse of CosH.</returns>
	template<typename T>
	Vec<3, T> ACosH(const Vec<3, T>& v);
	/// <summary>
	/// Arc hyperbolic cosine.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Non-negative inverse of CosH.</returns>
	template<typename T>
	Vec<4, T> ACosH(const Vec<4, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Arc hyperbolic tangent.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Inverse of TanH. Note: Results are undefined if Abs(x) >= 1!</returns>
	template<typename T>
	T ATanH(T x);
	
	/// <summary>
	/// Arc hyperbolic tangent.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Inverse of TanH. Note: Results are undefined if Abs(x) >= 1!</returns>
	template<typename T>
	Vec<2, T> ATanH(const Vec<2, T>& v);
	/// <summary>
	/// Arc hyperbolic tangent.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Inverse of TanH. Note: Results are undefined if Abs(x) >= 1!</returns>
	template<typename T>
	Vec<3, T> ATanH(const Vec<3, T>& v);
	/// <summary>
	/// Arc hyperbolic tangent.
	/// </summary>
	/// <typeparam name="T">Floating-point scalar types.</typeparam>
	/// <returns>Inverse of TanH. Note: Results are undefined if Abs(x) >= 1!</returns>
	template<typename T>
	Vec<4, T> ATanH(const Vec<4, T>& v);
}

//-------------------------------------------------------------------------------------------------------------------//
//Common-------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
constexpr genType TRAP::Math::Min(genType x, genType y)
{
	static_assert(std::numeric_limits<genType>::is_iec559 || std::numeric_limits<genType>::is_integer, "'Min' only accepts floating-point or integer inputs");

	return (y < x) ? y : x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
constexpr genType TRAP::Math::Max(genType x, genType y)
{
	static_assert(std::numeric_limits<genType>::is_iec559 || std::numeric_limits<genType>::is_integer, "'Max' only accepts floating-point or integer inputs");

	return (x < y) ? y : x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
constexpr int TRAP::Math::Abs(const int x)
{
	const int y = x >> (sizeof(int) * 8 - 1);

	return (x ^ y) - y;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Round(genType x)
{
	return std::round(x);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Trunc(genType x)
{
	return std::trunc(x);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genFIType>
constexpr genFIType TRAP::Math::Abs(genFIType x)
{
	if constexpr (std::numeric_limits<genFIType>::is_signed)
	{
		static_assert(std::numeric_limits<genFIType>::is_iec559 || std::numeric_limits<genFIType>::is_signed, "'Abs' only accepts floating-point and integer scalar or vector inputs");

		return x >= genFIType(0) ? x : -x;
	}

	return {};
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Abs(const Vec<2, T>& x)
{
	return Vec<2, T>(std::abs(x.x), std::abs(x.y));
}
template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Abs(const Vec<3, T>& x)
{
	return Vec<3, T>(std::abs(x.x), std::abs(x.y), std::abs(x.z));
}
template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Abs(const Vec<4, T>& x)
{
	return Vec<4, T>(std::abs(x.x), std::abs(x.y), std::abs(x.z), std::abs(x.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genFIType>
genFIType TRAP::Math::Sign(genFIType x)
{
	static_assert(std::numeric_limits<genFIType>::is_iec559 || (std::numeric_limits<genFIType>::is_signed && std::numeric_limits<genFIType>::is_integer), "'Sign' only accepts signed inputs");

	return Vec<1, genFIType>(LessThan(Vec<1, genFIType>(0), x)) - Vec<1, genFIType>(LessThan(x, Vec<1, genFIType>(0)));
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Sign(const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559 || (std::numeric_limits<T>::is_signed && std::numeric_limits<T>::is_integer), "'Sign' only accepts signed inputs");

	return Vec<2, T>(LessThan(Vec<2, T>(0, 0), x)) - Vec<2, T>(LessThan(x, Vec<2, T>(0, 0)));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Sign(const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559 || (std::numeric_limits<T>::is_signed && std::numeric_limits<T>::is_integer), "'Sign' only accepts signed inputs");

	return Vec<3, T>(LessThan(Vec<3, T>(0, 0, 0), x)) - Vec<3, T>(LessThan(x, Vec<3, T>(0, 0, 0)));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Sign(const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559 || (std::numeric_limits<T>::is_signed && std::numeric_limits<T>::is_integer), "'Sign' only accepts signed inputs");

	return Vec<4, T>(LessThan(Vec<4, T>(0, 0, 0, 0), x)) - Vec<4, T>(LessThan(x, Vec<4, T>(0, 0, 0, 0)));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Floor(T x)
{
	return std::floor(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Floor(const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Floor' only accepts floating-point inputs.");

	return Vec<2, T>(std::floor(x.x), std::floor(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Floor(const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Floor' only accepts floating-point inputs.");

	return Vec<3, T>(std::floor(x.x), std::floor(x.y), std::floor(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Floor(const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Floor' only accepts floating-point inputs.");

	return Vec<4, T>(std::floor(x.x), std::floor(x.y), std::floor(x.z), std::floor(x.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Trunc(const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Trunc' only accepts floating-point inputs.");

	return Vec<2, T>(std::trunc(x.x), std::trunc(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Trunc(const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Trunc' only accepts floating-point inputs.");

	return Vec<3, T>(std::trunc(x.x), std::trunc(x.y), std::trunc(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Trunc(const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Trunc' only accepts floating-point inputs.");

	return Vec<4, T>(std::trunc(x.x), std::trunc(x.y), std::trunc(x.z), std::trunc(x.w));
}

//-------------------------------------------------------------------------------------------------------------------/

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Round(const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Round' only accepts floating-point inputs.");

	return Vec<2, T>(std::round(x.x), std::round(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Round(const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Round' only accepts floating-point inputs.");

	return Vec<3, T>(std::round(x.x), std::round(x.y), std::round(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Round(const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Round' only accepts floating-point inputs.");

	return Vec<4, T>(std::round(x.x), std::round(x.y), std::round(x.z), std::round(x.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::RoundEven(genType x)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'RoundEven' only accepts floating-point inputs");

	int integer = static_cast<int>(x);
	genType integerPart = static_cast<genType>(integer);
	genType fractionalPart = Fract(x);

	if (fractionalPart > static_cast<genType>(0.5) || fractionalPart < static_cast<genType>(0.5))
		return std::round(x);
	if ((integer % 2) == 0)
		return integerPart;
	if (x <= static_cast<genType>(0))
		return integerPart - static_cast<genType>(1);

	return integerPart + static_cast<genType>(1);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::RoundEven(const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'RoundEven' only accepts floating-point inputs");

	return Vec<2, T>(RoundEven(x.x), RoundEven(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::RoundEven(const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'RoundEven' only accepts floating-point inputs");

	return Vec<3, T>(RoundEven(x.x), RoundEven(x.y), RoundEven(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::RoundEven(const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'RoundEven' only accepts floating-point inputs");

	return Vec<4, T>(RoundEven(x.x), RoundEven(x.y), RoundEven(x.z), RoundEven(x.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Ceil(T x)
{
	return std::ceil(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Ceil(const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Ceil' only accepts floating-point inputs");

	return Vec<2, T>(std::ceil(x.x), std::ceil(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Ceil(const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Ceil' only accepts floating-point inputs");

	return Vec<3, T>(std::ceil(x.x), std::ceil(x.y), std::ceil(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Ceil(const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Ceil' only accepts floating-point inputs");

	return Vec<4, T>(std::ceil(x.x), std::ceil(x.y), std::ceil(x.z), std::ceil(x.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Fract(genType x)
{
	return x - Floor(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Fract(const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Fract' only accepts floating-point inputs");

	return x - Floor(x);
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Fract(const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Fract' only accepts floating-point inputs");

	return x - Floor(x);
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Fract(const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Fract' only accepts floating-point inputs");

	return x - Floor(x);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Mod(genType x, genType y)
{
	return x - y * Floor(x / y);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Mod(const Vec<2, T>& x, T y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Mod' only accepts floating-point inputs.");

	return x - Vec<2, T>(y) * Floor(x / Vec<2, T>(y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Mod(const Vec<3, T>& x, T y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Mod' only accepts floating-point inputs.");

	return x - Vec<3, T>(y) * Floor(x / Vec<3, T>(y));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Mod(const Vec<4, T>& x, T y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Mod' only accepts floating-point inputs.");

	return x - Vec<4, T>(y) * Floor(x / Vec<4, T>(y));
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Mod(const Vec<2, T>& x, const Vec<2, T>& y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Mod' only accepts floating-point inputs.");

	return x - y * Floor(x / y);
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Mod(const Vec<3, T>& x, const Vec<3, T>& y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Mod' only accepts floating-point inputs.");

	return x - y * Floor(x / y);
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Mod(const Vec<4, T>& x, const Vec<4, T>& y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Mod' only accepts floating-point inputs.");

	return x - y * Floor(x / y);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Modf(genType x, genType& i)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'Modf' only accepts floating-point inputs");

	return std::modf(x, &i);
}

template<typename T>
TRAP::Math::Vec<1, T> TRAP::Math::Modf(const Vec<1, T>& x, Vec<1, T>& i)
{
	return Vec<1, T>(std::modf(x.x, i.x));
}
template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Modf(const Vec<2, T>& x, Vec<2, T>& i)
{
	return Vec<2, T>(std::modf(x.x, i.x), std::modf(x.y, i.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Modf(const Vec<3, T>& x, Vec<3, T>& i)
{
	return Vec<3, T>(std::modf(x.x, i.x), std::modf(x.y, i.y), std::modf(x.z, i.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Modf(const Vec<4, T>& x, Vec<4, T>& i)
{
	return Vec<4, T>(std::modf(x.x, i.x), std::modf(x.y, i.y), std::modf(x.z, i.z), std::modf(x.w, i.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Min(const Vec<2, T>& a, T b)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Min' only accepts floating-point or integer inputs");

	return Vec<2, T>(std::min(a.x, b), std::min(a.y, b));
}
template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Min(const Vec<3, T>& a, T b)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Min' only accepts floating-point or integer inputs");

	return Vec<3, T>(std::min(a.x, b), std::min(a.y, b), std::min(a.z, b));
}
template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Min(const Vec<4, T>& a, T b)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Min' only accepts floating-point or integer inputs");

	return Vec<4, T>(std::min(a.x, b), std::min(a.y, b), std::min(a.z, b), std::min(a.w, b));
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Min(const Vec<2, T>& a, const Vec<2, T>& b)
{
	return Vec<2, T>(std::min(a.x, b.x), std::min(a.y, b.y));
}
template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Min(const Vec<3, T>& a, const Vec<3, T>& b)
{
	return Vec<3, T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}
template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Min(const Vec<4, T>& a, const Vec<4, T>& b)
{
	return Vec<4, T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Max(const Vec<2, T>& a, T b)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Max' only accepts floating-point or integer inputs");

	return Vec<2, T>(std::max(a.x, b), std::max(a.y, b));
}
template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Max(const Vec<3, T>& a, T b)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Max' only accepts floating-point or integer inputs");

	return Vec<3, T>(std::max(a.x, b), std::max(a.y, b), std::max(a.z, b));
}
template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Max(const Vec<4, T>& a, T b)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Max' only accepts floating-point or integer inputs");

	return Vec<4, T>(std::max(a.x, b), std::max(a.y, b), std::max(a.z, b), std::max(a.w, b));
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Max(const Vec<2, T>& a, const Vec<2, T>& b)
{
	return Vec<2, T>(std::max(a.x, b.x), std::max(a.y, b.y));
}
template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Max(const Vec<3, T>& a, const Vec<3, T>& b)
{
	return Vec<3, T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}
template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Max(const Vec<4, T>& a, const Vec<4, T>& b)
{
	return Vec<4, T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
constexpr genType TRAP::Math::Clamp(genType x, genType minVal, genType maxVal)
{
	static_assert(std::numeric_limits<genType>::is_iec559 || std::numeric_limits<genType>::is_integer, "'Clamp' only accepts floating-point or integer inputs");

	return Min(Max(x, minVal), maxVal);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Clamp(const Vec<2, T>& x, T minVal, T maxVal)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Clamp' only accepts floating-point or integer inputs");

	return Vec<2, T>(Min(Max(x, Vec<2, T>(minVal)), Vec<2, T>(maxVal)));
}
template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Clamp(const Vec<3, T>& x, T minVal, T maxVal)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Clamp' only accepts floating-point or integer inputs");

	return Vec<3, T>(Min(Max(x, Vec<3, T>(minVal)), Vec<3, T>(maxVal)));
}
template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Clamp(const Vec<4, T>& x, T minVal, T maxVal)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Clamp' only accepts floating-point or integer inputs");

	return Vec<4, T>(Min(Max(x, Vec<4, T>(minVal)), Vec<4, T>(maxVal)));
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Clamp(const Vec<2, T>& x, const Vec<2, T>& minVal, const Vec<2, T>& maxVal)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Clamp' only accepts floating-point or integer inputs");

	return Vec<2, T>(Min(Max(x, minVal), maxVal));
}
template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Clamp(const Vec<3, T>& x, const Vec<3, T>& minVal, const Vec<3, T>& maxVal)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Clamp' only accepts floating-point or integer inputs");

	return Vec<3, T>(Min(Max(x, minVal), maxVal));
}
template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Clamp(const Vec<4, T>& x, const Vec<4, T>& minVal, const Vec<4, T>& maxVal)
{
	static_assert(std::numeric_limits<T>::is_iec559 || std::numeric_limits<T>::is_integer, "'Clamp' only accepts floating-point or integer inputs");

	return Vec<4, T>(Min(Max(x, minVal), maxVal));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genTypeT, typename genTypeU>
genTypeT TRAP::Math::Mix(genTypeT x, genTypeT y, genTypeU a)
{
	static_assert(std::numeric_limits<genTypeU>::is_iec559, "'Mix' only accepts floating-point inputs for the interpolator a");

	return static_cast<genTypeT>(static_cast<genTypeU>(x)* (static_cast<genTypeU>(1) - a) + static_cast<genTypeU>(y)* a);
}

template<typename T, typename U>
TRAP::Math::Vec<2, T> TRAP::Math::Mix(const Vec<2, T>& x, const Vec<2, T>& y, U a)
{
	static_assert(std::numeric_limits<U>::is_iec559, "'Mix' only accepts floating-point inputs for the interpolator a");

	return Vec<2, T>(Vec<2, U>(x) * (static_cast<U>(1) - a) + Vec<2, U>(y) * a);
}
template<typename T, typename U>
TRAP::Math::Vec<3, T> TRAP::Math::Mix(const Vec<3, T>& x, const Vec<3, T>& y, U a)
{
	static_assert(std::numeric_limits<U>::is_iec559, "'Mix' only accepts floating-point inputs for the interpolator a");

	return Vec<3, T>(Vec<3, U>(x) * (static_cast<U>(1) - a) + Vec<3, U>(y) * a);
}
template<typename T, typename U>
TRAP::Math::Vec<4, T> TRAP::Math::Mix(const Vec<4, T>& x, const Vec<4, T>& y, U a)
{
	static_assert(std::numeric_limits<U>::is_iec559, "'Mix' only accepts floating-point inputs for the interpolator a");

	return Vec<4, T>(Vec<4, U>(x) * (static_cast<U>(1) - a) + Vec<4, U>(y) * a);
}

template<typename T, typename U>
TRAP::Math::Vec<2, T> TRAP::Math::Mix(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, U>& a)
{
	static_assert(std::numeric_limits<U>::is_iec559, "'Mix' only accepts floating-point inputs for the interpolator a");

	return Vec<2, T>(Vec<2, U>(x) * (static_cast<U>(1) - a) + Vec<2, U>(y) * a);
}
template<typename T, typename U>
TRAP::Math::Vec<3, T> TRAP::Math::Mix(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, U>& a)
{
	static_assert(std::numeric_limits<U>::is_iec559, "'Mix' only accepts floating-point inputs for the interpolator a");

	return Vec<3, T>(Vec<3, U>(x) * (static_cast<U>(1) - a) + Vec<3, U>(y) * a);
}
template<typename T, typename U>
TRAP::Math::Vec<4, T> TRAP::Math::Mix(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, U>& a)
{
	static_assert(std::numeric_limits<U>::is_iec559, "'Mix' only accepts floating-point inputs for the interpolator a");

	return Vec<4, T>(Vec<4, U>(x) * (static_cast<U>(1) - a) + Vec<4, U>(y) * a);
}

template<typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Mix(const tQuaternion<T>& x, const tQuaternion<T>& y, T a)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'mix' only accepts floating-point inputs");

	const T cosTheta = Dot(x, y);

	//Perform a linear interpolation when cosTheta is close to 1 to avoid sie effect of Sin(angle) becoming a zero denominator
	if(cosTheta > static_cast<T>(1) - Epsilon<T>())
	{
		//Linear interpolation
		return tQuaternion<T>(Mix(x.w, y.w, a), Mix(x.x, y.x, a), Mix(x.y, y.y, a), Mix(x.z, y.z, a));
	}
	
	T angle = ACos(cosTheta);
	return (Sin((static_cast<T>(1) - a)* angle)* x + Sin(a * angle) * y) / Sin(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Step(genType edge, genType x)
{
	return x < edge ? static_cast<genType>(0) : static_cast<genType>(1);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Step(T edge, const Vec<2, T>& x)
{
	return LessThan(x, Vec<2, T>(edge)) ? Vec<2, T>(0) : Vec<2, T>(1);
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Step(T edge, const Vec<3, T>& x)
{
	return LessThan(x, Vec<3, T>(edge)) ? Vec<3, T>(0) : Vec<3, T>(1);
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Step(T edge, const Vec<4, T>& x)
{
	return LessThan(x, Vec<4, T>(edge)) ? Vec<4, T>(0) : Vec<4, T>(1);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Step(const Vec<2, T>& edge, const Vec<2, T>& x)
{
	return LessThan(x, edge) ? Vec<2, T>(0) : Vec<2, T>(1);
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Step(const Vec<3, T>& edge, const Vec<3, T>& x)
{
	return LessThan(x, edge) ? Vec<3, T>(0) : Vec<3, T>(1);
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Step(const Vec<4, T>& edge, const Vec<4, T>& x)
{
	return LessThan(x, edge) ? Vec<4, T>(0) : Vec<4, T>(1);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::SmoothStep(genType edge0, genType edge1, genType x)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'SmoothStep' only accepts floating-point inputs");

	const genType tmp(Clamp((x - edge0) / (edge1 - edge0), genType(0), genType(1)));

	return tmp * tmp * (genType(3) - genType(2) * tmp);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::SmoothStep(T edge0, T edge1, const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'SmoothStep' only accepts floating-point inputs");

	const Vec<2, T> tmp(Clamp((x - Vec<2, T>(edge0)) / (Vec<2, T>(edge1) - Vec<2, T>(edge0)), static_cast<T>(0), static_cast<T>(1)));

	return tmp * tmp * (static_cast<T>(3) - static_cast<T>(2)* tmp);
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::SmoothStep(T edge0, T edge1, const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'SmoothStep' only accepts floating-point inputs");

	const Vec<3, T> tmp(Clamp((x - Vec<3, T>(edge0)) / (Vec<3, T>(edge1) - Vec<3, T>(edge0)), static_cast<T>(0), static_cast<T>(1)));

	return tmp * tmp * (static_cast<T>(3) - static_cast<T>(2)* tmp);
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::SmoothStep(T edge0, T edge1, const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'SmoothStep' only accepts floating-point inputs");

	const Vec<4, T> tmp(Clamp((x - Vec<4, T>(edge0)) / (Vec<4, T>(edge1) - Vec<4, T>(edge0)), static_cast<T>(0), static_cast<T>(1)));

	return tmp * tmp * (static_cast<T>(3) - static_cast<T>(2)* tmp);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::SmoothStep(const Vec<2, T>& edge0, const Vec<2, T>& edge1, const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'SmoothStep' only accepts floating-point inputs");

	const Vec<2, T> tmp(Clamp((x - edge0) / (edge1 - edge0), static_cast<T>(0), static_cast<T>(1)));

	return tmp * tmp * (static_cast<T>(3) - static_cast<T>(2)* tmp);
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::SmoothStep(const Vec<3, T>& edge0, const Vec<3, T>& edge1, const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'SmoothStep' only accepts floating-point inputs");

	const Vec<3, T> tmp(Clamp((x - edge0) / (edge1 - edge0), static_cast<T>(0), static_cast<T>(1)));

	return tmp * tmp * (static_cast<T>(3) - static_cast<T>(2)* tmp);
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::SmoothStep(const Vec<4, T>& edge0, const Vec<4, T>& edge1, const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'SmoothStep' only accepts floating-point inputs");

	const Vec<4, T> tmp(Clamp((x - edge0) / (edge1 - edge0), static_cast<T>(0), static_cast<T>(1)));

	return tmp * tmp * (static_cast<T>(3) - static_cast<T>(2)* tmp);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
bool TRAP::Math::IsNaN(genType x)
{
	return std::isnan(x);
}

template<typename T>
TRAP::Math::Vec<2, bool> TRAP::Math::IsNaN(const Vec<2, T>& v)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'IsNaN' only accepts floating-point inputs");

	Vec<2, bool> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::isnan(v[l]);

	return result;
}
template<typename T>
TRAP::Math::Vec<3, bool> TRAP::Math::IsNaN(const Vec<3, T>& v)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'IsNaN' only accepts floating-point inputs");

	Vec<3, bool> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::isnan(v[l]);

	return result;
}
template<typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::IsNaN(const Vec<4, T>& v)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'IsNaN' only accepts floating-point inputs");

	Vec<4, bool> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::isnan(v[l]);

	return result;
}

template <typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::IsNaN(const tQuaternion<T>& q)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'IsNan' only accepts floating-point inputs");

	return Vec<4, bool>(IsNan(q.x), IsNan(q.y), IsNan(q.z), IsNan(q.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
bool TRAP::Math::IsInf(genType x)
{
	return std::isinf(x);
}

template<typename T>
TRAP::Math::Vec<2, bool> TRAP::Math::IsInf(const Vec<2, T>& v)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'IsInf' only accepts floating-point inputs");

	Vec<2, bool> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::isinf(v[l]);

	return result;
}
template<typename T>
TRAP::Math::Vec<3, bool> TRAP::Math::IsInf(const Vec<3, T>& v)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'IsInf' only accepts floating-point inputs");

	Vec<3, bool> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::isinf(v[l]);

	return result;
}
template<typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::IsInf(const Vec<4, T>& v)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'IsInf' only accepts floating-point inputs");

	Vec<4, bool> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::isinf(v[l]);

	return result;
}

template <typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::IsInf(const tQuaternion<T>& q)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'isinf' only accepts floating-point inputs");

	return Vec<4, bool>(IsInf(q.x), IsInf(q.y), IsInf(q.z), IsInf(q.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::FMA(const genType& a, const genType& b, const genType& c)
{
	return std::fma(a, b, c);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::FrExp(genType x, int& exp)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'FrExp' only accepts floating-point inputs");

	return std::frexp(x, &exp);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::FrExp(const Vec<2, T>& v, const Vec<2, int>& exp)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'FrExp' only accepts floating-point inputs");

	Vec<2, T> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::frexp(v[l], &exp[l]);

	return result;
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::FrExp(const Vec<3, T>& v, const Vec<3, int>& exp)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'FrExp' only accepts floating-point inputs");

	Vec<3, T> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::frexp(v[l], &exp[l]);

	return result;
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::FrExp(const Vec<4, T>& v, const Vec<4, int>& exp)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'FrExp' only accepts floating-point inputs");

	Vec<4, T> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::frexp(v[l], &exp[l]);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::LdExp(const genType& x, const int& exp)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'LdExp' only accepts floating-point inputs");

	return std::ldexp(x, exp);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::LdExp(const Vec<2, T>& v, const Vec<2, int>& exp)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'LdExp' only accepts floating-point inputs");

	Vec<2, T> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::ldexp(v[l], exp[l]);

	return result;
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::LdExp(const Vec<3, T>& v, const Vec<3, int>& exp)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'LdExp' only accepts floating-point inputs");

	Vec<3, T> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::ldexp(v[l], exp[l]);

	return result;
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::LdExp(const Vec<4, T>& v, const Vec<4, int>& exp)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'LdExp' only accepts floating-point inputs");

	Vec<4, T> result;
	for (int l = 0; l < v.Length(); ++l)
		result[l] = std::ldexp(v[l], exp[l]);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
bool TRAP::Math::IsPowerOfTwo(genType value)
{
	static_assert(std::numeric_limits<genType>::is_integer, "'IsPowerOfTwo' only accepts integer inputs");

	const genType result = Abs(value);

	return !(result & (result - 1));
}

template<typename T>
TRAP::Math::Vec<2, bool> TRAP::Math::IsPowerOfTwo(const Vec<2, T>& v)
{
	static_assert(std::numeric_limits<T>::is_integer, "'IsPowerOfTwo' only accepts integer inputs");

	const Vec<2, T> result(Abs(v));

	return Equal(result & (result - Vec<2, T>(1)), Vec<2, T>(0));
}
template<typename T>
TRAP::Math::Vec<3, bool> TRAP::Math::IsPowerOfTwo(const Vec<3, T>& v)
{
	static_assert(std::numeric_limits<T>::is_integer, "'IsPowerOfTwo' only accepts integer inputs");

	const Vec<3, T> result(Abs(v));

	return Equal(result & (result - Vec<3, T>(1)), Vec<3, T>(0));
}
template<typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::IsPowerOfTwo(const Vec<4, T>& v)
{
	static_assert(std::numeric_limits<T>::is_integer, "'IsPowerOfTwo' only accepts integer inputs");

	const Vec<4, T> result(Abs(v));

	return Equal(result & (result - Vec<4, T>(1)), Vec<4, T>(0));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::FMod(genType x, genType y)
{
	return std::fmod(x, y);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::FMod(const Vec<2, T>& x, T y)
{
	return Vec<2, T>(std::fmod(x.x, y), std::fmod(x.y, y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::FMod(const Vec<3, T>& x, T y)
{
	return Vec<3, T>(std::fmod(x.x, y), std::fmod(x.y, y), std::fmod(x.z, y));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::FMod(const Vec<4, T>& x, T y)
{
	return Vec<4, T>(std::fmod(x.x, y), std::fmod(x.y, y), std::fmod(x.z, y), std::fmod(x.w, y));
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::FMod(const Vec<2, T>& x, const Vec<2, T>& y)
{
	return Vec<2, T>(std::fmod(x.x, y.y), std::fmod(x.y, y.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::FMod(const Vec<3, T>& x, const Vec<3, T>& y)
{
	return Vec<3, T>(std::fmod(x.x, y.y), std::fmod(x.y, y.y), std::fmod(x.z, y.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::FMod(const Vec<4, T>& x, const Vec<4, T>& y)
{
	return Vec<3, T>(std::fmod(x.x, y.y), std::fmod(x.y, y.y), std::fmod(x.z, y.z), std::fmod(x.w, y.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Lerp(T x, T y, T a)
{
	return Mix(x, y, a);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Lerp(const Vec<2, T>& x, const Vec<2, T>& y, T a)
{
	return Mix(x, y, a);
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Lerp(const Vec<3, T>& x, const Vec<3, T>& y, T a)
{
	return Mix(x, y, a);
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Lerp(const Vec<4, T>& x, const Vec<4, T>& y, T a)
{
	return Mix(x, y, a);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Lerp(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, T>& a)
{
	return Mix(x, y, a);
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Lerp(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, T>& a)
{
	return Mix(x, y, a);
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Lerp(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, T>& a)
{
	return Mix(x, y, a);
}

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Lerp(const tQuaternion<T>& x, const tQuaternion<T>& y, T a)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'lerp' only accepts floating-point inputs");

	//Lerp is only defined in [0, 1]
	assert(a >= static_cast<T>(0));
	assert(a <= static_cast<T>(1));

	return x * (static_cast<T>(1) - 1) + (y * a);
}

//-------------------------------------------------------------------------------------------------------------------//
//Exponential--------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Pow(T base, T exponent)
{
	return std::pow(base, exponent);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Pow(const Vec<2, T>& base, const Vec<2, T>& exponent)
{
	return Vec<2, T>(std::pow(base.x, exponent.x), std::pow(base.y, exponent.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Pow(const Vec<3, T>& base, const Vec<3, T>& exponent)
{
	return Vec<3, T>(std::pow(base.x, exponent.x), std::pow(base.y, exponent.y), std::pow(base.z, exponent.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Pow(const Vec<4, T>& base, const Vec<4, T>& exponent)
{
	return Vec<4, T>(std::pow(base.x, exponent.x), std::pow(base.y, exponent.y), std::pow(base.z, exponent.z), std::pow(base.w, exponent.w));
}

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Pow(const tQuaternion<T>& x, T y)
{
	//Raising to the power of 0 should yield 1
	//Needed to prevent a division by 0 error later on
	if (y > -Epsilon<T>() && y < Epsilon<T>())
		return tQuaternion<T>(1, 0, 0, 0);

	//To deal with non-unit quaternions
	T magnitude = Sqrt(x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w);

	T angle;
	if(Abs(x.w / magnitude) > CosOneOverTwo<T>())
	{
		//Scalar component is close to 1; using it to recover angle would lose precision
		//Instead, we use the non-scalar components since Sin() is accurate around 0
		
		//Prevent a division by 0 error later on
		T vectorMagnitude = x.x * x.x + x.y * x.y + x.z * x.z;
		//if(Abs(vectorMagnitude - static_cast<T>(0)) < Epsilon<T>())
		//Despite the compiler might say, we actually want to compare vectorMagnitude to 0.
		if(vectorMagnitude < std::numeric_limits<T>::min())
		{
			//Equivalent to raising a real number to a power
			return tQuaternion<T>(Pow(x.w, y), 0, 0, 0);
		}

		angle = ASin(Sqrt(vectorMagnitude) / magnitude);
	}
	else
	{
		//Scalar component is small, shouldn't cause loss of precision
		angle = ACos(x.w / magnitude);
	}

	T newAngle = angle * y;
	T div = Sin(newAngle) / Sin(angle);
	T mag = Pow(magnitude, y - static_cast<T>(1));
	return tQuaternion<T>(Cos(newAngle) * magnitude * mag, x.x * div * mag, x.y * div * mag, x.z * div * mag);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Exp(T x)
{
	return std::exp(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Exp(const Vec<2, T>& x)
{
	return Vec<2, T>(std::exp(x.x), std::exp(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Exp(const Vec<3, T>& x)
{
	return Vec<3, T>(std::exp(x.x), std::exp(x.y), std::exp(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Exp(const Vec<4, T>& x)
{
	return Vec<4, T>(std::exp(x.x), std::exp(x.y), std::exp(x.z), std::exp(x.w));
}

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Exp(const tQuaternion<T>& q)
{
	Vec<3, T> u(q.x, q.y, q.z);
	T const angle = Length(u);
	if (angle < Epsilon<T>())
		return tQuaternion<T>();

	const Vec<3, T> v(u / angle);
	return tQuaternion<T>(Cos(angle), Sin(angle) * v);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Log(T x)
{
	return std::log(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Log(const Vec<2, T>& x)
{
	return Vec<2, T>(std::log(x.x), std::log(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Log(const Vec<3, T>& x)
{
	return Vec<3, T>(std::log(x.x), std::log(x.y), std::log(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Log(const Vec<4, T>& x)
{
	return Vec<4, T>(std::log(x.x), std::log(x.y), std::log(x.z), std::log(x.w));
}

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Log(const tQuaternion<T>& q)
{
	Vec<3, T> u(q.x, q.y, q.z);
	T vec3Len = Length(u);

	if(vec3Len < Epsilon<T>())
	{
		if (q.w > static_cast<T>(0))
			return tQuaternion<T>(Log(q.w), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		if (q.w < static_cast<T>(0))
			return tQuaternion<T>(Log(-q.w), PI<T>(), static_cast<T>(0), static_cast<T>(0));
		return tQuaternion<T>(std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity());
	}
	
	T t = ATan(vec3Len, T(q.w)) / vec3Len;
	T quatLen2 = vec3Len * vec3Len + q.w * q.w;
	return tQuaternion<T>(static_cast<T>(0.5)* Log(quatLen2), t* q.x, t* q.y, t* q.z);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Exp2(genType x)
{
	return std::exp2(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Exp2(const Vec<2, T>& x)
{
	return Vec<2, T>(std::exp2(x.x), std::exp2(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Exp2(const Vec<3, T>& x)
{
	return Vec<3, T>(std::exp2(x.x), std::exp2(x.y), std::exp2(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Exp2(const Vec<4, T>& x)
{
	return Vec<4, T>(std::exp2(x.x), std::exp2(x.y), std::exp2(x.z), std::exp2(x.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Log2(genType x)
{
	return std::log2(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Log2(const Vec<2, T>& x)
{
	return Vec<2, T>(std::log2(x.x), std::log2(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Log2(const Vec<3, T>& x)
{
	return Vec<3, T>(std::log2(x.x), std::log2(x.y), std::log2(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Log2(const Vec<4, T>& x)
{
	return Vec<4, T>(std::log2(x.x), std::log2(x.y), std::log2(x.z), std::log2(x.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Sqrt(T x)
{
	return std::sqrt(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Sqrt(const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Sqrt' only accepts floating-point inputs");

	return Vec<2, T>(std::sqrt(x.x), std::sqrt(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Sqrt(const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Sqrt' only accepts floating-point inputs");

	return Vec<3, T>(std::sqrt(x.x), std::sqrt(x.y), std::sqrt(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Sqrt(const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Sqrt' only accepts floating-point inputs");

	return Vec<4, T>(std::sqrt(x.x), std::sqrt(x.y), std::sqrt(x.z), std::sqrt(x.w));
}

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Sqrt(const tQuaternion<T>& x)
{
	return Pow(x, static_cast<T>(0.5));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::InverseSqrt(genType x)
{
	return static_cast<genType>(1) / Sqrt(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::InverseSqrt(const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'InverseSqrt' only accepts floating-point inputs");
	
	return Vec<2, T>(static_cast<T>(1) / Sqrt(x.x), static_cast<T>(1) / Sqrt(x.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::InverseSqrt(const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'InverseSqrt' only accepts floating-point inputs");

	return Vec<3, T>(static_cast<T>(1) / Sqrt(x.x), static_cast<T>(1) / Sqrt(x.y), static_cast<T>(1) / Sqrt(x.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::InverseSqrt(const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'InverseSqrt' only accepts floating-point inputs");

	return Vec<4, T>(static_cast<T>(1) / Sqrt(x.x), static_cast<T>(1) / Sqrt(x.y), static_cast<T>(1) / Sqrt(x.z), static_cast<T>(1) / Sqrt(x.w));
}

//-------------------------------------------------------------------------------------------------------------------//
//Geometric----------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Length(genType x)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'Length' accepts only floating-point inputs");

	return Abs(x);
}

template<typename T>
T TRAP::Math::Length(const Vec<2, T>& v)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Length' accepts only floating-point inputs");

	return Sqrt(Dot(v, v));
}
template<typename T>
T TRAP::Math::Length(const Vec<3, T>& v)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Length' accepts only floating-point inputs");

	return Sqrt(Dot(v, v));
}
template<typename T>
T TRAP::Math::Length(const Vec<4, T>& v)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Length' accepts only floating-point inputs");

	return Sqrt(Dot(v, v));
}

template <typename T>
T TRAP::Math::Length(const tQuaternion<T>& q)
{
	return Sqrt(Dot(q, q));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Distance(const genType& p0, const genType& p1)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'Distance' accepts only floating-point inputs");

	return Length(p1 - p0);
}

template<typename T>
T TRAP::Math::Distance(const Vec<2, T>& p0, const Vec<2, T>& p1)
{
	return Length(p1 - p0);
}
template<typename T>
T TRAP::Math::Distance(const Vec<3, T>& p0, const Vec<3, T>& p1)
{
	return Length(p1 - p0);
}
template<typename T>
T TRAP::Math::Distance(const Vec<4, T>& p0, const Vec<4, T>& p1)
{
	return Length(p1 - p0);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Dot(T x, T y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Dot' accepts only floating-point inputs");
	
	return x * y;
}

template<typename T>
T TRAP::Math::Dot(const Vec<2, T>& x, const Vec<2, T>& y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Dot' accepts only floating-point inputs");

	Vec<2, T> tmp(x * y);

	return tmp.x + tmp.y;
}
template<typename T>
T TRAP::Math::Dot(const Vec<3, T>& x, const Vec<3, T>& y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Dot' accepts only floating-point inputs");

	Vec<3, T> tmp(x * y);

	return tmp.x + tmp.y + tmp.z;
}
template<typename T>
T TRAP::Math::Dot(const Vec<4, T>& x, const Vec<4, T>& y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Dot' accepts only floating-point inputs");

	Vec<4, T> tmp(x * y);

	return (tmp.x + tmp.y) + (tmp.z + tmp.w);
}

template <typename T>
T TRAP::Math::Dot(const tQuaternion<T>& x, const tQuaternion<T>& y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Dot' accepts only floating-point inputs");

	Vec<4, T> tmp(x.w * y.w, x.x * y.x, x.y * y.y, x.z * y.z);

	return (tmp.x + tmp.y) + (tmp.z + tmp.w);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Cross(const Vec<3, T>& x, const Vec<3, T>& y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Cross' accepts only floating-point inputs");

	return Vec<3, T>(x.y * y.z - y.y * x.z,
					 x.z * y.x - y.z * x.x,
					 x.x * y.y - y.x * x.y);
}

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Cross(const tQuaternion<T>& q1, const tQuaternion<T>& q2)
{
	return tQuaternion<T>(q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
						  q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
						  q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
						  q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Normalize(const Vec<2, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Normalize' accepts only floating-point inputs");

	return x * InverseSqrt(Dot(x, x));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Normalize(const Vec<3, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Normalize' accepts only floating-point inputs");

	return x * InverseSqrt(Dot(x, x));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Normalize(const Vec<4, T>& x)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Normalize' accepts only floating-point inputs");

	return x * InverseSqrt(Dot(x, x));
}

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Normalize(const tQuaternion<T>& q)
{
	T len = Length(q);
	if (len <= static_cast<T>(0)) //Problem
		return tQuaternion<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
	
	T oneOverLen = static_cast<T>(1) / len;
	return tQuaternion<T>(q.w * oneOverLen, q.x * oneOverLen, q.y * oneOverLen, q.z * oneOverLen);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::FaceForward(const genType& N, const genType& I, const genType& NRef)
{
	return Dot(NRef, I) < static_cast<genType>(0) ? N : -N;
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::FaceForward(const Vec<2, T>& N, const Vec<2, T>& I, const Vec<2, T>& NRef)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Normalize' accepts only floating-point inputs");

	return Dot(NRef, I) < static_cast<T>(0) ? N : -N;
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::FaceForward(const Vec<3, T>& N, const Vec<3, T>& I, const Vec<3, T>& NRef)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Normalize' accepts only floating-point inputs");

	return Dot(NRef, I) < static_cast<T>(0) ? N : -N;
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::FaceForward(const Vec<4, T>& N, const Vec<4, T>& I, const Vec<4, T>& NRef)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Normalize' accepts only floating-point inputs");

	return Dot(NRef, I) < static_cast<T>(0) ? N : -N;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Reflect(const genType& I, const genType& N)
{
	return I - N * Dot(N, I) * genType(2);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Reflect(const Vec<2, T>& I, const Vec<2, T>& N)
{
	return I - N * Dot(N, I) * static_cast<T>(2);
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Reflect(const Vec<3, T>& I, const Vec<3, T>& N)
{
	return I - N * Dot(N, I) * static_cast<T>(2);
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Reflect(const Vec<4, T>& I, const Vec<4, T>& N)
{
	return I - N * Dot(N, I) * static_cast<T>(2);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::Refract(const genType& I, const genType& N, genType eta)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'Refract' accepts only floating-point inputs");

	const genType dotValue(Dot(N, I));
	const genType k(static_cast<genType>(1) - eta * eta * (static_cast<genType>(1) - dotValue * dotValue));

	return (eta * I - (eta * dotValue + Sqrt(k)) * N) * static_cast<genType>(k >= static_cast<genType>(0));
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Refract(const Vec<2, T>& I, const Vec<2, T>& N, T eta)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Refract' accepts only floating-point inputs");

	const T dotValue(Dot(N, I));
	const T k(static_cast<T>(1) - eta * eta * (static_cast<T>(1) - dotValue * dotValue));
	const Vec<2, T> result = (k >= static_cast<T>(0)) ? (eta * I - (eta * dotValue + std::sqrt(k)) * N) : Vec<2, T>(0);

	return result;
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Refract(const Vec<3, T>& I, const Vec<3, T>& N, T eta)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Refract' accepts only floating-point inputs");

	const T dotValue(Dot(N, I));
	const T k(static_cast<T>(1) - eta * eta * (static_cast<T>(1) - dotValue * dotValue));
	const Vec<3, T> result = (k >= static_cast<T>(0)) ? (eta * I - (eta * dotValue + std::sqrt(k)) * N) : Vec<3, T>(0);

	return result;
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Refract(const Vec<4, T>& I, const Vec<4, T>& N, T eta)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Refract' accepts only floating-point inputs");

	const T dotValue(Dot(N, I));
	const T k(static_cast<T>(1) - eta * eta * (static_cast<T>(1) - dotValue * dotValue));
	const Vec<4, T> result = (k >= static_cast<T>(0)) ? (eta * I - (eta * dotValue + std::sqrt(k)) * N) : Vec<4, T>(0);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Equal(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y)
{
	return Equal(x, y, static_cast<T>(0));
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y)
{
	return Equal(x, y, static_cast<T>(0));
}

template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Equal(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, T epsilon)
{
	return Equal(x, y, Vec<3, T>(epsilon));
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, T epsilon)
{
	return Equal(x, y, Vec<4, T>(epsilon));
}

template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Equal(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, const Vec<3, T>& epsilon)
{
	Vec<3, bool> result(true);
	for (uint32_t i = 0; i < 3; ++i)
		result[i] = All(Equal(x[i], y[i], epsilon[i]));
	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, const Vec<4, T>& epsilon)
{
	Vec<4, bool> result(true);
	for (uint32_t i = 0; i < 4; ++i)
		result[i] = All(Equal(x[i], y[i], epsilon[i]));
	return result;
}

template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Equal(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, int ULPs)
{
	return Equal(x, y, Vec<3, int>(ULPs));
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, int ULPs)
{
	return Equal(x, y, Vec<4, int>(ULPs));
}

template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Equal(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, const Vec<3, int>& ULPs)
{
	Vec<3, bool> result(true);
	for (uint32_t i = 0; i < 3; ++i)
		result[i] = All(Equal(x[i], y[i], ULPs[i]));
	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, const Vec<3, int>& ULPs)
{
	Vec<4, bool> result(true);
	for (uint32_t i = 0; i < 4; ++i)
		result[i] = All(Equal(x[i], y[i], ULPs[i]));
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::NotEqual(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y)
{
	return NotEqual(x, y, static_cast<T>(0));
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y)
{
	return Equal(x, y, static_cast<T>(0));
}

template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::NotEqual(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, T epsilon)
{
	return Equal(x, y, Vec<3, T>(epsilon));
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, T epsilon)
{
	return Equal(x, y, Vec<4, T>(epsilon));
}

template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::NotEqual(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, const Vec<3, T>& epsilon)
{
	Vec<3, bool> result(true);
	for (uint32_t i = 0; i < 3; ++i)
		result[i] = All(NotEqual(x[i], y[i], epsilon[i]));
	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, const Vec<4, T>& epsilon)
{
	Vec<4, bool> result(true);
	for (uint32_t i = 0; i < 4; ++i)
		result[i] = All(NotEqual(x[i], y[i], epsilon[i]));
	return result;
}

template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::NotEqual(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, int ULPs)
{
	return NotEqual(x, y, Vec<3, int>(ULPs));
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, int ULPs)
{
	return NotEqual(x, y, Vec<4, int>(ULPs));
}

template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::NotEqual(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, const Vec<3, int>& ULPs)
{
	Vec<3, bool> result(true);
	for (uint32_t i = 0; i < 3; ++i)
		result[i] = All(NotEqual(x[i], y[i], ULPs[i]));
	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, const Vec<3, int>& ULPs)
{
	Vec<4, bool> result(true);
	for (uint32_t i = 0; i < 4; ++i)
		result[i] = All(NotEqual(x[i], y[i], ULPs[i]));
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Matrix-------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::MatrixCompMult(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'MatrixCompMult' only accepts floating-point inputs");

	Mat<3, 3, T> result;
	for (int i = 0; i < result.Length(); ++i)
		result[i] = x[i] * y[i];

	return result;
}
template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::MatrixCompMult(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'MatrixCompMult' only accepts floating-point inputs");

	Mat<4, 4, T> result;
	for (int i = 0; i < result.Length(); ++i)
		result[i] = x[i] * y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::OuterProduct(const Vec<3, T>& c, const Vec<3, T>& r)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'OuterProduct' only accepts floating-point inputs");

	Mat<3, 3, T> m;

	for (int i = 0; i < m.Length(); ++i)
		m[i] = c * r[i];

	return m;
}
template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::OuterProduct(const Vec<4, T>& c, const Vec<4, T>& r)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'OuterProduct' only accepts floating-point inputs");

	Mat<4, 4, T> m;

	for (int i = 0; i < m.Length(); ++i)
		m[i] = c * r[i];

	return m;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
typename TRAP::Math::Mat<3, 3, T>::transposeType TRAP::Math::Transpose(const Mat<3, 3, T>& m)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Transpose' only accepts floating-point inputs");

	Mat<3, 3, T> result;

	result[0][0] = m[0][0];
	result[0][1] = m[1][0];
	result[0][2] = m[2][0];

	result[1][0] = m[0][1];
	result[1][1] = m[1][1];
	result[1][2] = m[2][1];

	result[2][0] = m[0][2];
	result[2][1] = m[1][2];
	result[2][2] = m[2][2];

	return result;
}
template <typename T>
typename TRAP::Math::Mat<4, 4, T>::transposeType TRAP::Math::Transpose(const Mat<4, 4, T>& m)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Transpose' only accepts floating-point inputs");

	Mat<4, 4, T> result;

	result[0][0] = m[0][0];
	result[0][1] = m[1][0];
	result[0][2] = m[2][0];
	result[0][3] = m[3][0];

	result[1][0] = m[0][1];
	result[1][1] = m[1][1];
	result[1][2] = m[2][1];
	result[1][3] = m[3][1];

	result[2][0] = m[0][2];
	result[2][1] = m[1][2];
	result[2][2] = m[2][2];
	result[2][3] = m[3][2];

	result[3][0] = m[0][3];
	result[3][1] = m[1][3];
	result[3][2] = m[2][3];
	result[3][3] = m[3][3];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Determinant(const Mat<3, 3, T>& m)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Determinant' only accepts floating-point inputs");

	return + m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
		   - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
		   + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
}
template<typename T>
T TRAP::Math::Determinant(const Mat<4, 4, T>& m)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Determinant' only accepts floating-point inputs");
	
	T subFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	T subFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	T subFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	T subFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	T subFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	T subFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

	Vec<4, T> detCof(+(m[1][1] * subFactor00 - m[1][2] * subFactor01 + m[1][3] * subFactor02),
		             -(m[1][0] * subFactor00 - m[1][2] * subFactor03 + m[1][3] * subFactor04),
		             +(m[1][0] * subFactor01 - m[1][1] * subFactor03 + m[1][3] * subFactor05),
		             -(m[1][0] * subFactor02 - m[1][1] * subFactor04 + m[1][2] * subFactor05));

	return m[0][0] * detCof[0] + m[0][1] * detCof[1] +
		   m[0][2] * detCof[2] + m[0][3] * detCof[3];
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::Inverse(const Mat<3, 3, T>& m)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Inverse' only accepts floating-points inputs");

	T oneOverDeterminant = static_cast<T>(1) / (+m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
		- m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
		+ m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

	Mat<3, 3, T> inverse;
	inverse[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]) * oneOverDeterminant;
	inverse[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * oneOverDeterminant;
	inverse[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]) * oneOverDeterminant;
	inverse[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * oneOverDeterminant;
	inverse[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]) * oneOverDeterminant;
	inverse[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * oneOverDeterminant;
	inverse[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]) * oneOverDeterminant;
	inverse[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * oneOverDeterminant;
	inverse[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]) * oneOverDeterminant;

	return inverse;
}
template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Inverse(const Mat<4, 4, T>& m)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'Inverse' only accepts floating-points inputs");

	T coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	T coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
	T coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

	T coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	T coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
	T coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

	T coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	T coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
	T coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

	T coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	T coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
	T coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

	T coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	T coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
	T coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

	T coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	T coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
	T coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

	Vec<4, T> fac0(coef00, coef00, coef02, coef03);
	Vec<4, T> fac1(coef04, coef04, coef06, coef07);
	Vec<4, T> fac2(coef08, coef08, coef10, coef11);
	Vec<4, T> fac3(coef12, coef12, coef14, coef15);
	Vec<4, T> fac4(coef16, coef16, coef18, coef19);
	Vec<4, T> fac5(coef20, coef20, coef22, coef23);

	Vec<4, T> vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
	Vec<4, T> vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
	Vec<4, T> vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
	Vec<4, T> vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

	Vec<4, T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
	Vec<4, T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
	Vec<4, T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
	Vec<4, T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

	Vec<4, T> signA(+1, -1, +1, -1);
	Vec<4, T> signB(-1, +1, -1, +1);
	Mat<4, 4, T> inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

	Vec<4, T> row0(inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]);

	Vec<4, T> dot0(m[0] * row0);
	T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

	T oneOverDeterminant = static_cast<T>(1) / dot1;

	return inverse * oneOverDeterminant;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Orthographic(T left, T right, T bottom, T top)
{
	Mat<4, 4, T> result(static_cast<T>(1));

	result[0][0] = static_cast<T>(2) / (right - left);
	result[1][1] = static_cast<T>(2) / (top - bottom);
	result[2][2] = -static_cast<T>(1);
	result[3][0] = -(right + left) / (right - left);
	result[3][1] = -(top + bottom) / (top - bottom);

	return result;
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Orthographic(T left, T right, T bottom, T top, T zNear, T zFar)
{
	Mat<4, 4, T> result(static_cast<T>(1));

	result[0][0] = static_cast<T>(2) / (right - left);
	result[1][1] = static_cast<T>(2) / (top - bottom);
	result[2][2] = -static_cast<T>(2) / (zFar - zNear);
	result[3][0] = -(right + left) / (right - left);
	result[3][1] = -(top + bottom) / (top - bottom);
	result[3][2] = -(zFar + zNear) / (zFar - zNear);
	
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Frustum(T left, T right, T bottom, T top, T nearVal, T farVal)
{
	Mat<4, 4, T> result(static_cast<T>(0));

	result[0][0] = (static_cast<T>(2)* nearVal) / (right - left);
	result[1][1] = (static_cast<T>(2)* nearVal) / (top - bottom);
	result[2][0] = (right + left) / (right - left);
	result[2][1] = (top + bottom) / (top - bottom);
	result[2][2] = -(farVal + nearVal) / (farVal - nearVal);
	result[2][3] = static_cast<T>(-1);
	result[3][2] = -(static_cast<T>(2)* farVal* nearVal) / (farVal - nearVal);
	
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Perspective(T fovY, T aspect, T zNear, T zFar)
{
	assert(std::abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

	T const tanHalfFoVY = std::tan(fovY / static_cast<T>(2));

	Mat<4, 4, T> result(static_cast<T>(0));

	result[0][0] = static_cast<T>(1) / (aspect * tanHalfFoVY);
	result[1][1] = static_cast<T>(1) / (tanHalfFoVY);
	result[2][2] = -(zFar + zNear) / (zFar - zNear);
	result[2][3] = -static_cast<T>(1);
	result[3][2] = -(static_cast<T>(2)* zFar* zNear) / (zFar - zNear);
	
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::PerspectiveFoV(T fov, T width, T height, T zNear, T zFar)
{
	assert(width > static_cast<T>(0));
	assert(height > static_cast<T>(0));
	assert(fov > static_cast<T>(0));

	T const rad = fov;
	T const h = Cos(static_cast<T>(0.5)* rad) / Sin(static_cast<T>(0.5)* rad);
	T const w = h * height / width;

	Mat<4, 4, T> result(static_cast<T>(0));
	
	result[0][0] = w;
	result[1][1] = h;
	result[2][2] = -(zFar + zNear) / (zFar - zNear);
	result[2][3] = -static_cast<T>(1);
	result[3][2] = -(static_cast<T>(2)* zFar* zNear) / (zFar - zNear);
	
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::InfinitePerspective(T fovY, T aspect, T zNear)
{
	T const range = std::tan(fovY / static_cast<T>(2))* zNear;
	T const left = -range * aspect;
	T const right = range * aspect;
	T const bottom = -range;
	T const top = range;

	Mat<4, 4, T> result(static_cast<T>(0));
	
	result[0][0] = (static_cast<T>(2)* zNear) / (right - left);
	result[1][1] = (static_cast<T>(2)* zNear) / (top - bottom);
	result[2][2] = -static_cast<T>(1);
	result[2][3] = -static_cast<T>(1);
	result[3][2] = -static_cast<T>(2)* zNear;
	
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T, typename U>
TRAP::Math::Mat<3, 3, T> TRAP::Math::Mix(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, U a)
{
	return Mat<3, 3, U>(x) * (static_cast<U>(1) - a) + Mat<3, 3, U>(y) * a;
}
template<typename T, typename U>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Mix(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, U a)
{
	return Mat<4, 4, U>(x) * (static_cast<U>(1) - a) + Mat<4, 4, U>(y) * a;
}

template<typename T, typename U>
TRAP::Math::Mat<3, 3, T> TRAP::Math::Mix(const Mat<3, 3, T>& x, const Mat<3, 3, T>& y, const Mat<3, 3, U>& a)
{
	return MatrixCompMult(Mat<3, 3, U>(x), static_cast<U>(1) - a) + MatrixCompMult(Mat<3, 3, U>(y), a);
}
template<typename T, typename U>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Mix(const Mat<4, 4, T>& x, const Mat<4, 4, T>& y, const Mat<4, 4, U>& a)
{
	return MatrixCompMult(Mat<4, 4, U>(x), static_cast<U>(1) - a) + MatrixCompMult(Mat<4, 4, U>(y), a);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Translate(const Mat<4, 4, T>& m, const Vec<3, T>& v)
{
	Mat<4, 4, T> result(m);

	result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	
	return result;
}

template <typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Translate(const Vec<3, T>& v)
{
	Mat<4, 4, T> result(T(1.0));

	result[3] = result[0] * v[0] + result[1] * v[1] + result[2] * v[2] + result[3];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Rotate(const Mat<4, 4, T>& m, T angleInRadians, const Vec<3, T>& v)
{
	const T a = angleInRadians;
	const T c = std::cos(a);
	const T s = std::sin(a);

	Vec<3, T> axis(Normalize(v));
	Vec<3, T> temp((T(1) - c) * axis);

	Mat<4, 4, T> rotate;
	
	rotate[0][0] = c + temp[0] * axis[0];
	rotate[0][1] = temp[0] * axis[1] + s * axis[2];
	rotate[0][2] = temp[0] * axis[2] - s * axis[1];

	rotate[1][0] = temp[1] * axis[0] - s * axis[2];
	rotate[1][1] = c + temp[1] * axis[1];
	rotate[1][2] = temp[1] * axis[2] + s * axis[0];

	rotate[2][0] = temp[2] * axis[0] + s * axis[1];
	rotate[2][1] = temp[2] * axis[1] - s * axis[0];
	rotate[2][2] = c + temp[2] * axis[2];

	Mat<4, 4, T> result;
	
	result[0] = m[0] * rotate[0][0] + m[1] * rotate[0][1] + m[2] * rotate[0][2];
	result[1] = m[0] * rotate[1][0] + m[1] * rotate[1][1] + m[2] * rotate[1][2];
	result[2] = m[0] * rotate[2][0] + m[1] * rotate[2][1] + m[2] * rotate[2][2];
	result[3] = m[3];
	
	return result;
}

template<typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Rotate(T angleInRadians, const Vec<3, T>& v)
{
	const T a = angleInRadians;
	const T c = std::cos(a);
	const T s = std::sin(a);

	Vec<3, T> axis(Normalize(v));
	Vec<3, T> temp((T(1) - c) * axis);

	Mat<4, 4, T> rotate;

	rotate[0][0] = c + temp[0] * axis[0];
	rotate[0][1] = temp[0] * axis[1] + s * axis[2];
	rotate[0][2] = temp[0] * axis[2] - s * axis[1];

	rotate[1][0] = temp[1] * axis[0] - s * axis[2];
	rotate[1][1] = c + temp[1] * axis[1];
	rotate[1][2] = temp[1] * axis[2] + s * axis[0];

	rotate[2][0] = temp[2] * axis[0] + s * axis[1];
	rotate[2][1] = temp[2] * axis[1] - s * axis[0];
	rotate[2][2] = c + temp[2] * axis[2];

	Mat<4, 4, T> identity(1.0f);
	Mat<4, 4, T> result;

	result[0] = identity[0] * rotate[0][0] + identity[1] * rotate[0][1] + identity[2] * rotate[0][2];
	result[1] = identity[0] * rotate[1][0] + identity[1] * rotate[1][1] + identity[2] * rotate[1][2];
	result[2] = identity[0] * rotate[2][0] + identity[1] * rotate[2][1] + identity[2] * rotate[2][2];
	result[3] = identity[3];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Scale(const Mat<4, 4, T>& m, const Vec<3, T>& v)
{
	Mat<4, 4, T> result;
	
	result[0] = m[0] * v[0];
	result[1] = m[1] * v[1];
	result[2] = m[2] * v[2];
	result[3] = m[3];
	
	return result;
}

template <typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Scale(const Vec<3, T>& v)
{
	Mat<4, 4, T> result(1.0f);

	result[0] *= v[0];
	result[1] *= v[1];
	result[2] *= v[2];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::LookAt(const Vec<3, T>& eye, const Vec<3, T>& center, const Vec<3, T>& up)
{
	const Vec<3, T> f(Normalize(center - eye));
	const Vec<3, T> s(Normalize(Cross(f, up)));
	const Vec<3, T> u(Cross(s, f));

	Mat<4, 4, T> result(1);
	
	result[0][0] = s.x;
	result[1][0] = s.y;
	result[2][0] = s.z;
	result[0][1] = u.x;
	result[1][1] = u.y;
	result[2][1] = u.z;
	result[0][2] = -f.x;
	result[1][2] = -f.y;
	result[2][2] = -f.z;
	result[3][0] = -Dot(s, eye);
	result[3][1] = -Dot(u, eye);
	result[3][2] = Dot(f, eye);
	
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Quaternion---------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::SLerp(const tQuaternion<T>& x, const tQuaternion<T>& y, T a)
{
	static_assert(std::numeric_limits<T>::is_iec559, "'slerp' only accepts floating-point inputs");

	tQuaternion<T> z = y;

	T cosTheta = Dot(x, y);

	//If cosTheta < 0, the interpolation will take the long way around the sphere.
	//To fix this, one quat must be negated.
	if (cosTheta < static_cast<T>(0))
	{
		z = -y;
		cosTheta = -cosTheta;
	}

	//Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of Sin(angle) becoming a zero denominator
	if (cosTheta > static_cast<T>(1) - Epsilon<T>())
	{
		//Linear interpolation
		return tQuaternion<T>(Mix(x.w, z.w, a), Mix(x.w, z.x, a), Mix(x.y, z.y, a), Mix(x.y, z.y, a), Mix(x.z, z.z, a));
	}

	T angle = ACos(cosTheta);
	return (Sin((static_cast<T>(1) - a) * angle) * x + Sin(a * angle) * z) / Sin(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Conjugate(const tQuaternion<T>& q)
{
	return tQuaternion<T>(q.w, -q.x, -q.y, -q.z);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Inverse(const tQuaternion<T>& q)
{
	return Conjugate(q) / Dot(q, q);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Vec<3, T> TRAP::Math::EulerAngles(const tQuaternion<T>& x)
{
	return Vec<3, T>(Pitch(x), Yaw(x), Roll(x));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
T TRAP::Math::Roll(const tQuaternion<T>& q)
{
	const T y = static_cast<T>(2) * (q.x * q.y + q.w * q.z);
	const T x = q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z;

	if (All(Equal(tVec2<T>(x, y), tVec2<T>(0), Epsilon<T>())))
		return static_cast<T>(0);

	return static_cast<T>(ATan(y, x));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
T TRAP::Math::Pitch(const tQuaternion<T>& q)
{
	const T y = static_cast<T>(2) * (q.y * q.z + q.w * q.x);
	const T x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

	if (All(Equal(Vec<2, T>(x, y), Vec<2, T>(0), Epsilon<T>())))
		return static_cast<T>(static_cast<T>(2)* ATan(q.x, q.w));

	return static_cast<T>(ATan(y, x));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
T TRAP::Math::Yaw(const tQuaternion<T>& x)
{
	return ASin(Clamp(static_cast<T>(-2)* (x.x * x.z - x.w * x.y), static_cast<T>(-1), static_cast<T>(1)));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Mat<3, 3, T> TRAP::Math::Mat3Cast(const tQuaternion<T>& q)
{
	Mat<3, 3, T> result(T(1));
	T qxx(q.x * q.x);
	T qyy(q.y * q.y);
	T qzz(q.z * q.z);
	T qxz(q.x * q.z);
	T qxy(q.x * q.y);
	T qyz(q.y * q.z);
	T qwx(q.w * q.x);
	T qwy(q.w * q.y);
	T qwz(q.w * q.z);

	result[0][0] = T(1) - T(2) * (qyy + qzz);
	result[0][1] = T(2) * (qxy + qwz);
	result[0][2] = T(2) * (qxz - qwy);

	result[1][0] = T(2) * (qxy - qwz);
	result[1][1] = T(1) - T(2) * (qxx + qzz);
	result[1][2] = T(2) * (qyz + qwx);

	result[2][0] = T(2) * (qxz + qwy);
	result[2][1] = T(2) * (qyz - qwx);
	result[2][2] = T(1) - T(2) * (qxx + qyy);

	return result;
}

template <typename T>
TRAP::Math::Mat<4, 4, T> TRAP::Math::Mat4Cast(const tQuaternion<T>& q)
{
	return Mat<4, 4, T>(Mat3Cast(q));
}

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::QuaternionCast(const Mat<3, 3, T>& m)
{
	T fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
	T fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
	T fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
	T fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

	int32_t biggestIndex = 0;
	T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	T biggestVal = Sqrt(fourBiggestSquaredMinus1 + static_cast<T>(1))* static_cast<T>(0.5);
	T mult = static_cast<T>(0.25) / biggestVal;

	switch (biggestIndex)
	{
	case 0:
		return tQuaternion<T>(biggestVal, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult);

	case 1:
		return tQuaternion<T>((m[1][2] - m[2][1]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult);

	case 2:
		return tQuaternion<T>((m[2][0] - m[0][2]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult);

	case 3:
		return tQuaternion<T>((m[0][1] - m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal);

	default:
		assert(false);
		return tQuaternion<T>(1, 0, 0, 0);
	}
}

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::QuaternionCast(const Mat<4, 4, T>& m)
{
	return QuaternionCast(Mat<3, 3, T>(m));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::LessThan(const tQuaternion<T>& x, const tQuaternion<T>& y)
{
	Vec<4, bool> result;
	for (int i = 0; i < x.Length(); ++i)
		result[i] = x[i] < y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::LessThanEqual(const tQuaternion<T>& x, const tQuaternion<T>& y)
{
	Vec<4, bool> result;
	for (int i = 0; i < x.Length(); ++i)
		result[i] = x[i] <= y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::GreaterThan(const tQuaternion<T>& x, const tQuaternion<T>& y)
{
	Vec<4, bool> result;
	for (int i = 0; i < x.Length(); ++i)
		result[i] = x[i] > y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::GreaterThanEqual(const tQuaternion<T>& x, const tQuaternion<T>& y)
{
	Vec<4, bool> result;
	for (int i = 0; i < x.Length(); ++i)
		result[i] = x[i] >= y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::QuaternionLookAt(const Vec<3, T>& direction, const Vec<3, T>& up)
{
	Mat<3, 3, T> result;

	result[2] = direction;
	result[0] = Normalize(Cross(up, result[2]));
	result[1] = Cross(result[2], result[0]);

	return QuaternionCast(result);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const tQuaternion<T>& x, const tQuaternion<T>& y)
{
	Vec<4, bool> result;
	for (int i = 0; i < x.Length(); ++i)
		result[i] = x[i] == y[i];

	return result;
}

template <typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const tQuaternion<T>& x, const tQuaternion<T>& y, T epsilon)
{
	Vec<4, T> v(x.x - y.x, x.y - y.y, x.z - y.z, x.w - y.w);
	return LessThan(Abs(v), Vec<4, T>(epsilon));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const tQuaternion<T>& x, const tQuaternion<T>& y)
{
	Vec<4, bool> result;
	for (int i = 0; i < x.Length(); ++i)
		result[i] = x[i] != y[i];

	return result;
}

template <typename T>
TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const tQuaternion<T>& x, const tQuaternion<T>& y, T epsilon)
{
	Vec<4, T> v(x.x - y.x, x.y - y.y, x.z - y.z, x.w - y.w);
	return GreaterThanEqual(Abs(v), Vec<4, T>(epsilon));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
T TRAP::Math::Angle(const tQuaternion<T>& x)
{
	if (Abs(x.w) > CosOneOverTwo<T>())
		return ASin(Sqrt(x.x * x.x + x.y * x.y + x.z * x.z)) * static_cast<T>(2);

	return ACos(x.w) * static_cast<T>(2);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Axis(const tQuaternion<T>& x)
{
	const T tmp1 = static_cast<T>(1) - x.w * x.w;
	
	if (tmp1 <= static_cast<T>(0))
		return Vec<3, T>(0, 0, 1);
	
	const T tmp2 = static_cast<T>(1) / Sqrt(tmp1);
	return Vec<3, T>(x.x * tmp2, x.y * tmp2, x.z * tmp2);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::AngleAxis(const T& angle, const Vec<3, T>& v)
{
	const T a(angle);
	const T s = Sin(a * static_cast<T>(0.5));

	return tQuaternion<T>(Cos(a * static_cast<T>(0.5)), v * s);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
TRAP::Math::tQuaternion<T> TRAP::Math::Rotate(const tQuaternion<T>& q, const T& angle, const Vec<3, T>& v)
{
	Vec<3, T> tmp = v;

	//Axis of rotation must be normalized
	T len = Length(tmp);
	if(Abs(len - static_cast<T>(1)) > static_cast<T>(0.001))
	{
		T oneOverLen = static_cast<T>(1) / len;
		tmp.x *= oneOverLen;
		tmp.y *= oneOverLen;
		tmp.z *= oneOverLen;
	}

	const T angleRad(angle);
	const T sin = Sin(angleRad * static_cast<T>(0.5));

	return q * tQuaternion<T>(Cos(angleRad * static_cast<T>(0.5)), tmp.x * sin, tmp.y * sin, tmp.z * sin);
}

//-------------------------------------------------------------------------------------------------------------------//
//Vector-------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::XAxis()
{
	return Vec<3, T>(T(1.0), T(0.0), T(0.0));
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::YAxis()
{
	return Vec<3, T>(T(0.0), T(1.0), T(0.0));
}

template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::ZAxis()
{
	return Vec<3, T>(T(0.0), T(0.0), T(1.0));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::LessThan(const Vec<2, T>& x, const Vec<2, T>& y)
{
	Vec<2, bool> result(true);

	for (int i = 0; i < 2; ++i)
		result[i] = x[i] < y[i];

	return result;
}
template<typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::LessThan(const Vec<3, T>& x, const Vec<3, T>& y)
{
	Vec<3, bool> result(true);

	for (int i = 0; i < 3; ++i)
		result[i] = x[i] < y[i];

	return result;
}
template<typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::LessThan(const Vec<4, T>& x, const Vec<4, T>& y)
{
	Vec<4, bool> result(true);

	for (int i = 0; i < 4; ++i)
		result[i] = x[i] < y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::LessThanEqual(const Vec<2, T>& x, const Vec<2, T>& y)
{
	Vec<2, bool> result(true);

	for (int i = 0; i < 2; ++i)
		result[i] = x[i] <= y[i];

	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::LessThanEqual(const Vec<3, T>& x, const Vec<3, T>& y)
{
	Vec<3, bool> result(true);

	for (int i = 0; i < 3; ++i)
		result[i] = x[i] <= y[i];

	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::LessThanEqual(const Vec<4, T>& x, const Vec<4, T>& y)
{
	Vec<4, bool> result(true);

	for (int i = 0; i < 4; ++i)
		result[i] = x[i] <= y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::GreaterThan(const Vec<2, T>& x, const Vec<2, T>& y)
{
	Vec<2, bool> result(true);

	for (int i = 0; i < 2; ++i)
		result[i] = x[i] > y[i];

	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::GreaterThan(const Vec<3, T>& x, const Vec<3, T>& y)
{
	Vec<3, bool> result(true);

	for (int i = 0; i < 3; ++i)
		result[i] = x[i] > y[i];

	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::GreaterThan(const Vec<4, T>& x, const Vec<4, T>& y)
{
	Vec<4, bool> result(true);

	for (int i = 0; i < 4; ++i)
		result[i] = x[i] > y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::GreaterThanEqual(const Vec<2, T>& x, const Vec<2, T>& y)
{
	Vec<2, bool> result(true);

	for (int i = 0; i < 2; ++i)
		result[i] = x[i] >= y[i];

	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::GreaterThanEqual(const Vec<3, T>& x, const Vec<3, T>& y)
{
	Vec<3, bool> result(true);

	for (int i = 0; i < 3; ++i)
		result[i] = x[i] >= y[i];

	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::GreaterThanEqual(const Vec<4, T>& x, const Vec<4, T>& y)
{
	Vec<4, bool> result(true);

	for (int i = 0; i < 4; ++i)
		result[i] = x[i] >= y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::Equal(const Vec<2, T>& x, const Vec<2, T>& y)
{
	Vec<2, bool> result(true);

	for (int i = 0; i < 2; ++i)
		result[i] = x[i] == y[i];

	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Equal(const Vec<3, T>& x, const Vec<3, T>& y)
{
	Vec<3, bool> result(true);

	for (int i = 0; i < 3; ++i)
		result[i] = x[i] == y[i];

	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const Vec<4, T>& x, const Vec<4, T>& y)
{
	Vec<4, bool> result(true);

	for (int i = 0; i < 4; ++i)
		result[i] = x[i] == y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::Equal(const Vec<2, T>& x, const Vec<2, T>& y, T epsilon)
{
	return Equal(x, y, Vec<2, T>(epsilon));
}
template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Equal(const Vec<3, T>& x, const Vec<3, T>& y, T epsilon)
{
	return Equal(x, y, Vec<3, T>(epsilon));
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const Vec<4, T>& x, const Vec<4, T>& y, T epsilon)
{
	return Equal(x, y, Vec<4, T>(epsilon));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::Equal(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, T>& epsilon)
{
	return LessThanEqual(Abs(x - y), epsilon);
}
template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Equal(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, T>& epsilon)
{
	return LessThanEqual(Abs(x - y), epsilon);
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, T>& epsilon)
{
	return LessThanEqual(Abs(x - y), epsilon);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::Equal(const Vec<2, T>& x, const Vec<2, T>& y, int ULPs)
{
	return Equal(x, y, Vec<2, int>(ULPs));
}
template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Equal(const Vec<3, T>& x, const Vec<3, T>& y, int ULPs)
{
	return Equal(x, y, Vec<3, int>(ULPs));
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const Vec<4, T>& x, const Vec<4, T>& y, int ULPs)
{
	return Equal(x, y, Vec<4, int>(ULPs));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::Equal(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, int>& ULPs)
{
	Vec<2, bool> result(false);
	for(uint32_t i = 0; i < 2; i++)
	{
		const float a(x[i]);
		const float b(y[i]);

		//Different signs means they do not match
		if((a < 0) != (b < 0))
		{
			//Check for equality to make sure +0==-0
			//a mantissa == b mantissa && a exponent == b exponent
			result[i] = (static_cast<int>(a) & ((1 << 23) - 1)) == (static_cast<int>(b) & ((1 << 23) - 1)) && ((static_cast<int>(a) >> 23) & ((1 << 8) - 1)) == ((static_cast<int>(b) >> 23) & ((1 << 8) - 1));
		}
		else
		{
			//Find the difference in ULPs.
			const int diffULPs = Abs(static_cast<int>(a) - static_cast<int>(b));
			result[i] = diffULPs <= ULPs[i];
		}
	}

	return result;
}
template<typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Equal(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, int>& ULPs)
{
	Vec<3, bool> result(false);
	for (uint32_t i = 0; i < 3; i++)
	{
		const float a(x[i]);
		const float b(y[i]);

		//Different signs means they do not match
		if ((a < 0) != (b < 0))
		{
			//Check for equality to make sure +0==-0
			//a mantissa == b mantissa && a exponent == b exponent
			result[i] = (static_cast<int>(a) & ((1 << 23) - 1)) == (static_cast<int>(b) & ((1 << 23) - 1)) && ((static_cast<int>(a) >> 23) & ((1 << 8) - 1)) == ((static_cast<int>(b) >> 23) & ((1 << 8) - 1));
		}
		else
		{
			//Find the difference in ULPs.
			const int diffULPs = Abs(static_cast<int>(a) - static_cast<int>(b));
			result[i] = diffULPs <= ULPs[i];
		}
	}

	return result;
}
template<typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, int>& ULPs)
{
	Vec<4, bool> result(false);
	for (uint32_t i = 0; i < 4; i++)
	{
		const float a(x[i]);
		const float b(y[i]);

		//Different signs means they do not match
		if ((a < 0) != (b < 0))
		{
			//Check for equality to make sure +0==-0
			//a mantissa == b mantissa && a exponent == b exponent
			result[i] = (static_cast<int>(a) & ((1 << 23) - 1)) == (static_cast<int>(b) & ((1 << 23) - 1)) && ((static_cast<int>(a) >> 23) & ((1 << 8) - 1)) == ((static_cast<int>(b) >> 23) & ((1 << 8) - 1));
		}
		else
		{
			//Find the difference in ULPs.
			const int diffULPs = Abs(static_cast<int>(a) - static_cast<int>(b));
			result[i] = diffULPs <= ULPs[i];
		}
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::NotEqual(const Vec<2, T>& x, const Vec<2, T>& y)
{
	Vec<2, bool> result(true);

	for (int i = 0; i < 2; ++i)
		result[i] = x[i] != y[i];

	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::NotEqual(const Vec<3, T>& x, const Vec<3, T>& y)
{
	Vec<3, bool> result(true);

	for (int i = 0; i < 3; ++i)
		result[i] = x[i] != y[i];

	return result;
}
template <typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const Vec<4, T>& x, const Vec<4, T>& y)
{
	Vec<4, bool> result(true);

	for (int i = 0; i < 4; ++i)
		result[i] = x[i] != y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::NotEqual(const Vec<2, T>& x, const Vec<2, T>& y, T epsilon)
{
	return NotEqual(x, y, Vec<2, T>(epsilon));
}
template<typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::NotEqual(const Vec<3, T>& x, const Vec<3, T>& y, T epsilon)
{
	return NotEqual(x, y, Vec<3, T>(epsilon));
}
template<typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const Vec<4, T>& x, const Vec<4, T>& y, T epsilon)
{
	return NotEqual(x, y, Vec<4, T>(epsilon));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::NotEqual(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, T>& epsilon)
{
	return GreaterThan(Abs(x - y), epsilon);
}
template<typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::NotEqual(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, T>& epsilon)
{
	return GreaterThan(Abs(x - y), epsilon);
}
template<typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, T>& epsilon)
{
	return GreaterThan(Abs(x - y), epsilon);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::NotEqual(const Vec<2, T>& x, const Vec<2, T>& y, int ULPs)
{
	return NotEqual(x, y, Vec<2, int>(ULPs));
}
template<typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::NotEqual(const Vec<3, T>& x, const Vec<3, T>& y, int ULPs)
{
	return NotEqual(x, y, Vec<2, int>(ULPs));
}
template<typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const Vec<4, T>& x, const Vec<4, T>& y, int ULPs)
{
	return NotEqual(x, y, Vec<2, int>(ULPs));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr TRAP::Math::Vec<2, bool> TRAP::Math::NotEqual(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, int>& ULPs)
{
	return Not(Equal(x, y, ULPs));
}
template<typename T>
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::NotEqual(const Vec<3, T>& x, const Vec<3, T>& y, const Vec<3, int>& ULPs)
{
	return Not(Equal(x, y, ULPs));
}
template<typename T>
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const Vec<4, T>& x, const Vec<4, T>& y, const Vec<4, int>& ULPs)
{
	return Not(Equal(x, y, ULPs));
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::Math::Any(const Vec<2, bool>& v)
{
	bool result = false;

	for (int i = 0; i < 2; ++i)
		result = result || v[i];

	return result;
}
constexpr bool TRAP::Math::Any(const Vec<3, bool>& v)
{
	bool result = false;

	for (int i = 0; i < 3; ++i)
		result = result || v[i];

	return result;
}constexpr bool TRAP::Math::Any(const Vec<4, bool>& v)
{
	bool result = false;

	for (int i = 0; i < 4; ++i)
		result = result || v[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::Math::All(const Vec<2, bool>& v)
{
	bool result = true;

	for (int i = 0; i < 2; ++i)
		result = result && v[i];

	return result;
}
constexpr bool TRAP::Math::All(const Vec<3, bool>& v)
{
	bool result = true;

	for (int i = 0; i < 3; ++i)
		result = result && v[i];

	return result;
}
constexpr bool TRAP::Math::All(const Vec<4, bool>& v)
{
	bool result = true;

	for (int i = 0; i < 4; ++i)
		result = result && v[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Math::Vec<2, bool> TRAP::Math::Not(const Vec<2, bool>& v)
{
	Vec<2, bool> result(true);

	for (int i = 0; i < 2; ++i)
		result[i] = !v[i];

	return result;
}
constexpr TRAP::Math::Vec<3, bool> TRAP::Math::Not(const Vec<3, bool>& v)
{
	Vec<3, bool> result(true);

	for (int i = 0; i < 3; ++i)
		result[i] = !v[i];

	return result;
}
constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Not(const Vec<4, bool>& v)
{
	Vec<4, bool> result(true);

	for (int i = 0; i < 4; ++i)
		result[i] = !v[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Trigonometric------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
constexpr genType TRAP::Math::Radians(genType degrees)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'Radians' only accepts floating-point inputs");

	return degrees * static_cast<genType>(0.01745329251994329576923690768489);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Radians(const Vec<2, T>& v)
{
	return Vec<2, T>(Radians(v.x), Radians(v.y));
}
template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Radians(const Vec<3, T>& v)
{
	return Vec<3, T>(Radians(v.x), Radians(v.y), Radians(v.z));
}
template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Radians(const Vec<4, T>& v)
{
	return Vec<4, T>(Radians(v.x), Radians(v.y), Radians(v.z), Radians(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
constexpr genType TRAP::Math::Degrees(genType radians)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'Degrees' only accepts floating-point inputs");

	return radians * static_cast<genType>(57.295779513082320876798154814105);
}

template<typename T>
constexpr TRAP::Math::Vec<2, T> TRAP::Math::Degrees(const Vec<2, T>& v)
{
	return Vec<2, T>(Degrees(v.x), Degrees(v.y));
}
template<typename T>
constexpr TRAP::Math::Vec<3, T> TRAP::Math::Degrees(const Vec<3, T>& v)
{
	return Vec<3, T>(Degrees(v.x), Degrees(v.y), Degrees(v.z));
}
template<typename T>
constexpr TRAP::Math::Vec<4, T> TRAP::Math::Degrees(const Vec<4, T>& v)
{
	return Vec<4, T>(Degrees(v.x), Degrees(v.y), Degrees(v.z), Degrees(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Sin(T x)
{
	return std::sin(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Sin(const Vec<2, T>& v)
{
	return Vec<2, T>(std::sin(v.x), std::sin(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Sin(const Vec<3, T>& v)
{
	return Vec<3, T>(std::sin(v.x), std::sin(v.y), std::sin(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Sin(const Vec<4, T>& v)
{
	return Vec<4, T>(std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Cos(T x)
{
	return std::cos(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Cos(const Vec<2, T>& v)
{
	return Vec<2, T>(std::cos(v.x), std::cos(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Cos(const Vec<3, T>& v)
{
	return Vec<3, T>(std::cos(v.x), std::cos(v.y), std::cos(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Cos(const Vec<4, T>& v)
{
	return Vec<4, T>(std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Tan(T x)
{
	return std::tan(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::Tan(const Vec<2, T>& v)
{
	return Vec<2, T>(std::tan(v.x), std::tan(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::Tan(const Vec<3, T>& v)
{
	return Vec<3, T>(std::tan(v.x), std::tan(v.y), std::tan(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::Tan(const Vec<4, T>& v)
{
	return Vec<4, T>(std::tan(v.x), std::tan(v.y), std::tan(v.z), std::tan(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ASin(T x)
{
	return std::asin(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::ASin(const Vec<2, T>& v)
{
	return Vec<2, T>(std::asin(v.x), std::asin(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::ASin(const Vec<3, T>& v)
{
	return Vec<3, T>(std::asin(v.x), std::asin(v.y), std::asin(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::ASin(const Vec<4, T>& v)
{
	return Vec<4, T>(std::asin(v.x), std::asin(v.y), std::asin(v.z), std::asin(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ACos(T x)
{
	return std::acos;
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::ACos(const Vec<2, T>& v)
{
	return Vec<2, T>(std::acos(v.x), std::acos(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::ACos(const Vec<3, T>& v)
{
	return Vec<3, T>(std::acos(v.x), std::acos(v.y), std::acos(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::ACos(const Vec<4, T>& v)
{
	return Vec<4, T>(std::acos(v.x), std::acos(v.y), std::acos(v.z), std::acos(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
genType TRAP::Math::ATan(genType y, genType x)
{
	static_assert(std::numeric_limits<genType>::is_iec559, "'ATan' only accepts floating-point inputs");

	return std::atan2(y, x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::ATan(const Vec<2, T>& a, const Vec<2, T>& b)
{
	return Vec<2, T>(std::atan2(a.x, b.x), std::atan2(a.y, b.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::ATan(const Vec<3, T>& a, const Vec<3, T>& b)
{
	return Vec<3, T>(std::atan2(a.x, b.x), std::atan2(a.y, b.y), std::atan2(a.z, b.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::ATan(const Vec<4, T>& a, const Vec<4, T>& b)
{
	return Vec<4, T>(std::atan2(a.x, b.x), std::atan2(a.y, b.y), std::atan2(a.z, b.z), std::atan2(a.w, b.w));
}

template<typename T>
T TRAP::Math::ATan(T x)
{
	return std::atan(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::ATan(const Vec<2, T>& v)
{
	return Vec<2, T>(std::atan(v.x), std::atan(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::ATan(const Vec<3, T>& v)
{
	return Vec<3, T>(std::atan(v.x), std::atan(v.y), std::atan(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::ATan(const Vec<4, T>& v)
{
	return Vec<4, T>(std::atan(v.x), std::atan(v.y), std::atan(v.z), std::atan(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::SinH(T x)
{
	return std::sinh(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::SinH(const Vec<2, T>& v)
{
	return Vec<2, T>(std::sinh(v.x), std::sinh(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::SinH(const Vec<3, T>& v)
{
	return Vec<3, T>(std::sinh(v.x), std::sinh(v.y), std::sinh(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::SinH(const Vec<4, T>& v)
{
	return Vec<4, T>(std::sinh(v.x), std::sinh(v.y), std::sinh(v.z), std::sinh(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::CosH(T x)
{
	return std::cosh(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::CosH(const Vec<2, T>& v)
{
	return Vec<2, T>(std::cosh(v.x), std::cosh(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::CosH(const Vec<3, T>& v)
{
	return Vec<3, T>(std::cosh(v.x), std::cosh(v.y), std::cosh(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::CosH(const Vec<4, T>& v)
{
	return Vec<4, T>(std::cosh(v.x), std::cosh(v.y), std::cosh(v.z), std::cosh(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::TanH(T x)
{
	return std::tanh(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::TanH(const Vec<2, T>& v)
{
	return Vec<2, T>(std::tanh(v.x), std::tanh(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::TanH(const Vec<3, T>& v)
{
	return Vec<3, T>(std::tanh(v.x), std::tanh(v.y), std::tanh(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::TanH(const Vec<4, T>& v)
{
	return Vec<3, T>(std::tanh(v.x), std::tanh(v.y), std::tanh(v.z), std::tanh(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ASinH(T x)
{
	return std::asinh(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::ASinH(const Vec<2, T>& v)
{
	return Vec<2, T>(std::asinh(v.x), std::asinh(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::ASinH(const Vec<3, T>& v)
{
	return Vec<3, T>(std::asinh(v.x), std::asinh(v.y), std::asinh(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::ASinH(const Vec<4, T>& v)
{
	return Vec<4, T>(std::asinh(v.x), std::asinh(v.y), std::asinh(v.z), std::asinh(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ACosH(T x)
{
	return std::acosh(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::ACosH(const Vec<2, T>& v)
{
	return Vec<2, T>(std::acosh(v.x), std::acosh(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::ACosH(const Vec<3, T>& v)
{
	return Vec<3, T>(std::acosh(v.x), std::acosh(v.y), std::acosh(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::ACosH(const Vec<4, T>& v)
{
	return Vec<4, T>(std::acosh(v.x), std::acosh(v.y), std::acosh(v.z), std::acosh(v.w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ATanH(T x)
{
	return std::atanh(x);
}

template<typename T>
TRAP::Math::Vec<2, T> TRAP::Math::ATanH(const Vec<2, T>& v)
{
	return Vec<2, T>(std::atanh(v.x), std::atanh(v.y));
}
template<typename T>
TRAP::Math::Vec<3, T> TRAP::Math::ATanH(const Vec<3, T>& v)
{
	return Vec<3, T>(std::atanh(v.x), std::atanh(v.y), std::atanh(v.z));
}
template<typename T>
TRAP::Math::Vec<4, T> TRAP::Math::ATanH(const Vec<4, T>& v)
{
	return Vec<4, T>(std::atanh(v.x), std::atanh(v.y), std::atanh(v.z), std::atanh(v.w));
}

#endif /*_TRAP_MATH_H_*/
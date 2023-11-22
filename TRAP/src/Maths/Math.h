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

#ifndef TRAP_MATH_H
#define TRAP_MATH_H

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat3.h"
#include "Mat4.h"
#include "Quaternion.h"
#include "Core/Base.h"
#include "TRAP_Assert.h"

#include <cmath>
#include <limits>
#include <numbers>
#include <numeric>

#include <gcem.hpp>

namespace TRAP::Math
{
	//Constants

	/// @brief Return the epsilon constant.
	template<typename T>
	[[nodiscard]] constexpr T Epsilon()
	{
		return std::numeric_limits<T>::epsilon();
	}
	/// @brief Approximation of cos(1/2) in radians.
	template<typename T>
	[[nodiscard]] constexpr T CosOneOverTwo() noexcept
	{
		return T(0.877582561890372716130286068203503191);
	}
	/// @brief Return the pi constant.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T PI() noexcept
	{
		return std::numbers::pi_v<T>;
	}
	/// @brief Return tau = 2 * pi.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T TAU() noexcept
	{
		return T(6.2831853071795864769252867665581);
	}
	/// @brief Return pi * 2, same as TAU.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T TwoPI() noexcept
	{
		return T(6.28318530717958647692528676655900576);
	}
	/// @brief Return square root of pi.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T RootPI() noexcept
	{
		return T(1.772453850905516027);
	}
	/// @brief Return pi / 2.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T HalfPI() noexcept
	{
		return T(1.57079632679489661923132169163975144);
	}
	/// @brief Return pi / 2 * 3.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T ThreeOverTwoPI() noexcept
	{
		return T(4.71238898038468985769396507491925432);
	}
	/// @brief Return pi / 4.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T QuarterPI() noexcept
	{
		return T(0.785398163397448309615660845819875721);
	}
	/// @brief Return 1 / pi.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T OneOverPI() noexcept
	{
		return std::numbers::inv_pi_v<T>;
	}
	/// @brief Return 1 / (pi * 2).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T OneOverTwoPI() noexcept
	{
		return T(0.159154943091895335768883763372514362);
	}
	/// @brief Return 2 / pi.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T TwoOverPI() noexcept
	{
		return T(0.636619772367581343075535053490057448);
	}
	/// @brief Return 4 / pi.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T FourOverPI() noexcept
	{
		return T(1.273239544735162686151070106980114898);
	}
	/// @brief Return 2 / sqrt(pi).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T TwoOverRootPI() noexcept
	{
		return T(1.12837916709551257389615890312154517);
	}
	/// @brief Return 1 / sqrt(2).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T OneOverRootTwo() noexcept
	{
		return T(0.707106781186547524400844362104849039);
	}
	/// @brief Return sqrt(pi / 2).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T RootHalfPI() noexcept
	{
		return T(1.253314137315500251);
	}
	/// @brief Return sqrt(2 * pi).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T RootTwoPI() noexcept
	{
		return T(2.506628274631000502);
	}
	/// @brief Return sqrt(ln(4)).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T RootLnFour() noexcept
	{
		return T(1.17741002251547469);
	}
	/// @brief Return e constant.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T e() noexcept
	{
		return std::numbers::e_v<T>;
	}
	/// @brief Return Euler's constant.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Euler() noexcept
	{
		return T(0.577215664901532860606);
	}
	/// @brief Return sqrt(2).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T RootTwo() noexcept
	{
		return std::numbers::sqrt2_v<T>;
	}
	/// @brief Return sqrt(3).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T RootThree() noexcept
	{
		return std::numbers::sqrt3_v<T>;
	}
	/// @brief Return sqrt(5).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T RootFive() noexcept
	{
		return T(2.23606797749978969640917366873127623);
	}
	/// @brief Return ln(2).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T LnTwo() noexcept
	{
		return std::numbers::ln2_v<T>;
	}
	/// @brief Return ln(10).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T LnTen() noexcept
	{
		return std::numbers::ln10_v<T>;
	}
	/// @brief Return ln(ln(2)).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T LnLnTwo() noexcept
	{
		return T(-0.3665129205816643);
	}
	/// @brief Return 1 / 3.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Third() noexcept
	{
		return T(0.3333333333333333333333333333333333333333);
	}
	/// @brief Return 2 / 3.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T TwoThirds() noexcept
	{
		return T(0.666666666666666666666666666666666666667);
	}
	/// @brief Return the golden ratio constant.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T GoldenRatio() noexcept
	{
		return std::numbers::phi_v<T>;
	}

	//-------------------------------------------------------------------------------------------------------------------//
	//Common-------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the minimum value of x and y.
	/// @param x Specify the first value to compare.
	/// @param y Specify the second value to compare.
	/// @return y if y < x; otherwise it returns x.
	template<typename genType>
	requires std::is_arithmetic_v<genType>
	[[nodiscard]] constexpr genType Min(genType x, genType y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the minimum value of x, y and z.
	/// @param x Specify the first value to compare.
	/// @param y Specify the second value to compare.
	/// @param z Specify the third value to compare.
	/// @return Returns the smallest value of x, y and z.
	template<typename genType>
	requires std::is_arithmetic_v<genType>
	[[nodiscard]] constexpr genType Min(genType x, genType y, genType z);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the minimum value of x, y, z and w.
	/// @param x Specify the first value to compare.
	/// @param y Specify the second value to compare.
	/// @param z Specify the third value to compare.
	/// @param w Specify the fourth value to compare.
	/// @return Returns the smallest value of x, y, z and w.
	template<typename genType>
	requires std::is_arithmetic_v<genType>
	[[nodiscard]] constexpr genType Min(genType x, genType y, genType z, genType w);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the maximum value of x and y.
	/// @param x Specify the first value to compare.
	/// @param y Specify the second value to compare.
	/// @return y if x < y; otherwise it returns x.
	template<typename genType>
	requires std::is_arithmetic_v<genType>
	[[nodiscard]] constexpr genType Max(genType x, genType y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the maximum value of x, y and z.
	/// @param x Specify the first value to compare.
	/// @param y Specify the second value to compare.
	/// @param z Specify the third value to compare.
	/// @return Returns the greatest value of x, y and z.
	template<typename genType>
	requires std::is_arithmetic_v<genType>
	[[nodiscard]] constexpr genType Max(genType x, genType y, genType z);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the maximum value of x, y, z and w.
	/// @param x Specify the first value to compare.
	/// @param y Specify the second value to compare.
	/// @param z Specify the third value to compare.
	/// @param w Specify the fourth value to compare.
	/// @return Returns the greatest value of x, y, z and w.
	template<typename genType>
	requires std::is_arithmetic_v<genType>
	[[nodiscard]] constexpr genType Max(genType x, genType y, genType z, genType w);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Find the nearest integer of x.
	///
	/// The fraction 0.5 will round in a direction chosen by the implementation,
	/// presumably the direction that is fastest.
	/// This includes the possibility that Round(x) returns the same value as RoundEven(x) for all values of x.
	/// @param x Specify the value to evaluate.
	/// @return Value equal to the nearest integer to x.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType Round(genType x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Find the nearest integer less than or equal to x.
	/// @param x Specify the value to evaluate.
	/// @return Value equal to the nearest integer to x whose absolute value is not larger than the absolute value of x.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType Trunc(genType x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the absolute value of x.
	/// @param x Specify the value of which to return the absolute.
	/// @return x if x >= 0; otherwise it returns -x.
	template<typename genFIType>
	requires std::signed_integral<genFIType> || std::floating_point<genFIType>
	[[nodiscard]] constexpr genFIType Abs(genFIType x);

	/// @brief Retrieve the absolute value of x.
	/// @param x Specify the value of which to return the absolute.
	/// @return x if x >= 0; otherwise it returns -x.
	template<u32 L, typename T>
	requires std::signed_integral<T> || std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Abs(Vec<L, T> x);

	/// @brief Retrieve the absolute value of x.
	/// @param x Specify the value of which to return the absolute.
	/// @return x if x >= 0; otherwise it returns -x.
	template<u32 C, u32 R, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<C, R, T> Abs(Mat<C, R, T> x);

	//-------------------------------------------------------------------------------------------------------------------//

	//Fast and works for any type

	/// @brief Extract the sign of x.
	/// @param x Specify the value from which to extract the sign.
	/// @return 1.0f if x > 0, 0.0 if x == 0, or -1.0 if x < 0.
	template<typename genFIType>
	requires std::floating_point<genFIType> || std::signed_integral<genFIType>
	[[nodiscard]] constexpr genFIType Sign(genFIType x);

	/// @brief Extract the sign of x.
	/// @param x Specify the value from which to extract the sign.
	/// @return 1.0f if x > 0, 0.0 if x == 0, or -1.0 if x < 0.
	template<u32 L, typename T>
	requires std::floating_point<T> || std::signed_integral<T>
	[[nodiscard]] constexpr Vec<L, T> Sign(const Vec<L, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Find the nearest integer less than or equal to x.
	/// @param x Specify the value to evaluate.
	/// @return Value equal to the nearest integer that is less then or equal to x.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Floor(T x);

	/// @brief Find the nearest integer less than or equal to x.
	/// @param x Specify the value to evaluate.
	/// @return Value equal to the nearest integer that is less then or equal to x.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Floor(Vec<L, T> x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Truncate value.
	/// @param x Specify the value to evaluate.
	/// @return Value equal to the nearest integer to x whose absolute value is not larger than the absolute value of x.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Trunc(Vec<L, T> x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Find the nearest integer less than or equal to the parameter.
	///
	/// The fraction 0.5 will round in a direction chosen by the implementation,
	/// presumably the direction that is fastest.
	/// This includes the possibility that Round(x) returns the same value as RoundEven(x) for all values of x.
	/// @param x Specify the value to evaluate.
	/// @return Value equal to the nearest integer to x.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Round(Vec<L, T> x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Find the nearest even integer to x.
	///
	/// A fractional part of 0.5 will round toward the nearest even integer.
	/// (Both 3.5 and 4.5 for x will return 4.0).
	/// @param x Specify the value to evaluate.
	/// @return Value equal to the nearest integer to x.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType RoundEven(genType x);

	/// @brief Find the nearest even integer to x.
	///
	/// A fractional part of 0.5 will round toward the nearest even integer.
	/// (Both 3.5 and 4.5 for x will return 4.0).
	/// @param x Specify the value to evaluate.
	/// @return Value equal to the nearest integer to x.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> RoundEven(Vec<L, T> x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Find the nearest integer that is greater than or equal to x.
	/// @param x Specify the value to evaluate.
	/// @return Value equal to the nearest integer that is greater than or equal to x.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Ceil(T x);

	/// @brief Find the nearest integer that is greater than or equal to x.
	/// @param x Specify the value to evaluate.
	/// @return Value equal to the nearest integer that is greater than or equal to x.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Ceil(Vec<L, T> x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve fractional part of x.
	/// @param x Specify the value to evaluate.
	/// @return x - Floor(x).
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType Fract(genType x);

	/// @brief Retrieve fractional part of x.
	/// @param x Specify the value to evaluate.
	/// @return x - Floor(x).
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Fract(const Vec<L, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve value of x modulo y.
	/// @param x Specify the value to evaluate.
	/// @param y Specify the value to evaluate.
	/// @return x - y * Floor(x / y) for each component in x using the floating point value y.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType Mod(genType x, genType y);

	/// @brief Retrieve value of x modulo y.
	/// @param x Specify the value to evaluate.
	/// @param y Specify the value to evaluate.
	/// @return x - y * Floor(x / y) for each component in x using the floating point value y.
	template<typename genType>
	requires std::signed_integral<genType>
	[[nodiscard]] constexpr genType Mod(genType x, genType y);

	/// @brief Retrieve value of x modulo y.
	/// @param x Specify the value to evaluate.
	/// @param y Specify the value to evaluate.
	/// @return x - y * Floor(x / y) for each component in x using the floating point value y.
	template<typename genType>
	requires std::unsigned_integral<genType>
	[[nodiscard]] constexpr genType Mod(genType x, genType y);

	/// @brief Retrieve value of x modulo y.
	/// @param x Specify the value to evaluate.
	/// @param y Specify the value to evaluate.
	/// @return x - y * Floor(x / y) for each component in x using the floating point value y.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Mod(Vec<L, T> x, T y);

	/// @brief Retrieve value of x modulo y.
	/// @param x Specify the value to evaluate.
	/// @param y Specify the value to evaluate.
	/// @return x - y * Floor(x / y) for each component in x using the floating point value y.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Mod(Vec<L, T> x, const Vec<L, T>& y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Separate a value into its integer and fractional components
	///
	/// Both the return value and the output parameter will have the same sign as x.
	/// @param x Specify the value to separate.
	/// @param i Out variable that receives the integer part of the argument.
	/// @return Fractional part of x and sets i to the integer part (as a whole number floating point value).
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] genType Modf(genType x, genType& i);

	/// @brief Separate a value into its integer and fractional components
	///
	/// Both the return value and the output parameter will have the same sign as x.
	/// @param x Specify the value to separate.
	/// @param i Out variable that receives the integer part of the argument.
	/// @return Fractional part of x and sets i to the integer part (as a whole number floating point value).
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] Vec<L, T> Modf(Vec<L, T> x, Vec<L, T>& i);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the minimum value of x and y.
	/// @param a Specify the first value to compare.
	/// @param b Specify the second value to compare.
	/// @return y if y < x; otherwise it returns x.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, T> Min(Vec<L, T> a, T b);

	/// @brief Retrieve the minimum value of x and y.
	/// @param a Specify the first value to compare.
	/// @param b Specify the second value to compare.
	/// @return y if y < x; otherwise it returns x.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, T> Min(Vec<L, T> a, const Vec<L, T>& b);

	/// @brief Retrieve the minimum value of x and y.
	/// @brief @param a Specify the first value to compare.
	/// @param b Specify the second value to compare.
	/// @param c Specify the third value to compare.
	/// @return Returns the minimum component-wise values of 3 inputs
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, T> Min(const Vec<L, T>& a, const Vec<L, T>& b, const Vec<L, T>& c);

	/// @brief Retrieve the minimum value of x and y.
	/// @brief @param a Specify the first value to compare.
	/// @param b Specify the second value to compare.
	/// @param c Specify the third value to compare.
	/// @param d Specify the fourth value to compare.
	/// @return Returns the minimum component-wise values of 4 inputs
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, T> Min(const Vec<L, T>& a, const Vec<L, T>& b, const Vec<L, T>& c, const Vec<L, T>& d);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the maximum value of x and y.
	/// @param a Specify the first value to compare.
	/// @param b Specify the second value to compare.
	/// @return y if x < y; otherwise it returns x.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, T> Max(Vec<L, T> a, T b);

	/// @brief Retrieve the maximum value of x and y.
	/// @param a Specify the first value to compare.
	/// @param b Specify the second value to compare.
	/// @return y if x < y; otherwise it returns x.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, T> Max(Vec<L, T> a, const Vec<L, T>& b);

	/// @brief @param a Specify the first value to compare.
	/// @param b Specify the second value to compare.
	/// @param c Specify the third value to compare.
	/// @return Returns the maximum component-wise values of 3 inputs
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, T> Max(const Vec<L, T>& a, const Vec<L, T>& b, const Vec<L, T>& c);

	/// @brief @param a Specify the first value to compare.
	/// @param b Specify the second value to compare.
	/// @param c Specify the third value to compare.
	/// @param d Specify the fourth value to compare.
	/// @return Returns the maximum component-wise values of 4 inputs
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, T> Max(const Vec<L, T>& a, const Vec<L, T>& b, const Vec<L, T>& c, const Vec<L, T>& d);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Constrain x to lie between minVal and maxVal.
	/// @param x Specify the value to constrain.
	/// @param minVal Specify the lower end of the range into which to constrain x.
	/// @param maxVal Specify the upper end of the range into which to constrain x.
	/// @return Min(Max(x, minval), maxVal) for each component in x using the floating-point values minval and maxVal.
	template<typename genType>
	requires std::is_arithmetic_v<genType>
	[[nodiscard]]
#ifndef TRAP_ENABLE_ASSERTS
	constexpr
#endif /*TRAP_ENABLE_ASSERTS*/
	genType Clamp(genType x, genType minVal, genType maxVal);

	/// @brief Constrain x to lie between minVal and maxVal.
	/// @param x Specify the value to constrain.
	/// @param minVal Specify the lower end of the range into which to constrain x.
	/// @param maxVal Specify the upper end of the range into which to constrain x.
	/// @return Min(Max(x, minval), maxVal) for each component in x using the floating-point values minval and maxVal.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]]
#ifndef TRAP_ENABLE_ASSERTS
	constexpr
#endif /*TRAP_ENABLE_ASSERTS*/
	Vec<L, T> Clamp(const Vec<L, T>& x, T minVal, T maxVal);

	/// @brief Constrain x to lie between minVal and maxVal.
	/// @param x Specify the value to constrain.
	/// @param minVal Specify the lower end of the range into which to constrain x.
	/// @param maxVal Specify the upper end of the range into which to constrain x.
	/// @return Min(Max(x, minval), maxVal) for each component in x using the floating-point values minval and maxVal.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]]
#ifndef TRAP_ENABLE_ASSERTS
	constexpr
#endif /*TRAP_ENABLE_ASSERTS*/
	Vec<L, T> Clamp(const Vec<L, T>& x, const Vec<L, T>& minVal, const Vec<L, T>& maxVal);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Linearly interpolate between the values.
	///
	/// If genTypeU is a floating scalar or vector:
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.
	/// The value for a is not restricted to the range [0, 1].
	///
	/// If genTypeU is a boolean scalar or vector:
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a component of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point
	/// values and will have no effect on the results.
	/// @param x Specify the start of the range in which to interpolate.
	/// @param y Specify the end of the range in which to interpolate.
	/// @param a SPecify the value to use to interpolate between x and y.
	/// @return See description.
	template<typename genTypeT, typename genTypeU>
	requires std::floating_point<genTypeU> || std::same_as<genTypeU, bool>
	[[nodiscard]] constexpr genTypeT Mix(genTypeT x, genTypeT y, genTypeU a);

	/// @brief Linearly interpolate between the values.
	///
	/// If genTypeU is a floating scalar or vector:
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.
	/// The value for a is not restricted to the range [0, 1].
	///
	/// If genTypeU is a boolean scalar or vector:
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a component of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point
	/// values and will have no effect on the results.
	/// @param x Specify the start of the range in which to interpolate.
	/// @param y Specify the end of the range in which to interpolate.
	/// @param a SPecify the value to use to interpolate between x and y.
	/// @return See description.
	template<u32 L, typename T, typename U>
	requires std::floating_point<U> || std::same_as<U, bool>
	[[nodiscard]] constexpr Vec<L, T> Mix(const Vec<L, T>& x, const Vec<L, T>& y, U a);

	/// @brief Linearly interpolate between the values.
	///
	/// If genTypeU is a floating scalar or vector:
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.
	/// The value for a is not restricted to the range [0, 1].
	///
	/// If genTypeU is a boolean scalar or vector:
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a component of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point
	/// values and will have no effect on the results.
	/// @param x Specify the start of the range in which to interpolate.
	/// @param y Specify the end of the range in which to interpolate.
	/// @param a SPecify the value to use to interpolate between x and y.
	/// @return See description.
	template<u32 L, typename T, typename U>
	requires std::floating_point<U> || std::same_as<U, bool>
	[[nodiscard]] constexpr Vec<L, T> Mix(const Vec<L, T>& x, const Vec<L, T>& y, const Vec<L, U>& a);

	/// @brief Spherical linear interpolation of two quaternions.
	/// The interpolation is oriented and the rotation is performed at constant speed.
	/// For short path spherical linear interpolation, use the Slerp function.
	/// @param x A quaternion.
	/// @param y A quaternion.
	/// @param a Interpolation factor. The interpolation is defined beyond the range [0, 1].
	/// @return Spherical linear interpolation of two quaternions.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Mix(const tQuat<T>& x, const tQuat<T>& y, T a);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a equal-to comparison of two values.
	/// @param x Specifies the first value to be used in the comparison operation.
	/// @param y Specifies the second value to be used in the comparison operation.
	/// @return Comparison result of x == y.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr bool Equal(T x, T y) noexcept;

	/// @brief Perform a equal-to comparison of two values taking epsilon into account.
	/// @param x Specifies the first value to be used in the comparison operation.
	/// @param y Specifies the second value to be used in the comparison operation.
	/// @param epsilon
	/// @return Comparison of |x - y| < epsilon. True if this expression is satisfied.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr bool Equal(T x, T y, T epsilon);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a not-equal-to comparison of two values.
	/// @param x Specifies the first value to be used in the comparison operation.
	/// @param y Specifies the second value to be used in the comparison operation.
	/// @return Comparison result of x != y.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr bool NotEqual(T x, T y) noexcept;

	/// @brief Perform a not-equal-to comparison of two values taking epsilon into account.
	/// @param x Specifies the first value to be used in the comparison operation.
	/// @param y Specifies the second value to be used in the comparison operation.
	/// @param epsilon
	/// @return Comparison result of x != y.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr bool NotEqual(T x, T y, T epsilon);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a greater-than comparison of two values.
	/// @param x Specifies the first value to be used in the comparison operation.
	/// @param y Specifies the second value to be used in the comparison operation.
	/// @return Comparison result of x > y.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr bool GreaterThan(T x, T y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a greater-than-or-equal comparison of two values.
	/// @param x Specifies the first value to be used in the comparison operation.
	/// @param y Specifies the second value to be used in the comparison operation.
	/// @return Comparison result of x >= y.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr bool GreaterThanEqual(T x, T y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a less-than comparison of two values.
	/// @param x Specifies the first value to be used in the comparison operation.
	/// @param y Specifies the second value to be used in the comparison operation.
	/// @return Comparison result of x < y.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr bool LessThan(T x, T y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a less-than-or-equal comparison of two values.
	/// @param x Specifies the first values to be used in the comparison operation.
	/// @param y Specifies the second values to be used in the comparison operation.
	/// @return Comparison result of x <= y.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr bool LessThanEqual(T x, T y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Logically invert a boolean value.
	/// @param v Specifies the value to be inverted.
	/// @return Logical complement of x.
	[[nodiscard]] constexpr bool Not(bool v) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Generate a step function by comparing x to edgee.
	/// @param edge Specifies the location of the edge of the step function.
	/// @param x Specify the value to be used to generate the step function.
	/// @return 0.0 if x < edge, otherwise it returns 1.0 for each component of a genType.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType Step(genType edge, genType x);

	/// @brief Generate a step function by comparing x to edgee.
	/// @param edge Specifies the location of the edge of the step function.
	/// @param x Specify the value to be used to generate the step function.
	/// @return 0.0 if x < edge, otherwise it returns 1.0.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Step(T edge, const Vec<L, T>& x);

	/// @brief Generate a step function by comparing x to edgee.
	/// @param edge Specifies the location of the edge of the step function.
	/// @param x Specify the value to be used to generate the step function.
	/// @return 0.0 if x < edge, otherwise it returns 1.0.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Step(const Vec<L, T>& edge, const Vec<L, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform Hermite interpolation between 0 and 1 when edge0 < x < edge1.
	///
	/// This is useful in cases where you would want a threshold function with a smooth transition.
	/// This is equivalent to: genType t; t = Clamp((x - edge0) / (edge1 - edge0), 0, 1); return t * t * (3 - 2 * t);
	/// @param edge0 Specifies the value of the lower edge of the Hermite function.
	/// @param edge1 Specifies the value of the upper edge of the Hermite function.
	/// @param x Specifies the source value for interpolation.
	/// @return
	/// 0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.
	/// @note Results are undefined if edge0 >= edge1!
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType SmoothStep(genType edge0, genType edge1, genType x);

	/// @brief Perform Hermite interpolation between 0 and 1 when edge0 < x < edge1.
	///
	/// This is useful in cases where you would want a threshold function with a smooth transition.
	/// This is equivalent to: genType t; t = Clamp((x - edge0) / (edge1 - edge0), 0, 1); return t * t * (3 - 2 * t);
	/// @param edge0 Specifies the value of the lower edge of the Hermite function.
	/// @param edge1 Specifies the value of the upper edge of the Hermite function.
	/// @param x Specifies the source value for interpolation.
	/// @return
	/// 0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.
	/// @note Results are undefined if edge0 >= edge1!
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> SmoothStep(T edge0, T edge1, const Vec<L, T>& x);

	/// @brief Perform Hermite interpolation between 0 and 1 when edge0 < x < edge1.
	///
	/// This is useful in cases where you would want a threshold function with a smooth transition.
	/// This is equivalent to: genType t; t = Clamp((x - edge0) / (edge1 - edge0), 0, 1); return t * t * (3 - 2 * t);
	/// @param edge0 Specifies the value of the lower edge of the Hermite function.
	/// @param edge1 Specifies the value of the upper edge of the Hermite function.
	/// @param x Specifies the source value for interpolation.
	/// @return
	/// 0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.
	/// @note Results are undefined if edge0 >= edge1!
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> SmoothStep(const Vec<L, T>& edge0, const Vec<L, T>& edge1, const Vec<L, T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Determine whether the parameter is a number.
	/// @param x Specifies the value to test for NaN.
	/// @return
	/// True if x hold a NaN (not a number) representation in the underlying implementation's
	/// set of floating-point representations.
	/// False otherwise, including for implementations with no NaN representations.
	/// @note When using compiler fast math, this function may fail!
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr bool IsNaN(genType x) noexcept;

	/// @brief Determine whether the parameter is a number.
	/// @param v Specifies the value to test for NaN.
	/// @return
	/// True if x hold a NaN (not a number) representation in the underlying implementation's
	/// set of floating-point representations.
	/// False otherwise, including for implementations with no NaN representations.
	/// @note When using compiler fast math, this function may fail!
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, bool> IsNaN(const Vec<L, T>& v);

	/// @brief Determine whether the parameter is a number.
	/// @param q Specifies the value to test for NaN.
	/// @return
	/// True if x hold a NaN (not a number) representation in the underlying implementation's
	/// set of floating-point representations.
	/// False otherwise, including for implementations with no NaN representations.
	/// @note When using compiler fast math, this function may fail!
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, bool> IsNaN(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Determine whether the paramter is positive or negative infinity
	/// @param x Specifies the value to test for infinity.
	/// @return True if x holds a positive infinity or negative infinity representation in the underlying implementation's
	/// set of floating-point representations.
	/// False otherwise, including for implementations with no infinity representations.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr bool IsInf(genType x) noexcept;

	/// @brief Determine whether the paramter is positive or negative infinity
	/// @param v Specifies the value to test for infinity.
	/// @return True if x holds a positive infinity or negative infinity representation in the underlying implementation's
	/// set of floating-point representations.
	/// False otherwise, including for implementations with no infinity representations.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, bool> IsInf(const Vec<L, T>& v);

	/// @brief Determine whether the paramter is positive or negative infinity
	/// @param q Specifies the value to test for infinity.
	/// @return True if x holds a positive infinity or negative infinity representation in the underlying implementation's
	/// set of floating-point representations.
	/// False otherwise, including for implementations with no infinity representations.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, bool> IsInf(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Computes and returns a * b + c.
	/// @param a Specifies the first multiplicand.
	/// @param b Specifies the second multiplicand.
	/// @param c Specifies the value to be added to the result.
	/// @return Computes and returns a * b + c.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] genType FMA(genType a, genType b, genType c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Splits x into a floating-point significant in the range [0.5, 1.0) and an integral exponent of two,
	/// such that x = significant * exp(2, exponent).
	/// This significant is returned by the function and exponent is returned in the parameter exp.
	/// For a floating-point value of zero, the significant and exponent are both zero.
	/// @param x Specifies the value from which significand and exponent are to be extracted.
	/// @param exp Out specifies the variable into which to place the exponent.
	/// @return See description.
	/// @note For a floating-point value that is at infinity or is not a number, the results are undefined!
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] genType FrExp(genType x, i32& exp);

	/// @brief Splits x into a floating-point significant in the range [0.5, 1.0) and an integral exponent of two,
	/// such that x = significant * Exp(2, exponent).
	/// This significant is returned by the function and exponent is returned in the parameter exp.
	/// For a floating-point value of zero, the significant and exponent are both zero.
	/// @param v Specifies the value from which significand and exponent are to be extracted.
	/// @param exp Out specifies the variable into which to place the exponent.
	/// @return See description.
	/// @note For a floating-point value that is at infinity or is not a number, the results are undefined!
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] Vec<L, T> FrExp(Vec<L, T> v, Vec<L, i32>& exp);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Builds a floating-point number from x and the corresponding integral exponent of two in exp, returning:
	/// significant * Exp(2, exponent).
	/// @param x Specifies the value to be used as a source of significand.
	/// @param exp Specifies the value to be used as a source of exponent.
	/// @return See description.
	/// @note If this product is too large to be represented in the floating-point type, the result is undefined!
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] genType LdExp(const genType& x, const i32& exp);

	/// @brief Builds a floating-point number from x and the corresponding integral exponent of two in exp, returning:
	/// significant * Exp(2, exponent).
	/// @param v Specifies the value to be used as a source of significand.
	/// @param exp Specifies the value to be used as a source of exponent.
	/// @return See description.
	/// @note If this product is too large to be represented in the floating-point type, the result is undefined!
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] Vec<L, T> LdExp(Vec<L, T> v, const Vec<L, i32>& exp);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check if value is a power of two number.
	/// @param value Specify the value to evaluate.
	/// @return True if the value is a power of two number.
	template<typename genType>
	requires std::unsigned_integral<genType>
	[[nodiscard]] constexpr bool IsPowerOfTwo(genType value);

	/// @brief Check if value is a power of two number.
	/// @param v Specify the value to evaluate.
	/// @return True if the value is a power of two number.
	template<u32 L, typename T>
	requires std::unsigned_integral<T>
	[[nodiscard]] constexpr Vec<L, bool> IsPowerOfTwo(const Vec<L, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Similar to 'Mod' but with a different rounding and integer support.
	/// @return 'x - y * Trunc(x / y)' instead of 'x - y * Floor(x / y)'.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T FMod(T x, T y);

	/// @brief Similar to 'Mod' but with a different rounding and integer support.
	/// @return 'x - y * Trunc(x / y)' instead of 'x - y * Floor(x / y)'.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> FMod(Vec<L, T> x, T y);

	/// @brief Similar to 'Mod' but with a different rounding and integer support.
	/// @return 'x - y * Trunc(x / y)' instead of 'x - y * Floor(x / y)'.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> FMod(Vec<L, T> x, const Vec<L, T>& y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Linear interpolation.
	/// @return Linear interpolation.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Lerp(T x, T y, T a);

	/// @brief Linear interpolation.
	/// @return Linear interpolation.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Lerp(const Vec<L, T>& x, const Vec<L, T>& y, T a);

	/// @brief Linear interpolation.
	/// @return Linear interpolation.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Lerp(const Vec<L, T>& x, const Vec<L, T>& y, const Vec<L, T>& a);

	/// @brief Linear interpolation of two quaternions.
	/// The interpolation is oriented.
	/// @param x A quaternion.
	/// @param y A quaternion.
	/// @param a Interpolation factor. The interpolation is defined in the range [0, 1].
	/// @return Linear interpolation of two quaternions.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Lerp(const tQuat<T>& x, const tQuat<T>& y, T a);

	//-------------------------------------------------------------------------------------------------------------------//
	//Exponential--------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the value of the first parameter raised to the power of the second.
	/// @param base Pow function is defined for input values of 'base' defined in the range (inf-, inf+)
	/// in the limit of the type qualifier.
	/// @param exponent Floating-point value representing the 'exponent'.
	/// @return 'base' raised to the power 'exponent'.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Pow(T base, T exponent);

	/// @brief Retrieve the value of the first parameter raised to the power of the second.
	/// @param base Floating-point value.
	/// Pow function is defined for input values of 'base' defined in the range (inf-, inf+)
	/// in the limit of the type qualifier.
	/// @param exponent Floating-point value representing the 'exponent'.
	/// @return 'base' raised to the power 'exponent'.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Pow(Vec<L, T> base, const Vec<L, T>& exponent);

	/// @brief Retrieve the value of the first parameter raised to the power of the second.
	/// @return Quaternion raised to a power.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Pow(const tQuat<T>& x, T y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the natural exponentiation of the parameter.
	///
	/// Exp function is defined for input values of x defined in the range (inf-, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value to exponentiate.
	/// @return Natural exponentiation of x. i.e., e^x.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Exp(T x);

	/// @brief Retrieve the natural exponentiation of the parameter.
	///
	/// Exp function is defined for input values of x defined in the range (inf-, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value to exponentiate.
	/// @return Natural exponentiation of x. i.e., e^x.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Exp(Vec<L, T> x);

	/// @brief Retrieve the natural exponentiation of the parameter.
	/// @param q Specify the value to exponentiate.
	/// @return Exponential of a quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Exp(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the natural logarithm of the parameter
	///
	/// Log function is defined for input values of x defined in the range (0, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value of which to take the natural logarithm.
	/// @return Natural logarithm of x, i.e., returns the value y which satisfies the equation x = e^y.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Log(T x);

	/// @brief Retrieve the natural logarithm of the parameter
	///
	/// Log function is defined for input values of x defined in the range (0, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value of which to take the natural logarithm.
	/// @return Natural logarithm of x, i.e., returns the value y which satisfies the equation x = e^y.
	/// @note Results are undefined if x <= 0!
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Log(Vec<L, T> x);

	/// @brief Retrieve the natural logarithm of the parameter
	/// @param q Specify the value of which to take the natural logarithm.
	/// @return Logarithm of a quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Log(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve 2 raised to the power of the parameter
	///
	/// Exp2 function is defined for input values of x defined in the range (inf-, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value of the power to which 2 will be raised.
	/// @return 2 raised to the x power.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] genType Exp2(genType x);

	/// @brief Retrieve 2 raised to the power of the parameter
	///
	/// Exp2 function is defined for input values of x defined in the range (inf-, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value of the power to which 2 will be raised.
	/// @return 2 raised to the x power.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] Vec<L, T> Exp2(Vec<L, T> x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the base 2 logarithm of the parameter.
	///
	/// Log2 function is defined for input values of x defined in the range (0, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value of which to take the base 2 logarithm.
	/// @return Base 2 log of x, i.e., returns the value y, which satisfies the equation x = 2^y.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType Log2(genType x);

	/// @brief Retrieve the base 2 logarithm of the parameter.
	///
	/// Log2 function is defined for input values of x defined in the range (0, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value of which to take the base 2 logarithm.
	/// @return Base 2 log of x, i.e., returns the value y, which satisfies the equation x = 2^y.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Log2(Vec<L, T> x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the square root of the parameter.
	///
	/// Sqrt function is defined for input values of x defined in the range [0, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value of which to take the square root.
	/// @return Positive square root of x.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Sqrt(T x);

	/// @brief Retrieve the square root of the parameter.
	///
	/// Sqrt function is defined for input values of x defined in the range [0, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value of which to take the square root.
	/// @return Positive square root of x.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Sqrt(Vec<L, T> x);

	/// @brief Retrieve the square root of the parameter.
	/// @param x Specify the value of which to take the square root.
	/// @return Square root of a quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Sqrt(const tQuat<T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the inverse square root of the parameter.
	///
	/// InverseSqrt function is defined for input values of x defined in the range [0, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value of which to take the inverse of the square root.
	/// @return Inverse of the square root of x.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType InverseSqrt(genType x);

	/// @brief Retrieve the inverse square root of the parameter.
	///
	/// InverseSqrt function is defined for input values of x defined in the range [0, inf+)
	/// in the limit of the type qualifier.
	/// @param x Specify the value of which to take the inverse of the square root.
	/// @return Inverse of the square root of x.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> InverseSqrt(Vec<L, T> x);

	//-------------------------------------------------------------------------------------------------------------------//
	//Geometric----------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculates the length of a vector.
	/// @param v Specifies a vector of which to calculate the length.
	/// @return Length of x, i.e., Sqrt(x * x).
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Length(const Vec<L, T>& v);

	/// @brief Calculates the length of a vector.
	/// @param q Specifies a quaternion of which to calculate the length.
	/// @return Norm of a quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Length(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the distance between two points.
	/// @param p0 Specifies the first of two points.
	/// @param p1 Specifies the second of two points.
	/// @return Distance between p0 and p1, i.e., Length(p0 - p1).
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Distance(const Vec<L, T>& p0, const Vec<L, T>& p1);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the dot product.
	/// @param x Specifies the first of two vectors.
	/// @param y Specifies the second of two vectors.
	/// @return Dot product of x and y, i.e., result = x * y.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Dot(const Vec<L, T>& x, const Vec<L, T>& y);

	/// @brief Calculate the dot product.
	/// @param x Specifies the first of two quaternions.
	/// @param y Specifies the second of two quaternions.
	/// @return Compute a cross product.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Dot(const tQuat<T>& x, const tQuat<T>& y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the cross product.
	/// @param x Specifies the first of two vectors.
	/// @param y Specifies the second of two vectors.
	/// @return Cross product of x and y.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<3, T> Cross(const Vec<3, T>& x, const Vec<3, T>& y);

	/// @brief Calculate the cross product.
	/// @tparam T Floating-point scalar types.
	/// @param q1 Specifies the first of two quaternions.
	/// @param q2 Specifies the second of two quaternions.
	/// @return Compute a cross product.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Cross(const tQuat<T>& q1, const tQuat<T>& q2) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the normalized product.
	/// @param x Specifies the vector to normalize.
	/// @return Vector in the same direction as x but with a length of 1.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Normalize(const Vec<L, T>& x);

	/// @brief Calculate the normalized product.
	/// @param q Specifies the quaternion to normalize.
	/// @return Normalized quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Normalize(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve a vector pointing in the same direction as another.
	/// @param N Specifies the vector to orient.
	/// @param I Specifies the incident vector.
	/// @param NRef Specifies the reference vector.
	/// @return If Dot(NRef, I) < 0.0, returns N, otherwise; returns -N.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> FaceForward(const Vec<L, T>& N, const Vec<L, T>& I, const Vec<L, T>& NRef);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the reflection direction for an incident vector.
	/// @param I Specifies the incident vector.
	/// @param N Specifies the normal vector.
	/// @return For the incident vector I and surface orientation N,
	/// returns the reflection direction: result = I - 2.0 * Dot(N, I) * N.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Reflect(const Vec<L, T>& I, const Vec<L, T>& N);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the refraction direction for an incident vector.
	/// @param I Specifies the incident vector.
	/// @param N Specifies the normal vector.
	/// @param eta Specifies the ratio of indices of refraction.
	/// @return For the incident vector I and surface normal N,
	/// and the ratio of indices of refraction eta, returns the refraction vector.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Refract(const Vec<L, T>& I, const Vec<L, T>& N, T eta);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise equal-to comparison of two matrices.
	/// @param x Specifies the first matrix to be used in the comparison operation.
	/// @param y Specifies the second matrix to be used in the comparison operation.
	/// @return Boolean vector which components value is true if this expression is satisfied per column of the matrices.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T> || std::same_as<T, bool>
	[[nodiscard]] constexpr Vec<L, bool> Equal(const Mat<L, L, T>& x, const Mat<L, L, T>& y);

	/// @brief Perform a component-wise equal-to comparison of two matrices taking epsilon into account.
	/// @param x Specifies the first matrix to be used in the comparison operation.
	/// @param y Specifies the second matrix to be used in the comparison operation.
	/// @param epsilon Specifies the epsilon to be used in the comparison operation.
	/// @return Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T> || std::same_as<T, bool>
	[[nodiscard]] constexpr Vec<L, bool> Equal(const Mat<L, L, T>& x, const Mat<L, L, T>& y, T epsilon);

	/// @brief Perform a component-wise equal-to comparison of two matrices taking epsilon into account.
	/// @param x Specifies the first matrix to be used in the comparison operation.
	/// @param y Specifies the second matrix to be used in the comparison operation.
	/// @param epsilon Specifies the epsilon to be used in the comparison operation.
	/// @return Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T> || std::same_as<T, bool>
	[[nodiscard]] constexpr Vec<L, bool> Equal(const Mat<L, L, T>& x, const Mat<L, L, T>& y, const Vec<L, T>& epsilon);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise-not-equal-to comparison of two matrices.
	/// @param x Specifies the first matrix to be used in the comparison operation.
	/// @param y Specifies the second matrix to be used in the comparison operation.
	/// @return Boolean vector which components value is true if this expression is satisfied per column of the matrices.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T> || std::same_as<T, bool>
	[[nodiscard]] constexpr Vec<L, bool> NotEqual(const Mat<L, L, T>& x, const Mat<L, L, T>& y);

	/// @brief Perform a component-wise-not-equal-to comparison of two matrices taking epsilon into account.
	/// @param x Specifies the first matrix to be used in the comparison operation.
	/// @param y Specifies the second matrix to be used in the comparison operation.
	/// @param epsilon Specifies the epsilon to be used in the comparison operation.
	/// @return Component-wise comparison of |x - y| >= epsilon. True if this expression is not satisfied.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T> || std::same_as<T, bool>
	[[nodiscard]] constexpr Vec<L, bool> NotEqual(const Mat<L, L, T>& x, const Mat<L, L, T>& y, T epsilon);

	/// @brief Perform a component-wise-not-equal-to comparison of two matrices taking epsilon into account.
	/// @param x Specifies the first matrix to be used in the comparison operation.
	/// @param y Specifies the second matrix to be used in the comparison operation.
	/// @param epsilon Specifies the epsilon to be used in the comparison operation.
	/// @return Component-wise comparison of |x - y| >= epsilon. True if this expression is not satisfied.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T> || std::same_as<T, bool>
	[[nodiscard]] constexpr Vec<L, bool> NotEqual(const Mat<L, L, T>& x, const Mat<L, L, T>& y, const Vec<L, T>& epsilon);

	//-------------------------------------------------------------------------------------------------------------------//
	//Matrix-------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Multiply matrix x by matrix y component-wise, i.e., result[i][j] is the scalar product of x[i][j] and y[i][j].
	/// @param x Specifies the first matrix multiplicand.
	/// @param y Specifies the second matrix multiplicand.
	/// @return Multiplied matrix.
	template<u32 L, typename T>
	requires (std::floating_point<T> && L > 2)
	[[nodiscard]] constexpr Mat<L, L, T> MatrixCompMult(Mat<L, L, T> x, const Mat<L, L, T>& y);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the outer product of a pair of vectors.
	///
	/// Treats the first parameter c as a column vector and the second parameter r as a row
	/// vector and does a linear algebraic matrix multiply c * r.
	/// @param c Specifies the parameter to be treated as a column matrix.
	/// @param r Specifies the parameter to be treated as a row matrix.
	/// @return Multiplied matrix.
	template<u32 L, typename T>
	requires (std::floating_point<T> && L > 2)
	[[nodiscard]] constexpr Mat<L, L, T> OuterProduct(const Vec<L, T>& c, const Vec<L, T>& r);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the transpose of a matrix.
	/// @param m Specifies the matrix of which to take the transpose.
	/// @return Transposed matrix of m.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<3, 3, T> Transpose(const Mat<3, 3, T>& m);
	/// @brief Calculate the transpose of a matrix.
	/// @param m Specifies the matrix of which to take the transpose.
	/// @return Transposed matrix of m.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Transpose(const Mat<4, 4, T>& m);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the determinant of a matrix.
	/// @param m Specifies the matrix of which to take the determinant.
	/// @return Determinant of a squared matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Determinant(const Mat<3, 3, T>& m);
	/// @brief Calculate the determinant of a matrix.
	/// @param m Specifies the matrix of which to take the determinant.
	/// @return Determinant of a squared matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Determinant(const Mat<4, 4, T>& m);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the inverse of a matrix.
	/// @param m Specifies the matrix of which to take the inverse.
	/// @return Inverse of a squared matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<3, 3, T> Inverse(const Mat<3, 3, T>& m);
	/// @brief Calculate the inverse of a matrix.
	/// @param m Specifies the matrix of which to take the inverse.
	/// @return Inverse of a squared matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Inverse(const Mat<4, 4, T>& m);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a matrix for an orthographic parallel viewing volume.
	/// @return Projection matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Orthographic(T left, T right, T bottom, T top, T zNear, T zFar) noexcept;
	/// @brief Create a matrix for an orthographic parallel viewing volume.
	/// @return Projection matrix with a reversed Z axis.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> OrthographicReverseZ(T left, T right, T bottom, T top, T zNear, T zFar) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Creates a frustum matrix.
	/// @return Frustum matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Frustum(T left, T right, T bottom, T top, T nearVal, T farVal) noexcept;

	/// @brief Creates a frustum matrix.
	/// @return Frustum matrix with a reversed Z axis.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> FrustumReverseZ(T left, T right, T bottom, T top, T nearVal, T farVal) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Creates a matrix for a symmetric perspective-view frustum.
	/// @param fovY Specifies the field of view angle in the y direction. Expressed in radians.
	/// @param aspect Specified the aspect ratio that determines the field of view in the x direction.
	/// The aspect ratio is the ratio of x(width) to y(height).
	/// @param zNear Specifies the distance from the viewer to the near clipping plange (always positive).
	/// @param zFar Specifies the distance from the viewer to the far clipping plane (always positive).
	/// @return Perspective-view matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Perspective(T fovY, T aspect, T zNear, T zFar);

	/// @brief Creates a matrix for a symmetric perspective-view frustum.
	/// @param fovY Specifies the field of view angle in the y direction. Expressed in radians.
	/// @param aspect Specified the aspect ratio that determines the field of view in the x direction.
	/// The aspect ratio is the ratio of x(width) to y(height).
	/// @param zNear Specifies the distance from the viewer to the near clipping plange (always positive).
	/// @param zFar Specifies the distance from the viewer to the far clipping plane (always positive).
	/// @return Perspective-view matrix with a reversed Z axis.
	template <typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> PerspectiveReverseZ(T fovY, T aspect, T zNear, T zFar);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Builds a perspective projection matrix based on a field of view.
	/// @param fov Expressed in radians.
	/// @param width Width of the viewport.
	/// @param height Height of the viewport.
	/// @param zNear Specifies the distance from the viewer to the near clipping plane (always positive).
	/// @param zFar Specifies the distance from the viewer to the far clipping plane (always positive).
	/// @return Field of view based perspective projection matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> PerspectiveFoV(T fov, T width, T height, T zNear, T zFar);

	/// @brief Builds a perspective projection matrix based on a field of view.
	/// @param fov Expressed in radians.
	/// @param width Width of the viewport.
	/// @param height Height of the viewport.
	/// @param zNear Specifies the distance from the viewer to the near clipping plane (always positive).
	/// @param zFar Specifies the distance from the viewer to the far clipping plane (always positive).
	/// @return Field of view based perspective projection matrix with reversed Z axis.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> PerspectiveFoVReverseZ(T fov, T width, T height, T zNear, T zFar);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Creates a matrix for a symmetric perspective-view frustum with far plane at infinite.
	/// @param fovY Specifies the field of view angle, in degrees, in the y direction. Expressed in radians.
	/// @param aspect Specifies the aspect ratio that determines the field of view in the x direction.
	/// The aspect ratio is the ratio of x (width) to y (height).
	/// @param zNear Specifies the distance from the viewer to the near clipping plane (always positive).
	/// @return Symmetric perspective matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> InfinitePerspective(T fovY, T aspect, T zNear);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Linearly interpolate.
	///
	/// If U is a floating scalar or vector:
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.
	/// The value for a is not restricted to the range [0, 1].
	///
	/// If U is a boolean scalar or vector:
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a component of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point
	/// values and will have no effect on the results.
	/// @param x Specify the start of the range in which to interpolate.
	/// @param y Specify the end of the range in which to interpolate.
	/// @param a Specify the value to use to interpolate between x and y.
	/// @return See description.
	template<u32 L, typename T, typename U>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<L, L, T> Mix(const Mat<L, L, T>& x, const Mat<L, L, T>& y, const Mat<L, L, U>& a);

	/// @brief Linearly interpolate.
	///
	/// If U is a floating scalar or vector:
	/// Returns x * (1.0f - 1) + y * a, i.e., the linear blend of x and y using the floating-point value a.
	/// The value for a is not restricted to the range [0, 1].
	///
	/// If U is a boolean scalar or vector:
	/// Select which vector each returned component comes from.
	/// For a component of 'a' that is false, the corresponding component of 'x' is returned.
	/// For a component of 'a' that is true, the corresponding component of 'y' is returned.
	/// Components of 'x' and 'y' that are not selected are allowed to be invalid floating point
	/// values and will have no effect on the results.
	/// @param x Specify the start of the range in which to interpolate.
	/// @param y Specify the end of the range in which to interpolate.
	/// @param a Specify the value to use to interpolate between x and y.
	/// @return See description.
	template<u32 L, typename T, typename U>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<L, L, T> Mix(const Mat<L, L, T>& x, const Mat<L, L, T>& y, U a);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Builds a translation 4 * 4 matrix created from a vector of 3 components.
	/// @param m Input matrix multiplied by this translation matrix.
	/// @param v Coordinates of a translation vector.
	/// @return Translation matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Translate(const Mat<4, 4, T>& m, const Vec<3, T>& v) noexcept;

	/// @brief Transforms a matrix with a translation 4 * 4 matrix created from 3 scalars.
	/// @param v Coordinate of a translation vector.
	/// @return Transformation matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Translate(const Vec<3, T>& v) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Builds a rotation 4 * 4 matrix created from an axis vector and an angle.
	/// @param m Input matrix multiplied by this rotation matrix.
	/// @param angleInRadians Rotation angle expressed in radians.
	/// @param v Rotation axis, recommended to be normalized.
	/// @return Rotation matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Rotate(const Mat<4, 4, T>& m, T angleInRadians, const Vec<3, T>& v);

	/// @brief Builds a rotation 4 * 4 matrix created from an axis of 3 scalars and an angle expressed in radians.
	/// @param angleInRadians Rotation angle expressed in radians.
	/// @param v Scalar
	/// @return Rotation matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Rotate(T angleInRadians, const Vec<3, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Builds a scale 4 * 4 matrix created from 3 scalars.
	/// @param m Input matrix multiplied by this scale matrix.
	/// @param v Ratio of scaling for each axis.
	/// @return Scaled matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Scale(const Mat<4, 4, T>& m, const Vec<3, T>& v) noexcept;

	/// @brief Transforms a matrix with a scale 4 * 4 matrix created from a vector of 3 components.
	/// @param v 3 Component vector.
	/// @return Scaled matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Scale(const Vec<3, T>& v) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Build a look at view matrix.
	/// @param eye Position of the camera.
	/// @param center Position where the camera is looking at.
	/// @param up Normalized up vector, how the camera is oriented. Typically (0, 0, 1).
	/// @return Look at view matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> LookAt(const Vec<3, T>& eye, const Vec<3, T>& center, const Vec<3, T>& up);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Get a specific row of a matrix.
	/// @param m Matrix to retrieve row from.
	/// @param index Row index to retrieve.
	/// @return Row vector.
	template<typename T>
	requires IsMat<T>
	[[nodiscard]] constexpr typename T::row_type Row(const T& m, usize index);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Set a specific row to a matrix.
	/// @param m Matrix to set row for.
	/// @param index Row index to set.
	/// @param x New row data.
	/// @return Matrix with new row data.
	template<typename T>
	requires IsMat<T>
	[[nodiscard]] constexpr T Row(const T& m, usize index, const typename T::row_type& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Get a specific column of a matrix.
	/// @param m Matrix to retrieve column from.
	/// @param index Column index to retrieve.
	/// @return Column vector.
	template<typename T>
	requires IsMat<T>
	[[nodiscard]] constexpr typename T::col_type Column(const T& m, usize index);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Set a specific column to a matrix.
	/// @param m Matrix to set column for.
	/// @param index Column index to set.
	/// @param x New column data.
	/// @return Matrix with new column data.
	template<typename T>
	requires IsMat<T>
	[[nodiscard]] constexpr T Column(T m, usize index, const typename T::col_type& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Decompose a matrix into its position, rotation and scale components.
	/// @param m Matrix to decompose.
	/// @param outPosition Output for the position.
	/// @param outRotation Output for the rotation.
	/// @param outScale Output for the scale.
	/// @return True on successful decompose, false otherwise.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr bool Decompose(Mat<4, 4, T> m, Vec<3, T>& outPosition, tQuat<T>& outRotation, Vec<3, T>& outScale);
	/// @brief Decompose a matrix into its position, rotation (euler angles) and scale components.
	/// @param m Matrix to decompose.
	/// @param outPosition Output for the position.
	/// @param outRotation Output for the rotation (in euler angles, degrees).
	/// @param outScale Output for the scale.
	/// @return True on successful decompose, false otherwise.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr bool Decompose(Mat<4, 4, T> m, Vec<3, T>& outPosition, Vec<3, T>& outRotation, Vec<3, T>& outScale);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Recomopse a matrix from its position, rotation and scale components.
	/// @param position Position to recompose.
	/// @param rotation Rotation to recompose.
	/// @param scale Scale to recompose.
	/// @return Recomposed matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Recompose(const Vec<3, T>& position, const tQuat<T>& rotation, const Vec<3, T>& scale);

	/// @brief Recomopse a matrix from its position, rotation and scale components.
	/// @param position Position to recompose.
	/// @param rotation Rotation (in euler angles, radians) to recompose.
	/// @param scale Scale to recompose.
	/// @return Recomposed matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Recompose(const Vec<3, T>& position, const Vec<3, T>& rotation, const Vec<3, T>& scale);

	//-------------------------------------------------------------------------------------------------------------------//
	//Quaternion---------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Spherical linear interpolation of two quaternions.
	/// @param x A quaternion.
	/// @param y A quaternion.
	/// @param a Interpolation factor. The interpolation is defined beyond the range [0, 1].
	/// @return Interpolated quaternions.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> SLerp(const tQuat<T>& x, const tQuat<T>& y, T a);

	/// @brief Spherical linear interpolation of two quaternions with multiple spins over rotation axis.
	/// The interpolation always takes the short path when the spin count is positive and long path
	/// when count is negative.
	/// Rotation is performed at constant speed.
	///
	/// @param x A quaternion.
	/// @param y A quaternion.
	/// @param a Interpolation factor. The interpolation is defined beyond the range [0, 1].
	/// @param k Additional spin count. If value is negative interpolation will be on "long" path.
	/// @return Interpolated quaternions.
	template<typename T, typename S>
	requires std::floating_point<T> && std::integral<S>
	[[nodiscard]] constexpr tQuat<T> SLerp(const tQuat<T>& x, const tQuat<T>& y, T a, S k);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Conjugate a quaternion.
	/// @param q Quaternion to conjugate.
	/// @return Conjugated quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Conjugate(const tQuat<T>& q) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Calculate the inverse of a quaternion.
	/// @param q Specifies the quaternion of which to take the inverse.
	/// @return Inverted quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Inverse(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve Euler angles from a quaternion.
	/// @param q Quaternion to retrieve Euler angles from.
	/// @return Euler angles, pitch as x, yaw as y, roll as z. The result is expressed in radians.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<3, T> EulerAngles(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve roll from a quaternion.
	/// @param q Quaternion to retrieve Euler angle from.
	/// @return Roll value of euler angles expressed in radians.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Roll(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve pitch from a quaternion.
	/// @param q Quaternion to retrieve Euler angle from.
	/// @return Pitch value of euler angles expressed in radians.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Pitch(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve yaw from a quaternion.
	/// @param q Quaternion to retrieve Euler angle from.
	/// @return Yaw value of euler angles expressed in radians.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Yaw(const tQuat<T>& q);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Converts a quaternion to a 3 * 3 matrix.
	/// @param q Quaternion to convert.
	/// @return Converted matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<3, 3, T> Mat3Cast(const tQuat<T>& q) noexcept;
	/// @brief Converts a quaternion to a 4 * 4 matrix.
	/// @param q Quaternion to convert.
	/// @return Converted matrix.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Mat<4, 4, T> Mat4Cast(const tQuat<T>& q) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Converts a pure rotation 3 * 3 matrix to a quaternion.
	/// @param m Pure rotation matrix to convert.
	/// @return Converted quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> QuaternionCast(const Mat<3, 3, T>& m);
	/// @brief Converts a pure rotation 4 * 4 matrix to a quaternion.
	/// @param m Pure rotation matrix to convert.
	/// @return Converted quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> QuaternionCast(const Mat<4, 4, T>& m);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise less-than comparison of two quaternions.
	/// @param x Specifies the first quaternion to be used in the comparison operation.
	/// @param y Specifies the second quaternion to be used in the comparison operation.
	/// @return Component-wise comparison result of x < y.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, bool> LessThan(const tQuat<T>& x, const tQuat<T>& y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise less-than-or-equal comparison of two quaternions.
	/// @param x Specifies the first quaternion to be used in the comparison operation.
	/// @param y Specifies the second quaternion to be used in the comparison operation.
	/// @return Component-wise comparison result of x <= y.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, bool> LessThanEqual(const tQuat<T>& x, const tQuat<T>& y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise greater-than comparison of two quaternions.
	/// @param x Specifies the first quaternion to be used in the comparison operation.
	/// @param y Specifies the second quaternion to be used in the comparison operation.
	/// @return Component-wise comparison result of x > y.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, bool> GreaterThan(const tQuat<T>& x, const tQuat<T>& y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise greater-than-or-equal comparison of two quaternions.
	/// @param x Specifies the first quaternion to be used in the comparison operation.
	/// @param y Specifies the second quaternion to be used in the comparison operation.
	/// @return Component-wise comparison result of x >= y.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, bool> GreaterThanEqual(const tQuat<T>& x, const tQuat<T>& y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Build a look at quaternion.
	/// @param direction Desired forward direction. Needs to be normalized.
	/// @param up Up vector, how the camera is oriented. Typically (0, 1, 0).
	/// @return Look at quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> QuaternionLookAt(const Vec<3, T>& direction, const Vec<3, T>& up);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise equal-to comparison of two quaternions.
	/// @param x Specifies the first quaternion to be used in the comparison operation.
	/// @param y Specifies the second quaternion to be used in the comparison operation.
	/// @return Component-wise comparison of result x == y.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, bool> Equal(const tQuat<T>& x, const tQuat<T>& y) noexcept;

	/// @brief Perform a component-wise equal-to comparison of two quaternions taking epsilon into account.
	/// @param x Specifies the first quaternion to be used in the comparison operation.
	/// @param y Specifies the second quaternion to be used in the comparison operation.
	/// @param epsilon
	/// @return Component-wise comparison of |x - y| < epsilon.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, bool> Equal(const tQuat<T>& x, const tQuat<T>& y, T epsilon);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise not-equal-to comparison of two quaternions.
	/// @param x Specifies the first quaternion to be used in the comparison operation.
	/// @param y Specifies the second quaternion to be used in the comparison operation.
	/// @return Component-wise comparison of result x != y.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, bool> NotEqual(const tQuat<T>& x, const tQuat<T>& y) noexcept;

	/// @brief Perform a component-wise not-equal-to comparison of two quaternions taking epsilon into account.
	/// @param x Specifies the first quaternion to be used in the comparison operation.
	/// @param y Specifies the second quaternion to be used in the comparison operation.
	/// @param epsilon
	/// @return Component-wise comparison of |x - y| >= epsilon.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<4, bool> NotEqual(const tQuat<T>& x, const tQuat<T>& y, T epsilon);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the rotation angle of a quaternion.
	/// @param x Quaternion.
	/// @return Quaternion rotation angle.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Angle(const tQuat<T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the rotation axis of a quaternion.
	/// @param x Quaternion.
	/// @return Rotation axis.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<3, T> Axis(const tQuat<T>& x);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Build a quaternion from an angle and a normalized axis.
	/// @param angle Angle expressed in radians.
	/// @param v Axis of the quaternion, must be normalized.
	/// @return Quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> AngleAxis(const T& angle, const Vec<3, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Rotates a quaternion from a vector of 3 components axis and an angle.
	/// @param q Source orientation.
	/// @param angle Angle expressed in radians.
	/// @param v Axis of the rotation.
	/// @return Rotated quaternion.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr tQuat<T> Rotate(const tQuat<T>& q, const T& angle, const Vec<3, T>& v);

	//-------------------------------------------------------------------------------------------------------------------//
	//Vector-------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve a normalized x-axis rotation vector (1, 0, 0).
	/// @return Normalized rotation vector for the x-axis.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<3, T> XAxis() noexcept;

	/// @brief Retrieve a normalized y-axis rotation vector (0, 1, 0).
	/// @return Normalized rotation vector for the y-axis.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<3, T> YAxis() noexcept;

	/// @brief Retrieve a normalized z-axis rotation vector (0, 0, 1).
	/// @return Normalized rotation vector for the z-axis.
	template<typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<3, T> ZAxis() noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise less-than comparison of two vectors.
	/// @param x Specifies the first vector to be used in the comparison operation.
	/// @param y Specifies the second vector to be used in the comparison operation.
	/// @return Component-wise comparison result of x < y.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, bool> LessThan(const Vec<L, T>& x, const Vec<L, T>& y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise less-than-or-equal comparison of two vectors.
	/// @param x Specifies the first vector to be used in the comparison operation.
	/// @param y Specifies the second vector to be used in the comparison operation.
	/// @return Component-wise comparison result of x <= y.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, bool> LessThanEqual(const Vec<L, T>& x, const Vec<L, T>& y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise greater-than comparison of two vectors.
	/// @param x Specifies the first vector to be used in the comparison operation.
	/// @param y Specifies the second vector to be used in the comparison operation.
	/// @return Component-wise comparison result of x > y.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, bool> GreaterThan(const Vec<L, T>& x, const Vec<L, T>& y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise greater-than-or-equal comparison of two vectors.
	/// @param x Specifies the first vector to be used in the comparison operation.
	/// @param y Specifies the second vector to be used in the comparison operation.
	/// @return Component-wise comparison result of x >= y.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, bool> GreaterThanEqual(const Vec<L, T>& x, const Vec<L, T>& y) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise equal-to comparison of two vectors.
	/// @param x Specifies the first vector to be used in the comparison operation.
	/// @param y Specifies the second vector to be used in the comparison operation.
	/// @return Component-wise comparison result of x == y.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, bool> Equal(const Vec<L, T>& x, const Vec<L, T>& y) noexcept;

	/// @brief Perform a component-wise equal-to comparison of two vectors taking epsilon into account.
	/// @param x Specifies the first vector to be used in the comparison operation.
	/// @param y Specifies the second vector to be used in the comparison operation.
	/// @param epsilon
	/// @return Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, bool> Equal(const Vec<L, T>& x, const Vec<L, T>& y, T epsilon);

	/// @brief Perform a component-wise equal-to comparison of two vectors taking epsilon into account.
	/// @param x Specifies the first vector to be used in the comparison operation.
	/// @param y Specifies the second vector to be used in the comparison operation.
	/// @param epsilon
	/// @return Component-wise comparison of |x - y| < epsilon. True if this expression is satisfied.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, bool> Equal(const Vec<L, T>& x, const Vec<L, T>& y, const Vec<L, T>& epsilon);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Perform a component-wise not-equal-to comparison of two vectors.
	/// @param x Specifies the first vector to be used in the comparison operation.
	/// @param y Specifies the second vector to be used in the comparison operation.
	/// @return Component-wise comparison result of x != y.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, bool> NotEqual(const Vec<L, T>& x, const Vec<L, T>& y) noexcept;

	/// @brief Perform a component-wise not-equal-to comparison of two vectors taking epsilon into account.
	/// @param x Specifies the first vector to be used in the comparison operation.
	/// @param y Specifies the second vector to be used in the comparison operation.
	/// @param epsilon
	/// @return Component-wise comparison result of x != y.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, bool> NotEqual(const Vec<L, T>& x, const Vec<L, T>& y, T epsilon);

	/// @brief Perform a component-wise not-equal-to comparison of two vectors taking epsilon into account.
	/// @param x Specifies the first vector to be used in the comparison operation.
	/// @param y Specifies the second vector to be used in the comparison operation.
	/// @param epsilon
	/// @return Component-wise comparison result of x != y.
	template<u32 L, typename T>
	requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr Vec<L, bool> NotEqual(const Vec<L, T>& x, const Vec<L, T>& y, const Vec<L, T>& epsilon);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check whether any element of a boolean vector is true.
	/// @param v Specifies the vector to be tested for truth.
	/// @return True if any component of v is true.
	template<u32 L>
	[[nodiscard]] constexpr bool Any(const Vec<L, bool>& v) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check whether all elements of a boolean vector are true.
	/// @param v Specifies the vector to be tested for truth.
	/// @return True if all component of v are true.
	template<u32 L>
	[[nodiscard]] constexpr bool All(const Vec<L, bool>& v) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Logically invert a boolean vector.
	/// @param v Specifies the vector to be inverted.
	/// @return Component-wise logical complement of x.
	template<u32 L>
	[[nodiscard]] constexpr Vec<L, bool> Not(Vec<L, bool> v) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//
	//Trigonometric------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Converts degrees to radians and returns the result.
	/// @param degrees Specify the quantity, in degrees, to be converted to radians.
	/// @return Radians.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType Radians(genType degrees);

	/// @brief Converts degrees to radians and returns the result.
	/// @param v Specify the quantity, in degrees, to be converted to radians.
	/// @return Radians.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Radians(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Converts radians to degrees and returns the result.
	/// @param radians Specify the quantity, in radians, to be converted to degrees.
	/// @return Degrees.
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType Degrees(genType radians);

	/// @brief Converts radians to degrees and returns the result.
	/// @param v Specify the quantity, in radians, to be converted to degrees.
	/// @return Degrees.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Degrees(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Standard trigonometric sine function.
	/// Values returned by this function will range from [-1, 1].
	/// @param x Specify the quantity, in radians, of which to return the sine.
	/// @return Sine.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Sin(T x);

	/// @brief Standard trigonometric sine function.
	/// Values returned by this function will range from [-1, 1].
	/// @param v Specify the quantity, in radians, of which to return the sine.
	/// @return Sine.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Sin(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Standard trigonometric cosine function.
	/// Values returned by this function will range from [-1, 1].
	/// @param x Specify the quantity, in radians, of which to return the cosine.
	/// @return Cosine.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Cos(T x);

	/// @brief Standard trigonometric cosine function.
	/// Values returned by this function will range from [-1, 1].
	/// @param v Specify the quantity, in radians, of which to return the cosine.
	/// @return Cosine.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Cos(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Standard trigonometric tangent function.
	/// @param x Specify the quantity, in radians, of which to return the tangent.
	/// @return Tangent.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T Tan(T x);

	/// @brief Standard trigonometric tangent function.
	/// @param v Specify the quantity, in radians, of which to return the tangent.
	/// @return Tangent.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> Tan(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Arc sine.
	/// The range of values returned by this function is [-PI / 2, PI / 2].
	/// @param x Specify the value whose arcsine to return.
	/// @return Angle whose sine is x.
	/// @note Results are undefined if |x| > 1!
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T ASin(T x);

	/// @brief Arc sine.
	/// The range of values returned by this function is [-PI / 2, PI / 2].
	/// @param v Specify the value whose arcsine to return.
	/// @return Angle whose sine is x.
	/// @note Results are undefined if |x| > 1!
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ASin(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Arc cosine.
	/// The range of values returned by this function is [0, PI].
	/// @param x Specify the value whose arccosine to return.
	/// @return Angle whose cosine is x.
	/// @note Results are undefined if |x| > 1!
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T ACos(T x);

	/// @brief Arc cosine.
	/// The range of values returned by this function is [0, PI].
	/// @param v Specify the value whose arccosine to return.
	/// @return Angle whose cosine is x.
	/// @note Results are undefined if |x| > 1!
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ACos(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Arc tangent.
	/// The range of values returned by this function is [-PI, PI].
	/// @param y Specify the numerator of the fraction whose arctangent to return.
	/// @param x Specify the denominator of the fraction whose arctangent to return.
	/// @return
	/// Angle whose tangent is y / x.
	/// The signs of x and y are used to determine what quadrant the angle is in.
	/// @note Results are undefined if x and y are both 0!
	template<typename genType>
	requires std::floating_point<genType>
	[[nodiscard]] constexpr genType ATan(genType y, genType x);

	/// @brief Arc tangent.
	/// The range of values returned by this function is [-PI, PI].
	/// @param y Specify the numerator of the fraction whose arctangent to return.
	/// @param x Specify the denominator of the fraction whose arctangent to return.
	/// @return
	/// Angle whose tangent is y / x.
	/// The signs of x and y are used to determine what quadrant the angle is in.
	/// @note Results are undefined if x and y are both 0!
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ATan(Vec<L, T> y, const Vec<L, T>& x);

	/// @brief Arc tangent.
	/// The range of values returned by this function is [-PI / 2, PI / 2].
	/// @param x Specify the fraction whose arctangent to return.
	/// @return Angle whose tangent is y over x.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T ATan(T x);

	/// @brief Arc tangent.
	/// The range of values returned by this function is [-PI / 2, PI / 2].
	/// @param v Specify the fraction whose arctangent to return.
	/// @return Angle whose tangent is y over x.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ATan(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the hyperbolic sine of the parameter.
	/// @param x Specify the value whose hyperbolic sine to return.
	/// @return Hyperbolic sine function, (Exp(x) - Exp(-x)) / 2.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T SinH(T x);

	/// @brief Retrieve the hyperbolic sine of the parameter.
	/// @param v Specify the value whose hyperbolic sine to return.
	/// @return Hyperbolic sine function, (Exp(x) - Exp(-x)) / 2.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> SinH(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the hyperbolic cosine of the parameter.
	/// @param x Specify the value whose hyperbolic cosine to return.
	/// @return Hyperbolic cosine function, (Exp(x) + Exp(-x)) / 2.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T CosH(T x);

	/// @brief Retrieve the hyperbolic cosine of the parameter.
	/// @param v Specify the value whose hyperbolic cosine to return.
	/// @return Hyperbolic cosine function, (Exp(x) + Exp(-x)) / 2.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> CosH(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the hyperbolic tangent of the parameter.
	/// @param x Specify the value whose hyperbolic tangent to return.
	/// @return Hyperbolic tangent function, SinH(angle) / CosH(angle).
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T TanH(T x);

	/// @brief Retrieve the hyperbolic tangent of the parameter.
	/// @param v Specify the value whose hyperbolic tangent to return.
	/// @return Hyperbolic tangent function, SinH(angle) / CosH(angle).
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> TanH(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Ary hyperbolic sine.
	/// @param x Specify the value whose arc hyperbolic sine to return.
	/// @return Inverse of SinH.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T ASinH(T x);

	/// @brief Ary hyperbolic sine.
	/// @param v Specify the value whose arc hyperbolic sine to return.
	/// @return Inverse of SinH.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ASinH(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Arc hyperbolic cosine.
	/// @param x Specify the value whose arc hyperbolic cosine to return.
	/// @return Non-negative inverse of CosH.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T ACosH(T x);

	/// @brief Arc hyperbolic cosine.
	/// @param v Specify the value whose arc hyperbolic cosine to return.
	/// @return Non-negative inverse of CosH.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ACosH(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Arc hyperbolic tangent.
	/// @param x Specify the value whose arc hyperbolic tangent to return.
	/// @return Inverse of TanH.
	/// @note Results are undefined if Abs(x) >= 1!
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr T ATanH(T x);

	/// @brief Arc hyperbolic tangent.
	/// @param v Specify the value whose arc hyperbolic tangent to return.
	/// @return Inverse of TanH.
	/// @note Results are undefined if Abs(x) >= 1!
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ATanH(Vec<L, T> v);

	//-------------------------------------------------------------------------------------------------------------------//
	//Color space--------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert a linear color to sRGB color using a standard gamma correction.
	/// IEC 61966-2-1:1999 / Rec. 709 specification https://www.w3.org/Graphics/Color/srgb
	/// @param colorLinear Color in linear space.
	/// @return Color in sRGB space.
	template<u32 L, typename T>
	requires (L == 3 || L == 4) && std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ConvertLinearToSRGB(const Vec<L, T>& colorLinear);

	/// @brief Convert a linear color to sRGB color using a custom gamma correction.
	/// IEC 61966-2-1:1999 / Rec. 709 specification https://www.w3.org/Graphics/Color/srgb
	/// @param colorLinear Color in linear space.
	/// @param gamma Gamma correction value to use. Typically 0.41666 is used.
	/// @return Color in sRGB space.
	template<u32 L, typename T>
	requires (L == 3 || L == 4) && std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ConvertLinearToSRGB(const Vec<L, T>& colorLinear, T gamma);

	/// @brief Convert a sRGB color to linear color using a standard gamma correction.
	/// IEC 61966-2-1:1999 / Rec. 709 specification https://www.w3.org/Graphics/Color/srgb
	/// @param colorSRGB Color in sRGB space.
	/// @return Color in linear space.
	template<u32 L, typename T>
	requires (L == 3 || L == 4) && std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ConvertSRGBToLinear(const Vec<L, T>& colorSRGB);

	/// @brief Convert a sRGB color to linear color using a custom gamma correction.
	/// IEC 61966-2-1:1999 / Rec. 709 specification https://www.w3.org/Graphics/Color/srgb
	/// @param colorSRGB Color in sRGB space.
	/// @param gamma Gamma correction value to use. Typically 2.4 is used.
	/// @return Color in linear space.
	template<u32 L, typename T>
	requires (L == 3 || L == 4) && std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, T> ConvertSRGBToLinear(const Vec<L, T>& colorSRGB, T gamma);

	//-------------------------------------------------------------------------------------------------------------------//
	//Other--------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Returns whether the given integer is odd or not.
	/// @param x Integer to check.
	/// @return True if the given integer is odd, false otherwise.
	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr bool IsOdd(T x);

	/// @brief Returns whether the given integers are odd or not.
	/// @param x Integers to check.
	/// @return Vector containing True if a given integer is odd, false otherwise.
	template<u32 L, typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<L, bool> IsOdd(const Vec<L, T>& x) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Returns whether the given integer is even or not.
	/// @param x Integer to check.
	/// @return True if the given integer is even, false otherwise.
	template<typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr bool IsEven(T x);

	/// @brief Returns whether the given integers are even or not.
	/// @param x Integers to check.
	/// @return Vector containing True if a given integer is even, false otherwise.
	template<u32 L, typename T>
	requires std::integral<T>
	[[nodiscard]] constexpr Vec<L, bool> IsEven(const Vec<L, T>& x) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Returns whether the given number is finite or not.
	/// @param x Number to check.
	/// @return True if the given number is finite, false otherwise.
	template<typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr bool IsFinite(T x) noexcept;

	/// @brief Returns whether the given numbers are finite or not.
	/// @param x Numbers to check.
	/// @return True if a given number is finite, false otherwise.
	template<u32 L, typename T>
	requires std::floating_point<T>
	[[nodiscard]] constexpr Vec<L, bool> IsFinite(const Vec<L, T>& x) noexcept;
}

//-------------------------------------------------------------------------------------------------------------------//
//Common-------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::is_arithmetic_v<genType>
[[nodiscard]] constexpr genType TRAP::Math::Min(const genType x, const genType y)
{
	return (y < x) ? y : x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::is_arithmetic_v<genType>
[[nodiscard]] constexpr genType TRAP::Math::Min(const genType x, const genType y, const genType z)
{
	return Min(Min(x, y), z);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::is_arithmetic_v<genType>
[[nodiscard]] constexpr genType TRAP::Math::Min(const genType x, const genType y, const genType z, const genType w)
{
	return Min(Min(x, y), Min(z, w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::is_arithmetic_v<genType>
[[nodiscard]] constexpr genType TRAP::Math::Max(const genType x, const genType y)
{
	return (x < y) ? y : x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::is_arithmetic_v<genType>
[[nodiscard]] constexpr genType TRAP::Math::Max(const genType x, const genType y, const genType z)
{
	return Max(Max(x, y), z);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::is_arithmetic_v<genType>
[[nodiscard]] constexpr genType TRAP::Math::Max(const genType x, const genType y, const genType z, const genType w)
{
	return Max(Max(x, y), Max(z, w));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::Round(const genType x)
{
	if(std::is_constant_evaluated())
		return gcem::round(x);

	return std::round(x);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::Trunc(const genType x)
{
	if(std::is_constant_evaluated())
		return gcem::trunc(x);

	return std::trunc(x);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genFIType>
requires std::signed_integral<genFIType> || std::floating_point<genFIType>
[[nodiscard]] constexpr genFIType TRAP::Math::Abs(const genFIType x)
{
	if constexpr (std::signed_integral<genFIType>)
	{
		const genFIType y = x >> (sizeof(genFIType) * 8u - 1u);
		return (x ^ y) - y;
	}
	else
	{
		return x >= static_cast<genFIType>(0) ? x : -x;
	}
}

template<u32 L, typename T>
requires std::signed_integral<T> || std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Abs(Vec<L, T> x)
{
	for(T& n : x)
		n = Abs(n);

	return x;
}

template<u32 C, u32 R, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<C, R, T> TRAP::Math::Abs(Mat<C, R, T> x)
{
	for(auto& col : x)
	{
		for(T& n : col)
			n = Abs(n);
	}

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genFIType>
requires std::floating_point<genFIType> || std::signed_integral<genFIType>
[[nodiscard]] constexpr genFIType TRAP::Math::Sign(const genFIType x)
{
	return static_cast<genFIType>(static_cast<genFIType>(static_cast<genFIType>(0) < x) -
	                              static_cast<genFIType>(x < static_cast<genFIType>(0)));
}

template<u32 L, typename T>
requires std::floating_point<T> || std::signed_integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Sign(const Vec<L, T>& x)
{
	return Vec<L, T>(LessThan(Vec<L, T>(static_cast<T>(0)), x)) -
	       Vec<L, T>(LessThan(x, Vec<L, T>(static_cast<T>(0))));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Floor(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::floor(x);

	return std::floor(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Floor(Vec<L, T> x)
{
	for(T& n : x)
		n = Floor(n);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Trunc(Vec<L, T> x)
{
	for(T& n : x)
		n = Trunc(n);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------/

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Round(Vec<L, T> x)
{
	for(T& n : x)
		n = Round(n);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::RoundEven(const genType x)
{
	TRAP_ASSERT(!IsNaN(x), "Math::RoundEven(): x is NaN!");
	TRAP_ASSERT(!IsInf(x), "Math::RoundEven(): x is Inf!");

	const i32 integer = static_cast<i32>(x);
	const genType integerPart = static_cast<genType>(integer);
	const genType fractionalPart = Fract(x);

	if (fractionalPart > static_cast<genType>(0.5) || fractionalPart < static_cast<genType>(0.5))
		return Round(x);
	if ((integer % 2) == 0)
		return integerPart;
	if (x <= static_cast<genType>(0))
		return integerPart - static_cast<genType>(1);

	return integerPart + static_cast<genType>(1);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::RoundEven(Vec<L, T> x)
{
	for(T& n : x)
		n = RoundEven(n);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Ceil(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::ceil(x);

	return std::ceil(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Ceil(Vec<L, T> x)
{
	for(T& n : x)
		n = Ceil(n);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::Fract(const genType x)
{
	return x - Floor(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Fract(const Vec<L, T>& x)
{
	return x - Floor(x);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::Mod(const genType x, const genType y)
{
	if(std::is_constant_evaluated())
		return gcem::fmod(x, y);

	return std::fmod(x, y);
}

template<typename genType>
requires std::signed_integral<genType>
[[nodiscard]] constexpr genType TRAP::Math::Mod(const genType x, const genType y)
{
	return static_cast<genType>(static_cast<genType>(static_cast<genType>(x % y) + y) % y);
}

template<typename genType>
requires std::unsigned_integral<genType>
[[nodiscard]] constexpr genType TRAP::Math::Mod(const genType x, const genType y)
{
	return static_cast<genType>(x - y * (x / y));
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Mod(Vec<L, T> x, const T y)
{
	for(T& n : x)
		n = Mod(n, y);

	return x;
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Mod(Vec<L, T> x, const Vec<L, T>& y)
{
	for(u32 i = 0; i < L; ++i)
		x[i] = Mod(x[i], y[i]);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] genType TRAP::Math::Modf(const genType x, genType& i)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return std::modf(x, &i);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] TRAP::Math::Vec<L, T> TRAP::Math::Modf(Vec<L, T> x, Vec<L, T>& i)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	for (u32 j = 0u; j < L; j++)
		x[j] = std::modf(x[j], &i[j]);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Min(Vec<L, T> a, const T b)
{
	for(T& n : a)
		n = Min(n, b);

	return a;
}

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Min(Vec<L, T> a, const Vec<L, T>& b)
{
	for (u32 i = 0u; i < L; i++)
		a[i] = Min(a[i], b[i]);

	return a;
}

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Min(const Vec<L, T>&a, const Vec<L, T>& b, const Vec<L, T>& c)
{
	return Min(Min(a, b), c);
}

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Min(const Vec<L, T>&a, const Vec<L, T>& b, const Vec<L, T>& c,
                                                              const Vec<L, T>& d)
{
	return Min(Min(a, b), Min(c, d));
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Max(Vec<L, T> a, const T b)
{
	for(T& n : a)
		n = Max(n, b);

	return a;
}

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Max(Vec<L, T> a, const Vec<L, T>& b)
{
	for (u32 i = 0u; i < L; i++)
		a[i] = Max(a[i], b[i]);

	return a;
}

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Max(const Vec<L, T>& a, const Vec<L, T>& b, const Vec<L, T>& c)
{
	return Max(Max(a, b), c);
}

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Max(const Vec<L, T>& a, const Vec<L, T>& b, const Vec<L, T>& c,
                                                              const Vec<L, T>& d)
{
	return Max(Max(a, b), Max(c, d));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::is_arithmetic_v<genType>
[[nodiscard]]
#ifndef TRAP_ENABLE_ASSERTS
constexpr
#endif /*TRAP_ENABLE_ASSERTS*/
genType TRAP::Math::Clamp(const genType x, const genType minVal, const genType maxVal)
{
	TRAP_ASSERT(minVal <= maxVal, "TRAP::Math::Clamp(): minVal is greater than maxVal!");

	return Min(Max(x, minVal), maxVal);
}

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]]
#ifndef TRAP_ENABLE_ASSERTS
constexpr
#endif /*TRAP_ENABLE_ASSERTS*/
TRAP::Math::Vec<L, T> TRAP::Math::Clamp(const Vec<L, T>& x, const T minVal, const T maxVal)
{
	TRAP_ASSERT(minVal <= maxVal, "TRAP::Math::Clamp(): minVal is greater than maxVal!");

	return Vec<L, T>(Min(Max(x, Vec<L, T>(minVal)), Vec<L, T>(maxVal)));
}

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]]
#ifndef TRAP_ENABLE_ASSERTS
constexpr
#endif /*TRAP_ENABLE_ASSERTS*/
TRAP::Math::Vec<L, T> TRAP::Math::Clamp(const Vec<L, T>& x, const Vec<L, T>& minVal,
                                        const Vec<L, T>& maxVal)
{
	TRAP_ASSERT(TRAP::Math::All(TRAP::Math::LessThan(minVal, maxVal)), "TRAP::Math::Clamp(): minVal is greater than maxVal!");

	return Vec<L, T>(Min(Max(x, minVal), maxVal));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genTypeT, typename genTypeU>
requires std::floating_point<genTypeU> || std::same_as<genTypeU, bool>
[[nodiscard]] constexpr genTypeT TRAP::Math::Mix(const genTypeT x, const genTypeT y, const genTypeU a)
{
	if constexpr (std::same_as<genTypeU, bool>)
		return a ? y : x;
	else
	{
		return static_cast<genTypeT>(static_cast<genTypeU>(x) *
									(static_cast<genTypeU>(1) - a) + static_cast<genTypeU>(y) * a);

	}
}

template<u32 L, typename T, typename U>
requires std::floating_point<U> || std::same_as<U, bool>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Mix(const Vec<L, T>& x, const Vec<L, T>& y, U a)
{
	if constexpr (std::same_as<U, bool>)
		return a ? y : x;
	else
	{
		return Vec<L, T>(Vec<L, U>(x) * (static_cast<U>(1) - a) + Vec<L, U>(y) * a);
	}
}

template<u32 L, typename T, typename U>
requires std::floating_point<U> || std::same_as<U, bool>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Mix(const Vec<L, T>& x, const Vec<L, T>& y, const Vec<L, U>& a)
{
	if constexpr (std::same_as<U, bool>)
	{
		Vec<L, T> result{};
		for (u32 i = 0; i < L; ++i)
			result[i] = a[i] ? y[i] : x[i];
		return result;
	}
	else
	{
		return Vec<L, T>(Vec<L, U>(x) * (static_cast<U>(1) - a) + Vec<L, U>(y) * a);
	}
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Mix(const tQuat<T>& x, const tQuat<T>& y, const T a)
{
	const T cosTheta = Dot(x, y);

	//Perform a linear interpolation when cosTheta is close to 1
	//to avoid side effect of Sin(angle) becoming a zero denominator
	if(cosTheta > static_cast<T>(1) - Epsilon<T>())
	{
		//Linear interpolation
		return tQuat<T>(Mix(x.w(), y.w(), a), Mix(x.x(), y.x(), a), Mix(x.y(), y.y(), a), Mix(x.z(), y.z(), a));
	}

	const T angle = ACos(cosTheta);
	return (Sin((static_cast<T>(1) - a) * angle) * x + Sin(a * angle) * y) / Sin(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool TRAP::Math::Equal(const T x, const T y) noexcept
{
	return x == y;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool TRAP::Math::Equal(const T x, const T y, const T epsilon)
{
	if constexpr(std::signed_integral<T> || std::floating_point<T>)
	{
		return Abs(static_cast<T>(x - y)) <= epsilon;
	}
	else if constexpr(std::unsigned_integral<T>)
	{
		return static_cast<T>(x - y) <= epsilon;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool TRAP::Math::NotEqual(const T x, const T y) noexcept
{
	return x != y;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool TRAP::Math::NotEqual(const T x, const T y, const T epsilon)
{
	if constexpr(std::signed_integral<T> || std::floating_point<T>)
	{
		return Abs(static_cast<T>(x - y)) > epsilon;
	}
	else if constexpr(std::unsigned_integral<T>)
	{
		return static_cast<T>(x - y) > epsilon;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool TRAP::Math::GreaterThan(const T x, const T y) noexcept
{
	return x > y;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool TRAP::Math::GreaterThanEqual(const T x, const T y) noexcept
{
	return x >= y;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool TRAP::Math::LessThan(const T x, const T y) noexcept
{
	return x < y;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool TRAP::Math::LessThanEqual(const T x, const T y) noexcept
{
	return x <= y;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Math::Not(const bool v) noexcept
{
	return !v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::Step(const genType edge, const genType x)
{
	return Mix(static_cast<genType>(1), static_cast<genType>(0), x < edge);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Step(const T edge, const Vec<L, T>& x)
{
	return Mix(Vec<L, T>(static_cast<T>(1)), Vec<L, T>(static_cast<T>(0)), LessThan(x, Vec<L, T>(edge)));
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Step(const Vec<L, T>& edge, const Vec<L, T>& x)
{
	return Mix(Vec<L, T>(static_cast<T>(1)), Vec<L, T>(static_cast<T>(0)), LessThan(x, edge));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::SmoothStep(const genType edge0, const genType edge1, const genType x)
{
	const genType tmp(Clamp((x - edge0) / (edge1 - edge0), static_cast<genType>(0), static_cast<genType>(1)));

	return tmp * tmp * (static_cast<genType>(3) - static_cast<genType>(2) * tmp);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::SmoothStep(const T edge0, const T edge1, const Vec<L, T>& x)
{
	const Vec<L, T> tmp(Clamp((x - Vec<L, T>(edge0)) / (Vec<L, T>(edge1) - Vec<L, T>(edge0)),
	                    static_cast<T>(0), static_cast<T>(1)));

	return tmp * tmp * (static_cast<T>(3) - static_cast<T>(2) * tmp);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::SmoothStep(const Vec<L, T>& edge0, const Vec<L, T>& edge1, const Vec<L, T>& x)
{
	const Vec<L, T> tmp(Clamp((x - edge0) / (edge1 - edge0), static_cast<T>(0), static_cast<T>(1)));

	return tmp * tmp * (static_cast<T>(3) - static_cast<T>(2) * tmp);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr bool TRAP::Math::IsNaN(const genType x) noexcept
{
	return x != x;
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::IsNaN(const Vec<L, T>& v)
{
	Vec<L, bool> result;
	for (u32 i = 0u; i < L; ++i)
		result[i] = TRAP::Math::IsNaN(v[i]);
	return result;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, bool> TRAP::Math::IsNaN(const tQuat<T>& q)
{
	return Vec<4, bool>(TRAP::Math::IsNaN(q.x()), TRAP::Math::IsNaN(q.y()), TRAP::Math::IsNaN(q.z()), TRAP::Math::IsNaN(q.w()));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr bool TRAP::Math::IsInf(const genType x) noexcept
{
	return x == std::numeric_limits<decltype(x)>::infinity() || x == -std::numeric_limits<decltype(x)>::infinity();
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::IsInf(const Vec<L, T>& v)
{
	Vec<L, bool> result;
	for (u32 i = 0u; i < L; ++i)
		result[i] = TRAP::Math::IsInf(v[i]);
	return result;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, bool> TRAP::Math::IsInf(const tQuat<T>& q)
{
	return Vec<4, bool>(TRAP::Math::IsInf(q.x()), TRAP::Math::IsInf(q.y()), TRAP::Math::IsInf(q.z()), TRAP::Math::IsInf(q.w()));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] genType TRAP::Math::FMA(const genType a, const genType b, const genType c)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return std::fma(a, b, c);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] genType TRAP::Math::FrExp(const genType x, i32& exp)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return std::frexp(x, &exp);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] TRAP::Math::Vec<L, T> TRAP::Math::FrExp(Vec<L, T> v, Vec<L, i32>& exp)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	for (u32 i = 0u; i < L; ++i)
		v[i] = std::frexp(v[i], &exp[i]);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] genType TRAP::Math::LdExp(const genType& x, const i32& exp)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return std::ldexp(x, exp);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] TRAP::Math::Vec<L, T> TRAP::Math::LdExp(Vec<L, T> v, const Vec<L, i32>& exp)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	for (u32 i = 0u; i < L; ++i)
		v[i] = std::ldexp(v[i], exp[i]);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::unsigned_integral<genType>
[[nodiscard]] constexpr bool TRAP::Math::IsPowerOfTwo(const genType value)
{
	return std::has_single_bit(value);
}

template<u32 L, typename T>
requires std::unsigned_integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::IsPowerOfTwo(const Vec<L, T>& v)
{
	TRAP::Math::Vec<L, bool> res{};
	for(u32 i = 0; i < L; ++i)
		res[i] = std::has_single_bit(v[i]);

	return res;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::FMod(const T x, const T y)
{
	if(std::is_constant_evaluated())
		return gcem::fmod(x, y);

	return std::fmod(x, y);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::FMod(Vec<L, T> x, const T y)
{
	for(T& n : x)
		n = FMod(n, y);

	return x;
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::FMod(Vec<L, T> x, const Vec<L, T>& y)
{
	for (u32 i = 0u; i < L; i++)
		x[i] = FMod(x[i], y[i]);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Lerp(const T x, const T y, const T a)
{
	return Mix(x, y, a);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Lerp(const Vec<L, T>& x, const Vec<L, T>& y, T a)
{
	return Mix(x, y, a);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Lerp(const Vec<L, T>& x, const Vec<L, T>& y, const Vec<L, T>& a)
{
	return Mix(x, y, a);
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Lerp(const tQuat<T>& x, const tQuat<T>& y, T a)
{
	return x * (static_cast<T>(1) - static_cast<T>(a)) + (y * a);
}

//-------------------------------------------------------------------------------------------------------------------//
//Exponential--------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Pow(const T base, const T exponent)
{
	if(std::is_constant_evaluated())
		return static_cast<T>(gcem::pow(base, exponent));

	return static_cast<T>(std::pow(base, exponent));
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Pow(Vec<L, T> base, const Vec<L, T>& exponent)
{
	for (u32 i = 0u; i < L; i++)
		base[i] = static_cast<T>(Pow(base[i], exponent[i]));

	return base;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Pow(const tQuat<T>& x, const T y)
{
	//Raising to the power of 0 should yield 1
	//Needed to prevent a division by 0 error later on
	if (y > -Epsilon<T>() && y < Epsilon<T>())
		return tQuat<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));

	//To deal with non-unit quaternions
	const T magnitude = Sqrt(x.x() * x.x() + x.y() * x.y() + x.z() * x.z() + x.w() * x.w());

	T angle;
	if(Abs(x.w() / magnitude) > CosOneOverTwo<T>())
	{
		//Scalar component is close to 1; using it to recover angle would lose precision
		//Instead, we use the non-scalar components since Sin() is accurate around 0

		//Prevent a division by 0 error later on
		const T vectorMagnitude = x.x() * x.x() + x.y() * x.y() + x.z() * x.z();
		//Despite the compiler might say, we actually want to compare vectorMagnitude to 0.
		//Here, we could use denorm_int() compiling a project with unsafe maths optimizations
		//might make the comparison always false, even when vectorMagnitude is 0.
		if(vectorMagnitude < std::numeric_limits<T>::min())
		{
			//Equivalent to raising a real number to a power
			return tQuat<T>(Pow(x.w(), y), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		}

		angle = ASin(Sqrt(vectorMagnitude) / magnitude);
	}
	else
	{
		//Scalar component is small, shouldn't cause loss of precision
		angle = ACos(x.w() / magnitude);
	}

	const T newAngle = angle * y;
	const T div = Sin(newAngle) / Sin(angle);
	const T mag = Pow(magnitude, y - static_cast<T>(1));
	return tQuat<T>(Cos(newAngle) * magnitude * mag, x.x() * div * mag, x.y() * div * mag, x.z() * div * mag);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Exp(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::exp(x);

	return std::exp(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Exp(Vec<L, T> x)
{
	for(T& n : x)
		n = Exp(n);

	return x;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Exp(const tQuat<T>& q)
{
	Vec<3, T> u(q.x(), q.y(), q.z());
	const T angle = Length(u);
	if (angle < Epsilon<T>())
		return tQuat<T>();

	const Vec<3, T> v(u / angle);
	return tQuat<T>(Cos(angle), Sin(angle) * v);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Log(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::log(x);

	return std::log(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Log(Vec<L, T> x)
{
	for(T& n : x)
		n = Log(n);

	return x;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Log(const tQuat<T>& q)
{
	Vec<3, T> u(q.x(), q.y(), q.z());
	const T vec3Len = Length(u);

	if(vec3Len < Epsilon<T>())
	{
		if (q.w() > static_cast<T>(0))
			return tQuat<T>(Log(q.w()), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		if (q.w() < static_cast<T>(0))
			return tQuat<T>(Log(-q.w()), PI<T>(), static_cast<T>(0), static_cast<T>(0));
		return tQuat<T>(std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(),
		                      std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity());
	}

	const T t = ATan(vec3Len, T(q.w())) / vec3Len;
	const T quatLen2 = vec3Len * vec3Len + q.w() * q.w();
	return tQuat<T>(static_cast<T>(0.5) * Log(quatLen2), t * q.x(), t * q.y(), t * q.z());
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] genType TRAP::Math::Exp2(const genType x)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return std::exp2(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] TRAP::Math::Vec<L, T> TRAP::Math::Exp2(Vec<L, T> x)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	for(T& n : x)
		n = std::exp2(n);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::Log2(const genType x)
{
	if(std::is_constant_evaluated())
		return static_cast<genType>(gcem::log2(x));

	return static_cast<genType>(std::log2(x));
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Log2(Vec<L, T> x)
{
	for(T& n : x)
		n = Log2(n);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Sqrt(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::sqrt(x);

	return std::sqrt(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Sqrt(Vec<L, T> x)
{
	for(T& n : x)
		n = Sqrt(n);

	return x;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Sqrt(const tQuat<T>& x)
{
	return Pow(x, static_cast<T>(0.5));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::InverseSqrt(const genType x)
{
	return static_cast<genType>(1) / Sqrt(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::InverseSqrt(Vec<L, T> x)
{
	for(T& n : x)
		n = static_cast<T>(1) / Sqrt(n);

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//
//Geometric----------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Length(const Vec<L, T>& v)
{
	return Sqrt(Dot(v, v));
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Length(const tQuat<T>& q)
{
	return Sqrt(Dot(q, q));
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Distance(const Vec<L, T>& p0, const Vec<L, T>& p1)
{
	return Length(p1 - p0);
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Dot(const Vec<L, T>& x, const Vec<L, T>& y)
{
	const Vec<L, T> tmp(x * y);

	return std::accumulate(tmp.begin(), tmp.end(), static_cast<T>(0));
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Dot(const tQuat<T>& x, const tQuat<T>& y)
{
	const Vec<4, T> tmp(x.w() * y.w(), x.x() * y.x(), x.y() * y.y(), x.z() * y.z());

	return (tmp.x() + tmp.y()) + (tmp.z() + tmp.w());
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T> TRAP::Math::Cross(const Vec<3, T>& x, const Vec<3, T>& y)
{
	return Vec<3, T>(x.y() * y.z() - y.y() * x.z(),
					 x.z() * y.x() - y.z() * x.x(),
					 x.x() * y.y() - y.x() * x.y());
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Cross(const tQuat<T>& q1, const tQuat<T>& q2) noexcept
{
	return tQuat<T>(q1.w() * q2.w() - q1.x() * q2.x() - q1.y() * q2.y() - q1.z() * q2.z(),
					q1.w() * q2.x() + q1.x() * q2.w() + q1.y() * q2.z() - q1.z() * q2.y(),
					q1.w() * q2.y() + q1.y() * q2.w() + q1.z() * q2.x() - q1.x() * q2.z(),
					q1.w() * q2.z() + q1.z() * q2.w() + q1.x() * q2.y() - q1.y() * q2.x());
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Normalize(const Vec<L, T>& x)
{
	return x * InverseSqrt(Dot(x, x));
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Normalize(const tQuat<T>& q)
{
	const T len = Length(q);
	if (len <= static_cast<T>(0)) //Problem
		return tQuat<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));

	const T oneOverLen = static_cast<T>(1) / len;
	return tQuat<T>(q.w() * oneOverLen, q.x() * oneOverLen, q.y() * oneOverLen, q.z() * oneOverLen);
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::FaceForward(const Vec<L, T>& N, const Vec<L, T>& I, const Vec<L, T>& NRef)
{
	return Dot(NRef, I) < static_cast<T>(0) ? N : -N;
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Reflect(const Vec<L, T>& I, const Vec<L, T>& N)
{
	return I - N * Dot(N, I) * static_cast<T>(2);
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Refract(const Vec<L, T>& I, const Vec<L, T>& N, T eta)
{
	const T dotValue(Dot(N, I));
	const T k(static_cast<T>(1) - eta * eta * (static_cast<T>(1) - dotValue * dotValue));
	const Vec<L, T> result = (k >= static_cast<T>(0)) ? (eta * I - (eta * dotValue + Sqrt(k)) * N) : Vec<L, T>(static_cast<T>(0));

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <u32 L, typename T>
requires std::is_arithmetic_v<T> || std::same_as<T, bool>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::Equal(const Mat<L, L, T>& x, const Mat<L, L, T>& y)
{
	Vec<L, bool> result(true);
	for(u32 i = 0u; i < L; ++i)
		result[i] = All(Equal(x[i], y[i]));
	return result;
}

template <u32 L, typename T>
requires std::is_arithmetic_v<T> || std::same_as<T, bool>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::Equal(const Mat<L, L, T>& x, const Mat<L, L, T>& y, T epsilon)
{
	return Equal(x, y, Vec<L, T>(epsilon));
}

template <u32 L, typename T>
requires std::is_arithmetic_v<T> || std::same_as<T, bool>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::Equal(const Mat<L, L, T>& x, const Mat<L, L, T>& y,
                                                                   const Vec<L, T>& epsilon)
{
	Vec<L, bool> result(true);
	for (u32 i = 0u; i < L; ++i)
		result[i] = All(Equal(x[i], y[i], epsilon[i]));
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <u32 L, typename T>
requires std::is_arithmetic_v<T> || std::same_as<T, bool>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::NotEqual(const Mat<L, L, T>& x, const Mat<L, L, T>& y)
{
	Vec<L, bool> result(true);
	for(u32 i = 0u; i < L; ++i)
		result[i] = Any(NotEqual(x[i], y[i]));
	return result;
}

template <u32 L, typename T>
requires std::is_arithmetic_v<T> || std::same_as<T, bool>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::NotEqual(const Mat<L, L, T>& x, const Mat<L, L, T>& y, T epsilon)
{
	return NotEqual(x, y, Vec<L, T>(epsilon));
}

template <u32 L, typename T>
requires std::is_arithmetic_v<T> || std::same_as<T, bool>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::NotEqual(const Mat<L, L, T>& x, const Mat<L, L, T>& y,
                                                                      const Vec<L, T>& epsilon)
{
	Vec<L, bool> result(true);
	for (u32 i = 0u; i < L; ++i)
		result[i] = Any(NotEqual(x[i], y[i], epsilon[i]));
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//
//Matrix-------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires (std::floating_point<T> && L > 2)
[[nodiscard]] constexpr TRAP::Math::Mat<L, L, T> TRAP::Math::MatrixCompMult(Mat<L, L, T> x, const Mat<L, L, T>& y)
{
	for (usize i = 0u; i < x.Length(); ++i)
		x[i] *= y[i];

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires (std::floating_point<T> && L > 2)
[[nodiscard]] constexpr TRAP::Math::Mat<L, L, T> TRAP::Math::OuterProduct(const Vec<L, T>& c, const Vec<L, T>& r)
{
	Mat<L, L, T> m;
	for (usize i = 0u; i < m.Length(); ++i)
		m[i] = c * r[i];
	return m;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::Transpose(const Mat<3, 3, T>& m)
{
	Mat<3, 3, T> result;

	std::get<0>(std::get<0>(result)) = std::get<0>(std::get<0>(m));
	std::get<1>(std::get<0>(result)) = std::get<0>(std::get<1>(m));
	std::get<2>(std::get<0>(result)) = std::get<0>(std::get<2>(m));

	std::get<0>(std::get<1>(result)) = std::get<1>(std::get<0>(m));
	std::get<1>(std::get<1>(result)) = std::get<1>(std::get<1>(m));
	std::get<2>(std::get<1>(result)) = std::get<1>(std::get<2>(m));

	std::get<0>(std::get<2>(result)) = std::get<2>(std::get<0>(m));
	std::get<1>(std::get<2>(result)) = std::get<2>(std::get<1>(m));
	std::get<2>(std::get<2>(result)) = std::get<2>(std::get<2>(m));

	return result;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Transpose(const Mat<4, 4, T>& m)
{
	Mat<4, 4, T> result;

	std::get<0>(std::get<0>(result)) = std::get<0>(std::get<0>(m));
	std::get<1>(std::get<0>(result)) = std::get<0>(std::get<1>(m));
	std::get<2>(std::get<0>(result)) = std::get<0>(std::get<2>(m));
	std::get<3>(std::get<0>(result)) = std::get<0>(std::get<3>(m));

	std::get<0>(std::get<1>(result)) = std::get<1>(std::get<0>(m));
	std::get<1>(std::get<1>(result)) = std::get<1>(std::get<1>(m));
	std::get<2>(std::get<1>(result)) = std::get<1>(std::get<2>(m));
	std::get<3>(std::get<1>(result)) = std::get<1>(std::get<3>(m));

	std::get<0>(std::get<2>(result)) = std::get<2>(std::get<0>(m));
	std::get<1>(std::get<2>(result)) = std::get<2>(std::get<1>(m));
	std::get<2>(std::get<2>(result)) = std::get<2>(std::get<2>(m));
	std::get<3>(std::get<2>(result)) = std::get<2>(std::get<3>(m));

	std::get<0>(std::get<3>(result)) = std::get<3>(std::get<0>(m));
	std::get<1>(std::get<3>(result)) = std::get<3>(std::get<1>(m));
	std::get<2>(std::get<3>(result)) = std::get<3>(std::get<2>(m));
	std::get<3>(std::get<3>(result)) = std::get<3>(std::get<3>(m));

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Determinant(const Mat<3, 3, T>& m)
{
	return + std::get<0>(std::get<0>(m)) * (std::get<1>(std::get<1>(m)) * std::get<2>(std::get<2>(m)) - std::get<1>(std::get<2>(m)) * std::get<2>(std::get<1>(m)))
		   - std::get<0>(std::get<1>(m)) * (std::get<1>(std::get<0>(m)) * std::get<2>(std::get<2>(m)) - std::get<1>(std::get<2>(m)) * std::get<2>(std::get<0>(m)))
		   + std::get<0>(std::get<2>(m)) * (std::get<1>(std::get<0>(m)) * std::get<2>(std::get<1>(m)) - std::get<1>(std::get<1>(m)) * std::get<2>(std::get<0>(m)));
}
template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Determinant(const Mat<4, 4, T>& m)
{
	const T subFactor00 = std::get<2>(std::get<2>(m)) * std::get<3>(std::get<3>(m)) - std::get<2>(std::get<3>(m)) * std::get<3>(std::get<2>(m));
	const T subFactor01 = std::get<1>(std::get<2>(m)) * std::get<3>(std::get<3>(m)) - std::get<1>(std::get<3>(m)) * std::get<3>(std::get<2>(m));
	const T subFactor02 = std::get<1>(std::get<2>(m)) * std::get<2>(std::get<3>(m)) - std::get<1>(std::get<3>(m)) * std::get<2>(std::get<2>(m));
	const T subFactor03 = std::get<0>(std::get<2>(m)) * std::get<3>(std::get<3>(m)) - std::get<0>(std::get<3>(m)) * std::get<3>(std::get<2>(m));
	const T subFactor04 = std::get<0>(std::get<2>(m)) * std::get<2>(std::get<3>(m)) - std::get<0>(std::get<3>(m)) * std::get<2>(std::get<2>(m));
	const T subFactor05 = std::get<0>(std::get<2>(m)) * std::get<1>(std::get<3>(m)) - std::get<0>(std::get<3>(m)) * std::get<1>(std::get<2>(m));

	const Vec<4, T> detCof(+(std::get<1>(std::get<1>(m)) * subFactor00 - std::get<2>(std::get<1>(m)) * subFactor01 + std::get<3>(std::get<1>(m)) * subFactor02),
		                   -(std::get<0>(std::get<1>(m)) * subFactor00 - std::get<2>(std::get<1>(m)) * subFactor03 + std::get<3>(std::get<1>(m)) * subFactor04),
		                   +(std::get<0>(std::get<1>(m)) * subFactor01 - std::get<1>(std::get<1>(m)) * subFactor03 + std::get<3>(std::get<1>(m)) * subFactor05),
		                   -(std::get<0>(std::get<1>(m)) * subFactor02 - std::get<1>(std::get<1>(m)) * subFactor04 + std::get<2>(std::get<1>(m)) * subFactor05));

	return std::get<0>(std::get<0>(m)) * std::get<0>(detCof) + std::get<1>(std::get<0>(m)) * std::get<1>(detCof) +
		   std::get<2>(std::get<0>(m)) * std::get<2>(detCof) + std::get<3>(std::get<0>(m)) * std::get<3>(detCof);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::Inverse(const Mat<3, 3, T>& m)
{
	const T oneOverDeterminant = static_cast<T>(1) / (+ std::get<0>(std::get<0>(m)) * (std::get<1>(std::get<1>(m)) * std::get<2>(std::get<2>(m)) - std::get<1>(std::get<2>(m)) * std::get<2>(std::get<1>(m)))
		                                              - std::get<0>(std::get<1>(m)) * (std::get<1>(std::get<0>(m)) * std::get<2>(std::get<2>(m)) - std::get<1>(std::get<2>(m)) * std::get<2>(std::get<0>(m)))
		                                              + std::get<0>(std::get<2>(m)) * (std::get<1>(std::get<0>(m)) * std::get<2>(std::get<1>(m)) - std::get<1>(std::get<1>(m)) * std::get<2>(std::get<0>(m))));

	Mat<3, 3, T> inverse;
	std::get<0>(std::get<0>(inverse)) = +(std::get<1>(std::get<1>(m)) * std::get<2>(std::get<2>(m)) - std::get<1>(std::get<2>(m)) * std::get<2>(std::get<1>(m))) * oneOverDeterminant;
	std::get<0>(std::get<1>(inverse)) = -(std::get<0>(std::get<1>(m)) * std::get<2>(std::get<2>(m)) - std::get<0>(std::get<2>(m)) * std::get<2>(std::get<1>(m))) * oneOverDeterminant;
	std::get<0>(std::get<2>(inverse)) = +(std::get<0>(std::get<1>(m)) * std::get<1>(std::get<2>(m)) - std::get<0>(std::get<2>(m)) * std::get<1>(std::get<1>(m))) * oneOverDeterminant;
	std::get<1>(std::get<0>(inverse)) = -(std::get<1>(std::get<0>(m)) * std::get<2>(std::get<2>(m)) - std::get<1>(std::get<2>(m)) * std::get<2>(std::get<0>(m))) * oneOverDeterminant;
	std::get<1>(std::get<1>(inverse)) = +(std::get<0>(std::get<0>(m)) * std::get<2>(std::get<2>(m)) - std::get<0>(std::get<2>(m)) * std::get<2>(std::get<0>(m))) * oneOverDeterminant;
	std::get<1>(std::get<2>(inverse)) = -(std::get<0>(std::get<0>(m)) * std::get<1>(std::get<2>(m)) - std::get<0>(std::get<2>(m)) * std::get<1>(std::get<0>(m))) * oneOverDeterminant;
	std::get<2>(std::get<0>(inverse)) = +(std::get<1>(std::get<0>(m)) * std::get<2>(std::get<1>(m)) - std::get<1>(std::get<1>(m)) * std::get<2>(std::get<0>(m))) * oneOverDeterminant;
	std::get<2>(std::get<1>(inverse)) = -(std::get<0>(std::get<0>(m)) * std::get<2>(std::get<1>(m)) - std::get<0>(std::get<1>(m)) * std::get<2>(std::get<0>(m))) * oneOverDeterminant;
	std::get<2>(std::get<2>(inverse)) = +(std::get<0>(std::get<0>(m)) * std::get<1>(std::get<1>(m)) - std::get<0>(std::get<1>(m)) * std::get<1>(std::get<0>(m))) * oneOverDeterminant;

	return inverse;
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Inverse(const Mat<4, 4, T>& m)
{
	const T coef00 = std::get<2>(std::get<2>(m)) * std::get<3>(std::get<3>(m)) - std::get<2>(std::get<3>(m)) * std::get<3>(std::get<2>(m));
	const T coef02 = std::get<2>(std::get<1>(m)) * std::get<3>(std::get<3>(m)) - std::get<2>(std::get<3>(m)) * std::get<3>(std::get<1>(m));
	const T coef03 = std::get<2>(std::get<1>(m)) * std::get<3>(std::get<2>(m)) - std::get<2>(std::get<2>(m)) * std::get<3>(std::get<1>(m));

	const T coef04 = std::get<1>(std::get<2>(m)) * std::get<3>(std::get<3>(m)) - std::get<1>(std::get<3>(m)) * std::get<3>(std::get<2>(m));
	const T coef06 = std::get<1>(std::get<1>(m)) * std::get<3>(std::get<3>(m)) - std::get<1>(std::get<3>(m)) * std::get<3>(std::get<1>(m));
	const T coef07 = std::get<1>(std::get<1>(m)) * std::get<3>(std::get<2>(m)) - std::get<1>(std::get<2>(m)) * std::get<3>(std::get<1>(m));

	const T coef08 = std::get<1>(std::get<2>(m)) * std::get<2>(std::get<3>(m)) - std::get<1>(std::get<3>(m)) * std::get<2>(std::get<2>(m));
	const T coef10 = std::get<1>(std::get<1>(m)) * std::get<2>(std::get<3>(m)) - std::get<1>(std::get<3>(m)) * std::get<2>(std::get<1>(m));
	const T coef11 = std::get<1>(std::get<1>(m)) * std::get<2>(std::get<2>(m)) - std::get<1>(std::get<2>(m)) * std::get<2>(std::get<1>(m));

	const T coef12 = std::get<0>(std::get<2>(m)) * std::get<3>(std::get<3>(m)) - std::get<0>(std::get<3>(m)) * std::get<3>(std::get<2>(m));
	const T coef14 = std::get<0>(std::get<1>(m)) * std::get<3>(std::get<3>(m)) - std::get<0>(std::get<3>(m)) * std::get<3>(std::get<1>(m));
	const T coef15 = std::get<0>(std::get<1>(m)) * std::get<3>(std::get<2>(m)) - std::get<0>(std::get<2>(m)) * std::get<3>(std::get<1>(m));

	const T coef16 = std::get<0>(std::get<2>(m)) * std::get<2>(std::get<3>(m)) - std::get<0>(std::get<3>(m)) * std::get<2>(std::get<2>(m));
	const T coef18 = std::get<0>(std::get<1>(m)) * std::get<2>(std::get<3>(m)) - std::get<0>(std::get<3>(m)) * std::get<2>(std::get<1>(m));
	const T coef19 = std::get<0>(std::get<1>(m)) * std::get<2>(std::get<2>(m)) - std::get<0>(std::get<2>(m)) * std::get<2>(std::get<1>(m));

	const T coef20 = std::get<0>(std::get<2>(m)) * std::get<1>(std::get<3>(m)) - std::get<0>(std::get<3>(m)) * std::get<1>(std::get<2>(m));
	const T coef22 = std::get<0>(std::get<1>(m)) * std::get<1>(std::get<3>(m)) - std::get<0>(std::get<3>(m)) * std::get<1>(std::get<1>(m));
	const T coef23 = std::get<0>(std::get<1>(m)) * std::get<1>(std::get<2>(m)) - std::get<0>(std::get<2>(m)) * std::get<1>(std::get<1>(m));

	const Vec<4, T> fac0(coef00, coef00, coef02, coef03);
	const Vec<4, T> fac1(coef04, coef04, coef06, coef07);
	const Vec<4, T> fac2(coef08, coef08, coef10, coef11);
	const Vec<4, T> fac3(coef12, coef12, coef14, coef15);
	const Vec<4, T> fac4(coef16, coef16, coef18, coef19);
	const Vec<4, T> fac5(coef20, coef20, coef22, coef23);

	const Vec<4, T> vec0(std::get<0>(std::get<1>(m)), std::get<0>(std::get<0>(m)), std::get<0>(std::get<0>(m)), std::get<0>(std::get<0>(m)));
	const Vec<4, T> vec1(std::get<1>(std::get<1>(m)), std::get<1>(std::get<0>(m)), std::get<1>(std::get<0>(m)), std::get<1>(std::get<0>(m)));
	const Vec<4, T> vec2(std::get<2>(std::get<1>(m)), std::get<2>(std::get<0>(m)), std::get<2>(std::get<0>(m)), std::get<2>(std::get<0>(m)));
	const Vec<4, T> vec3(std::get<3>(std::get<1>(m)), std::get<3>(std::get<0>(m)), std::get<3>(std::get<0>(m)), std::get<3>(std::get<0>(m)));

	const Vec<4, T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
	const Vec<4, T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
	const Vec<4, T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
	const Vec<4, T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

	const Vec<4, T> signA(static_cast<T>(+1), static_cast<T>(-1), static_cast<T>(+1), static_cast<T>(-1));
	const Vec<4, T> signB(static_cast<T>(-1), static_cast<T>(+1), static_cast<T>(-1), static_cast<T>(+1));
	const Mat<4, 4, T> inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

	const Vec<4, T> row0(std::get<0>(std::get<0>(inverse)), std::get<0>(std::get<1>(inverse)), std::get<0>(std::get<2>(inverse)), std::get<0>(std::get<3>(inverse)));

	const Vec<4, T> dot0(std::get<0>(m) * row0);
	const T dot1 = (dot0.x() + dot0.y()) + (dot0.z() + dot0.w());

	const T oneOverDeterminant = static_cast<T>(1) / dot1;

	return inverse * oneOverDeterminant;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Orthographic(const T left, const T right,
                                                                          const T bottom, const T top,
                                                                          const T zNear, const T zFar) noexcept
{
	//RH_ZO
	Mat<4, 4, T> result(static_cast<T>(1));

	std::get<0>(std::get<0>(result)) = static_cast<T>(2) / (right - left);
	std::get<1>(std::get<1>(result)) = static_cast<T>(2) / (top - bottom);
	std::get<2>(std::get<2>(result)) = -static_cast<T>(1) / (zFar - zNear);
	std::get<0>(std::get<3>(result)) = -(right + left) / (right - left);
	std::get<1>(std::get<3>(result)) = -(top + bottom) / (top - bottom);
	std::get<2>(std::get<3>(result)) = -zNear / (zFar - zNear);

	return result;
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::OrthographicReverseZ(const T left, const T right,
                                                                                  const T bottom, const T top,
                                                                                  const T zNear, const T zFar) noexcept
{
	return Orthographic(left, right, bottom, top, zFar, zNear);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Frustum(const T left, const T right,
                                                                     const T bottom, const T top,
                                                                     const T nearVal, const T farVal) noexcept
{
	//RH_ZO
	Mat<4, 4, T> result(static_cast<T>(0));

	std::get<0>(std::get<0>(result)) = (static_cast<T>(2) * nearVal) / (right - left);
	std::get<1>(std::get<1>(result)) = (static_cast<T>(2) * nearVal) / (top - bottom);
	std::get<0>(std::get<2>(result)) = (right + left) / (right - left);
	std::get<1>(std::get<2>(result)) = (top + bottom) / (top - bottom);
	std::get<2>(std::get<2>(result)) = farVal / (nearVal - farVal);
	std::get<3>(std::get<2>(result)) = static_cast<T>(-1);
	std::get<2>(std::get<3>(result)) = -(farVal * nearVal) / (farVal - nearVal);

	return result;
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::FrustumReverseZ(const T left, const T right,
                                                                             const T bottom, const T top,
                                                                             const T nearVal, const T farVal) noexcept
{
	return Frustum(left, right, bottom, top, farVal, nearVal);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Perspective(const T fovY, const T aspect, const T zNear, const T zFar)
{
	//RH_ZO

	TRAP_ASSERT(Abs(aspect - Epsilon<T>()) > static_cast<T>(0), "Math::Perspective(): Division by zero!");

	const T tanHalfFoVY = Tan(fovY / static_cast<T>(2));

	Mat<4, 4, T> result(static_cast<T>(0));

	std::get<0>(std::get<0>(result)) = static_cast<T>(1) / (aspect * tanHalfFoVY);
	std::get<1>(std::get<1>(result)) = static_cast<T>(1) / (tanHalfFoVY);
	std::get<2>(std::get<2>(result)) = zFar / (zNear - zFar);
	std::get<3>(std::get<2>(result)) = -static_cast<T>(1);
	std::get<2>(std::get<3>(result)) = -(zFar * zNear) / (zFar - zNear);

	return result;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::PerspectiveReverseZ(const T fovY, const T aspect, const T zNear, const T zFar)
{
	return Perspective(fovY, aspect, zFar, zNear);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::PerspectiveFoV(const T fov, const T width, const T height,
                                                                            const T zNear, const T zFar)
{
	//RH_ZO
	TRAP_ASSERT(width > static_cast<T>(0), "Math::PerspectiveFoV(): Width must be greater than zero!");
	TRAP_ASSERT(height > static_cast<T>(0), "Math::PerspectiveFoV(): Height must be greater than zero!");
	TRAP_ASSERT(fov > static_cast<T>(0), "Math::PerspectiveFoV(): FOV must be greater than zero!");
	TRAP_ASSERT((zNear - zFar) != static_cast<T>(0), "Math::PerspectiveFoV(): zNear - zFar must not be zero!");

	const T rad = fov;
	const T h = Cos(static_cast<T>(0.5) * rad) / Sin(static_cast<T>(0.5) * rad);
	const T w = h * height / width;

	Mat<4, 4, T> result(static_cast<T>(0));

	std::get<0>(std::get<0>(result)) = w;
	std::get<1>(std::get<1>(result)) = h;
	std::get<2>(std::get<2>(result)) = zFar / (zNear - zFar);
	std::get<3>(std::get<2>(result)) = -static_cast<T>(1);
	std::get<2>(std::get<3>(result)) = -(zFar * zNear) / (zFar - zNear);

	return result;
}


template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::PerspectiveFoVReverseZ(const T fov, const T width, const T height,
                                                                                    const T zNear, const T zFar)
{
	return PerspectiveFoV(fov, width, height, zFar, zNear);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::InfinitePerspective(const T fovY, const T aspect, const T zNear)
{
	//RH_ZO

	const T range = Tan(fovY / static_cast<T>(2)) * zNear;
	const T left = -range * aspect;
	const T right = range * aspect;
	const T bottom = -range;
	const T top = range;

	Mat<4, 4, T> result(static_cast<T>(0));

	std::get<0>(std::get<0>(result)) = (static_cast<T>(2) * zNear) / (right - left);
	std::get<1>(std::get<1>(result)) = (static_cast<T>(2) * zNear) / (top - bottom);
	std::get<2>(std::get<2>(result)) = -static_cast<T>(1);
	std::get<3>(std::get<2>(result)) = -static_cast<T>(1);
	std::get<2>(std::get<3>(result)) = -zNear;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T, typename U>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<L, L, T> TRAP::Math::Mix(const Mat<L, L, T>& x, const Mat<L, L, T>& y, U a)
{
	return Mat<L, L, U>(x) * (static_cast<U>(1) - a) + Mat<L, L, U>(y) * a;
}

template<u32 L, typename T, typename U>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<L, L, T> TRAP::Math::Mix(const Mat<L, L, T>& x, const Mat<L, L, T>& y, const Mat<L, L, U>& a)
{
	return MatrixCompMult(Mat<L, L, U>(x), static_cast<U>(1) - a) + MatrixCompMult(Mat<L, L, U>(y), a);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Translate(const Mat<4, 4, T>& m, const Vec<3, T>& v) noexcept
{
	Mat<4, 4, T> result(m);

	std::get<3>(result) = std::get<0>(m) * std::get<0>(v) + std::get<1>(m) * std::get<1>(v) + std::get<2>(m) * std::get<2>(v) + std::get<3>(m);

	return result;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Translate(const Vec<3, T>& v) noexcept
{
	Mat<4, 4, T> result(static_cast<T>(1.0));

	std::get<3>(result) = std::get<0>(result) * std::get<0>(v) + std::get<1>(result) * std::get<1>(v) + std::get<2>(result) * std::get<2>(v) + std::get<3>(result);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Rotate(const Mat<4, 4, T>& m, const T angleInRadians, const Vec<3, T>& v)
{
	const T a = angleInRadians;
	const T c = Cos(a);
	const T s = Sin(a);

	const Vec<3, T> axis(Normalize(v));
	const Vec<3, T> temp((static_cast<T>(1) - c) * axis);

	Mat<4, 4, T> rotate;

	std::get<0>(std::get<0>(rotate)) = c + std::get<0>(temp) * std::get<0>(axis);
	std::get<1>(std::get<0>(rotate)) = std::get<0>(temp) * std::get<1>(axis) + s * std::get<2>(axis);
	std::get<2>(std::get<0>(rotate)) = std::get<0>(temp) * std::get<2>(axis) - s * std::get<1>(axis);

	std::get<0>(std::get<1>(rotate)) = std::get<1>(temp) * std::get<0>(axis) - s * std::get<2>(axis);
	std::get<1>(std::get<1>(rotate)) = c + std::get<1>(temp) * std::get<1>(axis);
	std::get<2>(std::get<1>(rotate)) = std::get<1>(temp) * std::get<2>(axis) + s * std::get<0>(axis);

	std::get<0>(std::get<2>(rotate)) = std::get<2>(temp) * std::get<0>(axis) + s * std::get<1>(axis);
	std::get<1>(std::get<2>(rotate)) = std::get<2>(temp) * std::get<1>(axis) - s * std::get<0>(axis);
	std::get<2>(std::get<2>(rotate)) = c + std::get<2>(temp) * std::get<2>(axis);

	Mat<4, 4, T> result;

	std::get<0>(result) = std::get<0>(m) * std::get<0>(std::get<0>(rotate)) + std::get<1>(m) * std::get<1>(std::get<0>(rotate)) + std::get<2>(m) * std::get<2>(std::get<0>(rotate));
	std::get<1>(result) = std::get<0>(m) * std::get<0>(std::get<1>(rotate)) + std::get<1>(m) * std::get<1>(std::get<1>(rotate)) + std::get<2>(m) * std::get<2>(std::get<1>(rotate));
	std::get<2>(result) = std::get<0>(m) * std::get<0>(std::get<2>(rotate)) + std::get<1>(m) * std::get<1>(std::get<2>(rotate)) + std::get<2>(m) * std::get<2>(std::get<2>(rotate));
	std::get<3>(result) = std::get<3>(m);

	return result;
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Rotate(const T angleInRadians, const Vec<3, T>& v)
{
	const T a = angleInRadians;
	const T c = Cos(a);
	const T s = Sin(a);

	const Vec<3, T> axis(Normalize(v));
	const Vec<3, T> temp((static_cast<T>(1) - c) * axis);

	Mat<4, 4, T> rotate;

	std::get<0>(std::get<0>(rotate)) = c + std::get<0>(temp) * std::get<0>(axis);
	std::get<1>(std::get<0>(rotate)) = std::get<0>(temp) * std::get<1>(axis) + s * std::get<2>(axis);
	std::get<2>(std::get<0>(rotate)) = std::get<0>(temp) * std::get<2>(axis) - s * std::get<1>(axis);

	std::get<0>(std::get<1>(rotate)) = std::get<1>(temp) * std::get<0>(axis) - s * std::get<2>(axis);
	std::get<1>(std::get<1>(rotate)) = c + std::get<1>(temp) * std::get<1>(axis);
	std::get<2>(std::get<1>(rotate)) = std::get<1>(temp) * std::get<2>(axis) + s * std::get<0>(axis);

	std::get<0>(std::get<2>(rotate)) = std::get<2>(temp) * std::get<0>(axis) + s * std::get<1>(axis);
	std::get<1>(std::get<2>(rotate)) = std::get<2>(temp) * std::get<1>(axis) - s * std::get<0>(axis);
	std::get<2>(std::get<2>(rotate)) = c + std::get<2>(temp) * std::get<2>(axis);

	const Mat<4, 4, T> identity(static_cast<T>(1.0));
	Mat<4, 4, T> result;

	std::get<0>(result) = std::get<0>(identity) * std::get<0>(std::get<0>(rotate)) + std::get<1>(identity) * std::get<1>(std::get<0>(rotate)) + std::get<2>(identity) * std::get<2>(std::get<0>(rotate));
	std::get<1>(result) = std::get<0>(identity) * std::get<0>(std::get<1>(rotate)) + std::get<1>(identity) * std::get<1>(std::get<1>(rotate)) + std::get<2>(identity) * std::get<2>(std::get<1>(rotate));
	std::get<2>(result) = std::get<0>(identity) * std::get<0>(std::get<2>(rotate)) + std::get<1>(identity) * std::get<1>(std::get<2>(rotate)) + std::get<2>(identity) * std::get<2>(std::get<2>(rotate));
	std::get<3>(result) = std::get<3>(identity);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Scale(const Mat<4, 4, T>& m, const Vec<3, T>& v) noexcept
{
	Mat<4, 4, T> result;

	std::get<0>(result) = std::get<0>(m) * std::get<0>(v);
	std::get<1>(result) = std::get<1>(m) * std::get<1>(v);
	std::get<2>(result) = std::get<2>(m) * std::get<2>(v);
	std::get<3>(result) = std::get<3>(m);

	return result;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Scale(const Vec<3, T>& v) noexcept
{
	Mat<4, 4, T> result(static_cast<T>(1.0f));

	std::get<0>(result) *= std::get<0>(v);
	std::get<1>(result) *= std::get<1>(v);
	std::get<2>(result) *= std::get<2>(v);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::LookAt(const Vec<3, T>& eye, const Vec<3, T>& center, const Vec<3, T>& up)
{
	//RH
	const Vec<3, T> f(Normalize(center - eye));
	const Vec<3, T> s(Normalize(Cross(f, up)));
	const Vec<3, T> u(Cross(s, f));

	Mat<4, 4, T> result(static_cast<T>(1));

	std::get<0>(std::get<0>(result)) = s.x();
	std::get<0>(std::get<1>(result)) = s.y();
	std::get<0>(std::get<2>(result)) = s.z();
	std::get<1>(std::get<0>(result)) = u.x();
	std::get<1>(std::get<1>(result)) = u.y();
	std::get<1>(std::get<2>(result)) = u.z();
	std::get<2>(std::get<0>(result)) = -f.x();
	std::get<2>(std::get<1>(result)) = -f.y();
	std::get<2>(std::get<2>(result)) = -f.z();
	std::get<0>(std::get<3>(result)) = -Dot(s, eye);
	std::get<1>(std::get<3>(result)) = -Dot(u, eye);
	std::get<2>(std::get<3>(result)) = Dot(f, eye);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires TRAP::Math::IsMat<T>
[[nodiscard]] constexpr T TRAP::Math::Row(const T& m, const usize index, const typename T::row_type& x)
{
	TRAP_ASSERT(index < std::get<0>(m).Length(), "Math::Row(): Index out of range!");

	T result = m;

	for(usize i = 0u; i < m.Length(); ++i)
		result[i][index] = x[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires TRAP::Math::IsMat<T>
[[nodiscard]] constexpr typename T::row_type TRAP::Math::Row(const T& m, const usize index)
{
	TRAP_ASSERT(index < std::get<0>(m).Length(), "Math::Row(): Index out of range!");

	typename T::row_type result(0);

	for(usize i = 0u; i < m.Length(); ++i)
		result[i] = m[i][index];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires TRAP::Math::IsMat<T>
[[nodiscard]] constexpr T TRAP::Math::Column(T m, const usize index, const typename T::col_type& x)
{
	TRAP_ASSERT(index < m.Length(), "Math::Column(): Index out of range!");

	m[index] = x;

	return m;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires TRAP::Math::IsMat<T>
[[nodiscard]] constexpr typename T::col_type TRAP::Math::Column(const T& m, const usize index)
{
	TRAP_ASSERT(index < m.Length(), "Math::Column(): Index out of range!");

	return m[index];
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr bool TRAP::Math::Decompose(Mat<4, 4, T> m, Vec<3, T>& outPosition, tQuat<T>& outRotation, Vec<3, T>& outScale)
{
	constexpr auto LocalScale = [](const Vec<3, T>& v, const T desiredLength)
	{
		return v * desiredLength / Length(v);
	};
	constexpr auto LocalCombine = [](const Vec<3, T>& a, const Vec<3, T>& b, const T ascl, const T bscl)
	{
		return (a * ascl) + (b * bscl);
	};

	//Normalize the matrix
	if(Equal(std::get<3>(std::get<3>(m)), static_cast<T>(0.0f), Epsilon<T>()))
		return false;

	for(auto& col : m)
	{
		for(T& n : col)
			n /= std::get<3>(std::get<3>(m));
	}

	//perspectiveMatrix is used to solve for perspective, but it also provides
	//an easy way to test for singulariy of the upper 3x3 component.
	Mat<4, 4, T> perspectiveMatrix(m);

	for(u32 i = 0; i < 3; ++i)
		std::get<3>(perspectiveMatrix[i]) = static_cast<T>(0.0f);
	std::get<3>(std::get<3>(perspectiveMatrix)) = static_cast<T>(1.0f);

	if(Equal(Determinant(perspectiveMatrix), static_cast<T>(0.0f), Epsilon<T>()))
		return false;

	//First, isolate perspective.
	if(NotEqual(std::get<3>(std::get<0>(m)), static_cast<T>(0.0f), Epsilon<T>()) ||
	   NotEqual(std::get<3>(std::get<1>(m)), static_cast<T>(0.0f), Epsilon<T>()) ||
	   NotEqual(std::get<3>(std::get<2>(m)), static_cast<T>(0.0f), Epsilon<T>()))
	{
		//Clear the perspective partition
		std::get<3>(std::get<0>(m)) = std::get<3>(std::get<1>(m)) = std::get<3>(std::get<2>(m)) = static_cast<T>(0.0f);
		std::get<3>(std::get<3>(m)) = static_cast<T>(1.0f);
	}

	//Take care of translation
	outPosition = Vec<3, T>(std::get<3>(m));
	std::get<3>(m) = Vec<4, T>(0.0f, 0.0f, 0.0f, std::get<3>(m).w());

	std::array<Vec<3, T>, 3> row{};

	//Now get scale and shear.
	for(u32 i = 0; i < row.size(); ++i)
	{
		for(u32 j = 0; j < row[i].Length(); ++j)
			row[i][j] = m[i][j];
	}

	//Compute X scale factor and normalize first row
	outScale.x() = Length(std::get<0>(row));

	std::get<0>(row) = LocalScale(std::get<0>(row), static_cast<T>(1.0f));

	//Compute XY shear factor and make 2nd row orthogonal to 1st.
	const T skewZ = Dot(std::get<0>(row), std::get<1>(row));
	std::get<1>(row) = LocalCombine(std::get<1>(row), std::get<0>(row), static_cast<T>(1.0f), -skewZ);

	//Now, comute Y scale and normalize 2nd row.
	outScale.y() = Length(std::get<1>(row));
	std::get<1>(row) = LocalScale(std::get<1>(row), static_cast<T>(1.0f));

	//Compute XZ and YZ shears, orthogonalize 3rd row.
	const T skewY = Dot(std::get<0>(row), std::get<2>(row));
	std::get<2>(row) = LocalCombine(std::get<2>(row), std::get<0>(row), static_cast<T>(1.0f), -skewY);
	const T skewX = Dot(std::get<1>(row), std::get<2>(row));
	std::get<2>(row) = LocalCombine(std::get<2>(row), std::get<1>(row), static_cast<T>(1.0f), -skewX);

	//Next, get Z scale and normalize 3rd row.
	outScale.z() = Length(std::get<2>(row));
	std::get<2>(row) = LocalScale(std::get<2>(row), static_cast<T>(1.0f));

	//At this point, the matrix (in rows[]) is orthonormal.
	//Check for a coordinate system flip, If the determinant is -1,
	//then negate the matrix and the scaling factors.
	Vec<3, T> pdum3 = Cross(std::get<1>(row), std::get<2>(row));
	if(Dot(std::get<0>(row), pdum3) < 0)
	{
		for(T& n : outScale)
			n *= static_cast<T>(-1.0f);
		for(auto& n : row)
			n *= static_cast<T>(-1.0f);
	}

	//Now, get the rotations out.
	const T trace = std::get<0>(row).x() + std::get<1>(row).y() + std::get<2>(row).z();
	if(trace > static_cast<T>(0.0f))
	{
		T root = Sqrt(trace + static_cast<T>(1.0f));
		outRotation.w() = static_cast<T>(0.5f) * root;
		root = static_cast<T>(0.5f) / root;
		outRotation.x() = root * (std::get<1>(row).z() - std::get<2>(row).y());
		outRotation.y() = root * (std::get<2>(row).x() - std::get<0>(row).z());
		outRotation.z() = root * (std::get<0>(row).y() - std::get<1>(row).x());
	}
	else
	{
		constexpr std::array<u32, 3> next{1, 2, 0};
		u8 i = 0;
		if(std::get<2>(row).z() > row[i][i])
			i = 2;
		else if(std::get<1>(row).y() > std::get<0>(row).x())
			i = 1;
		const u32 j = next[i];
		const u32 k = next[j];

		T root = Sqrt(row[i][i] - row[j][j] - row[k][k] + static_cast<T>(1.0f));

		outRotation[i + 1] = static_cast<T>(0.5f) * root;
		root = static_cast<T>(0.5f) / root;
		outRotation[j + 1] = root * (row[i][j] + row[j][i]);
		outRotation[k + 1] = root * (row[i][k] + row[k][i]);
		outRotation.w() = root * (row[j][k] - row[k][j]);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr bool TRAP::Math::Decompose(Mat<4, 4, T> m, Vec<3, T>& outPosition, Vec<3, T>& outRotation, Vec<3, T>& outScale)
{
	constexpr auto LocalScale = [](const Vec<3, T>& v, const T desiredLength)
	{
		return v * desiredLength / Length(v);
	};
	constexpr auto LocalCombine = [](const Vec<3, T>& a, const Vec<3, T>& b, const T ascl, const T bscl)
	{
		return (a * ascl) + (b * bscl);
	};

	//Normalize the matrix
	if(Equal(std::get<3>(std::get<3>(m)), static_cast<T>(0.0f), Epsilon<T>()))
		return false;

	for(auto& col : m)
	{
		for(T& n : col)
			n /= std::get<3>(std::get<3>(m));
	}

	//perspectiveMatrix is used to solve for perspective, but it also provides
	//an easy way to test for singulariy of the upper 3x3 component.
	Mat<4, 4, T> perspectiveMatrix(m);

	for(u32 i = 0; i < 3; ++i)
		std::get<3>(perspectiveMatrix[i]) = static_cast<T>(0.0f);
	std::get<3>(std::get<3>(perspectiveMatrix)) = static_cast<T>(1.0f);

	if(Equal(Determinant(perspectiveMatrix), static_cast<T>(0.0f), Epsilon<T>()))
		return false;

	//First, isolate perspective.
	if(NotEqual(std::get<3>(std::get<0>(m)), static_cast<T>(0.0f), Epsilon<T>()) ||
	   NotEqual(std::get<3>(std::get<1>(m)), static_cast<T>(0.0f), Epsilon<T>()) ||
	   NotEqual(std::get<3>(std::get<2>(m)), static_cast<T>(0.0f), Epsilon<T>()))
	{
		//Clear the perspective partition
		std::get<3>(std::get<0>(m)) = std::get<3>(std::get<1>(m)) = std::get<3>(std::get<2>(m)) = static_cast<T>(0.0f);
		std::get<3>(std::get<3>(m)) = static_cast<T>(1.0f);
	}

	//Take care of translation
	outPosition = Vec<3, T>(std::get<3>(m));
	std::get<3>(m) = Vec<4, T>(0.0f, 0.0f, 0.0f, std::get<3>(m).w());

	std::array<Vec<3, T>, 3> row{};

	//Now get scale and shear.
	for(u32 i = 0; i < 3; ++i)
	{
		for(u32 j = 0; j < 3; ++j)
			row[i][j] = m[i][j];
	}

	//Compute X scale factor and normalize first row
	outScale.x() = Length(std::get<0>(row));

	std::get<0>(row) = LocalScale(std::get<0>(row), static_cast<T>(1.0f));

	//Compute XY shear factor and make 2nd row orthogonal to 1st.
	const T skewZ = Dot(std::get<0>(row), std::get<1>(row));
	std::get<1>(row) = LocalCombine(std::get<1>(row), std::get<0>(row), static_cast<T>(1.0f), -skewZ);

	//Now, comute Y scale and normalize 2nd row.
	outScale.y() = Length(std::get<1>(row));
	std::get<1>(row) = LocalScale(std::get<1>(row), static_cast<T>(1.0f));

	//Compute XZ and YZ shears, orthogonalize 3rd row.
	const T skewY = Dot(std::get<0>(row), std::get<2>(row));
	std::get<2>(row) = LocalCombine(std::get<2>(row), std::get<0>(row), static_cast<T>(1.0f), -skewY);
	const T skewX = Dot(std::get<1>(row), std::get<2>(row));
	std::get<2>(row) = LocalCombine(std::get<2>(row), std::get<1>(row), static_cast<T>(1.0f), -skewX);

	//Next, get Z scale and normalize 3rd row.
	outScale.z() = Length(std::get<2>(row));
	std::get<2>(row) = LocalScale(std::get<2>(row), static_cast<T>(1.0f));

	//At this point, the matrix (in rows[]) is orthonormal.
	//Check for a coordinate system flip, If the determinant is -1,
	//then negate the matrix and the scaling factors.
	Vec<3, T> pdum3 = Cross(std::get<1>(row), std::get<2>(row));
	if(Dot(std::get<0>(row), pdum3) < 0)
	{
		for(T& n : outScale)
			n *= static_cast<T>(-1.0f);
		for(auto& n : row)
			n *= static_cast<T>(-1.0f);
	}

	//Now, get the rotations (euler angles) out.
	outRotation.y() = Degrees(ASin(-std::get<2>(std::get<0>(row))));
	if(Cos(outRotation.y()) != T(0.0f))
	{
		outRotation.x() = Degrees(ATan(std::get<2>(std::get<1>(row)), std::get<2>(std::get<2>(row))));
		outRotation.z() = Degrees(ATan(std::get<1>(std::get<0>(row)), std::get<0>(std::get<0>(row))));
	}
	else
	{
		outRotation.x() = Degrees(ATan(-std::get<0>(std::get<2>(row)), std::get<1>(std::get<1>(row))));
		outRotation.z() = T(0.0f);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Recompose(const Vec<3, T>& position, const tQuat<T>& rotation,
                                                                       const Vec<3, T>& scale)
{
	return Mat<4, 4, T>(static_cast<T>(1.0)) * Translate(position) * Mat4Cast(rotation) * Scale(scale);
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Recompose(const Vec<3, T>& position, const Vec<3, T>& rotation,
                                                                       const Vec<3, T>& scale)
{
	return Mat<4, 4, T>(static_cast<T>(1.0)) * Translate(position) * Mat4Cast(tQuat<T>(Radians(rotation))) * Scale(scale);
}

//-------------------------------------------------------------------------------------------------------------------//
//Quaternion---------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::SLerp(const tQuat<T>& x, const tQuat<T>& y, const T a)
{
	tQuat<T> z = y;

	T cosTheta = Dot(x, y);

	//If cosTheta < 0, the interpolation will take the long way around the sphere.
	//To fix this, one quat must be negated.
	if (cosTheta < static_cast<T>(0))
	{
		z = -y;
		cosTheta = -cosTheta;
	}

	//Perform a linear interpolation when cosTheta is close to 1
	//to avoid side effect of Sin(angle) becoming a zero denominator
	if (cosTheta > static_cast<T>(1) - Epsilon<T>())
	{
		//Linear interpolation
		return tQuat<T>(Mix(x.w(), z.w(), a), Mix(x.x(), z.x(), a), Mix(x.y(), z.y(), a), Mix(x.z(), z.z(), a));
	}

	const T angle = ACos(cosTheta);
	return (Sin((static_cast<T>(1) - a) * angle) * x + Sin(a * angle) * z) / Sin(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T, typename S>
requires std::floating_point<T> && std::integral<S>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::SLerp(const tQuat<T>& x, const tQuat<T>& y, const T a, const S k)
{
	tQuat<T> z = y;

	T cosTheta = Dot(x, y);

	//If cosTheta < 0, the interpolation will take the long way around the sphere.
	//To fix this, one quat must be negated.
	if(cosTheta < static_cast<T>(0))
	{
		z = -y;
		cosTheta = -cosTheta;
	}

	//Perform a linear interpolation when cosTheta is close to 1
	//to avoid side effect of Sin(angle) becoming a zero denominator
	if(cosTheta > static_cast<T>(1) - Epsilon<T>())
	{
		//Linear interpolation
		return tQuat<T>(Mix(x.w(), z.w(), a), Mix(x.x(), z.x(), a), Mix(x.y(), z.y(), a), Mix(x.z(), z.z(), a));
	}

	//Graphics Gems III, page 96
	const T angle = ACos(cosTheta);
	const T phi = angle + static_cast<T>(k) * PI<T>();
	return (Sin(angle - a * phi) * x + Sin(a * phi) * z) / Sin(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Conjugate(const tQuat<T>& q) noexcept
{
	return tQuat<T>(q.w(), -q.x(), -q.y(), -q.z());
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Inverse(const tQuat<T>& q)
{
	return Conjugate(q) / Dot(q, q);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T> TRAP::Math::EulerAngles(const tQuat<T>& q)
{
	return Vec<3, T>(Pitch(q), Yaw(q), Roll(q));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Roll(const tQuat<T>& q)
{
	const T y = static_cast<T>(2) * (q.x() * q.y() + q.w() * q.z());
	const T x = q.w() * q.w() + q.x() * q.x() - q.y() * q.y() - q.z() * q.z();

	if (All(Equal(tVec2<T>(x, y), tVec2<T>(static_cast<T>(0)), Epsilon<T>()))) //Avoid ATan2(0, 0) - handle singularity
		return static_cast<T>(0);

	return static_cast<T>(ATan(y, x));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Pitch(const tQuat<T>& q)
{
	const T y = static_cast<T>(2) * (q.y() * q.z() + q.w() * q.x());
	const T x = q.w() * q.w() - q.x() * q.x() - q.y() * q.y() + q.z() * q.z();

	if (All(Equal(Vec<2, T>(x, y), Vec<2, T>(static_cast<T>(0)), Epsilon<T>())))
		return static_cast<T>(static_cast<T>(2) * ATan(q.x(), q.w()));

	return static_cast<T>(ATan(y, x));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Yaw(const tQuat<T>& q)
{
	return ASin(Clamp(static_cast<T>(-2) * (q.x() * q.z() - q.w() * q.y()), static_cast<T>(-1), static_cast<T>(1)));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<3, 3, T> TRAP::Math::Mat3Cast(const tQuat<T>& q) noexcept
{
	Mat<3, 3, T> result(static_cast<T>(1));
	const T qxx(q.x() * q.x());
	const T qyy(q.y() * q.y());
	const T qzz(q.z() * q.z());
	const T qxz(q.x() * q.z());
	const T qxy(q.x() * q.y());
	const T qyz(q.y() * q.z());
	const T qwx(q.w() * q.x());
	const T qwy(q.w() * q.y());
	const T qwz(q.w() * q.z());

	std::get<0>(std::get<0>(result)) = static_cast<T>(1) - static_cast<T>(2) * (qyy + qzz);
	std::get<1>(std::get<0>(result)) = static_cast<T>(2) * (qxy + qwz);
	std::get<2>(std::get<0>(result)) = static_cast<T>(2) * (qxz - qwy);

	std::get<0>(std::get<1>(result)) = static_cast<T>(2) * (qxy - qwz);
	std::get<1>(std::get<1>(result)) = static_cast<T>(1) - static_cast<T>(2) * (qxx + qzz);
	std::get<2>(std::get<1>(result)) = static_cast<T>(2) * (qyz + qwx);

	std::get<0>(std::get<2>(result)) = static_cast<T>(2) * (qxz + qwy);
	std::get<1>(std::get<2>(result)) = static_cast<T>(2) * (qyz - qwx);
	std::get<2>(std::get<2>(result)) = static_cast<T>(1) - static_cast<T>(2) * (qxx + qyy);

	return result;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Mat<4, 4, T> TRAP::Math::Mat4Cast(const tQuat<T>& q) noexcept
{
	return Mat<4, 4, T>(Mat3Cast(q));
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::QuaternionCast(const Mat<3, 3, T>& m)
{
	const T fourXSquaredMinus1 = std::get<0>(std::get<0>(m)) - std::get<1>(std::get<1>(m)) - std::get<2>(std::get<2>(m));
	const T fourYSquaredMinus1 = std::get<1>(std::get<1>(m)) - std::get<0>(std::get<0>(m)) - std::get<2>(std::get<2>(m));
	const T fourZSquaredMinus1 = std::get<2>(std::get<2>(m)) - std::get<0>(std::get<0>(m)) - std::get<1>(std::get<1>(m));
	const T fourWSquaredMinus1 = std::get<0>(std::get<0>(m)) + std::get<1>(std::get<1>(m)) + std::get<2>(std::get<2>(m));

	u32 biggestIndex = 0;
	T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1u;
	}
	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2u;
	}
	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3u;
	}

	const T biggestVal = Sqrt(fourBiggestSquaredMinus1 + static_cast<T>(1))* static_cast<T>(0.5);
	const T mult = static_cast<T>(0.25) / biggestVal;

	switch (biggestIndex)
	{
	case 0:
		return tQuat<T>(biggestVal, (std::get<2>(std::get<1>(m)) - std::get<1>(std::get<2>(m))) * mult,
		                            (std::get<0>(std::get<2>(m)) - std::get<2>(std::get<0>(m))) * mult,
		                            (std::get<1>(std::get<0>(m)) - std::get<0>(std::get<1>(m))) * mult);

	case 1:
		return tQuat<T>((std::get<2>(std::get<1>(m)) - std::get<1>(std::get<2>(m))) * mult, biggestVal,
		                (std::get<1>(std::get<0>(m)) + std::get<0>(std::get<1>(m))) * mult,
		                (std::get<0>(std::get<2>(m)) + std::get<2>(std::get<0>(m))) * mult);

	case 2:
		return tQuat<T>((std::get<0>(std::get<2>(m)) - std::get<2>(std::get<0>(m))) * mult,
		                (std::get<1>(std::get<0>(m)) + std::get<0>(std::get<1>(m))) * mult, biggestVal,
		                (std::get<2>(std::get<1>(m)) + std::get<1>(std::get<2>(m))) * mult);

	case 3:
		return tQuat<T>((std::get<1>(std::get<0>(m)) - std::get<0>(std::get<1>(m))) * mult,
		                (std::get<0>(std::get<2>(m)) + std::get<2>(std::get<0>(m))) * mult,
						(std::get<2>(std::get<1>(m)) + std::get<1>(std::get<2>(m))) * mult,
		                biggestVal);

	default:
		std::unreachable();
	}
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::QuaternionCast(const Mat<4, 4, T>& m)
{
	return QuaternionCast(Mat<3, 3, T>(m));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, bool> TRAP::Math::LessThan(const tQuat<T>& x, const tQuat<T>& y) noexcept
{
	Vec<4, bool> result(false, false, false, false);
	for (usize i = 0u; i < x.Length(); ++i)
		result[i] = x[i] < y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, bool> TRAP::Math::LessThanEqual(const tQuat<T>& x, const tQuat<T>& y) noexcept
{
	Vec<4, bool> result(false, false, false, false);
	for (usize i = 0u; i < x.Length(); ++i)
		result[i] = x[i] <= y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, bool> TRAP::Math::GreaterThan(const tQuat<T>& x, const tQuat<T>& y) noexcept
{
	Vec<4, bool> result(false, false, false, false);
	for (usize i = 0u; i < x.Length(); ++i)
		result[i] = x[i] > y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, bool> TRAP::Math::GreaterThanEqual(const tQuat<T>& x, const tQuat<T>& y) noexcept
{
	Vec<4, bool> result(false, false, false, false);
	for (usize i = 0u; i < x.Length(); ++i)
		result[i] = x[i] >= y[i];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::QuaternionLookAt(const Vec<3, T>& direction, const Vec<3, T>& up)
{
	//RH
	Mat<3, 3, T> result;

	std::get<2>(result) = -direction;
	const Vec<3, T> right = Cross(up, std::get<2>(result));
	std::get<0>(result) = right * InverseSqrt(Max(static_cast<T>(0.00001), Dot(right, right)));
	std::get<1>(result) = Cross(std::get<2>(result), std::get<0>(result));

	return QuaternionCast(result);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const tQuat<T>& x, const tQuat<T>& y) noexcept
{
	Vec<4, bool> result{};
	for (usize i = 0u; i < x.Length(); ++i)
		result[i] = x[i] == y[i];

	return result;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, bool> TRAP::Math::Equal(const tQuat<T>& x, const tQuat<T>& y, const T epsilon)
{
	Vec<4, T> v(x.x() - y.x(), x.y() - y.y(), x.z() - y.z(), x.w() - y.w());
	return LessThan(Abs(v), Vec<4, T>(epsilon));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const tQuat<T>& x, const tQuat<T>& y) noexcept
{
	Vec<4, bool> result{};
	for (usize i = 0u; i < x.Length(); ++i)
		result[i] = x[i] != y[i];

	return result;
}

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<4, bool> TRAP::Math::NotEqual(const tQuat<T>& x, const tQuat<T>& y, const T epsilon)
{
	const Vec<4, T> v(x.x() - y.x(), x.y() - y.y(), x.z() - y.z(), x.w() - y.w());
	return GreaterThanEqual(Abs(v), Vec<4, T>(epsilon));
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Angle(const tQuat<T>& x)
{
	if (Abs(x.w()) > CosOneOverTwo<T>())
	{
		const T a = ASin(Sqrt(x.x() * x.x() + x.y() * x.y() + x.z() * x.z())) * static_cast<T>(2);
		if(x.w() < static_cast<T>(0))
			return PI<T>() * static_cast<T>(2) - a;

		return a;
	}

	return ACos(x.w()) * static_cast<T>(2);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T> TRAP::Math::Axis(const tQuat<T>& x)
{
	const T tmp1 = static_cast<T>(1) - x.w() * x.w();

	if (tmp1 <= static_cast<T>(0))
		return Vec<3, T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

	const T tmp2 = static_cast<T>(1) / Sqrt(tmp1);
	return Vec<3, T>(x.x() * tmp2, x.y() * tmp2, x.z() * tmp2);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::AngleAxis(const T& angle, const Vec<3, T>& v)
{
	const T a(angle);
	const T s = Sin(a * static_cast<T>(0.5));

	return tQuat<T>(Cos(a * static_cast<T>(0.5)), v * s);
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::tQuat<T> TRAP::Math::Rotate(const tQuat<T>& q, const T& angle, const Vec<3, T>& v)
{
	Vec<3, T> tmp = v;

	//Axis of rotation must be normalized
	const T len = Length(tmp);
	if(Abs(len - static_cast<T>(1)) > static_cast<T>(0.001))
	{
		const T oneOverLen = static_cast<T>(1) / len;
		tmp.x() *= oneOverLen;
		tmp.y() *= oneOverLen;
		tmp.z() *= oneOverLen;
	}

	const T angleRad(angle);
	const T sin = Sin(angleRad * static_cast<T>(0.5));

	return q * tQuat<T>(Cos(angleRad * static_cast<T>(0.5)), tmp.x() * sin, tmp.y() * sin, tmp.z() * sin);
}

//-------------------------------------------------------------------------------------------------------------------//
//Vector-------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T> TRAP::Math::XAxis() noexcept
{
	return Vec<3, T>(static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0));
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T> TRAP::Math::YAxis() noexcept
{
	return Vec<3, T>(static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0));
}

template<typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<3, T> TRAP::Math::ZAxis() noexcept
{
	return Vec<3, T>(static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0));
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::LessThan(const Vec<L, T>& x, const Vec<L, T>& y) noexcept
{
	Vec<L, bool> result(true);
	for (u32 i = 0u; i < L; ++i)
		result[i] = x[i] < y[i];
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::LessThanEqual(const Vec<L, T>& x, const Vec<L, T>& y) noexcept
{
	Vec<L, bool> result(true);
	for (u32 i = 0u; i < L; ++i)
		result[i] = x[i] <= y[i];
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::GreaterThan(const Vec<L, T>& x, const Vec<L, T>& y) noexcept
{
	Vec<L, bool> result(true);
	for (u32 i = 0u; i < L; ++i)
		result[i] = x[i] > y[i];
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::GreaterThanEqual(const Vec<L, T>& x, const Vec<L, T>& y) noexcept
{
	Vec<L, bool> result(true);
	for (u32 i = 0u; i < L; ++i)
		result[i] = x[i] >= y[i];
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::Equal(const Vec<L, T>& x, const Vec<L, T>& y) noexcept
{
	Vec<L, bool> result(true);
	for (u32 i = 0u; i < L; ++i)
		result[i] = x[i] == y[i];
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template <u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::Equal(const Vec<L, T>& x, const Vec<L, T>& y, const T epsilon)
{
	return Equal(x, y, Vec<L, T>(epsilon));
}

//-------------------------------------------------------------------------------------------------------------------//

template <u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::Equal(const Vec<L, T>& x, const Vec<L, T>& y,
                                                                   const Vec<L, T>& epsilon)
{
	if constexpr(std::signed_integral<T> || std::floating_point<T>)
	{
		return LessThanEqual(Abs(x - y), epsilon);
	}
	else if constexpr(std::unsigned_integral<T>)
	{
		return LessThanEqual(x - y, epsilon);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template <u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::NotEqual(const Vec<L, T>& x, const Vec<L, T>& y) noexcept
{
	Vec<L, bool> result(true);
	for (u32 i = 0u; i < L; ++i)
		result[i] = x[i] != y[i];
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::NotEqual(const Vec<L, T>& x, const Vec<L, T>& y, const T epsilon)
{
	return NotEqual(x, y, Vec<L, T>(epsilon));
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::NotEqual(const Vec<L, T>& x, const Vec<L, T>& y,
                                                                      const Vec<L, T>& epsilon)
{
	if constexpr(std::signed_integral<T> || std::floating_point<T>)
	{
		return GreaterThan(Abs(x - y), epsilon);
	}
	else if constexpr(std::unsigned_integral<T>)
	{
		return GreaterThan(x - y, epsilon);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L>
[[nodiscard]] constexpr bool TRAP::Math::Any(const Vec<L, bool>& v) noexcept
{
	bool result = false;
	for(const bool n : v)
		result = result || n;
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L>
[[nodiscard]] constexpr bool TRAP::Math::All(const Vec<L, bool>& v) noexcept
{
	bool result = true;
	for(const bool n : v)
		result = result && n;
	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::Not(Vec<L, bool> v) noexcept
{
	for(bool& n : v)
		n = !n;

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//
//Trigonometric------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::Radians(const genType degrees)
{
	return degrees * static_cast<genType>(0.01745329251994329576923690768489);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Radians(Vec<L, T> v)
{
	for(T& n : v)
		n = Radians(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::Degrees(const genType radians)
{
	return radians * static_cast<genType>(57.295779513082320876798154814105);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Degrees(Vec<L, T> v)
{
	for(T& n : v)
		n = Degrees(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Sin(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::sin(x);

	return std::sin(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Sin(Vec<L, T> v)
{
	for(T& n : v)
		n = Sin(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Cos(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::cos(x);

	return std::cos(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Cos(Vec<L, T> v)
{
	for(T& n : v)
		n = Cos(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::Tan(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::tan(x);

	return std::tan(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::Tan(Vec<L, T> v)
{
	for(T& n : v)
		n = Tan(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::ASin(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::asin(x);

	return std::asin(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ASin(Vec<L, T> v)
{
	for(T& n : v)
		n = ASin(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::ACos(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::acos(x);

	return std::acos(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ACos(Vec<L, T> v)
{
	for(T& n : v)
		n = ACos(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename genType>
requires std::floating_point<genType>
[[nodiscard]] constexpr genType TRAP::Math::ATan(const genType y, const genType x)
{
	if(std::is_constant_evaluated())
		return gcem::atan2(y, x);

	return std::atan2(y, x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ATan(Vec<L, T> y, const Vec<L, T>& x)
{
	for (u32 i = 0u; i < L; i++)
		y[i] = ATan(y[i], x[i]);

	return y;
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::ATan(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::atan(x);

	return std::atan(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ATan(Vec<L, T> v)
{
	for(T& n : v)
		n = ATan(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::SinH(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::sinh(x);

	return std::sinh(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::SinH(Vec<L, T> v)
{
	for(T& n : v)
		n = SinH(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::CosH(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::cosh(x);

	return std::cosh(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::CosH(Vec<L, T> v)
{
	for(T& n : v)
		n = CosH(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::TanH(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::tanh(x);

	return std::tanh(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::TanH(Vec<L, T> v)
{
	for(T& n : v)
		n = TanH(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::ASinH(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::asinh(x);

	return std::asinh(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ASinH(Vec<L, T> v)
{
	for(T& n : v)
		n = ASinH(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::ACosH(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::acosh(x);

	return std::acosh(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ACosH(Vec<L, T> v)
{
	for(T& n : v)
		n = ACosH(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr T TRAP::Math::ATanH(const T x)
{
	if(std::is_constant_evaluated())
		return gcem::atanh(x);

	return std::atanh(x);
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ATanH(Vec<L, T> v)
{
	for(T& n : v)
		n = ATanH(n);

	return v;
}

//-------------------------------------------------------------------------------------------------------------------//
//Color space--------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires (L == 3 || L == 4) && std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ConvertLinearToSRGB(const Vec<L, T>& colorLinear)
{
	constexpr T gammaCorrection = static_cast<T>(0.41666);

	if constexpr (L == 4)
	{
		const Vec<3, T> clampedColor(Clamp(colorLinear, static_cast<T>(0), static_cast<T>(1)));

		//Don't modify the alpha value
		return Vec<L, T>(Mix(Pow(clampedColor, Vec<3, T>(gammaCorrection)) * static_cast<T>(1.055) - static_cast<T>(0.055),
	                     clampedColor * static_cast<T>(12.92), LessThan(clampedColor, Vec<3, T>(static_cast<T>(0.0031308)))),
						 colorLinear.w());
	}
	else
	{
		const Vec<L, T> clampedColor(Clamp(colorLinear, static_cast<T>(0), static_cast<T>(1)));

		return Mix(Pow(clampedColor, Vec<L, T>(gammaCorrection)) * static_cast<T>(1.055) - static_cast<T>(0.055),
				   clampedColor * static_cast<T>(12.92), LessThan(clampedColor, Vec<L, T>(static_cast<T>(0.0031308))));
	}
}

template<u32 L, typename T>
requires (L == 3 || L == 4) && std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ConvertLinearToSRGB(const Vec<L, T>& colorLinear, const T gamma)
{
	const T gammaCorrection = static_cast<T>(1) / gamma;

	if constexpr (L == 4)
	{
		const Vec<3, T> clampedColor(Clamp(colorLinear, static_cast<T>(0), static_cast<T>(1)));

		//Don't modify the alpha value
		return Vec<L, T>(Mix(Pow(clampedColor, Vec<3, T>(gammaCorrection)) * static_cast<T>(1.055) - static_cast<T>(0.055),
	                     clampedColor * static_cast<T>(12.92), LessThan(clampedColor, Vec<3, T>(static_cast<T>(0.0031308)))),
						 colorLinear.w());
	}
	else
	{
		const Vec<L, T> clampedColor(Clamp(colorLinear, static_cast<T>(0), static_cast<T>(1)));

		return Mix(Pow(clampedColor, Vec<L, T>(gammaCorrection)) * static_cast<T>(1.055) - static_cast<T>(0.055),
				   clampedColor * static_cast<T>(12.92), LessThan(clampedColor, Vec<L, T>(static_cast<T>(0.0031308))));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<u32 L, typename T>
requires (L == 3 || L == 4) && std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ConvertSRGBToLinear(const Vec<L, T>& colorSRGB)
{
	constexpr T gammaCorrection = static_cast<T>(2.4);

	if constexpr (L == 4)
	{
		const Vec<3, T> vec3ColorSRGB(colorSRGB);

		return Vec<L, T>(Mix(Pow((vec3ColorSRGB + static_cast<T>(0.055)) * static_cast<T>(0.94786729857819905213270142180095),
	                             Vec<3, T>(gammaCorrection)), vec3ColorSRGB * static_cast<T>(0.07739938080495356037151702786378),
			             LessThanEqual(vec3ColorSRGB, Vec<3, T>(static_cast<T>(0.04045)))), colorSRGB.w());
	}
	else
	{
		return Mix(Pow((colorSRGB + static_cast<T>(0.055)) * static_cast<T>(0.94786729857819905213270142180095),
					   Vec<L, T>(gammaCorrection)), colorSRGB * static_cast<T>(0.07739938080495356037151702786378),
				   LessThanEqual(colorSRGB, Vec<L, T>(static_cast<T>(0.04045))));
	}
}

template<u32 L, typename T>
requires (L == 3 || L == 4) && std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, T> TRAP::Math::ConvertSRGBToLinear(const Vec<L, T>& colorSRGB, const T gamma)
{
	if constexpr (L == 4)
	{
		const Vec<3, T> vec3ColorSRGB(colorSRGB);

		return Vec<L, T>(Mix(Pow((vec3ColorSRGB + static_cast<T>(0.055)) * static_cast<T>(0.94786729857819905213270142180095),
	                             Vec<3, T>(gamma)), vec3ColorSRGB * static_cast<T>(0.07739938080495356037151702786378),
			             LessThanEqual(vec3ColorSRGB, Vec<3, T>(static_cast<T>(0.04045)))), colorSRGB.w());
	}
	else
	{
		return Mix(Pow((colorSRGB + static_cast<T>(0.055)) * static_cast<T>(0.94786729857819905213270142180095),
					   Vec<L, T>(gamma)), colorSRGB * static_cast<T>(0.07739938080495356037151702786378),
				   LessThanEqual(colorSRGB, Vec<L, T>(static_cast<T>(0.04045))));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr bool TRAP::Math::IsOdd(const T x)
{
	return (x & static_cast<T>(1u)) != 0;
}

template<u32 L, typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::IsOdd(const Vec<L, T>& x) noexcept
{
	Vec<L, bool> result{};

	for(u32 i = 0u; i < L; ++i)
		result[i] = (x[i] & static_cast<T>(1u)) != 0;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::integral<T>
[[nodiscard]] constexpr bool TRAP::Math::IsEven(const T x)
{
	return !IsOdd(x);
}

template<u32 L, typename T>
requires std::integral<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::IsEven(const Vec<L, T>& x) noexcept
{
	Vec<L, bool> result{};

	for(u32 i = 0u; i < L; ++i)
		result[i] = !IsOdd(x[i]);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr bool TRAP::Math::IsFinite(const T x) noexcept
{
	return (!IsNaN(x)) && (!IsInf(x));
}

template<u32 L, typename T>
requires std::floating_point<T>
[[nodiscard]] constexpr TRAP::Math::Vec<L, bool> TRAP::Math::IsFinite(const Vec<L, T>& x) noexcept
{
	Vec<L, bool> result{};

	for(u32 i = 0u; i < L; ++i)
		result[i] = TRAP::Math::IsFinite(x[i]);

	return result;
}

#endif /*TRAP_MATH_H*/
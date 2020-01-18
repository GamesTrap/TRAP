#ifndef _TRAP_MATHSFUNC_H_
#define _TRAP_MATHSFUNC_H_

#include <cmath>

constexpr float TP_PI = 3.14159265358f;

namespace TRAP::Math
{
	template<typename T>
	T ToRadians(T degrees);
	template<typename T>
	T ToDegrees(T radians);

	template<typename T>
	int32_t Sign(T value);

	template<typename T>
	T Sin(T angle);
	template<typename T>
	T Cos(T angle);
	template<typename T>
	T Tan(T angle);

	template<typename T>
	T Sqrt(T value);
	template<typename T>
	T RSqrt(T value);

	template<typename T>
	T ASin(T angle);
	template<typename T>
	T ACos(T angle);
	template<typename T>
	T ATan(T angle);
	template<typename T>
	T ATan2(T y, T x);

	template<typename T>
	constexpr const T& Min(const T& value, const T& minimum);
	template<typename T>
	constexpr const T& Max(const T& value, const T& maximum);

	template<typename T>
	constexpr const T& Clamp(const T& value, const T& minimum, const T& maximum);

	template<typename T>
	T Abs(T value);

	template<typename T>
	constexpr bool IsPow2(T x);
	
	template<typename T>
	T Round(const T& arg);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ToRadians(const T degrees)
{
	return (degrees * (TP_PI / T(180)));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ToDegrees(const T radians)
{
	return radians * (T(180) / TP_PI);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
int32_t TRAP::Math::Sign(const T value)
{
	return (T(0) < value) - (value < T(0));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Sin(T angle)
{
	return std::sin(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Cos(T angle)
{
	return std::cos(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Tan(T angle)
{
	return std::tan(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Sqrt(T value)
{
	return std::sqrt(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::RSqrt(T value)
{
	return 1.0 / std::sqrt(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ASin(T angle)
{
	return std::asin(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ACos(T angle)
{
	return std::acos(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ATan(T angle)
{
	return std::atan(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ATan2(T y, T x)
{
	return std::atan2(y, x);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr const T& TRAP::Math::Min(const T& value, const T& minimum)
{
	return std::min(value, minimum);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr const T& TRAP::Math::Max(const T& value, const T& maximum)
{
	return std::max(value, maximum);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr const T& TRAP::Math::Clamp(const T& value, const T& minimum, const T& maximum)
{
	return std::clamp(value, minimum, maximum);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Abs(T value)
{
	return std::abs(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr bool TRAP::Math::IsPow2(const T x)
{
	return !(x == T(0)) && !(x & (x - T(1)));
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Round(const T& arg)
{
	return std::round(arg);
}

#endif /*_TRAP_MATHSFUNC_H_*/
#ifndef _TRAP_MATHSFUNC_H_
#define _TRAP_MATHSFUNC_H_

#include <cmath>

constexpr float TP_PI = 3.14159265358f;

namespace TRAP::Math
{
	float ToRadians(float degrees);
	float ToDegrees(float radians);

	int32_t Sign(float value);

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
	T ASin(T value);
	template<typename T>
	T ACos(T value);
	template<typename T>
	T ATan(T value);
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

	constexpr bool IsPow2(const uint32_t x);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Sin(T angle)
{
	TP_ASSERT(false, "Invalid type!");
	return 0;
}
template<>
inline float TRAP::Math::Sin(float angle)
{
	return std::sin(angle);
}
template<>
inline double TRAP::Math::Sin(double angle)
{
	return std::sin(angle);
}
template<>
inline long double TRAP::Math::Sin(long double angle)
{
	return std::sin(angle);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Cos(T angle)
{
	TP_ASSERT(false, "Invalid type!");
	return 0;
}
template<>
inline float TRAP::Math::Cos(float angle)
{
	return std::cos(angle);
}
template<>
inline double TRAP::Math::Cos(double angle)
{
	return std::cos(angle);
}
template<>
inline long double TRAP::Math::Cos(long double angle)
{
	return std::cos(angle);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Tan(T angle)
{
	TP_ASSERT(false, "Invalid type!");
	return 0;
}
template<>
inline float TRAP::Math::Tan(float angle)
{
	return std::tan(angle);
}
template<>
inline double TRAP::Math::Tan(double angle)
{
	return std::tan(angle);
}
template<>
inline long double TRAP::Math::Tan(long double angle)
{
	return std::tan(angle);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Sqrt(T value)
{
	TP_ASSERT(false, "Invalid type!");
	return 0;
}
template<>
inline float TRAP::Math::Sqrt(float value)
{
	return std::sqrt(value);
}
template<>
inline double TRAP::Math::Sqrt(double value)
{
	return std::sqrt(value);
}
template<>
inline long double TRAP::Math::Sqrt(long double value)
{
	return std::sqrt(value);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::RSqrt(T value)
{
	TP_ASSERT(false, "Invalid type!");
	return 0;
}
template<>
inline float TRAP::Math::RSqrt(float value)
{
	return 1.0f / std::sqrt(value);
}
template<>
inline double TRAP::Math::RSqrt(double value)
{
	return 1.0 / std::sqrt(value);
}
template<>
inline long double TRAP::Math::RSqrt(long double value)
{
	return 1.0 / std::sqrt(value);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ASin(T value)
{
	TP_ASSERT(false, "Invalid type!");
	return 0;
}
template<>
inline float TRAP::Math::ASin(float value)
{
	return std::asin(value);
}
template<>
inline double TRAP::Math::ASin(double value)
{
	return std::asin(value);
}
template<>
inline long double TRAP::Math::ASin(long double value)
{
	return std::asin(value);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ACos(T value)
{
	TP_ASSERT(false, "Invalid type!");
	return 0;
}
template<>
inline float TRAP::Math::ACos(float value)
{
	return std::acos(value);
}
template<>
inline double TRAP::Math::ACos(double value)
{
	return std::acos(value);
}
template<>
inline long double TRAP::Math::ACos(long double value)
{
	return std::acos(value);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ATan(T value)
{
	TP_ASSERT(false, "Invalid type!");
	return 0;
}
template<>
inline float TRAP::Math::ATan(float value)
{
	return std::atan(value);
}
template<>
inline double TRAP::Math::ATan(double value)
{
	return std::atan(value);
}
template<>
inline long double TRAP::Math::ATan(long double value)
{
	return std::atan(value);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::ATan2(T y, T x)
{
	TP_ASSERT(false, "Invalid type!");
	return 0;
}
template<>
inline float TRAP::Math::ATan2(float y, float x)
{
	return std::atan2(y, x);
}
template<>
inline double TRAP::Math::ATan2(double y, double x)
{
	return std::atan2(y, x);
}
template<>
inline long double TRAP::Math::ATan2(long double y, long double x)
{
	return std::atan2(y, x);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr const T& TRAP::Math::Min(const T& value, const T& minimum)
{
	return std::min(value, minimum);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr const T& TRAP::Math::Max(const T& value, const T& maximum)
{
	return std::max(value, maximum);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
constexpr const T& TRAP::Math::Clamp(const T& value, const T& minimum, const T& maximum)
{
	return std::clamp(value, minimum, maximum);
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Math::Abs(T value)
{
	TP_ASSERT(false, "Invalid type!");
	return 0;
}
template<>
inline int32_t TRAP::Math::Abs(int32_t value)
{
	return std::abs(value);
}
template<>
inline long TRAP::Math::Abs(long value)
{
	return std::abs(value);
}
template<>
inline long long TRAP::Math::Abs(long long value)
{
	return std::abs(value);
}
template<>
inline float TRAP::Math::Abs(float value)
{
	return std::abs(value);
}
template<>
inline double TRAP::Math::Abs(double value)
{
	return std::abs(value);
}
template<>
inline long double TRAP::Math::Abs(long double value)
{
	return std::abs(value);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::Math::IsPow2(const uint32_t x)
{
	return !(x == 0) && !(x & (x - 1));
}

#endif /*_TRAP_MATHSFUNC_H_*/
#include "TRAPPCH.h"
#include "MathsFunc.h"

namespace TRAP::Maths
{
	float ToRadians(const float degrees)
	{
		return static_cast<float>(degrees * (TP_PI / 180.0f));
	}

	float ToDegrees(const float radians)
	{
		return static_cast<float>(radians * (180.0f / TP_PI));
	}

	int32_t Sign(const float value)
	{
		return (value > 0) - (value < 0);
	}

	float Sin(const float angle)
	{
		return ::sin(angle);
	}

	float Cos(const float angle)
	{
		return ::cos(angle);
	}

	float Tan(const float angle)
	{
		return ::tan(angle);
	}

	float Sqrt(const float value)
	{
		return ::sqrt(value);
	}

	float RSqrt(const float value)
	{
		return 1.0f / ::sqrt(value);
	}

	float ASin(const float value)
	{
		return ::asin(value);
	}

	float ACos(const float value)
	{
		return ::acos(value);
	}

	float ATan(const float value)
	{
		return ::atan(value);
	}

	float ATan2(const float y, const float x)
	{
		return ::atan2(y, x);
	}

	float Min(float value, float minimum)
	{
		return (value < minimum) ? minimum : value;
	}

	float Max(float value, float maximum)
	{
		return (value > maximum) ? maximum : value;
	}

	float Clamp(float value, float minimum, float maximum)
	{
		return (value > minimum) ? (value < maximum) ? value : maximum : minimum;
	}
}

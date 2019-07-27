#include "TRAPPCH.h"
#include "MathsFunc.h"

#include <cmath>

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::ToRadians(const float degrees)
{
	return static_cast<float>(degrees * (TP_PI / 180.0f));
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::ToDegrees(const float radians)
{
	return static_cast<float>(radians * (180.0f / TP_PI));
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Maths::Sign(const float value)
{
	return (value > 0) - (value < 0);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::Sin(const float angle)
{
	return ::sin(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::Cos(const float angle)
{
	return ::cos(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::Tan(const float angle)
{
	return ::tan(angle);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::Sqrt(const float value)
{
	return ::sqrt(value);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::RSqrt(const float value)
{
	return 1.0f / ::sqrt(value);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::ASin(const float value)
{
	return ::asin(value);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::ACos(const float value)
{
	return ::acos(value);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::ATan(const float value)
{
	return ::atan(value);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::ATan2(const float y, const float x)
{
	return ::atan2(y, x);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::Min(float value, float minimum)
{
	return (value < minimum) ? minimum : value;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::Max(float value, float maximum)
{
	return (value > maximum) ? maximum : value;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Maths::Clamp(float value, float minimum, float maximum)
{
	return (value > minimum) ? (value < maximum) ? value : maximum : minimum;
}
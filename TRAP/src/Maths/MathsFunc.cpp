#include "TRAPPCH.h"
#include "MathsFunc.h"

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Math::ToRadians(const float degrees)
{
	return static_cast<float>(degrees * (TP_PI / 180.0f));
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Math::ToDegrees(const float radians)
{
	return static_cast<float>(radians * (180.0f / TP_PI));
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Math::Sign(const float value)
{
	return (value > 0) - (value < 0);
}
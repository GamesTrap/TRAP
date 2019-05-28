#pragma once

constexpr float TP_PI = 3.14159265358f;

namespace TRAP::Maths
{
	float ToRadians(float degrees);
	float ToDegrees(float radians);

	int32_t Sign(float value);

	float Sin(float angle);
	float Cos(float angle);
	float Tan(float angle);

	float Sqrt(float value);
	float RSqrt(float value);

	float ASin(float value);
	float ACos(float value);
	float ATan(float value);
	float ATan2(float y, float x);

	float Min(float value, float minimum);
	float Max(float value, float maximum);

	float Clamp(float value, float minimum, float maximum);
}
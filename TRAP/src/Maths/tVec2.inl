#ifndef _TRAP_TVEC2_INL_
#define _TRAP_TVEC2_INL_

#include "Maths.h"

#include <ostream>

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>::tVec2()
	: x(0), y(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>::tVec2(const T& scalar)
	: x(scalar), y(scalar)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>::tVec2(const T& x, const T& y)
	: x(x), y(y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template<class T>
TRAP::Maths::tVec2<T>::tVec2(const tVec3<T>& vector)
	: x(vector.x), y(vector.y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template<class T>
TRAP::Maths::tVec2<T>::tVec2(const tVec4<T>& vector)
	: x(vector.x), y(vector.y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Add(const tVec2<T>& other)
{
	x += other.x;
	y += other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Subtract(const tVec2<T>& other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Multiply(const tVec2<T>& other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Divide(const tVec2<T>& other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Add(const tVec3<T>& other)
{
	x += other.x;
	y += other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Subtract(const tVec3<T>& other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Multiply(const tVec3<T>& other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Divide(const tVec3<T>& other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Add(const tVec4<T>& other)
{
	x += other.x;
	y += other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Subtract(const tVec4<T>& other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Multiply(const tVec4<T>& other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Divide(const tVec4<T>& other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Add(const T& value)
{
	x += value;
	y += value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Subtract(const T& value)
{
	x -= value;
	y -= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Multiply(const T& value)
{
	x *= value;
	y *= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Divide(const T& value)
{
	x /= value;
	y /= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Add(const T& x, const T& y)
{
	this.x += x;
	this.y += y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Subtract(const T& x, const T& y)
{
	this.x -= x;
	this.y -= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Multiply(const T& x, const T& y)
{
	this.x *= x;
	this.y *= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::Divide(const T& x, const T& y)
{
	this.x /= x;
	this.y /= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator+(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec2<T>& right)
{
	return left.Add(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator-(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec2<T>& right)
{
	return left.Subtract(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator*(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec2<T>& right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator/(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec2<T>& right)
{
	return left.Divide(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator+(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec3<T>& right)
{
	return left.Add(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator-(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec3<T>& right)
{
	return left.Subtract(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator*(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec3<T>& right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator/(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec3<T>& right)
{
	return left.Divide(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator+(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec4<T>& right)
{
	return left.Add(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator-(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec4<T>& right)
{
	return left.Subtract(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator*(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec4<T>& right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator/(TRAP::Maths::tVec2<T> left, const TRAP::Maths::tVec4<T>& right)
{
	return left.Divide(right);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator+(TRAP::Maths::tVec2<T> left, T value)
{
	return tVec2<T>(left.x + value, left.y + value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator-(TRAP::Maths::tVec2<T> left, T value)
{
	return tVec2<T>(left.x - value, left.y - value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator*(TRAP::Maths::tVec2<T> left, T value)
{
	return tVec2<T>(left.x * value, left.y * value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> operator/(TRAP::Maths::tVec2<T> left, T value)
{
	return tVec2<T>(left.x / value, left.y / value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator==(const tVec2<T>& other) const
{
	return x == other.x && y == other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator!=(const tVec2<T>& other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator==(const tVec3<T>& other) const
{
	return x == other.x && y == other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator!=(const tVec3<T>& other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator==(const tVec4<T>& other) const
{
	return x == other.x && y == other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator!=(const tVec4<T>& other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator+=(const tVec2<T>& other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator-=(const tVec2<T>& other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator*=(const tVec2<T>& other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator/=(const tVec2<T>& other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator+=(const tVec3<T>& other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator-=(const tVec3<T>& other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator*=(const tVec3<T>& other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator/=(const tVec3<T>& other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator+=(const tVec4<T>& other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator-=(const tVec4<T>& other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator*=(const tVec4<T>& other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator/=(const tVec4<T>& other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator+=(T value)
{
	return Add(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator-=(T value)
{
	return Subtract(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator*=(T value)
{
	return Multiply(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T>& TRAP::Maths::tVec2<T>::operator/=(T value)
{
	return Divide(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator<(const tVec2<T>& other) const
{
	return x < other.x && y < other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator<=(const tVec2<T>& other) const
{
	return x <= other.x && y <= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator>(const tVec2<T>& other) const
{
	return x > other.x && y > other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator>=(const tVec2<T>& other) const
{
	return x >= other.x && y >= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator<(const tVec3<T>& other) const
{
	return x < other.x && y < other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator<=(const tVec3<T>& other) const
{
	return x <= other.x && y <= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator>(const tVec3<T>& other) const
{
	return x > other.x && y > other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator>=(const tVec3<T>& other) const
{
	return x >= other.x && y >= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator<(const tVec4<T>& other) const
{
	return x < other.x && y < other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator<=(const tVec4<T>& other) const
{
	return x <= other.x && y <= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator>(const tVec4<T>& other) const
{
	return x > other.x && y > other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec2<T>::operator>=(const tVec4<T>& other) const
{
	return x >= other.x && y >= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec2<T>::Dot(const tVec2<T>& other) const
{
	return x * other.x + y * other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec2<T>::Dot(const tVec3<T>& other) const
{
	return x * other.x + y * other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec2<T>::Dot(const tVec4<T>& other) const
{
	return x * other.x + y * other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec2<T>::Magnitude() const
{
	return Sqrt(x * x + y * y);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec2<T> TRAP::Maths::tVec2<T>::Normalize() const
{
	float length = Magnitude();

	return tVec2<T>(x / length, y / length);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec2<T>::Distance(const tVec2<T>& other) const
{
	const float a = x - other.x;
	const float b = y - other.y;

	return Sqrt(a * a + b * b);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec2<T>::Distance(const tVec3<T>& other) const
{
	const float a = x - other.x;
	const float b = y - other.y;

	return Sqrt(a * a + b * b);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec2<T>::Distance(const tVec4<T>& other) const
{
	const float a = x - other.x;
	const float b = y - other.y;

	return Sqrt(a * a + b * b);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
std::string TRAP::Maths::tVec2<T>::ToString() const
{
	std::stringstream result;
	result << "Vec2: (" << x << ", " << y << ')';

	return result.str();
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
std::ostream& operator<<(std::ostream& stream, const TRAP::Maths::tVec2<T>& vector)
{
	return stream << vector.ToString();
}

#endif /*_TRAP_TVEC2_INL_*/
#ifndef _TRAP_TVEC3_INL_
#define _TRAP_TVEC3_INL_

#include "Maths.h"

#include <ostream>

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>::tVec3()
	: x(0), y(0), z(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>::tVec3(const T& scalar)
	: x(scalar), y(scalar), z(scalar)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>::tVec3(const T& x, const T& y)
	: x(x), y(y), z(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>::tVec3(const T& x, const T& y, const T& z)
	: x(x), y(y), z(z)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>::tVec3(const tVec2<T>& xy, const T& z)
	: x(xy.x), y(xy.y), z(z)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>::tVec3(const T& x, const tVec2<T>& yz)
	: x(x), y(yz.x), z(yz.y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>::tVec3(const tVec4<T>& vector)
	: x(vector.x), y(vector.y), z(vector.z)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::Up()
{
	return tVec3<T>(0.0f, 1.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::Down()
{
	return tVec3<T>(0.0f, -1.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::Left()
{
	return tVec3<T>(-1.0f, 0.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::Right()
{
	return tVec3<T>(1.0f, 1.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::Zero()
{
	return tVec3<T>(0.0f, 0.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::XAxis()
{
	return tVec3<T>(1.0f, 0.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::YAxis()
{
	return tVec3<T>(0.0f, 1.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::ZAxis()
{
	return tVec3<T>(0.0f, 0.0f, 1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Add(const tVec2<T>& other)
{
	x += other.x;
	y += other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Subtract(const tVec2<T>& other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Multiply(const tVec2<T>& other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Divide(const tVec2<T>& other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Add(const tVec3<T>& other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Subtract(const tVec3<T>& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Multiply(const tVec3<T>& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Divide(const tVec3<T>& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Add(const tVec4<T>& other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Subtract(const tVec4<T>& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Multiply(const tVec4<T>& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Divide(const tVec4<T>& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Add(const T& value)
{
	x += value;
	y += value;
	z += value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Subtract(const T& value)
{
	x -= value;
	y -= value;
	z -= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Multiply(const T& value)
{
	x *= value;
	y *= value;
	z *= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Divide(const T& value)
{
	x /= value;
	y /= value;
	z /= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Add(const T& x, const T& y)
{
	this.x += x;
	this.y += y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Subtract(const T& x, const T& y)
{
	this.x -= x;
	this.y -= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Multiply(const T& x, const T& y)
{
	this.x *= x;
	this.y *= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Divide(const T& x, const T& y)
{
	this.x /= x;
	this.y /= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Add(const T& x, const T& y, const T& z)
{
	this.x += x;
	this.y += y;
	this.z += z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Subtract(const T& x, const T& y, const T& z)
{
	this.x -= x;
	this.y -= y;
	this.z -= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Multiply(const T& x, const T& y, const T& z)
{
	this.x *= x;
	this.y *= y;
	this.z *= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::Divide(const T& x, const T& y, const T& z)
{
	this.x /= x;
	this.y /= y;
	this.z /= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::Multiply(const Mat3& transform) const
{
	return tVec3<T>
		(
			transform.rows[0].x * x + transform.rows[0].y * y + transform.rows[0].z * z,
			transform.rows[1].x * x + transform.rows[1].y * y + transform.rows[1].z * z,
			transform.rows[2].x * x + transform.rows[2].y * y + transform.rows[2].z * z
		);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::Multiply(const Mat4& transform) const
{
	return tVec3<T>
		(
			transform.rows[0].x * x + transform.rows[0].y * y + transform.rows[0].z * z + transform.rows[0].w,
			transform.rows[1].x * x + transform.rows[1].y * y + transform.rows[1].z * z + transform.rows[1].w,
			transform.rows[2].x * x + transform.rows[2].y * y + transform.rows[2].z * z + transform.rows[2].w
		);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator==(const tVec2<T>& other) const
{
	return x == other.x && y == other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator!=(const tVec2<T>& other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator==(const tVec3<T>& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator!=(const tVec3<T>& other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator==(const tVec4<T>& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator!=(const tVec4<T>& other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator+=(const tVec2<T>& other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator-=(const tVec2<T>& other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator*=(const tVec2<T>& other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator/=(const tVec2<T>& other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator+=(const tVec3<T>& other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator-=(const tVec3<T>& other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator*=(const tVec3<T>& other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator/=(const tVec3<T>& other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator+=(const tVec4<T>& other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator-=(const tVec4<T>& other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator*=(const tVec4<T>& other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator/=(const tVec4<T>& other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator+=(T value)
{
	return Add(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator-=(T value)
{
	return Subtract(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator*=(T value)
{
	return Multiply(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T>& TRAP::Maths::tVec3<T>::operator/=(T value)
{
	return Divide(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator<(const tVec2<T>& other) const
{
	return x < other.x && y < other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator<=(const tVec2<T>& other) const
{
	return x <= other.x && y <= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator>(const tVec2<T>& other) const
{
	return x > other.x && y > other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator>=(const tVec2<T>& other) const
{
	return x >= other.x && y >= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator<(const tVec3<T>& other) const
{
	return x < other.x && y < other.y && z < other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator<=(const tVec3<T>& other) const
{
	return x <= other.x && y <= other.y && z <= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator>(const tVec3<T>& other) const
{
	return x > other.x && y > other.y && z > other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator>=(const tVec3<T>& other) const
{
	return x >= other.x && y >= other.y && y >= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator<(const tVec4<T>& other) const
{
	return x < other.x && y < other.y && z < other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator<=(const tVec4<T>& other) const
{
	return x <= other.x && y <= other.y && z <= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator>(const tVec4<T>& other) const
{
	return x > other.x && y > other.y && z > other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec3<T>::operator>=(const tVec4<T>& other) const
{
	return x >= other.x && y >= other.y && z >= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec3<T>::Dot(const tVec2<T>& other) const
{
	return x * other.x + y * other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec3<T>::Dot(const tVec3<T>& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec3<T>::Dot(const tVec4<T>& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::Cross(const tVec3<T>& other) const
{
	return tVec3<T>(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec3<T>::Magnitude() const
{
	return Sqrt(x * x + y * y + z * z);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec3<T> TRAP::Maths::tVec3<T>::Normalize() const
{
	float length = Magnitude();

	return tVec3<T>(x / length, y / length, z / length);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec3<T>::Distance(const tVec2<T>& other) const
{
	const float a = x - other.x;
	const float b = y - other.y;

	return Sqrt(a * a + b * b);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec3<T>::Distance(const tVec3<T>& other) const
{
	const float a = x - other.x;
	const float b = y - other.y;
	const float c = z - other.z;

	return Sqrt(a * a + b * b + c * c);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec3<T>::Distance(const tVec4<T>& other) const
{
	const float a = x - other.x;
	const float b = y - other.y;
	const float c = z - other.z;

	return Sqrt(a * a + b * b + c * c);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
std::string TRAP::Maths::tVec3<T>::ToString() const
{
	std::stringstream result;
	result << "Vec3: (" << x << ", " << y << ", " << z << ')';

	return result.str();
}

#endif /*_TRAP_TVEC3_INL_*/
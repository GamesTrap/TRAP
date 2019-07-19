#ifndef _TRAP_TVEC4_INL_
#define _TRAP_TVEC4_INL_

#include "Maths.h"

#include <ostream>

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4()
	: x(0), y(0), z(0), w(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const T& scalar)
	: x(scalar), y(scalar), z(scalar), w(scalar)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const T& x, const T& y)
	: x(x), y(y), z(0), w(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const T& x, const T& y, const T& z)
	: x(x), y(y), z(z), w(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const T& x, const T& y, const T& z, const T& w)
	: x(x), y(y), z(z), w(w)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const tVec2<T>& xy, const T& z)
	: x(xy.x), y(xy, y), z(z), w(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const T& x, const tVec2<T>& yz)
	: x(x), y(yz.x), z(yz.y), w(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const tVec2<T>& xy, const T& z, const T& w)
	: x(xy.x), y(xy.y), z(z), w(w)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const T& x, const T& y, const tVec2<T>& zw)
	: x(x), y(y), z(zw.x), w(zw.y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const tVec2<T>& xy, const tVec2<T>& zw)
	: x(xy.x), y(xy.y), z(zw.x), w(zw.y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const tVec3<T>& xyz, const T& w)
	: x(xyz.x), y(xyz.y), z(xyz.z), w(w)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>::tVec4(const T& x, const tVec3<T>& yzw)
	: x(x), y(yzw.x), z(yzw.y), w(yzw.z)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Add(const tVec2<T>& other)
{
	x += other.x;
	y += other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Subtract(const tVec2<T>& other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Multiply(const tVec2<T>& other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Divide(const tVec2<T>& other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Add(const tVec3<T>& other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Subtract(const tVec3<T>& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Multiply(const tVec3<T>& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Divide(const tVec3<T>& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Add(const tVec4<T>& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Subtract(const tVec4<T>& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Multiply(const tVec4<T>& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	w *= other.w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Divide(const tVec4<T>& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	w /= other.w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Add(const T& value)
{
	x += value;
	y += value;
	z += value;
	w += value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Subtract(const T& value)
{
	x -= value;
	y -= value;
	z -= value;
	w -= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Multiply(const T& value)
{
	x *= value;
	y *= value;
	z *= value;
	w *= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Divide(const T& value)
{
	x /= value;
	y /= value;
	z /= value;
	w /= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Add(const T& x, const T& y)
{
	this.x += x;
	this.y += y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Subtract(const T& x, const T& y)
{
	this.x -= x;
	this.y -= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Multiply(const T& x, const T& y)
{
	this.x *= x;
	this.y *= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Divide(const T& x, const T& y)
{
	this.x /= x;
	this.y /= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Add(const T& x, const T& y, const T& z)
{
	this.x += x;
	this.y += y;
	this.z += z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Subtract(const T& x, const T& y, const T& z)
{
	this.x -= x;
	this.y -= y;
	this.z -= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Multiply(const T& x, const T& y, const T& z)
{
	this.x *= x;
	this.y *= y;
	this.z *= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Divide(const T& x, const T& y, const T& z)
{
	this.x /= x;
	this.y /= y;
	this.z /= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Add(const T& x, const T& y, const T& z, const T& w)
{
	this.x += x;
	this.y += y;
	this.z += z;
	this.w += w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Subtract(const T& x, const T& y, const T& z, const T& w)
{
	this.x -= x;
	this.y -= y;
	this.z -= z;
	this.w -= w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Multiply(const T& x, const T& y, const T& z, const T& w)
{
	this.x *= x;
	this.y *= y;
	this.z *= z;
	this.w *= w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::Divide(const T& x, const T& y, const T& z, const T& w)
{
	this.x /= x;
	this.y /= y;
	this.z /= z;
	this.w /= w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T> TRAP::Maths::tVec4<T>::Multiply(const Mat3& transform) const
{
	return tVec4<T>
		(
			transform.rows[0].x * x + transform.rows[0].y * y + transform.rows[0].z * z + w,
			transform.rows[1].x * x + transform.rows[1].y * y + transform.rows[1].z * z + w,
			transform.rows[2].x * x + transform.rows[2].y * y + transform.rows[2].z * z + w,
			transform.rows[3].x * x + transform.rows[3].y * y + transform.rows[3].z * z + w
			);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T> TRAP::Maths::tVec4<T>::Multiply(const Mat4& transform) const
{
	return tVec4<T>
		(
			transform.rows[0].x * x + transform.rows[0].y * y + transform.rows[0].z * z + transform.rows[0].w * w,
			transform.rows[1].x * x + transform.rows[1].y * y + transform.rows[1].z * z + transform.rows[1].w * w,
			transform.rows[2].x * x + transform.rows[2].y * y + transform.rows[2].z * z + transform.rows[2].w * w,
			transform.rows[3].x * x + transform.rows[3].y * y + transform.rows[3].z * z + transform.rows[3].w * w
			);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator==(const tVec2<T>& other) const
{
	return x == other.x && y == other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator!=(const tVec2<T>& other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator==(const tVec3<T>& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator!=(const tVec3<T>& other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator==(const tVec4<T>& other) const
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator!=(const tVec4<T>& other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator+=(const tVec2<T>& other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator-=(const tVec2<T>& other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator*=(const tVec2<T>& other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator/=(const tVec2<T>& other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator+=(const tVec3<T>& other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator-=(const tVec3<T>& other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator*=(const tVec3<T>& other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator/=(const tVec3<T>& other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator+=(const tVec4<T>& other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator-=(const tVec4<T>& other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator*=(const tVec4<T>& other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator/=(const tVec4<T>& other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator+=(T value)
{
	return Add(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator-=(T value)
{
	return Subtract(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator*=(T value)
{
	return Multiply(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T>& TRAP::Maths::tVec4<T>::operator/=(T value)
{
	return Divide(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator<(const tVec2<T>& other) const
{
	return x < other.x && y < other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator<=(const tVec2<T>& other) const
{
	return x <= other.x && y <= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator>(const tVec2<T>& other) const
{
	return x > other.x && y > other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator>=(const tVec2<T>& other) const
{
	return x >= other.x && y >= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator<(const tVec3<T>& other) const
{
	return x < other.x && y < other.y && z < other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator<=(const tVec3<T>& other) const
{
	return x <= other.x && y <= other.y && z <= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator>(const tVec3<T>& other) const
{
	return x > other.x && y > other.y && z > other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator>=(const tVec3<T>& other) const
{
	return x >= other.x && y >= other.y && y >= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator<(const tVec4<T>& other) const
{
	return x < other.x && y < other.y && z < other.z && w < other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator<=(const tVec4<T>& other) const
{
	return x <= other.x && y <= other.y && z <= other.z && w <= other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator>(const tVec4<T>& other) const
{
	return x > other.x && y > other.y && z > other.z && w > other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Maths::tVec4<T>::operator>=(const tVec4<T>& other) const
{
	return x >= other.x && y >= other.y && z >= other.z && w >= other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec4<T>::Dot(const tVec2<T>& other) const
{
	return x * other.x + y * other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec4<T>::Dot(const tVec3<T>& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec4<T>::Dot(const tVec4<T>& other) const
{
	return x * other.x + y * other.y + z * other.z + w * other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec4<T>::Magnitude() const
{
	return Sqrt(x * x + y * y + z * z + w * w);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Maths::tVec4<T> TRAP::Maths::tVec4<T>::Normalize() const
{
	float length = Magnitude();

	return tVec4<T>(x / length, y / length, z / length, w / length);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec4<T>::Distance(const tVec2<T>& other) const
{
	const float a = x - other.x;
	const float b = y - other.y;

	return Sqrt(a * a + b * b);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec4<T>::Distance(const tVec3<T>& other) const
{
	const float a = x - other.x;
	const float b = y - other.y;
	const float c = z - other.z;

	return Sqrt(a * a + b * b + c * c);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Maths::tVec4<T>::Distance(const tVec4<T>& other) const
{
	const float a = x - other.x;
	const float b = y - other.y;
	const float c = z - other.z;
	const float d = w - other.w;

	return Sqrt(a * a + b * b + c * c + d * d);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
std::string TRAP::Maths::tVec4<T>::ToString() const
{
	std::stringstream result;
	result << "Vec4: (" << x << ", " << y << ", " << z << ", " << w << ')';

	return result.str();
}

#endif /*_TRAP_TVEC4_INL_*/
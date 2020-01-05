#ifndef _TRAP_MATH_H_
#define _TRAP_MATH_H_

#include <string>
#include "MathFunctions.h"

namespace TRAP::Math
{
template <class T>
struct tVec2;
template <class T>
struct tVec3;
template <class T>
struct tVec4;
struct Mat3;
struct Mat4;

typedef tVec2<float> Vec2;
typedef tVec2<float> Vec2f;
typedef tVec2<int32_t> Vec2i;
typedef tVec2<uint32_t> Vec2ui;

typedef tVec3<float> Vec3;
typedef tVec3<float> Vec3f;
typedef tVec3<int32_t> Vec3i;
typedef tVec3<uint32_t> Vec3ui;

typedef tVec4<float> Vec4;
typedef tVec4<float> Vec4f;
typedef tVec4<int32_t> Vec4i;
typedef tVec4<uint32_t> Vec4ui;
	
////////////
//Vector 2//
////////////
template <class T>
struct tVec2
{
	T x, y;

	tVec2<T>();
	explicit tVec2(const T &scalar);
	tVec2<T>(const T &x, const T &y);
	explicit tVec2<T>(const tVec3<T> &vector);
	explicit tVec2<T>(const tVec4<T> &vector);

	tVec2<T> &Add(const tVec2<T> &other);
	tVec2<T> &Subtract(const tVec2<T> &other);
	tVec2<T> &Multiply(const tVec2<T> &other);
	tVec2<T> &Divide(const tVec2<T> &other);

	tVec2<T> &Add(const tVec3<T> &other);
	tVec2<T> &Subtract(const tVec3<T> &other);
	tVec2<T> &Multiply(const tVec3<T> &other);
	tVec2<T> &Divide(const tVec3<T> &other);

	tVec2<T> &Add(const tVec4<T> &other);
	tVec2<T> &Subtract(const tVec4<T> &other);
	tVec2<T> &Multiply(const tVec4<T> &other);
	tVec2<T> &Divide(const tVec4<T> &other);

	tVec2<T> &Add(const T &value);
	tVec2<T> &Subtract(const T &value);
	tVec2<T> &Multiply(const T &value);
	tVec2<T> &Divide(const T &value);

	tVec2<T> &Add(const T &x, const T &y);
	tVec2<T> &Subtract(const T &x, const T &y);
	tVec2<T> &Multiply(const T &x, const T &y);
	tVec2<T> &Divide(const T &x, const T &y);

	friend tVec2<T> operator+(tVec2<T> left, const tVec2<T> &right) { return left.Add(right); }
	friend tVec2<T> operator-(tVec2<T> left, const tVec2<T> &right) { return left.Subtract(right); }
	friend tVec2<T> operator*(tVec2<T> left, const tVec2<T> &right) { return left.Multiply(right); }
	friend tVec2<T> operator/(tVec2<T> left, const tVec2<T> &right) { return left.Divide(right); }

	friend tVec2<T> operator+(tVec2<T> left, const tVec3<T> &right) { return left.Add(right); }
	friend tVec2<T> operator-(tVec2<T> left, const tVec3<T> &right) { return left.Subtract(right); }
	friend tVec2<T> operator*(tVec2<T> left, const tVec3<T> &right) { return left.Multiply(right); }
	friend tVec2<T> operator/(tVec2<T> left, const tVec3<T> &right) { return left.Divide(right); }

	friend tVec2<T> operator+(tVec2<T> left, const tVec4<T> &right) { return left.Add(right); }
	friend tVec2<T> operator-(tVec2<T> left, const tVec4<T> &right) { return left.Subtract(right); }
	friend tVec2<T> operator*(tVec2<T> left, const tVec4<T> &right) { return left.Multiply(right); }
	friend tVec2<T> operator/(tVec2<T> left, const tVec4<T> &right) { return left.Divide(right); }

	friend tVec2<T> operator+(tVec2<T> left, T value) { return tVec2<T>(left.x + value, left.y + value); }
	friend tVec2<T> operator-(tVec2<T> left, T value) { return tVec2<T>(left.x - value, left.y - value); }
	friend tVec2<T> operator*(tVec2<T> left, T value) { return tVec2<T>(left.x * value, left.y * value); }
	friend tVec2<T> operator/(tVec2<T> left, T value) { return tVec2<T>(left.x / value, left.y / value); }

	bool operator==(const tVec2<T> &other) const;
	bool operator!=(const tVec2<T> &other) const;

	bool operator==(const tVec3<T> &other) const;
	bool operator!=(const tVec3<T> &other) const;

	bool operator==(const tVec4<T> &other) const;
	bool operator!=(const tVec4<T> &other) const;

	tVec2<T> &operator+=(const tVec2<T> &other);
	tVec2<T> &operator-=(const tVec2<T> &other);
	tVec2<T> &operator*=(const tVec2<T> &other);
	tVec2<T> &operator/=(const tVec2<T> &other);

	tVec2<T> &operator+=(const tVec3<T> &other);
	tVec2<T> &operator-=(const tVec3<T> &other);
	tVec2<T> &operator*=(const tVec3<T> &other);
	tVec2<T> &operator/=(const tVec3<T> &other);

	tVec2<T> &operator+=(const tVec4<T> &other);
	tVec2<T> &operator-=(const tVec4<T> &other);
	tVec2<T> &operator*=(const tVec4<T> &other);
	tVec2<T> &operator/=(const tVec4<T> &other);

	tVec2<T> &operator+=(T value);
	tVec2<T> &operator-=(T value);
	tVec2<T> &operator*=(T value);
	tVec2<T> &operator/=(T value);

	bool operator<(const tVec2<T> &other) const;
	bool operator<=(const tVec2<T> &other) const;
	bool operator>(const tVec2<T> &other) const;
	bool operator>=(const tVec2<T> &other) const;

	bool operator<(const tVec3<T> &other) const;
	bool operator<=(const tVec3<T> &other) const;
	bool operator>(const tVec3<T> &other) const;
	bool operator>=(const tVec3<T> &other) const;

	bool operator<(const tVec4<T> &other) const;
	bool operator<=(const tVec4<T> &other) const;
	bool operator>(const tVec4<T> &other) const;
	bool operator>=(const tVec4<T> &other) const;

	float Dot(const tVec2<T> &other) const;
	float Dot(const tVec3<T> &other) const;
	float Dot(const tVec4<T> &other) const;

	float Magnitude() const;
	tVec2<T> Normalize() const;
	float Distance(const tVec2<T> &other) const;
	float Distance(const tVec3<T> &other) const;
	float Distance(const tVec4<T> &other) const;

	std::string ToString() const;

	friend std::ostream &operator<<(std::ostream &stream, const tVec2<T> &vector) { return stream << vector.ToString(); }
};

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

////////////
//Vector 3//
////////////
template <class T>
struct tVec3
{
	T x, y, z;

	tVec3<T>();
	explicit tVec3<T>(const T &scalar);
	tVec3<T>(const T &x, const T &y);
	tVec3<T>(const T &x, const T &y, const T &z);
	tVec3<T>(const tVec2<T> &xy, const T &z);
	tVec3<T>(const T &x, const tVec2<T> &yz);
	explicit tVec3<T>(const tVec4<T> &vector);

	static tVec3<T> Up();
	static tVec3<T> Down();
	static tVec3<T> Left();
	static tVec3<T> Right();
	static tVec3<T> Zero();

	static tVec3<T> XAxis();
	static tVec3<T> YAxis();
	static tVec3<T> ZAxis();

	tVec3<T> &Add(const tVec2<T> &other);
	tVec3<T> &Subtract(const tVec2<T> &other);
	tVec3<T> &Multiply(const tVec2<T> &other);
	tVec3<T> &Divide(const tVec2<T> &other);

	tVec3<T> &Add(const tVec3<T> &other);
	tVec3<T> &Subtract(const tVec3<T> &other);
	tVec3<T> &Multiply(const tVec3<T> &other);
	tVec3<T> &Divide(const tVec3<T> &other);

	tVec3<T> &Add(const tVec4<T> &other);
	tVec3<T> &Subtract(const tVec4<T> &other);
	tVec3<T> &Multiply(const tVec4<T> &other);
	tVec3<T> &Divide(const tVec4<T> &other);

	tVec3<T> &Add(const T &value);
	tVec3<T> &Subtract(const T &value);
	tVec3<T> &Multiply(const T &value);
	tVec3<T> &Divide(const T &value);

	tVec3<T> &Add(const T &x, const T &y);
	tVec3<T> &Subtract(const T &x, const T &y);
	tVec3<T> &Multiply(const T &x, const T &y);
	tVec3<T> &Divide(const T &x, const T &y);

	tVec3<T> &Add(const T &x, const T &y, const T &z);
	tVec3<T> &Subtract(const T &x, const T &y, const T &z);
	tVec3<T> &Multiply(const T &x, const T &y, const T &z);
	tVec3<T> &Divide(const T &x, const T &y, const T &z);

	tVec3<T> Multiply(const Mat3 &transform) const;
	tVec3<T> Multiply(const Mat4 &transform) const;

	friend tVec3<T> operator+(tVec3<T> left, const tVec2<T> &right) { return left.Add(right); }
	friend tVec3<T> operator-(tVec3<T> left, const tVec2<T> &right) { return left.Subtract(right); }
	friend tVec3<T> operator*(tVec3<T> left, const tVec2<T> &right) { return left.Multiply(right); }
	friend tVec3<T> operator/(tVec3<T> left, const tVec2<T> &right) { return left.Divide(right); }

	friend tVec3<T> operator+(tVec3<T> left, const tVec3<T> &right) { return left.Add(right); };
	friend tVec3<T> operator-(tVec3<T> left, const tVec3<T> &right) { return left.Subtract(right); };
	friend tVec3<T> operator*(tVec3<T> left, const tVec3<T> &right) { return left.Multiply(right); };
	friend tVec3<T> operator/(tVec3<T> left, const tVec3<T> &right) { return left.Divide(right); };

	friend tVec3<T> operator+(tVec3<T> left, const tVec4<T> &right) { return left.Add(right); }
	friend tVec3<T> operator-(tVec3<T> left, const tVec4<T> &right) { return left.Subtract(right); }
	friend tVec3<T> operator*(tVec3<T> left, const tVec4<T> &right) { return left.Multiply(right); }
	friend tVec3<T> operator/(tVec3<T> left, const tVec4<T> &right) { return left.Divide(right); }

	friend tVec3<T> operator+(tVec3<T> left, T value) { return tVec3<T>(left.x + value, left.y + value, left.z + value); }
	friend tVec3<T> operator-(tVec3<T> left, T value) { return tVec3<T>(left.x - value, left.y - value, left.z - value); }
	friend tVec3<T> operator*(tVec3<T> left, T value) { return tVec3<T>(left.x * value, left.y * value, left.z * value); }
	friend tVec3<T> operator/(tVec3<T> left, T value) { return tVec3<T>(left.x / value, left.y / value, left.z / value); }

	bool operator==(const tVec2<T> &other) const;
	bool operator!=(const tVec2<T> &other) const;

	bool operator==(const tVec3<T> &other) const;
	bool operator!=(const tVec3<T> &other) const;

	bool operator==(const tVec4<T> &other) const;
	bool operator!=(const tVec4<T> &other) const;

	tVec3<T> &operator+=(const tVec2<T> &other);
	tVec3<T> &operator-=(const tVec2<T> &other);
	tVec3<T> &operator*=(const tVec2<T> &other);
	tVec3<T> &operator/=(const tVec2<T> &other);

	tVec3<T> &operator+=(const tVec3<T> &other);
	tVec3<T> &operator-=(const tVec3<T> &other);
	tVec3<T> &operator*=(const tVec3<T> &other);
	tVec3<T> &operator/=(const tVec3<T> &other);

	tVec3<T> &operator+=(const tVec4<T> &other);
	tVec3<T> &operator-=(const tVec4<T> &other);
	tVec3<T> &operator*=(const tVec4<T> &other);
	tVec3<T> &operator/=(const tVec4<T> &other);

	tVec3<T> &operator+=(T value);
	tVec3<T> &operator-=(T value);
	tVec3<T> &operator*=(T value);
	tVec3<T> &operator/=(T value);

	bool operator<(const tVec2<T> &other) const;
	bool operator<=(const tVec2<T> &other) const;
	bool operator>(const tVec2<T> &other) const;
	bool operator>=(const tVec2<T> &other) const;

	bool operator<(const tVec3<T> &other) const;
	bool operator<=(const tVec3<T> &other) const;
	bool operator>(const tVec3<T> &other) const;
	bool operator>=(const tVec3<T> &other) const;

	bool operator<(const tVec4<T> &other) const;
	bool operator<=(const tVec4<T> &other) const;
	bool operator>(const tVec4<T> &other) const;
	bool operator>=(const tVec4<T> &other) const;

	float Dot(const tVec2<T> &other) const;
	float Dot(const tVec3<T> &other) const;
	float Dot(const tVec4<T> &other) const;
	tVec3<T> Cross(const tVec3<T> &other) const;

	float Magnitude() const;
	tVec3<T> Normalize() const;
	float Distance(const tVec2<T> &other) const;
	float Distance(const tVec3<T> &other) const;
	float Distance(const tVec4<T> &other) const;

	std::string ToString() const;

	friend std::ostream &operator<<(std::ostream &stream, const tVec3<T> &vector) { return stream << vector.ToString(); }
};

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

////////////
//Vector 4//
////////////
template <class T>
struct tVec4
{
	T x, y, z, w;

	tVec4<T>();
	explicit tVec4<T>(const T &scalar);
	tVec4<T>(const T &x, const T &y);
	tVec4<T>(const T &x, const T &y, const T &z);
	tVec4<T>(const T &x, const T &y, const T &z, const T &w);
	tVec4<T>(const tVec2<T> &xy, const T &z);
	tVec4<T>(const T &x, const tVec2<T> &yz);
	tVec4<T>(const tVec2<T> &xy, const T &z, const T &w);
	tVec4<T>(const T &x, const T &y, const tVec2<T> &zw);
	tVec4<T>(const tVec2<T> &xy, const tVec2<T> &zw);
	tVec4<T>(const tVec3<T> &xyz, const T &w);
	tVec4<T>(const T &x, const tVec3<T> &yzw);

	tVec4<T> &Add(const tVec2<T> &other);
	tVec4<T> &Subtract(const tVec2<T> &other);
	tVec4<T> &Multiply(const tVec2<T> &other);
	tVec4<T> &Divide(const tVec2<T> &other);

	tVec4<T> &Add(const tVec3<T> &other);
	tVec4<T> &Subtract(const tVec3<T> &other);
	tVec4<T> &Multiply(const tVec3<T> &other);
	tVec4<T> &Divide(const tVec3<T> &other);

	tVec4<T> &Add(const tVec4<T> &other);
	tVec4<T> &Subtract(const tVec4<T> &other);
	tVec4<T> &Multiply(const tVec4<T> &other);
	tVec4<T> &Divide(const tVec4<T> &other);

	tVec4<T> &Add(const T &value);
	tVec4<T> &Subtract(const T &value);
	tVec4<T> &Multiply(const T &value);
	tVec4<T> &Divide(const T &value);

	tVec4<T> &Add(const T &x, const T &y);
	tVec4<T> &Subtract(const T &x, const T &y);
	tVec4<T> &Multiply(const T &x, const T &y);
	tVec4<T> &Divide(const T &x, const T &y);

	tVec4<T> &Add(const T &x, const T &y, const T &z);
	tVec4<T> &Subtract(const T &x, const T &y, const T &z);
	tVec4<T> &Multiply(const T &x, const T &y, const T &z);
	tVec4<T> &Divide(const T &x, const T &y, const T &z);

	tVec4<T> &Add(const T &x, const T &y, const T &z, const T &w);
	tVec4<T> &Subtract(const T &x, const T &y, const T &z, const T &w);
	tVec4<T> &Multiply(const T &x, const T &y, const T &z, const T &w);
	tVec4<T> &Divide(const T &x, const T &y, const T &z, const T &w);

	tVec4<T> Multiply(const Mat3 &transform) const;
	tVec4<T> Multiply(const Mat4 &transform) const;

	friend tVec4<T> operator+(tVec4<T> left, const tVec2<T> &right) { return left.Add(right); }
	friend tVec4<T> operator-(tVec4<T> left, const tVec2<T> &right) { return left.Subtract(right); }
	friend tVec4<T> operator*(tVec4<T> left, const tVec2<T> &right) { return left.Multiply(right); }
	friend tVec4<T> operator/(tVec4<T> left, const tVec2<T> &right) { return left.Divide(right); }

	friend tVec4<T> operator+(tVec4<T> left, const tVec3<T> &right) { return left.Add(right); }
	friend tVec4<T> operator-(tVec4<T> left, const tVec3<T> &right) { return left.Subtract(right); }
	friend tVec4<T> operator*(tVec4<T> left, const tVec3<T> &right) { return left.Multiply(right); }
	friend tVec4<T> operator/(tVec4<T> left, const tVec3<T> &right) { return left.Divide(right); }

	friend tVec4<T> operator+(tVec4<T> left, const tVec4<T> &right) { return left.Add(right); };
	friend tVec4<T> operator-(tVec4<T> left, const tVec4<T> &right) { return left.Subtract(right); };
	friend tVec4<T> operator*(tVec4<T> left, const tVec4<T> &right) { return left.Multiply(right); };
	friend tVec4<T> operator/(tVec4<T> left, const tVec4<T> &right) { return left.Divide(right); };

	friend tVec4<T> operator+(tVec4<T> left, T value) { return tVec4<T>(left.x + value, left.y + value, left.z + value, left.w + value); }
	friend tVec4<T> operator-(tVec4<T> left, T value) { return tVec4<T>(left.x - value, left.y - value, left.z - value, left.w - value); }
	friend tVec4<T> operator*(tVec4<T> left, T value) { return tVec4<T>(left.x * value, left.y * value, left.z * value, left.w * value); }
	friend tVec4<T> operator/(tVec4<T> left, T value) { return tVec4<T>(left.x / value, left.y / value, left.z / value, left.w / value); }

	bool operator==(const tVec2<T> &other) const;
	bool operator!=(const tVec2<T> &other) const;

	bool operator==(const tVec3<T> &other) const;
	bool operator!=(const tVec3<T> &other) const;

	bool operator==(const tVec4<T> &other) const;
	bool operator!=(const tVec4<T> &other) const;

	tVec4<T> &operator+=(const tVec2<T> &other);
	tVec4<T> &operator-=(const tVec2<T> &other);
	tVec4<T> &operator*=(const tVec2<T> &other);
	tVec4<T> &operator/=(const tVec2<T> &other);

	tVec4<T> &operator+=(const tVec3<T> &other);
	tVec4<T> &operator-=(const tVec3<T> &other);
	tVec4<T> &operator*=(const tVec3<T> &other);
	tVec4<T> &operator/=(const tVec3<T> &other);

	tVec4<T> &operator+=(const tVec4<T> &other);
	tVec4<T> &operator-=(const tVec4<T> &other);
	tVec4<T> &operator*=(const tVec4<T> &other);
	tVec4<T> &operator/=(const tVec4<T> &other);

	tVec4<T> &operator+=(T value);
	tVec4<T> &operator-=(T value);
	tVec4<T> &operator*=(T value);
	tVec4<T> &operator/=(T value);

	bool operator<(const tVec2<T> &other) const;
	bool operator<=(const tVec2<T> &other) const;
	bool operator>(const tVec2<T> &other) const;
	bool operator>=(const tVec2<T> &other) const;

	bool operator<(const tVec3<T> &other) const;
	bool operator<=(const tVec3<T> &other) const;
	bool operator>(const tVec3<T> &other) const;
	bool operator>=(const tVec3<T> &other) const;

	bool operator<(const tVec4<T> &other) const;
	bool operator<=(const tVec4<T> &other) const;
	bool operator>(const tVec4<T> &other) const;
	bool operator>=(const tVec4<T> &other) const;

	float Dot(const tVec2<T> &other) const;
	float Dot(const tVec3<T> &other) const;
	float Dot(const tVec4<T> &other) const;

	float Magnitude() const;
	tVec4<T> Normalize() const;
	float Distance(const tVec2<T> &other) const;
	float Distance(const tVec3<T> &other) const;
	float Distance(const tVec4<T> &other) const;

	std::string ToString() const;

	friend std::ostream &operator<<(std::ostream &stream, const tVec4<T> &vector) { return stream << vector.ToString(); }
};

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

////////////
//Matrix 3//
////////////
struct Mat3
{
	union
	{
		std::array<float, 3 * 3> elements{};
		std::array<tVec3<float>, 3> rows;
	};

	Mat3() = delete;
	explicit Mat3(float diagonal);
	explicit Mat3(float *elements);
	Mat3(const tVec3<float> &row0, const tVec3<float> &row1, const tVec3<float> &row2);

	static Mat3 Identity();

	Mat3 &Multiply(const Mat3 &other);
	friend Mat3 operator*(Mat3 left, const Mat3 &right);
	Mat3 &operator*=(const Mat3 &other);

	tVec3<float> Multiply(const tVec3<float> &other) const;
	friend tVec3<float> operator*(const Mat3 &left, const tVec3<float> &right);

	tVec4<float> Multiply(const tVec4<float> &other) const;
	friend tVec4<float> operator*(const Mat3 &left, const tVec4<float> &right);

	tVec3<float> GetColumn(int32_t index) const;
	void SetColumn(uint32_t index, const tVec3<float> &column);
	tVec3<float> GetPosition() const;
	void SetPosition(const tVec3<float> &position);

	static Mat3 Transpose(const Mat3 &matrix);

	std::string ToString() const;
};

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

////////////
//Matrix 4//
////////////
struct Mat4
{
	union {
		std::array<float, 4 * 4> elements{};
		std::array<tVec4<float>, 4> rows;
	};

	Mat4() = delete;
	explicit Mat4(float diagonal);
	explicit Mat4(float *elements);
	Mat4(const tVec4<float> &row0, const tVec4<float> &row1, const tVec4<float> &row2, const tVec4<float> &row3);

	static Mat4 Identity();

	Mat4 &Multiply(const Mat4 &other);
	friend Mat4 operator*(Mat4 left, const Mat4 &right);
	Mat4 &operator*=(const Mat4 &other);

	tVec3<float> Multiply(const tVec3<float> &other) const;
	friend tVec3<float> operator*(const Mat4 &left, const tVec3<float> &right);

	tVec4<float> Multiply(const tVec4<float> &other) const;
	friend tVec4<float> operator*(const Mat4 &left, const tVec4<float> &right);

	Mat4 &Invert();

	tVec4<float> GetColumn(int32_t index) const;
	void SetColumn(uint32_t index, const tVec4<float> &column);
	tVec3<float> GetPosition() const;
	void SetPosition(const tVec3<float> &position);

	static Mat4 Orthographic(float left, float right, float bottom, float top, float near, float far);
	static Mat4 Perspective(float fov, float aspectRatio, float near, float far);
	static Mat4 LookAt(const tVec3<float> &camera, const tVec3<float> &object, const tVec3<float> &up);

	static Mat4 Translate(const tVec3<float> &translation);
	static Mat4 Rotate(float angle, const tVec3<float> &axis);
	static Mat4 Scale(const tVec3<float> &scale);
	static Mat4 Invert(const Mat4 &matrix);

	static Mat4 Transpose(const Mat4 &matrix);

	std::string ToString() const;
};
} // namespace TRAP::Math

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

////////////
//Vector 2//
////////////
template <class T>
TRAP::Math::tVec2<T>::tVec2()
	: x(0), y(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T>::tVec2(const T &scalar)
	: x(scalar), y(scalar)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T>::tVec2(const T &x, const T &y)
	: x(x), y(y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T>::tVec2(const tVec3<T> &vector)
	: x(vector.x), y(vector.y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T>::tVec2(const tVec4<T> &vector)
	: x(vector.x), y(vector.y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Add(const tVec2<T> &other)
{
	x += other.x;
	y += other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Subtract(const tVec2<T> &other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Multiply(const tVec2<T> &other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Divide(const tVec2<T> &other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Add(const tVec3<T> &other)
{
	x += other.x;
	y += other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Subtract(const tVec3<T> &other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Multiply(const tVec3<T> &other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Divide(const tVec3<T> &other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Add(const tVec4<T> &other)
{
	x += other.x;
	y += other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Subtract(const tVec4<T> &other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Multiply(const tVec4<T> &other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Divide(const tVec4<T> &other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Add(const T &value)
{
	x += value;
	y += value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Subtract(const T &value)
{
	x -= value;
	y -= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Multiply(const T &value)
{
	x *= value;
	y *= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Divide(const T &value)
{
	x /= value;
	y /= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Add(const T &x, const T &y)
{
	this.x += x;
	this.y += y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Subtract(const T &x, const T &y)
{
	this.x -= x;
	this.y -= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Multiply(const T &x, const T &y)
{
	this.x *= x;
	this.y *= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::Divide(const T &x, const T &y)
{
	this.x /= x;
	this.y /= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator==(const tVec2<T> &other) const
{
	return x == other.x && y == other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator!=(const tVec2<T> &other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator==(const tVec3<T> &other) const
{
	return x == other.x && y == other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator!=(const tVec3<T> &other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator==(const tVec4<T> &other) const
{
	return x == other.x && y == other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator!=(const tVec4<T> &other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator+=(const tVec2<T> &other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator-=(const tVec2<T> &other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator*=(const tVec2<T> &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator/=(const tVec2<T> &other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator+=(const tVec3<T> &other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator-=(const tVec3<T> &other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator*=(const tVec3<T> &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator/=(const tVec3<T> &other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator+=(const tVec4<T> &other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator-=(const tVec4<T> &other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator*=(const tVec4<T> &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator/=(const tVec4<T> &other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator+=(T value)
{
	return Add(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator-=(T value)
{
	return Subtract(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator*=(T value)
{
	return Multiply(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> &TRAP::Math::tVec2<T>::operator/=(T value)
{
	return Divide(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator<(const tVec2<T> &other) const
{
	return x < other.x && y < other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator<=(const tVec2<T> &other) const
{
	return x <= other.x && y <= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator>(const tVec2<T> &other) const
{
	return x > other.x && y > other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator>=(const tVec2<T> &other) const
{
	return x >= other.x && y >= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator<(const tVec3<T> &other) const
{
	return x < other.x && y < other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator<=(const tVec3<T> &other) const
{
	return x <= other.x && y <= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator>(const tVec3<T> &other) const
{
	return x > other.x && y > other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator>=(const tVec3<T> &other) const
{
	return x >= other.x && y >= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator<(const tVec4<T> &other) const
{
	return x < other.x && y < other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator<=(const tVec4<T> &other) const
{
	return x <= other.x && y <= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator>(const tVec4<T> &other) const
{
	return x > other.x && y > other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec2<T>::operator>=(const tVec4<T> &other) const
{
	return x >= other.x && y >= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec2<T>::Dot(const tVec2<T> &other) const
{
	return x * other.x + y * other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec2<T>::Dot(const tVec3<T> &other) const
{
	return x * other.x + y * other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec2<T>::Dot(const tVec4<T> &other) const
{
	return x * other.x + y * other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec2<T>::Magnitude() const
{
	return Sqrt(x * x + y * y);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec2<T> TRAP::Math::tVec2<T>::Normalize() const
{
	float length = Magnitude();

	return tVec2<T>(x / length, y / length);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec2<T>::Distance(const tVec2<T> &other) const
{
	const float a = x - other.x;
	const float b = y - other.y;

	return Sqrt(a * a + b * b);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec2<T>::Distance(const tVec3<T> &other) const
{
	const float a = x - other.x;
	const float b = y - other.y;

	return Sqrt(a * a + b * b);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec2<T>::Distance(const tVec4<T> &other) const
{
	const float a = x - other.x;
	const float b = y - other.y;

	return Sqrt(a * a + b * b);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
std::string TRAP::Math::tVec2<T>::ToString() const
{
	std::stringstream result;
	result << x << ", " << y;

	return result.str();
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

////////////
//Vector 3//
////////////
template <class T>
TRAP::Math::tVec3<T>::tVec3()
	: x(0), y(0), z(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T>::tVec3(const T &scalar)
	: x(scalar), y(scalar), z(scalar)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T>::tVec3(const T &x, const T &y)
	: x(x), y(y), z(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T>::tVec3(const T &x, const T &y, const T &z)
	: x(x), y(y), z(z)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T>::tVec3(const tVec2<T> &xy, const T &z)
	: x(xy.x), y(xy.y), z(z)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T>::tVec3(const T &x, const tVec2<T> &yz)
	: x(x), y(yz.x), z(yz.y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T>::tVec3(const tVec4<T> &vector)
	: x(vector.x), y(vector.y), z(vector.z)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::Up()
{
	return tVec3<T>(0.0f, 1.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::Down()
{
	return tVec3<T>(0.0f, -1.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::Left()
{
	return tVec3<T>(-1.0f, 0.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::Right()
{
	return tVec3<T>(1.0f, 1.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::Zero()
{
	return tVec3<T>(0.0f, 0.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::XAxis()
{
	return tVec3<T>(1.0f, 0.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::YAxis()
{
	return tVec3<T>(0.0f, 1.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::ZAxis()
{
	return tVec3<T>(0.0f, 0.0f, 1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Add(const tVec2<T> &other)
{
	x += other.x;
	y += other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Subtract(const tVec2<T> &other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Multiply(const tVec2<T> &other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Divide(const tVec2<T> &other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Add(const tVec3<T> &other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Subtract(const tVec3<T> &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Multiply(const tVec3<T> &other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Divide(const tVec3<T> &other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Add(const tVec4<T> &other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Subtract(const tVec4<T> &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Multiply(const tVec4<T> &other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Divide(const tVec4<T> &other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Add(const T &value)
{
	x += value;
	y += value;
	z += value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Subtract(const T &value)
{
	x -= value;
	y -= value;
	z -= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Multiply(const T &value)
{
	x *= value;
	y *= value;
	z *= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Divide(const T &value)
{
	x /= value;
	y /= value;
	z /= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Add(const T &x, const T &y)
{
	this.x += x;
	this.y += y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Subtract(const T &x, const T &y)
{
	this.x -= x;
	this.y -= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Multiply(const T &x, const T &y)
{
	this.x *= x;
	this.y *= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Divide(const T &x, const T &y)
{
	this.x /= x;
	this.y /= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Add(const T &x, const T &y, const T &z)
{
	this.x += x;
	this.y += y;
	this.z += z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Subtract(const T &x, const T &y, const T &z)
{
	this.x -= x;
	this.y -= y;
	this.z -= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Multiply(const T &x, const T &y, const T &z)
{
	this.x *= x;
	this.y *= y;
	this.z *= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::Divide(const T &x, const T &y, const T &z)
{
	this.x /= x;
	this.y /= y;
	this.z /= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::Multiply(const Mat3 &transform) const
{
	return tVec3<T>(
		transform.rows[0].x * x + transform.rows[0].y * y + transform.rows[0].z * z,
		transform.rows[1].x * x + transform.rows[1].y * y + transform.rows[1].z * z,
		transform.rows[2].x * x + transform.rows[2].y * y + transform.rows[2].z * z);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::Multiply(const Mat4 &transform) const
{
	return tVec3<T>(
		transform.rows[0].x * x + transform.rows[0].y * y + transform.rows[0].z * z + transform.rows[0].w,
		transform.rows[1].x * x + transform.rows[1].y * y + transform.rows[1].z * z + transform.rows[1].w,
		transform.rows[2].x * x + transform.rows[2].y * y + transform.rows[2].z * z + transform.rows[2].w);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator==(const tVec2<T> &other) const
{
	return x == other.x && y == other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator!=(const tVec2<T> &other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator==(const tVec3<T> &other) const
{
	return x == other.x && y == other.y && z == other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator!=(const tVec3<T> &other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator==(const tVec4<T> &other) const
{
	return x == other.x && y == other.y && z == other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator!=(const tVec4<T> &other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator+=(const tVec2<T> &other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator-=(const tVec2<T> &other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator*=(const tVec2<T> &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator/=(const tVec2<T> &other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator+=(const tVec3<T> &other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator-=(const tVec3<T> &other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator*=(const tVec3<T> &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator/=(const tVec3<T> &other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator+=(const tVec4<T> &other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator-=(const tVec4<T> &other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator*=(const tVec4<T> &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator/=(const tVec4<T> &other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator+=(T value)
{
	return Add(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator-=(T value)
{
	return Subtract(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator*=(T value)
{
	return Multiply(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> &TRAP::Math::tVec3<T>::operator/=(T value)
{
	return Divide(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator<(const tVec2<T> &other) const
{
	return x < other.x && y < other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator<=(const tVec2<T> &other) const
{
	return x <= other.x && y <= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator>(const tVec2<T> &other) const
{
	return x > other.x && y > other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator>=(const tVec2<T> &other) const
{
	return x >= other.x && y >= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator<(const tVec3<T> &other) const
{
	return x < other.x && y < other.y && z < other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator<=(const tVec3<T> &other) const
{
	return x <= other.x && y <= other.y && z <= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator>(const tVec3<T> &other) const
{
	return x > other.x && y > other.y && z > other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator>=(const tVec3<T> &other) const
{
	return x >= other.x && y >= other.y && y >= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator<(const tVec4<T> &other) const
{
	return x < other.x && y < other.y && z < other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator<=(const tVec4<T> &other) const
{
	return x <= other.x && y <= other.y && z <= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator>(const tVec4<T> &other) const
{
	return x > other.x && y > other.y && z > other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec3<T>::operator>=(const tVec4<T> &other) const
{
	return x >= other.x && y >= other.y && z >= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec3<T>::Dot(const tVec2<T> &other) const
{
	return x * other.x + y * other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec3<T>::Dot(const tVec3<T> &other) const
{
	return x * other.x + y * other.y + z * other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec3<T>::Dot(const tVec4<T> &other) const
{
	return x * other.x + y * other.y + z * other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::Cross(const tVec3<T> &other) const
{
	return tVec3<T>(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec3<T>::Magnitude() const
{
	return Sqrt(x * x + y * y + z * z);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec3<T> TRAP::Math::tVec3<T>::Normalize() const
{
	float length = Magnitude();

	return tVec3<T>(x / length, y / length, z / length);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec3<T>::Distance(const tVec2<T> &other) const
{
	const float a = x - other.x;
	const float b = y - other.y;

	return Sqrt(a * a + b * b);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec3<T>::Distance(const tVec3<T> &other) const
{
	const float a = x - other.x;
	const float b = y - other.y;
	const float c = z - other.z;

	return Sqrt(a * a + b * b + c * c);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec3<T>::Distance(const tVec4<T> &other) const
{
	const float a = x - other.x;
	const float b = y - other.y;
	const float c = z - other.z;

	return Sqrt(a * a + b * b + c * c);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
std::string TRAP::Math::tVec3<T>::ToString() const
{
	std::stringstream result;
	result << x << ", " << y << ", " << z;

	return result.str();
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

////////////
//Vector 4//
////////////
template <class T>
TRAP::Math::tVec4<T>::tVec4()
	: x(0), y(0), z(0), w(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const T &scalar)
	: x(scalar), y(scalar), z(scalar), w(scalar)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const T &x, const T &y)
	: x(x), y(y), z(0), w(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const T &x, const T &y, const T &z)
	: x(x), y(y), z(z), w(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const T &x, const T &y, const T &z, const T &w)
	: x(x), y(y), z(z), w(w)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const tVec2<T> &xy, const T &z)
	: x(xy.x), y(xy, y), z(z), w(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const T &x, const tVec2<T> &yz)
	: x(x), y(yz.x), z(yz.y), w(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const tVec2<T> &xy, const T &z, const T &w)
	: x(xy.x), y(xy.y), z(z), w(w)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const T &x, const T &y, const tVec2<T> &zw)
	: x(x), y(y), z(zw.x), w(zw.y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const tVec2<T> &xy, const tVec2<T> &zw)
	: x(xy.x), y(xy.y), z(zw.x), w(zw.y)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const tVec3<T> &xyz, const T &w)
	: x(xyz.x), y(xyz.y), z(xyz.z), w(w)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T>::tVec4(const T &x, const tVec3<T> &yzw)
	: x(x), y(yzw.x), z(yzw.y), w(yzw.z)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Add(const tVec2<T> &other)
{
	x += other.x;
	y += other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Subtract(const tVec2<T> &other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Multiply(const tVec2<T> &other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Divide(const tVec2<T> &other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Add(const tVec3<T> &other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Subtract(const tVec3<T> &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Multiply(const tVec3<T> &other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Divide(const tVec3<T> &other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Add(const tVec4<T> &other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Subtract(const tVec4<T> &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Multiply(const tVec4<T> &other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	w *= other.w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Divide(const tVec4<T> &other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	w /= other.w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Add(const T &value)
{
	x += value;
	y += value;
	z += value;
	w += value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Subtract(const T &value)
{
	x -= value;
	y -= value;
	z -= value;
	w -= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Multiply(const T &value)
{
	x *= value;
	y *= value;
	z *= value;
	w *= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Divide(const T &value)
{
	x /= value;
	y /= value;
	z /= value;
	w /= value;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Add(const T &x, const T &y)
{
	this.x += x;
	this.y += y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Subtract(const T &x, const T &y)
{
	this.x -= x;
	this.y -= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Multiply(const T &x, const T &y)
{
	this.x *= x;
	this.y *= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Divide(const T &x, const T &y)
{
	this.x /= x;
	this.y /= y;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Add(const T &x, const T &y, const T &z)
{
	this.x += x;
	this.y += y;
	this.z += z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Subtract(const T &x, const T &y, const T &z)
{
	this.x -= x;
	this.y -= y;
	this.z -= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Multiply(const T &x, const T &y, const T &z)
{
	this.x *= x;
	this.y *= y;
	this.z *= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Divide(const T &x, const T &y, const T &z)
{
	this.x /= x;
	this.y /= y;
	this.z /= z;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Add(const T &x, const T &y, const T &z, const T &w)
{
	this.x += x;
	this.y += y;
	this.z += z;
	this.w += w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Subtract(const T &x, const T &y, const T &z, const T &w)
{
	this.x -= x;
	this.y -= y;
	this.z -= z;
	this.w -= w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Multiply(const T &x, const T &y, const T &z, const T &w)
{
	this.x *= x;
	this.y *= y;
	this.z *= z;
	this.w *= w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::Divide(const T &x, const T &y, const T &z, const T &w)
{
	this.x /= x;
	this.y /= y;
	this.z /= z;
	this.w /= w;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> TRAP::Math::tVec4<T>::Multiply(const Mat3 &transform) const
{
	return tVec4<T>(
		transform.rows[0].x * x + transform.rows[0].y * y + transform.rows[0].z * z + w,
		transform.rows[1].x * x + transform.rows[1].y * y + transform.rows[1].z * z + w,
		transform.rows[2].x * x + transform.rows[2].y * y + transform.rows[2].z * z + w,
		transform.rows[3].x * x + transform.rows[3].y * y + transform.rows[3].z * z + w);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> TRAP::Math::tVec4<T>::Multiply(const Mat4 &transform) const
{
	return tVec4<T>(
		transform.rows[0].x * x + transform.rows[0].y * y + transform.rows[0].z * z + transform.rows[0].w * w,
		transform.rows[1].x * x + transform.rows[1].y * y + transform.rows[1].z * z + transform.rows[1].w * w,
		transform.rows[2].x * x + transform.rows[2].y * y + transform.rows[2].z * z + transform.rows[2].w * w,
		transform.rows[3].x * x + transform.rows[3].y * y + transform.rows[3].z * z + transform.rows[3].w * w);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator==(const tVec2<T> &other) const
{
	return x == other.x && y == other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator!=(const tVec2<T> &other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator==(const tVec3<T> &other) const
{
	return x == other.x && y == other.y && z == other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator!=(const tVec3<T> &other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator==(const tVec4<T> &other) const
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator!=(const tVec4<T> &other) const
{
	return !(*this == other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator+=(const tVec2<T> &other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator-=(const tVec2<T> &other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator*=(const tVec2<T> &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator/=(const tVec2<T> &other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator+=(const tVec3<T> &other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator-=(const tVec3<T> &other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator*=(const tVec3<T> &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator/=(const tVec3<T> &other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator+=(const tVec4<T> &other)
{
	return Add(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator-=(const tVec4<T> &other)
{
	return Subtract(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator*=(const tVec4<T> &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator/=(const tVec4<T> &other)
{
	return Divide(other);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator+=(T value)
{
	return Add(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator-=(T value)
{
	return Subtract(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator*=(T value)
{
	return Multiply(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> &TRAP::Math::tVec4<T>::operator/=(T value)
{
	return Divide(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator<(const tVec2<T> &other) const
{
	return x < other.x && y < other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator<=(const tVec2<T> &other) const
{
	return x <= other.x && y <= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator>(const tVec2<T> &other) const
{
	return x > other.x && y > other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator>=(const tVec2<T> &other) const
{
	return x >= other.x && y >= other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator<(const tVec3<T> &other) const
{
	return x < other.x && y < other.y && z < other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator<=(const tVec3<T> &other) const
{
	return x <= other.x && y <= other.y && z <= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator>(const tVec3<T> &other) const
{
	return x > other.x && y > other.y && z > other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator>=(const tVec3<T> &other) const
{
	return x >= other.x && y >= other.y && y >= other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator<(const tVec4<T> &other) const
{
	return x < other.x && y < other.y && z < other.z && w < other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator<=(const tVec4<T> &other) const
{
	return x <= other.x && y <= other.y && z <= other.z && w <= other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator>(const tVec4<T> &other) const
{
	return x > other.x && y > other.y && z > other.z && w > other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
bool TRAP::Math::tVec4<T>::operator>=(const tVec4<T> &other) const
{
	return x >= other.x && y >= other.y && z >= other.z && w >= other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec4<T>::Dot(const tVec2<T> &other) const
{
	return x * other.x + y * other.y;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec4<T>::Dot(const tVec3<T> &other) const
{
	return x * other.x + y * other.y + z * other.z;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec4<T>::Dot(const tVec4<T> &other) const
{
	return x * other.x + y * other.y + z * other.z + w * other.w;
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec4<T>::Magnitude() const
{
	return Sqrt(x * x + y * y + z * z + w * w);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
TRAP::Math::tVec4<T> TRAP::Math::tVec4<T>::Normalize() const
{
	float length = Magnitude();

	return tVec4<T>(x / length, y / length, z / length, w / length);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec4<T>::Distance(const tVec2<T> &other) const
{
	const float a = x - other.x;
	const float b = y - other.y;

	return Sqrt(a * a + b * b);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec4<T>::Distance(const tVec3<T> &other) const
{
	const float a = x - other.x;
	const float b = y - other.y;
	const float c = z - other.z;

	return Sqrt(a * a + b * b + c * c);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
float TRAP::Math::tVec4<T>::Distance(const tVec4<T> &other) const
{
	const float a = x - other.x;
	const float b = y - other.y;
	const float c = z - other.z;
	const float d = w - other.w;

	return Sqrt(a * a + b * b + c * c + d * d);
}

//-------------------------------------------------------------------------------------------------------------------//

template <class T>
std::string TRAP::Math::tVec4<T>::ToString() const
{
	std::stringstream result;
	result << x << ", " << y << ", " << z << ", " << w;

	return result.str();
}

#endif /*_TRAP_MATH_H_*/
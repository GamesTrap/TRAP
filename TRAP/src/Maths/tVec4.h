#ifndef _TRAP_TVEC4_H_
#define _TRAP_TVEC4_H_

namespace TRAP::Maths
{
	struct Mat3;
	struct Mat4;

	template<class T>
	struct tVec2;
	template<class T>
	struct tVec3;

	template<class T>
	struct tVec4
	{
		T x, y, z, w;

		tVec4<T>();
		explicit tVec4<T>(const T& scalar);
		tVec4<T>(const T& x, const T& y);
		tVec4<T>(const T& x, const T& y, const T& z);
		tVec4<T>(const T& x, const T& y, const T& z, const T& w);
		tVec4<T>(const tVec2<T>& xy, const T& z);
		tVec4<T>(const T& x, const tVec2<T>& yz);
		tVec4<T>(const tVec2<T>& xy, const T& z, const T& w);
		tVec4<T>(const T& x, const T& y, const tVec2<T>& zw);
		tVec4<T>(const tVec2<T>& xy, const tVec2<T>& zw);
		tVec4<T>(const tVec3<T>& xyz, const T& w);
		tVec4<T>(const T& x, const tVec3<T>& yzw);

		tVec4<T>& Add(const tVec2<T>& other);
		tVec4<T>& Subtract(const tVec2<T>& other);
		tVec4<T>& Multiply(const tVec2<T>& other);
		tVec4<T>& Divide(const tVec2<T>& other);

		tVec4<T>& Add(const tVec3<T>& other);
		tVec4<T>& Subtract(const tVec3<T>& other);
		tVec4<T>& Multiply(const tVec3<T>& other);
		tVec4<T>& Divide(const tVec3<T>& other);

		tVec4<T>& Add(const tVec4<T>& other);
		tVec4<T>& Subtract(const tVec4<T>& other);
		tVec4<T>& Multiply(const tVec4<T>& other);
		tVec4<T>& Divide(const tVec4<T>& other);

		tVec4<T>& Add(const T& value);
		tVec4<T>& Subtract(const T& value);
		tVec4<T>& Multiply(const T& value);
		tVec4<T>& Divide(const T& value);

		tVec4<T>& Add(const T& x, const T& y);
		tVec4<T>& Subtract(const T& x, const T& y);
		tVec4<T>& Multiply(const T& x, const T& y);
		tVec4<T>& Divide(const T& x, const T& y);

		tVec4<T>& Add(const T& x, const T& y, const T& z);
		tVec4<T>& Subtract(const T& x, const T& y, const T& z);
		tVec4<T>& Multiply(const T& x, const T& y, const T& z);
		tVec4<T>& Divide(const T& x, const T& y, const T& z);

		tVec4<T>& Add(const T& x, const T& y, const T& z, const T& w);
		tVec4<T>& Subtract(const T& x, const T& y, const T& z, const T& w);
		tVec4<T>& Multiply(const T& x, const T& y, const T& z, const T& w);
		tVec4<T>& Divide(const T& x, const T& y, const T& z, const T& w);

		tVec4<T> Multiply(const Mat3& transform) const;
		tVec4<T> Multiply(const Mat4& transform) const;

		friend tVec4<T> operator+(tVec4<T> left, const tVec2<T>& right) { return left.Add(right); }
		friend tVec4<T> operator-(tVec4<T> left, const tVec2<T>& right) { return left.Subtract(right); }
		friend tVec4<T> operator*(tVec4<T> left, const tVec2<T>& right) { return left.Multiply(right); }
		friend tVec4<T> operator/(tVec4<T> left, const tVec2<T>& right) { return left.Divide(right); }

		friend tVec4<T> operator+(tVec4<T> left, const tVec3<T>& right) { return left.Add(right); }
		friend tVec4<T> operator-(tVec4<T> left, const tVec3<T>& right) { return left.Subtract(right); }
		friend tVec4<T> operator*(tVec4<T> left, const tVec3<T>& right) { return left.Multiply(right); }
		friend tVec4<T> operator/(tVec4<T> left, const tVec3<T>& right) { return left.Divide(right); }

		friend tVec4<T> operator+(tVec4<T> left, const tVec4<T>& right) { return left.Add(right); }
		friend tVec4<T> operator-(tVec4<T> left, const tVec4<T>& right) { return left.Subtract(right); }
		friend tVec4<T> operator*(tVec4<T> left, const tVec4<T>& right) { return left.Multiply(right); }
		friend tVec4<T> operator/(tVec4<T> left, const tVec4<T>& right) { return left.Divide(right); }

		friend tVec4<T> operator+(tVec4<T> left, T value) { return tVec4<T>(left.x + value, left.y + value, left.z + value, left.w + value); }
		friend tVec4<T> operator-(tVec4<T> left, T value) { return tVec4<T>(left.x - value, left.y - value, left.z - value, left.w - value); }
		friend tVec4<T> operator*(tVec4<T> left, T value) { return tVec4<T>(left.x * value, left.y * value, left.z * value, left.w * value); }
		friend tVec4<T> operator/(tVec4<T> left, T value) { return tVec4<T>(left.x / value, left.y / value, left.z / value, left.w / value); }

		bool operator==(const tVec2<T>& other) const;
		bool operator!=(const tVec2<T>& other) const;

		bool operator==(const tVec3<T>& other) const;
		bool operator!=(const tVec3<T>& other) const;

		bool operator==(const tVec4<T>& other) const;
		bool operator!=(const tVec4<T>& other) const;

		tVec4<T>& operator+=(const tVec2<T>& other);
		tVec4<T>& operator-=(const tVec2<T>& other);
		tVec4<T>& operator*=(const tVec2<T>& other);
		tVec4<T>& operator/=(const tVec2<T>& other);

		tVec4<T>& operator+=(const tVec3<T>& other);
		tVec4<T>& operator-=(const tVec3<T>& other);
		tVec4<T>& operator*=(const tVec3<T>& other);
		tVec4<T>& operator/=(const tVec3<T>& other);

		tVec4<T>& operator+=(const tVec4<T>& other);
		tVec4<T>& operator-=(const tVec4<T>& other);
		tVec4<T>& operator*=(const tVec4<T>& other);
		tVec4<T>& operator/=(const tVec4<T>& other);

		tVec4<T>& operator+=(T value);
		tVec4<T>& operator-=(T value);
		tVec4<T>& operator*=(T value);
		tVec4<T>& operator/=(T value);

		bool operator<(const tVec2<T>& other) const;
		bool operator<=(const tVec2<T>& other) const;
		bool operator>(const tVec2<T>& other) const;
		bool operator>=(const tVec2<T>& other) const;

		bool operator<(const tVec3<T>& other) const;
		bool operator<=(const tVec3<T>& other) const;
		bool operator>(const tVec3<T>& other) const;
		bool operator>=(const tVec3<T>& other) const;

		bool operator<(const tVec4<T>& other) const;
		bool operator<=(const tVec4<T>& other) const;
		bool operator>(const tVec4<T>& other) const;
		bool operator>=(const tVec4<T>& other) const;

		float Dot(const tVec2<T>& other) const;
		float Dot(const tVec3<T>& other) const;
		float Dot(const tVec4<T>& other) const;

		float Magnitude() const;
		tVec4<T> Normalize() const;
		float Distance(const tVec2<T>& other) const;
		float Distance(const tVec3<T>& other) const;
		float Distance(const tVec4<T>& other) const;

		std::string ToString() const;

		friend std::ostream& operator<<(std::ostream& stream, const tVec4<T>& vector) { return stream << vector.ToString(); }
	};
}

#include "tVec4.inl"

#endif /*_TRAP_TVEC4_H_*/
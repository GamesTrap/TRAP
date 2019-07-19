#ifndef _TRAP_TVEC3_H_
#define _TRAP_TVEC3_H_

namespace TRAP::Maths
{
	struct Mat3;
	struct Mat4;

	template<class T>
	struct tVec2;
	template<class T>
	struct tVec4;

	template<class T>
	struct tVec3
	{
		T x, y, z;

		tVec3<T>();
		explicit tVec3<T>(const T& scalar);
		tVec3<T>(const T& x, const T& y);
		tVec3<T>(const T& x, const T& y, const T& z);
		tVec3<T>(const tVec2<T>& xy, const T& z);
		tVec3<T>(const T& x, const tVec2<T>& yz);
		explicit tVec3<T>(const tVec4<T>& vector);

		static tVec3<T> Up();
		static tVec3<T> Down();
		static tVec3<T> Left();
		static tVec3<T> Right();
		static tVec3<T> Zero();

		static tVec3<T> XAxis();
		static tVec3<T> YAxis();
		static tVec3<T> ZAxis();

		tVec3<T>& Add(const tVec2<T>& other);
		tVec3<T>& Subtract(const tVec2<T>& other);
		tVec3<T>& Multiply(const tVec2<T>& other);
		tVec3<T>& Divide(const tVec2<T>& other);

		tVec3<T>& Add(const tVec3<T>& other);
		tVec3<T>& Subtract(const tVec3<T>& other);
		tVec3<T>& Multiply(const tVec3<T>& other);
		tVec3<T>& Divide(const tVec3<T>& other);

		tVec3<T>& Add(const tVec4<T>& other);
		tVec3<T>& Subtract(const tVec4<T>& other);
		tVec3<T>& Multiply(const tVec4<T>& other);
		tVec3<T>& Divide(const tVec4<T>& other);

		tVec3<T>& Add(const T& value);
		tVec3<T>& Subtract(const T& value);
		tVec3<T>& Multiply(const T& value);
		tVec3<T>& Divide(const T& value);

		tVec3<T>& Add(const T& x, const T& y);
		tVec3<T>& Subtract(const T& x, const T& y);
		tVec3<T>& Multiply(const T& x, const T& y);
		tVec3<T>& Divide(const T& x, const T& y);

		tVec3<T>& Add(const T& x, const T& y, const T& z);
		tVec3<T>& Subtract(const T& x, const T& y, const T& z);
		tVec3<T>& Multiply(const T& x, const T& y, const T& z);
		tVec3<T>& Divide(const T& x, const T& y, const T& z);

		tVec3<T> Multiply(const Mat3& transform) const;
		tVec3<T> Multiply(const Mat4& transform) const;

		friend tVec3<T> operator+(tVec3<T> left, const tVec2<T>& right) { return left.Add(right); }
		friend tVec3<T> operator-(tVec3<T> left, const tVec2<T>& right) { return left.Subtract(right); }
		friend tVec3<T> operator*(tVec3<T> left, const tVec2<T>& right) { return left.Multiply(right); }
		friend tVec3<T> operator/(tVec3<T> left, const tVec2<T>& right) { return left.Divide(right); }

		friend tVec3<T> operator+(tVec3<T> left, const tVec3<T>& right) { return left.Add(right); }
		friend tVec3<T> operator-(tVec3<T> left, const tVec3<T>& right) { return left.Subtract(right); }
		friend tVec3<T> operator*(tVec3<T> left, const tVec3<T>& right) { return left.Multiply(right); }
		friend tVec3<T> operator/(tVec3<T> left, const tVec3<T>& right) { return left.Divide(right); }

		friend tVec3<T> operator+(tVec3<T> left, const tVec4<T>& right) { return left.Add(right); }
		friend tVec3<T> operator-(tVec3<T> left, const tVec4<T>& right) { return left.Subtract(right); }
		friend tVec3<T> operator*(tVec3<T> left, const tVec4<T>& right) { return left.Multiply(right); }
		friend tVec3<T> operator/(tVec3<T> left, const tVec4<T>& right) { return left.Divide(right); }

		friend tVec3<T> operator+(tVec3<T> left, T value) { return tVec3<T>(left.x + value, left.y + value, left.z + value); }
		friend tVec3<T> operator-(tVec3<T> left, T value) { return tVec3<T>(left.x - value, left.y - value, left.z - value); }
		friend tVec3<T> operator*(tVec3<T> left, T value) { return tVec3<T>(left.x * value, left.y * value, left.z * value); }
		friend tVec3<T> operator/(tVec3<T> left, T value) { return tVec3<T>(left.x / value, left.y / value, left.z / value); }

		bool operator==(const tVec2<T>& other) const;
		bool operator!=(const tVec2<T>& other) const;

		bool operator==(const tVec3<T>& other) const;
		bool operator!=(const tVec3<T>& other) const;

		bool operator==(const tVec4<T>& other) const;
		bool operator!=(const tVec4<T>& other) const;

		tVec3<T>& operator+=(const tVec2<T>& other);
		tVec3<T>& operator-=(const tVec2<T>& other);
		tVec3<T>& operator*=(const tVec2<T>& other);
		tVec3<T>& operator/=(const tVec2<T>& other);

		tVec3<T>& operator+=(const tVec3<T>& other);
		tVec3<T>& operator-=(const tVec3<T>& other);
		tVec3<T>& operator*=(const tVec3<T>& other);
		tVec3<T>& operator/=(const tVec3<T>& other);

		tVec3<T>& operator+=(const tVec4<T>& other);
		tVec3<T>& operator-=(const tVec4<T>& other);
		tVec3<T>& operator*=(const tVec4<T>& other);
		tVec3<T>& operator/=(const tVec4<T>& other);

		tVec3<T>& operator+=(T value);
		tVec3<T>& operator-=(T value);
		tVec3<T>& operator*=(T value);
		tVec3<T>& operator/=(T value);

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
		tVec3<T> Cross(const tVec3<T>& other) const;

		float Magnitude() const;
		tVec3<T> Normalize() const;
		float Distance(const tVec2<T>& other) const;
		float Distance(const tVec3<T>& other) const;
		float Distance(const tVec4<T>& other) const;

		std::string ToString() const;

		friend std::ostream& operator<<(std::ostream& stream, const tVec3<T>& vector) { return stream << vector.ToString(); }
	};
}

#include "tVec3.inl"

#endif /*_TRAP_TVEC3_H_*/
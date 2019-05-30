#ifndef _TRAP_TVEC2_H_
#define _TRAP_TVEC2_H_

namespace TRAP::Maths
{
	template<class T>
	struct tVec3;
	template<class T>
	struct tVec4;

	template<class T>
	struct tVec2
	{
		T x, y;

		tVec2<T>();
		explicit tVec2<T>(const T& scalar);
		tVec2<T>(const T& x, const T& y);
		explicit tVec2<T>(const tVec3<T>& vector);
		explicit tVec2<T>(const tVec4<T>& vector);

		tVec2<T>& Add(const tVec2<T>& other);
		tVec2<T>& Subtract(const tVec2<T>& other);
		tVec2<T>& Multiply(const tVec2<T>& other);
		tVec2<T>& Divide(const tVec2<T>& other);

		tVec2<T>& Add(const tVec3<T>& other);
		tVec2<T>& Subtract(const tVec3<T>& other);
		tVec2<T>& Multiply(const tVec3<T>& other);
		tVec2<T>& Divide(const tVec3<T>& other);

		tVec2<T>& Add(const tVec4<T>& other);
		tVec2<T>& Subtract(const tVec4<T>& other);
		tVec2<T>& Multiply(const tVec4<T>& other);
		tVec2<T>& Divide(const tVec4<T>& other);

		tVec2<T>& Add(const T& value);
		tVec2<T>& Subtract(const T& value);
		tVec2<T>& Multiply(const T& value);
		tVec2<T>& Divide(const T& value);

		tVec2<T>& Add(const T& x, const T& y);
		tVec2<T>& Subtract(const T& x, const T& y);
		tVec2<T>& Multiply(const T& x, const T& y);
		tVec2<T>& Divide(const T& x, const T& y);

		friend tVec2<T> operator+(tVec2<T> left, const tVec2<T>& right);
		friend tVec2<T> operator-(tVec2<T> left, const tVec2<T>& right);
		friend tVec2<T> operator*(tVec2<T> left, const tVec2<T>& right);
		friend tVec2<T> operator/(tVec2<T> left, const tVec2<T>& right);

		friend tVec2<T> operator+(tVec2<T> left, const tVec3<T>& right);
		friend tVec2<T> operator-(tVec2<T> left, const tVec3<T>& right);
		friend tVec2<T> operator*(tVec2<T> left, const tVec3<T>& right);
		friend tVec2<T> operator/(tVec2<T> left, const tVec3<T>& right);

		friend tVec2<T> operator+(tVec2<T> left, const tVec4<T>& right);
		friend tVec2<T> operator-(tVec2<T> left, const tVec4<T>& right);
		friend tVec2<T> operator*(tVec2<T> left, const tVec4<T>& right);
		friend tVec2<T> operator/(tVec2<T> left, const tVec4<T>& right);

		friend tVec2<T> operator+(tVec2<T> left, T value);
		friend tVec2<T> operator-(tVec2<T> left, T value);
		friend tVec2<T> operator*(tVec2<T> left, T value);
		friend tVec2<T> operator/(tVec2<T> left, T value);

		bool operator==(const tVec2<T>& other) const;
		bool operator!=(const tVec2<T>& other) const;

		bool operator==(const tVec3<T>& other) const;
		bool operator!=(const tVec3<T>& other) const;

		bool operator==(const tVec4<T>& other) const;
		bool operator!=(const tVec4<T>& other) const;

		tVec2<T>& operator+=(const tVec2<T>& other);
		tVec2<T>& operator-=(const tVec2<T>& other);
		tVec2<T>& operator*=(const tVec2<T>& other);
		tVec2<T>& operator/=(const tVec2<T>& other);

		tVec2<T>& operator+=(const tVec3<T>& other);
		tVec2<T>& operator-=(const tVec3<T>& other);
		tVec2<T>& operator*=(const tVec3<T>& other);
		tVec2<T>& operator/=(const tVec3<T>& other);

		tVec2<T>& operator+=(const tVec4<T>& other);
		tVec2<T>& operator-=(const tVec4<T>& other);
		tVec2<T>& operator*=(const tVec4<T>& other);
		tVec2<T>& operator/=(const tVec4<T>& other);

		tVec2<T>& operator+=(T value);
		tVec2<T>& operator-=(T value);
		tVec2<T>& operator*=(T value);
		tVec2<T>& operator/=(T value);

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
		tVec2<T> Normalize() const;
		float Distance(const tVec2<T>& other) const;
		float Distance(const tVec3<T>& other) const;
		float Distance(const tVec4<T>& other) const;

		std::string ToString() const;

		friend std::ostream& operator<<(std::ostream& stream, const tVec2<T>& vector);
	};
}

#include "tVec2.inl"

#endif /*_TRAP_TVEC2_H_*/
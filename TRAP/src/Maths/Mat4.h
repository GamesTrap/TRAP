#ifndef _TRAP_MAT4_H_
#define _TRAP_MAT4_H_

#include "tVec4.h"

namespace TRAP::Maths
{
	template<class T>
	struct tVec3;
	typedef tVec3<float> Vec3;

	typedef tVec4<float> Vec4;

	struct Mat4
	{
		union
		{
			std::array<float, 4*4> elements;
			std::array<Vec4, 4> rows;
		};

		Mat4();
		explicit Mat4(float diagonal);
		explicit Mat4(float* elements);
		Mat4(const Vec4& row0, const Vec4& row1, const Vec4& row2, const Vec4& row3);

		static Mat4 Identity();

		Mat4& Multiply(const Mat4& other);
		friend Mat4 operator*(Mat4 left, const Mat4& right);
		Mat4& operator*=(const Mat4& other);

		Vec3 Multiply(const Vec3& other) const;
		friend Vec3 operator*(const Mat4& left, const Vec3& right);

		Vec4 Multiply(const Vec4& other) const;
		friend Vec4 operator*(const Mat4& left, const Vec4& right);

		Mat4& Invert();

		Vec4 GetColumn(int index) const;
		void SetColumn(unsigned int index, const Vec4& column);
		Vec3 GetPosition() const;
		void SetPosition(const Vec3& position);

		static Mat4 Orthographic(float left, float right, float bottom, float top, float near, float far);
		static Mat4 Perspective(float fov, float aspectRatio, float near, float far);
		static Mat4 LookAt(const Vec3& camera, const Vec3& object, const Vec3& up);

		static Mat4 Translate(const Vec3& translation);
		static Mat4 Rotate(float angle, const Vec3& axis);
		static Mat4 Scale(const Vec3& scale);
		static Mat4 Invert(const Mat4& matrix);

		static Mat4 Transpose(const Mat4& matrix);

		std::string ToString() const;
	};
}

#endif /*_TRAP_MAT4_H_*/
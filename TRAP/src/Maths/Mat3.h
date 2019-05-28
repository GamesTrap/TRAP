#pragma once

#include "tVec3.h"

namespace TRAP::Maths
{
	typedef tVec3<float> Vec3;

	template<class T>
	struct tVec4;
	typedef tVec4<float> Vec4;

	struct Mat3
	{
		union
		{
			std::array<float, 3 * 3> elements;
			std::array<Vec3, 3> rows;
		};

		Mat3();
		explicit Mat3(float diagonal);
		explicit Mat3(float* elements);
		Mat3(const Vec3& row0, const Vec3& row1, const Vec3& row2);

		static Mat3 Identity();

		Mat3& Multiply(const Mat3& other);
		friend Mat3 operator*(Mat3 left, const Mat3& right);
		Mat3& operator*=(const Mat3& other);

		Vec3 Multiply(const Vec3& other) const;
		friend Vec3 operator*(const Mat3& left, const Vec3& right);

		Vec4 Multiply(const Vec4& other) const;
		friend Vec4 operator*(const Mat3& left, const Vec4& right);

		Vec3 GetColumn(int index) const;
		void SetColumn(unsigned int index, const Vec3& column);
		Vec3 GetPosition() const;
		void SetPosition(const Vec3& position);

		static Mat3 Transpose(const Mat3& matrix);

		std::string ToString() const;
	};
}

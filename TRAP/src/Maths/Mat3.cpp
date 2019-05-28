#include "TRAPPCH.h"
#include "Mat3.h"

#include "Maths.h"

namespace TRAP::Maths
{
	Mat3::Mat3()
		: elements{}
	{		
	}

	Mat3::Mat3(const float diagonal)
		: elements{}
	{
		elements[0 + 0 * 3] = diagonal;
		elements[1 + 1 * 3] = diagonal;
		elements[2 + 2 * 3] = diagonal;
	}

	Mat3::Mat3(float* elements)
		: elements{}
	{
		std::memcpy(this->elements.data(), elements, 3 * 3 * sizeof(float));
	}

	Mat3::Mat3(const Vec3 & row0, const Vec3 & row1, const Vec3 & row2)
		: elements{}
	{
		rows[0] = row0;
		rows[1] = row1;
		rows[2] = row2;
	}

	Mat3 Mat3::Identity()
	{
		return Mat3(1.0f);
	}

	Mat3& Mat3::Multiply(const Mat3 & other)
	{
		std::array<float, 9> data{};
		for (auto row = 0; row < 3; row++)
			for (auto col = 0; col < 3; col++)
			{
				float sum = 0.0f;
				for (auto e = 0; e < 3; e++)
					sum += elements[e + row * 3] * other.elements[col + e * 3];

				data[col + row * 3] = sum;
			}

		std::memcpy(elements.data(), data.data(), 3 * 3 * sizeof(float));

		return *this;
	}

	Vec3 Mat3::Multiply(const Vec3 & other) const
	{
		return other.Multiply(*this);
	}

	Vec4 Mat3::Multiply(const Vec4 & other) const
	{
		return other.Multiply(*this);
	}

	Mat3 operator*(Mat3 left, const Mat3 & right)
	{
		return left.Multiply(right);
	}

	Mat3& Mat3::operator*=(const Mat3 & other)
	{
		return Multiply(other);
	}

	Vec3 operator*(const Mat3 & left, const Vec3 & right)
	{
		return left.Multiply(right);
	}

	Vec4 operator*(const Mat3 & left, const Vec4 & right)
	{
		return  left.Multiply(right);
	}

	Vec3 Mat3::GetColumn(const int index) const
	{
		return Vec3(elements[index + 0 * 3], elements[index + 1 * 3], elements[index + 2 * 3]);
	}

	void Mat3::SetColumn(const unsigned int index, const Vec3 & column)
	{
		elements[index + 0 * 3] = column.x;
		elements[index + 1 * 3] = column.y;
		elements[index + 2 * 3] = column.z;
	}

	Vec3 Mat3::GetPosition() const
	{
		return Vec3(GetColumn(3));
	}

	void Mat3::SetPosition(const Vec3 & position)
	{
		SetColumn(3, position);
	}

	Mat3 Mat3::Transpose(const Mat3 & matrix)
	{
		return Mat3
		(
			Vec3(matrix.rows[0].x, matrix.rows[1].x, matrix.rows[2].x),
			Vec3(matrix.rows[0].y, matrix.rows[1].y, matrix.rows[2].y),
			Vec3(matrix.rows[0].z, matrix.rows[1].z, matrix.rows[2].z)
		);
	}

	std::string Mat3::ToString() const
	{
		std::stringstream result;
		result << "Mat3: (" << rows[0].x << ", " << rows[1].x << ", " << rows[2].x << "), "
			<< '(' << rows[0].y << ", " << rows[1].y << ", " << rows[2].y << "), "
			<< '(' << rows[0].z << ", " << rows[1].z << ", " << rows[2].z << ')';

		return result.str();
	}
}

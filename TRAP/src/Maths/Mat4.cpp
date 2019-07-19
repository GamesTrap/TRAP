#include "TRAPPCH.h"
#include "Mat4.h"

#include "Maths.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4::Mat4()
	: elements{}
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4::Mat4(const float diagonal)
	: elements{}
{
	elements[0 + 0 * 4] = diagonal;
	elements[1 + 1 * 4] = diagonal;
	elements[2 + 2 * 4] = diagonal;
	elements[3 + 3 * 4] = diagonal;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4::Mat4(float* elements)
	: elements{}
{
	std::memcpy(this->elements.data(), elements, 4 * 4 * sizeof(float));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4::Mat4(const Vec4& row0, const Vec4& row1, const Vec4& row2, const Vec4& row3)
	: elements{}
{
	rows[0] = row0;
	rows[1] = row1;
	rows[2] = row2;
	rows[3] = row3;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4 TRAP::Maths::Mat4::Identity()
{
	return Mat4(1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4& TRAP::Maths::Mat4::Multiply(const Mat4& other)
{
	std::array<float, 16> data{};
	for (unsigned int row = 0; row < 4; row++)
		for (unsigned int col = 0; col < 4; col++)
		{
			float sum = 0.0f;
			for (unsigned int e = 0; e < 4; e++)
				sum += elements[4 * row + e] * other.elements[col + e * 4];

			data[col + row * 4] = sum;
		}

	std::memcpy(elements.data(), data.data(), 4 * 4 * sizeof(float));

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Vec3 TRAP::Maths::Mat4::Multiply(const Vec3& other) const
{
	return other.Multiply(*this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Vec4 TRAP::Maths::Mat4::Multiply(const Vec4& other) const
{
	return other.Multiply(*this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4 TRAP::Maths::operator*(Mat4 left, const Mat4& right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4& TRAP::Maths::Mat4::operator*=(const Mat4& other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Vec3 TRAP::Maths::operator*(const Mat4& left, const Vec3& right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Vec4 TRAP::Maths::operator*(const Mat4& left, const Vec4& right)
{
	return  left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4& TRAP::Maths::Mat4::Invert()
{
	std::array<float, 16> temp{};

	temp[0] = elements[5] * elements[10] * elements[15] - elements[5] * elements[11] * elements[14] - elements[9] * elements[6] * elements[15] + elements[9] * elements[7] * elements[14] + elements[13] * elements[6] * elements[11] - elements[13] * elements[7] * elements[10];
	temp[4] = -elements[4] * elements[10] * elements[15] + elements[4] * elements[11] * elements[14] + elements[8] * elements[6] * elements[15] - elements[8] * elements[7] * elements[14] - elements[12] * elements[6] * elements[11] + elements[12] * elements[7] * elements[10];
	temp[8] = elements[4] * elements[9] * elements[15] - elements[4] * elements[11] * elements[13] - elements[8] * elements[5] * elements[15] + elements[8] * elements[7] * elements[13] + elements[12] * elements[5] * elements[11] - elements[12] * elements[7] * elements[9];
	temp[12] = -elements[4] * elements[9] * elements[14] + elements[4] * elements[10] * elements[13] + elements[8] * elements[5] * elements[14] - elements[8] * elements[6] * elements[13] - elements[12] * elements[5] * elements[10] + elements[12] * elements[6] * elements[9];
	temp[1] = -elements[1] * elements[10] * elements[15] + elements[1] * elements[11] * elements[14] + elements[9] * elements[2] * elements[15] - elements[9] * elements[3] * elements[14] - elements[13] * elements[2] * elements[11] + elements[13] * elements[3] * elements[10];
	temp[5] = elements[0] * elements[10] * elements[15] - elements[0] * elements[11] * elements[14] - elements[8] * elements[2] * elements[15] + elements[8] * elements[3] * elements[14] + elements[12] * elements[2] * elements[11] - elements[12] * elements[3] * elements[10];
	temp[9] = -elements[0] * elements[9] * elements[15] + elements[0] * elements[11] * elements[13] + elements[8] * elements[1] * elements[15] - elements[8] * elements[3] * elements[13] - elements[12] * elements[1] * elements[11] + elements[12] * elements[3] * elements[9];
	temp[13] = elements[0] * elements[9] * elements[14] - elements[0] * elements[10] * elements[13] - elements[8] * elements[1] * elements[14] + elements[8] * elements[2] * elements[13] + elements[12] * elements[1] * elements[10] - elements[12] * elements[2] * elements[9];
	temp[2] = elements[1] * elements[6] * elements[15] - elements[1] * elements[7] * elements[14] - elements[5] * elements[2] * elements[15] + elements[5] * elements[3] * elements[14] + elements[13] * elements[2] * elements[7] - elements[13] * elements[3] * elements[6];
	temp[6] = -elements[0] * elements[6] * elements[15] + elements[0] * elements[7] * elements[14] + elements[4] * elements[2] * elements[15] -	elements[4] * elements[3] * elements[14] - elements[12] * elements[2] * elements[7] + elements[12] * elements[3] * elements[6];
	temp[10] = elements[0] * elements[5] * elements[15] - elements[0] * elements[7] * elements[13] - elements[4] * elements[1] * elements[15] + elements[4] * elements[3] * elements[13] + elements[12] * elements[1] * elements[7] - elements[12] * elements[3] * elements[5];
	temp[14] = -elements[0] * elements[5] * elements[14] + elements[0] * elements[6] * elements[13] + elements[4] * elements[1] * elements[14] - elements[4] * elements[2] * elements[13] - elements[12] * elements[1] * elements[6] + elements[12] * elements[2] * elements[5];
	temp[3] = -elements[1] * elements[6] * elements[11] + elements[1] * elements[7] * elements[10] + elements[5] * elements[2] * elements[11] - elements[5] * elements[3] * elements[10] - elements[9] * elements[2] * elements[7] +elements[9] * elements[3] * elements[6];
	temp[7] = elements[0] * elements[6] * elements[11] - elements[0] * elements[7] * elements[10] - elements[4] * elements[2] * elements[11] + elements[4] * elements[3] * elements[10] + elements[8] * elements[2] * elements[7] - elements[8] * elements[3] * elements[6];
	temp[11] = -elements[0] * elements[5] * elements[11] + elements[0] * elements[7] * elements[9] + elements[4] * elements[1] * elements[11] - elements[4] * elements[3] * elements[9] - elements[8] * elements[1] * elements[7] +elements[8] * elements[3] * elements[5];
	temp[15] = elements[0] * elements[5] * elements[10] - elements[0] * elements[6] * elements[9] - elements[4] * elements[1] * elements[10] + elements[4] * elements[2] * elements[9] + elements[8] * elements[1] * elements[6] -elements[8] * elements[2] * elements[5];

	float determinant = elements[0] * temp[0] + elements[1] * temp[4] + elements[2] * temp[8] + elements[3] * temp[12];
	determinant = 1.0f / determinant;

	for (int32_t i = 0; i < 4 * 4; i++)
		elements[i] = temp[i] * determinant;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Vec4 TRAP::Maths::Mat4::GetColumn(const int index) const
{
	return Vec4(elements[index + 0 * 4], elements[index + 1 * 4], elements[index + 2 * 4], elements[index + 3 * 4]);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Maths::Mat4::SetColumn(const unsigned int index, const Vec4& column)
{
	elements[index + 0 * 4] = column.x;
	elements[index + 1 * 4] = column.y;
	elements[index + 2 * 4] = column.z;
	elements[index + 3 * 4] = column.w;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Vec3 TRAP::Maths::Mat4::GetPosition() const
{
	return Vec3(GetColumn(3));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Maths::Mat4::SetPosition(const Vec3& position)
{
	SetColumn(3, Vec4(position, 1.0f));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4 TRAP::Maths::Mat4::Orthographic(const float left, const float right, const float bottom, const float top, const float near, const float far)
{
	Mat4 result = Identity();

	result.elements[0 + 0 * 4] = 2.0f / (right - left);
	result.elements[1 + 1 * 4] = 2.0f / (top - bottom);
	result.elements[2 + 2 * 4] = 2.0f / (near - far);

	result.elements[3 + 0 * 4] = (left + right) / (left - right);
	result.elements[3 + 1 * 4] = (bottom + top) / (bottom - top);
	result.elements[3 + 2 * 4] = (far + near) / (far - near);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4 TRAP::Maths::Mat4::Perspective(const float fov, const float aspectRatio, const float near, const float far)
{
	Mat4 result = Identity();

	const float q = 1.0f / Tan(ToRadians(0.5f * fov));
	const float a = q / aspectRatio;

	const float b = (near + far) / (near - far);
	const float c = (2.0f * near * far) / (near - far);

	result.elements[0 + 0 * 4] = a;
	result.elements[1 + 1 * 4] = q;
	result.elements[2 + 2 * 4] = b;
	result.elements[2 + 3 * 4] = -1.0f;
	result.elements[3 + 2 * 4] = c;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4 TRAP::Maths::Mat4::LookAt(const Vec3& camera, const Vec3& object, const Vec3& up)
{
	Mat4 result = Identity();
	const Vec3 f = (object - camera).Normalize();
	const Vec3 s = f.Cross(up.Normalize());
	const Vec3 u = s.Cross(f);

	result.elements[0 + 0 * 4] = s.x;
	result.elements[0 + 1 * 4] = s.y;
	result.elements[0 + 2 * 4] = s.z;

	result.elements[1 + 0 * 4] = u.x;
	result.elements[1 + 1 * 4] = u.y;
	result.elements[1 + 2 * 4] = u.z;

	result.elements[2 + 0 * 4] = -f.x;
	result.elements[2 + 1 * 4] = -f.y;
	result.elements[2 + 2 * 4] = -f.z;

	return result * Translate(Vec3(-camera.x, -camera.y, -camera.z));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4 TRAP::Maths::Mat4::Translate(const Vec3& translation)
{
	Mat4 result = Identity();

	result.elements[3 + 0 * 4] = translation.x;
	result.elements[3 + 1 * 4] = translation.y;
	result.elements[3 + 2 * 4] = translation.z;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4 TRAP::Maths::Mat4::Rotate(const float angle, const Vec3& axis)
{
	Mat4 result = Identity();

	const float r = ToRadians(angle);
	const float c = Cos(r);
	const float s = Sin(r);
	const float omc = 1.0f - c;

	const float x = axis.x;
	const float y = axis.y;
	const float z = axis.z;

	result.elements[0 + 0 * 4] = x * x * omc + c;
	result.elements[0 + 1 * 4] = y * x * omc + z * s;
	result.elements[0 + 2 * 4] = x * z * omc - y * s;

	result.elements[1 + 0 * 4] = x * y * omc - z * s;
	result.elements[1 + 1 * 4] = y * y * omc + c;
	result.elements[1 + 2 * 4] = y * z * omc + x * s;

	result.elements[2 + 0 * 4] = x * z * omc + y * s;
	result.elements[2 + 1 * 4] = y * z * omc - x * s;
	result.elements[2 + 2 * 4] = z * z * omc + c;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4 TRAP::Maths::Mat4::Scale(const Vec3& scale)
{
	Mat4 result = Identity();

	result.elements[0 + 0 * 4] = scale.x;
	result.elements[1 + 1 * 4] = scale.y;
	result.elements[2 + 2 * 4] = scale.z;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4 TRAP::Maths::Mat4::Invert(const Mat4& matrix)
{
	Mat4 result = matrix;

	return result.Invert();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Maths::Mat4 TRAP::Maths::Mat4::Transpose(const Mat4& matrix)
{
	return Mat4
	(
		Vec4(matrix.rows[0].x, matrix.rows[1].x, matrix.rows[2].x, matrix.rows[3].x),
		Vec4(matrix.rows[0].y, matrix.rows[1].y, matrix.rows[2].y, matrix.rows[3].y),
		Vec4(matrix.rows[0].z, matrix.rows[1].z, matrix.rows[2].z, matrix.rows[3].z),
		Vec4(matrix.rows[0].w, matrix.rows[1].w, matrix.rows[2].w, matrix.rows[3].w)
	);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Maths::Mat4::ToString() const
{
	std::stringstream result;
	result << "Mat4: (" << rows[0].x << ", " << rows[1].x << ", " << rows[2].x << ", " << rows[3].x << "), "
		<< '(' << rows[0].y << ", " << rows[1].y << ", " << rows[2].y << ", " << rows[3].y << "), "
		<< '(' << rows[0].z << ", " << rows[1].z << ", " << rows[2].z << ", " << rows[3].z << "), "
		<< '(' << rows[0].w << ", " << rows[1].w << ", " << rows[2].w << ", " << rows[3].w << ')';

	return result.str();
}
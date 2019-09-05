#include "TRAPPCH.h"
#include "Math.h"

////////////
//Matrix 3//
////////////
TRAP::Math::Mat3::Mat3(const float diagonal)
{
	elements[0 + 0 * 3] = diagonal;
	elements[1 + 1 * 3] = diagonal;
	elements[2 + 2 * 3] = diagonal;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat3::Mat3(float *elements)
{
	std::memcpy(this->elements.data(), elements, 3 * 3 * sizeof(float));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat3::Mat3(const tVec3<float> &row0, const tVec3<float> &row1, const tVec3<float> &row2)
{
	rows[0] = row0;
	rows[1] = row1;
	rows[2] = row2;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat3 TRAP::Math::Mat3::Identity()
{
	return Mat3(1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat3 &TRAP::Math::Mat3::Multiply(const Mat3 &other)
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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec3<float> TRAP::Math::Mat3::Multiply(const tVec3<float> &other) const
{
	return other.Multiply(*this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec4<float> TRAP::Math::Mat3::Multiply(const tVec4<float> &other) const
{
	return other.Multiply(*this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat3 TRAP::Math::operator*(Mat3 left, const Mat3 &right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat3 &TRAP::Math::Mat3::operator*=(const Mat3 &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec3<float> TRAP::Math::operator*(const Mat3 &left, const tVec3<float> &right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec4<float> TRAP::Math::operator*(const Mat3 &left, const tVec4<float> &right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec3<float> TRAP::Math::Mat3::GetColumn(const int32_t index) const
{
	return tVec3<float>(elements[index + 0 * 3], elements[index + 1 * 3], elements[index + 2 * 3]);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Math::Mat3::SetColumn(const uint32_t index, const tVec3<float> &column)
{
	elements[index + 0 * 3] = column.x;
	elements[index + 1 * 3] = column.y;
	elements[index + 2 * 3] = column.z;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec3<float> TRAP::Math::Mat3::GetPosition() const
{
	return tVec3<float>(GetColumn(3));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Math::Mat3::SetPosition(const tVec3<float> &position)
{
	SetColumn(3, position);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat3 TRAP::Math::Mat3::Transpose(const Mat3 &matrix)
{
	return Mat3(
		tVec3<float>(matrix.rows[0].x, matrix.rows[1].x, matrix.rows[2].x),
		tVec3<float>(matrix.rows[0].y, matrix.rows[1].y, matrix.rows[2].y),
		tVec3<float>(matrix.rows[0].z, matrix.rows[1].z, matrix.rows[2].z));
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Math::Mat3::ToString() const
{
	std::stringstream result;
	result << rows[0].x << ", " << rows[1].x << ", " << rows[2].x << ", "
		   << rows[0].y << ", " << rows[1].y << ", " << rows[2].y << ", "
		   << rows[0].z << ", " << rows[1].z << ", " << rows[2].z;

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
//Matrix 4//
////////////
TRAP::Math::Mat4::Mat4(const float diagonal)
{
	elements[0 + 0 * 4] = diagonal;
	elements[1 + 1 * 4] = diagonal;
	elements[2 + 2 * 4] = diagonal;
	elements[3 + 3 * 4] = diagonal;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4::Mat4(float *elements)
{
	std::memcpy(this->elements.data(), elements, 4 * 4 * sizeof(float));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4::Mat4(const tVec4<float> &row0, const tVec4<float> &row1, const tVec4<float> &row2, const tVec4<float> &row3)
{
	rows[0] = row0;
	rows[1] = row1;
	rows[2] = row2;
	rows[3] = row3;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 TRAP::Math::Mat4::Identity()
{
	return Mat4(1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 &TRAP::Math::Mat4::Multiply(const Mat4 &other)
{
	std::array<float, 16> data{};
	for (uint32_t row = 0; row < 4; row++)
		for (uint32_t col = 0; col < 4; col++)
		{
			float sum = 0.0f;
			for (uint32_t e = 0; e < 4; e++)
				sum += elements[4 * row + e] * other.elements[col + e * 4];

			data[col + row * 4] = sum;
		}

	std::memcpy(elements.data(), data.data(), 4 * 4 * sizeof(float));

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec3<float> TRAP::Math::Mat4::Multiply(const tVec3<float> &other) const
{
	return other.Multiply(*this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec4<float> TRAP::Math::Mat4::Multiply(const tVec4<float> &other) const
{
	return other.Multiply(*this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 TRAP::Math::operator*(Mat4 left, const Mat4 &right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 &TRAP::Math::Mat4::operator*=(const Mat4 &other)
{
	return Multiply(other);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec3<float> TRAP::Math::operator*(const Mat4 &left, const tVec3<float> &right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec4<float> TRAP::Math::operator*(const Mat4 &left, const tVec4<float> &right)
{
	return left.Multiply(right);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 &TRAP::Math::Mat4::Invert()
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
	temp[6] = -elements[0] * elements[6] * elements[15] + elements[0] * elements[7] * elements[14] + elements[4] * elements[2] * elements[15] - elements[4] * elements[3] * elements[14] - elements[12] * elements[2] * elements[7] + elements[12] * elements[3] * elements[6];
	temp[10] = elements[0] * elements[5] * elements[15] - elements[0] * elements[7] * elements[13] - elements[4] * elements[1] * elements[15] + elements[4] * elements[3] * elements[13] + elements[12] * elements[1] * elements[7] - elements[12] * elements[3] * elements[5];
	temp[14] = -elements[0] * elements[5] * elements[14] + elements[0] * elements[6] * elements[13] + elements[4] * elements[1] * elements[14] - elements[4] * elements[2] * elements[13] - elements[12] * elements[1] * elements[6] + elements[12] * elements[2] * elements[5];
	temp[3] = -elements[1] * elements[6] * elements[11] + elements[1] * elements[7] * elements[10] + elements[5] * elements[2] * elements[11] - elements[5] * elements[3] * elements[10] - elements[9] * elements[2] * elements[7] + elements[9] * elements[3] * elements[6];
	temp[7] = elements[0] * elements[6] * elements[11] - elements[0] * elements[7] * elements[10] - elements[4] * elements[2] * elements[11] + elements[4] * elements[3] * elements[10] + elements[8] * elements[2] * elements[7] - elements[8] * elements[3] * elements[6];
	temp[11] = -elements[0] * elements[5] * elements[11] + elements[0] * elements[7] * elements[9] + elements[4] * elements[1] * elements[11] - elements[4] * elements[3] * elements[9] - elements[8] * elements[1] * elements[7] + elements[8] * elements[3] * elements[5];
	temp[15] = elements[0] * elements[5] * elements[10] - elements[0] * elements[6] * elements[9] - elements[4] * elements[1] * elements[10] + elements[4] * elements[2] * elements[9] + elements[8] * elements[1] * elements[6] - elements[8] * elements[2] * elements[5];

	float determinant = elements[0] * temp[0] + elements[1] * temp[4] + elements[2] * temp[8] + elements[3] * temp[12];
	determinant = 1.0f / determinant;

	for (int32_t i = 0; i < 4 * 4; i++)
		elements[i] = temp[i] * determinant;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec4<float> TRAP::Math::Mat4::GetColumn(const int32_t index) const
{
	return tVec4<float>(elements[index + 0 * 4], elements[index + 1 * 4], elements[index + 2 * 4], elements[index + 3 * 4]);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Math::Mat4::SetColumn(const uint32_t index, const tVec4<float> &column)
{
	elements[index + 0 * 4] = column.x;
	elements[index + 1 * 4] = column.y;
	elements[index + 2 * 4] = column.z;
	elements[index + 3 * 4] = column.w;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::tVec3<float> TRAP::Math::Mat4::GetPosition() const
{
	//return tVec3<float>(GetColumn(3));
	return tVec3<float>(GetColumn(3).x, GetColumn(3).y, GetColumn(3).z);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Math::Mat4::SetPosition(const tVec3<float> &position)
{
	//SetColumn(3, tVec4<float>(position, 1.0f));
	SetColumn(3, tVec4<float>(position.x, position.y, position.z, 1.0f));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 TRAP::Math::Mat4::Orthographic(const float left, const float right, const float bottom, const float top, const float near, const float far)
{
	Mat4 result = Identity();

	result.elements[0 + 0 * 4] = 2.0f / (right - left);
	result.elements[1 + 1 * 4] = 2.0f / (top - bottom);
	result.elements[2 + 2 * 4] = -2.0f / (far - near);

	result.elements[3 + 0 * 4] = -(right + left) / (right - left);
	result.elements[3 + 1 * 4] = -(top + bottom) / (top - bottom);
	result.elements[3 + 2 * 4] = -(far + near) / (far - near);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 TRAP::Math::Mat4::Perspective(const float fov, const float aspectRatio, const float near, const float far)
{
	Mat4 result = Mat4(0.0f);

	const float tanHalfFOV = Tan(ToRadians(fov) / 2.0f);

	result.elements[0 + 0 * 4] = 1.0f / (aspectRatio * tanHalfFOV);
	result.elements[1 + 1 * 4] = 1.0f / tanHalfFOV;
	result.elements[2 + 2 * 4] = -(far + near) / (far - near);
	result.elements[2 + 3 * 4] = -1.0f;
	result.elements[3 + 2 * 4] = -(2.0f * far * near) / (far - near);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 TRAP::Math::Mat4::LookAt(const tVec3<float> &camera, const tVec3<float> &object, const tVec3<float> &up)
{
	Mat4 result = Identity();
	const tVec3<float> f = (object - camera).Normalize();
	const tVec3<float> s = f.Cross(up.Normalize());
	const tVec3<float> u = s.Cross(f);

	result.elements[0 + 0 * 4] = s.x;
	result.elements[0 + 1 * 4] = s.y;
	result.elements[0 + 2 * 4] = s.z;

	result.elements[1 + 0 * 4] = u.x;
	result.elements[1 + 1 * 4] = u.y;
	result.elements[1 + 2 * 4] = u.z;

	result.elements[2 + 0 * 4] = -f.x;
	result.elements[2 + 1 * 4] = -f.y;
	result.elements[2 + 2 * 4] = -f.z;

	return result * Translate(tVec3<float>(-camera.x, -camera.y, -camera.z));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 TRAP::Math::Mat4::Translate(const tVec3<float> &translation)
{
	Mat4 result = Identity();

	result.elements[3 + 0 * 4] = translation.x;
	result.elements[3 + 1 * 4] = translation.y;
	result.elements[3 + 2 * 4] = translation.z;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 TRAP::Math::Mat4::Rotate(const float angle, const tVec3<float> &axis)
{
	Mat4 result = Identity();

	const float r = ToRadians(angle);
	const float c = Cos(r);
	const float s = Sin(r);
	const float omc = 1.0f - c;

	result.elements[0 + 0 * 4] = axis.x * axis.x * omc + c;
	result.elements[0 + 1 * 4] = axis.y * axis.x * omc + axis.z * s;
	result.elements[0 + 2 * 4] = axis.x * axis.z * omc - axis.y * s;

	result.elements[1 + 0 * 4] = axis.x * axis.y * omc - axis.z * s;
	result.elements[1 + 1 * 4] = axis.y * axis.y * omc + c;
	result.elements[1 + 2 * 4] = axis.y * axis.z * omc + axis.x * s;

	result.elements[2 + 0 * 4] = axis.x * axis.z * omc + axis.y * s;
	result.elements[2 + 1 * 4] = axis.y * axis.z * omc - axis.x * s;
	result.elements[2 + 2 * 4] = axis.z * axis.z * omc + c;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 TRAP::Math::Mat4::Scale(const tVec3<float> &scale)
{
	Mat4 result = Identity();

	result.elements[0 + 0 * 4] = scale.x;
	result.elements[1 + 1 * 4] = scale.y;
	result.elements[2 + 2 * 4] = scale.z;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 TRAP::Math::Mat4::Invert(const Mat4 &matrix)
{
	Mat4 result = matrix;

	return result.Invert();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Mat4 TRAP::Math::Mat4::Transpose(const Mat4 &matrix)
{
	return Mat4(
		tVec4<float>(matrix.rows[0].x, matrix.rows[1].x, matrix.rows[2].x, matrix.rows[3].x),
		tVec4<float>(matrix.rows[0].y, matrix.rows[1].y, matrix.rows[2].y, matrix.rows[3].y),
		tVec4<float>(matrix.rows[0].z, matrix.rows[1].z, matrix.rows[2].z, matrix.rows[3].z),
		tVec4<float>(matrix.rows[0].w, matrix.rows[1].w, matrix.rows[2].w, matrix.rows[3].w));
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Math::Mat4::ToString() const
{
	std::stringstream result;
	result << rows[0].x << ", " << rows[1].x << ", " << rows[2].x << ", " << rows[3].x << ", "
		   << rows[0].y << ", " << rows[1].y << ", " << rows[2].y << ", " << rows[3].y << ", "
		   << rows[0].z << ", " << rows[1].z << ", " << rows[2].z << ", " << rows[3].z << ", "
		   << rows[0].w << ", " << rows[1].w << ", " << rows[2].w << ", " << rows[3].w;

	return result.str();
}
#ifndef _TRAP_TYPES_H_
#define _TRAP_TYPES_H_

namespace TRAP::Math
{
	template<int L, typename T> struct Vec;
	template<int C, int R, typename T> struct Mat;
	template<typename T> struct tQuaternion;

	template <typename T> using tVec2 = Vec<2, T>;
	template <typename T> using tVec3 = Vec<3, T>;
	template <typename T> using tVec4 = Vec<4, T>;
	template <typename T> using tMat3 = Mat<3, 3, T>;
	template <typename T> using tMat4 = Mat<4, 4, T>;
	
	using Vec2    = Vec<2, float>;
	using Vec2f   = Vec<2, float>;
	using Vec2d   = Vec<2, double>;
	using Vec2i   = Vec<2, int32_t>;
	using Vec2ui  = Vec<2, uint32_t>;
	using Vec2i8  = Vec<2, int8_t>;
	using Vec2i16 = Vec<2, int16_t>;
	using Vec2i32 = Vec<2, int32_t>;
	using Vec2i64 = Vec<2, int64_t>;
	using Vec2b   = Vec<2, bool>;

	using Vec3    = Vec<3, float>;
	using Vec3f   = Vec<3, float>;
	using Vec3d   = Vec<3, double>;
	using Vec3i   = Vec<3, int32_t>;
	using Vec3ui  = Vec<3, uint32_t>;
	using Vec3i8  = Vec<3, int8_t>;
	using Vec3i16 = Vec<3, int16_t>;
	using Vec3i32 = Vec<3, int32_t>;
	using Vec3i64 = Vec<3, int64_t>;
	using Vec3b   = Vec<3, bool>;

	using Vec4    = Vec<4, float>;
	using Vec4f   = Vec<4, float>;
	using Vec4d   = Vec<4, double>;
	using Vec4i   = Vec<4, int32_t>;
	using Vec4ui  = Vec<4, uint32_t>;
	using Vec4i8  = Vec<4, int8_t>;
	using Vec4i16 = Vec<4, int16_t>;
	using Vec4i32 = Vec<4, int32_t>;
	using Vec4i64 = Vec<4, int64_t>;
	using Vec4b   = Vec<4, bool>;

	using Mat3  = Mat<3, 3, float>;
	using Mat3f = Mat<3, 3, float>;
	using Mat3d = Mat<3, 3, double>;

	using Mat4  = Mat<4, 4, float>;
	using Mat4f = Mat<4, 4, float>;
	using Mat4d = Mat<4, 4, double>;

	using Quaternion = tQuaternion<float>;
	using Quaternionf = tQuaternion<float>;
	using Quaterniond = tQuaternion<double>;	
}

#endif /*_TRAP_TYPES_H_*/
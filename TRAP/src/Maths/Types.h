#ifndef TRAP_TYPES_H
#define TRAP_TYPES_H

#include <cstdint>
#include <concepts>

namespace TRAP::Math
{
	template<uint32_t L, typename T> requires std::is_arithmetic_v<T> struct Vec;
	template<uint32_t C, uint32_t R, typename T> requires std::floating_point<T> struct Mat;
	template<typename T> requires std::floating_point<T> struct tQuat;

	template <typename T> using tVec2 = Vec<2, T>;
	template <typename T> using tVec3 = Vec<3, T>;
	template <typename T> using tVec4 = Vec<4, T>;
	template <typename T> using tMat3 = Mat<3, 3, T>;
	template <typename T> using tMat4 = Mat<4, 4, T>;

	using Vec2      = Vec<2, float>;
	using Vec2f     = Vec<2, float>;
	using Vec2d     = Vec<2, double>;
	using Vec2i     = Vec<2, int32_t>;
	using Vec2ui    = Vec<2, uint32_t>;
	using Vec2ui8   = Vec<2, uint8_t>;
	using Vec2ui16  = Vec<2, uint16_t>;
	using Vec2ui32  = Vec<2, uint32_t>;
	using Vec2ui64  = Vec<2, uint64_t>;
	using Vec2i8    = Vec<2, int8_t>;
	using Vec2i16   = Vec<2, int16_t>;
	using Vec2i32   = Vec<2, int32_t>;
	using Vec2i64   = Vec<2, int64_t>;
	using Vec2b     = Vec<2, bool>;

	using Vec3      = Vec<3, float>;
	using Vec3f     = Vec<3, float>;
	using Vec3d     = Vec<3, double>;
	using Vec3i     = Vec<3, int32_t>;
	using Vec3ui    = Vec<3, uint32_t>;
	using Vec3ui8   = Vec<3, uint8_t>;
	using Vec3ui16  = Vec<3, uint16_t>;
	using Vec3ui32  = Vec<3, uint32_t>;
	using Vec3ui64  = Vec<3, uint64_t>;
	using Vec3i8    = Vec<3, int8_t>;
	using Vec3i16   = Vec<3, int16_t>;
	using Vec3i32   = Vec<3, int32_t>;
	using Vec3i64   = Vec<3, int64_t>;
	using Vec3b     = Vec<3, bool>;

	using Vec4      = Vec<4, float>;
	using Vec4f     = Vec<4, float>;
	using Vec4d     = Vec<4, double>;
	using Vec4i     = Vec<4, int32_t>;
	using Vec4ui    = Vec<4, uint32_t>;
	using Vec4ui8   = Vec<4, uint8_t>;
	using Vec4ui16  = Vec<4, uint16_t>;
	using Vec4ui32  = Vec<4, uint32_t>;
	using Vec4ui64  = Vec<4, uint64_t>;
	using Vec4i8    = Vec<4, int8_t>;
	using Vec4i16   = Vec<4, int16_t>;
	using Vec4i32   = Vec<4, int32_t>;
	using Vec4i64   = Vec<4, int64_t>;
	using Vec4b     = Vec<4, bool>;

	using Mat3  = Mat<3, 3, float>;
	using Mat3f = Mat<3, 3, float>;
	using Mat3d = Mat<3, 3, double>;

	using Mat4  = Mat<4, 4, float>;
	using Mat4f = Mat<4, 4, float>;
	using Mat4d = Mat<4, 4, double>;

	using Quat  = tQuat<float>;
	using Quatf = tQuat<float>;
	using Quatd = tQuat<double>;

	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	//Concepts

	/// <summary>
	/// Concept which evaluates to true if T is of type TRAP::Math::tVec2<>
	/// </summary>
	template<typename T>
	concept IsVec2 = std::same_as<T, tVec2<typename T::valueType>>;

	/// <summary>
	/// Concept which evaluates to true if T is of type TRAP::Math::tVec3<>
	/// </summary>
	template<typename T>
	concept IsVec3 = std::same_as<T, tVec3<typename T::valueType>>;

	/// <summary>
	/// Concept which evaluates to true if T is of type TRAP::Math::tVec4<>
	/// </summary>
	template<typename T>
	concept IsVec4 = std::same_as<T, tVec4<typename T::valueType>>;

	/// <summary>
	/// Concept which evaluates to true if T is any of type
	/// TRAP::Math::tVec2<>, TRAP::Math::tVec3<> or TRAP::Math::tVec4<>
	/// </summary>
	template<typename T>
	concept IsVec = IsVec2<T> || IsVec3<T> || IsVec4<T>;

	/// <summary>
	/// Concept which evaluates to true if T is of type TRAP::Math::tMat3<>
	/// </summary>
	template<typename T>
	concept IsMat3 = std::same_as<T, tMat3<typename T::valueType>>;

	/// <summary>
	/// Concept which evaluates to true if T is of type TRAP::Math::tMat4<>
	/// </summary>
	template<typename T>
	concept IsMat4 = std::same_as<T, tMat4<typename T::valueType>>;

	/// <summary>
	/// Concept which evaluates to true if T is any of type
	/// TRAP::Math::tMat3<> or TRAP::Math::tMat4<>
	/// </summary>
	template<typename T>
	concept IsMat = IsMat3<T> || IsMat4<T>;

	/// <summary>
	/// Concept which evaluates to true if T is of type TRAP::Math::tQuat<>
	/// </summary>
	template<typename T>
	concept IsQuat = std::same_as<T, tQuat<typename T::valueType>>;
}

#endif /*TRAP_TYPES_H*/
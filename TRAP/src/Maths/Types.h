#ifndef TRAP_TYPES_H
#define TRAP_TYPES_H

#include <cstdint>
#include <concepts>

#include "Core/Types.h"

namespace TRAP::Math
{
	template<u32 L, typename T> requires std::is_arithmetic_v<T> struct Vec;
	template<u32 C, u32 R, typename T> requires std::floating_point<T> struct Mat;
	template<typename T> requires std::floating_point<T> struct tQuat;

	template <typename T> using tVec2 = Vec<2, T>;
	template <typename T> using tVec3 = Vec<3, T>;
	template <typename T> using tVec4 = Vec<4, T>;
	template <typename T> using tMat3 = Mat<3, 3, T>;
	template <typename T> using tMat4 = Mat<4, 4, T>;

	using Vec2      = Vec<2, f32>;
	using Vec2f     = Vec<2, f32>;
	using Vec2d     = Vec<2, f64>;
	using Vec2i     = Vec<2, i32>;
	using Vec2ui    = Vec<2, u32>;
	using Vec2ui8   = Vec<2, u8>;
	using Vec2ui16  = Vec<2, u16>;
	using Vec2ui32  = Vec<2, u32>;
	using Vec2ui64  = Vec<2, u64>;
	using Vec2i8    = Vec<2, i8>;
	using Vec2i16   = Vec<2, i16>;
	using Vec2i32   = Vec<2, i32>;
	using Vec2i64   = Vec<2, i64>;
	using Vec2b     = Vec<2, bool>;

	using Vec3      = Vec<3, f32>;
	using Vec3f     = Vec<3, f32>;
	using Vec3d     = Vec<3, f64>;
	using Vec3i     = Vec<3, i32>;
	using Vec3ui    = Vec<3, u32>;
	using Vec3ui8   = Vec<3, u8>;
	using Vec3ui16  = Vec<3, u16>;
	using Vec3ui32  = Vec<3, u32>;
	using Vec3ui64  = Vec<3, u64>;
	using Vec3i8    = Vec<3, i8>;
	using Vec3i16   = Vec<3, i16>;
	using Vec3i32   = Vec<3, i32>;
	using Vec3i64   = Vec<3, i64>;
	using Vec3b     = Vec<3, bool>;

	using Vec4      = Vec<4, f32>;
	using Vec4f     = Vec<4, f32>;
	using Vec4d     = Vec<4, f64>;
	using Vec4i     = Vec<4, i32>;
	using Vec4ui    = Vec<4, u32>;
	using Vec4ui8   = Vec<4, u8>;
	using Vec4ui16  = Vec<4, u16>;
	using Vec4ui32  = Vec<4, u32>;
	using Vec4ui64  = Vec<4, u64>;
	using Vec4i8    = Vec<4, i8>;
	using Vec4i16   = Vec<4, i16>;
	using Vec4i32   = Vec<4, i32>;
	using Vec4i64   = Vec<4, i64>;
	using Vec4b     = Vec<4, bool>;

	using Mat3  = Mat<3, 3, f32>;
	using Mat3f = Mat<3, 3, f32>;
	using Mat3d = Mat<3, 3, f64>;

	using Mat4  = Mat<4, 4, f32>;
	using Mat4f = Mat<4, 4, f32>;
	using Mat4d = Mat<4, 4, f64>;

	using Quat  = tQuat<f32>;
	using Quatf = tQuat<f32>;
	using Quatd = tQuat<f64>;

	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	//Concepts

	/// @brief Concept which evaluates to true if T is of type TRAP::Math::tVec2<>
	template<typename T>
	concept IsVec2 = std::same_as<T, tVec2<typename T::value_type>>;

	/// @brief Concept which evaluates to true if T is of type TRAP::Math::tVec3<>
	template<typename T>
	concept IsVec3 = std::same_as<T, tVec3<typename T::value_type>>;

	/// @brief Concept which evaluates to true if T is of type TRAP::Math::tVec4<>
	template<typename T>
	concept IsVec4 = std::same_as<T, tVec4<typename T::value_type>>;

	/// @brief Concept which evaluates to true if T is any of type
	/// TRAP::Math::tVec2<>, TRAP::Math::tVec3<> or TRAP::Math::tVec4<>
	template<typename T>
	concept IsVec = IsVec2<T> || IsVec3<T> || IsVec4<T>;

	/// @brief Concept which evaluates to true if T is of type TRAP::Math::tMat3<>
	template<typename T>
	concept IsMat3 = std::same_as<T, tMat3<typename T::value_type>>;

	/// @brief Concept which evaluates to true if T is of type TRAP::Math::tMat4<>
	template<typename T>
	concept IsMat4 = std::same_as<T, tMat4<typename T::value_type>>;

	/// @brief Concept which evaluates to true if T is any of type
	/// TRAP::Math::tMat3<> or TRAP::Math::tMat4<>
	template<typename T>
	concept IsMat = IsMat3<T> || IsMat4<T>;

	/// @brief Concept which evaluates to true if T is of type TRAP::Math::tQuat<>
	template<typename T>
	concept IsQuat = std::same_as<T, tQuat<typename T::value_type>>;
}

#endif /*TRAP_TYPES_H*/

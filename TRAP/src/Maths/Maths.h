#ifndef _TRAP_MATHS_H_
#define _TRAP_MATHS_H_

#include "MathsFunc.h"
#include "Math.h"

namespace TRAP::Math
{
	template<class T>
	struct tVec2;
	template<class T>
	struct tVec3;
	template<class T>
	struct tVec4;
	struct Mat3;
	struct Mat4;

	typedef tVec2<float> Vec2;
	typedef tVec2<float> Vec2f;
	typedef tVec2<int32_t> Vec2i;
	typedef tVec2<uint32_t> Vec2ui;

	typedef tVec3<float> Vec3;
	typedef tVec3<float> Vec3f;
	typedef tVec3<int32_t> Vec3i;
	typedef tVec3<uint32_t> Vec3ui;

	typedef tVec4<float> Vec4;
	typedef tVec4<float> Vec4f;
	typedef tVec4<int32_t> Vec4i;
	typedef tVec4<uint32_t> Vec4ui;
}

#endif /*_TRAP_MATHS_H_*/
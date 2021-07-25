#ifndef _TRAP_CAMERA_H_
#define _TRAP_CAMERA_H_

#include "Maths/Math.h"

namespace TRAP::Graphics
{
	class Camera
	{
	public:
		Camera() = default;
		explicit Camera(const Math::Mat4& projection);
		virtual ~Camera() = default;
		Camera(const Camera&) = default;
		Camera(Camera&&) = default;
		Camera& operator=(const Camera&) = default;
		Camera& operator=(Camera&&) = default;

		const Math::Mat4& GetProjectionMatrix() const;

	protected:
		Math::Mat4 m_projection = Math::Mat4(1.0f);
	};
}

#endif /*_TRAP_CAMERA_H_*/
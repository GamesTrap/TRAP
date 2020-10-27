#ifndef _TRAP_SCENECAMERA_H_
#define _TRAP_SCENECAMERA_H_

#include "Graphics/Cameras/Camera.h"

namespace TRAP
{
	class SceneCamera : public Graphics::Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		SceneCamera(const SceneCamera&) = default;
		SceneCamera(SceneCamera&&) = default;
		SceneCamera& operator=(const SceneCamera&) = default;
		SceneCamera& operator=(SceneCamera&&) = default;

		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthographicSize() const;
		void SetOrthographicSize(float size);
	private:
		void RecalculateProjection();
		
		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;

		float m_aspectRatio = 0.0f;
	};
}

#endif /*_TRAP_SCENECAMERA_H_*/
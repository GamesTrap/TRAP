#ifndef TRAP_SCENECAMERA_H
#define TRAP_SCENECAMERA_H

#include "Graphics/Cameras/Camera.h"

namespace TRAP
{
	//TODO Document
	class SceneCamera : public Graphics::Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0,
			Orthographic = 1
		};

		SceneCamera();
		virtual ~SceneCamera() = default;

		SceneCamera(const SceneCamera&) = default;
		SceneCamera(SceneCamera&&) = default;
		SceneCamera& operator=(const SceneCamera&) = default;
		SceneCamera& operator=(SceneCamera&&) = default;

		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const;
		void SetPerspectiveVerticalFOV(float verticalFov);
		Math::Vec2 GetPerspectiveClip() const;
		void SetPerspectiveClip(Math::Vec2 clip);
		float GetPerspectiveNearClip() const;
		void SetPerspectiveNearClip(float nearClip);
		float GetPerspectiveFarClip() const;
		void SetPerspectiveFarClip(float farClip);

		float GetOrthographicSize() const;
		void SetOrthographicSize(float size);
		Math::Vec2 GetOrthographicClip() const;
		void SetOrthographicClip(Math::Vec2 clip);
		float GetOrthographicNearClip() const;
		void SetOrthographicNearClip(float nearClip);
		float GetOrthographicFarClip() const;
		void SetOrthographicFarClip(float farClip);

		ProjectionType GetProjectionType() const;
		void SetProjectionType(ProjectionType type);
	private:
		void RecalculateProjection();

		ProjectionType m_projectionType = ProjectionType::Orthographic;

		float m_perspectiveFOV = Math::Radians(45.0f);
		float m_perspectiveNear = 0.01f, m_perspectiveFar = 1000.0f;

		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;

		float m_aspectRatio = 0.0f;
	};
}

#endif /*TRAP_SCENECAMERA_H*/
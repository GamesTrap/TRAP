#ifndef TRAP_SCENECAMERA_H
#define TRAP_SCENECAMERA_H

#include "Graphics/Cameras/Camera.h"

namespace TRAP
{
	//TODO Document
	class SceneCamera : public Graphics::Camera
	{
	public:
		enum class ProjectionType : uint32_t
		{
			Perspective = 0,
			Orthographic = 1
		};

		SceneCamera();
		~SceneCamera() override = default;

		SceneCamera(const SceneCamera&) noexcept = default;
		SceneCamera(SceneCamera&&) noexcept = default;
		SceneCamera& operator=(const SceneCamera&) noexcept = default;
		SceneCamera& operator=(SceneCamera&&) noexcept = default;

		void SetPerspective(float verticalFOV, float nearClip);
		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		[[nodiscard]] float GetPerspectiveVerticalFOV() const noexcept;
		void SetPerspectiveVerticalFOV(float verticalFov);
		[[nodiscard]] float GetPerspectiveNearClip() const noexcept;
		void SetPerspectiveNearClip(float nearClip);

		[[nodiscard]] float GetOrthographicSize() const noexcept;
		void SetOrthographicSize(float size);
		[[nodiscard]] Math::Vec2 GetOrthographicClip() const noexcept;
		void SetOrthographicClip(Math::Vec2 clip);
		[[nodiscard]] float GetOrthographicNearClip() const noexcept;
		void SetOrthographicNearClip(float nearClip);
		[[nodiscard]] float GetOrthographicFarClip() const noexcept;
		void SetOrthographicFarClip(float farClip);

		[[nodiscard]] ProjectionType GetProjectionType() const noexcept;
		void SetProjectionType(ProjectionType type);
	private:
		void RecalculateProjection();

		ProjectionType m_projectionType = ProjectionType::Orthographic;

		float m_perspectiveFOV = Math::Radians(45.0f);
		float m_perspectiveNear = 0.01f;

		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;

		float m_aspectRatio = 0.0f;
	};
}

#endif /*TRAP_SCENECAMERA_H*/
#ifndef TRAP_SCENECAMERA_H
#define TRAP_SCENECAMERA_H

#include "Graphics/Cameras/Camera.h"
#include "Maths/Math.h"

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
		constexpr ~SceneCamera() override = default;

		constexpr SceneCamera(const SceneCamera&) noexcept = default;
		constexpr SceneCamera(SceneCamera&&) noexcept = default;
		SceneCamera& operator=(const SceneCamera&) noexcept = default;
		SceneCamera& operator=(SceneCamera&&) noexcept = default;

		void SetPerspective(float verticalFOV, float nearClip);
		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		[[nodiscard]] constexpr float GetPerspectiveVerticalFOV() const noexcept;
		void SetPerspectiveVerticalFOV(float verticalFov);
		[[nodiscard]] constexpr float GetPerspectiveNearClip() const noexcept;
		void SetPerspectiveNearClip(float nearClip);

		[[nodiscard]] constexpr float GetOrthographicSize() const noexcept;
		void SetOrthographicSize(float size);
		[[nodiscard]] constexpr Math::Vec2 GetOrthographicClip() const noexcept;
		void SetOrthographicClip(Math::Vec2 clip);
		[[nodiscard]] constexpr float GetOrthographicNearClip() const noexcept;
		void SetOrthographicNearClip(float nearClip);
		[[nodiscard]] constexpr float GetOrthographicFarClip() const noexcept;
		void SetOrthographicFarClip(float farClip);

		[[nodiscard]] constexpr ProjectionType GetProjectionType() const noexcept;
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

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::SceneCamera::GetPerspectiveVerticalFOV() const noexcept
{
	return m_perspectiveFOV;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::SceneCamera::GetPerspectiveNearClip() const noexcept
{
	return m_perspectiveNear;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::SceneCamera::GetOrthographicSize() const noexcept
{
	return m_orthographicSize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2 TRAP::SceneCamera::GetOrthographicClip() const noexcept
{
	return { m_orthographicNear, m_orthographicFar };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::SceneCamera::GetOrthographicNearClip() const noexcept
{
	return m_orthographicNear;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::SceneCamera::GetOrthographicFarClip() const noexcept
{
	return m_orthographicFar;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::SceneCamera::ProjectionType TRAP::SceneCamera::GetProjectionType() const noexcept
{
	return m_projectionType;
}

#endif /*TRAP_SCENECAMERA_H*/
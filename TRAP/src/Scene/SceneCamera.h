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
		enum class ProjectionType : u32
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

		void SetPerspective(f32 verticalFOV, f32 nearClip);
		void SetOrthographic(f32 size, f32 nearClip, f32 farClip);

		void SetViewportSize(u32 width, u32 height);

		[[nodiscard]] constexpr f32 GetPerspectiveVerticalFOV() const noexcept;
		void SetPerspectiveVerticalFOV(f32 verticalFov);
		[[nodiscard]] constexpr f32 GetPerspectiveNearClip() const noexcept;
		void SetPerspectiveNearClip(f32 nearClip);

		[[nodiscard]] constexpr f32 GetOrthographicSize() const noexcept;
		void SetOrthographicSize(f32 size);
		[[nodiscard]] constexpr Math::Vec2 GetOrthographicClip() const noexcept;
		void SetOrthographicClip(Math::Vec2 clip);
		[[nodiscard]] constexpr f32 GetOrthographicNearClip() const noexcept;
		void SetOrthographicNearClip(f32 nearClip);
		[[nodiscard]] constexpr f32 GetOrthographicFarClip() const noexcept;
		void SetOrthographicFarClip(f32 farClip);

		[[nodiscard]] constexpr ProjectionType GetProjectionType() const noexcept;
		void SetProjectionType(ProjectionType type);
	private:
		void RecalculateProjection();

		ProjectionType m_projectionType = ProjectionType::Orthographic;

		f32 m_perspectiveFOV = Math::Radians(45.0f);
		f32 m_perspectiveNear = 0.01f;

		f32 m_orthographicSize = 10.0f;
		f32 m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;

		f32 m_aspectRatio = 0.0f;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::SceneCamera::GetPerspectiveVerticalFOV() const noexcept
{
	return m_perspectiveFOV;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::SceneCamera::GetPerspectiveNearClip() const noexcept
{
	return m_perspectiveNear;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::SceneCamera::GetOrthographicSize() const noexcept
{
	return m_orthographicSize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2 TRAP::SceneCamera::GetOrthographicClip() const noexcept
{
	return { m_orthographicNear, m_orthographicFar };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::SceneCamera::GetOrthographicNearClip() const noexcept
{
	return m_orthographicNear;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::SceneCamera::GetOrthographicFarClip() const noexcept
{
	return m_orthographicFar;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::SceneCamera::ProjectionType TRAP::SceneCamera::GetProjectionType() const noexcept
{
	return m_projectionType;
}

#endif /*TRAP_SCENECAMERA_H*/
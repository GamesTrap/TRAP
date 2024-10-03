#ifndef TRAP_SCENECAMERA_H
#define TRAP_SCENECAMERA_H

#include "Graphics/Cameras/Camera.h"
#include "Maths/Math.h"
#include "Utils/NumericCasts.h"

namespace TRAP
{
	//TODO Document
	class SceneCamera : public Graphics::Camera
	{
	public:
		enum class ProjectionType : u8
		{
			Perspective = 0,
			Orthographic = 1
		};

		/// @brief Constructor.
		constexpr SceneCamera();
		/// @brief Destructor
		constexpr ~SceneCamera() override = default;

		/// @brief Copy constructor.
		constexpr SceneCamera(const SceneCamera&) noexcept = default;
		/// @brief Move constructor.
		constexpr SceneCamera(SceneCamera&&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr SceneCamera& operator=(const SceneCamera&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr SceneCamera& operator=(SceneCamera&&) noexcept = default;

		/// @brief Make the camera perspective.
		/// @param verticalFOV Field ov view to use.
		/// @param nearClip Near clip plane.
		/// @note This camera uses an infinite far plane.
		constexpr void SetPerspective(f32 verticalFOV, f32 nearClip);
		/// @brief Make the camera orthographic.
		/// @param size Size for the camera.
		/// @param nearClip Near clip plane.
		/// @param farClip Far clip plane.
		/// @note This camera uses reversed Z coordinates.
		constexpr void SetOrthographic(f32 size, f32 nearClip, f32 farClip);

		/// @brief Update viewport size.
		/// @param width New width.
		/// @param height New height.
		constexpr void SetViewportSize(u32 width, u32 height);

		/// @brief Retrieve the vertical field of view.
		/// @return Vertical field of view.
		[[nodiscard]] constexpr f32 GetPerspectiveVerticalFOV() const noexcept;
		/// @brief Set the vertical field of view.
		/// @param verticalFov Vertical field of view to use.
		constexpr void SetPerspectiveVerticalFOV(f32 verticalFov);
		/// @brief Retrieve the near clip plane.
		/// @return Near clip plane.
		[[nodiscard]] constexpr f32 GetPerspectiveNearClip() const noexcept;
		/// @brief Set the near clip plane.
		/// @param nearClip Near clip plane to use.
		constexpr void SetPerspectiveNearClip(f32 nearClip);

		/// @brief Retrieve the camera size.
		/// @return Camera size.
		[[nodiscard]] constexpr f32 GetOrthographicSize() const noexcept;
		/// @brief Set the camera size.
		/// @param size Size to use.
		constexpr void SetOrthographicSize(f32 size);
		/// @brief Retrieve the clip planes.
		/// @return Near and far clip planes.
		[[nodiscard]] constexpr Math::Vec2 GetOrthographicClip() const noexcept;
		/// @brief Set the near and far clip planes.
		/// @param clip Clip planes to use.
		constexpr void SetOrthographicClip(Math::Vec2 clip);
		/// @brief Retrieve the near clip plane.
		/// @return Near clip plane.
		[[nodiscard]] constexpr f32 GetOrthographicNearClip() const noexcept;
		/// @brief Set the near clip plane.
		/// @param nearClip Near clip plane to use.
		constexpr void SetOrthographicNearClip(f32 nearClip);
		/// @brief Retrieve the far clip plane.
		/// @return Far clip plane.
		[[nodiscard]] constexpr f32 GetOrthographicFarClip() const noexcept;
		/// @brief Set the far clip plane.
		/// @param farClip Far clip plane to use.
		constexpr void SetOrthographicFarClip(f32 farClip);

		/// @brief Retrieve the current projection type of the camera.
		/// @return Current projection type.
		[[nodiscard]] constexpr ProjectionType GetProjectionType() const noexcept;
		/// @brief Set the projection type of the camera.
		/// @param type Projection type to use.
		constexpr void SetProjectionType(ProjectionType type);
	private:
		/// @brief Recalculate the projection matrix of the camera.
		constexpr void RecalculateProjection();

		ProjectionType m_projectionType = ProjectionType::Orthographic;

		f32 m_perspectiveFOV = Math::Radians(45.0f);
		f32 m_perspectiveNear = 0.01f;

		f32 m_orthographicSize = 10.0f;
		f32 m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;

		f32 m_aspectRatio = 0.0f;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::SceneCamera::SceneCamera()
{
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::SetPerspective(const f32 verticalFOV, const f32 nearClip)
{
	m_projectionType = ProjectionType::Perspective;
	m_perspectiveFOV = verticalFOV;
	m_perspectiveNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::SetOrthographic(const f32 size, const f32 nearClip, const f32 farClip)
{
	m_projectionType = ProjectionType::Orthographic;
	m_orthographicSize = size;
	m_orthographicNear = nearClip;
	m_orthographicFar = farClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::SetViewportSize(const u32 width, const u32 height)
{
	m_aspectRatio = NumericCast<f32>(width) / NumericCast<f32>(height);
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::SceneCamera::GetPerspectiveVerticalFOV() const noexcept
{
	return m_perspectiveFOV;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::SetPerspectiveVerticalFOV(const f32 verticalFov)
{
	m_perspectiveFOV = verticalFov;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::SceneCamera::GetPerspectiveNearClip() const noexcept
{
	return m_perspectiveNear;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::SetPerspectiveNearClip(const f32 nearClip)
{
	m_perspectiveNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::SceneCamera::GetOrthographicSize() const noexcept
{
	return m_orthographicSize;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::SetOrthographicSize(const f32 size)
{
	m_orthographicSize = size;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2 TRAP::SceneCamera::GetOrthographicClip() const noexcept
{
	return { m_orthographicNear, m_orthographicFar };
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::SetOrthographicClip(const Math::Vec2 clip)
{
	m_orthographicNear = clip.x();
	m_orthographicFar = clip.y();
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::SceneCamera::GetOrthographicNearClip() const noexcept
{
	return m_orthographicNear;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::SetOrthographicNearClip(const f32 nearClip)
{
	m_orthographicNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::SceneCamera::GetOrthographicFarClip() const noexcept
{
	return m_orthographicFar;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::SetOrthographicFarClip(const f32 farClip)
{
	m_orthographicFar = farClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::SceneCamera::ProjectionType TRAP::SceneCamera::GetProjectionType() const noexcept
{
	return m_projectionType;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::SetProjectionType(const ProjectionType type)
{
	m_projectionType = type;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneCamera::RecalculateProjection()
{
	if(m_projectionType == ProjectionType::Perspective)
		m_projection = Math::InfinitePerspectiveReverseZ(m_perspectiveFOV, m_aspectRatio, m_perspectiveNear);
	else //if (m_projectionType == ProjectionType::Orthographic)
	{
		const f32 orthographicLeft = -m_orthographicSize * m_aspectRatio * 0.5f;
		const f32 orthographicRight = m_orthographicSize * m_aspectRatio * 0.5f;
		const f32 orthographicBottom = -m_orthographicSize * 0.5f;
		const f32 orthographicTop = m_orthographicSize * 0.5f;

		m_projection = Math::OrthographicReverseZ(orthographicLeft,
			                                      orthographicRight,
			                                      orthographicBottom,
			                                      orthographicTop,
										          m_orthographicNear,
												  m_orthographicFar);
	}
}

#endif /*TRAP_SCENECAMERA_H*/

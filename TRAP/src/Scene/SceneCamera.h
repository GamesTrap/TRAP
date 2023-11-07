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

		/// @brief Constructor.
		SceneCamera();
		/// @brief Destructor
		constexpr ~SceneCamera() override = default;

		/// @brief Copy constructor.
		constexpr SceneCamera(const SceneCamera&) noexcept = default;
		/// @brief Move constructor.
		constexpr SceneCamera(SceneCamera&&) noexcept = default;
		/// @brief Copy assignment operator.
		SceneCamera& operator=(const SceneCamera&) noexcept = default;
		/// @brief Move assignment operator.
		SceneCamera& operator=(SceneCamera&&) noexcept = default;

		/// @brief Make the camera perspective.
		/// @param verticalFOV Field ov view to use.
		/// @param nearClip Near clip plane.
		/// @note This camera uses an infinite far plane.
		void SetPerspective(f32 verticalFOV, f32 nearClip);
		/// @brief Make the camera orthographic.
		/// @param size Size for the camera.
		/// @param nearClip Near clip plane.
		/// @param farClip Far clip plane.
		/// @note This camera uses reversed Z coordinates.
		void SetOrthographic(f32 size, f32 nearClip, f32 farClip);

		/// @brief Update viewport size.
		/// @param width New width.
		/// @param height New height.
		void SetViewportSize(u32 width, u32 height);

		/// @brief Retrieve the vertical field of view.
		/// @return Vertical field of view.
		[[nodiscard]] constexpr f32 GetPerspectiveVerticalFOV() const noexcept;
		/// @brief Set the vertical field of view.
		/// @param verticalFov Vertical field of view to use.
		void SetPerspectiveVerticalFOV(f32 verticalFov);
		/// @brief Retrieve the near clip plane.
		/// @return Near clip plane.
		[[nodiscard]] constexpr f32 GetPerspectiveNearClip() const noexcept;
		/// @brief Set the near clip plane.
		/// @param nearClip Near clip plane to use.
		void SetPerspectiveNearClip(f32 nearClip);

		/// @brief Retrieve the camera size.
		/// @return Camera size.
		[[nodiscard]] constexpr f32 GetOrthographicSize() const noexcept;
		/// @brief Set the camera size.
		/// @param size Size to use.
		void SetOrthographicSize(f32 size);
		/// @brief Retrieve the clip planes.
		/// @return Near and far clip planes.
		[[nodiscard]] constexpr Math::Vec2 GetOrthographicClip() const noexcept;
		/// @brief Set the near and far clip planes.
		/// @param clip Clip planes to use.
		void SetOrthographicClip(Math::Vec2 clip);
		/// @brief Retrieve the near clip plane.
		/// @return Near clip plane.
		[[nodiscard]] constexpr f32 GetOrthographicNearClip() const noexcept;
		/// @brief Set the near clip plane.
		/// @param nearClip Near clip plane to use.
		void SetOrthographicNearClip(f32 nearClip);
		/// @brief Retrieve the far clip plane.
		/// @return Far clip plane.
		[[nodiscard]] constexpr f32 GetOrthographicFarClip() const noexcept;
		/// @brief Set the far clip plane.
		/// @param farClip Far clip plane to use.
		void SetOrthographicFarClip(f32 farClip);

		/// @brief Retrieve the current projection type of the camera.
		/// @return Current projection type.
		[[nodiscard]] constexpr ProjectionType GetProjectionType() const noexcept;
		/// @brief Set the projection type of the camera.
		/// @param type Projection type to use.
		void SetProjectionType(ProjectionType type);
	private:
		/// @brief Recalculate the projection matrix of the camera.
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
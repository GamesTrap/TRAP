#ifndef TRAP_ORTHOGRAPHICCAMERA_H
#define TRAP_ORTHOGRAPHICCAMERA_H

#include "Maths/Math.h"

namespace TRAP::Graphics
{
	//TODO Make this derived from Camera
	class OrthographicCamera final
	{
	public:
		/// @brief Constructor.
		/// Initializes the Orthographic/2D camera.
		/// @param left Left screen coordinate.
		/// @param right Right screen coordinate.
		/// @param bottom Bottom screen coordinate.
		/// @param top Top screen coordinate.
		/// @param near Near distance.
		/// @param far Far distance.
		constexpr OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

		/// @brief Set the projection matrix via screen coordinates.
		/// @param left Left screen coordinate.
		/// @param right Right screen coordinate.
		/// @param bottom Bottom screen coordinate.
		/// @param top Top screen coordinate.
		/// @param near Near distance.
		/// @param far Far distance.
		constexpr void SetProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

		/// @brief Retrieve the current camera position.
		/// @return Current camera position.
		[[nodiscard]] constexpr const Math::Vec3& GetPosition() const noexcept;
		/// @brief Set the camera position.
		/// @param position New camera position.
		constexpr void SetPosition(const Math::Vec3& position);

		/// @brief Retrieve the current camera rotation.
		/// @return Current camera rotation.
		[[nodiscard]] constexpr const Math::Vec3& GetRotation() const noexcept;
		/// @brief Set the camera rotation.
		/// @param rotation New camera rotation.
		constexpr void SetRotation(const Math::Vec3& rotation);

		/// @brief Retrieve the projection matrix.
		/// @return Projection matrix.
		[[nodiscard]] constexpr const Math::Mat4& GetProjectionMatrix() const noexcept;
		/// @brief Retrieve the view matrix.
		/// @return View matrix.
		[[nodiscard]] constexpr const Math::Mat4& GetViewMatrix() const noexcept;

	private:
		/// @brief Recalculate the view matrix.
		/// This applies the camera position and rotation to the view matrix.
		constexpr void RecalculateViewMatrix();

		Math::Mat4 m_projectionMatrix;
		Math::Mat4 m_viewMatrix = Math::Mat4(1.0f);

		Math::Vec3 m_position = Math::Vec3(0.0f);
		Math::Vec3 m_rotation = Math::Vec3(0.0f);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::OrthographicCamera::OrthographicCamera(const f32 left, const f32 right, const f32 bottom,
                                                                 const f32 top, const f32 near, const f32 far)
	: m_projectionMatrix(Math::OrthographicReverseZ(left, right, bottom, top, near, far))
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCamera::SetProjection(const f32 left, const f32 right, const f32 bottom,
                                                                 const f32 top, const f32 near, const f32 far)
{
	m_projectionMatrix = Math::OrthographicReverseZ(left, right, bottom, top, near, far);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetPosition() const noexcept
{
	return m_position;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCamera::SetPosition(const Math::Vec3& position)
{
	m_position = position;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetRotation() const noexcept
{
	return m_rotation;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCamera::SetRotation(const Math::Vec3& rotation)
{
	m_rotation = rotation;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetProjectionMatrix() const noexcept
{
	return m_projectionMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetViewMatrix() const noexcept
{
	return m_viewMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCamera::RecalculateViewMatrix()
{
	const Math::Mat4 transform = Translate(m_position) * Mat4Cast(Math::Quat(Radians(m_rotation)));

	m_viewMatrix = Inverse(transform);
}

#endif /*TRAP_ORTHOGRAPHICCAMERA_H*/
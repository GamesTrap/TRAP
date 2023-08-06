#ifndef TRAP_ORTHOGRAPHICCAMERA_H
#define TRAP_ORTHOGRAPHICCAMERA_H

#include "Maths/Math.h"

namespace TRAP::Graphics
{
	//TODO Make this derived from Camera
	class OrthographicCamera final
	{
	public:
		/// <summary>
		/// Constructor.
		/// Initializes the Orthographic/2D camera.
		/// </summary>
		/// <param name="left">Left screen coordinate.</param>
		/// <param name="right">Right screen coordinate.</param>
		/// <param name="bottom">Bottom screen coordinate.</param>
		/// <param name="top">Top screen coordinate.</param>
		/// <param name="near">Near distance.</param>
		/// <param name="far">Far distance.</param>
		constexpr OrthographicCamera(float left, float right, float bottom, float top, float near, float far);

		/// <summary>
		/// Set the projection matrix via screen coordinates.
		/// </summary>
		/// <param name="left">Left screen coordinate.</param>
		/// <param name="right">Right screen coordinate.</param>
		/// <param name="bottom">Bottom screen coordinate.</param>
		/// <param name="top">Top screen coordinate.</param>
		/// <param name="near">Near distance.</param>
		/// <param name="far">Far distance.</param>
		constexpr void SetProjection(float left, float right, float bottom, float top, float near, float far);

		/// <summary>
		/// Retrieve the current camera position.
		/// </summary>
		/// <returns>Current camera position.</returns>
		[[nodiscard]] constexpr const Math::Vec3& GetPosition() const noexcept;
		/// <summary>
		/// Set the camera position.
		/// </summary>
		/// <param name="position">New camera position.</param>
		constexpr void SetPosition(const Math::Vec3& position);

		/// <summary>
		/// Retrieve the current camera rotation.
		/// </summary>
		/// <returns>Current camera rotation.</returns>
		[[nodiscard]] constexpr const Math::Vec3& GetRotation() const noexcept;
		/// <summary>
		/// Set the camera rotation.
		/// </summary>
		/// <param name="rotation">New camera rotation.</param>
		constexpr void SetRotation(const Math::Vec3& rotation);

		/// <summary>
		/// Retrieve the projection matrix.
		/// </summary>
		/// <returns>Projection matrix.</returns>
		[[nodiscard]] constexpr const Math::Mat4& GetProjectionMatrix() const noexcept;
		/// <summary>
		/// Retrieve the view matrix.
		/// </summary>
		/// <returns>View matrix.</returns>
		[[nodiscard]] constexpr const Math::Mat4& GetViewMatrix() const noexcept;

	private:
		/// <summary>
		/// Recalculate the view matrix.
		/// This applies the camera position and rotation to the view matrix.
		/// </summary>
		constexpr void RecalculateViewMatrix();

		Math::Mat4 m_projectionMatrix;
		Math::Mat4 m_viewMatrix = Math::Mat4(1.0f);

		Math::Vec3 m_position = Math::Vec3(0.0f);
		Math::Vec3 m_rotation = Math::Vec3(0.0f);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::OrthographicCamera::OrthographicCamera(const float left, const float right, const float bottom,
                                                                 const float top, const float near, const float far)
	: m_projectionMatrix(Math::OrthographicReverseZ(left, right, bottom, top, near, far))
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCamera::SetProjection(const float left, const float right, const float bottom,
                                                                 const float top, const float near, const float far)
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
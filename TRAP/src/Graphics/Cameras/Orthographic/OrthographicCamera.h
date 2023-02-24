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
		OrthographicCamera(float left, float right, float bottom, float top, float near, float far);

		/// <summary>
		/// Set the projection matrix via screen coordinates.
		/// </summary>
		/// <param name="left">Left screen coordinate.</param>
		/// <param name="right">Right screen coordinate.</param>
		/// <param name="bottom">Bottom screen coordinate.</param>
		/// <param name="top">Top screen coordinate.</param>
		/// <param name="near">Near distance.</param>
		/// <param name="far">Far distance.</param>
		void SetProjection(float left, float right, float bottom, float top, float near, float far);

		/// <summary>
		/// Retrieve the current camera position.
		/// </summary>
		/// <returns>Current camera position.</returns>
		[[nodiscard]] const Math::Vec3& GetPosition() const noexcept;
		/// <summary>
		/// Set the camera position.
		/// </summary>
		/// <param name="position">New camera position.</param>
		void SetPosition(const Math::Vec3& position);

		/// <summary>
		/// Retrieve the current camera rotation.
		/// </summary>
		/// <returns>Current camera rotation.</returns>
		[[nodiscard]] const Math::Vec3& GetRotation() const noexcept;
		/// <summary>
		/// Set the camera rotation.
		/// </summary>
		/// <param name="rotation">New camera rotation.</param>
		void SetRotation(const Math::Vec3& rotation);

		/// <summary>
		/// Retrieve the projection matrix.
		/// </summary>
		/// <returns>Projection matrix.</returns>
		[[nodiscard]] const Math::Mat4& GetProjectionMatrix() const noexcept;
		/// <summary>
		/// Retrieve the view matrix.
		/// </summary>
		/// <returns>View matrix.</returns>
		[[nodiscard]] const Math::Mat4& GetViewMatrix() const noexcept;

	private:
		/// <summary>
		/// Recalculate the view matrix.
		/// This applies the camera position and rotation to the view matrix.
		/// </summary>
		void RecalculateViewMatrix();

		Math::Mat4 m_projectionMatrix;
		Math::Mat4 m_viewMatrix;

		Math::Vec3 m_position;
		Math::Vec3 m_rotation;
	};
}

#endif /*TRAP_ORTHOGRAPHICCAMERA_H*/
#ifndef TRAP_CAMERA_H
#define TRAP_CAMERA_H

#include "Maths/Math.h"

namespace TRAP::Graphics
{
	class Camera
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		Camera() noexcept = default;
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="projection">Projection matrix.</param>
		explicit Camera(const Math::Mat4& projection) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Camera() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		Camera(const Camera&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Camera(Camera&&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Camera& operator=(const Camera&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Camera& operator=(Camera&&) noexcept = default;

		/// <summary>
		/// Retrieve the projection matrix.
		/// </summary>
		/// <returns>Projection matrix.</returns>
		const Math::Mat4& GetProjectionMatrix() const noexcept;

	protected:
		//Projection matrix, identity by default.
		Math::Mat4 m_projection = Math::Mat4(1.0f);
	};
}

#endif /*TRAP_CAMERA_H*/
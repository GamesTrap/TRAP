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
		constexpr Camera() noexcept = default;
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="projection">Projection matrix.</param>
		constexpr explicit Camera(const Math::Mat4& projection) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr virtual ~Camera() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Camera(const Camera&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Camera(Camera&&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr Camera& operator=(const Camera&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr Camera& operator=(Camera&&) noexcept = default;

		/// <summary>
		/// Retrieve the projection matrix.
		/// </summary>
		/// <returns>Projection matrix.</returns>
		[[nodiscard]] constexpr Math::Mat4 GetProjectionMatrix() const noexcept;

	protected:
		//Projection matrix, identity by default.
		Math::Mat4 m_projection = Math::Mat4(1.0f);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::Camera::Camera(const TRAP::Math::Mat4& projection) noexcept
	: m_projection(projection)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Mat4 TRAP::Graphics::Camera::GetProjectionMatrix() const noexcept
{
	return m_projection;
}

#endif /*TRAP_CAMERA_H*/
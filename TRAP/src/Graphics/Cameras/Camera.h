#ifndef TRAP_CAMERA_H
#define TRAP_CAMERA_H

#include "Maths/Types.h"
#include "Maths/Mat4.h"

namespace TRAP::Graphics
{
	class Camera
	{
	public:
		/// @brief Constructor.
		constexpr Camera() noexcept = default;
		/// @brief Constructor.
		/// @param projection Projection matrix.
		constexpr explicit Camera(const Math::Mat4& projection) noexcept;
		/// @brief Destructor.
		constexpr virtual ~Camera() = default;
		/// @brief Copy constructor.
		constexpr Camera(const Camera&) noexcept = default;
		/// @brief Move constructor.
		constexpr Camera(Camera&&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr Camera& operator=(const Camera&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr Camera& operator=(Camera&&) noexcept = default;

		/// @brief Retrieve the projection matrix.
		/// @return Projection matrix.
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
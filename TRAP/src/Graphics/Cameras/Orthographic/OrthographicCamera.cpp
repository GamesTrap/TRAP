#include "TRAPPCH.h"
#include "OrthographicCamera.h"

TRAP::Graphics::OrthographicCamera::OrthographicCamera(const float left, const float right, const float bottom,
                                                       const float top, const float near, const float far)
	: m_projectionMatrix(Math::Orthographic(left, right, bottom, top, far, near)), m_viewMatrix(Math::Mat4(1.0f)),
	  m_position(0.0f), m_rotation(0.0f)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetProjection(const float left, const float right, const float bottom,
                                                       const float top, const float near, const float far)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	m_projectionMatrix = Math::Orthographic(left, right, bottom, top, far, near);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetPosition() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_position;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetPosition(const Math::Vec3& position)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	m_position = position;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetRotation() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_rotation;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetRotation(const Math::Vec3& rotation)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	m_rotation = rotation;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetProjectionMatrix() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_projectionMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetViewMatrix() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_viewMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::RecalculateViewMatrix()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const Math::Mat4 transform = Translate(m_position) * Mat4Cast(Math::Quat(Radians(m_rotation)));

	m_viewMatrix = Inverse(transform);
}
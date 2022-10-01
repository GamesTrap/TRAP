#include "TRAPPCH.h"
#include "OrthographicCamera.h"

TRAP::Graphics::OrthographicCamera::OrthographicCamera(const float left, const float right, const float bottom,
                                                       const float top, const float near, const float far)
	: m_projectionMatrix(Math::Orthographic(left, right, bottom, top, far, near)), m_viewMatrix(Math::Mat4(1.0f)),
	  m_position(0.0f), m_rotation(0.0f)
{
	ZoneScopedC(tracy::Color::Red);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetProjection(const float left, const float right, const float bottom,
                                                       const float top, const float near, const float far)
{
	ZoneScopedC(tracy::Color::Red);

	m_projectionMatrix = Math::Orthographic(left, right, bottom, top, far, near);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetPosition() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_position;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetPosition(const Math::Vec3& position)
{
	ZoneScopedC(tracy::Color::Red);

	m_position = position;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetRotation() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_rotation;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetRotation(const Math::Vec3& rotation)
{
	ZoneScopedC(tracy::Color::Red);

	m_rotation = rotation;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetProjectionMatrix() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_projectionMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetViewMatrix() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_viewMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::RecalculateViewMatrix()
{
	ZoneScopedC(tracy::Color::Red);

	const Math::Mat4 transform = Translate(m_position) * Mat4Cast(Math::Quat(Radians(m_rotation)));

	m_viewMatrix = Inverse(transform);
}
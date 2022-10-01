#include "TRAPPCH.h"
#include "OrthographicCamera.h"

TRAP::Graphics::OrthographicCamera::OrthographicCamera(const float left, const float right, const float bottom,
                                                       const float top, const float near, const float far)
	: m_projectionMatrix(Math::Orthographic(left, right, bottom, top, far, near)), m_viewMatrix(Math::Mat4(1.0f)),
	  m_position(0.0f), m_rotation(0.0f)
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetProjection(const float left, const float right, const float bottom,
                                                       const float top, const float near, const float far)
{
	ZoneScoped;

	m_projectionMatrix = Math::Orthographic(left, right, bottom, top, far, near);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetPosition() const
{
	ZoneScoped;

	return m_position;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetPosition(const Math::Vec3& position)
{
	ZoneScoped;

	m_position = position;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetRotation() const
{
	ZoneScoped;

	return m_rotation;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetRotation(const Math::Vec3& rotation)
{
	ZoneScoped;

	m_rotation = rotation;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetProjectionMatrix() const
{
	ZoneScoped;

	return m_projectionMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetViewMatrix() const
{
	ZoneScoped;

	return m_viewMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::RecalculateViewMatrix()
{
	ZoneScoped;

	const Math::Mat4 transform = Translate(m_position) * Mat4Cast(Math::Quat(Radians(m_rotation)));

	m_viewMatrix = Inverse(transform);
}
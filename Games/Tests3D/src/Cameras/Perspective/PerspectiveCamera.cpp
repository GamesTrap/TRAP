#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(const float fov,
	                                 const float left,
	                                 const float right,
	                                 const float bottom,
	                                 const float top,
	                                 const float near,
	                                 const float far)
	: m_projectionMatrix(TRAP::Math::PerspectiveFoV(TRAP::Math::Radians(fov), (right - left), (top - bottom), near, far)),
	  m_viewMatrix(TRAP::Math::Mat4(1.0f)),
	  m_position(0.0f, 0.0f, 0.0f),
	  m_rotation(0.0f),
	  m_fov(fov)
{
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

void PerspectiveCamera::SetProjection(const float fov,
                                      const float left,
                                      const float right,
                                      const float bottom,
                                      const float top,
                                      const float near,
                                      const float far)
{
	m_fov = fov;
	m_projectionMatrix = TRAP::Math::PerspectiveFoV(TRAP::Math::Radians(fov), (right - left), (top - bottom), near, far);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Vec3& PerspectiveCamera::GetPosition() const
{
	return m_position;
}

//-------------------------------------------------------------------------------------------------------------------//

void PerspectiveCamera::SetPosition(const TRAP::Math::Vec3& position)
{
	m_position = position;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Vec3& PerspectiveCamera::GetRotation() const
{
	return m_rotation;
}

//-------------------------------------------------------------------------------------------------------------------//

void PerspectiveCamera::SetRotation(const TRAP::Math::Vec3& rotation)
{
	m_rotation = rotation;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& PerspectiveCamera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& PerspectiveCamera::GetViewMatrix() const
{
	return m_viewMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

void PerspectiveCamera::RecalculateViewMatrix()
{
	const TRAP::Math::Vec3 rotVec{ m_rotation.x, -m_rotation.y, m_rotation.z };
	const TRAP::Math::Mat4 transform = TRAP::Math::Translate(m_position) * TRAP::Math::Mat4Cast(TRAP::Math::Quaternion(TRAP::Math::Radians(rotVec)));

	m_viewMatrix = TRAP::Math::Inverse(transform);
}

//-------------------------------------------------------------------------------------------------------------------//

float PerspectiveCamera::GetFoV() const
{
	return m_fov;
}
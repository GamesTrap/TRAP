#include "TRAPPCH.h"
#include "OrthographicCamera.h"

TRAP::Graphics::OrthographicCamera::OrthographicCamera(const float left,
                                                       const float right,
                                                       const float bottom,
                                                       const float top,
                                                       const float near,
                                                       const float far)
	: m_projectionMatrix(Math::Orthographic(left, right, bottom, top, near, far)),
	  m_viewMatrix(Math::Mat4(1.0f)),
	  m_position(0.0f),
	  m_rotation(0.0f)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetProjection(const float left,
                                                       const float right,
                                                       const float bottom,
                                                       const float top,
                                                       const float near,
                                                       const float far)
{
	TP_PROFILE_FUNCTION();
	
	m_projectionMatrix = Math::Orthographic(left, right, bottom, top, near, far);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetPosition() const
{
	return m_position;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetPosition(const Math::Vec3& position)
{
	TP_PROFILE_FUNCTION();

	m_position = position;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetRotation() const
{
	return m_rotation;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetRotation(const Math::Vec3& rotation)
{
	TP_PROFILE_FUNCTION();

	m_rotation = rotation;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetViewMatrix() const
{
	return m_viewMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::RecalculateViewMatrix()
{	
	const Math::Mat4 transform = Translate(m_position) *
		                         Rotate(Math::Radians(m_rotation.x), Math::XAxis<float>()) * 
		                         Rotate(Math::Radians(m_rotation.y), Math::YAxis<float>()) *
		                         Rotate(Math::Radians(m_rotation.z), Math::ZAxis<float>());

	m_viewMatrix = Inverse(transform);
}
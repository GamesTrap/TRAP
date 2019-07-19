#include "TRAPPCH.h"
#include "OrthographicCamera.h"

TRAP::Graphics::OrthographicCamera::OrthographicCamera(const float left,
                                                       const float right,
                                                       const float bottom,
                                                       const float top,
                                                       const float near,
                                                       const float far)
	: m_projectionMatrix(Maths::Mat4::Orthographic(left, right, bottom, top, near, far)), m_viewMatrix(Maths::Mat4::Identity()), m_position(0.0f), m_rotation(0.0f)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::RecalculateViewMatrix()
{
	const Maths::Mat4 transform = Maths::Mat4::Translate(m_position) *
		                          Maths::Mat4::Rotate(m_rotation.x, Maths::Vec3::XAxis()) * 
		                          Maths::Mat4::Rotate(m_rotation.y, Maths::Vec3::YAxis()) * 
		                          Maths::Mat4::Rotate(m_rotation.z, Maths::Vec3::ZAxis());

	m_viewMatrix = Maths::Mat4::Invert(transform);
}
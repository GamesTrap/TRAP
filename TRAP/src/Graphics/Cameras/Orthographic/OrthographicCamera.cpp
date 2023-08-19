#include "TRAPPCH.h"
#include "OrthographicCamera.h"

void TRAP::Graphics::OrthographicCamera::SetPosition(const Math::Vec3& position)
{
	m_position = position;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::SetRotation(const Math::Vec3& rotation)
{
	m_rotation = rotation;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCamera::RecalculateViewMatrix()
{
	const Math::Mat4 transform = Translate(m_position) * Mat4Cast(Math::Quat(Radians(m_rotation)));

	m_viewMatrix = Inverse(transform);
}

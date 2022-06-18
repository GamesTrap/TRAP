#include "TRAPPCH.h"
#include "Camera.h"

#include "Maths/Math.h"

const TRAP::Math::Mat4& TRAP::Graphics::Camera::GetProjectionMatrix() const
{
	return m_projection;
}

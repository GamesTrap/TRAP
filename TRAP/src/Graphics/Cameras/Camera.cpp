#include "TRAPPCH.h"
#include "Camera.h"

#include "Maths/Math.h"

TRAP::Graphics::Camera::Camera(const TRAP::Math::Mat4& projection)
	: m_projection(projection)
{
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& TRAP::Graphics::Camera::GetProjectionMatrix() const
{
	return m_projection;
}

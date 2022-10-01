#include "TRAPPCH.h"
#include "Camera.h"

#include "Maths/Math.h"

TRAP::Graphics::Camera::Camera(const TRAP::Math::Mat4& projection)
	: m_projection(projection)
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& TRAP::Graphics::Camera::GetProjectionMatrix() const
{
	ZoneScoped;

	return m_projection;
}

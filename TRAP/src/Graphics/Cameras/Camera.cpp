#include "TRAPPCH.h"
#include "Camera.h"

#include "Maths/Math.h"

TRAP::Graphics::Camera::Camera(const TRAP::Math::Mat4& projection) noexcept
	: m_projection(projection)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& TRAP::Graphics::Camera::GetProjectionMatrix() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_projection;
}

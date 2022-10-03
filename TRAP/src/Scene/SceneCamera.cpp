#include "TRAPPCH.h"
#include "SceneCamera.h"

TRAP::SceneCamera::SceneCamera()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetPerspective(const float verticalFOV, const float nearClip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_projectionType = ProjectionType::Perspective;
	m_perspectiveFOV = verticalFOV;
	m_perspectiveNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographic(const float size, const float nearClip, const float farClip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_projectionType = ProjectionType::Orthographic;
	m_orthographicSize = size;
	m_orthographicNear = nearClip;
	m_orthographicFar = farClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetViewportSize(const uint32_t width, const uint32_t height)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetPerspectiveVerticalFOV() const
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_perspectiveFOV;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetPerspectiveVerticalFOV(const float verticalFov)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_perspectiveFOV = verticalFov;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetPerspectiveNearClip() const
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_perspectiveNear;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetPerspectiveNearClip(const float nearClip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_perspectiveNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetOrthographicSize() const
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_orthographicSize;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicSize(const float size)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_orthographicSize = size;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::SceneCamera::GetOrthographicClip() const
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return { m_orthographicNear, m_orthographicFar };
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicClip(const Math::Vec2 clip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_orthographicNear = clip.x;
	m_orthographicFar = clip.y;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetOrthographicNearClip() const
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_orthographicNear;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicNearClip(const float nearClip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_orthographicNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetOrthographicFarClip() const
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_orthographicFar;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicFarClip(const float farClip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_orthographicFar = farClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::SceneCamera::ProjectionType TRAP::SceneCamera::GetProjectionType() const
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_projectionType;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetProjectionType(const ProjectionType type)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_projectionType = type;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::RecalculateProjection()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	if(m_projectionType == ProjectionType::Perspective)
		m_projection = Math::InfinitePerspective(m_perspectiveFOV, m_aspectRatio, m_perspectiveNear);
	else //if (m_projectionType == ProjectionType::Orthographic)
	{
		const float orthographicLeft = -m_orthographicSize * m_aspectRatio * 0.5f;
		const float orthographicRight = m_orthographicSize * m_aspectRatio * 0.5f;
		const float orthographicBottom = -m_orthographicSize * 0.5f;
		const float orthographicTop = m_orthographicSize * 0.5f;

		m_projection = Math::Orthographic(orthographicLeft,
			                              orthographicRight,
			                              orthographicBottom,
			                              orthographicTop,
			                              m_orthographicFar,
										  m_orthographicNear);
	}
}
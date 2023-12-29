#include "TRAPPCH.h"
#include "SceneCamera.h"

TRAP::SceneCamera::SceneCamera()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetPerspective(const f32 verticalFOV, const f32 nearClip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_projectionType = ProjectionType::Perspective;
	m_perspectiveFOV = verticalFOV;
	m_perspectiveNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographic(const f32 size, const f32 nearClip, const f32 farClip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_projectionType = ProjectionType::Orthographic;
	m_orthographicSize = size;
	m_orthographicNear = nearClip;
	m_orthographicFar = farClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetViewportSize(const u32 width, const u32 height)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_aspectRatio = NumericCast<f32>(width) / NumericCast<f32>(height);
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetPerspectiveVerticalFOV(const f32 verticalFov)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_perspectiveFOV = verticalFov;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetPerspectiveNearClip(const f32 nearClip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_perspectiveNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicSize(const f32 size)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_orthographicSize = size;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicClip(const Math::Vec2 clip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_orthographicNear = clip.x();
	m_orthographicFar = clip.y();
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicNearClip(const f32 nearClip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_orthographicNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicFarClip(const f32 farClip)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_orthographicFar = farClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetProjectionType(const ProjectionType type)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_projectionType = type;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::RecalculateProjection()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	if(m_projectionType == ProjectionType::Perspective)
		m_projection = Math::InfinitePerspective(m_perspectiveFOV, m_aspectRatio, m_perspectiveNear);
	else //if (m_projectionType == ProjectionType::Orthographic)
	{
		const f32 orthographicLeft = -m_orthographicSize * m_aspectRatio * 0.5f;
		const f32 orthographicRight = m_orthographicSize * m_aspectRatio * 0.5f;
		const f32 orthographicBottom = -m_orthographicSize * 0.5f;
		const f32 orthographicTop = m_orthographicSize * 0.5f;

		m_projection = Math::OrthographicReverseZ(orthographicLeft,
			                                      orthographicRight,
			                                      orthographicBottom,
			                                      orthographicTop,
										          m_orthographicNear,
												  m_orthographicFar);
	}
}
#include "TRAPPCH.h"
#include "SceneCamera.h"

TRAP::SceneCamera::SceneCamera()
{
	ZoneScoped;

	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetPerspective(const float verticalFOV, const float nearClip)
{
	ZoneScoped;

	m_projectionType = ProjectionType::Perspective;
	m_perspectiveFOV = verticalFOV;
	m_perspectiveNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographic(const float size, const float nearClip, const float farClip)
{
	ZoneScoped;

	m_projectionType = ProjectionType::Orthographic;
	m_orthographicSize = size;
	m_orthographicNear = nearClip;
	m_orthographicFar = farClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetViewportSize(const uint32_t width, const uint32_t height)
{
	ZoneScoped;

	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetPerspectiveVerticalFOV() const
{
	ZoneScoped;

	return m_perspectiveFOV;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetPerspectiveVerticalFOV(const float verticalFov)
{
	ZoneScoped;

	m_perspectiveFOV = verticalFov;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetPerspectiveNearClip() const
{
	ZoneScoped;

	return m_perspectiveNear;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetPerspectiveNearClip(const float nearClip)
{
	ZoneScoped;

	m_perspectiveNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetOrthographicSize() const
{
	ZoneScoped;

	return m_orthographicSize;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicSize(const float size)
{
	ZoneScoped;

	m_orthographicSize = size;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::SceneCamera::GetOrthographicClip() const
{
	ZoneScoped;

	return { m_orthographicNear, m_orthographicFar };
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicClip(const Math::Vec2 clip)
{
	ZoneScoped;

	m_orthographicNear = clip.x;
	m_orthographicFar = clip.y;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetOrthographicNearClip() const
{
	ZoneScoped;

	return m_orthographicNear;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicNearClip(const float nearClip)
{
	ZoneScoped;

	m_orthographicNear = nearClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetOrthographicFarClip() const
{
	ZoneScoped;

	return m_orthographicFar;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicFarClip(const float farClip)
{
	ZoneScoped;

	m_orthographicFar = farClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::SceneCamera::ProjectionType TRAP::SceneCamera::GetProjectionType() const
{
	ZoneScoped;

	return m_projectionType;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetProjectionType(const ProjectionType type)
{
	ZoneScoped;

	m_projectionType = type;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::RecalculateProjection()
{
	ZoneScoped;

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
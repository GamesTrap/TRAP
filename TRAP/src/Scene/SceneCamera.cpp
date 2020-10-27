#include "TRAPPCH.h"
#include "SceneCamera.h"

TRAP::SceneCamera::SceneCamera()
{
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographic(const float size, const float nearClip, const float farClip)
{
	m_orthographicSize = size;
	m_orthographicNear = nearClip;
	m_orthographicFar = farClip;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetViewportSize(const uint32_t width, const uint32_t height)
{
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::SceneCamera::GetOrthographicSize() const
{
	return m_orthographicSize;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::SetOrthographicSize(const float size)
{
	m_orthographicSize = size;
	RecalculateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneCamera::RecalculateProjection()
{
	const float orthographicLeft = -m_orthographicSize * m_aspectRatio * 0.5f;
	const float orthographicRight = m_orthographicSize * m_aspectRatio * 0.5f;
	const float orthographicBottom = -m_orthographicSize * 0.5f;
	const float orthographicTop = m_orthographicSize * 0.5f;

	m_projection = Math::Orthographic(orthographicLeft,
		orthographicRight,
		orthographicBottom,
		orthographicTop,
		m_orthographicNear,
		m_orthographicFar);
}
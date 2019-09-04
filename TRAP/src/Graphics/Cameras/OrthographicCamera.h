#ifndef _TRAP_ORTHOGRAPHICCAMERA_H_
#define _TRAP_ORTHOGRAPHICCAMERA_H_

#include "Maths/Maths.h"
#include "Camera.h"

namespace TRAP::Graphics
{
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float near, float far);

		const Math::Vec3& GetPosition() const override;
		void SetPosition(const Math::Vec3& position) override;

		const Math::Vec3& GetRotation() const override;
		void SetRotation(const Math::Vec3& rotation) override;

		const Math::Mat4& GetProjectionMatrix() const override;
		const Math::Mat4& GetViewMatrix() const override;

		void Translate(const Math::Vec3& translation) override;
		void Rotate(const Math::Vec3& rotation) override;

	private:
		void RecalculateViewMatrix();

		Math::Mat4 m_projectionMatrix;
		Math::Mat4 m_viewMatrix;

		Math::Vec3 m_position;
		Math::Vec3 m_rotation;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetPosition() const
{
	return m_position;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::OrthographicCamera::SetPosition(const Math::Vec3& position)
{
	m_position = position;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Math::Vec3& TRAP::Graphics::OrthographicCamera::GetRotation() const
{
	return m_rotation;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::OrthographicCamera::SetRotation(const Math::Vec3& rotation)
{
	m_rotation = rotation;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Math::Mat4& TRAP::Graphics::OrthographicCamera::GetViewMatrix() const
{
	return m_viewMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::OrthographicCamera::Translate(const Math::Vec3& translation)
{
	m_position += translation;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::OrthographicCamera::Rotate(const Math::Vec3& rotation)
{
	m_rotation += rotation;
}

#endif /*_TRAP_ORTHOGRAPHICCAMERA_H_*/
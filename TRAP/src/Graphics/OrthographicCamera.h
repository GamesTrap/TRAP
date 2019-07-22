#ifndef _TRAP_ORTHOGRAPHICCAMERA_H_
#define _TRAP_ORTHOGRAPHICCAMERA_H_

#include "Maths/Maths.h"

namespace TRAP::Graphics
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float near, float far);

		const Maths::Vec3& GetPosition() const;
		void SetPosition(const Maths::Vec3& position);

		const Maths::Vec3& GetRotation() const;
		void SetRotation(const Maths::Vec3& rotation);

		const Maths::Mat4& GetProjectionMatrix() const;
		const Maths::Mat4& GetViewMatrix() const;

		void Translate(const Maths::Vec3& translation);
		void Rotate(const Maths::Vec3& rotation);

	private:
		void RecalculateViewMatrix();

		Maths::Mat4 m_projectionMatrix;
		Maths::Mat4 m_viewMatrix;

		Maths::Vec3 m_position;
		Maths::Vec3 m_rotation;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Maths::Vec3& TRAP::Graphics::OrthographicCamera::GetPosition() const
{
	return m_position;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::OrthographicCamera::SetPosition(const Maths::Vec3& position)
{
	m_position = position;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Maths::Vec3& TRAP::Graphics::OrthographicCamera::GetRotation() const
{
	return m_rotation;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::OrthographicCamera::SetRotation(const Maths::Vec3& rotation)
{
	m_rotation = rotation;
	RecalculateViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Maths::Mat4& TRAP::Graphics::OrthographicCamera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Maths::Mat4& TRAP::Graphics::OrthographicCamera::GetViewMatrix() const
{
	return m_viewMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::OrthographicCamera::Translate(const Maths::Vec3& translation)
{
	m_position += translation;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::OrthographicCamera::Rotate(const Maths::Vec3& rotation)
{
	m_rotation += rotation;
}

#endif /*_TRAP_ORTHOGRAPHICCAMERA_H_*/
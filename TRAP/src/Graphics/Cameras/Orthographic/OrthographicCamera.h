#ifndef _TRAP_ORTHOGRAPHICCAMERA_H_
#define _TRAP_ORTHOGRAPHICCAMERA_H_

#include "Maths/Math.h"

namespace TRAP::Graphics
{
	class OrthographicCamera final
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float near, float far);
		~OrthographicCamera() = default;

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		OrthographicCamera(const OrthographicCamera&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		OrthographicCamera& operator=(const OrthographicCamera&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		OrthographicCamera(OrthographicCamera&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		OrthographicCamera& operator=(OrthographicCamera&&) = default;

		void SetProjection(float left, float right, float bottom, float top, float near, float far);

		const Math::Vec3& GetPosition() const;
		void SetPosition(const Math::Vec3& position);

		const Math::Vec3& GetRotation() const;
		void SetRotation(const Math::Vec3& rotation);

		const Math::Mat4& GetProjectionMatrix() const;
		const Math::Mat4& GetViewMatrix() const;

	private:
		void RecalculateViewMatrix();

		Math::Mat4 m_projectionMatrix;
		Math::Mat4 m_viewMatrix;

		Math::Vec3 m_position;
		Math::Vec3 m_rotation;
	};
}

#endif /*_TRAP_ORTHOGRAPHICCAMERA_H_*/
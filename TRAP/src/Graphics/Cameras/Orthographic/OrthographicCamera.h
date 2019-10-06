#ifndef _TRAP_ORTHOGRAPHICCAMERA_H_
#define _TRAP_ORTHOGRAPHICCAMERA_H_

#include "Maths/Maths.h"
#include "Graphics/Cameras/Camera.h"

namespace TRAP::Graphics
{
	class OrthographicCamera final : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float near, float far);
		void SetProjection(float left, float right, float bottom, float top, float near, float far);

		const Math::Vec3& GetPosition() const override;
		void SetPosition(const Math::Vec3& position) override;

		const Math::Vec3& GetRotation() const override;
		void SetRotation(const Math::Vec3& rotation) override;

		const Math::Mat4& GetProjectionMatrix() const override;
		const Math::Mat4& GetViewMatrix() const override;

	private:
		void RecalculateViewMatrix();

		Math::Mat4 m_projectionMatrix;
		Math::Mat4 m_viewMatrix;

		Math::Vec3 m_position;
		Math::Vec3 m_rotation;
	};
}

#endif /*_TRAP_ORTHOGRAPHICCAMERA_H_*/
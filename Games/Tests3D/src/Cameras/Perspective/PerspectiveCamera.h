#ifndef _GAMESTRAP_PERSPECTIVECAMERA_H_
#define _GAMESTRAP_PERSPECTIVECAMERA_H_

#include <TRAP.h>

class PerspectiveCamera : public TRAP::Graphics::Camera
{
public:
	PerspectiveCamera(float fov, float left, float right, float bottom, float top, float near, float far);
	void SetProjection(float fov, float left, float right, float bottom, float top, float near, float far);
	
	const TRAP::Math::Vec3& GetPosition() const override;
	void SetPosition(const TRAP::Math::Vec3& position) override;

	const TRAP::Math::Vec3& GetRotation() const override;
	void SetRotation(const TRAP::Math::Vec3& rotation) override;

	const TRAP::Math::Mat4& GetProjectionMatrix() const override;
	const TRAP::Math::Mat4& GetViewMatrix() const override;

	float GetFoV() const;
	
private:
	void RecalculateViewMatrix();
	
	TRAP::Math::Mat4 m_projectionMatrix;
	TRAP::Math::Mat4 m_viewMatrix;

	TRAP::Math::Vec3 m_position;
	TRAP::Math::Vec3 m_rotation;

	float m_fov;
};

#endif /*_GAMESTRAP_PERSPECTIVECAMERA_H_*/
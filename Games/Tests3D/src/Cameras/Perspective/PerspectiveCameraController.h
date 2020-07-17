#ifndef _GAMESTRAP_PERSPECTIVECAMERACONTROLLER_H_
#define _GAMESTRAP_PERSPECTIVECAMERACONTROLLER_H_

#include <TRAP.h>

#include "PerspectiveCamera.h"

class PerspectiveCameraController
{
public:
	explicit PerspectiveCameraController(float aspectRatio, 
										 float fov = 45.0f,		                                 
										 float sensitivity = 0.10f,
	                                     const std::unique_ptr<TRAP::Window>& window = TRAP::Application::GetWindow());
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime);
	void OnEvent(TRAP::Events::Event& e);

	void OnResize(float width, float height);

	PerspectiveCamera& GetCamera();
	const PerspectiveCamera& GetCamera() const;

	float GetTranslationSpeed() const;
	void SetTranslationSpeed(float translationSpeed);

	float GetMouseSensitivity() const;
	void SetMouseSensitivity(float sensitivity);

	float GetFoV() const;
	void SetFoV(float fov);

	const TRAP::Graphics::OrthographicCameraBounds& GetBounds() const;
	
private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& e);
	bool OnMouseMove(TRAP::Events::MouseMoveEvent& e);
	bool OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& e);

	float m_aspectRatio;
	float m_fov = 45.0f;
	float m_sensitivity;

	bool m_firstMouse;

	TRAP::Graphics::OrthographicCameraBounds m_bounds;
	PerspectiveCamera m_camera;

	const TRAP::Scope<TRAP::Window>& m_window;

	TRAP::Math::Vec3 m_cameraPosition;
	TRAP::Math::Vec3 m_cameraRotation;
	float m_cameraTranslationSpeed = 1.0f;

	bool m_toggleMouseMovement;
};

#endif /*_GAMESTRAP_PERSPECTIVECAMERACONTROLLER_H_*/
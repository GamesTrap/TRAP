#ifndef TRAP_ORTHOGRAPHICCAMERACONTROLLER_H
#define TRAP_ORTHOGRAPHICCAMERACONTROLLER_H

#include "OrthographicCamera.h"
#include "Utils/Time/TimeStep.h"
#include "Events/MouseEvent.h"
#include "Application.h"

namespace TRAP::Graphics
{
	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() const;
		float GetHeight() const;
	};

class OrthographicCameraController
	{
	public:
		explicit OrthographicCameraController(float aspectRatio, bool rotation = false, bool useController = false,
		                                      Input::Controller controller = Input::Controller::One,
											  const std::unique_ptr<Window>& window = Application::GetWindow());
		~OrthographicCameraController() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		OrthographicCameraController(const OrthographicCameraController&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		OrthographicCameraController& operator=(const OrthographicCameraController&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		OrthographicCameraController(OrthographicCameraController&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		OrthographicCameraController& operator=(OrthographicCameraController&&) = delete;

		void OnUpdate(const Utils::TimeStep& deltaTime);
		void OnEvent(Events::Event& e);

		void OnResize(float width, float height);

		OrthographicCamera& GetCamera();
		const OrthographicCamera& GetCamera() const;

		float GetTranslationSpeed() const;
		void SetTranslationSpeed(float translationSpeed);

		float GetRotationSpeed() const;
		void SetRotationSpeed(float rotationSpeed);

		float GetZoomLevel() const;
		void SetZoomLevel(float zoomLevel);

		const OrthographicCameraBounds& GetBounds() const;

	private:
		bool OnMouseScroll(Events::MouseScrollEvent& e);
		bool OnFrameBufferResize(Events::FrameBufferResizeEvent& e);

		float m_aspectRatio;
		float m_zoomLevel = 1.0f;

		OrthographicCameraBounds m_bounds;
		OrthographicCamera m_camera;

		bool m_rotation;
		bool m_useController;
		Input::Controller m_controller;
		const Scope<Window>& m_window;

		Math::Vec3 m_cameraPosition{ 0.0f };
		Math::Vec3 m_cameraRotation{ 0.0f };
		float m_cameraTranslationSpeed = 1.0f, m_cameraRotationSpeed = 180.0f;
	};
}

#endif /*TRAP_ORTHOGRAPHICCAMERACONTROLLER_H*/
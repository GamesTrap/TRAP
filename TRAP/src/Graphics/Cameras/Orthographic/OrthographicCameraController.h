#ifndef TRAP_ORTHOGRAPHICCAMERACONTROLLER_H
#define TRAP_ORTHOGRAPHICCAMERACONTROLLER_H

#ifndef TRAP_HEADLESS_MODE

#include "OrthographicCamera.h"
#include "Utils/Time/TimeStep.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

namespace TRAP::Graphics
{
	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		/// <summary>
		/// Retrieve the width of the camera bounds.
		/// </summary>
		/// <returns>Width of the camera bounds.</returns>
		[[nodiscard]] float GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the height of the camera bounds.
		/// </summary>
		/// <returns>Height of the camera bounds.</returns>
		[[nodiscard]] float GetHeight() const noexcept;
	};

	class OrthographicCameraController
	{
	public:
		/// <summary>
		/// Constructor.
		/// Initializes the Orthographic/2D camera and its controls.
		/// </summary>
		/// <param name="aspectRatio">Aspect ratio for the camera.</param>
		/// <param name="rotation">Enabled or disable rotation controls.</param>
		/// <param name="useController">Use the mouse and keyboard to control the camera or a controller.</param>
		/// <param name="controller">Which controller slot to use when useController is set to true.</param>
		explicit OrthographicCameraController(float aspectRatio, bool rotation = false, bool useController = false,
		                                      Input::Controller controller = Input::Controller::One) noexcept; //TODO Could be split into 2 constructors, one containing controller.
		/// <summary>
		/// Destructor.
		/// </summary>
		~OrthographicCameraController() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		OrthographicCameraController(const OrthographicCameraController&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		OrthographicCameraController& operator=(const OrthographicCameraController&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		OrthographicCameraController(OrthographicCameraController&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		OrthographicCameraController& operator=(OrthographicCameraController&&) = delete;

		/// <summary>
		/// Update the camera.
		///
		/// 1. Update controller or mouse/keyboard state depending on useController variable.
		/// 2. Update camera position and optionally rotation.
		/// </summary>
		/// <param name="deltaTime">Time since last frame.</param>
		void OnUpdate(const Utils::TimeStep& deltaTime);

		/// <summary>
		/// Receive and handle mouse scroll and framebuffer resize events.
		/// </summary>
		/// <param name="e">Event to handle.</param>
		void OnEvent(Events::Event& e);

		/// <summary>
		/// Resize the camera.
		///
		/// Recalculates the aspect ratio, the camera bounds and the projection matrix.
		/// </summary>
		/// <param name="width">New width of the camera.</param>
		/// <param name="height">New height of the camera.</param>
		void OnResize(float width, float height);

		/// <summary>
		/// Retrieve the Orthographic/2D camera.
		/// </summary>
		/// <returns>Orthographic/2D camera.</returns>
		[[nodiscard]] OrthographicCamera& GetCamera() noexcept;
		/// <summary>
		/// Retrieve the Orthographic/2D camera.
		/// </summary>
		/// <returns>Orthographic/2D camera.</returns>
		[[nodiscard]] const OrthographicCamera& GetCamera() const noexcept;

		/// <summary>
		/// Retrieve the currently used translation/movement speed.
		/// </summary>
		/// <returns>Translation/movement speed.</returns>
		[[nodiscard]] float GetTranslationSpeed() const noexcept;
		/// <summary>
		/// Set the translation/movement speed.
		/// </summary>
		/// <param name="translationSpeed">New translation/movement speed.</param>
		void SetTranslationSpeed(float translationSpeed) noexcept;

		/// <summary>
		/// Retrieve the currently used rotation speed.
		/// </summary>
		/// <returns>Rotation speed.</returns>
		[[nodiscard]] float GetRotationSpeed() const noexcept;
		/// <summary>
		/// Set the rotation speed.
		/// </summary>
		/// <param name="rotationSpeed">New rotation speed.</param>
		void SetRotationSpeed(float rotationSpeed) noexcept;

		/// <summary>
		/// Retrieve the current zoom level.
		/// </summary>
		/// <returns>Zoom level.</returns>
		[[nodiscard]] float GetZoomLevel() const noexcept;
		/// <summary>
		/// Set the zoom level.
		/// </summary>
		/// <param name="zoomLevel">New zoom level.</param>
		void SetZoomLevel(float zoomLevel);

		/// <summary>
		/// Retrieve the current camera bounds.
		/// </summary>
		/// <returns>Camera bounds.</returns>
		[[nodiscard]] const OrthographicCameraBounds& GetBounds() const noexcept;

	private:
		/// <summary>
		/// Event handler for mouse scroll event.
		///
		/// Updates zoom level, translation speed, bounds and recalculates the projection matrix.
		/// </sumary>
		/// <param name="e">Mouse scroll event.</param>
		/// <returns>False.</returns>
		bool OnMouseScroll(const Events::MouseScrollEvent& e);
		/// <summary>
		/// Event handler for framebuffer resize event.
		///
		/// Calls OrthographicCameraController::OnResize.
		/// </sumary>
		/// <param name="e">Framebuffer resize event.</param>
		/// <returns>False.</returns>
		bool OnFrameBufferResize(const Events::FrameBufferResizeEvent& e);

		float m_aspectRatio;
		float m_zoomLevel = 1.0f;

		OrthographicCameraBounds m_bounds;
		OrthographicCamera m_camera;

		bool m_rotation;
		bool m_useController;
		Input::Controller m_controller;

		Math::Vec3 m_cameraPosition{ 0.0f };
		Math::Vec3 m_cameraRotation{ 0.0f };
		float m_cameraTranslationSpeed = 1.0f, m_cameraRotationSpeed = 180.0f;
	};
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_ORTHOGRAPHICCAMERACONTROLLER_H*/
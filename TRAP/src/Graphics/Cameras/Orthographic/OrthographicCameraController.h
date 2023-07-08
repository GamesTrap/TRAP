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
		[[nodiscard]] constexpr float GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the height of the camera bounds.
		/// </summary>
		/// <returns>Height of the camera bounds.</returns>
		[[nodiscard]] constexpr float GetHeight() const noexcept;
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
		constexpr explicit OrthographicCameraController(float aspectRatio, bool rotation = false, bool useController = false,
		                                                Input::Controller controller = Input::Controller::One) noexcept; //TODO Could be split into 2 constructors, one containing controller.
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~OrthographicCameraController() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr OrthographicCameraController(const OrthographicCameraController&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr OrthographicCameraController& operator=(const OrthographicCameraController&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr OrthographicCameraController(OrthographicCameraController&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr OrthographicCameraController& operator=(OrthographicCameraController&&) = delete;

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
		constexpr void OnResize(float width, float height);

		/// <summary>
		/// Retrieve the Orthographic/2D camera.
		/// </summary>
		/// <returns>Orthographic/2D camera.</returns>
		[[nodiscard]] constexpr OrthographicCamera& GetCamera() noexcept;
		/// <summary>
		/// Retrieve the Orthographic/2D camera.
		/// </summary>
		/// <returns>Orthographic/2D camera.</returns>
		[[nodiscard]] constexpr const OrthographicCamera& GetCamera() const noexcept;

		/// <summary>
		/// Retrieve the currently used translation/movement speed.
		/// </summary>
		/// <returns>Translation/movement speed.</returns>
		[[nodiscard]] constexpr float GetTranslationSpeed() const noexcept;
		/// <summary>
		/// Set the translation/movement speed.
		/// </summary>
		/// <param name="translationSpeed">New translation/movement speed.</param>
		constexpr void SetTranslationSpeed(float translationSpeed) noexcept;

		/// <summary>
		/// Retrieve the currently used rotation speed.
		/// </summary>
		/// <returns>Rotation speed.</returns>
		[[nodiscard]] constexpr float GetRotationSpeed() const noexcept;
		/// <summary>
		/// Set the rotation speed.
		/// </summary>
		/// <param name="rotationSpeed">New rotation speed.</param>
		constexpr void SetRotationSpeed(float rotationSpeed) noexcept;

		/// <summary>
		/// Retrieve the current zoom level.
		/// </summary>
		/// <returns>Zoom level.</returns>
		[[nodiscard]] constexpr float GetZoomLevel() const noexcept;
		/// <summary>
		/// Set the zoom level.
		/// </summary>
		/// <param name="zoomLevel">New zoom level.</param>
		constexpr void SetZoomLevel(float zoomLevel);

		/// <summary>
		/// Retrieve the current camera bounds.
		/// </summary>
		/// <returns>Camera bounds.</returns>
		[[nodiscard]] constexpr const OrthographicCameraBounds& GetBounds() const noexcept;

	private:
		/// <summary>
		/// Event handler for mouse scroll event.
		///
		/// Updates zoom level, translation speed, bounds and recalculates the projection matrix.
		/// </sumary>
		/// <param name="e">Mouse scroll event.</param>
		/// <returns>False.</returns>
		constexpr bool OnMouseScroll(const Events::MouseScrollEvent& e);
		/// <summary>
		/// Event handler for framebuffer resize event.
		///
		/// Calls OrthographicCameraController::OnResize.
		/// </sumary>
		/// <param name="e">Framebuffer resize event.</param>
		/// <returns>False.</returns>
		constexpr bool OnFrameBufferResize(const Events::FrameBufferResizeEvent& e);

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

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::OrthographicCameraBounds::GetWidth() const noexcept
{
	return Right - Left;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::OrthographicCameraBounds::GetHeight() const noexcept
{
	return Top - Bottom;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::OrthographicCameraController::OrthographicCameraController(const float aspectRatio,
																		             const bool rotation,
																		             const bool useController,
																		             const Input::Controller controller) noexcept
	: m_aspectRatio(aspectRatio),
	  m_bounds({ -m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel }),
	  m_camera(m_bounds.Left, m_bounds.Right, m_bounds.Bottom, m_bounds.Top, 1.0f, -1.0f),
	  m_rotation(rotation),
	  m_useController(useController),
	  m_controller(controller)
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCameraController::OnResize(const float width, const float height)
{
	m_aspectRatio = width / height;
	m_bounds = { -m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel };
	m_camera.SetProjection(m_bounds.Left, m_bounds.Right, m_bounds.Bottom, m_bounds.Top, 1.0f, -1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::OrthographicCamera& TRAP::Graphics::OrthographicCameraController::GetCamera() noexcept
{
	return m_camera;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Graphics::OrthographicCamera& TRAP::Graphics::OrthographicCameraController::GetCamera() const noexcept
{
	return m_camera;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::OrthographicCameraController::GetTranslationSpeed() const noexcept
{
	return m_cameraTranslationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCameraController::SetTranslationSpeed(const float translationSpeed) noexcept
{
	m_cameraTranslationSpeed = translationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::OrthographicCameraController::GetRotationSpeed() const noexcept
{
	return m_cameraRotationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCameraController::SetRotationSpeed(const float rotationSpeed) noexcept
{
	m_cameraRotationSpeed = rotationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::OrthographicCameraController::GetZoomLevel() const noexcept
{
	return m_zoomLevel;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCameraController::SetZoomLevel(const float zoomLevel)
{
	m_zoomLevel = zoomLevel;
	m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
	m_cameraTranslationSpeed = m_zoomLevel;
	m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel,
	                       1.0f, -1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Graphics::OrthographicCameraBounds& TRAP::Graphics::OrthographicCameraController::GetBounds() const noexcept
{
	return m_bounds;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::Graphics::OrthographicCameraController::OnMouseScroll(const Events::MouseScrollEvent& e)
{
	m_zoomLevel -= e.GetYOffset() * 0.25f;
	m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
	m_cameraTranslationSpeed = m_zoomLevel;
	m_bounds = { -m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel };
	m_camera.SetProjection(m_bounds.Left, m_bounds.Right, m_bounds.Bottom, m_bounds.Top, 1.0f, -1.0f);

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool TRAP::Graphics::OrthographicCameraController::OnFrameBufferResize(const Events::FrameBufferResizeEvent& e)
{
	if (e.GetWidth() > 0 && e.GetHeight() > 0)
		OnResize(NumericCast<float>(e.GetWidth()), NumericCast<float>(e.GetHeight()));

	return false;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_ORTHOGRAPHICCAMERACONTROLLER_H*/
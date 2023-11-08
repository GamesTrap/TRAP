#ifndef TRAP_ORTHOGRAPHICCAMERACONTROLLER_H
#define TRAP_ORTHOGRAPHICCAMERACONTROLLER_H

#ifndef TRAP_HEADLESS_MODE

#include "OrthographicCamera.h"
#include "Utils/Time/TimeStep.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

namespace TRAP::Graphics
{
	/// @remark @headless This struct is not available in headless mode.
	struct OrthographicCameraBounds
	{
		f32 Left, Right;
		f32 Bottom, Top;

		/// @brief Retrieve the width of the camera bounds.
		/// @return Width of the camera bounds.
		[[nodiscard]] constexpr f32 GetWidth() const noexcept;
		/// @brief Retrieve the height of the camera bounds.
		/// @return Height of the camera bounds.
		[[nodiscard]] constexpr f32 GetHeight() const noexcept;
	};

	/// @remark @headless This class is not available in headless mode.
	class OrthographicCameraController
	{
	public:
		/// @brief Constructor.
		/// Initializes the Orthographic/2D camera and its controls.
		/// @param aspectRatio Aspect ratio for the camera.
		/// @param rotation Enabled or disable rotation controls.
		/// @param useController Use the mouse and keyboard to control the camera or a controller.
		/// @param controller Which controller slot to use when useController is set to true.
		constexpr explicit OrthographicCameraController(f32 aspectRatio, bool rotation = false, bool useController = false,
		                                                Input::Controller controller = Input::Controller::One) noexcept; //TODO Could be split into 2 constructors, one containing controller.
		/// @brief Destructor.
		constexpr ~OrthographicCameraController() = default;

		/// @brief Copy constructor.
		constexpr OrthographicCameraController(const OrthographicCameraController&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr OrthographicCameraController& operator=(const OrthographicCameraController&) = default;
		/// @brief Move constructor.
		constexpr OrthographicCameraController(OrthographicCameraController&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr OrthographicCameraController& operator=(OrthographicCameraController&&) = default;

		/// @brief Update the camera.
		///
		/// 1. Update controller or mouse/keyboard state depending on useController variable.
		/// 2. Update camera position and optionally rotation.
		/// @param deltaTime Time since last frame.
		void OnUpdate(const Utils::TimeStep& deltaTime);

		/// @brief Receive and handle mouse scroll and framebuffer resize events.
		/// @param e Event to handle.
		void OnEvent(Events::Event& e);

		/// @brief Resize the camera.
		///
		/// Recalculates the aspect ratio, the camera bounds and the projection matrix.
		/// @param width New width of the camera.
		/// @param height New height of the camera.
		constexpr void OnResize(f32 width, f32 height);

		/// @brief Retrieve the Orthographic/2D camera.
		/// @return Orthographic/2D camera.
		[[nodiscard]] constexpr OrthographicCamera& GetCamera() noexcept;
		/// @brief Retrieve the Orthographic/2D camera.
		/// @return Orthographic/2D camera.
		[[nodiscard]] constexpr const OrthographicCamera& GetCamera() const noexcept;

		/// @brief Retrieve the currently used translation/movement speed.
		/// @return Translation/movement speed.
		[[nodiscard]] constexpr f32 GetTranslationSpeed() const noexcept;
		/// @brief Set the translation/movement speed.
		/// @param translationSpeed New translation/movement speed.
		constexpr void SetTranslationSpeed(f32 translationSpeed) noexcept;

		/// @brief Retrieve the currently used rotation speed.
		/// @return Rotation speed.
		[[nodiscard]] constexpr f32 GetRotationSpeed() const noexcept;
		/// @brief Set the rotation speed.
		/// @param rotationSpeed New rotation speed.
		constexpr void SetRotationSpeed(f32 rotationSpeed) noexcept;

		/// @brief Retrieve the current zoom level.
		/// @return Zoom level.
		[[nodiscard]] constexpr f32 GetZoomLevel() const noexcept;
		/// @brief Set the zoom level.
		/// @param zoomLevel New zoom level.
		constexpr void SetZoomLevel(f32 zoomLevel);

		/// @brief Retrieve the current camera bounds.
		/// @return Camera bounds.
		[[nodiscard]] constexpr const OrthographicCameraBounds& GetBounds() const noexcept;

	private:
		/// @brief Event handler for mouse scroll event.
		///
		/// Updates zoom level, translation speed, bounds and recalculates the projection matrix.
		/// @param e Mouse scroll event.
		/// @return False.
		constexpr bool OnMouseScroll(const Events::MouseScrollEvent& e);
		/// @brief Event handler for framebuffer resize event.
		///
		/// Calls OrthographicCameraController::OnResize.
		/// @param e Framebuffer resize event.
		/// @return False.
		constexpr bool OnFrameBufferResize(const Events::FrameBufferResizeEvent& e);

		f32 m_aspectRatio;
		f32 m_zoomLevel = 1.0f;

		OrthographicCameraBounds m_bounds;
		OrthographicCamera m_camera;

		bool m_rotation;
		bool m_useController;
		Input::Controller m_controller;

		Math::Vec3 m_cameraPosition{ 0.0f };
		Math::Vec3 m_cameraRotation{ 0.0f };
		f32 m_cameraTranslationSpeed = 1.0f, m_cameraRotationSpeed = 180.0f;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::OrthographicCameraBounds::GetWidth() const noexcept
{
	return Right - Left;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::OrthographicCameraBounds::GetHeight() const noexcept
{
	return Top - Bottom;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::OrthographicCameraController::OrthographicCameraController(const f32 aspectRatio,
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

constexpr void TRAP::Graphics::OrthographicCameraController::OnResize(const f32 width, const f32 height)
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

[[nodiscard]] constexpr f32 TRAP::Graphics::OrthographicCameraController::GetTranslationSpeed() const noexcept
{
	return m_cameraTranslationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCameraController::SetTranslationSpeed(const f32 translationSpeed) noexcept
{
	m_cameraTranslationSpeed = translationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::OrthographicCameraController::GetRotationSpeed() const noexcept
{
	return m_cameraRotationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCameraController::SetRotationSpeed(const f32 rotationSpeed) noexcept
{
	m_cameraRotationSpeed = rotationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::OrthographicCameraController::GetZoomLevel() const noexcept
{
	return m_zoomLevel;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::OrthographicCameraController::SetZoomLevel(const f32 zoomLevel)
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
		OnResize(NumericCast<f32>(e.GetWidth()), NumericCast<f32>(e.GetHeight()));

	return false;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_ORTHOGRAPHICCAMERACONTROLLER_H*/
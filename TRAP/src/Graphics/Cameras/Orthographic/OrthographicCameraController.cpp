#include "TRAPPCH.h"
#include "OrthographicCameraController.h"

#include "Input/Input.h"
#include "Events/WindowEvent.h"

float TRAP::Graphics::OrthographicCameraBounds::GetWidth() const
{
	return Right - Left;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::OrthographicCameraBounds::GetHeight() const
{
	return Top - Bottom;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::OrthographicCameraController::OrthographicCameraController(
	const float aspectRatio,
	const bool rotation,
	const bool useController,
	const Input::Controller controller,
	const Scope<Window>& window)
	: m_aspectRatio(aspectRatio),
	  m_bounds({ -m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel }),
	  m_camera(m_bounds.Left, m_bounds.Right, m_bounds.Bottom, m_bounds.Top, -1.0f, 1.0f),
	  m_rotation(rotation),
	  m_useController(useController),
	  m_controller(controller),
	  m_window(window)
{
	TRAP_CORE_ASSERT(m_window.get(), "[OrthographicCameraController] Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCameraController::OnUpdate(const Utils::TimeStep& deltaTime)
{
	TP_PROFILE_FUNCTION();
	
	if (m_useController)
	{
		if (Input::IsControllerConnected(m_controller)) //Controller
		{
			const float LeftXAxis = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Left_X);
			if (LeftXAxis < -0.1f || LeftXAxis > 0.1f)
				m_cameraPosition.x += LeftXAxis * m_cameraTranslationSpeed * deltaTime;
			const float LeftYAxis = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Left_Y);
			if (LeftYAxis < -0.1f || LeftYAxis > 0.1f)
				m_cameraPosition.y -= LeftYAxis * m_cameraTranslationSpeed * deltaTime;

			const float RightTrigger = (Input::GetControllerAxis(m_controller, Input::ControllerAxis::Right_Trigger) + 1) / 2;
			if (RightTrigger > 0.0f)
			{
				m_zoomLevel -= RightTrigger * 0.002f;
				m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
				m_cameraTranslationSpeed = m_zoomLevel;
				m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 1.0f);
			}
			const float LeftTrigger = (Input::GetControllerAxis(m_controller, Input::ControllerAxis::Left_Trigger) + 1) / 2;
			if (LeftTrigger > 0.0f)
			{
				m_zoomLevel += LeftTrigger * 0.002f;
				m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
				m_cameraTranslationSpeed = m_zoomLevel;
				m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 1.0f);
			}

			if (m_rotation)
			{
				const float RightXAxis = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Right_X);
				if (RightXAxis < -0.1f || RightXAxis > 0.1f)
					m_cameraRotation.y += RightXAxis * m_cameraRotationSpeed * deltaTime;
				const float RightYAxis = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Right_Y);
				if (RightYAxis < -0.1f || RightYAxis > 0.1f)
					m_cameraRotation.x -= RightYAxis * m_cameraRotationSpeed * deltaTime;
			}
		}
	}
	else
	{
		//Keyboard
		if (Input::IsKeyPressed(Input::Key::A))
			m_cameraPosition.x -= m_cameraTranslationSpeed * deltaTime;
		if (Input::IsKeyPressed(Input::Key::D))
			m_cameraPosition.x += m_cameraTranslationSpeed * deltaTime;
		if (Input::IsKeyPressed(Input::Key::W))
			m_cameraPosition.y += m_cameraTranslationSpeed * deltaTime;
		if (Input::IsKeyPressed(Input::Key::S))
			m_cameraPosition.y -= m_cameraTranslationSpeed * deltaTime;

		if (m_rotation)
		{
			if (Input::IsKeyPressed(Input::Key::KP_4))
				m_cameraRotation.z += m_cameraRotationSpeed * deltaTime;
			if (Input::IsKeyPressed(Input::Key::KP_6))
				m_cameraRotation.z -= m_cameraRotationSpeed * deltaTime;
			if (Input::IsKeyPressed(Input::Key::KP_8))
				m_cameraRotation.x += m_cameraRotationSpeed * deltaTime;
			if (Input::IsKeyPressed(Input::Key::KP_2))
				m_cameraRotation.x -= m_cameraRotationSpeed * deltaTime;
			if (Input::IsKeyPressed(Input::Key::KP_7))
				m_cameraRotation.y += m_cameraRotationSpeed * deltaTime;
			if (Input::IsKeyPressed(Input::Key::KP_9))
				m_cameraRotation.y -= m_cameraRotationSpeed * deltaTime;
		}
	}

	m_camera.SetPosition(m_cameraPosition);
	if (m_rotation)
		m_camera.SetRotation(m_cameraRotation);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCameraController::OnEvent(Events::Event& e)
{
	TP_PROFILE_FUNCTION();
	
	Events::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Events::MouseScrollEvent>([this](Events::MouseScrollEvent& e) {return OnMouseScroll(e); });
	dispatcher.Dispatch<Events::FrameBufferResizeEvent>([this](Events::FrameBufferResizeEvent& e) {return OnFrameBufferResize(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCameraController::OnResize(const float width, const float height)
{
	TP_PROFILE_FUNCTION();

	m_aspectRatio = width / height;
	m_bounds = { -m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel };
	m_camera.SetProjection(m_bounds.Left, m_bounds.Right, m_bounds.Bottom, m_bounds.Top, -1.0f, 1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::OrthographicCamera& TRAP::Graphics::OrthographicCameraController::GetCamera()
{
	return m_camera;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::OrthographicCamera& TRAP::Graphics::OrthographicCameraController::GetCamera() const
{
	return m_camera;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::OrthographicCameraController::GetTranslationSpeed() const
{
	return m_cameraTranslationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCameraController::SetTranslationSpeed(const float translationSpeed)
{
	m_cameraTranslationSpeed = translationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::OrthographicCameraController::GetRotationSpeed() const
{
	return m_cameraRotationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCameraController::SetRotationSpeed(const float rotationSpeed)
{
	m_cameraRotationSpeed = rotationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::OrthographicCameraController::GetZoomLevel() const
{
	return m_zoomLevel;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCameraController::SetZoomLevel(const float zoomLevel)
{
	TP_PROFILE_FUNCTION();

	m_zoomLevel = zoomLevel;
	m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
	m_cameraTranslationSpeed = m_zoomLevel;
	m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::OrthographicCameraBounds& TRAP::Graphics::OrthographicCameraController::GetBounds() const
{
	return m_bounds;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::OrthographicCameraController::OnMouseScroll(Events::MouseScrollEvent& e)
{
	TP_PROFILE_FUNCTION();
	
	if (m_window && m_window->GetTitle() == e.GetTitle())
	{
		if (!Input::IsControllerConnected(Input::Controller::One))
		{
			m_zoomLevel -= e.GetYOffset() * 0.25f;
			m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
			m_cameraTranslationSpeed = m_zoomLevel;
			m_bounds = { -m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel };
			m_camera.SetProjection(m_bounds.Left, m_bounds.Right, m_bounds.Bottom, m_bounds.Top, -1.0f, 1.0f);
		}
	}
	
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::OrthographicCameraController::OnFrameBufferResize(Events::FrameBufferResizeEvent& e)
{
	TP_PROFILE_FUNCTION();
	
	if (m_window && m_window->GetTitle() == e.GetTitle() && e.GetWidth() > 0 && e.GetHeight() > 0)
		OnResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
	
	return false;
}
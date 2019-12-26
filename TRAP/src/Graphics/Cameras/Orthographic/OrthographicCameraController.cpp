#include "TRAPPCH.h"
#include "OrthographicCameraController.h"

#include "Input/Input.h"

TRAP::Graphics::OrthographicCameraController::OrthographicCameraController(
	const float aspectRatio,
	const bool rotation,
	const bool useController,
	const Input::Controller controller,
	const std::unique_ptr<Window>& window)
	: m_aspectRatio(aspectRatio),
	  m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 1.0f),
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

			const float RightTrigger = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Right_Trigger);
			if (RightTrigger > 0.0f)
			{
				m_zoomLevel -= RightTrigger * 0.002f;
				m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
				m_cameraTranslationSpeed = m_zoomLevel;
				m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 1.0f);
			}
			const float LeftTrigger = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Left_Trigger);
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

void TRAP::Graphics::OrthographicCameraController::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseScrolledEvent>(TRAP_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
	dispatcher.Dispatch<WindowResizeEvent>(TRAP_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
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
	m_zoomLevel = zoomLevel;
	m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
	m_cameraTranslationSpeed = m_zoomLevel;
	m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
	if (m_window && m_window->GetTitle() == e.GetTitle())
	{
		if (!Input::IsControllerConnected(Input::Controller::One))
		{
			m_zoomLevel -= e.GetYOffset() * 0.25f;
			m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
			m_cameraTranslationSpeed = m_zoomLevel;
			m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 1.0f);
		}
	}
	
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
{
	if (m_window && m_window->GetTitle() == e.GetTitle())
	{
		m_aspectRatio = static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight());
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 1.0f);
	}
	
	return false;
}
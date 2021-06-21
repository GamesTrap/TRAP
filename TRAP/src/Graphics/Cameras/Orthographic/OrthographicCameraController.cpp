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

TRAP::Graphics::OrthographicCameraController::OrthographicCameraController(const float aspectRatio,
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
			const float leftXAxis = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Left_X);
			if (leftXAxis < -0.1f || leftXAxis > 0.1f) //Dead zone
			{
				m_cameraPosition.x += leftXAxis * Math::Cos(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
				m_cameraPosition.y += leftXAxis * Math::Sin(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
			}
			const float leftYAxis = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Left_Y);
			if (leftYAxis < -0.1f || leftYAxis > 0.1f) //Dead zone
			{
				m_cameraPosition.x -= leftYAxis * -Math::Sin(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
				m_cameraPosition.y -= leftYAxis * Math::Cos(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
			}

			const float rightTrigger = (Input::GetControllerAxis(m_controller, Input::ControllerAxis::Right_Trigger) + 1) / 2;
			if (rightTrigger > 0.0f)
			{
				m_zoomLevel -= rightTrigger * 0.002f;
				m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
				m_cameraTranslationSpeed = m_zoomLevel;
				m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 1.0f);
			}
			const float leftTrigger = (Input::GetControllerAxis(m_controller, Input::ControllerAxis::Left_Trigger) + 1) / 2;
			if (leftTrigger > 0.0f)
			{
				m_zoomLevel += leftTrigger * 0.002f;
				m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
				m_cameraTranslationSpeed = m_zoomLevel;
				m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 1.0f);
			}

			if (m_rotation)
			{
				const float rightXAxis = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Right_X);
				if (rightXAxis < -0.1f || rightXAxis > 0.1f) //Dead zone
					m_cameraRotation.z += rightXAxis * m_cameraRotationSpeed * deltaTime;
			}
		}
	}
	else
	{
		//Keyboard
		if (Input::IsKeyPressed(Input::Key::A))
		{
			m_cameraPosition.x -= Math::Cos(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
			m_cameraPosition.y -= Math::Sin(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
		}
		if (Input::IsKeyPressed(Input::Key::D))
		{
			m_cameraPosition.x += Math::Cos(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
			m_cameraPosition.y += Math::Sin(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
		}
		if (Input::IsKeyPressed(Input::Key::W))
		{
			m_cameraPosition.x += -Math::Sin(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
			m_cameraPosition.y += Math::Cos(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
		}
		if (Input::IsKeyPressed(Input::Key::S))
		{
			m_cameraPosition.x -= -Math::Sin(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
			m_cameraPosition.y -= Math::Cos(Math::Radians(m_cameraRotation.z)) * m_cameraTranslationSpeed * deltaTime;
		}

		if (m_rotation)
		{
			if (Input::IsKeyPressed(Input::Key::Comma))
				m_cameraRotation.z += m_cameraRotationSpeed * deltaTime;
			if (Input::IsKeyPressed(Input::Key::Period))
				m_cameraRotation.z -= m_cameraRotationSpeed * deltaTime;
		}
	}

	m_camera.SetPosition(m_cameraPosition);
	if (m_rotation)
	{
		if (m_cameraRotation.z > 180.0f)
			m_cameraRotation -= 360.0f;
		else if (m_cameraRotation.z <= -180.0f)
			m_cameraRotation.z += 360.0f;
		
		m_camera.SetRotation(m_cameraRotation);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCameraController::OnEvent(Events::Event& e)
{
	TP_PROFILE_FUNCTION();
	
	Events::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Events::MouseScrollEvent>([this](Events::MouseScrollEvent& event) {return OnMouseScroll(event); });
	dispatcher.Dispatch<Events::FrameBufferResizeEvent>([this](Events::FrameBufferResizeEvent& event) {return OnFrameBufferResize(event); });
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

	if (m_window && m_window.get() == e.GetWindow())
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

	if (m_window && m_window.get() == e.GetWindow() && e.GetWidth() > 0 && e.GetHeight() > 0)
		OnResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));

	return false;
}
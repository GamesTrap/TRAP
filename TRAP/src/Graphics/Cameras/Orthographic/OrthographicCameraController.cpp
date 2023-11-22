#include "TRAPPCH.h"
#include "OrthographicCameraController.h"

#ifndef TRAP_HEADLESS_MODE

#include "Input/Input.h"
#include "Events/WindowEvent.h"

void TRAP::Graphics::OrthographicCameraController::OnUpdate(const Utils::TimeStep& deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if (m_useController)
	{
		if (Input::IsControllerConnected(m_controller)) //Controller
		{
			const f32 leftXAxis = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Left_X);
			if (leftXAxis < -0.1f || leftXAxis > 0.1f) //Dead zone
			{
				m_cameraPosition.x() += leftXAxis * Math::Cos(Math::Radians(m_cameraRotation.z())) *
				                      m_cameraTranslationSpeed * deltaTime;
				m_cameraPosition.y() += leftXAxis * Math::Sin(Math::Radians(m_cameraRotation.z())) *
				                      m_cameraTranslationSpeed * deltaTime;
			}
			const f32 leftYAxis = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Left_Y);
			if (leftYAxis < -0.1f || leftYAxis > 0.1f) //Dead zone
			{
				m_cameraPosition.x() -= leftYAxis * -Math::Sin(Math::Radians(m_cameraRotation.z())) *
				                      m_cameraTranslationSpeed * deltaTime;
				m_cameraPosition.y() -= leftYAxis * Math::Cos(Math::Radians(m_cameraRotation.z())) *
				                      m_cameraTranslationSpeed * deltaTime;
			}

			const f32 rightTrigger = (Input::GetControllerAxis(m_controller, Input::ControllerAxis::Right_Trigger) +
			                            1) / 2;
			if (rightTrigger > 0.0f)
			{
				m_zoomLevel -= rightTrigger * 0.002f;
				m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
				m_cameraTranslationSpeed = m_zoomLevel;
				m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel,
				                       m_zoomLevel, 1.0f, -1.0f);
			}
			const f32 leftTrigger = (Input::GetControllerAxis(m_controller, Input::ControllerAxis::Left_Trigger) +
			                           1) / 2;
			if (leftTrigger > 0.0f)
			{
				m_zoomLevel += leftTrigger * 0.002f;
				m_zoomLevel = Math::Max(m_zoomLevel, 0.25f);
				m_cameraTranslationSpeed = m_zoomLevel;
				m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel,
				                       m_zoomLevel, 1.0f, -1.0f);
			}

			if (m_rotation)
			{
				const f32 rightXAxis = Input::GetControllerAxis(m_controller, Input::ControllerAxis::Right_X);
				if (rightXAxis < -0.1f || rightXAxis > 0.1f) //Dead zone
					m_cameraRotation.z() += rightXAxis * m_cameraRotationSpeed * deltaTime;
			}
		}
	}
	else
	{
		//Keyboard
		if (Input::IsKeyPressed(Input::Key::A))
		{
			m_cameraPosition.x() -= Math::Cos(Math::Radians(m_cameraRotation.z())) * m_cameraTranslationSpeed *
			                      deltaTime;
			m_cameraPosition.y() -= Math::Sin(Math::Radians(m_cameraRotation.z())) * m_cameraTranslationSpeed *
			                      deltaTime;
		}
		if (Input::IsKeyPressed(Input::Key::D))
		{
			m_cameraPosition.x() += Math::Cos(Math::Radians(m_cameraRotation.z())) * m_cameraTranslationSpeed *
			                      deltaTime;
			m_cameraPosition.y() += Math::Sin(Math::Radians(m_cameraRotation.z())) * m_cameraTranslationSpeed *
			                      deltaTime;
		}
		if (Input::IsKeyPressed(Input::Key::W))
		{
			m_cameraPosition.x() += -Math::Sin(Math::Radians(m_cameraRotation.z())) * m_cameraTranslationSpeed *
			                      deltaTime;
			m_cameraPosition.y() += Math::Cos(Math::Radians(m_cameraRotation.z())) * m_cameraTranslationSpeed *
			                      deltaTime;
		}
		if (Input::IsKeyPressed(Input::Key::S))
		{
			m_cameraPosition.x() -= -Math::Sin(Math::Radians(m_cameraRotation.z())) * m_cameraTranslationSpeed *
			                      deltaTime;
			m_cameraPosition.y() -= Math::Cos(Math::Radians(m_cameraRotation.z())) * m_cameraTranslationSpeed *
			                      deltaTime;
		}

		if (m_rotation)
		{
			if (Input::IsKeyPressed(Input::Key::Comma))
				m_cameraRotation.z() += m_cameraRotationSpeed * deltaTime;
			if (Input::IsKeyPressed(Input::Key::Period))
				m_cameraRotation.z() -= m_cameraRotationSpeed * deltaTime;
		}
	}

	m_camera.SetPosition(m_cameraPosition);
	if (m_rotation)
	{
		if (m_cameraRotation.z() > 180.0f)
			m_cameraRotation -= 360.0f;
		else if (m_cameraRotation.z() <= -180.0f)
			m_cameraRotation.z() += 360.0f;

		m_camera.SetRotation(m_cameraRotation);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::OrthographicCameraController::OnEvent(Events::Event& e)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	Events::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Events::MouseScrollEvent>(std::bind_front(&OrthographicCameraController::OnMouseScroll, this));
	dispatcher.Dispatch<Events::FrameBufferResizeEvent>(std::bind_front(&OrthographicCameraController::OnFrameBufferResize, this));
}

#endif /*TRAP_HEADLESS_MODE*/
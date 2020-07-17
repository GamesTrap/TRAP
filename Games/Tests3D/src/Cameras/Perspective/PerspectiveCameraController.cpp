#include "PerspectiveCameraController.h"

PerspectiveCameraController::PerspectiveCameraController(const float aspectRatio,
														 const float fov,
														 const float sensitivity,
                                                         const std::unique_ptr<TRAP::Window>& window)
	: m_aspectRatio(aspectRatio),
	m_fov(fov),
	m_sensitivity(sensitivity),
	m_firstMouse(true),
	m_bounds({ -m_aspectRatio, m_aspectRatio, -1.0f, 1.0}),
	m_camera(m_fov, m_bounds.Left, m_bounds.Right, m_bounds.Bottom, m_bounds.Top, 0.1f, 100.0f),
	m_window(window),
	m_cameraPosition(),
	m_cameraRotation(),
	m_toggleMouseMovement(false)
{
	TRAP_CORE_ASSERT(m_window.get(), "[PerspectiveCameraController] Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

void PerspectiveCameraController::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	const TRAP::Math::Vec3 rotVec{ m_cameraRotation.x, -m_cameraRotation.y, m_cameraRotation.z };
	const TRAP::Math::Quaternion orientation = TRAP::Math::Quaternion(TRAP::Math::Radians(rotVec));
	const TRAP::Math::Quaternion qF = orientation * TRAP::Math::Quaternion(0.0f, 0.0f, 0.0f, -1.0f) * TRAP::Math::Conjugate(orientation);
	const TRAP::Math::Vec3 front = { qF.x, qF.y, qF.z };
	const TRAP::Math::Vec3 right = TRAP::Math::Normalize(TRAP::Math::Cross(front, TRAP::Math::Vec3(0.0f, 1.0f, 0.0f)));
	
	//Keyboard Position
	if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::A))
		m_cameraPosition -= right * (m_cameraTranslationSpeed * deltaTime);
	if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::D))
		m_cameraPosition += right * (m_cameraTranslationSpeed * deltaTime);
	if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::W))
		m_cameraPosition += front * (m_cameraTranslationSpeed * deltaTime);
	if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::S))
		m_cameraPosition -= front * (m_cameraTranslationSpeed * deltaTime);
	if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Space))
		m_cameraPosition.y += m_cameraTranslationSpeed * deltaTime;
	if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Shift))
		m_cameraPosition.y -= m_cameraTranslationSpeed * deltaTime;
	
	m_camera.SetPosition(m_cameraPosition);
}

//-------------------------------------------------------------------------------------------------------------------//

void PerspectiveCameraController::OnEvent(TRAP::Events::Event& e)
{
	TRAP::Events::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>([this](TRAP::Events::FrameBufferResizeEvent& e) {return OnFrameBufferResize(e); });
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) {return OnKeyPress(e); });
	dispatcher.Dispatch<TRAP::Events::MouseMoveEvent>([this](TRAP::Events::MouseMoveEvent& e) {return OnMouseMove(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

void PerspectiveCameraController::OnResize(const float width, const float height)
{
	m_aspectRatio = width / height;
	m_bounds = { -m_aspectRatio, m_aspectRatio, -1.0f, 1.0f };
	m_camera.SetProjection(m_fov, m_bounds.Left, m_bounds.Right, m_bounds.Bottom, m_bounds.Top, 0.1f, 100.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

PerspectiveCamera& PerspectiveCameraController::GetCamera()
{
	return m_camera;
}

//-------------------------------------------------------------------------------------------------------------------//

const PerspectiveCamera& PerspectiveCameraController::GetCamera() const
{
	return m_camera;
}

//-------------------------------------------------------------------------------------------------------------------//

float PerspectiveCameraController::GetTranslationSpeed() const
{
	return m_cameraTranslationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

void PerspectiveCameraController::SetTranslationSpeed(const float translationSpeed)
{
	m_cameraTranslationSpeed = translationSpeed;
}

//-------------------------------------------------------------------------------------------------------------------//

float PerspectiveCameraController::GetMouseSensitivity() const
{
	return m_sensitivity;
}

//-------------------------------------------------------------------------------------------------------------------//

void PerspectiveCameraController::SetMouseSensitivity(const float sensitivity)
{
	m_sensitivity = sensitivity;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::OrthographicCameraBounds& PerspectiveCameraController::GetBounds() const
{
	return m_bounds;
}

//-------------------------------------------------------------------------------------------------------------------//

bool PerspectiveCameraController::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
	if(e.GetKey() == TRAP::Input::Key::Left_ALT && e.GetRepeatCount() == 0)
	{
		m_toggleMouseMovement = !m_toggleMouseMovement;
		
		if (m_toggleMouseMovement)
			TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Disabled);
		else
		{
			m_firstMouse = true;
			TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Normal);
		}
		
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool PerspectiveCameraController::OnMouseMove(TRAP::Events::MouseMoveEvent& e)
{
	if (m_toggleMouseMovement)
	{
		static float lastX = 0.0f;
		static float lastY = 0.0f;
		if (m_firstMouse)
		{
			lastX = e.GetX();
			lastY = e.GetY();
			m_firstMouse = false;
		}

		float pitch = e.GetX() - lastX;
		float yaw = lastY - e.GetY();
		lastX = e.GetX();
		lastY = e.GetY();

		pitch *= m_sensitivity;
		yaw *= m_sensitivity;

		m_cameraRotation.x += yaw;
		m_cameraRotation.y += pitch;

		//Limit pitch movement
		if (m_cameraRotation.x > 89.0f)
			m_cameraRotation.x = 90.0f;
		if (m_cameraRotation.x < -89.0f)
			m_cameraRotation.x = -90.0f;

		m_camera.SetRotation(m_cameraRotation);

		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool PerspectiveCameraController::OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& e)
{
	if (m_window && m_window->GetTitle() == e.GetTitle())
		OnResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

float PerspectiveCameraController::GetFoV() const
{
	return m_fov;
}

//-------------------------------------------------------------------------------------------------------------------//

void PerspectiveCameraController::SetFoV(const float fov)
{
	m_fov = fov;
	m_camera.SetProjection(m_fov, m_bounds.Left, m_bounds.Right, m_bounds.Bottom, m_bounds.Top, 0.1f, 100.0f);
}
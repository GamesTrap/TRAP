#include "Maths/Math.h"
#include "TRAPPCH.h"
#include "EditorCamera.h"

#include "Utils/Time/TimeStep.h"
#include "Events/Event.h"
#include "Events/MouseEvent.h"
#include "Layers/ImGui/ImGuiLayer.h"
#include "Application.h"

TRAP::Graphics::EditorCamera::EditorCamera(const float fov, const float aspectRatio, float nearClip)
    : Camera(TRAP::Math::InfinitePerspective(TRAP::Math::Radians(fov), aspectRatio, nearClip)),
      m_viewMatrix(), m_position(), m_direction(), m_focalPoint(), m_fov(fov), m_aspectRatio(aspectRatio),
      m_nearClip(nearClip), m_isActive(false), m_panning(), m_rotating(), m_initialMousePosition(),
      m_initialFocalPoint(), m_initialRotation(), m_distance(), m_normalSpeed(0.002f), m_pitch(),
      m_yaw(), m_pitchDelta(), m_yawDelta(), m_positionDelta(), m_rightDirection(),
      m_cameraMode(CameraMode::ArcBall), m_minFocusDistance(100.0f), m_viewportWidth(1280.0f),
      m_viewportHeight(720.0f)
{
	ZoneScoped;

    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::Init()
{
	ZoneScoped;

    constexpr TRAP::Math::Vec3 position{0.0f, 0.0f, -10.0f};
    m_distance = TRAP::Math::Distance(position, m_focalPoint);

    m_yaw = 0.0f;
    m_pitch = 0.0f;

    m_position = CalculatePosition();
    const TRAP::Math::Quat orientation = GetOrientation();
    m_direction = TRAP::Math::EulerAngles(orientation) * (180.0f / TRAP::Math::PI<float>());
    m_viewMatrix = TRAP::Math::Translate(TRAP::Math::Mat4(1.0f), m_position) * TRAP::Math::Mat4Cast(orientation);
    m_viewMatrix = TRAP::Math::Inverse(m_viewMatrix);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::Focus(const TRAP::Math::Vec3& focusPoint)
{
	ZoneScoped;

    m_focalPoint = focusPoint;
    m_cameraMode = CameraMode::FlyCam;
    if(m_distance > m_minFocusDistance)
    {
        m_distance -= m_distance - m_minFocusDistance;
        m_position = m_focalPoint - GetForwardDirection() * m_distance;
    }
    m_position = m_focalPoint - GetForwardDirection() * m_distance;
    UpdateView();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::OnUpdate(const Utils::TimeStep& deltaTime)
{
	ZoneScoped;

    const TRAP::Math::Vec2& mouse = TRAP::Input::GetMousePosition();
    const TRAP::Math::Vec2 delta = (mouse - m_initialMousePosition) * 0.002f;

    if(!m_isActive)
    {
        if(!ImGui::IsInputEnabled())
            ImGui::SetInputEnabled(true);
        return;
    }

    if(Input::IsMouseButtonPressed(Input::MouseButton::Right))
    {
        m_cameraMode = CameraMode::FlyCam;
        DisableMouse();

        const float yawSign = GetUpDirection().y < 0.0f ? -1.0f : 1.0f;
        const float speed = GetCameraSpeed();

        if(Input::IsKeyPressed(Input::Key::S))
            m_positionDelta -= deltaTime.GetMilliseconds() * speed * m_direction;
        if(Input::IsKeyPressed(Input::Key::W))
            m_positionDelta += deltaTime.GetMilliseconds() * speed * m_direction;
        if(Input::IsKeyPressed(Input::Key::A))
            m_positionDelta -= deltaTime.GetMilliseconds() * speed * m_rightDirection;
        if(Input::IsKeyPressed(Input::Key::D))
            m_positionDelta += deltaTime.GetMilliseconds() * speed * m_rightDirection;

        constexpr float maxRate = 0.12f;
        m_yawDelta += TRAP::Math::Clamp(yawSign * delta.x * RotationSpeed(), -maxRate, maxRate);
        m_pitchDelta += TRAP::Math::Clamp(delta.y * RotationSpeed(), -maxRate, maxRate);

        m_rightDirection = TRAP::Math::Cross(m_direction, TRAP::Math::Vec3(0.0f, yawSign, 0.0f));

        m_direction = TRAP::Math::Normalize(TRAP::Math::Cross(TRAP::Math::AngleAxis(-m_pitchDelta, m_rightDirection),
                                                             TRAP::Math::AngleAxis(-m_yawDelta, TRAP::Math::Vec3(0.0f, yawSign, 0.0f)))) * m_direction;

        const float distance = TRAP::Math::Distance(m_focalPoint, m_position);
        m_focalPoint = m_position + GetForwardDirection() * distance;
        m_distance = distance;
    }
    else
    {
        m_cameraMode = CameraMode::ArcBall;

        if ((Input::IsKeyPressed(TRAP::Input::Key::Left_Shift) ||
             Input::IsKeyPressed(TRAP::Input::Key::Right_Shift)) &&
            Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Middle))
        {
            DisableMouse();
            MousePan(delta);
        }
        else if((Input::IsKeyPressed(TRAP::Input::Key::Left_Control) ||
                 Input::IsKeyPressed(TRAP::Input::Key::Left_Control)) &&
                Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Middle))
        {
            DisableMouse();
            MouseZoom(delta.y);
        }
        else if(Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Middle))
        {
            DisableMouse();
            MouseRotate(delta);
        }
        else
            EnableMouse();
    }

    m_initialMousePosition = mouse;
    m_position += m_positionDelta;
    m_yaw += m_yawDelta;
    m_pitch += m_pitchDelta;

    if(m_cameraMode == CameraMode::ArcBall)
        m_position = CalculatePosition();

    UpdateView();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::OnEvent(Events::Event& event)
{
	ZoneScoped;

    Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Events::MouseScrollEvent>([this](Events::MouseScrollEvent& e){return EditorCamera::OnMouseScroll(e);});
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::EditorCamera::IsActive() const
{
	ZoneScoped;

    return m_isActive;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::SetActive(const bool active)
{
	ZoneScoped;

    m_isActive = active;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::EditorCamera::CameraMode TRAP::Graphics::EditorCamera::GetCurrentMode() const
{
	ZoneScoped;

    return m_cameraMode;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::EditorCamera::GetDistance() const
{
	ZoneScoped;

    return m_distance;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::SetDistance(const float distance)
{
	ZoneScoped;

    m_distance = distance;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Vec3& TRAP::Graphics::EditorCamera::GetFocalPoint() const
{
	ZoneScoped;

    return m_focalPoint;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::SetViewportSize(const float width, const float height)
{
	ZoneScoped;

    if(m_viewportWidth == width && m_viewportHeight == height)
        return;

    m_viewportWidth = width;
    m_viewportHeight = height;

    UpdateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Mat4& TRAP::Graphics::EditorCamera::GetViewMatrix() const
{
	ZoneScoped;

    return m_viewMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetUpDirection() const
{
	ZoneScoped;

    return GetOrientation() * TRAP::Math::Vec3(0.0f, 1.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetRightDirection() const
{
	ZoneScoped;

    return GetOrientation() * TRAP::Math::Vec3(1.0f, 0.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetForwardDirection() const
{
	ZoneScoped;

    return GetOrientation() * TRAP::Math::Vec3(0.0f, 0.0f, -1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Math::Vec3& TRAP::Graphics::EditorCamera::GetPosition() const
{
	ZoneScoped;

    return m_position;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Quat TRAP::Graphics::EditorCamera::GetOrientation() const
{
	ZoneScoped;

    return TRAP::Math::Quat(TRAP::Math::Vec3(-m_pitch - m_pitchDelta, -m_yaw - m_yawDelta, 0.0f));
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::EditorCamera::GetFOV() const
{
	ZoneScoped;

    return m_fov;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::EditorCamera::GetAspectRatio() const
{
	ZoneScoped;

    return m_aspectRatio;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::EditorCamera::GetNearClip() const
{
	ZoneScoped;

    return m_nearClip;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::EditorCamera::GetPitch() const
{
	ZoneScoped;

    return m_pitch;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::EditorCamera::GetYaw() const
{
	ZoneScoped;

    return m_yaw;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::EditorCamera::GetCameraSpeed() const
{
	ZoneScoped;

    float speed = m_normalSpeed;
    if(Input::IsKeyPressed(Input::Key::Left_Control) ||
       Input::IsKeyPressed(Input::Key::Right_Control))
    {
        speed /= 2 - TRAP::Math::Log(m_normalSpeed);
    }
    if(Input::IsKeyPressed(Input::Key::Left_Shift) ||
       Input::IsKeyPressed(Input::Key::Right_Shift))
    {
        speed *= 2 - TRAP::Math::Log(m_normalSpeed);
    }

    return TRAP::Math::Clamp(speed, MinSpeed, MaxSpeed);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::UpdateView()
{
	ZoneScoped;

    const float yawSign = GetUpDirection().y < 0.0f ? -1.0f : 1.0f;

    //Extra step to handle the problem when the camera direction is the same as the up vector
    const float cosAngle = TRAP::Math::Dot(GetForwardDirection(), GetUpDirection());
    if(cosAngle * yawSign > 0.99f)
        m_pitchDelta = 0.0f;

    const TRAP::Math::Vec3 lookAt = m_position + GetForwardDirection();
    m_direction = TRAP::Math::Normalize(lookAt - m_position);
    m_distance = TRAP::Math::Distance(m_position, m_focalPoint);
    m_viewMatrix = TRAP::Math::LookAt(m_position, lookAt, TRAP::Math::Vec3(0.0f, yawSign, 0.0f));

    m_yawDelta = 0.0f;
    m_pitchDelta = 0.0f;
    m_positionDelta = {0.0f, 0.0f, 0.0f};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::UpdateProjection()
{
	ZoneScoped;

    m_aspectRatio = m_viewportWidth / m_viewportHeight;
    m_projection = TRAP::Math::InfinitePerspective(TRAP::Math::Radians(m_fov), m_aspectRatio, m_nearClip);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::EditorCamera::OnMouseScroll(Events::MouseScrollEvent& event)
{
	ZoneScoped;

    if(Input::IsMouseButtonPressed(Input::MouseButton::Right))
    {
        m_normalSpeed += event.GetYOffset() * 0.3f * m_normalSpeed;
        m_normalSpeed = TRAP::Math::Clamp(m_normalSpeed, MinSpeed, MaxSpeed);
    }
    else
    {
        MouseZoom(event.GetYOffset() * 0.1f);
        UpdateView();
    }

    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::MousePan(const TRAP::Math::Vec2& delta)
{
	ZoneScoped;

    const TRAP::Math::Vec2 panSpeed = PanSpeed();
    m_focalPoint -= GetRightDirection() * delta.x * panSpeed.x * m_distance;
    m_focalPoint += GetUpDirection() * delta.y * panSpeed.y * m_distance;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::MouseRotate(const TRAP::Math::Vec2& delta)
{
	ZoneScoped;

    const float yawSign = GetUpDirection().y < 0.0f ? -1.0f : 1.0f;
    m_yawDelta += yawSign * delta.x * RotationSpeed();
    m_pitchDelta += delta.y * RotationSpeed();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::MouseZoom(const float delta)
{
	ZoneScoped;

    m_distance -= delta * ZoomSpeed();

    const TRAP::Math::Vec3 forwardDir = GetForwardDirection();
    m_position = m_focalPoint - forwardDir * m_distance;
    if(m_distance < 1.0f)
    {
        m_focalPoint += forwardDir * m_distance;
        m_distance = 1.0f;
    }
    m_positionDelta += delta * ZoomSpeed() * forwardDir;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::CalculatePosition() const
{
	ZoneScoped;

    return m_focalPoint - GetForwardDirection() * m_distance + m_positionDelta;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Graphics::EditorCamera::PanSpeed() const
{
	ZoneScoped;

    const float x = TRAP::Math::Min(m_viewportWidth / 1000.0f, 2.4f); //Max = 2.4f
    const float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    const float y = TRAP::Math::Min(m_viewportHeight / 1000.0f, 2.4f); //Max = 2.4f
    const float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return {xFactor, yFactor};
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::EditorCamera::ZoomSpeed() const
{
	ZoneScoped;

    const float distance = TRAP::Math::Max(m_distance * 0.3f, 0.0f);
    float speed = TRAP::Math::Min(distance * distance, 50.0f); //Max speed = 50.0f

    if (Input::IsKeyPressed(Input::Key::Left_Shift) ||
        Input::IsKeyPressed(Input::Key::Right_Shift) ||
        Input::IsKeyPressed(Input::Key::Left_Control) ||
        Input::IsKeyPressed(Input::Key::Right_Control) ||
        Input::IsKeyPressed(Input::Key::Left_ALT) ||
        Input::IsKeyPressed(Input::Key::Right_ALT))
    {
        speed *= (1.0f / 3.0f);
    }

    return speed;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::DisableMouse()
{
	ZoneScoped;

    ImGui::SetInputEnabled(false);
    if(TRAP::Application::GetWindow()->GetCursorMode() != TRAP::Window::CursorMode::Disabled)
        TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Disabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::EnableMouse()
{
	ZoneScoped;

    if(TRAP::Application::GetWindow()->GetCursorMode() != TRAP::Window::CursorMode::Normal)
        TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Normal);
    ImGui::SetInputEnabled(true);
}

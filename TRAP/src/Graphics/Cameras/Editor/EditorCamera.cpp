#include "TRAPPCH.h"
#include "EditorCamera.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/Time/TimeStep.h"
#include "Events/Event.h"
#include "Events/MouseEvent.h"
#include "Layers/ImGui/ImGuiLayer.h"
#include "Application.h"

TRAP::Graphics::EditorCamera::EditorCamera(const float fov, const float aspectRatio, float nearClip)
    : Camera(TRAP::Math::InfinitePerspective(TRAP::Math::Radians(fov), aspectRatio, nearClip)),
      m_position(), m_direction(), m_focalPoint(), m_fov(fov), m_aspectRatio(aspectRatio),
      m_nearClip(nearClip), m_isActive(false), m_panning(), m_rotating(), m_initialMousePosition(),
      m_initialFocalPoint(), m_initialRotation(), m_distance(), m_normalSpeed(0.002f), m_pitch(),
      m_yaw(), m_pitchDelta(), m_yawDelta(), m_positionDelta(), m_rightDirection(),
      m_cameraMode(CameraMode::ArcBall), m_minFocusDistance(100.0f), m_viewportWidth(1280.0f),
      m_viewportHeight(720.0f)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::Init()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    static constexpr TRAP::Math::Vec3 position{0.0f, 0.0f, -10.0f};
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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    const TRAP::Math::Vec2 mouse = TRAP::Input::GetMousePosition();
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

        static constexpr float maxRate = 0.12f;
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
                 Input::IsKeyPressed(TRAP::Input::Key::Right_Control)) &&
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
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Events::MouseScrollEvent>(this, &EditorCamera::OnMouseScroll);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::SetViewportSize(const float width, const float height)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    if(m_viewportWidth == width && m_viewportHeight == height)
        return;

    m_viewportWidth = width;
    m_viewportHeight = height;

    UpdateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetUpDirection() const
{
    return GetOrientation() * TRAP::Math::YAxis<float>();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetRightDirection() const
{
    return GetOrientation() * TRAP::Math::XAxis<float>();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetForwardDirection() const
{
    return GetOrientation() * -TRAP::Math::ZAxis<float>();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Quat TRAP::Graphics::EditorCamera::GetOrientation() const
{
    return TRAP::Math::Quat(TRAP::Math::Vec3(-m_pitch - m_pitchDelta, -m_yaw - m_yawDelta, 0.0f));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Graphics::EditorCamera::GetCameraSpeed() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    m_aspectRatio = m_viewportWidth / m_viewportHeight;
    m_projection = TRAP::Math::InfinitePerspective(TRAP::Math::Radians(m_fov), m_aspectRatio, m_nearClip);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::EditorCamera::OnMouseScroll(Events::MouseScrollEvent& event)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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
    const TRAP::Math::Vec2 panSpeed = PanSpeed();
    m_focalPoint -= GetRightDirection() * delta.x * panSpeed.x * m_distance;
    m_focalPoint += GetUpDirection() * delta.y * panSpeed.y * m_distance;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::MouseRotate(const TRAP::Math::Vec2& delta)
{
    const float yawSign = GetUpDirection().y < 0.0f ? -1.0f : 1.0f;
    m_yawDelta += yawSign * delta.x * RotationSpeed();
    m_pitchDelta += delta.y * RotationSpeed();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::MouseZoom(const float delta)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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

[[nodiscard]] TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::CalculatePosition() const
{
    return m_focalPoint - GetForwardDirection() * m_distance + m_positionDelta;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Graphics::EditorCamera::ZoomSpeed() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    ImGui::SetInputEnabled(false);
    if(TRAP::Application::GetWindow()->GetCursorMode() != TRAP::Window::CursorMode::Disabled)
        TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Disabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::EnableMouse()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    if(TRAP::Application::GetWindow()->GetCursorMode() != TRAP::Window::CursorMode::Normal)
        TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Normal);
    ImGui::SetInputEnabled(true);
}

#endif /*TRAP_HEADLESS_MODE*/
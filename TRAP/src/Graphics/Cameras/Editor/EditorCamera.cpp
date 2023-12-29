#include "TRAPPCH.h"
#include "EditorCamera.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/Time/TimeStep.h"
#include "Events/Event.h"
#include "Events/MouseEvent.h"
#include "Layers/ImGui/ImGuiLayer.h"
#include "Application.h"

void TRAP::Graphics::EditorCamera::OnUpdate(const Utils::TimeStep& deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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

        const f32 yawSign = GetUpDirection().y() < 0.0f ? -1.0f : 1.0f;
        const f32 speed = GetCameraSpeed();

        if(Input::IsKeyPressed(Input::Key::S))
            m_positionDelta -= deltaTime.GetMilliseconds() * speed * m_direction;
        if(Input::IsKeyPressed(Input::Key::W))
            m_positionDelta += deltaTime.GetMilliseconds() * speed * m_direction;
        if(Input::IsKeyPressed(Input::Key::A))
            m_positionDelta -= deltaTime.GetMilliseconds() * speed * m_rightDirection;
        if(Input::IsKeyPressed(Input::Key::D))
            m_positionDelta += deltaTime.GetMilliseconds() * speed * m_rightDirection;

        static constexpr f32 maxRate = 0.12f;
        m_yawDelta += TRAP::Math::Clamp(yawSign * delta.x() * RotationSpeed(), -maxRate, maxRate);
        m_pitchDelta += TRAP::Math::Clamp(delta.y() * RotationSpeed(), -maxRate, maxRate);

        m_rightDirection = TRAP::Math::Cross(m_direction, TRAP::Math::Vec3(0.0f, yawSign, 0.0f));

        m_direction = TRAP::Math::Normalize(TRAP::Math::Cross(TRAP::Math::AngleAxis(-m_pitchDelta, m_rightDirection),
                                                             TRAP::Math::AngleAxis(-m_yawDelta, TRAP::Math::Vec3(0.0f, yawSign, 0.0f)))) * m_direction;

        const f32 distance = TRAP::Math::Distance(m_focalPoint, m_position);
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
            MouseZoom(delta.y());
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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
                                           (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Events::MouseScrollEvent>(std::bind_front(&EditorCamera::OnMouseScroll, this));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Graphics::EditorCamera::GetCameraSpeed() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    f32 speed = m_normalSpeed;
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

bool TRAP::Graphics::EditorCamera::OnMouseScroll(const Events::MouseScrollEvent& event)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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

void TRAP::Graphics::EditorCamera::MouseZoom(const f32 delta)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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

[[nodiscard]] f32 TRAP::Graphics::EditorCamera::ZoomSpeed() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    const f32 distance = TRAP::Math::Max(m_distance * 0.3f, 0.0f);
    f32 speed = TRAP::Math::Min(distance * distance, 50.0f); //Max speed = 50.0f

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    ImGui::SetInputEnabled(false);
    if(TRAP::Application::GetWindow()->GetCursorMode() != TRAP::Window::CursorMode::Disabled)
        TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Disabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::EditorCamera::EnableMouse()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    if(TRAP::Application::GetWindow()->GetCursorMode() != TRAP::Window::CursorMode::Normal)
        TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Normal);
    ImGui::SetInputEnabled(true);
}

#endif /*TRAP_HEADLESS_MODE*/
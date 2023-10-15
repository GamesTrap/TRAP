#ifndef TRAP_EDITORCAMERA_H
#define TRAP_EDITORCAMERA_H

#ifndef TRAP_HEADLESS_MODE

#include "Graphics/Cameras/Camera.h"
#include "Maths/Math.h"

class TRAPEditorLayer;

namespace TRAP::Utils
{
    struct TimeStep;
}
namespace TRAP::Events
{
    class Event;
    class MouseScrollEvent;
}

namespace TRAP::Graphics
{
    class EditorCamera : public Camera
    {
    public:
        /// <summary>
        /// Camera modes.
        /// </summary>
        enum class CameraMode
        {
            None,
            FlyCam,
            ArcBall
        };

        /// <summary>
        /// Constructor.
        /// Create a new camera from the given parameters.
        /// </summary>
        /// <param name="fov">Field of view in degrees.</param>
        /// <param name="aspectRatio">Viewport aspect ratio.</param>
        /// <param name="nearClip">Near clip plane.</param>
        constexpr EditorCamera(float fov, float aspectRatio, float nearClip);
        /// <summary>
        /// Destructor
        /// </summary>
        constexpr ~EditorCamera() override = default;
        /// <summary>
        /// Copy constructor.
        /// </summary>
        constexpr EditorCamera(const EditorCamera&) noexcept = default;
        /// <summary>
        /// Move constructor.
        /// </summary>
        constexpr EditorCamera(EditorCamera&&) noexcept = default;

        /// <summary>
        /// Copy assignment operator.
        /// </summary>
        /// <returns>EditorCamera.</returns>
        constexpr EditorCamera& operator=(const EditorCamera&) noexcept = default;
        /// <summary>
        /// Move assignment operator.
        /// </summary>
        /// <returns>EditorCamera.</returns>
        constexpr EditorCamera& operator=(EditorCamera&&) noexcept = default;

        /// <summary>
        /// Initialize the camera.
        /// </summary>
        constexpr void Init();

        /// <summary>
        /// Focus the camera onto the given point.
        /// </summary>
        /// <param name="focusPoint">Point to focus on.</param>
        constexpr void Focus(const TRAP::Math::Vec3& focusPoint);
        /// <summary>
        /// Update the camera.
        /// </summary>
        /// <param name="deltaTime">Current frames delta time.</param>
        void OnUpdate(const Utils::TimeStep& deltaTime);
        /// <summary>
        /// Handle camera events.
        /// </summary>
        /// <param name="event">Triggered event.</param>
        void OnEvent(Events::Event& event);

        /// <summary>
        /// Retrieve whether the camera is active or not.
        /// </summary>
        /// <returns>True if camera is active, false otherwise.</returns>
        [[nodiscard]] constexpr bool IsActive() const noexcept;
        /// <summary>
        /// Set camera active or inactive
        /// </summary>
        /// <param name="active">Active or inactive.</param>
        constexpr void SetActive(bool active) noexcept;

        /// <summary>
        /// Retrieve the current mode of the camera.
        /// </summary>
        /// <returns>Current mode of the camera.</returns>
        [[nodiscard]] constexpr CameraMode GetCurrentMode() const noexcept;

        /// <summary>
        /// Retrieve the distance between the camera and the focal point.
        /// </summary>
        /// <returns>Distance.</returns>
        [[nodiscard]] constexpr float GetDistance() const noexcept;
        /// <summary>
        /// Set the distance between the camera and the focal point.
        /// </summary>
        /// <param name="distance">Distance to set.</param>
        constexpr void SetDistance(float distance) noexcept;

        /// <summary>
        /// Retrieve the current focal point of the camera.
        /// </summary>
        /// <returns>Current focal point.</returns>
        [[nodiscard]] constexpr const TRAP::Math::Vec3& GetFocalPoint() const noexcept;

        /// <summary>
        /// Set the viewport of the camera.
        /// This updates the projection matrix.
        /// </summary>
        /// <param name="width">Viewport width.</param>
        /// <param name="height">Viewport height.</param>
        constexpr void SetViewportSize(float width, float height);

        /// <summary>
        /// Retrieve the view matrix.
        /// </summary>
        /// <returns>View matrix.</returns>
        [[nodiscard]] constexpr const TRAP::Math::Mat4& GetViewMatrix() const noexcept;

        /// <summary>
        /// Retrieve the current up direction.
        /// </summary>
        /// <returns>Up direction.</returns>
        [[nodiscard]] constexpr TRAP::Math::Vec3 GetUpDirection() const;
        /// <summary>
        /// Retrieve the current right direction.
        /// </summary>
        /// <returns>Right direction.</returns>
        [[nodiscard]] constexpr TRAP::Math::Vec3 GetRightDirection() const;
        /// <summary>
        /// Retrieve the current forward direction.
        /// </summary>
        /// <returns>Forward direction.</returns>
        [[nodiscard]] constexpr TRAP::Math::Vec3 GetForwardDirection() const;

        /// <summary>
        /// Retrieve the current camera position.
        /// </summary>
        /// <returns>Camera position.</returns>
        [[nodiscard]] constexpr const TRAP::Math::Vec3& GetPosition() const noexcept;

        /// <summary>
        /// Retrieve the current camera orientation.
        /// </summary>
        /// <returns>Camera orientation.</returns>
        [[nodiscard]] constexpr TRAP::Math::Quat GetOrientation() const;

        /// <summary>
        /// Retrieve the current field of view in degrees.
        /// </summary>
        /// <returns>Field of view in degrees.</returns>
        [[nodiscard]] constexpr float GetFOV() const noexcept;
        /// <summary>
        /// Retrieve the current aspect ration.
        /// </summary>
        /// <returns>Aspect ratio.</returns>
        [[nodiscard]] constexpr float GetAspectRatio() const noexcept;
        /// <summary>
        /// Retrieve the nera clip plane.
        /// </summary>
        /// <returns>Near clip plane.</returns>
        [[nodiscard]] constexpr float GetNearClip() const noexcept;
        /// <summary>
        /// Retrieve the camera pitch.
        /// </summary>
        /// <returns>Pitch.</returns>
        [[nodiscard]] constexpr float GetPitch() const noexcept;
        /// <summary>
        /// Retrieve the camera yaw.
        /// </summary>
        /// <returns>Yaw.</returns>
        [[nodiscard]] constexpr float GetYaw() const noexcept;
        /// <summary>
        /// Retrieve the current camera speed.
        /// </summary>
        /// <returns>Camera speed.</returns>
        [[nodiscard]] float GetCameraSpeed() const;

    private:
        /// <summary>
        /// Update the view matrix.
        /// </summary>
        constexpr void UpdateView();
        /// <summary>
        /// Update the projection matrix.
        /// </summary>
        constexpr void UpdateProjection();

        /// <summary>
        /// Mouse scroll event callback.
        /// Used to zoom in and out and so set the camera speed.
        /// </summary>
        /// <param name="event">MouseScrollEvent.</param>
        /// <returns>False.</returns>
        bool OnMouseScroll(Events::MouseScrollEvent& event);

        /// <summary>
        /// Pan the camera via the given mouse delta.
        /// </summary>
        /// <param name="delta">Mouse delta.</param>
        constexpr void MousePan(const TRAP::Math::Vec2& delta);
        /// <summary>
        /// Rotate the camera via the given mouse pitch and yaw delta.
        /// </summary>
        /// <param name="delta">Mouse pitch and yaw delta.</param>
        constexpr void MouseRotate(const TRAP::Math::Vec2& delta);
        /// <summary>
        /// Zoom the camera with the given mouse delta.
        /// </summary>
        /// <param name="delta">Mouse delta.</param>
        void MouseZoom(float delta);

        /// <summary>
        /// Calculate the current camera position.
        /// </summary>
        /// <returns>Camera position.</returns>
        [[nodiscard]] constexpr TRAP::Math::Vec3 CalculatePosition() const;

        /// <summary>
        /// Retrieve the pan speed to use.
        /// </summary>
        /// <returns>Pan speed.</returns>
        [[nodiscard]] constexpr TRAP::Math::Vec2 PanSpeed() const;
        /// <summary>
        /// Retrieve the rotation speed to use.
        /// </summary>
        /// <returns>Rotation speed.</returns>
        [[nodiscard]] constexpr float RotationSpeed() const noexcept;
        /// <summary>
        /// Retrieve the zoom speed to use.
        /// </summary>
        /// <returns>Zoom speed.</returns>
        [[nodiscard]] float ZoomSpeed() const;

        /// <summary>
        /// Disable the mouse and ImGui mouse handling.
        /// Allows for infinite mouse movement without ImGui interaction.
        /// </summary>
        void DisableMouse();
        /// <summary>
        /// Enable the mouse and ImGui mouse handling.
        /// Disables the infinite mouse movement enabled by DisableMouse().
        /// </summary>
        void EnableMouse();

        TRAP::Math::Mat4 m_viewMatrix;
        TRAP::Math::Vec3 m_position{};
        TRAP::Math::Vec3 m_direction{};
        TRAP::Math::Vec3 m_focalPoint{};

        float m_fov;
        float m_aspectRatio;
        float m_nearClip;

        bool m_isActive = false;
        bool m_panning = false;
        bool m_rotating =  false;
        TRAP::Math::Vec2 m_initialMousePosition{};
        TRAP::Math::Vec3 m_initialFocalPoint{};
        TRAP::Math::Vec3 m_initialRotation{};

        float m_distance = 0.0f;
        float m_normalSpeed = 0.002f;

        float m_pitch = 0.0f;
        float m_yaw = 0.0f;
        float m_pitchDelta = 0.0f;
        float m_yawDelta = 0.0f;
        TRAP::Math::Vec3 m_positionDelta{};
        TRAP::Math::Vec3 m_rightDirection{};

        CameraMode m_cameraMode = CameraMode::ArcBall;

        float m_minFocusDistance = 100.0f;

        float m_viewportWidth = 1280.0f;
        float m_viewportHeight = 720.0f;

        constexpr static float MinSpeed = 0.002f;
        constexpr static float MaxSpeed = 2.5f;

        friend class ::TRAPEditorLayer;
    };
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::EditorCamera::EditorCamera(const float fov, const float aspectRatio, float nearClip)
    : Camera(TRAP::Math::InfinitePerspective(TRAP::Math::Radians(fov), aspectRatio, nearClip)),
      m_fov(fov), m_aspectRatio(aspectRatio), m_nearClip(nearClip)
{
    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::EditorCamera::Init()
{
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

constexpr void TRAP::Graphics::EditorCamera::Focus(const TRAP::Math::Vec3& focusPoint)
{
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

[[nodiscard]] constexpr bool TRAP::Graphics::EditorCamera::IsActive() const noexcept
{
    return m_isActive;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::EditorCamera::SetActive(const bool active) noexcept
{
    m_isActive = active;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::EditorCamera::CameraMode TRAP::Graphics::EditorCamera::GetCurrentMode() const noexcept
{
    return m_cameraMode;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::EditorCamera::GetDistance() const noexcept
{
    return m_distance;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::EditorCamera::SetDistance(const float distance) noexcept
{
    m_distance = distance;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Math::Vec3& TRAP::Graphics::EditorCamera::GetFocalPoint() const noexcept
{
    return m_focalPoint;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::EditorCamera::SetViewportSize(const float width, const float height)
{
    if(m_viewportWidth == width && m_viewportHeight == height)
        return;

    m_viewportWidth = width;
    m_viewportHeight = height;

    UpdateProjection();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Math::Mat4& TRAP::Graphics::EditorCamera::GetViewMatrix() const noexcept
{
    return m_viewMatrix;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetUpDirection() const
{
    return GetOrientation() * TRAP::Math::YAxis<float>();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetRightDirection() const
{
    return GetOrientation() * TRAP::Math::XAxis<float>();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetForwardDirection() const
{
    return GetOrientation() * -TRAP::Math::ZAxis<float>();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Math::Vec3& TRAP::Graphics::EditorCamera::GetPosition() const noexcept
{
    return m_position;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Quat TRAP::Graphics::EditorCamera::GetOrientation() const
{
    return TRAP::Math::Quat(TRAP::Math::Vec3(-m_pitch - m_pitchDelta, -m_yaw - m_yawDelta, 0.0f));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::EditorCamera::GetFOV() const noexcept
{
    return m_fov;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::EditorCamera::GetAspectRatio() const noexcept
{
    return m_aspectRatio;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::EditorCamera::GetNearClip() const noexcept
{
    return m_nearClip;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::EditorCamera::GetPitch() const noexcept
{
    return m_pitch;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::EditorCamera::GetYaw() const noexcept
{
    return m_yaw;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::EditorCamera::UpdateView()
{
    const float yawSign = GetUpDirection().y() < 0.0f ? -1.0f : 1.0f;

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

constexpr void TRAP::Graphics::EditorCamera::UpdateProjection()
{
    m_aspectRatio = m_viewportWidth / m_viewportHeight;
    m_projection = TRAP::Math::InfinitePerspective(TRAP::Math::Radians(m_fov), m_aspectRatio, m_nearClip);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::EditorCamera::MousePan(const TRAP::Math::Vec2& delta)
{
    const TRAP::Math::Vec2 panSpeed = PanSpeed();
    m_focalPoint -= GetRightDirection() * delta.x() * panSpeed.x() * m_distance;
    m_focalPoint += GetUpDirection() * delta.y() * panSpeed.y() * m_distance;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::EditorCamera::MouseRotate(const TRAP::Math::Vec2& delta)
{
    const float yawSign = GetUpDirection().y() < 0.0f ? -1.0f : 1.0f;
    m_yawDelta += yawSign * delta.x() * RotationSpeed();
    m_pitchDelta += delta.y() * RotationSpeed();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::CalculatePosition() const
{
    return m_focalPoint - GetForwardDirection() * m_distance + m_positionDelta;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2 TRAP::Graphics::EditorCamera::PanSpeed() const
{
    const float x = TRAP::Math::Min(m_viewportWidth / 1000.0f, 2.4f); //Max = 2.4f
    const float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    const float y = TRAP::Math::Min(m_viewportHeight / 1000.0f, 2.4f); //Max = 2.4f
    const float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return {xFactor, yFactor};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::EditorCamera::RotationSpeed() const noexcept
{
    return 0.3f;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_EDITORCAMERA_H*/
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
        constexpr EditorCamera(f32 fov, f32 aspectRatio, f32 nearClip);
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
        [[nodiscard]] constexpr f32 GetDistance() const noexcept;
        /// <summary>
        /// Set the distance between the camera and the focal point.
        /// </summary>
        /// <param name="distance">Distance to set.</param>
        constexpr void SetDistance(f32 distance) noexcept;

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
        constexpr void SetViewportSize(f32 width, f32 height);

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
        [[nodiscard]] constexpr f32 GetFOV() const noexcept;
        /// <summary>
        /// Retrieve the current aspect ration.
        /// </summary>
        /// <returns>Aspect ratio.</returns>
        [[nodiscard]] constexpr f32 GetAspectRatio() const noexcept;
        /// <summary>
        /// Retrieve the nera clip plane.
        /// </summary>
        /// <returns>Near clip plane.</returns>
        [[nodiscard]] constexpr f32 GetNearClip() const noexcept;
        /// <summary>
        /// Retrieve the camera pitch.
        /// </summary>
        /// <returns>Pitch.</returns>
        [[nodiscard]] constexpr f32 GetPitch() const noexcept;
        /// <summary>
        /// Retrieve the camera yaw.
        /// </summary>
        /// <returns>Yaw.</returns>
        [[nodiscard]] constexpr f32 GetYaw() const noexcept;
        /// <summary>
        /// Retrieve the current camera speed.
        /// </summary>
        /// <returns>Camera speed.</returns>
        [[nodiscard]] f32 GetCameraSpeed() const;

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
        void MouseZoom(f32 delta);

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
        [[nodiscard]] constexpr f32 RotationSpeed() const noexcept;
        /// <summary>
        /// Retrieve the zoom speed to use.
        /// </summary>
        /// <returns>Zoom speed.</returns>
        [[nodiscard]] f32 ZoomSpeed() const;

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

        f32 m_fov;
        f32 m_aspectRatio;
        f32 m_nearClip;

        bool m_isActive = false;
        bool m_panning = false;
        bool m_rotating =  false;
        TRAP::Math::Vec2 m_initialMousePosition{};
        TRAP::Math::Vec3 m_initialFocalPoint{};
        TRAP::Math::Vec3 m_initialRotation{};

        f32 m_distance = 0.0f;
        f32 m_normalSpeed = 0.002f;

        f32 m_pitch = 0.0f;
        f32 m_yaw = 0.0f;
        f32 m_pitchDelta = 0.0f;
        f32 m_yawDelta = 0.0f;
        TRAP::Math::Vec3 m_positionDelta{};
        TRAP::Math::Vec3 m_rightDirection{};

        CameraMode m_cameraMode = CameraMode::ArcBall;

        f32 m_minFocusDistance = 100.0f;

        f32 m_viewportWidth = 1280.0f;
        f32 m_viewportHeight = 720.0f;

        constexpr static f32 MinSpeed = 0.002f;
        constexpr static f32 MaxSpeed = 2.5f;

        friend class ::TRAPEditorLayer;
    };
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::EditorCamera::EditorCamera(const f32 fov, const f32 aspectRatio, f32 nearClip)
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
    m_direction = TRAP::Math::EulerAngles(orientation) * (180.0f / TRAP::Math::PI<f32>());
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

[[nodiscard]] constexpr f32 TRAP::Graphics::EditorCamera::GetDistance() const noexcept
{
    return m_distance;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::EditorCamera::SetDistance(const f32 distance) noexcept
{
    m_distance = distance;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Math::Vec3& TRAP::Graphics::EditorCamera::GetFocalPoint() const noexcept
{
    return m_focalPoint;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::EditorCamera::SetViewportSize(const f32 width, const f32 height)
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
    return GetOrientation() * TRAP::Math::YAxis<f32>();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetRightDirection() const
{
    return GetOrientation() * TRAP::Math::XAxis<f32>();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec3 TRAP::Graphics::EditorCamera::GetForwardDirection() const
{
    return GetOrientation() * -TRAP::Math::ZAxis<f32>();
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

[[nodiscard]] constexpr f32 TRAP::Graphics::EditorCamera::GetFOV() const noexcept
{
    return m_fov;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::EditorCamera::GetAspectRatio() const noexcept
{
    return m_aspectRatio;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::EditorCamera::GetNearClip() const noexcept
{
    return m_nearClip;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::EditorCamera::GetPitch() const noexcept
{
    return m_pitch;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::EditorCamera::GetYaw() const noexcept
{
    return m_yaw;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::EditorCamera::UpdateView()
{
    const f32 yawSign = GetUpDirection().y() < 0.0f ? -1.0f : 1.0f;

    //Extra step to handle the problem when the camera direction is the same as the up vector
    const f32 cosAngle = TRAP::Math::Dot(GetForwardDirection(), GetUpDirection());
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
    const f32 yawSign = GetUpDirection().y() < 0.0f ? -1.0f : 1.0f;
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
    const f32 x = TRAP::Math::Min(m_viewportWidth / 1000.0f, 2.4f); //Max = 2.4f
    const f32 xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    const f32 y = TRAP::Math::Min(m_viewportHeight / 1000.0f, 2.4f); //Max = 2.4f
    const f32 yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return {xFactor, yFactor};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::EditorCamera::RotationSpeed() const noexcept
{
    return 0.3f;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_EDITORCAMERA_H*/
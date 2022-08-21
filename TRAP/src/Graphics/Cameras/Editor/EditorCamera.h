#ifndef TRAP_EDITORCAMERA_H
#define TRAP_EDITORCAMERA_H

#include "Graphics/Cameras/Camera.h"

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

    public:
        /// <summary>
        /// Constructor.
        /// Create a new camera from the given parameters.
        /// </summary>
        /// <param name="fov">Field of view in degrees.</param>
        /// <param name="aspectRatio">Viewport aspect ratio.</param>
        /// <param name="nearClip">Near clip plane.</param>
        /// <param name="farClip">Far clip plane.</param>
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);
        /// <summary>
        /// Destructor
        /// </summary>
        ~EditorCamera() = default;
        /// <summary>
        /// Copy constructor.
        /// </summary>
        EditorCamera(const EditorCamera&) = default;
        /// <summary>
        /// Move constructor.
        /// </summary>
        EditorCamera(EditorCamera&&) = default;

        /// <summary>
        /// Copy assignment operator.
        /// </summary>
        /// <returns>EditorCamera.</returns>
        EditorCamera& operator=(const EditorCamera&) = default;
        /// <summary>
        /// Move assignment operator.
        /// </summary>
        /// <returns>EditorCamera.</returns>
        EditorCamera& operator=(EditorCamera&&) = default;

        /// <summary>
        /// Initialize the camera.
        /// </summary>
        void Init();

        /// <summary>
        /// Focus the camera onto the given point.
        /// </summary>
        /// <param name="focusPoint">Point to focus on.</param>
        void Focus(const TRAP::Math::Vec3& focusPoint);
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
        bool IsActive() const;
        /// <summary>
        /// Set camera active or inactive
        /// </summary>
        /// <param name="active">Active or inactive.</param>
        void SetActive(bool active);

        /// <summary>
        /// Retrieve the current mode of the camera.
        /// </summary>
        /// <returns>Current mode of the camera.</returns>
        CameraMode GetCurrentMode() const;

        /// <summary>
        /// Retrieve the distance between the camera and the focal point.
        /// </summary>
        /// <returns>Distance.</returns>
        float GetDistance() const;
        /// <summary>
        /// Set the distance between the camera and the focal point.
        /// </summary>
        /// <param name="distance">Distance to set.</param>
        void SetDistance(float distance);

        /// <summary>
        /// Retrieve the current focal point of the camera.
        /// </summary>
        /// <returns>Current focal point.</returns>
        const TRAP::Math::Vec3& GetFocalPoint() const;

        /// <summary>
        /// Set the viewport of the camera.
        /// This updates the projection matrix.
        /// </summary>
        /// <param name="width">Viewport width.</param>
        /// <param name="height">Viewport height.</param>
        void SetViewportSize(float width, float height);

        /// <summary>
        /// Retrieve the view matrix.
        /// </summary>
        /// <returns>View matrix.</returns>
        const TRAP::Math::Mat4& GetViewMatrix() const;

        /// <summary>
        /// Retrieve the current up direction.
        /// </summary>
        /// <returns>Up direction.</returns>
        TRAP::Math::Vec3 GetUpDirection() const;
        /// <summary>
        /// Retrieve the current right direction.
        /// </summary>
        /// <returns>Right direction.</returns>
        TRAP::Math::Vec3 GetRightDirection() const;
        /// <summary>
        /// Retrieve the current forward direction.
        /// </summary>
        /// <returns>Forward direction.</returns>
        TRAP::Math::Vec3 GetForwardDirection() const;

        /// <summary>
        /// Retrieve the current camera position.
        /// </summary>
        /// <returns>Camera position.</returns>
        const TRAP::Math::Vec3& GetPosition() const;

        /// <summary>
        /// Retrieve the current camera orientation.
        /// </summary>
        /// <returns>Camera orientation.</returns>
        TRAP::Math::Quat GetOrientation() const;

        /// <summary>
        /// Retrieve the current field of view in degrees.
        /// </summary>
        /// <returns>Field of view in degrees.</returns>
        float GetFOV() const;
        /// <summary>
        /// Retrieve the current aspect ration.
        /// </summary>
        /// <returns>Aspect ratio.</returns>
        float GetAspectRatio() const;
        /// <summary>
        /// Retrieve the nera clip plane.
        /// </summary>
        /// <returns>Near clip plane.</returns>
        float GetNearClip() const;
        /// <summary>
        /// Retrieve the far clip plane.
        /// </summary>
        /// <returns>Far clip plane.</returns>
        float GetFarClip() const;
        /// <summary>
        /// Retrieve the camera pitch.
        /// </summary>
        /// <returns>Pitch.</returns>
        float GetPitch() const;
        /// <summary>
        /// Retrieve the camera yaw.
        /// </summary>
        /// <returns>Yaw.</returns>
        float GetYaw() const;
        /// <summary>
        /// Retrieve the current camera speed.
        /// </summary>
        /// <returns>Camera speed.</returns>
        float GetCameraSpeed() const;

    private:
        /// <summary>
        /// Update the view matrix.
        /// </summary>
        void UpdateView();
        /// <summary>
        /// Update the projection matrix.
        /// </summary>
        void UpdateProjection();

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
        void MousePan(const TRAP::Math::Vec2& delta);
        /// <summary>
        /// Rotate the camera via the given mouse pitch and yaw delta.
        /// </summary>
        /// <param name="delta">Mouse pitch and yaw delta.</param>
        void MouseRotate(const TRAP::Math::Vec2& delta);
        /// <summary>
        /// Zoom the camera with the given mouse delta.
        /// </summary>
        /// <param name="delta">Mouse delta.</param>
        void MouseZoom(float delta);

        /// <summary>
        /// Calculate the current camera position.
        /// </summary>
        /// <returns>Camera position.</returns>
        TRAP::Math::Vec3 CalculatePosition() const;

        /// <summary>
        /// Retrieve the pan speed to use.
        /// </summary>
        /// <returns>Pan speed.</returns>
        TRAP::Math::Vec2 PanSpeed() const;
        /// <summary>
        /// Retrieve the rotation speed to use.
        /// </summary>
        /// <returns>Rotation speed.</returns>
        constexpr float RotationSpeed() const;
        /// <summary>
        /// Retrieve the zoom speed to use.
        /// </summary>
        /// <returns>Zoom speed.</returns>
        float ZoomSpeed() const;

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

    private:
        TRAP::Math::Mat4 m_viewMatrix;
        TRAP::Math::Vec3 m_position;
        TRAP::Math::Vec3 m_direction;
        TRAP::Math::Vec3 m_focalPoint;

        float m_fov;
        float m_aspectRatio;
        float m_nearClip;
        float m_farClip;

        bool m_isActive;
        bool m_panning;
        bool m_rotating;
        TRAP::Math::Vec2 m_initialMousePosition;
        TRAP::Math::Vec3 m_initialFocalPoint;
        TRAP::Math::Vec3 m_initialRotation;

        float m_distance;
        float m_normalSpeed;

        float m_pitch;
        float m_yaw;
        float m_pitchDelta;
        float m_yawDelta;
        TRAP::Math::Vec3 m_positionDelta;
        TRAP::Math::Vec3 m_rightDirection;

        CameraMode m_cameraMode;

        float m_minFocusDistance;

        float m_viewportWidth;
        float m_viewportHeight;

        constexpr static float MinSpeed = 0.002f;
        constexpr static float MaxSpeed = 2.5f;

        friend class ::TRAPEditorLayer;
    };
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr float TRAP::Graphics::EditorCamera::RotationSpeed() const
{
    return 0.3f;
}

#endif /*TRAP_EDITORCAMERA_H*/
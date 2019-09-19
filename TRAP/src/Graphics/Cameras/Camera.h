#ifndef _TRAP_CAMERA_H_
#define _TRAP_CAMERA_H_

namespace TRAP::Graphics
{
	class Camera
	{		
	public:
		Camera() = default;
		virtual ~Camera() = default;
		Camera(const Camera&) = default;
		Camera(Camera&&) = default;
		Camera& operator=(const Camera&) = default;
		Camera& operator=(Camera&&) = default;
		
		virtual const Math::Vec3& GetPosition() const = 0;
		virtual void SetPosition(const Math::Vec3& position) = 0;

		virtual const Math::Vec3& GetRotation() const = 0;
		virtual void SetRotation(const Math::Vec3& rotation) = 0;

		virtual const Math::Mat4& GetProjectionMatrix() const = 0;
		virtual const Math::Mat4& GetViewMatrix() const = 0;
	};
}

#endif /*_TRAP_CAMERA_H_*/
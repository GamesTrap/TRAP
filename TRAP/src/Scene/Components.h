#ifndef TRAP_COMPONENTS_H
#define TRAP_COMPONENTS_H

#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include "Maths/Math.h"

namespace TRAP
{
	/// <summary>
	/// Tag component.
	/// Every entity has a tag component containing the name for the entity.
	/// This component may not be added/remove to/from an entity.
	/// </summary>
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		explicit TagComponent(std::string tag)
			: Tag(std::move(tag))
		{}
	};

	/// <summary>
	/// Transform component.
	/// Every entity has a transform component containing position, rotation and scale.
	/// </summary>
	struct TransformComponent
	{
		Math::Vec3 Position{ 0.0f, 0.0f, 0.0f };
		Math::Vec3 Rotation{ 0.0f, 0.0f, 0.0f };
		Math::Vec3 Scale{ 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		explicit TransformComponent(const TRAP::Math::Vec3& position)
			: Position(position)
		{}
		TransformComponent(const TRAP::Math::Vec3& position, const TRAP::Math::Vec3& rotationInRadians)
			: Position(position), Rotation(rotationInRadians)
		{}
		TransformComponent(const TRAP::Math::Vec3& position, const TRAP::Math::Vec3& rotationInRadians,
		                   const TRAP::Math::Vec3& scale)
			: Position(position), Rotation(rotationInRadians), Scale(scale)
		{}

		/// <summary>
		/// Retrieve the transform calculated from current position, rotation and scale.
		/// </summary>
		/// <returns>Transform as Math::Mat4.</returns>
		Math::Mat4 GetTransform() const
		{
			if(Rotation.x != 0.0f || Rotation.y != 0.0f || Rotation.z != 0.0f)
			{
				const Math::Mat4 rotation = Mat4Cast(Math::Quaternion(Rotation));
				return Math::Translate(Position) * rotation * Math::Scale(Scale);
			}

			return Math::Translate(Position) * Math::Scale(Scale);
		}
	};

	/// <summary>
	/// Sprite renderer component.
	/// Optional component containing sprite renderer data.
	/// </summary>
	struct SpriteRendererComponent
	{
		Math::Vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		explicit SpriteRendererComponent(const TRAP::Math::Vec4& color)
			: Color(color)
		{}
	};

	/// <summary>
	/// Camera component.
	/// Optional component containing camera data.
	/// </summar>
	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; //TODO Maybe move to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
	};

	//TODO Document
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};
}

#endif /*TRAP_COMPONENTS_H*/
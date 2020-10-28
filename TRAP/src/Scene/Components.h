#ifndef _TRAP_COMPONENTS_H_
#define _TRAP_COMPONENTS_H_

#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include "Maths/Math.h"

namespace TRAP
{
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		~TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(TagComponent&&) = default;
		TagComponent& operator=(const TagComponent&) = default;
		TagComponent& operator=(TagComponent&&) = default;
		explicit TagComponent(std::string tag)
			: Tag(std::move(tag))
		{}
	};
	
	struct TransformComponent
	{
		Math::Vec3 Position{ 0.0f, 0.0f, 0.0f };
		Math::Vec3 Rotation{ 0.0f, 0.0f, 0.0f };
		Math::Vec3 Scale{ 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		~TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(TransformComponent&&) = default;
		TransformComponent& operator=(const TransformComponent&) = default;
		TransformComponent& operator=(TransformComponent&&) = default;
		explicit TransformComponent(const TRAP::Math::Vec3& position)
			: Position(position)
		{}

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

	struct SpriteRendererComponent
	{
		Math::Vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		~SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(SpriteRendererComponent&&) = default;
		SpriteRendererComponent& operator=(const SpriteRendererComponent&) = default;
		SpriteRendererComponent& operator=(SpriteRendererComponent&&) = default;
		explicit SpriteRendererComponent(const TRAP::Math::Vec4& color)
			: Color(color)
		{}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; //TODO Maybe move to Scene
		bool FixedAspectRatio = false;
		
		CameraComponent() = default;
		~CameraComponent() = default;
		CameraComponent(const CameraComponent&) = delete;
		CameraComponent(CameraComponent&&) = default;
		CameraComponent& operator=(const CameraComponent&) = delete;
		CameraComponent& operator=(CameraComponent&&) = default;
	};

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

#endif /*_TRAP_COMPONENTS_H_*/
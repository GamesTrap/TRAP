#ifndef _TRAP_COMPONENTS_H_
#define _TRAP_COMPONENTS_H_

#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include "Maths/Math.h"
#include "Utils/Time/TimeStep.h"

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
		Math::Mat4 Transform{ 1.0f };

		TransformComponent() = default;
		~TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(TransformComponent&&) = default;
		TransformComponent& operator=(const TransformComponent&) = default;
		TransformComponent& operator=(TransformComponent&&) = default;
		explicit TransformComponent(const TRAP::Math::Mat4& transform)
			: Transform(transform)
		{}

		operator TRAP::Math::Mat4&()
		{
			return Transform;
		}

		operator const TRAP::Math::Mat4&() const
		{
			return Transform;
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

		std::function<void()> InstantiateFunction;
		std::function<void()> DestroyInstanceFunction;
		
		std::function<void(ScriptableEntity*)> OnCreateFunction;
		std::function<void(ScriptableEntity*)> OnDestroyFunction;
		std::function<void(ScriptableEntity*, Utils::TimeStep)> OnUpdateFunction;
		std::function<void(ScriptableEntity*)> OnTickFunction;
		
		template<typename T>
		void Bind()
		{
			InstantiateFunction = [&]() { Instance = new T(); };
			DestroyInstanceFunction = [&]() {delete static_cast<T*>(Instance); Instance = nullptr; };
			
			OnCreateFunction = [](ScriptableEntity* instance) { static_cast<T*>(instance)->OnCreate(); };
			OnDestroyFunction = [](ScriptableEntity* instance) { static_cast<T*>(instance)->OnDestroy(); };
			OnUpdateFunction = [](ScriptableEntity* instance, Utils::TimeStep deltaTime) { static_cast<T*>(instance)->OnUpdate(deltaTime); };
			OnTickFunction = [](ScriptableEntity* instance) { static_cast<T*>(instance)->OnTick(); };
		}
	};
}

#endif /*_TRAP_COMPONENTS_H_*/
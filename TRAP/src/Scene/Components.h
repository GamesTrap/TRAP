#ifndef TRAP_COMPONENTS_H
#define TRAP_COMPONENTS_H

#include "SceneCamera.h"
#include "Maths/Math.h"
#include "Utils/Hash/UID.h"

namespace TRAP
{
	/// <summary>
	/// Unique identifier component.
	/// Every entity has a global unique identifier.
	/// This component may not be added/removed to/from an entity.
	/// </summary>
	struct UIDComponent
	{
		TRAP::Utils::UID UID;

		UIDComponent() = default;
	};

	/// <summary>
	/// Tag component.
	/// Every entity has a tag component containing the name for the entity.
	/// This component may not be added/removed to/from an entity.
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
				const Math::Mat4 rotation = Mat4Cast(Math::Quat(Rotation));
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
	/// Circle renderer component.
	/// Optional component containing circle renderer data.
	/// </summary>
	struct CircleRendererComponent
	{
		Math::Vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
	};

	/// <summary>
	/// Camera component.
	/// Optional component containing camera data.
	/// </summary>
	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; //TODO Maybe move to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
	};

	//TODO Document
	class ScriptableEntity;
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

	//-------------------------------------------------------------------------------------------------------------------//
	//Physics------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	struct Rigidbody2DComponent
	{
		enum class BodyType {Static, Dynamic, Kinematic};
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		//Storage for runtime data
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
	};

	struct BoxCollider2DComponent
	{
		TRAP::Math::Vec2 Offset{0.0f, 0.0f};
		TRAP::Math::Vec2 Size{0.5f, 0.5f};

		//TODO Move into Physics Material in future
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		//Storage for runtime data
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
	};

	template<typename... Component>
	struct ComponentGroup{};

	using AllComponents = ComponentGroup<TransformComponent, SpriteRendererComponent,
	                                     CircleRendererComponent, CameraComponent,
										 NativeScriptComponent, Rigidbody2DComponent,
										 BoxCollider2DComponent>;
}

#endif /*TRAP_COMPONENTS_H*/
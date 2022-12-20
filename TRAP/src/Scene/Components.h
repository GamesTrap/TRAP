#ifndef TRAP_COMPONENTS_H
#define TRAP_COMPONENTS_H

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#include "Scene/ScriptableEntity.h"
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

		TagComponent() noexcept = default;
		explicit TagComponent(std::string tag) noexcept
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

		TransformComponent() noexcept = default;
		explicit TransformComponent(const TRAP::Math::Vec3& position) noexcept
			: Position(position)
		{}
		TransformComponent(const TRAP::Math::Vec3& position, const TRAP::Math::Vec3& rotationInRadians) noexcept
			: Position(position), Rotation(rotationInRadians)
		{}
		TransformComponent(const TRAP::Math::Vec3& position, const TRAP::Math::Vec3& rotationInRadians,
		                   const TRAP::Math::Vec3& scale) noexcept
			: Position(position), Rotation(rotationInRadians), Scale(scale)
		{}

		/// <summary>
		/// Retrieve the transform calculated from current position, rotation and scale.
		/// </summary>
		/// <returns>Transform as Math::Mat4.</returns>
		Math::Mat4 GetTransform() const noexcept
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

		SpriteRendererComponent() noexcept = default;
		explicit SpriteRendererComponent(const TRAP::Math::Vec4& color) noexcept
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

		CircleRendererComponent() noexcept = default;
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
		using PFN_InstantiateScript = ScriptableEntity*(*)();
		using PFN_DestroyScript = void(*)(NativeScriptComponent*);

		ScriptableEntity* Instance = nullptr;

		PFN_InstantiateScript InstantiateScript;
		PFN_DestroyScript DestroyScript;

		template<typename T>
		void Bind()
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

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
		b2Body* RuntimeBody = nullptr;

		Rigidbody2DComponent() noexcept = default;
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
		b2Fixture* RuntimeFixture = nullptr;

		BoxCollider2DComponent() noexcept = default;
	};

	struct CircleCollider2DComponent
	{
		TRAP::Math::Vec2 Offset{0.0f, 0.0f};
		float Radius = 0.5f;

		//TODO Move into Physics Material in future
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		//Storage for runtime data
		b2Fixture* RuntimeFixture = nullptr;

		CircleCollider2DComponent() noexcept = default;
	};

	template<typename... Component>
	struct ComponentGroup{};

	using AllComponents = ComponentGroup<TransformComponent, SpriteRendererComponent,
	                                     CircleRendererComponent, CameraComponent,
										 NativeScriptComponent, Rigidbody2DComponent,
										 BoxCollider2DComponent, CircleCollider2DComponent>;
}

#endif /*TRAP_COMPONENTS_H*/
#ifndef TRAP_COMPONENTS_H
#define TRAP_COMPONENTS_H

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

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
		constexpr explicit UIDComponent(TRAP::Utils::UID uid)
			: UID(uid)
		{}
	};

	/// <summary>
	/// Tag component.
	/// Every entity has a tag component containing the name for the entity.
	/// This component may not be added/removed to/from an entity.
	/// </summary>
	struct TagComponent
	{
		std::string Tag;

		constexpr TagComponent() noexcept = default;
		inline constexpr explicit TagComponent(std::string tag) noexcept
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

		constexpr TransformComponent() noexcept = default;
		explicit TransformComponent(const TRAP::Math::Vec3& position) noexcept
			: Position(position)
		{}
		constexpr TransformComponent(const TRAP::Math::Vec3& position, const TRAP::Math::Vec3& rotationInRadians) noexcept
			: Position(position), Rotation(rotationInRadians)
		{}
		constexpr TransformComponent(const TRAP::Math::Vec3& position, const TRAP::Math::Vec3& rotationInRadians,
		                             const TRAP::Math::Vec3& scale) noexcept
			: Position(position), Rotation(rotationInRadians), Scale(scale)
		{}

		/// <summary>
		/// Retrieve the transform calculated from current position, rotation and scale.
		/// </summary>
		/// <returns>Transform as Math::Mat4.</returns>
		[[nodiscard]] constexpr Math::Mat4 GetTransform() const noexcept
		{
			if(Rotation.x() != 0.0f || Rotation.y() != 0.0f || Rotation.z() != 0.0f)
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

		constexpr SpriteRendererComponent() noexcept = default;
		constexpr explicit SpriteRendererComponent(const TRAP::Math::Vec4& color) noexcept
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
		f32 Thickness = 1.0f;
		f32 Fade = 0.005f;

		constexpr CircleRendererComponent() noexcept = default;
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

		PFN_InstantiateScript InstantiateScript = nullptr;
		PFN_DestroyScript DestroyScript = nullptr;

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
		enum class BodyType : u32 {Static, Dynamic, Kinematic};
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		//Storage for runtime data
		b2Body* RuntimeBody = nullptr;

		constexpr Rigidbody2DComponent() noexcept = default;
	};

	struct BoxCollider2DComponent
	{
		TRAP::Math::Vec2 Offset{0.0f, 0.0f};
		TRAP::Math::Vec2 Size{0.5f, 0.5f};

		//TODO Move into Physics Material in future
		f32 Density = 1.0f;
		f32 Friction = 0.5f;
		f32 Restitution = 0.0f;
		f32 RestitutionThreshold = 0.5f;

		//Storage for runtime data
		b2Fixture* RuntimeFixture = nullptr;

		constexpr BoxCollider2DComponent() noexcept = default;
	};

	struct CircleCollider2DComponent
	{
		TRAP::Math::Vec2 Offset{0.0f, 0.0f};
		f32 Radius = 0.5f;

		//TODO Move into Physics Material in future
		f32 Density = 1.0f;
		f32 Friction = 0.5f;
		f32 Restitution = 0.0f;
		f32 RestitutionThreshold = 0.5f;

		//Storage for runtime data
		b2Fixture* RuntimeFixture = nullptr;

		constexpr CircleCollider2DComponent() noexcept = default;
	};

	template<typename... Component>
	struct ComponentGroup{};

	using AllComponents = ComponentGroup<TransformComponent, SpriteRendererComponent,
	                                     CircleRendererComponent, CameraComponent,
										 NativeScriptComponent, Rigidbody2DComponent,
										 BoxCollider2DComponent, CircleCollider2DComponent>;

	template<typename T>
	concept IsComponent = std::same_as<T, TransformComponent>      || std::same_as<T, SpriteRendererComponent> ||
	                      std::same_as<T, CircleRendererComponent> || std::same_as<T, CameraComponent>         ||
						  std::same_as<T, NativeScriptComponent>   || std::same_as<T, Rigidbody2DComponent>    ||
						  std::same_as<T, BoxCollider2DComponent>  || std::same_as<T, CircleCollider2DComponent>;
}

#endif /*TRAP_COMPONENTS_H*/
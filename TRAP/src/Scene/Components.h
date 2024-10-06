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
	/// @brief Unique identifier component.
	/// Every entity has a global unique identifier.
	/// This component may not be added/removed to/from an entity.
	struct UIDComponent
	{
		TRAP::Utils::UID UID{};

		/// @brief Constructor.
		UIDComponent() = default;
	};

	/// @brief Tag component.
	/// Every entity has a tag component containing the name for the entity.
	/// This component may not be added/removed to/from an entity.
	struct TagComponent
	{
		std::string Tag;

		/// @brief Constructor.
		constexpr TagComponent() noexcept = default;
		/// @brief Constructor.
		/// @param tag Tag to use.
		constexpr explicit TagComponent(std::string tag) noexcept
			: Tag(std::move(tag))
		{}
	};

	/// @brief Transform component.
	/// Every entity has a transform component containing position, rotation and scale.
	struct TransformComponent
	{
		Math::Vec3 Position{ 0.0f, 0.0f, 0.0f };
		Math::Vec3 Rotation{ 0.0f, 0.0f, 0.0f };
		Math::Vec3 Scale{ 1.0f, 1.0f, 1.0f };

		/// @brief Constructor.
		constexpr TransformComponent() noexcept = default;
		/// @brief Constructor.
		/// @param position Position to use.
		explicit TransformComponent(const TRAP::Math::Vec3& position) noexcept
			: Position(position)
		{}
		/// @brief Constructor.
		/// @param position Position to use.
		/// @param rotationInRadians Rotation to use.
		constexpr TransformComponent(const TRAP::Math::Vec3& position, const TRAP::Math::Vec3& rotationInRadians) noexcept
			: Position(position), Rotation(rotationInRadians)
		{}
		/// @brief Constructor.
		/// @param position Position to use.
		/// @param rotationInRadians Rotation to use.
		/// @param scale Scale to use.
		constexpr TransformComponent(const TRAP::Math::Vec3& position, const TRAP::Math::Vec3& rotationInRadians,
		                             const TRAP::Math::Vec3& scale) noexcept
			: Position(position), Rotation(rotationInRadians), Scale(scale)
		{}

		/// @brief Retrieve the transform calculated from current position, rotation and scale.
		/// @return Transform as Math::Mat4.
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

	/// @brief Sprite renderer component.
	/// Optional component containing sprite renderer data.
	struct SpriteRendererComponent
	{
		Math::Vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		/// @brief Constructor.
		constexpr SpriteRendererComponent() noexcept = default;
		/// @brief Constructor.
		/// @param color Color to use.
		constexpr explicit SpriteRendererComponent(const TRAP::Math::Vec4& color) noexcept
			: Color(color)
		{}
	};

	/// @brief Circle renderer component.
	/// Optional component containing circle renderer data.
	struct CircleRendererComponent
	{
		Math::Vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		f32 Thickness = 1.0f;
		f32 Fade = 0.005f;

		/// @brief Constructor.
		constexpr CircleRendererComponent() noexcept = default;
	};

	/// @brief Camera component.
	/// Optional component containing camera data.
	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; //TODO Maybe move to Scene
		bool FixedAspectRatio = false;

		/// @brief Constructor.
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
			ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	//-------------------------------------------------------------------------------------------------------------------//
	//Physics------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	struct Rigidbody2DComponent
	{
		enum class BodyType : u8 {Static, Dynamic, Kinematic};
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		//Storage for runtime data
		b2Body* RuntimeBody = nullptr;

		/// @brief Constructor.
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

		/// @brief Constructor.
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

		/// @brief Constructor.
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

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Rigidbody2DComponent::BodyType>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Rigidbody2DComponent::BodyType bodyType,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(bodyType)
        {
        case TRAP::Rigidbody2DComponent::BodyType::Static:
            enumStr = "Static";
            break;
        case TRAP::Rigidbody2DComponent::BodyType::Dynamic:
            enumStr = "Dynamic";
            break;
        case TRAP::Rigidbody2DComponent::BodyType::Kinematic:
            enumStr = "Kinematic";
            break;
        }

		if(enumStr.empty())
		{
            TRAP_ASSERT(false, "fmt::formatter<TRAP::Rigidbody2DComponent::BodyType>: Missing enum value!");
            enumStr = "<MISSING ENUM VALUE>";
		}

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] constexpr TRAP::Rigidbody2DComponent::BodyType TRAP::Utils::String::ConvertToType(const std::string& input)
{
    if(Utils::String::CompareAnyCase("Static", input))
        return Rigidbody2DComponent::BodyType::Static;
    if(Utils::String::CompareAnyCase("Dynamic", input))
        return Rigidbody2DComponent::BodyType::Dynamic;
    if(Utils::String::CompareAnyCase("Kinematic", input))
        return Rigidbody2DComponent::BodyType::Kinematic;

    TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Rigidbody2DComponent::BodyType!");
    throw std::invalid_argument("Exception while converting string to TRAP::Rigidbody2DComponent::BodyType!");
}

#endif /*TRAP_COMPONENTS_H*/

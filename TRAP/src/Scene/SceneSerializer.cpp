#include "TRAPPCH.h"
#include "SceneSerializer.h"

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif /*_MSC_VER*/
#include <yaml-cpp/yaml.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif /*_MSC_VER*/

#include "Utils/String/String.h"
#include "Components.h"
#include "Entity.h"
#include "FileSystem/FileSystem.h"

namespace YAML
{
	template<>
	struct convert<TRAP::Math::Vec2>
	{
		[[nodiscard]] static Node encode(const TRAP::Math::Vec2& rhs)
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

			Node node;
			node.push_back(rhs.x());
			node.push_back(rhs.y());
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		[[nodiscard]] static bool decode(const Node& node, TRAP::Math::Vec2& rhs)
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x() = node[0].as<f32>();
			rhs.y() = node[1].as<f32>();
			return true;
		}
	};

	template<>
	struct convert<TRAP::Math::Vec3>
	{
		[[nodiscard]] static Node encode(const TRAP::Math::Vec3& rhs)
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

			Node node;
			node.push_back(rhs.x());
			node.push_back(rhs.y());
			node.push_back(rhs.z());
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		[[nodiscard]] static bool decode(const Node& node, TRAP::Math::Vec3& rhs)
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x() = node[0].as<f32>();
			rhs.y() = node[1].as<f32>();
			rhs.z() = node[2].as<f32>();
			return true;
		}
	};

	template<>
	struct convert<TRAP::Math::Vec4>
	{
		[[nodiscard]] static Node encode(const TRAP::Math::Vec4& rhs)
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

			Node node;
			node.push_back(rhs.x());
			node.push_back(rhs.y());
			node.push_back(rhs.z());
			node.push_back(rhs.w());
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		[[nodiscard]] static bool decode(const Node& node, TRAP::Math::Vec4& rhs)
		{
			ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x() = node[0].as<f32>();
			rhs.y() = node[1].as<f32>();
			rhs.z() = node[2].as<f32>();
			rhs.w() = node[3].as<f32>();
			return true;
		}
	};
}

namespace TRAP
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const Math::Vec2& v)
	{
		ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

		out << YAML::Flow;
		out << YAML::BeginSeq << v.x() << v.y() << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Math::Vec3& v)
	{
		ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

		out << YAML::Flow;
		out << YAML::BeginSeq << v.x() << v.y() << v.z() << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Math::Vec4& v)
	{
		ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

		out << YAML::Flow;
		out << YAML::BeginSeq << v.x() << v.y() << v.z() << v.w() << YAML::EndSeq;
		return out;
	}

	void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

		TRAP_ASSERT(entity.HasComponent<UIDComponent>(), "SerializeEntity(): Entity has no UIDComponent!");

		out << YAML::BeginMap; //Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUID(); //Entity UID

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; //TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; //TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; //TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << tc.Position;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; //TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; //CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; //Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << std::to_underlying(camera.GetProjectionType());
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; //Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; //CameraComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; //SpriteRendererComponent

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

			out << YAML::EndMap; //SpriteRendererComponent
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; //CircleRendererComponent

			auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;

			out << YAML::EndMap; //CircleRendererComponent
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; //Rigidbody2DComponent

			auto& rigidbody2DComponent = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << fmt::format("{}", rigidbody2DComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rigidbody2DComponent.FixedRotation;

			out << YAML::EndMap; //Rigidbody2DComponent
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; //BoxCollider2DComponent

			auto& boxCollider2DComponent = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << boxCollider2DComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << boxCollider2DComponent.Size;
			out << YAML::Key << "Density" << YAML::Value << boxCollider2DComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << boxCollider2DComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << boxCollider2DComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << boxCollider2DComponent.RestitutionThreshold;

			out << YAML::EndMap; //BoxCollider2DComponent
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; //CircleCollider2DComponent

			auto& circleCollider2DComponent = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << circleCollider2DComponent.Offset;
			out << YAML::Key << "Radius" << YAML::Value << circleCollider2DComponent.Radius;
			out << YAML::Key << "Density" << YAML::Value << circleCollider2DComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << circleCollider2DComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << circleCollider2DComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << circleCollider2DComponent.RestitutionThreshold;

			out << YAML::EndMap; //CircleCollider2DComponent
		}

		out << YAML::EndMap; //Entity
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::SceneSerializer::SceneSerializer(Ref<Scene> scene) noexcept
	: m_scene(std::move(scene))
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneSerializer::Serialize(const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

	for(const auto entityID : m_scene->m_registry.storage<entt::entity>())
	{
		const Entity entity = { entityID, m_scene.get() };
		if (!entity)
			return;

		SerializeEntity(out, entity);
	};

	out << YAML::EndSeq;
	out << YAML::EndMap;

	if(!FileSystem::WriteTextFile(filepath, out.c_str()))
		TP_ERROR(Log::SceneSerializerPrefix, " Saving to: ", filepath, " failed!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneSerializer::SerializeRuntime([[maybe_unused]] const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(false, "SceneSerializer::SerializeRuntime(): Not implemented yet!");
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::SceneSerializer::Deserialize(const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	if (!FileSystem::Exists(filepath))
	{
		TP_ERROR(Log::SceneSerializerPrefix, "File: ", filepath, " doesn't exists!");
		return false;
	}

	YAML::Node data;
	try
	{
		data = YAML::LoadFile(filepath.string());
	}
	catch(const YAML::ParserException& ex)
	{
		TP_ERROR(Log::SceneSerializerPrefix, "Failed to load scene file: ", filepath, " ", ex.what());
	}

	if (!data["Scene"])
		return false;

	const std::string sceneName = data["Scene"].as<std::string>();
	TP_TRACE(Log::SceneSerializerPrefix, "Deserializing scene '", sceneName, "'");

	auto entities = data["Entities"];
	if (entities)
	{
		for (auto entity : entities)
		{
			const u64 uid = entity["Entity"].as<u64>();

			std::string name;
			auto tagComponent = entity["TagComponent"];
			if (tagComponent)
				name = tagComponent["Tag"].as<std::string>();

			TP_TRACE(Log::SceneSerializerPrefix, "Deserialized entity with UID = ", uid, ", name = ", name);

			Entity deserializedEntity = m_scene->CreateEntity(name);
			deserializedEntity.GetComponent<UIDComponent>().UID = TRAP::Utils::UID(uid);

			auto transformComponent = entity["TransformComponent"];
			if (transformComponent)
			{
				//Entities always have transforms
				auto& tc = deserializedEntity.GetComponent<TransformComponent>();
				tc.Position = transformComponent["Position"].as<Math::Vec3>();
				tc.Rotation = transformComponent["Rotation"].as<Math::Vec3>();
				tc.Scale = transformComponent["Scale"].as<Math::Vec3>();
			}

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				auto& cc = deserializedEntity.AddComponent<CameraComponent>();

				auto cameraProps = cameraComponent["Camera"];
				cc.Camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>
					(
						cameraProps["ProjectionType"].as<i32>()
					));

				cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<f32>());
				cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<f32>());

				cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<f32>());
				cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<f32>());
				cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<f32>());

				cc.Primary = cameraComponent["Primary"].as<bool>();
				cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			}

			auto spriteRendererComponent = entity["SpriteRendererComponent"];
			if (spriteRendererComponent)
			{
				auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
				src.Color = spriteRendererComponent["Color"].as<Math::Vec4>();
			}

			auto circleRendererComponent = entity["CircleRendererComponent"];
			if (circleRendererComponent)
			{
				auto& src = deserializedEntity.AddComponent<CircleRendererComponent>();
				src.Color = circleRendererComponent["Color"].as<Math::Vec4>();
				src.Thickness = circleRendererComponent["Thickness"].as<f32>();
				src.Fade = circleRendererComponent["Fade"].as<f32>();
			}

			auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
			if (rigidbody2DComponent)
			{
				auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
				rb2d.Type = Utils::String::ConvertToType<Rigidbody2DComponent::BodyType>(rigidbody2DComponent["BodyType"].as<std::string>());
				rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
			}

			auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
			if (boxCollider2DComponent)
			{
				auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
				bc2d.Offset = boxCollider2DComponent["Offset"].as<Math::Vec2>();
				bc2d.Size = boxCollider2DComponent["Size"].as<Math::Vec2>();
				bc2d.Density = boxCollider2DComponent["Density"].as<f32>();
				bc2d.Friction = boxCollider2DComponent["Friction"].as<f32>();
				bc2d.Restitution = boxCollider2DComponent["Restitution"].as<f32>();
				bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<f32>();
			}

			auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
			if (circleCollider2DComponent)
			{
				auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
				cc2d.Offset = circleCollider2DComponent["Offset"].as<Math::Vec2>();
				cc2d.Radius = circleCollider2DComponent["Radius"].as<f32>();
				cc2d.Density = circleCollider2DComponent["Density"].as<f32>();
				cc2d.Friction = circleCollider2DComponent["Friction"].as<f32>();
				cc2d.Restitution = circleCollider2DComponent["Restitution"].as<f32>();
				cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<f32>();
			}
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::SceneSerializer::DeserializeRuntime([[maybe_unused]] const std::filesystem::path& filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(false, "SceneSerializer::DeserializeRuntime(): Not implemented yet!");
	return false;
}
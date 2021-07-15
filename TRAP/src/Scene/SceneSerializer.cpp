#include "TRAPPCH.h"
#include "SceneSerializer.h"

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <yaml-cpp/yaml.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "Components.h"
#include "Entity.h"
#include "VFS/VFS.h"

namespace YAML
{
	template<>
	struct convert<TRAP::Math::Vec3>
	{
		static Node encode(const TRAP::Math::Vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, TRAP::Math::Vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<TRAP::Math::Vec4>
	{
		static Node encode(const TRAP::Math::Vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, TRAP::Math::Vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace TRAP
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const Math::Vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Math::Vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; //Entity
		out << YAML::Key << "Entity" << YAML::Value << "00000000000000"; //Entity ID

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
			out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int32_t>(camera.GetProjectionType());
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
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

		out << YAML::EndMap; //Entity
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::SceneSerializer::SceneSerializer(Ref<Scene> scene)
	: m_scene(std::move(scene))
{}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneSerializer::Serialize(const std::string& filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	m_scene->m_registry.each([&](auto entityID)
	{
		const Entity entity = { entityID, m_scene.get() };
		if (!entity)
			return;

		SerializeEntity(out, entity);
	});
	out << YAML::EndSeq;
	out << YAML::EndMap;

	if(!VFS::WriteTextFile(filepath, out.c_str()))
		TP_ERROR(Log::SceneSerializerPrefix, " Saving to: \"", filepath, "\" failed!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneSerializer::SerializeRuntime(const std::string&)
{
	TRAP_ASSERT(false, "Not implemented yet!");
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::SceneSerializer::Deserialize(const std::string& filepath)
{
	std::filesystem::path path;
	if (!VFS::ResolveReadPhysicalPath(filepath, path, true))
	{
		TP_ERROR(Log::SceneSerializerPrefix, " Couldn't resolve FilePath: \"", path, "\"!");
		return false;
	}

	if (VFS::FileOrFolderExists(path))
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch(const YAML::ParserException& ex)
		{
			TP_ERROR(Log::SceneSerializerPrefix, " Failed to Load Scene file: \"", path, "\" ", ex.what());
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
				const uint64_t uuid = entity["Entity"].as<uint64_t>(); //TODO

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				TP_TRACE(Log::SceneSerializerPrefix, "Deserialized entity with ID = ", uuid, ", name = ", name);

				Entity deserializedEntity = m_scene->CreateEntity(name);

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
							cameraProps["ProjectionType"].as<int32_t>()
						));

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<Math::Vec4>();
				}
			}
		}

		return true;
	}

	TP_ERROR(Log::SceneSerializerPrefix, " File: \"", path, "\" doesnt exists!");
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::SceneSerializer::DeserializeRuntime(const std::string&)
{
	TRAP_ASSERT(false, "Not implemented yet!");
	return false;
}
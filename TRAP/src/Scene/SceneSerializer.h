#ifndef TRAP_SCENESERIALIZER_H
#define TRAP_SCENESERIALIZER_H

#include "Core/Base.h"
#include "Scene.h"

namespace TRAP
{
	class SceneSerializer
	{
	public:
		/// @brief Constructor. Create a new scene serializer for the given scene.
		/// @param scene Scene to serialize.
		explicit SceneSerializer(Ref<Scene> scene) noexcept;
		/// @brief Destructor.
		~SceneSerializer() = default;
		/// @brief Copy constructor.
		SceneSerializer(const SceneSerializer&) noexcept = default;
		/// @brief Copy assignment operator.
		SceneSerializer& operator=(const SceneSerializer&) noexcept = default;
		/// @brief Move constructor.
		SceneSerializer(SceneSerializer&&) noexcept = default;
		/// @brief Move assignment operator.
		SceneSerializer& operator=(SceneSerializer&&) noexcept = default;

		/// @brief Serialize the scene to the given filepath.
		/// @param filepath File to save serialized scene at.
		void Serialize(const std::filesystem::path& filepath);
		void SerializeRuntime(const std::filesystem::path& filepath);

		/// @brief Deserialize the scene from the given filepath.
		/// @param filepath File to load serialized scene from.
		/// @return True on successful deserialization, false otherwise.
		bool Deserialize(const std::filesystem::path& filepath);
		bool DeserializeRuntime(const std::filesystem::path& filepath);

	private:
		Ref<Scene> m_scene;
	};
}

#endif /*TRAP_SCENESERIALIZER_H*/
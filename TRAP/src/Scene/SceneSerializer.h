#ifndef TRAP_SCENESERIALIZER_H
#define TRAP_SCENESERIALIZER_H

#include "Core/Base.h"
#include "Scene.h"

namespace TRAP
{
	//TODO Document
	class SceneSerializer
	{
	public:
		explicit SceneSerializer(Ref<Scene> scene) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~SceneSerializer() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		SceneSerializer(const SceneSerializer&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		SceneSerializer& operator=(const SceneSerializer&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		SceneSerializer(SceneSerializer&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		SceneSerializer& operator=(SceneSerializer&&) noexcept = default;

		void Serialize(const std::filesystem::path& filepath);
		void SerializeRuntime(const std::filesystem::path& filepath);

		bool Deserialize(const std::filesystem::path& filepath);
		bool DeserializeRuntime(const std::filesystem::path& filepath);

	private:
		Ref<Scene> m_scene;
	};
}

#endif /*TRAP_SCENESERIALIZER_H*/
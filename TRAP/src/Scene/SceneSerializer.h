#ifndef TRAP_SCENESERIALIZER_H
#define TRAP_SCENESERIALIZER_H

#include "Scene.h"

namespace TRAP
{
	//TODO Document
	class SceneSerializer
	{
	public:
		explicit SceneSerializer(Ref<Scene> scene);
		/// <summary>
		/// Destructor.
		/// </summary>
		~SceneSerializer() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		SceneSerializer(const SceneSerializer&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		SceneSerializer& operator=(const SceneSerializer&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		SceneSerializer(SceneSerializer&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		SceneSerializer& operator=(SceneSerializer&&) = default;

		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);

	private:
		Ref<Scene> m_scene;
	};
}

#endif /*TRAP_SCENESERIALIZER_H*/
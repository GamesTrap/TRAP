#ifndef _TRAP_SCENESERIALIZER_H_
#define _TRAP_SCENESERIALIZER_H_

#include "Scene.h"

namespace TRAP
{
	//TODO Document
	class SceneSerializer
	{
	public:
		explicit SceneSerializer(Ref<Scene> scene);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~SceneSerializer() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		SceneSerializer(const SceneSerializer&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		SceneSerializer& operator=(const SceneSerializer&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		SceneSerializer(SceneSerializer&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
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

#endif /*_TRAP_SCENESERIALIZER_H_*/
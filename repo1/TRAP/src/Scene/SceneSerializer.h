#ifndef _TRAP_SCENESERIALIZER_H_
#define _TRAP_SCENESERIALIZER_H_

#include "Scene.h"

namespace TRAP
{
	//Document
	class SceneSerializer
	{
	public:
		explicit SceneSerializer(Ref<Scene> scene);

		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);

	private:
		Ref<Scene> m_scene;
	};
}

#endif /*_TRAP_SCENESERIALIZER_H_*/
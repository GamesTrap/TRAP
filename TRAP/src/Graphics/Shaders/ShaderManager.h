#ifndef _TRAP_SHADERMANAGER_H_
#define _TRAP_SHADERMANAGER_H_

#include "Shader.h"

namespace TRAP::Graphics
{
	class ShaderManager
	{
	public:
		static void Add(std::unique_ptr<Shader> shader);
		static const std::unique_ptr<Shader>& Get(const std::string& name);
		static const std::unordered_map<std::string, std::unique_ptr<Shader>>& GetShaders();
		static void Clean();

		static void Reload(const std::string& nameOrVirtualPath);
		static void Reload(const std::unique_ptr<Shader>& shader);
		static void ReloadAll();

		static void Shutdown();		

		static bool Exists(const std::string& name);

	private:
		ShaderManager() = default;

		static std::unordered_map<std::string, std::unique_ptr<Shader>> s_Shaders;
	};
}

#endif /*_TRAP_SHADERMANAGER_H_*/
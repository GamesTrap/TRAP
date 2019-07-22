#ifndef _TRAP_SHADERMANAGER_H_
#define _TRAP_SHADERMANAGER_H_

#include "Shader.h"

namespace TRAP::Graphics
{
	class ShaderManager
	{
	public:
		static void Add(std::unique_ptr<API::Shader> shader);
		static API::Shader* Get(std::string_view name);
		static void Clean();

		static void Reload(const std::string& nameOrVirtualPath);
		static void Reload(const std::unique_ptr<API::Shader>& shader);
		static void ReloadAll();

		static void Shutdown();

		static const std::vector<std::unique_ptr<API::Shader>>& GetShaders();

	private:
		ShaderManager() = default;

		static std::vector<std::unique_ptr<API::Shader>> s_Shaders;
	};
}

inline const std::vector<std::unique_ptr<TRAP::Graphics::API::Shader>>& TRAP::Graphics::ShaderManager::GetShaders()
{
	return s_Shaders;
}

#endif /*_TRAP_SHADERMANAGER_H_*/
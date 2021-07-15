#ifndef _TRAP_SHADERMANAGER_H_
#define _TRAP_SHADERMANAGER_H_

#include "Shader.h"

namespace TRAP::Graphics
{
	class ShaderManager
	{
	private:
		ShaderManager() = default;
		~ShaderManager() = default;

	public:
		ShaderManager(const ShaderManager&) = delete;
		ShaderManager& operator=(const ShaderManager&) = delete;
		ShaderManager(ShaderManager&&) = delete;
		ShaderManager& operator=(ShaderManager&&) = delete;

		static const Scope<Shader>& LoadFile(const std::string& filepath,
		                                     const std::vector<Shader::Macro>* userMacros = nullptr);
		static const Scope<Shader>& LoadFile(const std::string& name, const std::string& filepath,
		                                     const std::vector<Shader::Macro>* userMacros = nullptr);
		static const Scope<Shader>& LoadSource(const std::string& name,
		                                       const std::string& glslSource,
										       const std::vector<Shader::Macro>* userMacros = nullptr);

		static void Add(Scope<Shader> shader);
		static void Remove(const Scope<Shader>& shader);
		static void Remove(const std::string& name);
		static const Scope<Shader>& Get(const std::string& name);
		static const std::unordered_map<std::string, Scope<Shader>>& GetShaders();
		static void Clean();

		static void Reload(const std::string& nameOrVirtualPath);
		static void Reload(const Scope<Shader>& shader);
		static void ReloadAll();

		static void Shutdown();

		static bool Exists(const std::string& name);
		static bool ExistsVirtualPath(std::string_view virtualPath);

	private:
		static std::unordered_map<std::string, Scope<Shader>> s_Shaders;
	};
}

#endif /*_TRAP_SHADERMANAGER_H_*/
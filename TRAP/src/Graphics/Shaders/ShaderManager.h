#ifndef TRAP_SHADERMANAGER_H
#define TRAP_SHADERMANAGER_H

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

		static const Scope<Shader>& LoadFile(const std::filesystem::path& filepath,
		                                     const std::vector<Shader::Macro>* userMacros = nullptr);
		static const Scope<Shader>& LoadFile(const std::string& name, const std::filesystem::path& filepath,
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

		static Shader* Reload(const std::string& nameOrPath);
		static Shader* Reload(const Scope<Shader>& shader);
		static void ReloadAll();

		static void Shutdown();

		static bool Exists(const std::string& name);
		static bool ExistsPath(const std::filesystem::path& path);

	private:
		static std::unordered_map<std::string, Scope<Shader>> s_Shaders;
	};
}

#endif /*TRAP_SHADERMANAGER_H*/
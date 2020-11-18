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
		
		static const Scope<Shader>& Load(const std::string& filepath);
		static const Scope<Shader>& Load(const std::string& name, const std::string& filepath);
		static const Scope<Shader>& Load(const std::string& name,
		                                 std::string_view VSSource,
		                                 std::string_view FSSource,
		                                 std::string_view GSSource = "",
		                                 std::string_view TCSSource = "",
		                                 std::string_view TESSource = "",
		                                 std::string_view CSSSource = "");
		
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
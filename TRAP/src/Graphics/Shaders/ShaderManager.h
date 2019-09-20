#ifndef _TRAP_SHADERMANAGER_H_
#define _TRAP_SHADERMANAGER_H_

#include "Shader.h"

namespace TRAP::Graphics
{
	class ShaderManager
	{
	public:
		static const Scope<Shader>& Load(const std::string& filepath);
		static const Scope<Shader>& Load(const std::string& name, const std::string& filepath);
		static const Scope<Shader>& Load(const std::string& name,
		                                 const std::string& VSSource,
		                                 const std::string& FSSource,
		                                 const std::string& GSSource = "",
		                                 const std::string& TCSSource = "",
		                                 const std::string& TESSource = "",
		                                 const std::string& CSSSource = "");
		
		static void Add(Scope<Shader> shader);
		static const Scope<Shader>& Get(const std::string& name);
		static const std::unordered_map<std::string, Scope<Shader>>& GetShaders();
		static void Clean();

		static void Reload(const std::string& nameOrVirtualPath);
		static void Reload(const Scope<Shader>& shader);
		static void ReloadAll();

		static void Shutdown();		

		static bool Exists(const std::string& name);
		static bool ExistsVirtualPath(const std::string& virtualPath);
		
	private:
		ShaderManager() = default;

		static std::unordered_map<std::string, Scope<Shader>> s_Shaders;
	};
}

#endif /*_TRAP_SHADERMANAGER_H_*/
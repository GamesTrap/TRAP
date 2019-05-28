#pragma once

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

	private:
		ShaderManager() = default;

		static std::vector<std::unique_ptr<API::Shader>> s_Shaders;
	};
}

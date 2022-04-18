#ifndef TRAP_SHADERMANAGER_H
#define TRAP_SHADERMANAGER_H

#include "Shader.h"

namespace TRAP::Graphics
{
	class ShaderManager
	{
	private:
		/// <summary>
		/// Constructor.
		/// </summary>
		ShaderManager() = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~ShaderManager() = default;

	public:
		/// <summary>
		/// Copy constructor.
		/// </summary>
		ShaderManager(const ShaderManager&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		ShaderManager& operator=(const ShaderManager&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		ShaderManager(ShaderManager&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		ShaderManager& operator=(ShaderManager&&) = delete;

		/// <summary>
		/// Load a shader from file.
		/// File name will be used as the shader name.
		/// </summary>
		/// <param name="filepath">File path of shader to load.</param>
		/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
		/// <returns>Loaded shader.</returns>
		static const Scope<Shader>& LoadFile(const std::filesystem::path& filepath,
		                                     const std::vector<Shader::Macro>* userMacros = nullptr);
		/// <summary>
		/// Load a shader from file.
		/// </summary>
		/// <param name="name">Name for the shader.</param>
		/// <param name="filepath">File path of shader to load.</param>
		/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
		/// <returns>Loaded shader.</returns>
		static const Scope<Shader>& LoadFile(const std::string& name, const std::filesystem::path& filepath,
		                                     const std::vector<Shader::Macro>* userMacros = nullptr);
		/// <summary>
		/// Load a shader from GLSL source.
		/// </summary>
		/// <param name="name">Name for the shader.</param>
		/// <param name="glslSource">GLSL source code.</param>
		/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
		/// <returns>Loaded shader.</returns>
		static const Scope<Shader>& LoadSource(const std::string& name,
		                                       const std::string& glslSource,
										       const std::vector<Shader::Macro>* userMacros = nullptr);

		/// <summary>
		/// Add a shader to the ShaderManager.
		/// </summary>
		/// <param name="shader">Shader to add.</param>
		static void Add(Scope<Shader> shader);
		/// <summary>
		/// Remove a shader from the ShaderManager.
		/// </summary>
		/// <param name="shader">Shader to remove.</param>
		static void Remove(const Scope<Shader>& shader);
		/// <summary>
		/// Remove a shader from the ShaderManager via its name.
		/// </summary>
		/// <param name="name">Name of shader to remove.</param>
		static void Remove(const std::string& name);
		/// <summary>
		/// Retrieve a shader from the ShaderManager.
		/// </summary>
		/// <param name="name">Name of the shader to retrieve.</param>
		/// <returns>Shader, Fallback shader if not found.</returns>
		static const Scope<Shader>& Get(const std::string& name);
		/// <summary>
		/// Retrieve all loaded shader from the ShaderManager.
		/// </summary>
		/// <returns>Map of all loaded shaders.</returns>
		static const std::unordered_map<std::string, Scope<Shader>>& GetShaders();
		/// <summary>
		/// Clear all shaders from the ShaderManager.
		/// </summary>
		static void Clean();

		/// <summary>
		/// Reload a shader via its name or path.
		/// </summary>
		/// <param name="nameOrPath">Name or path of a shader.</param>
		/// <returns>Pointer to reloaded shader on success, nullptr otherwise.</returns>
		static Shader* Reload(const std::string& nameOrPath);
		/// <summary>
		/// Reload a shader.
		/// </summary>
		/// <param name="shader">Shader to reload.</param>
		/// <returns>Pointer to reloaded shader on success, nullptr otherwise.</returns>
		static Shader* Reload(const Scope<Shader>& shader);
		/// <summary>
		/// Reload all currently loaded shaders.
		/// </summary>
		static void ReloadAll();

		/// <summary>
		/// Shutdown ShaderManager.
		/// </summary>
		static void Shutdown();

		/// <summary>
		/// Check whether a shader exists.
		/// </summary>
		/// <param name="name">Name of shader to check.</param>
		/// <returns>True if shader exists, false otherwise.</returns>
		static bool Exists(const std::string& name);
		/// <summary>
		/// Check whether a shader exists by path.
		/// </summary>
		/// <param name="path">Path of shader to check.</param>
		/// <returns>True if shader exists, false otherwise.</returns>
		static bool ExistsPath(const std::filesystem::path& path);

	private:
		static std::unordered_map<std::string, Scope<Shader>> s_Shaders;
	};
}

#endif /*TRAP_SHADERMANAGER_H*/
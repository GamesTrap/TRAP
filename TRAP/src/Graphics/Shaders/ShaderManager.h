#ifndef TRAP_SHADERMANAGER_H
#define TRAP_SHADERMANAGER_H

#include "Shader.h"

namespace TRAP::Graphics::ShaderManager
{
	/// <summary>
	/// Load a shader from file.
	/// File name will be used as the shader name.
	/// </summary>
	/// <param name="filepath">File path of shader to load.</param>
	/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
	/// <returns>Loaded shader.</returns>
	Ref<Shader> LoadFile(const std::filesystem::path& filepath,
					     const std::vector<Shader::Macro>* userMacros = nullptr);
	/// <summary>
	/// Load a shader from file.
	/// </summary>
	/// <param name="name">Name for the shader.</param>
	/// <param name="filepath">File path of shader to load.</param>
	/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
	/// <returns>Loaded shader.</returns>
	Ref<Shader> LoadFile(const std::string& name, const std::filesystem::path& filepath,
					     const std::vector<Shader::Macro>* userMacros = nullptr);
	/// <summary>
	/// Load a shader from GLSL source.
	/// </summary>
	/// <param name="name">Name for the shader.</param>
	/// <param name="glslSource">GLSL source code.</param>
	/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
	/// <returns>Loaded shader.</returns>
	Ref<Shader> LoadSource(const std::string& name,
					       const std::string& glslSource,
					       const std::vector<Shader::Macro>* userMacros = nullptr);

	/// <summary>
	/// Add a shader to the ShaderManager.
	/// </summary>
	/// <param name="shader">Shader to add.</param>
	void Add(Ref<Shader> shader);
	/// <summary>
	/// Remove a shader from the ShaderManager.
	/// </summary>
	/// <param name="shader">Shader to remove.</param>
	/// <returns>Removed shader on success, nullptr otherwise.</returns>
	[[maybe_unused]] Ref<Shader> Remove(Ref<Shader> shader);
	/// <summary>
	/// Remove a shader from the ShaderManager via its name.
	/// </summary>
	/// <param name="name">Name of shader to remove.</param>
	/// <returns>Removed shader on success, nullptr otherwise.</returns>
	[[maybe_unused]] Ref<Shader> Remove(const std::string& name);
	/// <summary>
	/// Retrieve a shader from the ShaderManager.
	/// </summary>
	/// <param name="name">Name of the shader to retrieve.</param>
	/// <returns>Shader, Fallback shader if not found.</returns>
	Ref<Shader> Get(const std::string& name);
	/// <summary>
	/// Retrieve all loaded shader from the ShaderManager.
	/// </summary>
	/// <returns>Map of all loaded shaders.</returns>
	const std::unordered_map<std::string, Ref<Shader>>& GetShaders();
	/// <summary>
	/// Clear all shaders from the ShaderManager.
	/// </summary>
	void Clean();

	/// <summary>
	/// Reload a shader via its name or path.
	/// </summary>
	/// <param name="nameOrPath">Name or path of a shader.</param>
	/// <returns>
	/// Shader if found (even on unsucessful reload), nullptr otherwise.
	/// Should only return nullptr if the shader was not found.
	/// </returns>
	Ref<Shader> Reload(const std::string& nameOrPath);
	/// <summary>
	/// Reload a shader.
	/// </summary>
	/// <param name="shader">Shader to reload.</param>
	/// <returns>
	/// Shader if found (even on unsuccessful reload), nullptr otherwise.
	/// Should only return nullptr if the shader was not found.
	/// </returns>
	Ref<Shader> Reload(Ref<Shader> shader);
	/// <summary>
	/// Reload all currently loaded shaders.
	/// </summary>
	void ReloadAll();

	/// <summary>
	/// Check whether a shader exists.
	/// </summary>
	/// <param name="name">Name of shader to check.</param>
	/// <returns>True if shader exists, false otherwise.</returns>
	bool Exists(const std::string& name);
	/// <summary>
	/// Check whether a shader exists by path.
	/// </summary>
	/// <param name="path">Path of shader to check.</param>
	/// <returns>True if shader exists, false otherwise.</returns>
	bool ExistsPath(const std::filesystem::path& path);
}

#endif /*TRAP_SHADERMANAGER_H*/
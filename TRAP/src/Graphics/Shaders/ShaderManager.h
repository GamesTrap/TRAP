#ifndef TRAP_SHADERMANAGER_H
#define TRAP_SHADERMANAGER_H

#include "Shader.h"

namespace TRAP::Graphics::ShaderManager
{
	/// @brief Load a shader from file.
	/// File name will be used as the shader name.
	/// @param filepath File path of shader to load.
	/// @param userMacros Optional user defined macros. Default: nullptr.
	/// @return Loaded shader.
	Ref<Shader> LoadFile(const std::filesystem::path& filepath,
					     const std::vector<Shader::Macro>* userMacros = nullptr);
	/// @brief Load a shader from file.
	/// @param name Name for the shader.
	/// @param filepath File path of shader to load.
	/// @param userMacros Optional user defined macros. Default: nullptr.
	/// @return Loaded shader.
	Ref<Shader> LoadFile(const std::string& name, const std::filesystem::path& filepath,
					     const std::vector<Shader::Macro>* userMacros = nullptr);
	/// @brief Load a shader from GLSL source.
	/// @param name Name for the shader.
	/// @param glslSource GLSL source code.
	/// @param userMacros Optional user defined macros. Default: nullptr.
	/// @return Loaded shader.
	Ref<Shader> LoadSource(const std::string& name,
					       const std::string& glslSource,
					       const std::vector<Shader::Macro>* userMacros = nullptr);

	/// @brief Add a shader to the ShaderManager.
	/// @param shader Shader to add.
	void Add(Ref<Shader> shader);
	/// @brief Remove a shader from the ShaderManager.
	/// @param shader Shader to remove.
	/// @return Removed shader on success, nullptr otherwise.
	[[maybe_unused]] Ref<Shader> Remove(const Ref<Shader>& shader);
	/// @brief Remove a shader from the ShaderManager via its name.
	/// @param name Name of shader to remove.
	/// @return Removed shader on success, nullptr otherwise.
	[[maybe_unused]] Ref<Shader> Remove(const std::string& name);
	/// @brief Retrieve a shader from the ShaderManager.
	/// @param name Name of the shader to retrieve.
	/// @return Shader, Fallback shader if not found.
	[[nodiscard]] Ref<Shader> Get(const std::string& name);
	/// @brief Retrieve all loaded shader from the ShaderManager.
	/// @return Map of all loaded shaders.
	[[nodiscard]] const std::unordered_map<std::string, Ref<Shader>, Utils::StringHasher>& GetShaders() noexcept;
	/// @brief Clear all shaders from the ShaderManager.
	/// Except fallback shaders.
	void Clean();

	/// @brief Reload a shader via its name or path.
	/// @param nameOrPath Name or path of a shader.
	/// @return Shader if found (even on unsucessful reload), nullptr otherwise.
	/// Should only return nullptr if the shader was not found.
	Ref<Shader> Reload(const std::string& nameOrPath);
	/// @brief Reload a shader.
	/// @param shader Shader to reload.
	/// @return Shader if found (even on unsuccessful reload), nullptr otherwise.
	/// Should only return nullptr if the shader was not found.
	Ref<Shader> Reload(Ref<Shader> shader);
	/// @brief Reload all currently loaded shaders.
	void ReloadAll();

	/// @brief Check whether a shader exists.
	/// @param name Name of shader to check.
	/// @return True if shader exists, false otherwise.
	[[nodiscard]] bool Exists(const std::string& name);
	/// @brief Check whether a shader exists by path.
	/// @param path Path of shader to check.
	/// @return True if shader exists, false otherwise.
	[[nodiscard]] bool ExistsPath(const std::filesystem::path& path);

	/// @brief Shutdown the ShaderManager.
	void Shutdown() noexcept;
}

#endif /*TRAP_SHADERMANAGER_H*/
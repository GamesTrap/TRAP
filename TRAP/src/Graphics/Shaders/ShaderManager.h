#ifndef TRAP_SHADERMANAGER_H
#define TRAP_SHADERMANAGER_H

#include "Shader.h"

namespace TRAP::Graphics::ShaderManager
{
	/// @brief Load a shader from file.
	/// File name will be used as the shader name.
	/// @param filepath File path of shader to load.
	/// @param shaderType Type of the shader.
	/// @param userMacros Optional user defined macros. Default: nullptr.
	/// @return Loaded shader.
	Ref<Shader> LoadFile(const std::filesystem::path& filepath,
	                     RendererAPI::ShaderType shaderType,
					     const std::vector<Shader::Macro>& userMacros = {});
	/// @brief Load a shader from file.
	/// @param name Name for the shader.
	/// @param filepath File path of shader to load.
	/// @param shaderType Type of the shader.
	/// @param userMacros Optional user defined macros. Default: nullptr.
	/// @return Loaded shader.
	Ref<Shader> LoadFile(const std::string& name,
	                     const std::filesystem::path& filepath,
						 RendererAPI::ShaderType shaderType,
					     const std::vector<Shader::Macro>& userMacros = {});
	/// @brief Load a shader from GLSL source.
	/// @param name Name for the shader.
	/// @param glslSource GLSL source code.
	/// @param shaderType Type of the shader.
	/// @param userMacros Optional user defined macros. Default: nullptr.
	/// @return Loaded shader.
	Ref<Shader> LoadSource(const std::string& name,
					       const std::string& glslSource,
						   RendererAPI::ShaderType shaderType,
					       const std::vector<Shader::Macro>& userMacros = {});

	/// @brief Add a shader to the ShaderManager.
	/// @param shader Shader to add.
	void Add(const Ref<Shader>& shader);
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
	/// @param shaderType Type of the shader to retrieve.
	/// @return Shader, Fallback shader if not found.
	[[nodiscard]] Ref<Shader> Get(std::string_view name, RendererAPI::ShaderType shaderType);
	/// @brief Retrieve a graphics shader from the ShaderManager.
	/// @param name Name of the shader to retrieve.
	/// @return Shader, Fallback shader if not found.
	[[nodiscard]] Ref<Shader> GetGraphics(std::string_view name);
	/// @brief Retrieve a compute shader from the ShaderManager.
	/// @param name Name of the shader to retrieve.
	/// @return Shader, Fallback shader if not found.
	[[nodiscard]] Ref<Shader> GetCompute(std::string_view name);
	/// @brief Retrieve a shader from the ShaderManager.
	/// @param path Path of the shader to retrieve.
	/// @return Shader, nullptr if not found.
	[[nodiscard]] Ref<Shader> GetByPath(const std::filesystem::path& path);
	/// @brief Retrieve all loaded shaders from the ShaderManager.
	/// @return Map of all loaded shaders.
	[[nodiscard]] const TRAP::Utils::UnorderedStringMap<Ref<Shader>>& GetShaders() noexcept;
	/// @brief Clear all shaders from the ShaderManager.
	/// Except fallback shaders.
	void Clean();

	/// @brief Check whether a shader is in the ShaderManager.
	/// @param name Name of shader to check.
	/// @return True if shader is in ShaderManager, false otherwise.
	[[nodiscard]] bool Contains(std::string_view name);
	/// @brief Check whether a shader is in the ShaderManager by path.
	/// @param path Path of shader to check.
	/// @return True if shader is in ShaderManager, false otherwise.
	[[nodiscard]] bool ContainsByPath(const std::filesystem::path& path);

	/// @brief Shutdown the ShaderManager.
	void Shutdown() noexcept;
}

#endif /*TRAP_SHADERMANAGER_H*/

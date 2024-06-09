#ifndef TRAP_SHADER_H
#define TRAP_SHADER_H

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
//SPIRV
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
//SPIRV to GLSL
#include <spirv_glsl.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "Core/Base.h"
#include "Application.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/DescriptorSet.h"

namespace TRAP::Graphics
{
	class UniformBuffer;
	class StorageBuffer;

	class Shader
	{
	public:
		/// @brief Struct defining a shader macro.
		struct Macro
		{
			std::string Definition{};
			std::string Value{};
		};

	protected:
		/// @brief Constructor.
		Shader(std::string name, bool valid, RendererAPI::ShaderStage stages, const std::vector<Macro>& userMacros = {}, const std::filesystem::path& filepath = "");
	public:
		/// @brief Copy constructor.
		consteval Shader(const Shader&) = delete;
		/// @brief Copy assignment operator.
		consteval Shader& operator=(const Shader&) = delete;
		/// @brief Move constructor.
		Shader(Shader&&) noexcept = default;
		/// @brief Move assignment operator.
		Shader& operator=(Shader&&) noexcept = default;
		/// @brief Destructor.
		virtual ~Shader() = default;

		/// @brief Reload shader.
		/// @return True on successful reload (valid shader), else (invalid shader) otherwise.
		bool Reload();

		/// @brief Retrieve the unique identifier of the shader.
		/// @note The ID of the shader changes when reloaded.
		[[nodiscard]] constexpr virtual usize GetID() const noexcept = 0;

		/// @brief Retrieve the name of the shader.
		/// @return Name of the shader.
		[[nodiscard]] constexpr std::string GetName() const noexcept;

		/// @brief Retrieve the file path of the shader.
		/// @return File path of the shader.
		[[nodiscard]] std::filesystem::path GetFilePath() const noexcept;

		/// @brief Retrieve the shader stages of the shader.
		/// @brief @return Shader stages of the shader.
		[[nodiscard]] constexpr RendererAPI::ShaderStage GetShaderStages() const noexcept;

		/// @brief Retrieve the used macros of the shader.
		/// @return Used macros.
		[[nodiscard]] constexpr const std::vector<Macro>& GetMacros() const noexcept;

		/// @brief Retrieve the root signature of the shader.
		/// @return Root signature of the shader.
		[[nodiscard]] TRAP::Ref<RootSignature> GetRootSignature() const noexcept;
		/// @brief Retrieve the descriptor sets of the shader.
		/// @return Descriptor sets of the shader.
		[[nodiscard]] constexpr const std::array<TRAP::Scope<DescriptorSet>, RendererAPI::MaxDescriptorSets>& GetDescriptorSets() const noexcept;

		/// @brief Retrieve whether the shader is valid (i.e. loaded and compiled) or not.
		/// @return True if shader is valid, false otherwise.
		[[nodiscard]] constexpr bool IsShaderValid() const noexcept;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use shader for rendering on the given window.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void Use(const Window& window = *TRAP::Application::GetWindow()) = 0;
#else
		/// @brief Use shader for rendering.
		/// @remark This function is only available in headless mode.
		virtual void Use() = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Use texture with this shader on the given window.
		/// @param set Descriptor set to use the texture with.
		/// @param binding Binding point of the texture.
		/// @param texture Texture to use.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseTexture(u32 set, u32 binding, const TRAP::Graphics::Texture& texture,
		                        const Window& window = *TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use texture with this shader.
		/// @param set Descriptor set to use the texture with.
		/// @param binding Binding point of the texture.
		/// @param texture Texture to use.
		/// @remark This function is only available in headless mode.
		virtual void UseTexture(u32 set, u32 binding, const TRAP::Graphics::Texture& texture) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use multiple textures with this shader on the given window.
		/// @param set Descriptor set to use the textures with.
		/// @param binding Binding point of the textures.
		/// @param textures Textures to use.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseTextures(u32 set, u32 binding,
		                         const std::vector<const TRAP::Graphics::Texture*>& textures,
								 const Window& window = *TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use multiple textures with this shader.
		/// @param set Descriptor set to use the textures with.
		/// @param binding Binding point of the textures.
		/// @param textures Textures to use.
		/// @remark This function is only available in headless mode.
		virtual void UseTextures(u32 set, u32 binding,
		                         const std::vector<const TRAP::Graphics::Texture*>& textures) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use sampler with this shader on the given window.
		/// @param set Descriptor set to use the sampler with.
		/// @param binding Binding point of the sampler.
		/// @param sampler Sampler to use.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseSampler(u32 set, u32 binding, const TRAP::Graphics::Sampler& sampler,
		                        const Window& window = *TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use sampler with this shader.
		/// @param set Descriptor set to use the sampler with.
		/// @param binding Binding point of the sampler.
		/// @param sampler Sampler to use.
		/// @remark This function is only available in headless mode.
		virtual void UseSampler(u32 set, u32 binding, const TRAP::Graphics::Sampler& sampler) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Use multiple samplers with this shader on the given window.
		/// @param set Descriptor set to use the samplers with.
		/// @param binding Binding point of the samplers.
		/// @param samplers Samplers to use.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseSamplers(u32 set, u32 binding,
		                         const std::vector<const TRAP::Graphics::Sampler*>& samplers,
								 const Window& window = *TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use multiple samplers with this shader.
		/// @param set Descriptor set to use the samplers with.
		/// @param binding Binding point of the samplers.
		/// @param samplers Samplers to use.
		/// @remark This function is only available in headless mode.
		virtual void UseSamplers(u32 set, u32 binding,
		                         const std::vector<const TRAP::Graphics::Sampler*>& samplers) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use uniform buffer object with this shader on the given window.
		/// @param set Descriptor set to use the UBO with.
		/// @param binding Binding point of the UBO.
		/// @param uniformBuffer Uniform buffer to use.
		/// @param size Size of the UBO.
		/// @param offset Offset of the UBO.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseUBO(u32 set, u32 binding, const TRAP::Graphics::UniformBuffer& uniformBuffer,
							u64 size = 0, u64 offset = 0, const Window& window = *TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use uniform buffer object with this shader.
		/// @param set Descriptor set to use the UBO with.
		/// @param binding Binding point of the UBO.
		/// @param uniformBuffer Uniform buffer to use.
		/// @param size Size of the UBO.
		/// @param offset Offset of the UBO.
		/// @remark This function is only available in headless mode.
		virtual void UseUBO(u32 set, u32 binding, const TRAP::Graphics::UniformBuffer& uniformBuffer,
							u64 size = 0, u64 offset = 0) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use shader storage buffer object with this shader on the given window.
		/// @param set Descriptor set to use the SSBO with.
		/// @param binding Binding point of the SSBO.
		/// @param storageBuffer Storage buffer to use.
		/// @param size Size of the SSBO.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseSSBO(u32 set, u32 binding, const TRAP::Graphics::StorageBuffer& storageBuffer,
							 u64 size = 0, const Window& window = *TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use shader storage buffer object with this shader.
		/// @param set Descriptor set to use the SSBO with.
		/// @param binding Binding point of the SSBO.
		/// @param storageBuffer Storage buffer to use.
		/// @param size Size of the SSBO.
		/// @remark This function is only available in headless mode.
		virtual void UseSSBO(u32 set, u32 binding, const TRAP::Graphics::StorageBuffer& storageBuffer,
							 u64 size = 0) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Retrieve the shaders thread count per work group.
		/// @return Shaders thread count per work group.
		[[nodiscard]] virtual constexpr std::array<u32, 3> GetNumThreadsPerGroup() const noexcept = 0;

		/// @brief Create a shader from file.
		/// @param name Name for the shader.
		/// @param filePath File path of the shader.
		/// @param userMacros Optional user defined macros. Default: nullptr.
		/// @return Loaded Shader on success, Fallback Shader otherwise.
		[[nodiscard]] static Ref<Shader> CreateFromFile(const std::string& name, const std::filesystem::path& filePath,
		                                                const std::vector<Macro>& userMacros = {});
		/// @brief Create a shader from file.
		/// File name will be used as the shader name.
		/// @param filePath File path of the shader.
		/// @param userMacros Optional user defined macros. Default: nullptr.
		/// @return Loaded Shader on success, Fallback Shader otherwise.
		[[nodiscard]] static Ref<Shader> CreateFromFile(const std::filesystem::path& filePath,
		                                                const std::vector<Macro>& userMacros = {});
		/// @brief Create a shader from GLSL source.
		/// @param name Name for the shader.
		/// @param glslSource GLSL Source code.
		/// @param userMacros Optional user defined macros. Default: nullptr.
		/// @return Loaded Shader on success, Fallback Shader otherwise.
		[[nodiscard]] static Ref<Shader> CreateFromSource(const std::string& name, const std::string& glslSource,
		                                                  const std::vector<Macro>& userMacros = {});

		static constexpr std::array<std::string_view, 3> SupportedShaderFormatSuffixes{".shader", ".glsl", ".tp-spv"};

	protected:
		/// @brief Initialize API dependent shader.
		/// @param desc Binary shader description.
		virtual void Init(const RendererAPI::BinaryShaderDesc& desc) = 0;
		/// @brief Shutdown API dependent shader.
		virtual void Shutdown() = 0;

		std::string m_name;
		std::filesystem::path m_filepath;
		RendererAPI::ShaderStage m_shaderStages{};
		TRAP::Ref<RootSignature> m_rootSignature;
		std::array<TRAP::Scope<DescriptorSet>, RendererAPI::MaxDescriptorSets> m_descriptorSets{};
		std::vector<Macro> m_macros;
		bool m_valid{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Graphics::Shader::GetName() const noexcept
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::ShaderStage TRAP::Graphics::Shader::GetShaderStages() const noexcept
{
	return m_shaderStages;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::Shader::Macro>& TRAP::Graphics::Shader::GetMacros() const noexcept
{
	return m_macros;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<TRAP::Scope<TRAP::Graphics::DescriptorSet>,
                                         TRAP::Graphics::RendererAPI::MaxDescriptorSets>& TRAP::Graphics::Shader::GetDescriptorSets() const noexcept
{
	return m_descriptorSets;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::Shader::IsShaderValid() const noexcept
{
	return m_valid;
}

#endif /*TRAP_SHADER_H*/

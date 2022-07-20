#ifndef TRAP_VULKANSHADER_H
#define TRAP_VULKANSHADER_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/ShaderReflection.h"
#include "Graphics/Shaders/Shader.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanShader final : public Shader
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name for the shader.</param>
		/// <param name="filePath">Filepath of the shader.</param>
		/// <param name="desc">Binary shader description.</param>
		/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
		/// <param name="valid">Whether the shader is valid or not. Default: true.</param>
		VulkanShader(std::string name, std::filesystem::path filepath, const RendererAPI::BinaryShaderDesc& desc,
		             const std::vector<Macro>* userMacros = nullptr, bool valid = true);
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name for the shader.</param>
		/// <param name="desc">Binary shader description.</param>
		/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
		/// <param name="valid">Whether the shader is valid or not. Default: true.</param>
		VulkanShader(std::string name, const RendererAPI::BinaryShaderDesc& desc,
		             const std::vector<Macro>* userMacros = nullptr, bool valid = true);
		/// <summary>
		/// Constructor.
		///
		/// Note: Used for invalid shaders, this doesn't create a usable shader.
		/// </summary>
		/// <param name="name">Name for the shader.</param>
		/// <param name="filepath">Filepath of the shader.</param>
		/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
		VulkanShader(std::string name, std::filesystem::path filepath,
		             const std::vector<Macro>* userMacros = nullptr,
					 RendererAPI::ShaderStage stages = RendererAPI::ShaderStage::None);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanShader() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanShader(const VulkanShader&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanShader& operator=(const VulkanShader&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanShader(VulkanShader&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanShader& operator=(VulkanShader&&) = default;

		/// <summary>
		/// Retrieve the Vulkan shader module handles of each contained shader stage.
		/// </summary>
		/// <returns>Vulkan shader module handles.</returns>
		const std::vector<VkShaderModule>& GetVkShaderModules() const;
		/// <summary>
		/// Retrieve the reflection data of each contained shader stage.
		/// </summary>
		/// <returns>Shader reflection data.</returns>
		TRAP::Ref<ShaderReflection::PipelineReflection> GetReflection() const;
		/// <summary>
		/// Retrieve the entry point names used by each contained shader stage.
		/// </summary>
		/// <returns>Entry point names.</returns>
		const std::vector<std::string>& GetEntryNames() const;

		/// <summary>
		/// Use shader for rendering on the given window.
		/// </summary>
		/// <param name="window">Window to use the shader for.</param>
		void Use(Window* window) override;
		/// <summary>
		/// Use texture with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the texture with.</param>
		/// <param name="binding">Binding point of the texture.</param>
		/// <param name="texture">Texture to use.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseTexture(uint32_t set, uint32_t binding, TRAP::Graphics::Texture* texture,
		                Window* window) const override;


		//TODO Combined Textures
		/// <summary>
		/// Use multiple textures with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the textures with.</param>
		/// <param name="binding">Binding point of the textures.</param>
		/// <param name="textures">Textures to use.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseTextures(uint32_t set, uint32_t binding,
						 const std::vector<TRAP::Graphics::Texture*>& textures,
						 Window* window) const override;
		/// <summary>
		/// Use sampler with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the sampler with.</param>
		/// <param name="binding">Binding point of the sampler.</param>
		/// <param name="sampler">Sampler to use.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseSampler(uint32_t set, uint32_t binding, TRAP::Graphics::Sampler* sampler,
		                Window* window) const override;
		/// <summary>
		/// Use multiple samplers with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the samplers with.</param>
		/// <param name="binding">Binding point of the samplers.</param>
		/// <param name="samplers">Samplers to use.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseSamplers(uint32_t set, uint32_t binding,
		                 const std::vector<TRAP::Graphics::Sampler*>& samplers,
						 Window* window) const override;
		/// <summary>
		/// Use uniform buffer object with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the UBO with.</param>
		/// <param name="binding">Binding point of the UBO.</param>
		/// <param name="uniformBuffer">Uniform buffer to use.</param>
		/// <param name="size">Size of the UBO.</param>
		/// <param name="offset">Offset of the UBO.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseUBO(uint32_t set, uint32_t binding, TRAP::Graphics::UniformBuffer* uniformBuffer,
		            uint64_t size, uint64_t offset, Window* window) const override;
		/// <summary>
		/// Use shader storage buffer object with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the SSBO with.</param>
		/// <param name="binding">Binding point of the SSBO.</param>
		/// <param name="storageBuffer">Storage buffer to use.</param>
		/// <param name="size">Size of the SSBO.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseSSBO(uint32_t set, uint32_t binding, TRAP::Graphics::StorageBuffer* storageBuffer,
		             uint64_t size, Window* window) const override;

		/// <summary>
		/// Retrieve the shaders thread count per work group.
		/// </summary>
		/// <returns>Shaders thread count per work group.</returns>
		const std::array<uint32_t, 3>& GetNumThreadsPerGroup() const override;

	protected:
		/// <summary>
		/// Initialize API dependent shader.
		/// </summary>
		/// <param name="desc">Binary shader description.</param>
		void Init(const RendererAPI::BinaryShaderDesc& desc) override;
		/// <summary>
		/// Shutdown API dependent shader.
		/// </summary>
		void Shutdown() override;

	private:
		/// <summary>
		/// Set a name for the shader stage.
		/// </summary>
		/// <param name="name">Name for the shader stage.</param>
		/// <param name="stage">Shader stage to name.</param>
		void SetShaderStageName(std::string_view name, VkShaderModule stage) const;

		/// <summary>
		/// Use a buffer object with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the buffer with.</param>
		/// <param name="binding">Binding point of the buffer.</param>
		/// <param name="buffer">Buffer to use.</param>
		/// <param name="size">Size of the buffer.</param>
		/// <param name="offset">Offset into the buffer to start at.</param>
		/// <param name="window">Window to use the buffer for.
		void UseBuffer(uint32_t set, uint32_t binding, TRAP::Graphics::Buffer* buffer,
		               uint64_t size, uint64_t offset, Window* window) const;

		/// <summary>
		///	Retrieve a descriptor's name via its set, binding, descriptor type and size.
		/// </summary>
		/// <param name="set">Descriptor set used by the descriptor.</param>
		/// <param name="binding">Binding point used by the descriptor.</param>
		/// <param name="type">Descriptor type of the descriptor.</param>
		/// <param name="size">Size of the descriptor.</param>
		/// <returns>Descriptor's name if found, empty string otherwise.</returns>
		std::string RetrieveDescriptorName(uint32_t set, uint32_t binding, RendererAPI::DescriptorType type, bool* outUAV = nullptr, uint64_t size = 1) const;

		TRAP::Ref<VulkanDevice> m_device;

		std::array<uint32_t, 3> m_numThreadsPerGroup;

		std::vector<VkShaderModule> m_shaderModules;
		TRAP::Ref<ShaderReflection::PipelineReflection> m_reflection;
		std::vector<std::string> m_entryNames;
	};
}

#endif /*TRAP_VULKANSHADER_H*/
#ifndef TRAP_VULKANSHADER_H
#define TRAP_VULKANSHADER_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/ShaderReflection.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
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
		VulkanShader(const VulkanShader&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanShader& operator=(const VulkanShader&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanShader(VulkanShader&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanShader& operator=(VulkanShader&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan shader module handles of each contained shader stage.
		/// </summary>
		/// <returns>Vulkan shader module handles.</returns>
		[[nodiscard]] constexpr const std::vector<VkShaderModule>& GetVkShaderModules() const noexcept;
		/// <summary>
		/// Retrieve the reflection data of each contained shader stage.
		/// </summary>
		/// <returns>Shader reflection data.</returns>
		[[nodiscard]] TRAP::Ref<ShaderReflection::PipelineReflection> GetReflection() const noexcept;
		/// <summary>
		/// Retrieve the entry point names used by each contained shader stage.
		/// </summary>
		/// <returns>Entry point names.</returns>
		[[nodiscard]] constexpr const std::vector<std::string>& GetEntryNames() const noexcept;

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use shader for rendering on the given window.
		/// </summary>
		/// <param name="window">Window to use the shader for.</param>
		void Use(const Window* window) override;
#else
		/// <summary>
		/// Use shader for rendering.
		/// </summary>
		void Use() override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use texture with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the texture with.</param>
		/// <param name="binding">Binding point of the texture.</param>
		/// <param name="texture">Texture to use.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseTexture(u32 set, u32 binding, Ref<TRAP::Graphics::Texture> texture,
		                const Window* window) const override;
#else
		/// <summary>
		/// Use texture with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the texture with.</param>
		/// <param name="binding">Binding point of the texture.</param>
		/// <param name="texture">Texture to use.</param>
		void UseTexture(u32 set, u32 binding, Ref<TRAP::Graphics::Texture> texture) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use multiple textures with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the textures with.</param>
		/// <param name="binding">Binding point of the textures.</param>
		/// <param name="textures">Textures to use.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseTextures(u32 set, u32 binding,
						 const std::vector<Ref<TRAP::Graphics::Texture>>& textures,
						 const Window* window) const override;
#else
		/// <summary>
		/// Use multiple textures with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the textures with.</param>
		/// <param name="binding">Binding point of the textures.</param>
		/// <param name="textures">Textures to use.</param>
		void UseTextures(u32 set, u32 binding,
						 const std::vector<Ref<TRAP::Graphics::Texture>>& textures) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use sampler with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the sampler with.</param>
		/// <param name="binding">Binding point of the sampler.</param>
		/// <param name="sampler">Sampler to use.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseSampler(u32 set, u32 binding, TRAP::Graphics::Sampler* sampler,
		                const Window* window) const override;
#else
		/// <summary>
		/// Use sampler with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the sampler with.</param>
		/// <param name="binding">Binding point of the sampler.</param>
		/// <param name="sampler">Sampler to use.</param>
		void UseSampler(u32 set, u32 binding, TRAP::Graphics::Sampler* sampler) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use multiple samplers with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the samplers with.</param>
		/// <param name="binding">Binding point of the samplers.</param>
		/// <param name="samplers">Samplers to use.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseSamplers(u32 set, u32 binding,
		                 const std::vector<TRAP::Graphics::Sampler*>& samplers,
						 const Window* window) const override;
#else
		/// <summary>
		/// Use multiple samplers with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the samplers with.</param>
		/// <param name="binding">Binding point of the samplers.</param>
		/// <param name="samplers">Samplers to use.</param>
		void UseSamplers(u32 set, u32 binding,
		                 const std::vector<TRAP::Graphics::Sampler*>& samplers) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use uniform buffer object with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the UBO with.</param>
		/// <param name="binding">Binding point of the UBO.</param>
		/// <param name="uniformBuffer">Uniform buffer to use.</param>
		/// <param name="size">Size of the UBO.</param>
		/// <param name="offset">Offset of the UBO.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseUBO(u32 set, u32 binding, const TRAP::Graphics::UniformBuffer* constuniformBuffer,
		            u64 size, u64 offset, const Window* window) const override;
#else
		/// <summary>
		/// Use uniform buffer object with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the UBO with.</param>
		/// <param name="binding">Binding point of the UBO.</param>
		/// <param name="uniformBuffer">Uniform buffer to use.</param>
		/// <param name="size">Size of the UBO.</param>
		/// <param name="offset">Offset of the UBO.</param>
		void UseUBO(u32 set, u32 binding, const TRAP::Graphics::UniformBuffer* constuniformBuffer,
		            u64 size, u64 offset) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use shader storage buffer object with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the SSBO with.</param>
		/// <param name="binding">Binding point of the SSBO.</param>
		/// <param name="storageBuffer">Storage buffer to use.</param>
		/// <param name="size">Size of the SSBO.</param>
		/// <param name="window">Window to use the shader for.</param>
		void UseSSBO(u32 set, u32 binding, const TRAP::Graphics::StorageBuffer* storageBuffer,
		             u64 size, const Window* window) const override;
#else
		/// <summary>
		/// Use shader storage buffer object with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the SSBO with.</param>
		/// <param name="binding">Binding point of the SSBO.</param>
		/// <param name="storageBuffer">Storage buffer to use.</param>
		/// <param name="size">Size of the SSBO.</param>
		void UseSSBO(u32 set, u32 binding, const TRAP::Graphics::StorageBuffer* storageBuffer,
		             u64 size) const override;
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Retrieve the shaders thread count per work group.
		/// </summary>
		/// <returns>Shaders thread count per work group.</returns>
		[[nodiscard]] constexpr std::array<u32, 3> GetNumThreadsPerGroup() const noexcept override;

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

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use a buffer object with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the buffer with.</param>
		/// <param name="binding">Binding point of the buffer.</param>
		/// <param name="buffer">Buffer to use.</param>
		/// <param name="size">Size of the buffer.</param>
		/// <param name="offset">Offset into the buffer to start at.</param>
		/// <param name="window">Window to use the buffer for.</param>
		void UseBuffer(u32 set, u32 binding, TRAP::Graphics::Buffer* buffer,
		               u64 size, u64 offset, const Window* window) const;
#else
		/// <summary>
		/// Use a buffer object with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the buffer with.</param>
		/// <param name="binding">Binding point of the buffer.</param>
		/// <param name="buffer">Buffer to use.</param>
		/// <param name="size">Size of the buffer.</param>
		/// <param name="offset">Offset into the buffer to start at.</param>
		void UseBuffer(u32 set, u32 binding, TRAP::Graphics::Buffer* buffer,
		               u64 size, u64 offset) const;
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		///	Retrieve a descriptor's name via its set, binding, descriptor type and size.
		/// </summary>
		/// <param name="set">Descriptor set used by the descriptor.</param>
		/// <param name="binding">Binding point used by the descriptor.</param>
		/// <param name="type">Descriptor type of the descriptor.</param>
		/// <param name="size">Size of the descriptor.</param>
		/// <returns>Descriptor's name if found, empty string otherwise.</returns>
		[[nodiscard]] std::string RetrieveDescriptorName(u32 set, u32 binding, RendererAPI::DescriptorType type, bool* outUAV = nullptr, u64 size = 1) const;

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		std::array<u32, 3> m_numThreadsPerGroup{};

		std::vector<VkShaderModule> m_shaderModules{};
		TRAP::Ref<ShaderReflection::PipelineReflection> m_reflection = nullptr;
		std::vector<std::string> m_entryNames{};

		std::array<std::vector<TRAP::Scope<DescriptorSet>>, RendererAPI::ImageCount> m_dirtyDescriptorSets{};
		std::array<std::vector<TRAP::Scope<DescriptorSet>>, RendererAPI::ImageCount> m_cleanedDescriptorSets{};
		u32 m_lastImageIndex = std::numeric_limits<u32>::max();
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<VkShaderModule>& TRAP::Graphics::API::VulkanShader::GetVkShaderModules() const noexcept
{
	return m_shaderModules;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<std::string>& TRAP::Graphics::API::VulkanShader::GetEntryNames() const noexcept
{
	return m_entryNames;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::array<u32, 3> TRAP::Graphics::API::VulkanShader::GetNumThreadsPerGroup() const noexcept
{
	return m_numThreadsPerGroup;
}

#endif /*TRAP_VULKANSHADER_H*/
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
		/// @brief Constructor.
		/// @param name Name for the shader.
		/// @param filepath Filepath of the shader.
		/// @param desc Binary shader description.
		/// @param userMacros Optional user defined macros. Default: nullptr.
		/// @param valid Whether the shader is valid or not. Default: true.
		VulkanShader(std::string name, std::filesystem::path filepath, const RendererAPI::BinaryShaderDesc& desc,
		             const std::vector<Macro>* userMacros = nullptr, bool valid = true);
		/// @brief Constructor.
		/// @param name Name for the shader.
		/// @param desc Binary shader description.
		/// @param userMacros Optional user defined macros. Default: nullptr.
		/// @param valid Whether the shader is valid or not. Default: true.
		VulkanShader(std::string name, const RendererAPI::BinaryShaderDesc& desc,
		             const std::vector<Macro>* userMacros = nullptr, bool valid = true);
		/// @brief Constructor. Creates an invalid placeholder shader.
		/// @param name Name for the shader.
		/// @param filepath Filepath of the shader.
		/// @param userMacros Optional user defined macros. Default: nullptr.
		/// @param stages Optional Stages of the shader. Default: None.
		/// @note Used for invalid shaders, this doesn't create a usable shader.
		VulkanShader(std::string name, std::filesystem::path filepath,
		             const std::vector<Macro>* userMacros = nullptr,
					 RendererAPI::ShaderStage stages = RendererAPI::ShaderStage::None);
		/// @brief Destructor.
		~VulkanShader() override;

		/// @brief Copy constructor.
		consteval VulkanShader(const VulkanShader&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanShader& operator=(const VulkanShader&) noexcept = delete;
		/// @brief Move constructor.
		VulkanShader(VulkanShader&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanShader& operator=(VulkanShader&&) noexcept = default;

		/// @brief Retrieve the Vulkan shader module handles of each contained shader stage.
		/// @return Vulkan shader module handles.
		[[nodiscard]] constexpr const std::vector<VkShaderModule>& GetVkShaderModules() const noexcept;
		/// @brief Retrieve the reflection data of each contained shader stage.
		/// @return Shader reflection data.
		[[nodiscard]] TRAP::Ref<ShaderReflection::PipelineReflection> GetReflection() const noexcept;
		/// @brief Retrieve the entry point names used by each contained shader stage.
		/// @return Entry point names.
		[[nodiscard]] constexpr const std::vector<std::string>& GetEntryNames() const noexcept;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use shader for rendering on the given window.
		/// @param window Window to use the shader for.
		/// @remark @headless This function is not available in headless mode.
		void Use(const Window* window) override;
#else
		/// @brief Use shader for rendering.
		/// @remark This function is only available in headless mode.
		void Use() override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Use texture with this shader on the given window.
		/// @param set Descriptor set to use the texture with.
		/// @param binding Binding point of the texture.
		/// @param texture Texture to use.
		/// @param window Window to use the shader for.
		/// @remark @headless This function is not available in headless mode.
		void UseTexture(u32 set, u32 binding, Ref<TRAP::Graphics::Texture> texture,
		                const Window* window) const override;
#else
		/// @brief Use texture with this shader.
		/// @param set Descriptor set to use the texture with.
		/// @param binding Binding point of the texture.
		/// @param texture Texture to use.
		/// @remark This function is only available in headless mode.
		void UseTexture(u32 set, u32 binding, Ref<TRAP::Graphics::Texture> texture) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use multiple textures with this shader on the given window.
		/// @param set Descriptor set to use the textures with.
		/// @param binding Binding point of the textures.
		/// @param textures Textures to use.
		/// @param window Window to use the shader for.
		/// @remark @headless This function is not available in headless mode.
		void UseTextures(u32 set, u32 binding,
						 const std::vector<Ref<TRAP::Graphics::Texture>>& textures,
						 const Window* window) const override;
#else
		/// @brief Use multiple textures with this shader.
		/// @param set Descriptor set to use the textures with.
		/// @param binding Binding point of the textures.
		/// @param textures Textures to use.
		/// @remark This function is only available in headless mode.
		void UseTextures(u32 set, u32 binding,
						 const std::vector<Ref<TRAP::Graphics::Texture>>& textures) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use sampler with this shader on the given window.
		/// @param set Descriptor set to use the sampler with.
		/// @param binding Binding point of the sampler.
		/// @param sampler Sampler to use.
		/// @param window Window to use the shader for.
		/// @remark @headless This function is not available in headless mode.
		void UseSampler(u32 set, u32 binding, TRAP::Graphics::Sampler* sampler,
		                const Window* window) const override;
#else
		/// @brief Use sampler with this shader.
		/// @param set Descriptor set to use the sampler with.
		/// @param binding Binding point of the sampler.
		/// @param sampler Sampler to use.
		/// @remark This function is only available in headless mode.
		void UseSampler(u32 set, u32 binding, TRAP::Graphics::Sampler* sampler) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Use multiple samplers with this shader on the given window.
		/// @param set Descriptor set to use the samplers with.
		/// @param binding Binding point of the samplers.
		/// @param samplers Samplers to use.
		/// @param window Window to use the shader for.
		/// @remark @headless This function is not available in headless mode.
		void UseSamplers(u32 set, u32 binding,
		                 const std::vector<TRAP::Graphics::Sampler*>& samplers,
						 const Window* window) const override;
#else
		/// @brief Use multiple samplers with this shader.
		/// @param set Descriptor set to use the samplers with.
		/// @param binding Binding point of the samplers.
		/// @param samplers Samplers to use.
		/// @remark This function is only available in headless mode.
		void UseSamplers(u32 set, u32 binding,
		                 const std::vector<TRAP::Graphics::Sampler*>& samplers) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use uniform buffer object with this shader on the given window.
		/// @param set Descriptor set to use the UBO with.
		/// @param binding Binding point of the UBO.
		/// @param uniformBuffer Uniform buffer to use.
		/// @param size Size of the UBO.
		/// @param offset Offset of the UBO.
		/// @param window Window to use the shader for.
		/// @remark @headless This function is not available in headless mode.
		void UseUBO(u32 set, u32 binding, const TRAP::Graphics::UniformBuffer* const uniformBuffer,
		            u64 size, u64 offset, const Window* window) const override;
#else
		/// @brief Use uniform buffer object with this shader.
		/// @param set Descriptor set to use the UBO with.
		/// @param binding Binding point of the UBO.
		/// @param uniformBuffer Uniform buffer to use.
		/// @param size Size of the UBO.
		/// @param offset Offset of the UBO.
		/// @remark This function is only available in headless mode.
		void UseUBO(u32 set, u32 binding, const TRAP::Graphics::UniformBuffer* const uniformBuffer,
		            u64 size, u64 offset) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use shader storage buffer object with this shader on the given window.
		/// @param set Descriptor set to use the SSBO with.
		/// @param binding Binding point of the SSBO.
		/// @param storageBuffer Storage buffer to use.
		/// @param size Size of the SSBO.
		/// @param window Window to use the shader for.
		/// @remark @headless This function is not available in headless mode.
		void UseSSBO(u32 set, u32 binding, const TRAP::Graphics::StorageBuffer* storageBuffer,
		             u64 size, const Window* window) const override;
#else
		/// @brief Use shader storage buffer object with this shader.
		/// @param set Descriptor set to use the SSBO with.
		/// @param binding Binding point of the SSBO.
		/// @param storageBuffer Storage buffer to use.
		/// @param size Size of the SSBO.
		/// @remark This function is only available in headless mode.
		void UseSSBO(u32 set, u32 binding, const TRAP::Graphics::StorageBuffer* storageBuffer,
		             u64 size) const override;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Retrieve the shaders thread count per work group.
		/// @return Shaders thread count per work group.
		[[nodiscard]] constexpr std::array<u32, 3> GetNumThreadsPerGroup() const noexcept override;

	protected:
		/// @brief Initialize API dependent shader.
		/// @param desc Binary shader description.
		void Init(const RendererAPI::BinaryShaderDesc& desc) override;
		/// @brief Shutdown API dependent shader.
		void Shutdown() override;

	private:
		/// @brief Set a name for the shader stage.
		/// @param name Name for the shader stage.
		/// @param stage Shader stage to name.
		void SetShaderStageName(std::string_view name, VkShaderModule stage) const;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use a buffer object with this shader on the given window.
		/// @param set Descriptor set to use the buffer with.
		/// @param binding Binding point of the buffer.
		/// @param buffer Buffer to use.
		/// @param size Size of the buffer.
		/// @param offset Offset into the buffer to start at.
		/// @param window Window to use the buffer for.
		/// @remark @headless This function is not available in headless mode.
		void UseBuffer(u32 set, u32 binding, TRAP::Graphics::Buffer* buffer,
		               u64 size, u64 offset, const Window* window) const;
#else
		/// @brief Use a buffer object with this shader.
		/// @param set Descriptor set to use the buffer with.
		/// @param binding Binding point of the buffer.
		/// @param buffer Buffer to use.
		/// @param size Size of the buffer.
		/// @param offset Offset into the buffer to start at.
		/// @remark This function is only available in headless mode.
		void UseBuffer(u32 set, u32 binding, TRAP::Graphics::Buffer* buffer,
		               u64 size, u64 offset) const;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief </summary>
		/// @param set Descriptor set used by the descriptor.
		/// @param binding Binding point used by the descriptor.
		/// @param type Descriptor type of the descriptor.
		/// @param outUAV Out: Descriptor has UAV.
		/// @param size Size of the descriptor.
		/// @return Descriptor's name if found, empty string otherwise.
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
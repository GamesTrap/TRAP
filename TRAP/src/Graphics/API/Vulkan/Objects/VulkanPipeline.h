#ifndef TRAP_VULKANPIPELINE_H
#define TRAP_VULKANPIPELINE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Pipeline.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanPipeline final : public Pipeline
	{
	public:
		/// @brief Constructor.
		/// @param desc Pipeline description
		explicit VulkanPipeline(const RendererAPI::PipelineDesc& desc);
		/// @brief Destructor.
		~VulkanPipeline() override;

		/// @brief Copy constructor.
		consteval VulkanPipeline(const VulkanPipeline&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanPipeline& operator=(const VulkanPipeline&) noexcept = delete;
		/// @brief Move constructor.
		VulkanPipeline(VulkanPipeline&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanPipeline& operator=(VulkanPipeline&&) noexcept = default;

		/// @brief Retrieve the Vulkan pipeline handle.
		/// @return Vulkan pipeline handle
		[[nodiscard]] constexpr VkPipeline GetVkPipeline() const noexcept;
		/// @brief Retrieve the pipeline type.
		/// @return Pipeline type
		[[nodiscard]] constexpr RendererAPI::PipelineType GetPipelineType() const noexcept;

	private:
		/// @brief Initialize compute pipeline.
		/// @param desc Pipeline description
		void InitComputePipeline(const RendererAPI::PipelineDesc& desc);
		/// @brief Initialize graphics pipeline.
		/// @param desc Pipeline description
		void InitGraphicsPipeline(const RendererAPI::PipelineDesc& desc);

		VkPipeline m_vkPipeline = VK_NULL_HANDLE;
		RendererAPI::PipelineType m_type;
		//In DX12 this information is stored in ID3D12StateObject.
		//But for Vulkan we need to store it manually.
		std::vector<std::string> m_shaderStageNames{};

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipeline TRAP::Graphics::API::VulkanPipeline::GetVkPipeline() const noexcept
{
	return m_vkPipeline;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::PipelineType TRAP::Graphics::API::VulkanPipeline::GetPipelineType() const noexcept
{
	return m_type;
}

#endif /*TRAP_VULKANPIPELINE_H*/

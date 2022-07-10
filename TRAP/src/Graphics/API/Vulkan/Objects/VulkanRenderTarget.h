#ifndef TRAP_VULKANRENDERTARGET_H
#define TRAP_VULKANRENDERTARGET_H

#include "VulkanCommandBuffer.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/RenderTarget.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanDevice;

	class VulkanRenderTarget final : public RenderTarget
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Render target description.</param>
		explicit VulkanRenderTarget(const RendererAPI::RenderTargetDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanRenderTarget() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanRenderTarget(const VulkanRenderTarget&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanRenderTarget& operator=(const VulkanRenderTarget&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanRenderTarget(VulkanRenderTarget&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanRenderTarget& operator=(VulkanRenderTarget&&) = default;

		/// <summary>
		/// Retrieve the render target's Vulkan image view.
		/// </summary>
		/// <returns>Vulkan image view.</returns>
		VkImageView GetVkImageView() const;
		/// <summary>
		/// Retrieve the render target's Vulkan image view slices.
		/// </summary>
		/// <returns>Vulkan image view slices.</returns>
		const std::vector<VkImageView>& GetVkImageViewSlices() const;
		/// <summary>
		/// Retrieve the render target's ID.
		/// </summary>
		/// <returns>Render target ID.</returns>
		uint32_t GetID() const;

	private:
		friend void TRAP::Graphics::API::VulkanCommandBuffer::ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>* bufferBarriers,
			                                                                  const std::vector<RendererAPI::TextureBarrier>* textureBarriers,
			                                                                  const std::vector<RendererAPI::RenderTargetBarrier>* renderTargetBarriers) const;
		friend void TRAP::Graphics::API::VulkanCommandBuffer::ResourceBarrier(const RendererAPI::BufferBarrier* bufferBarrier,
			                                                                  const RendererAPI::TextureBarrier* textureBarrier,
			                                                                  const RendererAPI::RenderTargetBarrier* renderTargetBarrier) const;
		friend void TRAP::Graphics::API::VulkanCommandBuffer::BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
			                                                                    const TRAP::Ref<RenderTarget>& depthStencil,
			                                                                    const RendererAPI::LoadActionsDesc* loadActions,
			                                                                    std::vector<uint32_t>* colorArraySlices,
			                                                                    std::vector<uint32_t>* colorMipSlices,
			                                                                    uint32_t depthArraySlice,
			                                                                    uint32_t depthMipSlice);

		/// <summary>
		/// Set the name of the render target.
		/// </summary>
		/// <param name="name">Name to use.</param>
		void SetRenderTargetName(const std::string& name) const;

		TRAP::Ref<VulkanDevice> m_device;

		VkImageView m_vkDescriptor;
		std::vector<VkImageView> m_vkSliceDescriptors;
		volatile uint32_t m_used;

		uint32_t m_ID;

		static std::atomic<int32_t> s_RenderTargetIDs;
	};
}

#endif /*TRAP_VULKANRENDERTARGET_H*/
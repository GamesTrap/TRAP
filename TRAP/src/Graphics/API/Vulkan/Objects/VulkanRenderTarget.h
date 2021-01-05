#ifndef _TRAP_VULKANRENDERTARGET_H_
#define _TRAP_VULKANRENDERTARGET_H_

#include "VulkanCommandBuffer.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/RenderTarget.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanTexture;
	class VulkanDevice;
	
	class VulkanRenderTarget final : public RenderTarget
	{
	public:
		explicit VulkanRenderTarget(const RendererAPI::RenderTargetDesc& desc);
		~VulkanRenderTarget();

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		uint32_t GetDepth() const override;
		uint32_t GetArraySize() const override;
		uint32_t GetMipLevels() const override;
		RendererAPI::SampleCount GetSampleCount() const override;
		uint32_t GetSampleQuality() const override;
		RendererAPI::ImageFormat GetImageFormat() const override;
		RendererAPI::ClearValue GetClearValue() const override;
		RendererAPI::DescriptorType GetDescriptorType() const override;
		
		VkImageView& GetVkImageView();
		const std::vector<VkImageView>& GetVkImageViewSlices() const;
		uint32_t GetID() const;

	private:
		friend void TRAP::Graphics::API::VulkanCommandBuffer::ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
			const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
			const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const;
		friend void TRAP::Graphics::API::VulkanCommandBuffer::BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
			const TRAP::Ref<RenderTarget>& depthStencil,
			const RendererAPI::LoadActionsDesc* loadActions,
			const std::vector<uint32_t>& colorArraySlices,
			const std::vector<uint32_t>& colorMipSlices,
			uint32_t depthArraySlice,
			uint32_t depthMipSlice);
		
		void SetRenderTargetName(const char* name) const;

		TRAP::Ref<VulkanDevice> m_device;

		VkImageView m_vkDescriptor;
		std::vector<VkImageView> m_vkSliceDescriptors;
		volatile uint32_t m_used;
		
		uint32_t m_ID;
		
		static std::atomic<int32_t> s_RenderTargetIDs;
	};
}

#endif /*_TRAP_VULKANRENDERTARGET_H_*/
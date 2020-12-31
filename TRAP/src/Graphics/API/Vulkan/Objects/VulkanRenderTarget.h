#ifndef _TRAP_VULKANRENDERTARGET_H_
#define _TRAP_VULKANRENDERTARGET_H_

#include "VulkanCommandBuffer.h"
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanTexture;
	class VulkanDevice;
	
	class VulkanRenderTarget
	{
	public:
		VulkanRenderTarget(TRAP::Ref<VulkanDevice> device,
			const RendererAPI::RenderTargetDesc& desc,
			const TRAP::Ref<VulkanMemoryAllocator>& vma);
		~VulkanRenderTarget();

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetArraySize() const;
		uint32_t GetMipLevels() const;
		RendererAPI::SampleCount GetSampleCount() const;
		uint32_t GetSampleQuality() const;
		RendererAPI::ImageFormat GetImageFormat() const;
		RendererAPI::ClearValue GetClearValue() const;
		RendererAPI::DescriptorType GetDescriptorType() const;
		VkImageView& GetVkImageView();
		const std::vector<VkImageView>& GetVkImageViewSlices() const;
		uint32_t GetID() const;

	private:
		friend void TRAP::Graphics::API::VulkanCommandBuffer::ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
			const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
			const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const;
		friend void TRAP::Graphics::API::VulkanCommandBuffer::BindRenderTargets(const std::vector<TRAP::Ref<VulkanRenderTarget>>& renderTargets,
			const TRAP::Ref<VulkanRenderTarget>& depthStencil,
			const RendererAPI::LoadActionsDesc* loadActions,
			const std::vector<uint32_t>& colorArraySlices,
			const std::vector<uint32_t>& colorMipSlices,
			uint32_t depthArraySlice,
			uint32_t depthMipSlice);
		
		void SetRenderTargetName(const char* name) const;
		
		TRAP::Ref<VulkanDevice> m_device;

		TRAP::Ref<VulkanTexture> m_texture;

		VkImageView m_vkDescriptor;
		std::vector<VkImageView> m_vkSliceDescriptors;
		volatile uint32_t m_used;

		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_depth;
		uint32_t m_arraySize;
		uint32_t m_mipLevels;
		RendererAPI::SampleCount m_sampleCount;
		uint32_t m_sampleQuality;
		RendererAPI::ImageFormat m_format;
		RendererAPI::ClearValue m_clearValue;
		RendererAPI::DescriptorType m_descriptors;
		
		uint32_t m_ID;
		
		static std::atomic_int32_t s_RenderTargetIDs;
	};
}

#endif /*_TRAP_VULKANRENDERTARGET_H_*/
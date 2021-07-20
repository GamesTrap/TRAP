#ifndef _TRAP_VULKANTEXTURE_H_
#define _TRAP_VULKANTEXTURE_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Utils/VMA.h"


namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanDevice;

	class VulkanTexture
	{
	public:
		VulkanTexture(TRAP::Ref<VulkanDevice> device, const RendererAPI::TextureDesc& desc,
			          TRAP::Ref<VulkanMemoryAllocator> vma);
		~VulkanTexture();

		VkImageView& GetSRVVkImageView();
		VkImageView& GetSRVStencilVkImageView();
		const std::vector<VkImageView>& GetUAVVkImageViews() const;
		VkImage& GetVkImage();
		VmaAllocation& GetVMAAllocation();
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetMipLevels() const;
		uint32_t GetArraySizeMinusOne() const;
		TRAP::Graphics::API::ImageFormat GetImageFormat() const;
		uint32_t GetAspectMask() const;
		RendererAPI::DescriptorType GetUAV() const;
		bool OwnsImage() const;

		void SetTextureName(const std::string& name) const;

	private:
		static uint32_t GetMemoryType(uint32_t typeBits, const VkPhysicalDeviceMemoryProperties& memProps,
		                              VkMemoryPropertyFlags props, VkBool32* memTypeFound = nullptr);

		TRAP::Ref<VulkanDevice> m_device;
		TRAP::Ref<VulkanMemoryAllocator> m_vma;

		//Opaque handle used by shaders for doing read/write operations on the texture
		VkImageView m_vkSRVDescriptor;
		//Opaque handle used by shaders for doing read/write operations on the texture
		std::vector<VkImageView> m_vkUAVDescriptors;
		//Opaque handle used by shaders for doing read/write operations on the texture
		VkImageView m_vkSRVStencilDescriptor;
		//Native handle of the underlying resource
		VkImage m_vkImage;
		//Contains resource allocation info such as parent heap, offset in heap
		VmaAllocation m_vkAllocation;
		VkDeviceMemory m_vkDeviceMemory;

		//Current state of the buffer
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_depth;
		uint32_t m_mipLevels;
		uint32_t m_arraySizeMinusOne;
		TRAP::Graphics::API::ImageFormat m_format;
		//Flags specifying which aspects (Color, Depth, Stencil) are included in the m_vkImageView
		uint32_t m_aspectMask;
		RendererAPI::DescriptorType m_UAV;
		//This value will be false if the underlying resource is not owned by the texture (swapchain textures,...)
		bool m_ownsImage;
	};
}

#endif /*_TRAP_VULKANTEXTURE_H_*/
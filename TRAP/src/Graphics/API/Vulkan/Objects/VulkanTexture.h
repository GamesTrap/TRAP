#ifndef TRAP_VULKANTEXTURE_H
#define TRAP_VULKANTEXTURE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Utils/VMA.h"
#include "Graphics/Textures/TextureBase.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanDevice;

	class VulkanTexture : public TextureBase
	{
	public:
		VulkanTexture(TRAP::Ref<VulkanDevice> device, const RendererAPI::TextureDesc& desc,
			          TRAP::Ref<VulkanMemoryAllocator> vma);
		~VulkanTexture() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanTexture(const VulkanTexture&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanTexture& operator=(const VulkanTexture&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanTexture(VulkanTexture&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanTexture& operator=(VulkanTexture&&) = default;

		VkImageView GetSRVVkImageView() const;
		VkImageView GetSRVStencilVkImageView() const;
		const std::vector<VkImageView>& GetUAVVkImageViews() const;
		VkImage GetVkImage();
		VmaAllocation GetVMAAllocation() const;

		void SetTextureName(const std::string_view name) const override;

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
	};
}

#endif /*TRAP_VULKANTEXTURE_H*/
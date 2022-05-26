#ifndef TRAP_VULKANTEXTURE_H
#define TRAP_VULKANTEXTURE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/Textures/TextureBase.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanDevice;

	class VulkanTexture : public TextureBase
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="device">Vulkan device.</param>
		/// <param name="desc">Texture description.</param>
		/// <param name="vma">Vulkan memory allocator.</param>
		VulkanTexture(TRAP::Ref<VulkanDevice> device, const RendererAPI::TextureDesc& desc,
			          TRAP::Ref<VulkanMemoryAllocator> vma);
		/// <summary>
		/// Destructor.
		/// </summary>
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

		/// <summary>
		/// Retrieve the read only color Vulkan image view handle of the texture.
		/// </summary>
		/// <returns>Vulkan image view handle.</returns>
		VkImageView GetSRVVkImageView() const;
		/// <summary>
		/// Retrieve the read only  stencil Vulkan image view handle of the texture.
		/// </summary>
		/// <returns>Vulkan image view handle.</returns>
		VkImageView GetSRVStencilVkImageView() const;
		/// <summary>
		/// Retrieve all read-writable Vulkan image view handles of the texture.
		/// </summary>
		/// <returns>
		const std::vector<VkImageView>& GetUAVVkImageViews() const;
		/// <summary>
		/// Retrieve the Vulkan image handle.
		/// </summary>
		/// <returns>Vulkan image handle.</returns>
		VkImage GetVkImage();
		/// <summary>
		/// Retrieve the VMA allocation handle used by the texture.
		/// </summary>
		/// <returns>VMA allocation handle.</returns>
		VmaAllocation GetVMAAllocation() const;

		/// <summary>
		/// Retrieve whether the texture is lazily allocated or not.
		/// </summary>
		/// <returns>True if the texture is lazily allocated, false otherwise.</returns>
		bool IsLazilyAllocated() const;

		/// <summary>
		/// Set the name of the texture.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		void SetTextureName(const std::string_view name) const override;

	private:
		/// <summary>
		/// Retrieve the memory type index via the given parameters.
		/// </summary>
		/// <param name="typeBits">Memory type bits from Vulkan memory requirements.</param>
		/// <param name="memProps">Vulkan physical device memory properties.</param>
		/// <param name="props">Vulkan memory property flags.</param>
		/// <param name="memTypeFound">Output: True if memory type found, false otherwise.</param>
		/// <returns>Index of memory type.</returns>
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

		bool m_lazilyAllocated;
	};
}

#endif /*TRAP_VULKANTEXTURE_H*/
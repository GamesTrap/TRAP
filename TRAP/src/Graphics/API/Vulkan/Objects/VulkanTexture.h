#ifndef TRAP_VULKANTEXTURE_H
#define TRAP_VULKANTEXTURE_H

#include "Graphics/API/Vulkan/Objects/VulkanMemoryAllocator.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Textures/Texture.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanDevice;

	class VulkanTexture final : public Texture
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		VulkanTexture();
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepaths">Filepaths to images used by the texture.</param>
		VulkanTexture(std::string name, std::array<std::filesystem::path, 6> filepaths);
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepath">Filepath to image used by the texture.</param>
		/// <param name="type">Type of texture.</param>
		/// <param name="cubeFormat">Cube format used by the texture. Ignored when using TextureType::Texture2D.</param>
		VulkanTexture(std::string name, std::filesystem::path filepath,
					  TextureType type, TextureCubeFormat cubeFormat);
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="type">Type of texture.</param>
		VulkanTexture(TextureType type);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanTexture() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanTexture(const VulkanTexture&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanTexture& operator=(const VulkanTexture&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanTexture(VulkanTexture&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanTexture& operator=(VulkanTexture&&) noexcept = default;

		/// <summary>
		/// Initialize the Texture.
		/// </summary>
		/// <param name="desc">Texture description.</param>
		void Init(const RendererAPI::TextureDesc& desc) override;

		/// <summary>
		/// Retrieve the read only color Vulkan image view handle of the texture.
		/// </summary>
		/// <returns>Vulkan image view handle.</returns>
		VkImageView GetSRVVkImageView() const noexcept;
		/// <summary>
		/// Retrieve the read only  stencil Vulkan image view handle of the texture.
		/// </summary>
		/// <returns>Vulkan image view handle.</returns>
		VkImageView GetSRVStencilVkImageView() const noexcept;
		/// <summary>
		/// Retrieve all read-writable Vulkan image view handles of the texture.
		/// </summary>
		/// <returns>
		const std::vector<VkImageView>& GetUAVVkImageViews() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan image handle.
		/// </summary>
		/// <returns>Vulkan image handle.</returns>
		VkImage GetVkImage() const noexcept;
		/// <summary>
		/// Retrieve the VMA allocation handle used by the texture.
		/// </summary>
		/// <returns>VMA allocation handle.</returns>
		VmaAllocation GetVMAAllocation() const noexcept;

		/// <summary>
		/// Retrieve whether the texture is lazily allocated or not.
		/// </summary>
		/// <returns>True if the texture is lazily allocated, false otherwise.</returns>
		bool IsLazilyAllocated() const noexcept;

		/// <summary>
		/// Set the name of the texture.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		void SetTextureName(const std::string_view name) const override;

	protected:
		/// <summary>
		/// Shutdown API dependent texture.
		/// </summary>
		void Shutdown() override;

	private:
		/// <summary>
		/// Pre Initialization step run by every constructor.
		/// </summary>
		void PreInit();

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
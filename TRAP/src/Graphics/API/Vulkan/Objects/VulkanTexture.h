#ifndef TRAP_VULKANTEXTURE_H
#define TRAP_VULKANTEXTURE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanMemoryAllocator.h"
#include "Graphics/Textures/Texture.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanDevice;

	class VulkanTexture final : public Texture
	{
	public:
		/// @brief Constructor.
		VulkanTexture();
		/// @brief Constructor.
		/// @param name Name for the texture.
		/// @param filepaths Filepaths to images used by the texture.
		VulkanTexture(std::string name, std::array<std::filesystem::path, 6> filepaths);
		/// @brief Constructor.
		/// @param name Name for the texture.
		/// @param filepath Filepath to image used by the texture.
		/// @param type Type of texture.
		/// @param cubeFormat Cube format used by the texture. Ignored when using TextureType::Texture2D.
		VulkanTexture(std::string name, std::filesystem::path filepath,
					  TextureType type, TextureCubeFormat cubeFormat);
		/// @brief Constructor.
		/// @param type Type of texture.
		explicit VulkanTexture(TextureType type);
		/// @brief Destructor.
		~VulkanTexture() override;

		/// @brief Copy constructor.
		consteval VulkanTexture(const VulkanTexture&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanTexture& operator=(const VulkanTexture&) noexcept = delete;
		/// @brief Move constructor.
		VulkanTexture(VulkanTexture&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanTexture& operator=(VulkanTexture&&) noexcept = default;

		/// @brief Initialize the Texture.
		/// @param desc Texture description.
		void Init(const RendererAPI::TextureDesc& desc) override;

		/// @brief Retrieve the read only color Vulkan image view handle of the texture.
		/// @return Vulkan image view handle.
		[[nodiscard]] constexpr VkImageView GetSRVVkImageView() const noexcept;
		/// @brief Retrieve the read only  stencil Vulkan image view handle of the texture.
		/// @return Vulkan image view handle.
		[[nodiscard]] constexpr VkImageView GetSRVStencilVkImageView() const noexcept;
		/// @brief Retrieve all read-writable Vulkan image view handles of the texture.
		/// @return Vulkan image view handles.
		[[nodiscard]] constexpr const std::vector<VkImageView>& GetUAVVkImageViews() const noexcept;
		/// @brief Retrieve the Vulkan image handle.
		/// @return Vulkan image handle.
		[[nodiscard]] constexpr VkImage GetVkImage() const noexcept;
		/// @brief Retrieve the VMA allocation handle used by the texture.
		/// @return VMA allocation handle.
		[[nodiscard]] constexpr VmaAllocation GetVMAAllocation() const noexcept;

		/// @brief Retrieve whether the texture is lazily allocated or not.
		/// @return True if the texture is lazily allocated, false otherwise.
		[[nodiscard]] constexpr bool IsLazilyAllocated() const noexcept;

#ifdef ENABLE_GRAPHICS_DEBUG
		/// @brief Set the name of the texture.
		/// @param name Name for the texture.
		void SetTextureName(std::string_view name) const override;
#endif /*ENABLE_GRAPHICS_DEBUG*/

	protected:
		/// @brief Shutdown API dependent texture.
		void Shutdown() override;

	private:
		/// @brief Pre Initialization step run by every constructor.
		void PreInit();

		/// @brief Retrieve the memory type index via the given parameters.
		/// @param typeBits Memory type bits from Vulkan memory requirements.
		/// @param memProps Vulkan physical device memory properties.
		/// @param props Vulkan memory property flags.
		/// @param memTypeFound Output: True if memory type found, false otherwise.
		/// @return Index of memory type.
		[[nodiscard]] static u32 GetMemoryType(u32 typeBits, const VkPhysicalDeviceMemoryProperties& memProps,
		                                            VkMemoryPropertyFlags props, VkBool32* memTypeFound = nullptr);

		TRAP::Ref<VulkanDevice> m_device = nullptr;
		TRAP::Ref<VulkanMemoryAllocator> m_vma = nullptr;

		//Opaque handle used by shaders for doing read/write operations on the texture
		VkImageView m_vkSRVDescriptor = VK_NULL_HANDLE;
		//Opaque handle used by shaders for doing read/write operations on the texture
		std::vector<VkImageView> m_vkUAVDescriptors{};
		//Opaque handle used by shaders for doing read/write operations on the texture
		VkImageView m_vkSRVStencilDescriptor = VK_NULL_HANDLE;
		//Native handle of the underlying resource
		VkImage m_vkImage = VK_NULL_HANDLE;
		//Contains resource allocation info such as parent heap, offset in heap
		VmaAllocation m_vkAllocation = VK_NULL_HANDLE;
		VkDeviceMemory m_vkDeviceMemory = VK_NULL_HANDLE;

		bool m_lazilyAllocated = false;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<VkImageView> &TRAP::Graphics::API::VulkanTexture::GetUAVVkImageViews() const noexcept
{
	return m_vkUAVDescriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkImageView TRAP::Graphics::API::VulkanTexture::GetSRVVkImageView() const noexcept
{
	return m_vkSRVDescriptor;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkImageView TRAP::Graphics::API::VulkanTexture::GetSRVStencilVkImageView() const noexcept
{
	return m_vkSRVStencilDescriptor;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkImage TRAP::Graphics::API::VulkanTexture::GetVkImage() const noexcept
{
	return m_vkImage;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VmaAllocation TRAP::Graphics::API::VulkanTexture::GetVMAAllocation() const noexcept
{
	return m_vkAllocation;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::API::VulkanTexture::IsLazilyAllocated() const noexcept
{
	return m_lazilyAllocated;
}

#endif /*TRAP_VULKANTEXTURE_H*/
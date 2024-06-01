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
		/// @param name Name of the texture.
		explicit VulkanTexture(std::string name);
		/// @brief Constructor.
		/// @param name Name of the texture.
		/// @param filePaths File paths of the texture.
		VulkanTexture(std::string name, std::vector<std::filesystem::path> filePaths);
		/// @brief Constructor.
		/// @param name Name of the texture.
		/// @param filePaths File paths of the texture.
		/// @param cubeFormat Format for the cube texture.
		VulkanTexture(std::string name, std::vector<std::filesystem::path> filePaths, const TRAP::Optional<TextureCubeFormat>& cubeFormat);
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
		/// @brief Retrieve the read only stencil Vulkan image view handle of the texture.
		/// @return Vulkan image view handle.
		[[nodiscard]] constexpr VkImageView GetSRVStencilVkImageView() const noexcept;
		/// @brief Retrieve all read-writable Vulkan image view handles of the texture.
		/// @return Vulkan image view handles.
		[[nodiscard]] constexpr const std::vector<VkImageView>& GetUAVVkImageViews() const noexcept;
		/// @brief Retrieve the Vulkan image handle.
		/// @return Vulkan image handle.
		[[nodiscard]] constexpr VkImage GetVkImage() const noexcept;

		/// @brief Retrieve whether the texture is lazily allocated or not.
		/// @return True if the texture is lazily allocated, false otherwise.
		[[nodiscard]] constexpr bool IsLazilyAllocated() const noexcept;

	protected:
		/// @brief Shutdown API dependent texture.
		void Shutdown() override;

	private:
		/// @brief Pre Initialization step run by every constructor.
		void PreInit();

		void CreateVkImage(const TRAP::Graphics::RendererAPI::TextureDesc& desc,
		                   VkImageType imageType, bool cubeMapRequired,
                           TRAP::Graphics::RendererAPI::DescriptorType descriptors);
		void CreateVkImageViews(const TRAP::Graphics::RendererAPI::TextureDesc& desc,
                                VkImageType imageType, bool cubeMapRequired,
                                TRAP::Graphics::RendererAPI::DescriptorType descriptors);

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

[[nodiscard]] constexpr const std::vector<VkImageView>& TRAP::Graphics::API::VulkanTexture::GetUAVVkImageViews() const noexcept
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

[[nodiscard]] constexpr bool TRAP::Graphics::API::VulkanTexture::IsLazilyAllocated() const noexcept
{
	return m_lazilyAllocated;
}

#endif /*TRAP_VULKANTEXTURE_H*/

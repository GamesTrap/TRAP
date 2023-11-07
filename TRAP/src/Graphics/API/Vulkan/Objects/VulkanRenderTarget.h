#ifndef TRAP_VULKANRENDERTARGET_H
#define TRAP_VULKANRENDERTARGET_H

#include "Graphics/API/Vulkan/VulkanRenderer.h"
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
		/// @brief Constructor.
		/// @param desc Render target description.
		explicit VulkanRenderTarget(const RendererAPI::RenderTargetDesc& desc);
		/// @brief Destructor.
		~VulkanRenderTarget() override;

		/// @brief Copy constructor.
		constexpr VulkanRenderTarget(const VulkanRenderTarget&) = delete;
		/// @brief Copy assignment operator.
		constexpr VulkanRenderTarget& operator=(const VulkanRenderTarget&) = delete;
		/// @brief Move constructor.
		VulkanRenderTarget(VulkanRenderTarget&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanRenderTarget& operator=(VulkanRenderTarget&&) noexcept = default;

		/// @brief Retrieve the render target's Vulkan image view.
		/// @return Vulkan image view.
		[[nodiscard]] constexpr VkImageView GetVkImageView() const noexcept;
		/// @brief Retrieve the render target's Vulkan image view slices.
		/// @return Vulkan image view slices.
		[[nodiscard]] constexpr const std::vector<VkImageView>& GetVkImageViewSlices() const noexcept;
		/// @brief Retrieve the render target's ID.
		/// @return Render target ID.
		[[nodiscard]] constexpr u64 GetID() const noexcept;

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
			                                                                    const std::vector<u32>* colorArraySlices,
			                                                                    const std::vector<u32>* colorMipSlices,
			                                                                    u32 depthArraySlice,
			                                                                    u32 depthMipSlice,
							   												    const TRAP::Ref<RenderTarget>& shadingRate);

		/// @brief Set the name of the render target.
		/// @param name Name to use.
		void SetRenderTargetName(std::string_view name) const;

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		VkImageView m_vkDescriptor = VK_NULL_HANDLE;
		std::vector<VkImageView> m_vkSliceDescriptors{};
		volatile u32 m_used = 0;

		u64 m_ID = s_RenderTargetIDs++;

		inline constinit static std::atomic<u64> s_RenderTargetIDs = 1;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkImageView TRAP::Graphics::API::VulkanRenderTarget::GetVkImageView() const noexcept
{
	return m_vkDescriptor;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<VkImageView>& TRAP::Graphics::API::VulkanRenderTarget::GetVkImageViewSlices() const noexcept
{
	return m_vkSliceDescriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::Graphics::API::VulkanRenderTarget::GetID() const noexcept
{
	return m_ID;
}

#endif /*TRAP_VULKANRENDERTARGET_H*/
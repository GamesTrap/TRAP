#ifndef TRAP_VULKANCOMMANDPOOL_H
#define TRAP_VULKANCOMMANDPOOL_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/CommandPool.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanCommandBuffer;
	class VulkanQueue;
	class VulkanDevice;

	class VulkanCommandPool final : public CommandPool
	{
	public:
		/// @brief Constructor.
		/// @param desc Command pool description.
		explicit VulkanCommandPool(const RendererAPI::CommandPoolDesc& desc);
		/// @brief Destructor.
		~VulkanCommandPool() override;

		/// @brief Copy constructor.
		consteval VulkanCommandPool(const VulkanCommandPool&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanCommandPool& operator=(const VulkanCommandPool&) noexcept = delete;
		/// @brief Move constructor.
		VulkanCommandPool(VulkanCommandPool&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanCommandPool& operator=(VulkanCommandPool&&) noexcept = default;

		/// @brief Retrieve the VkCommandPool handle.
		/// @return VkCommandPool handle.
		[[nodiscard]] constexpr VkCommandPool GetVkCommandPool() const noexcept;

		/// @brief Retrieve a new command buffer.
		/// @param secondary Should the command buffer be a secondary command buffer.
		/// @param name Optional: Name for the command buffer.
		/// @return Newly created command buffer.
		/// @note The lifetime of the command buffer ends when the command pool gets
		///       destroyed or when ReleaseCommandBuffer() gets called.
		[[nodiscard]] CommandBuffer& GetCommandBuffer(bool secondary, std::string_view name = "") override;
		/// @brief Release a command buffer
		/// @param cmdBuffer Command buffer to release.
		void ReleaseCommandBuffer(const CommandBuffer& cmdBuffer) override;

		/// @brief Reset the command pool.
		///        All allocated command buffers are reset to their initial state (i.e. valid non-recording).
		void Reset() const override;

	private:
		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		VkCommandPool m_vkCommandPool = VK_NULL_HANDLE;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkCommandPool TRAP::Graphics::API::VulkanCommandPool::GetVkCommandPool() const noexcept
{
	return m_vkCommandPool;
}

#endif /*TRAP_VULKANCOMMANDPOOL_H*/

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

		/// @brief Allocate a new command buffer.
		/// @param secondary Should the command buffer be a secondary command buffer.
		///@return New command buffer.
		[[nodiscard]] CommandBuffer* AllocateCommandBuffer(bool secondary) override;
		/// @brief Free a command buffer
		/// @param cmdBuffer Command buffer to free.
		void FreeCommandBuffer(const CommandBuffer* cmdBuffer) override;

		/// @brief Reset the command pool.
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
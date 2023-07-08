#ifndef TRAP_VULKANCOMMANDPOOL_H
#define TRAP_VULKANCOMMANDPOOL_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/CommandPool.h"

namespace TRAP::Graphics::API
{
	class VulkanCommandBuffer;
	class VulkanQueue;
	class VulkanDevice;

	class VulkanCommandPool final : public CommandPool
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Command pool description.</param>
		explicit VulkanCommandPool(const RendererAPI::CommandPoolDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanCommandPool() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanCommandPool(const VulkanCommandPool&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanCommandPool& operator=(const VulkanCommandPool&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanCommandPool(VulkanCommandPool&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanCommandPool& operator=(VulkanCommandPool&&) noexcept = default;

		/// <summary>
		/// Retrieve the VkCommandPool handle.
		/// </summary>
		/// <returns>VkCommandPool handle.</returns>
		[[nodiscard]] constexpr VkCommandPool GetVkCommandPool() const noexcept;

		/// <summary>
		/// Allocate a new command buffer.
		/// </summary>
		/// <param name="secondary">Should the command buffer be a secondary command buffer.</param>
		///<returns>New command buffer.</returns>
		[[nodiscard]] CommandBuffer* AllocateCommandBuffer(bool secondary) override;
		/// <summary>
		/// Free a command buffer
		/// </summary>
		/// <param name="cmdBuffer">Command buffer to free.</param>
		void FreeCommandBuffer(const CommandBuffer* cmdBuffer) override;

		/// <summary>
		/// Reset the command pool.
		/// </summary>
		void Reset() const override;

	private:
		TRAP::Ref<VulkanDevice> m_device;

		VkCommandPool m_vkCommandPool;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkCommandPool TRAP::Graphics::API::VulkanCommandPool::GetVkCommandPool() const noexcept
{
	return m_vkCommandPool;
}

#endif /*TRAP_VULKANCOMMANDPOOL_H*/
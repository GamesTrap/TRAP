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
		explicit VulkanCommandPool(const RendererAPI::CommandPoolDesc& desc);
		~VulkanCommandPool() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanCommandPool(const VulkanCommandPool&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanCommandPool& operator=(const VulkanCommandPool&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanCommandPool(VulkanCommandPool&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanCommandPool& operator=(VulkanCommandPool&&) = default;

		VkCommandPool GetVkCommandPool() const;

		CommandBuffer* AllocateCommandBuffer(bool secondary) override;
		void FreeCommandBuffer(CommandBuffer* cmdBuffer) override;

		void Reset() const override;

	private:
		TRAP::Ref<VulkanDevice> m_device;

		VkCommandPool m_vkCommandPool;
	};
}

#endif /*TRAP_VULKANCOMMANDPOOL_H*/
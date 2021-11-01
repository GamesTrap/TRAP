#ifndef _TRAP_VULKANCOMMANDPOOL_H_
#define _TRAP_VULKANCOMMANDPOOL_H_

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
		/// Default Copy Constructor.
		/// </summary>
		VulkanCommandPool(const VulkanCommandPool&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VulkanCommandPool& operator=(const VulkanCommandPool&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VulkanCommandPool(VulkanCommandPool&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		VulkanCommandPool& operator=(VulkanCommandPool&&) = default;

		VkCommandPool& GetVkCommandPool();

		CommandBuffer* AllocateCommandBuffer(bool secondary) override;
		void FreeCommandBuffer(CommandBuffer* cmdBuffer) override;

		void Reset() const override;

	private:
		TRAP::Ref<VulkanDevice> m_device;

		VkCommandPool m_vkCommandPool;
	};
}

#endif /*_TRAP_VULKANCOMMANDPOOL_H_*/
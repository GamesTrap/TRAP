#ifndef _TRAP_VULKANCOMMANDPOOL_H_
#define _TRAP_VULKANCOMMANDPOOL_H_

namespace TRAP::Graphics::API::Vulkan
{
	class CommandBuffer;
	class Device;

	class CommandPool
	{
	public:
		CommandPool(const Scope<Device>& device, uint32_t queueFamilyIndex);
		~CommandPool();
		CommandPool(const CommandPool&) = default;
		CommandPool& operator=(const CommandPool&) = default;
		CommandPool(CommandPool&&) = default;
		CommandPool& operator=(CommandPool&&) = default;

		VkCommandPool& GetCommandPool();

		void ResetCommandPool() const;

		Scope<CommandBuffer> AllocateCommandBuffer() const;
		void FreeCommandBuffer(Scope<CommandBuffer> commandBuffer) const;

	private:
		VkCommandPool m_commandPool;
		Device* m_device;
	};
}

#endif /*_TRAP_VULKANCOMMANDPOOL_H_*/
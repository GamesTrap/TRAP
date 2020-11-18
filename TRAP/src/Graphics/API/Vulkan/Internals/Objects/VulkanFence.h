#ifndef _TRAP_VULKANFENCE_H_
#define _TRAP_VULKANFENCE_H_

namespace TRAP::Graphics::API::Vulkan
{
	class Device;

	class Fence
	{
	public:
		explicit Fence(Device* device, bool signaled = false);
		~Fence();
		Fence(const Fence&) = default;
		Fence& operator=(const Fence&) = default;
		Fence(Fence&&) = default;
		Fence& operator=(Fence&&) = default;

		VkFence& GetFence();
		VkResult GetStatus() const;
		void Reset() const;
		
	private:
		VkFence m_fence;
		Device* m_device;
	};
}

#endif /*_TRAP_VULKANFENCE_H_*/
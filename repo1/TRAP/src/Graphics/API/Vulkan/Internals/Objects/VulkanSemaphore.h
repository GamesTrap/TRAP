#ifndef _TRAP_VULKANSEMAPHORE_H_
#define _TRAP_VULKANSEMAPHORE_H_

namespace TRAP::Graphics::API::Vulkan
{
	class Device;
	
	class Semaphore
	{
	public:
		explicit Semaphore(Device* device);
		~Semaphore();
		Semaphore(const Semaphore&) = default;
		Semaphore& operator=(const Semaphore&) = default;
		Semaphore(Semaphore&&) = default;
		Semaphore& operator=(Semaphore&&) = default;

		VkSemaphore& GetSemaphore();
		
	private:
		VkSemaphore m_semaphore;
		Device* m_device;
	};
}

#endif /*_TRAP_VULKANSEMAPHORE_H_*/
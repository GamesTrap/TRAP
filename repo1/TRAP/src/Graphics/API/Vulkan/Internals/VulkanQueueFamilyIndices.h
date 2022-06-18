#ifndef _TRAP_VULKANQUEUEFAMILYINDICES_H_
#define _TRAP_VULKANQUEUEFAMILYINDICES_H_

namespace TRAP::Graphics::API::Vulkan
{
	struct QueueFamilyIndices
	{
		uint32_t GraphicsIndices = std::numeric_limits<uint32_t>::max();
		uint32_t PresentIndices = std::numeric_limits<uint32_t>::max();
		uint32_t ComputeIndices = std::numeric_limits<uint32_t>::max();
		uint32_t TransferIndices = std::numeric_limits<uint32_t>::max();
	};	
}

#endif /*_TRAP_VULKANQUEUEFAMILYINDICES_H_*/
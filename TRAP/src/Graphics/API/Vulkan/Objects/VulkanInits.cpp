#include "TRAPPCH.h"
#include "VulkanInits.h"

#include "Graphics/API/Vulkan/Objects/VulkanMemoryAllocator.h"
#include "Graphics/API/Vulkan/Objects/VulkanDevice.h"
#include "Graphics/API/Vulkan/Objects/VulkanPhysicalDevice.h"
#include "Graphics/API/RendererAPI.h"

[[nodiscard]] VmaAllocatorCreateInfo TRAP::Graphics::API::VulkanInits::VMAAllocatorCreateInfo(const VulkanDevice& device,
	                                                                                          VkInstance instance,
	                                                                                          const VmaVulkanFunctions& vulkanFunctions)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const auto& physicalDevice = device.GetPhysicalDevice();

	TRAP_ASSERT(device.GetVkDevice() != VK_NULL_HANDLE, "VulkanInits::VMAAllocatorCreateInfo(): Device can't be VK_NULL_HANDLE!");
	TRAP_ASSERT(physicalDevice.GetVkPhysicalDevice() != VK_NULL_HANDLE, "VulkanInits::VMAAllocatorCreateInfo(): PhysicalDevice can't be VK_NULL_HANDLE!");
	TRAP_ASSERT(instance != VK_NULL_HANDLE, "VulkanInits::VMAAllocatorCreateInfo(): Instance can't be VK_NULL_HANDLE!");

	VmaAllocatorCreateInfo info
	{
		.flags = VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT,
		.physicalDevice = physicalDevice.GetVkPhysicalDevice(),
		.device = device.GetVkDevice(),
		.preferredLargeHeapBlockSize = 0,
		.pAllocationCallbacks = nullptr,
		.pDeviceMemoryCallbacks = nullptr,
		.pHeapSizeLimit = nullptr,
		.pVulkanFunctions = &vulkanFunctions,
		.instance = instance,
		.vulkanApiVersion = VK_API_VERSION_1_1,
#if VMA_EXTERNAL_MEMORY
		.pTypeExternalMemoryHandleTypes = nullptr
#endif
	};

	if(device.GetPhysicalDevice().IsFeatureEnabled(VulkanPhysicalDeviceFeature::BufferDeviceAddress))
		info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

	return info;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkFragmentShadingRateAttachmentInfoKHR TRAP::Graphics::API::VulkanInits::FragmentShadingRateAttachmentInfo(const VkAttachmentReference2KHR& shadingRateAttachment,
	                                                                                                                     const VkExtent2D& texelSize)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(shadingRateAttachment.attachment != VK_ATTACHMENT_UNUSED, "VulkanInits::FragmentShadingRateAttachmentInfo(): ShadingRateAttachment can't be VK_ATTACHMENT_UNUSED!");
	TRAP_ASSERT(shadingRateAttachment.layout == VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR || shadingRateAttachment.layout == VK_IMAGE_LAYOUT_GENERAL, "VulkanInits::FragmentShadingRateAttachmentInfo(): ShadingRateAttachment Layout must be VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR or VK_IMAGE_LAYOUT_GENERAL!");
	TRAP_ASSERT(Math::IsPowerOfTwo(texelSize.width) && Math::IsPowerOfTwo(texelSize.height), "VulkanInits::FragmentShadingRateAttachmentInfo(): TexelSize must be a power of two!");
	TRAP_ASSERT(texelSize.width <= RendererAPI::GPUSettings.ShadingRateTexelWidth && texelSize.height <= RendererAPI::GPUSettings.ShadingRateTexelHeight, "VulkanInits::FragmentShadingRateAttachmentInfo(): TexelSize must be smaller than or equal to RendererAPI::GPUSettings.ShadingRateTexelWidth and RendererAPI::GPUSettings.ShadingRateTexelHeight!");

	return VkFragmentShadingRateAttachmentInfoKHR
	{
		.sType = VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR,
		.pNext = nullptr,
		.pFragmentShadingRateAttachment = &shadingRateAttachment,
		.shadingRateAttachmentTexelSize = texelSize
	};
}

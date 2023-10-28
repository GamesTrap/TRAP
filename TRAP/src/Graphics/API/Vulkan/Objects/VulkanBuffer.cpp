#include "TRAPPCH.h"
#include "VulkanBuffer.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanBuffer::VulkanBuffer(const RendererAPI::BufferDesc& desc)
	: TRAP::Graphics::Buffer(desc.Size, desc.Descriptors, desc.MemoryUsage)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanBuffer(): Vulkan Device is nullptr");
	TRAP_ASSERT(m_VMA, "VulkanBuffer(): VMA is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanBufferPrefix, "Creating Buffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	u64 allocationSize = desc.Size;
	//Align the buffer size to multiples of the dynamic uniform buffer minimum size
	if((desc.Descriptors & RendererAPI::DescriptorType::UniformBuffer) != RendererAPI::DescriptorType::Undefined)
	{
		const u64 minAlignment = RendererAPI::GPUSettings.UniformBufferAlignment;
		allocationSize = ((allocationSize + minAlignment - 1) / minAlignment) * minAlignment;
	}

	VkBufferCreateInfo info = VulkanInits::BufferCreateInfo(allocationSize,
	                                                        DescriptorTypeToVkBufferUsage(desc.Descriptors,
															                              desc.Format != TRAP::Graphics::API::ImageFormat::Undefined));

	//Buffer can be used as dest in a transfer command (Uploading data to a storage buffer, Readback query data)
	if (desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly ||
	    desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUToCPU)
		info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VmaAllocationCreateInfo vmaMemReqs{};
	vmaMemReqs.usage = static_cast<VmaMemoryUsage>(desc.MemoryUsage);
	vmaMemReqs.flags = 0;
	if ((desc.Flags & RendererAPI::BufferCreationFlags::OwnMemory) != RendererAPI::BufferCreationFlags::None)
		vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	if ((desc.Flags & RendererAPI::BufferCreationFlags::PersistentMap) != RendererAPI::BufferCreationFlags::None)
		vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
	if ((desc.Flags & RendererAPI::BufferCreationFlags::HostVisible) != RendererAPI::BufferCreationFlags::None)
		vmaMemReqs.flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	if ((desc.Flags & RendererAPI::BufferCreationFlags::HostCoherent) != RendererAPI::BufferCreationFlags::None)
		vmaMemReqs.flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

#ifdef TRAP_PLATFORM_ANDROID
	//UMA for Android devices
	if(vmaMemReqs.usage != VMA_MEMORY_USAGE_CPU_TO_GPU)
	{
		vmaMemReqs.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
	}
#endif /*TRAP_PLATFORM_ANDROID*/

	VmaAllocationInfo allocInfo{};
	VkCall(vmaCreateBuffer(m_VMA->GetVMAAllocator(), &info, &vmaMemReqs, &m_vkBuffer, &m_allocation, &allocInfo));

	TRAP_ASSERT(m_vkBuffer, "VulkanBuffer(): Failed to create Buffer");
	TRAP_ASSERT(m_allocation, "VulkanBuffer(): Failed to create Buffer");

	m_CPUMappedAddress = allocInfo.pMappedData;

	//Set descriptor data
	if (((desc.Descriptors & RendererAPI::DescriptorType::UniformBuffer) != RendererAPI::DescriptorType::Undefined) ||
		((desc.Descriptors & RendererAPI::DescriptorType::Buffer) != RendererAPI::DescriptorType::Undefined) ||
		((desc.Descriptors & RendererAPI::DescriptorType::RWBuffer) != RendererAPI::DescriptorType::Undefined))
	{
		if (((desc.Descriptors & RendererAPI::DescriptorType::Buffer) != RendererAPI::DescriptorType::Undefined) ||
			((desc.Descriptors & RendererAPI::DescriptorType::RWBuffer) != RendererAPI::DescriptorType::Undefined))
			m_offset = desc.StructStride * desc.FirstElement;
	}

	if((info.usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT) != 0u)
	{
		const VkBufferViewCreateInfo viewInfo = VulkanInits::BufferViewCreateInfo(m_vkBuffer,
		                                                                          ImageFormatToVkFormat(desc.Format),
		                                                                          desc.FirstElement * desc.StructStride,
		                                                                          desc.ElementCount * desc.StructStride);
		const VkFormatProperties formatProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceFormatProperties(viewInfo.format);
		if ((formatProps.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT) == 0u)
			TP_WARN(Log::RendererVulkanBufferPrefix, "Failed to create uniform texel buffer view for format ",
			        std::to_underlying(desc.Format));
		else
			VkCall(vkCreateBufferView(m_device->GetVkDevice(), &viewInfo, nullptr, &m_vkUniformTexelView));
	}
	if((info.usage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT) != 0u)
	{
		const VkBufferViewCreateInfo viewInfo = VulkanInits::BufferViewCreateInfo(m_vkBuffer,
		                                                                          ImageFormatToVkFormat(desc.Format),
		                                                                          desc.FirstElement * desc.StructStride,
		                                                                          desc.ElementCount * desc.StructStride);
		const VkFormatProperties formatProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceFormatProperties(viewInfo.format);
		if ((formatProps.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT) == 0u)
			TP_WARN(Log::RendererVulkanBufferPrefix, "Failed to create storage texel buffer view for format ",
			        std::to_underlying(desc.Format));
		else
			VkCall(vkCreateBufferView(m_device->GetVkDevice(), &viewInfo, nullptr, &m_vkStorageTexelView));
	}

#ifdef ENABLE_GRAPHICS_DEBUG
	if (!desc.Name.empty())
		SetBufferName(desc.Name);
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanBuffer::~VulkanBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanBufferPrefix, "Destroying Buffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(m_vkUniformTexelView != nullptr)
		vkDestroyBufferView(m_device->GetVkDevice(), m_vkUniformTexelView, nullptr);

	if(m_vkStorageTexelView != nullptr)
		vkDestroyBufferView(m_device->GetVkDevice(), m_vkStorageTexelView, nullptr);

	if(m_allocation != nullptr)
		vmaDestroyBuffer(m_VMA->GetVMAAllocator(), m_vkBuffer, m_allocation);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDeviceMemory TRAP::Graphics::API::VulkanBuffer::GetVkDeviceMemory() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VmaAllocationInfo allocInfo{};
	vmaGetAllocationInfo(m_VMA->GetVMAAllocator(), m_allocation, &allocInfo);

	return allocInfo.deviceMemory;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u64 TRAP::Graphics::API::VulkanBuffer::GetVkDeviceMemoryOffset() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VmaAllocationInfo allocInfo{};
	vmaGetAllocationInfo(m_VMA->GetVMAAllocator(), m_allocation, &allocInfo);

	return allocInfo.offset;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanBuffer::MapBuffer(const RendererAPI::ReadRange* const range)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_memoryUsage != RendererAPI::ResourceMemoryUsage::GPUOnly,
	            "VulkanBuffer::MapBuffer(): Trying to map non-CPU accessible resource");

	const VkResult res = vmaMapMemory(m_VMA->GetVMAAllocator(), m_allocation, &m_CPUMappedAddress);
	TRAP_ASSERT(res == VK_SUCCESS);

	if (range != nullptr)
		m_CPUMappedAddress = (static_cast<u8*>(m_CPUMappedAddress) + range->Offset);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanBuffer::UnMapBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_memoryUsage != RendererAPI::ResourceMemoryUsage::GPUOnly,
	            "VulkanBuffer::UnMapBuffer(): Trying to unmap non-CPU accessible resource");

	if (m_VMA && (m_allocation != nullptr))
		vmaUnmapMemory(m_VMA->GetVMAAllocator(), m_allocation);
	m_CPUMappedAddress = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanBuffer::SetBufferName(const std::string_view name) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(!name.empty(), "VulkanBuffer::SetBufferName(): Name is empty!");

	if(!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device->GetVkDevice(), std::bit_cast<u64>(m_vkBuffer), VK_OBJECT_TYPE_BUFFER, name);
#else
	VkSetObjectName(m_device->GetVkDevice(), std::bit_cast<u64>(m_vkBuffer), VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, name);
#endif
}
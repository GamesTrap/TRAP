#include "TRAPPCH.h"
#include "VulkanBuffer.h"

#include "VulkanMemoryAllocator.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanBuffer::VulkanBuffer(const RendererAPI::BufferDesc& desc)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()),
	  m_VMA(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetVMA()),
	  m_vkBuffer(VK_NULL_HANDLE),
	  m_vkStorageTexelView(VK_NULL_HANDLE),
	  m_vkUniformTexelView(VK_NULL_HANDLE),
	  m_allocation(VK_NULL_HANDLE),
	  m_offset()
{
	m_CPUMappedAddress = nullptr;
	m_size = desc.Size;
	m_descriptors = desc.Descriptors;
	m_memoryUsage = desc.MemoryUsage;

	TRAP_ASSERT(m_device, "device is nullptr");
	TRAP_ASSERT(m_VMA, "VMA is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanBufferPrefix, "Creating Buffer");
#endif

	uint64_t allocationSize = desc.Size;
	//Align the buffer size to multiples of the dynamic uniform buffer minimum size
	if(static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::UniformBuffer))
	{
		const uint64_t minAlignment = RendererAPI::GPUSettings.UniformBufferAlignment;
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
	if (static_cast<uint32_t>(desc.Flags & RendererAPI::BufferCreationFlags::OwnMemory))
		vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	if (static_cast<uint32_t>(desc.Flags & RendererAPI::BufferCreationFlags::PersistentMap))
		vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
	if (static_cast<uint32_t>(desc.Flags & RendererAPI::BufferCreationFlags::HostVisible))
		vmaMemReqs.flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	if (static_cast<uint32_t>(desc.Flags & RendererAPI::BufferCreationFlags::HostCoherent))
		vmaMemReqs.flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

#ifdef TRAP_PLATFORM_ANDROID
	//UMA for Android devices
	if(vmaMemReqs.usage != VMA_MEMORY_USAGE_CPU_TO_GPU)
	{
		vmaMemReqs.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
	}
#endif

	VmaAllocationInfo allocInfo{};
	VkCall(vmaCreateBuffer(m_VMA->GetVMAAllocator(), &info, &vmaMemReqs, &m_vkBuffer, &m_allocation, &allocInfo));

	m_CPUMappedAddress = allocInfo.pMappedData;

	//Set descriptor data
	if (static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::UniformBuffer) ||
		static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::Buffer) ||
		static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RWBuffer))
	{
		if (static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::Buffer) ||
			static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RWBuffer))
			m_offset = desc.StructStride * desc.FirstElement;
	}

	if(info.usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT)
	{
		VkBufferViewCreateInfo viewInfo = VulkanInits::BufferViewCreateInfo(m_vkBuffer,
		                                                                    ImageFormatToVkFormat(desc.Format),
		                                                                    desc.FirstElement * desc.StructStride,
		                                                                    desc.ElementCount * desc.StructStride);
		VkFormatProperties formatProps{};
		vkGetPhysicalDeviceFormatProperties(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(), viewInfo.format,
		                                    &formatProps);
		if (!(formatProps.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT))
			TP_WARN(Log::RendererVulkanBufferPrefix, "Failed to create uniform texel buffer view for format ",
			        static_cast<uint32_t>(desc.Format));
		else
			VkCall(vkCreateBufferView(m_device->GetVkDevice(), &viewInfo, nullptr, &m_vkUniformTexelView));
	}
	if(info.usage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT)
	{
		VkBufferViewCreateInfo viewInfo = VulkanInits::BufferViewCreateInfo(m_vkBuffer,
		                                                                    ImageFormatToVkFormat(desc.Format),
		                                                                    desc.FirstElement * desc.StructStride,
		                                                                    desc.ElementCount * desc.StructStride);
		VkFormatProperties formatProps{};
		vkGetPhysicalDeviceFormatProperties(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(), viewInfo.format,
		                                    &formatProps);
		if (!(formatProps.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT))
			TP_WARN(Log::RendererVulkanBufferPrefix, "Failed to create storage texel buffer view for format ",
			        static_cast<uint32_t>(desc.Format));
		else
			VkCall(vkCreateBufferView(m_device->GetVkDevice(), &viewInfo, nullptr, &m_vkStorageTexelView));
	}

#ifdef ENABLE_GRAPHICS_DEBUG
	if (!desc.Name.empty())
		SetBufferName(desc.Name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanBuffer::~VulkanBuffer()
{
	TRAP_ASSERT(m_vkBuffer);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanBufferPrefix, "Destroying Buffer");
#endif

	if(m_vkUniformTexelView)
	{
		vkDestroyBufferView(m_device->GetVkDevice(), m_vkUniformTexelView, nullptr);
		m_vkUniformTexelView = VK_NULL_HANDLE;
	}

	if(m_vkStorageTexelView)
	{
		vkDestroyBufferView(m_device->GetVkDevice(), m_vkStorageTexelView, nullptr);
		m_vkStorageTexelView = VK_NULL_HANDLE;
	}

	vmaDestroyBuffer(m_VMA->GetVMAAllocator(), m_vkBuffer, m_allocation);
}

//-------------------------------------------------------------------------------------------------------------------//

VkBuffer TRAP::Graphics::API::VulkanBuffer::GetVkBuffer() const
{
	return m_vkBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

VkBufferView TRAP::Graphics::API::VulkanBuffer::GetStorageTexelView() const
{
	return m_vkStorageTexelView;
}

//-------------------------------------------------------------------------------------------------------------------//

VkBufferView TRAP::Graphics::API::VulkanBuffer::GetUniformTexelView() const
{
	return m_vkUniformTexelView;
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::API::VulkanBuffer::GetOffset() const
{
	return m_offset;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDeviceMemory TRAP::Graphics::API::VulkanBuffer::GetVkDeviceMemory() const
{
	VmaAllocationInfo allocInfo{};
	vmaGetAllocationInfo(m_VMA->GetVMAAllocator(), m_allocation, &allocInfo);

	return allocInfo.deviceMemory;
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::API::VulkanBuffer::GetVkDeviceMemoryOffset() const
{
	VmaAllocationInfo allocInfo{};
	vmaGetAllocationInfo(m_VMA->GetVMAAllocator(), m_allocation, &allocInfo);

	return allocInfo.offset;
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::API::VulkanBuffer::GetSize() const
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::API::VulkanBuffer::GetDescriptors() const
{
	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::ResourceMemoryUsage TRAP::Graphics::API::VulkanBuffer::GetMemoryUsage() const
{
	return m_memoryUsage;
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::Graphics::API::VulkanBuffer::GetCPUMappedAddress() const
{
	return m_CPUMappedAddress;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanBuffer::MapBuffer(RendererAPI::ReadRange* range)
{
	TRAP_ASSERT(m_memoryUsage != RendererAPI::ResourceMemoryUsage::GPUOnly,
	            "Trying to unmap non-CPU accessible resource");

	const VkResult res = vmaMapMemory(m_VMA->GetVMAAllocator(), m_allocation, &m_CPUMappedAddress);
	TRAP_ASSERT(res == VK_SUCCESS);

	if (range)
		m_CPUMappedAddress = (static_cast<uint8_t*>(m_CPUMappedAddress) + range->Offset);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanBuffer::UnMapBuffer()
{
	TRAP_ASSERT(m_memoryUsage != RendererAPI::ResourceMemoryUsage::GPUOnly,
	            "Trying to unmap non-CPU accessible resource");

	if (m_VMA && m_allocation)
		vmaUnmapMemory(m_VMA->GetVMAAllocator(), m_allocation);
	m_CPUMappedAddress = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanBuffer::SetBufferName(const std::string_view name) const
{
	if(!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device->GetVkDevice(), reinterpret_cast<uint64_t>(m_vkBuffer), VK_OBJECT_TYPE_BUFFER, name);
#else
	VkSetObjectName(m_device->GetVkDevice(), reinterpret_cast<uint64_t>(m_vkBuffer), VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, name);
#endif
}
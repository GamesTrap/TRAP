#include "TRAPPCH.h"
#include "VulkanBuffer.h"

#include "VulkanMemoryAllocator.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanBuffer::VulkanBuffer(TRAP::Ref<VulkanDevice> device, TRAP::Ref<VulkanMemoryAllocator> vma, const RendererAPI::BufferDesc& desc)
	: m_device(std::move(device)),
	  m_VMA(std::move(vma)),
	  m_CPUMappedAddress(nullptr),
	  m_vkBuffer(VK_NULL_HANDLE),
	  m_vkStorageTexelView(VK_NULL_HANDLE),
	  m_vkUniformTexelView(VK_NULL_HANDLE),
	  m_allocation(VK_NULL_HANDLE),
	  m_offset(),
	  m_size(desc.Size),
	  m_descriptors(desc.Descriptors),
	  m_memoryUsage(desc.MemoryUsage)
{
	TRAP_ASSERT(device, "device is nullptr");
	TRAP_ASSERT(vma, "VMA is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanBufferPrefix, "Creating Buffer");
#endif

	uint64_t allocationSize = desc.Size;
	//Align the buffer size to multiples of the dynamic uniform buffer minimum size
	if(static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::UniformBuffer))
	{
		const uint64_t minAlignment = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment;
		allocationSize = ((allocationSize + minAlignment - 1) / minAlignment) * minAlignment;
	}

	VkBufferCreateInfo info = VulkanInits::BufferCreateInfo(allocationSize, DescriptorTypeToVkBufferUsage(desc.Descriptors, desc.Format != RendererAPI::ImageFormat::Undefined));

	//Buffer can be used as dest in a transfer command (Uploading data to a storage buffer, Readback query data)
	if (desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly || desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUToCPU)
		info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VmaAllocationCreateInfo vmaMemReqs{};
	vmaMemReqs.usage = static_cast<VmaMemoryUsage>(desc.MemoryUsage);
	vmaMemReqs.flags = 0;
	if (static_cast<uint32_t>(desc.Flags & RendererAPI::BufferCreationFlags::OnwMemory))
		vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	if (static_cast<uint32_t>(desc.Flags & RendererAPI::BufferCreationFlags::PersistentMap))
		vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

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
		{
			m_offset = desc.StructStride * desc.FirstElement;
		}
	}

	if(info.usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT)
	{
		VkBufferViewCreateInfo viewInfo = VulkanInits::BufferViewCreateInfo(m_vkBuffer,
		                                                                    ImageFormatToVkFormat(desc.Format),
		                                                                    desc.FirstElement * desc.StructStride,
		                                                                    desc.ElementCount * desc.StructStride);
		VkFormatProperties formatProps{};
		vkGetPhysicalDeviceFormatProperties(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(), viewInfo.format, &formatProps);
		if (!(formatProps.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT))
			TP_WARN(Log::RendererVulkanBufferPrefix, "Failed to create uniform texel buffer view for format ", static_cast<uint32_t>(desc.Format));
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
		vkGetPhysicalDeviceFormatProperties(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(), viewInfo.format, &formatProps);
		if (!(formatProps.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT))
			TP_WARN(Log::RendererVulkanBufferPrefix, "Failed to create storage texel buffer view for format ", static_cast<uint32_t>(desc.Format));
		else
			VkCall(vkCreateBufferView(m_device->GetVkDevice(), &viewInfo, nullptr, &m_vkStorageTexelView));
	}

#ifdef ENABLE_GRAPHICS_DEBUG
	if (desc.Name)
		SetBufferName(desc.Name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanBuffer::~VulkanBuffer()
{
	if(m_vkBuffer)
	{
#ifdef ENABLE_GRAPHICS_DEBUG
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
}

//-------------------------------------------------------------------------------------------------------------------//

VkBuffer& TRAP::Graphics::API::VulkanBuffer::GetVkBuffer()
{
	return m_vkBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

VkBufferView& TRAP::Graphics::API::VulkanBuffer::GetStorageTexelView()
{
	return m_vkStorageTexelView;
}

//-------------------------------------------------------------------------------------------------------------------//

VkBufferView& TRAP::Graphics::API::VulkanBuffer::GetUniformTexelView()
{
	return m_vkUniformTexelView;
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::Graphics::API::VulkanBuffer::GetOffset() const
{
	return m_offset;
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
	TRAP_ASSERT(m_memoryUsage != RendererAPI::ResourceMemoryUsage::GPUOnly, "Trying to unmap non-CPU accessible resource");

	const VkResult res = vmaMapMemory(m_VMA->GetVMAAllocator(), m_allocation, &m_CPUMappedAddress);
	TRAP_ASSERT(res == VK_SUCCESS);

	if (range)
		m_CPUMappedAddress = (static_cast<uint8_t*>(m_CPUMappedAddress) + range->Offset);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanBuffer::UnMapBuffer()
{
	TRAP_ASSERT(m_memoryUsage != RendererAPI::ResourceMemoryUsage::GPUOnly, "Trying to unmap non-CPU accessible resource");

	if (m_VMA && m_allocation)
		vmaUnmapMemory(m_VMA->GetVMAAllocator(), m_allocation);
	m_CPUMappedAddress = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanBuffer::SetBufferName(const char* name) const
{
	VkSetObjectName(m_device->GetVkDevice(), reinterpret_cast<uint64_t>(m_vkBuffer), VK_OBJECT_TYPE_BUFFER, name);
}
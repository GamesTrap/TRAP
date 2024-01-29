#include "TRAPPCH.h"
#include "VulkanBuffer.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace
{
	[[nodiscard]] u64 GetAlignedDynamicUniformBufferSize(const u64 allocationSize)
	{
		const u64 minAlignment = TRAP::Graphics::RendererAPI::GPUSettings.UniformBufferAlignment;
		return (allocationSize + minAlignment - 1) & -minAlignment; //Optimized, same as:
		// return ((allocationSize + minAlignment - 1) / minAlignment) * minAlignment;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] u64 GetAlignedStorageBufferSize(const u64 allocationSize)
	{
		const u64 minAlignment = TRAP::Graphics::RendererAPI::GPUSettings.StorageBufferAlignment;
		return (allocationSize + minAlignment - 1) & -minAlignment; //Optimized, same as:
		// return ((allocationSize + minAlignment - 1) / minAlignment) * minAlignment;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] u64 GetAlignedBufferSize(const u64 allocationSize, const TRAP::Graphics::RendererAPI::DescriptorType bufferType)
	{
		using enum TRAP::Graphics::RendererAPI::DescriptorType;

		//Align the buffer size to multiples of the dynamic uniform buffer minimum size
		if((bufferType & UniformBuffer) != Undefined)
			return GetAlignedDynamicUniformBufferSize(allocationSize);

		if((bufferType & RWBuffer) != Undefined)
			return GetAlignedStorageBufferSize(allocationSize);

		return allocationSize;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkBufferCreateInfo GetBufferCreateInfo(const TRAP::Graphics::RendererAPI::BufferDesc& desc)
	{
		using namespace TRAP::Graphics;

		const u64 alignedAllocSize = GetAlignedBufferSize(desc.Size, desc.Descriptors);
		const VkBufferUsageFlags usageFlags = TRAP::Graphics::API::DescriptorTypeToVkBufferUsage(desc.Descriptors, desc.Format != API::ImageFormat::Undefined);
		VkBufferCreateInfo info = TRAP::Graphics::API::VulkanInits::BufferCreateInfo(alignedAllocSize, usageFlags);

		//Buffer can be used as dest in a transfer command (Uploading data to a storage buffer, Readback query data)
		if (desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUOnly ||
			desc.MemoryUsage == RendererAPI::ResourceMemoryUsage::GPUToCPU)
		{
			info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}

		return info;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VmaAllocationCreateInfo GetVMAAllocationCreateInfo(const TRAP::Graphics::RendererAPI::ResourceMemoryUsage memoryUsage,
	                                                                 const TRAP::Graphics::RendererAPI::BufferCreationFlags creationFlags)
	{
		using enum TRAP::Graphics::RendererAPI::BufferCreationFlags;

		VmaAllocationCreateInfo vmaMemReqs{};
		vmaMemReqs.usage = static_cast<VmaMemoryUsage>(memoryUsage);
		vmaMemReqs.flags = 0;
		if ((creationFlags & OwnMemory) != None)
			vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		if ((creationFlags & PersistentMap) != None)
			vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
		if ((creationFlags & HostVisible) != None)
			vmaMemReqs.flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		if ((creationFlags & HostCoherent) != None)
			vmaMemReqs.flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	#ifdef TRAP_PLATFORM_ANDROID
		//UMA for Android devices
		if(vmaMemReqs.usage != VMA_MEMORY_USAGE_CPU_TO_GPU)
		{
			vmaMemReqs.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
			vmaMemReqs.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}
	#endif /*TRAP_PLATFORM_ANDROID*/

		return vmaMemReqs;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr VkFormatFeatureFlagBits BufferUsageToFormatFeatureFlag(const VkBufferUsageFlagBits bufferUsage)
	{
		switch(bufferUsage)
		{
		case VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT:
			return VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

		case VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT:
			return VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;

		default:
			return {};
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void CreateTexelBufferView(const TRAP::Graphics::RendererAPI::BufferDesc& desc,
	                           const VkBufferUsageFlagBits bufferUsage,
	                           VkBuffer& buffer, const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
							   VkBufferView& outBufferView)
	{
		TRAP_ASSERT((bufferUsage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT) != 0u ||
		            (bufferUsage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT) != 0u,
					"VulkanBuffer::CreateTexelBufferView(): bufferUsage must contain either uniform or storage texel buffer bit!");

		const VkBufferViewCreateInfo viewInfo = TRAP::Graphics::API::VulkanInits::BufferViewCreateInfo(buffer,
		                                                                                               ImageFormatToVkFormat(desc.Format),
		                                                                                               0,
		                                                                                               desc.ElementCount * desc.StructStride);
		const VkFormatProperties formatProps = device->GetPhysicalDevice()->GetVkPhysicalDeviceFormatProperties(viewInfo.format);
		if ((formatProps.bufferFeatures & BufferUsageToFormatFeatureFlag(bufferUsage)) == 0u) //Format doesnt support texel buffer usage
		{
			if((bufferUsage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT) != 0u)
			{
				TP_ERROR(TRAP::Log::RendererVulkanBufferPrefix, "Failed to create storage texel buffer view for format ",
						std::to_underlying(desc.Format));
				return;
			}
			if((bufferUsage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT) != 0u)
			{
				TP_ERROR(TRAP::Log::RendererVulkanBufferPrefix, "Failed to create uniform texel buffer view for format ",
						std::to_underlying(desc.Format));
				return;
			}
		}

		VkCall(vkCreateBufferView(device->GetVkDevice(), &viewInfo, nullptr, &outBufferView));
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanBuffer::VulkanBuffer(const RendererAPI::BufferDesc& desc)
	: TRAP::Graphics::Buffer(desc.Size, desc.Descriptors, desc.MemoryUsage)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(desc.Size != 0, "VulkanBuffer(): Trying to create empty buffer!");
	TRAP_ASSERT(m_device, "VulkanBuffer(): Vulkan Device is nullptr");
	TRAP_ASSERT(m_VMA, "VulkanBuffer(): VMA is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanBufferPrefix, "Creating Buffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	const VkBufferCreateInfo info = GetBufferCreateInfo(desc);

	const VmaAllocationCreateInfo vmaMemReqs = GetVMAAllocationCreateInfo(desc.MemoryUsage, desc.Flags);
	VmaAllocationInfo allocInfo{};
	VkCall(vmaCreateBuffer(m_VMA->GetVMAAllocator(), &info, &vmaMemReqs, &m_vkBuffer, &m_allocation, &allocInfo));

	TRAP_ASSERT(m_vkBuffer, "VulkanBuffer(): Failed to create Buffer");
	TRAP_ASSERT(m_allocation, "VulkanBuffer(): Failed to create Buffer");

	m_CPUMappedAddress = allocInfo.pMappedData;

	if((info.usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT) != 0u)
		CreateTexelBufferView(desc, VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, m_vkBuffer, m_device, m_vkUniformTexelView);
	else if((info.usage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT) != 0u)
		CreateTexelBufferView(desc, VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT, m_vkBuffer, m_device, m_vkStorageTexelView);

#ifdef ENABLE_GRAPHICS_DEBUG
	if (!desc.Name.empty())
		SetBufferName(desc.Name);
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanBuffer::~VulkanBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanBufferPrefix, "Destroying Buffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(m_vkUniformTexelView != VK_NULL_HANDLE)
		vkDestroyBufferView(m_device->GetVkDevice(), m_vkUniformTexelView, nullptr);

	if(m_vkStorageTexelView != VK_NULL_HANDLE)
		vkDestroyBufferView(m_device->GetVkDevice(), m_vkStorageTexelView, nullptr);

	if(m_allocation != VK_NULL_HANDLE)
		vmaDestroyBuffer(m_VMA->GetVMAAllocator(), m_vkBuffer, m_allocation);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::API::VulkanBuffer::MapBuffer(const RendererAPI::ReadRange& range)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan)  != ProfileSystems::None);

	TRAP_ASSERT(m_memoryUsage != RendererAPI::ResourceMemoryUsage::GPUOnly,
	            "VulkanBuffer::MapBuffer(): Trying to map non-CPU accessible resource");

	if(m_CPUMappedAddress != nullptr)
	{
		TP_ERROR(Log::RendererVulkanBufferPrefix, "VulkanBuffer::MapBuffer(): Buffer is already mapped! Missing UnMapBuffer() call?");
		return false;
	}

	void* mappedMemory = nullptr;
	const VkResult res = vmaMapMemory(m_VMA->GetVMAAllocator(), m_allocation, &mappedMemory);
	TRAP_ASSERT(res == VK_SUCCESS, "VulkanBuffer::MapBuffer(): Failed to map buffer for CPU access");

	if(res == VK_SUCCESS)
		m_CPUMappedAddress = static_cast<u8*>(mappedMemory) + range.Offset;

	return m_CPUMappedAddress != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanBuffer::UnMapBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_memoryUsage != RendererAPI::ResourceMemoryUsage::GPUOnly,
	            "VulkanBuffer::UnMapBuffer(): Trying to unmap non-CPU accessible resource");

#ifndef TRAP_RELEASE
	if(m_CPUMappedAddress == nullptr)
	{
		TP_WARN(Log::RendererVulkanBufferPrefix, "VulkanBuffer::UnMapBuffer(): Buffer is not mapped, ignoring call");
		return;
	}
#endif /*TRAP_RELEASE*/

	if (m_CPUMappedAddress != nullptr && m_VMA && (m_allocation != nullptr))
		vmaUnmapMemory(m_VMA->GetVMAAllocator(), m_allocation);
	m_CPUMappedAddress = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanBuffer::SetBufferName(const std::string_view name) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(!name.empty(), "VulkanBuffer::SetBufferName(): Name is empty!");

	if(!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device->GetVkDevice(), std::bit_cast<u64>(m_vkBuffer), VK_OBJECT_TYPE_BUFFER, name);
#else
	VkSetObjectName(m_device->GetVkDevice(), std::bit_cast<u64>(m_vkBuffer), VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, name);
#endif
}

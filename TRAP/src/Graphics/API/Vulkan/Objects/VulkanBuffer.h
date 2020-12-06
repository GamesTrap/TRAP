#ifndef _TRAP_VULKANBUFFER_H_
#define _TRAP_VULKANBUFFER_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Utils/VMA.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanDevice;
	
	class VulkanBuffer
	{
	public:
		VulkanBuffer(TRAP::Ref<VulkanDevice> device, TRAP::Ref<VulkanMemoryAllocator> vma, const RendererAPI::BufferDesc& desc);
		~VulkanBuffer();

		VkBuffer& GetVkBuffer();
		VkBufferView& GetStorageTexelView();
		VkBufferView& GetUniformTexelView();
		uint64_t GetOffset() const;
		uint64_t GetSize() const;
		RendererAPI::DescriptorType GetDescriptors() const;
		RendererAPI::ResourceMemoryUsage GetMemoryUsage() const;
		
		void* GetCPUMappedAddress() const;

		void MapBuffer(RendererAPI::ReadRange* range);
		void UnMapBuffer();
		
	private:
		void SetBufferName(const char* name) const;
		
		TRAP::Ref<VulkanDevice> m_device;
		TRAP::Ref<VulkanMemoryAllocator> m_VMA;

		//CPU address of mapped buffer applicable to buffers created in CPU accessible heaps (CPU, CPUToGPU, GPUToCPU)
		void* m_CPUMappedAddress;
		//Native handle of the underlying resource
		VkBuffer m_vkBuffer;
		//Buffer view
		VkBufferView m_vkStorageTexelView;
		VkBufferView m_vkUniformTexelView;
		//Contains resource allocation info such as parent heap, offset in heap
		VmaAllocation m_allocation;
		uint64_t m_offset;

		uint64_t m_size;
		RendererAPI::DescriptorType m_descriptors;
		RendererAPI::ResourceMemoryUsage m_memoryUsage;
	};
}

#endif /*_TRAP_VULKANBUFFER_H_*/
#ifndef TRAP_VULKANBUFFER_H
#define TRAP_VULKANBUFFER_H

#include <vk_mem_alloc.h>

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Buffer.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanDevice;

	class VulkanBuffer final : public Buffer
	{
	public:
		explicit VulkanBuffer(const RendererAPI::BufferDesc& desc);
		~VulkanBuffer() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanBuffer(const VulkanBuffer&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanBuffer& operator=(const VulkanBuffer&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanBuffer(VulkanBuffer&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanBuffer& operator=(VulkanBuffer&&) = default;

		VkBuffer GetVkBuffer() const;
		VkBufferView GetStorageTexelView() const;
		VkBufferView GetUniformTexelView() const;
		uint64_t GetOffset() const;
		VkDeviceMemory GetVkDeviceMemory() const;
		uint64_t GetVkDeviceMemoryOffset() const;

		uint64_t GetSize() const override;
		RendererAPI::DescriptorType GetDescriptors() const override;
		RendererAPI::ResourceMemoryUsage GetMemoryUsage() const override;

		void* GetCPUMappedAddress() const override;

		void MapBuffer(RendererAPI::ReadRange* range) override;
		void UnMapBuffer() override;

	private:
		void SetBufferName(const char* name) const;

		TRAP::Ref<VulkanDevice> m_device;
		TRAP::Ref<VulkanMemoryAllocator> m_VMA;

		//Native handle of the underlying resource
		VkBuffer m_vkBuffer;
		//Buffer view
		VkBufferView m_vkStorageTexelView;
		VkBufferView m_vkUniformTexelView;
		//Contains resource allocation info such as parent heap, offset in heap
		VmaAllocation m_allocation;
		uint64_t m_offset;
	};
}

#endif /*TRAP_VULKANBUFFER_H*/
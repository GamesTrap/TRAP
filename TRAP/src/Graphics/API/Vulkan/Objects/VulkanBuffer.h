#ifndef TRAP_VULKANBUFFER_H
#define TRAP_VULKANBUFFER_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Utils/VMA.h"
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
		/// Default Copy Constructor.
		/// </summary>
		VulkanBuffer(const VulkanBuffer&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VulkanBuffer& operator=(const VulkanBuffer&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VulkanBuffer(VulkanBuffer&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		VulkanBuffer& operator=(VulkanBuffer&&) = default;

		VkBuffer& GetVkBuffer();
		VkBufferView& GetStorageTexelView();
		VkBufferView& GetUniformTexelView();
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
#ifndef _TRAP_VULKANVERTEXBUFFER_H_
#define _TRAP_VULKANVERTEXBUFFER_H_

#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/BufferLayout.h"

namespace TRAP::Graphics::API
{
	class VulkanVertexBuffer final : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(float* vertices, uint32_t size);
		explicit VulkanVertexBuffer(uint32_t size);
		VulkanVertexBuffer(const VulkanVertexBuffer&) = default;
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer&) = default;
		VulkanVertexBuffer(VulkanVertexBuffer&&) = default;
		VulkanVertexBuffer& operator=(VulkanVertexBuffer&&) = default;
		~VulkanVertexBuffer();

		const BufferLayout& GetLayout() const override;
		void SetLayout(const BufferLayout& layout) override;
		uint32_t GetVertexCount() const override;
		void SetData(const void* data, uint32_t size) override;

	private:
		void AllocateBuffer();
		void DeallocateBuffer() const;
		void UploadData(const void* data) const;
		void CopyBuffer(VkBuffer& stagingBuffer) const;
		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);
		static void CreateBuffer(VkDeviceSize size,
		                         VkBufferUsageFlags usage,
		                         VkMemoryPropertyFlags properties,
		                         VkBuffer& buffer,
		                         VkDeviceMemory& bufferMemory);
		
		BufferLayout m_layout;
		uint32_t m_vertexCount;

		//Internal
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
	};
}

#endif /*_TRAP_VULKANVERTEXBUFFER_H_*/
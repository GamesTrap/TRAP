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
	};
}

#endif /*_TRAP_VULKANVERTEXBUFFER_H_*/
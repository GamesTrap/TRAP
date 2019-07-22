#ifndef _TRAP_VULKANINDEXBUFFER_H_
#define _TRAP_VULKANINDEXBUFFER_H_

#include "Graphics/Buffers/IndexBuffer.h"

namespace TRAP::Graphics::API
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* indices, uint32_t size, BufferUsage usage);
		VulkanIndexBuffer(const VulkanIndexBuffer&) = default;
		VulkanIndexBuffer& operator=(const VulkanIndexBuffer&) = default;
		VulkanIndexBuffer(VulkanIndexBuffer&&) = default;
		VulkanIndexBuffer& operator=(VulkanIndexBuffer&&) = default;
		virtual ~VulkanIndexBuffer();

		void Bind() const override;
		void Unbind() const override;

		uint32_t GetCount() const override;

	private:
		uint32_t m_count;
	};
}

#endif /*_TRAP_VULKANINDEXBUFFER_H_*/
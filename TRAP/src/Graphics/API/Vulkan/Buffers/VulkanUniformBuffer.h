#ifndef _TRAP_VULKANUNIFORMBUFFER_H_
#define _TRAP_VULKANUNIFORMBUFFER_H_

namespace TRAP::Graphics::API
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(const char* name, uint32_t size, BufferUsage usage);
		virtual ~VulkanUniformBuffer() = default;
		VulkanUniformBuffer(const VulkanUniformBuffer&) = default;
		VulkanUniformBuffer& operator=(const VulkanUniformBuffer&) = default;
		VulkanUniformBuffer(VulkanUniformBuffer&&) = default;
		VulkanUniformBuffer& operator=(VulkanUniformBuffer&&) = default;

		void Bind() const override;
		void Unbind() const override;

		void UpdateData(const void* data, uint32_t size, BufferUsage usage) override;
		void UpdateSubData(const void* data, uint32_t size, uint32_t offset) override;
	};
}

#endif /*_TRAP_VULKANUNIFORMBUFFER_H_*/
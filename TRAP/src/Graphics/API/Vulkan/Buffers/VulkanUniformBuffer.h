#ifndef _TRAP_VULKANUNIFORMBUFFER_H_
#define _TRAP_VULKANUNIFORMBUFFER_H_

#include "Graphics/Buffers/UniformBuffer.h"

namespace TRAP::Graphics::API
{
	class VulkanUniformBuffer final : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(std::string name, uint32_t size, BufferUsage usage);
		VulkanUniformBuffer(std::string name, const void* data, uint32_t size, BufferUsage usage);
		virtual ~VulkanUniformBuffer() = default;
		VulkanUniformBuffer(const VulkanUniformBuffer&) = default;
		VulkanUniformBuffer& operator=(const VulkanUniformBuffer&) = default;
		VulkanUniformBuffer(VulkanUniformBuffer&&) = default;
		VulkanUniformBuffer& operator=(VulkanUniformBuffer&&) = default;

		void Bind(uint32_t bindingPoint) const override;
		void Unbind(uint32_t bindingPoint) const override;

		void UpdateData(const void* data) override;
		void UpdateSubData(const void* data, uint32_t size, uint32_t offset) override;

		std::string_view GetName() const override;
		uint32_t GetSize() const override;
		BufferUsage GetUsage() const override;

	private:
		std::string m_name;
		uint32_t m_size;
		BufferUsage m_usage;
	};
}

#endif /*_TRAP_VULKANUNIFORMBUFFER_H_*/
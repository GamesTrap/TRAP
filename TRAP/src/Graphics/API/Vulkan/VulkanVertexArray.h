#ifndef _TRAP_VULKANVERTEXARRAY_H_
#define _TRAP_VULKANVERTEXARRAY_H_

namespace TRAP::Graphics::API
{
	class VulkanVertexArray final : public VertexArray
	{
	public:
		VulkanVertexArray();
		VulkanVertexArray(const VulkanVertexArray&) = default;
		VulkanVertexArray& operator=(const VulkanVertexArray&) = default;
		VulkanVertexArray(VulkanVertexArray&&) = default;
		VulkanVertexArray& operator=(VulkanVertexArray&&) = default;
		~VulkanVertexArray();

		void AddBuffer(std::unique_ptr<VertexBuffer>& buffer) override;

		void Bind() const override;
		void Unbind() const override;

		void Draw(uint32_t count) const override;
	};
}

#endif /*_TRAP_VULKANVERTEXARRAY_H_*/
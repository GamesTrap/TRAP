#ifndef _TRAP_VULKANVERTEXARRAY_H_
#define _TRAP_VULKANVERTEXARRAY_H_

#include "Graphics/Buffers/VertexArray.h"

namespace TRAP::Graphics::API
{
	class VulkanVertexArray final : public VertexArray
	{
	public:
		VulkanVertexArray();
		VulkanVertexArray(const VulkanVertexArray&) = delete;
		VulkanVertexArray& operator=(const VulkanVertexArray&) = delete;
		VulkanVertexArray(VulkanVertexArray&&) = default;
		VulkanVertexArray& operator=(VulkanVertexArray&&) = delete;
		~VulkanVertexArray();

		void AddVertexBuffer(Scope<VertexBuffer>& buffer) override;
		void SetIndexBuffer(Scope<IndexBuffer>& buffer) override;

		void Bind() const override;
		void Unbind() const override;
		
		std::vector<Scope<VertexBuffer>>& GetVertexBuffers() override;
		const Scope<IndexBuffer>& GetIndexBuffer() override;

	private:
		std::vector<Scope<VertexBuffer>> m_vertexBuffers;
		Scope<IndexBuffer> m_indexBuffer;
	};
}

#endif /*_TRAP_VULKANVERTEXARRAY_H_*/
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
		VulkanVertexArray& operator=(VulkanVertexArray&&) = default;
		~VulkanVertexArray();

		void AddVertexBuffer(std::unique_ptr<VertexBuffer>& buffer) override;
		void SetIndexBuffer(std::unique_ptr<IndexBuffer>& buffer) override;

		void Bind() const override;
		void Unbind() const override;
		
		std::vector<std::unique_ptr<VertexBuffer>>& GetVertexBuffers() override;
		IndexBuffer* GetIndexBuffer() override;
		uint32_t GetIndexCount() const override;

	private:
		std::vector<std::unique_ptr<VertexBuffer>> m_vertexBuffers;
		std::unique_ptr<IndexBuffer> m_indexBuffer;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::API::VulkanVertexArray::GetIndexCount() const
{
	return 0;
}

#endif /*_TRAP_VULKANVERTEXARRAY_H_*/
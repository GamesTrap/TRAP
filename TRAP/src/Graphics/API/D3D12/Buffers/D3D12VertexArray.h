#ifndef _TRAP_D3D12VERTEXARRAY_H_
#define _TRAP_D3D12VERTEXARRAY_H_

#include "Graphics/Buffers/VertexArray.h"

#ifdef TRAP_PLATFORM_WINDOWS

namespace TRAP::Graphics::API
{
	class D3D12VertexArray final : public VertexArray
	{
	public:
		D3D12VertexArray();
		D3D12VertexArray(const D3D12VertexArray&) = delete;
		D3D12VertexArray& operator=(const D3D12VertexArray&) = delete;
		D3D12VertexArray(D3D12VertexArray&&) = default;
		D3D12VertexArray& operator=(D3D12VertexArray&&) = default;
		~D3D12VertexArray();

		void AddVertexBuffer(Scope<VertexBuffer>& buffer) override;
		void SetIndexBuffer(Scope<IndexBuffer>& buffer) override;

		void Bind() const override;
		void Unbind() const override;
		
		std::vector<Scope<VertexBuffer>>& GetVertexBuffers() override;
		const Scope<IndexBuffer>& GetIndexBuffer() override;
		uint32_t GetIndexCount() const override;

	private:
		std::vector<Scope<VertexBuffer>> m_vertexBuffers;
		Scope<IndexBuffer> m_indexBuffer;
	};
}

#endif

#endif /*_TRAP_D3D12VERTEXARRAY_H_*/
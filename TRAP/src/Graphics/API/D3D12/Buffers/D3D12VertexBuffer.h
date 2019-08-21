#ifndef _TRAP_D3D12VERTEXBUFFER_H_
#define _TRAP_D3D12VERTEXBUFFER_H_

#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/BufferLayout.h"

#ifdef TRAP_PLATFORM_WINDOWS

namespace TRAP::Graphics::API
{
	class D3D12VertexBuffer final : public VertexBuffer
	{
	public:
		D3D12VertexBuffer(float* vertices, uint32_t size);
		D3D12VertexBuffer(const D3D12VertexBuffer&) = default;
		D3D12VertexBuffer& operator=(const D3D12VertexBuffer&) = default;
		D3D12VertexBuffer(D3D12VertexBuffer&&) = default;
		D3D12VertexBuffer& operator=(D3D12VertexBuffer&&) = default;
		~D3D12VertexBuffer();

		const BufferLayout& GetLayout() const override;
		void SetLayout(const BufferLayout& layout) override;

	private:
		BufferLayout m_layout;
	};
}

#endif

#endif /*_TRAP_D3D12VERTEXBUFFER_H_*/
#ifndef _TRAP_D3D12INDEXBUFFER_H_
#define _TRAP_D3D12INDEXBUFFER_H_

#include "Graphics/Buffers/IndexBuffer.h"

#ifdef TRAP_PLATFORM_WINDOWS

namespace TRAP::Graphics::API
{
	class D3D12IndexBuffer final : public IndexBuffer
	{
	public:
		D3D12IndexBuffer(uint32_t* indices, uint32_t size);
		D3D12IndexBuffer(const D3D12IndexBuffer&) = default;
		D3D12IndexBuffer& operator=(const D3D12IndexBuffer&) = default;
		D3D12IndexBuffer(D3D12IndexBuffer&&) = default;
		D3D12IndexBuffer& operator=(D3D12IndexBuffer&&) = default;
		virtual ~D3D12IndexBuffer();

		uint32_t GetCount() const override;

	private:
		uint32_t m_count;
	};
}

#endif

#endif /*_TRAP_D3D12INDEXBUFFER_H_*/
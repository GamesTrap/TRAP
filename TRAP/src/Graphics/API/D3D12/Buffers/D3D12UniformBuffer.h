#ifndef _TRAP_D3D12UNIFORMBUFFER_H_
#define _TRAP_D3D12UNIFORMBUFFER_H_

#include "Graphics/Buffers/UniformBuffer.h"

namespace TRAP::Graphics::API
{
	class D3D12UniformBuffer : public UniformBuffer
	{
	public:
		D3D12UniformBuffer(const char* name, uint32_t size, BufferUsage usage);
		D3D12UniformBuffer(const char* name, const void* data, uint32_t size, BufferUsage usage);
		virtual ~D3D12UniformBuffer() = default;
		D3D12UniformBuffer(const D3D12UniformBuffer&) = default;
		D3D12UniformBuffer& operator=(const D3D12UniformBuffer&) = default;
		D3D12UniformBuffer(D3D12UniformBuffer&&) = default;
		D3D12UniformBuffer& operator=(D3D12UniformBuffer&&) = default;

		void Bind(uint32_t bindingPoint) const override;
		void Unbind() const override;

		void UpdateData(const void* data) override;
		void UpdateSubData(const void* data, uint32_t size, uint32_t offset) override;
	};
}

#endif /*_TRAP_D3D12UNIFORMBUFFER_H_*/
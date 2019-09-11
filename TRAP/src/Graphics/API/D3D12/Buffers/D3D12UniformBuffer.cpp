#include "TRAPPCH.h"
#include "D3D12UniformBuffer.h"

TRAP::Graphics::API::D3D12UniformBuffer::D3D12UniformBuffer(const char* name, uint32_t size)
{
	TP_WARN("[UniformBuffer][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12UniformBuffer::D3D12UniformBuffer(const char* name, const void* data, uint32_t size)
{
	TP_WARN("[UniformBuffer][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12UniformBuffer::Bind(uint32_t bindingPoint) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12UniformBuffer::Unbind() const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12UniformBuffer::UpdateData(const void* data, uint32_t size)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12UniformBuffer::UpdateSubData(const void* data, uint32_t size, uint32_t offset)
{	
}
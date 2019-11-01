#include "TRAPPCH.h"
#include "D3D12UniformBuffer.h"

TRAP::Graphics::API::D3D12UniformBuffer::D3D12UniformBuffer(std::string name, const uint32_t size, const BufferUsage usage)
	: m_name(std::move(name)), m_size(size), m_usage(usage)
{
	TP_WARN("[UniformBuffer][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12UniformBuffer::D3D12UniformBuffer(std::string name, const void* data, const uint32_t size, const BufferUsage usage)
	: m_name(std::move(name)), m_size(size), m_usage(usage)
{
	TP_WARN("[UniformBuffer][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12UniformBuffer::Bind(uint32_t bindingPoint) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12UniformBuffer::Unbind(uint32_t bindingPoint) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12UniformBuffer::UpdateData(const void* data)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12UniformBuffer::UpdateSubData(const void* data, uint32_t size, uint32_t offset)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::D3D12UniformBuffer::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::D3D12UniformBuffer::GetSize() const
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferUsage TRAP::Graphics::API::D3D12UniformBuffer::GetUsage() const
{
	return m_usage;
}
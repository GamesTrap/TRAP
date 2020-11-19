#include "TRAPPCH.h"
#include "VulkanUniformBuffer.h"

TRAP::Graphics::API::VulkanUniformBuffer::VulkanUniformBuffer(std::string name, const uint32_t size, const BufferUsage usage)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanUniformBuffer::VulkanUniformBuffer(std::string name, const void* data, const uint32_t size, const BufferUsage usage)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::Bind(uint32_t bindingPoint) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::Unbind(uint32_t bindingPoint) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::UpdateData(const void* data)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::UpdateSubData(const void* data, uint32_t size, uint32_t offset)
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::API::VulkanUniformBuffer::GetName() const
{
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanUniformBuffer::GetSize() const
{
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferUsage TRAP::Graphics::API::VulkanUniformBuffer::GetUsage() const
{
	return {};
}
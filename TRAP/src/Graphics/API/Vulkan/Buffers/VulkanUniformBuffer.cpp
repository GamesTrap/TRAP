#include "TRAPPCH.h"
#include "VulkanUniformBuffer.h"

TRAP::Graphics::API::VulkanUniformBuffer::VulkanUniformBuffer(const char* name, uint32_t size)
{
	TP_WARN("[UniformBuffer][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanUniformBuffer::VulkanUniformBuffer(const char* name, const void* data, uint32_t size)
{
	TP_WARN("[UniformBuffer][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::Bind(uint32_t bindingPoint) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::Unbind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::UpdateData(const void* data, uint32_t size)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::UpdateSubData(const void* data, uint32_t size, uint32_t offset)
{	
}
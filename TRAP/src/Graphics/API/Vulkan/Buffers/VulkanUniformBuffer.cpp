#include "TRAPPCH.h"
#include "VulkanUniformBuffer.h"

TRAP::Graphics::API::VulkanUniformBuffer::VulkanUniformBuffer(std::string name, const uint32_t size, const BufferUsage usage)
	: m_name(std::move(name)), m_size(size), m_usage(usage)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN(Log::UniformBufferVulkanPrefix, "WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanUniformBuffer::VulkanUniformBuffer(std::string name, const void* data, const uint32_t size, const BufferUsage usage)
	: m_name(std::move(name)), m_size(size), m_usage(usage)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN(Log::UniformBufferVulkanPrefix, "WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::Bind(uint32_t bindingPoint) const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::Unbind(uint32_t bindingPoint) const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::UpdateData(const void* data)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanUniformBuffer::UpdateSubData(const void* data, uint32_t size, uint32_t offset)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::API::VulkanUniformBuffer::GetName() const
{
	TP_PROFILE_FUNCTION();

	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanUniformBuffer::GetSize() const
{
	TP_PROFILE_FUNCTION();

	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferUsage TRAP::Graphics::API::VulkanUniformBuffer::GetUsage() const
{
	TP_PROFILE_FUNCTION();

	return m_usage;
}
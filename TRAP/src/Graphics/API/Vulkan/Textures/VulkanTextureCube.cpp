#include "TRAPPCH.h"
#include "VulkanTextureCube.h"

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(const TextureParameters parameters)
	: m_name("FallbackCube"), m_parameters(parameters)
{
	TP_WARN("[TextureCube][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(std::string name, std::string filepath, InputFormat format, const TextureParameters parameters)
	: m_name(std::move(name)), m_parameters(parameters)
{
	TP_WARN("[TextureCube][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(std::string name, const std::array<std::string, 6>& filepaths, const TextureParameters parameters)
	: m_name(std::move(name)), m_parameters(parameters)
{
	TP_WARN("[TextureCube][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::~VulkanTextureCube() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::Bind(unsigned slot) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::Unbind(unsigned slot) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::SetWrap(TextureWrap wrap)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::SetFilter(TextureFilter filter)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::InputFormat TRAP::Graphics::API::VulkanTextureCube::GetInputFormat() const
{
	return InputFormat::NONE;
}
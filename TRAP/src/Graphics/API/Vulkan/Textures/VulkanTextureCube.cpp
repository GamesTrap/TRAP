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

void TRAP::Graphics::API::VulkanTextureCube::Bind(uint32_t slot) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::Unbind(uint32_t slot) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VulkanTextureCube::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image* TRAP::Graphics::API::VulkanTextureCube::GetImage()
{
	return m_images[0].get();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters TRAP::Graphics::API::VulkanTextureCube::GetParameters()
{
	return m_parameters;
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

//-------------------------------------------------------------------------------------------------------------------//

std::array<TRAP::Image*, 6> TRAP::Graphics::API::VulkanTextureCube::GetImages() const
{
	return { m_images[0].get(), m_images[1].get(), m_images[2].get(), m_images[3].get(), m_images[4].get(), m_images[5].get(), };
}
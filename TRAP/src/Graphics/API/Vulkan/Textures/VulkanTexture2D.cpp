#include "TRAPPCH.h"
#include "VulkanTexture2D.h"

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(TextureParameters parameters)
	: m_name("Fallback")
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, const std::string& filepath, TextureParameters parameters)
	: m_name(std::move(name))
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::~VulkanTexture2D()
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Bind(unsigned slot) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Unbind(unsigned slot) const
{	
}
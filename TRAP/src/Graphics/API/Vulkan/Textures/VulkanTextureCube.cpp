#include "TRAPPCH.h"
#include "VulkanTextureCube.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(const TextureParameters parameters)
{
	m_name = "FallbackCube";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(std::string name, const std::string_view filepath, InputFormat format, const TextureParameters parameters)
{
	m_name = std::move(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(std::string name, const std::array<std::string_view, 6>& filepaths, const TextureParameters parameters)
{
	m_name = std::move(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(std::string name, const Scope<Image>& img, const InputFormat format, const TextureParameters parameters)
{
	m_name = std::move(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::~VulkanTextureCube()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::Bind(uint32_t slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::Unbind(uint32_t slot) const
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
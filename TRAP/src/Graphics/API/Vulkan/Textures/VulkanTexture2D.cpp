#include "TRAPPCH.h"
#include "VulkanTexture2D.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(const TextureParameters parameters)
{
	m_name = "Fallback2D";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(const uint32_t width, const uint32_t height, uint32_t bitsPerPixel, const Image::ColorFormat format, const TextureParameters parameters)
{
	m_name = "Empty";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, const Scope<Image>& img, const TextureParameters parameters)
{
	m_name = std::move(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, const std::string_view filepath, const TextureParameters parameters)
{
	m_name = std::move(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::~VulkanTexture2D()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Bind(const uint32_t slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Unbind(const uint32_t slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::SetWrap(const TextureWrap wrap)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::SetFilter(const TextureFilter filter)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::UploadImage(const TRAP::Scope<TRAP::Image>& image)
{
}
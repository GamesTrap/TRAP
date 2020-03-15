#include "TRAPPCH.h"
#include "VulkanTexture2D.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = "Fallback2D";
	m_textureParameters = parameters;
	
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(const uint32_t width, const uint32_t height, uint32_t bitsPerPixel, const Image::ColorFormat format, const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = "Empty";
	m_textureParameters = parameters;
	
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, const Scope<Image>& img, const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_textureParameters = parameters;

	if(img)
	{
		m_filepath = VFS::MakeVirtualPathCompatible(std::string(img->GetFilePath()));		
	}

	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_filepath = VFS::MakeVirtualPathCompatible(filepath);
	m_textureParameters = parameters;
	
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::~VulkanTexture2D()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Bind(const uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Unbind(const uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::SetWrap(const TextureWrap wrap)
{
	TP_PROFILE_FUNCTION();
	
	m_textureParameters.Wrap = wrap;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::SetFilter(const TextureFilter filter)
{
	TP_PROFILE_FUNCTION();
	
	m_textureParameters.Filter = filter;
}
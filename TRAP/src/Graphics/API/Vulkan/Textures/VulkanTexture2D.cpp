#include "TRAPPCH.h"
#include "VulkanTexture2D.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(const TextureParameters parameters)
	: m_name("Fallback2D"), m_parameters(parameters)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(const uint32_t width, const uint32_t height, uint32_t bitsPerPixel, const ImageFormat format, const TextureParameters parameters)
	: m_name("Empty"), m_parameters(parameters)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, const std::vector<uint8_t>& pixelData, const TextureParameters parameters)
	: m_name(std::move(name)), m_filePath(""), m_parameters(parameters)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, const std::vector<uint16_t>& pixelData, const TextureParameters parameters)
	: m_name(std::move(name)), m_filePath(""), m_parameters(parameters)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, const std::vector<float>& pixelData, const TextureParameters parameters)
	: m_name(std::move(name)), m_filePath(""), m_parameters(parameters)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
	: m_name(std::move(name)), m_filePath(VFS::MakeVirtualPathCompatible(filepath)), m_parameters(parameters)
{
	TP_PROFILE_FUNCTION();
	
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

std::string_view TRAP::Graphics::API::VulkanTexture2D::GetName() const
{
	TP_PROFILE_FUNCTION();

	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::VulkanTexture2D::GetFilePath() const
{
	TP_PROFILE_FUNCTION();

	return m_filePath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters TRAP::Graphics::API::VulkanTexture2D::GetParameters()
{
	TP_PROFILE_FUNCTION();

	return m_parameters;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::SetWrap(const TextureWrap wrap)
{
	TP_PROFILE_FUNCTION();
	
	m_parameters.Wrap = wrap;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::SetFilter(const TextureFilter filter)
{
	TP_PROFILE_FUNCTION();
	
	m_parameters.Filter = filter;
}
#include "TRAPPCH.h"
#include "VulkanTexture2D.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(const TextureParameters parameters)
	: m_name("Fallback"), m_parameters(parameters)
{
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(const ImageFormat format, const uint32_t width, const uint32_t height, const TextureParameters parameters)
	: m_name("Empty"), m_parameters(parameters)
{
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
	: m_name(std::move(name)), m_parameters(parameters), m_filePath(VFS::MakeVirtualPathCompatible(filepath))
{
	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::~VulkanTexture2D() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Bind(const uint32_t slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Unbind(const uint32_t slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VulkanTexture2D::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VulkanTexture2D::GetFilePath() const
{
	return m_filePath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters TRAP::Graphics::API::VulkanTexture2D::GetParameters()
{
	return m_parameters;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::SetWrap(const TextureWrap wrap)
{
	m_parameters.Wrap = wrap;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::SetFilter(const TextureFilter filter)
{
	m_parameters.Filter = filter;
}
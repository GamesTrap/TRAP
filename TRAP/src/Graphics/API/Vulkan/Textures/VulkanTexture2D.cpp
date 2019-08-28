#include "TRAPPCH.h"
#include "VulkanTexture2D.h"

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(const TextureParameters parameters)
	: m_image(Image::LoadFallback()), m_name("Fallback"), m_parameters(parameters), m_handle(0)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(const ImageFormat format, const uint32_t width, const uint32_t height, const TextureParameters parameters)
	: m_image(Image::CreateEmpty(format, width, height)), m_name("Empty"), m_parameters(parameters), m_handle(0)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::VulkanTexture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
	: m_name(std::move(name)), m_parameters(parameters), m_handle(0)
{
	Load(filepath);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture2D::~VulkanTexture2D()
{
	TP_DEBUG("[Texture2D][Vulkan] Destroying Texture2D: \"", m_name, "\"");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Load(const std::string& filepath)
{
	TP_DEBUG("[Texture2D][Vulkan] Loading Texture: \"", m_name, "\"");
	m_image = Image::LoadFromFile(filepath);

	TP_WARN("[Texture2D][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Bind(const unsigned int slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::Unbind(const unsigned int slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::SetFilter(const TextureFilter filter)
{
	m_parameters.Filter = filter;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture2D::SetWrap(const TextureWrap wrap)
{
	m_parameters.Wrap = wrap;
}
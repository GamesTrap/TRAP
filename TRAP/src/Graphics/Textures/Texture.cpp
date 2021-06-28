#include "TRAPPCH.h"
#include "Texture.h"

#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"

//-------------------------------------------------------------------------------------------------------------------//

// TRAP::Graphics::TextureParameters::TextureParameters()
// 	: Filter(TextureFilter::Nearest), Wrap(TextureWrap::Clamp_To_Edge)
// {
// 	TP_PROFILE_FUNCTION();

// }

// //-------------------------------------------------------------------------------------------------------------------//

// TRAP::Graphics::TextureParameters::TextureParameters(const TextureFilter filter)
// 	: Filter(filter), Wrap(TextureWrap::Clamp_To_Edge)
// {
// 	TP_PROFILE_FUNCTION();

// }

// //-------------------------------------------------------------------------------------------------------------------//

// TRAP::Graphics::TextureParameters::TextureParameters(const TextureFilter filter, const TextureWrap wrap)
// 	: Filter(filter), Wrap(wrap)
// {
// 	TP_PROFILE_FUNCTION();

// }

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture::Texture()
	: m_textureType(TextureType::Texture2D),
	  m_texture(nullptr),
	  m_textureUsage(TextureUsage::Static),
	  m_syncToken()
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture::~Texture()
{
	m_texture.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::Texture::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureType TRAP::Graphics::Texture::GetType() const
{
	return m_textureType;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetWidth() const
{
	return m_texture->GetWidth();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetHeight() const
{
	return m_texture->GetHeight();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetDepth() const
{
	return m_texture->GetDepth();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Texture::GetMipLevels() const
{
	return m_texture->GetMipLevels();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureUsage TRAP::Graphics::Texture::GetTextureUsage() const
{
	return m_textureUsage;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Texture::IsLoaded() const
{
	return RendererAPI::GetResourceLoader()->IsTokenCompleted(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Texture::AwaitLoading() const
{
	RendererAPI::GetResourceLoader()->WaitForToken(&m_syncToken);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::API::VulkanTexture> TRAP::Graphics::Texture::GetTexture() const
{
	return m_texture;
}
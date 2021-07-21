#include "TRAPPCH.h"
#include "Texture.h"

#include "TextureBase.h"

TRAP::Graphics::Texture::Texture()
	: m_textureType(TextureType::Texture2D),
	  m_textureUsage(TextureUsage::Static),
	  m_syncToken(),
	  m_texture(nullptr)
{
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

TRAP::Ref<TRAP::Graphics::TextureBase> TRAP::Graphics::Texture::GetTexture() const
{
	return m_texture;
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
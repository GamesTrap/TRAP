#include "TRAPPCH.h"
#include "D3D12Texture2D.h"

#ifdef TRAP_PLATFORM_WINDOWS

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(TextureParameters parameters)
	: m_name("Fallback")
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(std::string name, const std::string& filepath, TextureParameters parameters)
	: m_name(std::move(name))
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::~D3D12Texture2D()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::Bind(unsigned slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::Unbind(unsigned slot) const
{
}

#endif
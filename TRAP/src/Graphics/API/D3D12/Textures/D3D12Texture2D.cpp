#include "TRAPPCH.h"
#include "D3D12Texture2D.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();
	
	m_name = "Fallback";
	m_textureParameters = parameters;

	TP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const Image::ColorFormat format, const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = "Fallback";
	m_textureParameters = parameters;

	TP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(std::string name, const Scope<Image>& img, const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_textureParameters = parameters;

	if(img)
	{
		m_filepath = VFS::MakeVirtualPathCompatible(std::string(img->GetFilePath()));		
	}

	TP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_filepath = VFS::MakeVirtualPathCompatible(filepath);
	m_textureParameters = parameters;

	TP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::~D3D12Texture2D()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::Bind(const uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::Unbind(const uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::SetWrap(const TextureWrap wrap)
{
	TP_PROFILE_FUNCTION();
	
	m_textureParameters.Wrap = wrap;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::SetFilter(const TextureFilter filter)
{
	TP_PROFILE_FUNCTION();
	
	m_textureParameters.Filter = filter;
}
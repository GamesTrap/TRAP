#include "TRAPPCH.h"
#include "D3D12Texture2D.h"

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(const TextureParameters parameters)
	: m_image(Image::LoadFromFile("")), m_name("Fallback"), m_parameters(parameters), m_handle(0)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(const ImageFormat format, const uint32_t width, const uint32_t height, const TextureParameters parameters)
	: m_image(Image::CreateEmpty(format, width, height)), m_name("Empty"), m_parameters(parameters), m_handle(0)
{
	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
	: m_name(std::move(name)), m_parameters(parameters), m_handle(0)
{
	Load(filepath);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::~D3D12Texture2D()
{
	TP_DEBUG("[Texture2D][D3D12] Destroying Texture2D: \"", m_name, "\"");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::Load(const std::string& filepath)
{
	TP_DEBUG("[Texture2D][D3D12] Loading Texture: \"", m_name, "\"");
	m_image = Image::LoadFromFile(filepath);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::Bind(const unsigned int slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::Unbind(const unsigned int slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::SetFilter(const TextureFilter filter)
{
	m_parameters.Filter = filter;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::SetWrap(const TextureWrap wrap)
{
	m_parameters.Wrap = wrap;
}
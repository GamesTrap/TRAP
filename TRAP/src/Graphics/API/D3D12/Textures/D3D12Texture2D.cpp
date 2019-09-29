#include "TRAPPCH.h"
#include "D3D12Texture2D.h"

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(const TextureParameters parameters)
	: m_image(Image::LoadFallback()), m_name("Fallback"), m_parameters(parameters), m_handle(0)
{
	TRAP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(const ImageFormat format, const uint32_t width, const uint32_t height, const TextureParameters parameters)
	: m_image(Image::CreateEmpty(format, width, height)), m_name("Empty"), m_parameters(parameters), m_handle(0)
{
	TRAP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
	: m_name(std::move(name)), m_parameters(parameters), m_handle(0)
{
	TRAP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::~D3D12Texture2D() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::Bind(const uint32_t slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::Unbind(const uint32_t slot) const
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
#include "TRAPPCH.h"
#include "D3D12Texture2D.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(const TextureParameters parameters)
	: m_name("Fallback"), m_parameters(parameters)
{
	TP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const ImageFormat format, const TextureParameters parameters)
	: m_name("Empty"), m_parameters(parameters)
{
	TP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(std::string name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, const std::vector<uint8_t>& pixelData, const TextureParameters parameters)
	: m_name(std::move(name)), m_filePath(""), m_parameters(parameters)
{
	TP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(std::string name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, const std::vector<uint16_t>& pixelData, const TextureParameters parameters)
	: m_name(std::move(name)), m_filePath(""), m_parameters(parameters)
{
	TP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(std::string name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, const std::vector<float>& pixelData, const TextureParameters parameters)
	: m_name(std::move(name)), m_filePath(""), m_parameters(parameters)
{
	TP_WARN("[Texture2D][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Texture2D::D3D12Texture2D(std::string name, const std::string& filepath, const TextureParameters parameters)
	: m_name(std::move(name)), m_filePath(VFS::MakeVirtualPathCompatible(filepath)), m_parameters(parameters)
{
	TP_WARN("[Texture2D][D3D12] WIP");
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

std::string_view TRAP::Graphics::API::D3D12Texture2D::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::D3D12Texture2D::GetFilePath() const
{
	return m_filePath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters TRAP::Graphics::API::D3D12Texture2D::GetParameters()
{
	return m_parameters;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::SetWrap(const TextureWrap wrap)
{
	m_parameters.Wrap = wrap;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Texture2D::SetFilter(const TextureFilter filter)
{
	m_parameters.Filter = filter;
}
#include "TRAPPCH.h"
#include "D3D12TextureCube.h"

TRAP::Graphics::API::D3D12TextureCube::D3D12TextureCube(const TextureParameters parameters)
	: m_name("FallbackCube"), m_parameters(parameters)
{
	TP_WARN("[TextureCube][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12TextureCube::D3D12TextureCube(std::string name, std::string filepath, InputFormat format, const TextureParameters parameters)
	: m_name(std::move(name)), m_parameters(parameters)
{
	TP_WARN("[TextureCube][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12TextureCube::D3D12TextureCube(std::string name, const std::array<std::string, 6>& filepaths, const TextureParameters parameters)
	: m_name(std::move(name)), m_parameters(parameters)
{
	TP_WARN("[TextureCube][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12TextureCube::~D3D12TextureCube() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::Bind(uint32_t slot) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::Unbind(uint32_t slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::D3D12TextureCube::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Image* TRAP::Graphics::API::D3D12TextureCube::GetImage()
{
	return m_images[0].get();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters TRAP::Graphics::API::D3D12TextureCube::GetParameters()
{
	return m_parameters;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::SetWrap(const TextureWrap wrap)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::SetFilter(const TextureFilter filter)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::InputFormat TRAP::Graphics::API::D3D12TextureCube::GetInputFormat() const
{
	return InputFormat::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<TRAP::Image*, 6> TRAP::Graphics::API::D3D12TextureCube::GetImages() const
{
	return { m_images[0].get(), m_images[1].get(), m_images[2].get(), m_images[3].get(), m_images[4].get(), m_images[5].get(), };
}
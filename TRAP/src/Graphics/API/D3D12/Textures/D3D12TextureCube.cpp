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

void TRAP::Graphics::API::D3D12TextureCube::Bind(unsigned int slot) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::Unbind(unsigned int slot) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::SetFilter(const TextureFilter filter)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::SetWrap(const TextureWrap wrap)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::InputFormat TRAP::Graphics::API::D3D12TextureCube::GetInputFormat() const
{
	return InputFormat::NONE;
}
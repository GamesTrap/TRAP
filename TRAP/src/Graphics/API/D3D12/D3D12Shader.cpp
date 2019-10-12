#include "TRAPPCH.h"
#include "D3D12Shader.h"

#ifdef TRAP_PLATFORM_WINDOWS

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string source)
	: m_name(std::move(name))
{
	TP_WARN("[Shader][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::vector<uint32_t>& source)
	: m_name(std::move(name))
{
	TP_WARN("[Shader][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
	: m_name(std::move(name))
{
	TP_WARN("[Shader][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::~D3D12Shader() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::Bind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::Unbind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::API::D3D12Shader::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::API::D3D12Shader::GetFilePath() const
{
	return m_filepath;
}

#endif
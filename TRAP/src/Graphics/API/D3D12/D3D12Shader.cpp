#include "TRAPPCH.h"
#include "D3D12Shader.h"

#ifdef TRAP_PLATFORM_WINDOWS

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string source)
	: m_name(std::move(name)), m_source(std::move(source))
{
	TRAP_WARN("[Shader][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
	: m_name(std::move(name)), m_VSSource(std::move(VSSource)), m_FSSource(std::move(FSSource)), m_GSSource(std::move(GSSource)), m_TCSSource(std::move(TCSSource)), m_TESSource(std::move(TESSource)), m_CSSource(std::move(CSSource))
{
	TRAP_WARN("[Shader][D3D12] WIP");
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

#endif
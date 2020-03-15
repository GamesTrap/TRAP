#include "TRAPPCH.h"
#include "D3D12Shader.h"

#ifdef TRAP_PLATFORM_WINDOWS

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string source)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);

	TP_WARN("[Shader][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::vector<uint32_t>& source)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);

	TP_WARN("[Shader][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
	: vsSource(std::move(VSSource)),
	  fsSource(std::move(FSSource)),
	  gsSource(std::move(GSSource)),
	  tcsSource(std::move(TCSSource)),
	  tesSource(std::move(TESSource)),
	  csSource(std::move(CSSource))
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);

	TP_WARN("[Shader][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::~D3D12Shader()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::Bind() const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::Unbind() const
{
	TP_PROFILE_FUNCTION();
}

#endif
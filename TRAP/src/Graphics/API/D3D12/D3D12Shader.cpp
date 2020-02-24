#include "TRAPPCH.h"
#include "D3D12Shader.h"

#ifdef TRAP_PLATFORM_WINDOWS

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string source)
	: m_name(std::move(name))
{
	TP_PROFILE_FUNCTION();

	TP_WARN("[Shader][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::vector<uint32_t>& source)
	: m_name(std::move(name))
{
	TP_PROFILE_FUNCTION();

	TP_WARN("[Shader][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
	: m_name(std::move(name)),
	  vsSource(std::move(VSSource)),
	  fsSource(std::move(FSSource)),
	  gsSource(std::move(GSSource)),
	  tcsSource(std::move(TCSSource)),
	  tesSource(std::move(TESSource)),
	  csSource(std::move(CSSource))
{
	TP_PROFILE_FUNCTION();

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

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::D3D12Shader::GetName() const
{
	TP_PROFILE_FUNCTION();
	
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::D3D12Shader::GetFilePath() const
{
	TP_PROFILE_FUNCTION();
	
	return m_filepath;
}

#endif
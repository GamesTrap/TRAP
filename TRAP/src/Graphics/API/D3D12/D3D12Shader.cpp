#include "TRAPPCH.h"
#include "D3D12Shader.h"

#ifdef TRAP_PLATFORM_WINDOWS

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string source)
	: m_name(std::move(name)), m_source(std::move(source))/*, m_VSUserUniformBuffer(nullptr), m_FSUserUniformBuffer(nullptr)*/
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
	: m_name(std::move(name)), m_VSSource(std::move(VSSource)), m_FSSource(std::move(FSSource)), m_GSSource(std::move(GSSource)), m_TCSSource(std::move(TCSSource)), m_TESSource(std::move(TESSource)), m_CSSource(std::move(CSSource))/*, m_VSUserUniformBuffer(nullptr), m_FSUserUniformBuffer(nullptr)*/
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::~D3D12Shader()
{
	TP_DEBUG("[Shader][D3D12] Destroying Shader: \"", m_name, "\"");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::Bind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::Unbind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetVSUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_VSUserUniformBuffer.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetFSUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_FSUserUniformBuffer.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetGSUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_GSUserUniformBuffer.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetTCSUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_TCSUserUniformBuffer.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetTESUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_TESUserUniformBuffer.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetCSUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_CSUserUniformBuffer.get(), data, size);
}

#endif
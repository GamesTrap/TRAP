#include "TRAPPCH.h"
#include "D3D12Shader.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Shader::D3D12Shader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
	: m_name(std::move(name)), m_VSSource(std::move(VSSource)), m_FSSource(std::move(FSSource)), m_GSSource(std::move(GSSource)), m_TCSSource(std::move(TCSSource)), m_TESSource(std::move(TESSource)), m_CSSource(std::move(CSSource))/*, m_VSUserUniformBuffer(nullptr), m_FSUserUniformBuffer(nullptr)*/
{
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

void TRAP::Graphics::API::D3D12Shader::SetVSSystemUniformBuffer(uint8_t* data, const unsigned int size, const unsigned int slot)
{
	Bind();
	TP_ASSERT(m_VSUniformBuffers.size() > slot, "[Shader][D3D12] VSUniformBuffers size is > slot");
	const std::unique_ptr<ShaderUniformBufferDeclaration>& declaration = m_VSUniformBuffers[slot];
	//ResolveAndSetUniforms(declaration.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetFSSystemUniformBuffer(uint8_t* data, const unsigned int size, const unsigned int slot)
{
	Bind();
	TP_ASSERT(m_FSUniformBuffers.size() > slot, "[Shader][D3D12] FSUniformBuffers size is > slot");
	const std::unique_ptr<ShaderUniformBufferDeclaration>& declaration = m_FSUniformBuffers[slot];
	//ResolveAndSetUniforms(declaration.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetGSSystemUniformBuffer(uint8_t* data, const unsigned int size, const unsigned int slot)
{
	Bind();
	TP_ASSERT(m_GSUniformBuffers.size() > slot, "[Shader][D3D12] GSUniformBuffers size is > slot");
	const std::unique_ptr<ShaderUniformBufferDeclaration>& declaration = m_GSUniformBuffers[slot];
	//ResolveAndSetUniforms(declaration.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetTCSSystemUniformBuffer(uint8_t* data, const unsigned int size, const unsigned int slot)
{
	Bind();
	TP_ASSERT(m_TCSUniformBuffers.size() > slot, "[Shader][D3D12] TCSUniformBuffers size is > slot");
	const std::unique_ptr<ShaderUniformBufferDeclaration>& declaration = m_TCSUniformBuffers[slot];
	//ResolveAndSetUniforms(declaration.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetTESSystemUniformBuffer(uint8_t* data, const unsigned int size, const unsigned int slot)
{
	Bind();
	TP_ASSERT(m_TESUniformBuffers.size() > slot, "[Shader][D3D12] TESUniformBuffers size is > slot");
	const std::unique_ptr<ShaderUniformBufferDeclaration>& declaration = m_TESUniformBuffers[slot];
	//ResolveAndSetUniforms(declaration.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetCSSystemUniformBuffer(uint8_t* data, const unsigned int size, const unsigned int slot)
{
	Bind();
	TP_ASSERT(m_CSUniformBuffers.size() > slot, "[Shader][D3D12] CSUniformBuffers size is > slot");
	const std::unique_ptr<ShaderUniformBufferDeclaration>& declaration = m_CSUniformBuffers[slot];
	//ResolveAndSetUniforms(declaration.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetVSUserUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_VSUserUniformBuffer.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetFSUserUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_FSUserUniformBuffer.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetGSUserUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_GSUserUniformBuffer.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetTCSUserUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_TCSUserUniformBuffer.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetTESUserUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_TESUserUniformBuffer.get(), data, size);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Shader::SetCSUserUniformBuffer(uint8_t* data, const unsigned int size)
{
	//ResolveAndSetUniforms(m_CSUserUniformBuffer.get(), data, size);
}
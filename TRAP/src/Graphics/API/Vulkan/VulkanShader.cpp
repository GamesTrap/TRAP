#include "TRAPPCH.h"
#include "VulkanShader.h"

#include "Graphics/Shaders/ShaderManager.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const std::string_view source)
{
	m_name = std::move(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const std::vector<uint32_t>& source)
{
	m_name = std::move(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name,
                                                const std::string_view VSSource,
                                                const std::string_view FSSource,
                                                const std::string_view GSSource,
                                                const std::string_view TCSSource,
                                                const std::string_view TESSource,
                                                const std::string_view CSSource)
{
	m_name = std::move(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::~VulkanShader()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Shutdown() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Bind() const
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Unbind() const
{
}
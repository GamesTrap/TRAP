#include "TRAPPCH.h"
#include "DummyShader.h"

#include "ShaderManager.h"

TRAP::Graphics::DummyShader::DummyShader(const std::string& name, const std::string& filepath)
{
	m_name = name;
	m_filepath = filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::Use(Window* window)
{
	TRAP::Graphics::ShaderManager::Get("Fallback")->Use(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseTexture(const uint32_t, const uint32_t,
                                             TRAP::Graphics::Texture* const)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseTextures(const uint32_t, const uint32_t,
                                              const std::vector<TRAP::Graphics::Texture*>&)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseSampler(const uint32_t, const uint32_t,
                                             TRAP::Graphics::Sampler* const)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseSamplers(const uint32_t, const uint32_t,
                                              const std::vector<TRAP::Graphics::Sampler*>&)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseUBO(const uint32_t, TRAP::Graphics::UniformBuffer* const)
{
}
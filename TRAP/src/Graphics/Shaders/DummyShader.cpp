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

void TRAP::Graphics::DummyShader::UseTexture(const uint32_t binding, const TRAP::Graphics::Texture* texture)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseSampler(uint32_t set, uint32_t binding, TRAP::Graphics::Sampler* const sampler)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseSamplers(uint32_t set, uint32_t binding,
                                              const std::vector<TRAP::Graphics::Sampler*>& samplers)
{
}
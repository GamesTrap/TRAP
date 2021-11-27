#include "TRAPPCH.h"
#include "DummyShader.h"

#include "ShaderManager.h"

TRAP::Graphics::DummyShader::DummyShader(std::string name, std::filesystem::path filepath)
{
	m_name = std::move(name);
	m_filepath = std::move(filepath);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::Use(Window* window)
{
	TRAP::Graphics::ShaderManager::Get("Fallback")->Use(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseTexture(const uint32_t, const uint32_t,
                                             TRAP::Graphics::Texture* const, Window*)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseTextures(const uint32_t, const uint32_t,
                                              const std::vector<TRAP::Graphics::Texture*>&, Window*)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseSampler(const uint32_t, const uint32_t,
                                             TRAP::Graphics::Sampler* const, Window*)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseSamplers(const uint32_t, const uint32_t,
                                              const std::vector<TRAP::Graphics::Sampler*>&, Window*)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseUBO(const uint32_t, const uint32_t,
                                         TRAP::Graphics::UniformBuffer* const, const uint64_t, const uint64_t,
                                         Window*)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::UseSSBO(const uint32_t, const uint32_t,
                                          TRAP::Graphics::StorageBuffer* const, const uint64_t, Window*)
{
}
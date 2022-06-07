#include "TRAPPCH.h"
#include "DummyShader.h"

#include "ShaderManager.h"

TRAP::Graphics::DummyShader::DummyShader(std::string name, std::filesystem::path filepath)
{
	m_name = std::move(name);
	m_filepath = std::move(filepath);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::DummyShader::DummyShader(std::string name, std::filesystem::path filepath,
                                         const RendererAPI::ShaderStage stages)
{
	m_name = std::move(name);
	m_filepath = std::move(filepath);
    m_shaderStages = stages;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::DummyShader::Use(Window* window)
{
    if(static_cast<bool>(m_shaderStages & RendererAPI::ShaderStage::Compute))
	    TRAP::Graphics::ShaderManager::Get("FallbackCompute")->Use(window);
    else
	    TRAP::Graphics::ShaderManager::Get("FallbackGraphics")->Use(window);
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
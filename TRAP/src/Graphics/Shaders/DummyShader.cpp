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
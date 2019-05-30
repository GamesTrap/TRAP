#include "TRAPPCH.h"
#include "ShaderFactory.h"

//-------------------------------------------------------------------------------------------------------------------//

//FALLBACK/PASSTHROUGH SHADER
static const char* s_PassthroughVSGLSL
{
	R"(
		#version 460 core

		layout(location = 0) in vec3 aPos;

		void main()
		{
			gl_Position = vec4(aPos, 1.0f);
		}
		)"
};
static const char* s_PassthroughFSGLSL
{
	R"(
		#version 460 core

		layout(location = 0) out vec4 FragColor;

		void main()
		{
			FragColor = vec4(1.0f);
		}
		)"
};

std::unique_ptr<TRAP::Graphics::API::Shader> TRAP::Graphics::ShaderFactory::PassthroughShader()
{
	switch (API::Context::GetRenderAPI())
	{
		/*case API::RenderAPI::D3D12:
			return API::Shader::CreateFromSource("Passthrough", s_DebugVertexShaderD3D12, s_DebugPixelShaderD3D12, "", "", "", "");*/

	case API::RenderAPI::VULKAN:
		return API::Shader::CreateFromSource("Passthrough", s_PassthroughVSGLSL, s_PassthroughFSGLSL, "", "", "", "");

	case API::RenderAPI::OPENGL:
		return API::Shader::CreateFromSource("Passthrough", s_PassthroughVSGLSL, s_PassthroughFSGLSL, "", "", "", "");

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//
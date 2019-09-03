#include "TRAPPCH.h"
#include "ShaderFactory.h"

#include "Graphics/API/Context.h"

//-------------------------------------------------------------------------------------------------------------------//

//FALLBACK/PASSTHROUGH SHADER
static constexpr const char* s_PassthroughVSGLSL
{
	R"(
		#version 460 core

		layout(location = 0) in vec3 Position;

		layout(std140, set = 0, binding = 0) uniform MatrixBuffer
		{
			mat4 sys_ProjectionMatrix;
			mat4 sys_ViewMatrix;
			mat4 sys_ModelMatrix;
		} Matrices;

		void main()
		{
			gl_Position = Matrices.sys_ProjectionMatrix * Matrices.sys_ViewMatrix * Matrices.sys_ModelMatrix * vec4(Position, 1.0f);
		}
	)"
};
static constexpr const char* s_PassthroughFSGLSL
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

std::unique_ptr<TRAP::Graphics::Shader> TRAP::Graphics::ShaderFactory::PassthroughShader()
{
	switch (API::Context::GetRenderAPI())
	{
/*#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return Shader::CreateFromSource("Passthrough", s_DebugVertexShaderD3D12, s_DebugPixelShaderD3D12, "", "", "", "");
#endif*/

	case API::RenderAPI::Vulkan:
		return Shader::CreateFromSource("Passthrough", s_PassthroughVSGLSL, s_PassthroughFSGLSL, "", "", "", "");

	case API::RenderAPI::OpenGL:
		return Shader::CreateFromSource("Passthrough", s_PassthroughVSGLSL, s_PassthroughFSGLSL, "", "", "", "");

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//
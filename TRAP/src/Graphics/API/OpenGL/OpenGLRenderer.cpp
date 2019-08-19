#include "TRAPPCH.h"
#include "OpenGLRenderer.h"

#include "OpenGLCommon.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/IndexBuffer.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLRenderer::OpenGLRenderer()
	: m_context(OpenGLContext::Get())
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLRenderer::~OpenGLRenderer()
{
	TP_DEBUG("[Renderer][OpenGL] Destroying Renderer");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::InitInternal()
{
#ifdef TRAP_DEBUG
	InitDebug();
#endif

	SetClearColor();
	SetDepthTesting(true);
	SetBlend(true);
	SetBlendFunction(RendererBlendFunction::Source_Alpha, RendererBlendFunction::One_Minus_Source_Alpha);
	SetCull(true);
	SetFrontFace(RendererFrontFace::Counter_Clockwise);
	SetCullMode(RendererCullMode::Back);

	TP_INFO("[Renderer][OpenGL] ----------------------------------");
	TP_INFO("[Renderer][OpenGL] OpenGL:");
	TP_INFO("[Renderer][OpenGL] Version:  ", glGetString(GL_VERSION));
	TP_INFO("[Renderer][OpenGL] Vendor:   ", glGetString(GL_VENDOR));
	TP_INFO("[Renderer][OpenGL] Renderer: ", glGetString(GL_RENDERER));
	TP_INFO("[Renderer][OpenGL] ----------------------------------");

	m_rendererTitle = "[OpenGL " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)) + std::string("]"));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::Clear(const unsigned int buffer)
{	
	OpenGLCall(glClear(TRAPRendererBufferToOpenGL(buffer)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::Present(Window* window)
{
	OpenGLContext::Present(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetClearColor(const Math::Vec4& color)
{
	OpenGLCall(glClearColor(color.x, color.y, color.z, color.w));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetDepthTesting(const bool enabled)
{
	if (enabled)
	{
		OpenGLCall(glEnable(GL_DEPTH_TEST));
	}
	else
	{
		OpenGLCall(glDisable(GL_DEPTH_TEST));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetBlend(const bool enabled)
{
	if (enabled)
	{
		OpenGLCall(glEnable(GL_BLEND));
	}
	else
	{
		OpenGLCall(glDisable(GL_BLEND));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetCull(const bool enabled)
{
	if(enabled)
	{
		OpenGLCall(glEnable(GL_CULL_FACE));
	}
	else
	{
		OpenGLCall(glDisable(GL_CULL_FACE));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetFrontFace(const RendererFrontFace frontFace)
{
	OpenGLCall(glFrontFace(TRAPRendererFrontFaceToOpenGL(frontFace)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetWireFrame(const bool enabled)
{
	if (enabled)
	{
		OpenGLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
	else
	{
		OpenGLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination)
{
	OpenGLCall(glBlendFunc(TRAPRendererBlendFunctionToOpenGL(source), TRAPRendererBlendFunctionToOpenGL(destination)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetBlendEquation(const RendererBlendEquation blendEquation)
{
	OpenGLCall(glBlendEquation(TRAPRendererBlendEquationToOpenGL(blendEquation)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetViewport(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height)
{
	OpenGLCall(glViewport(x, y, width, height));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetCullMode(const RendererCullMode cullMode)
{
	OpenGLCall(glCullFace(TRAPRendererCullModeToOpenGL(cullMode)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::DrawIndexed(const std::unique_ptr<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
	OpenGLCall(glDrawElements(TRAPRendererPrimitiveToOpenGL(primitive), vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));
}

//-------------------------------------------------------------------------------------------------------------------//

unsigned int TRAP::Graphics::API::OpenGLRenderer::TRAPRendererBufferToOpenGL(const unsigned int buffer)
{
	unsigned int result = 0;

	if (buffer & static_cast<int>(RENDERER_BUFFER_COLOR))
		result |= GL_COLOR_BUFFER_BIT;
	if (buffer & static_cast<int>(RENDERER_BUFFER_DEPTH))
		result |= GL_DEPTH_BUFFER_BIT;
	if (buffer & static_cast<int>(RENDERER_BUFFER_STENCIL))
		result |= GL_STENCIL_BUFFER_BIT;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

unsigned int TRAP::Graphics::API::OpenGLRenderer::TRAPRendererBlendFunctionToOpenGL(const RendererBlendFunction function)
{
	switch (function)
	{
	case RendererBlendFunction::Zero:
		return GL_ZERO;

	case RendererBlendFunction::One:
		return GL_ONE;

	case RendererBlendFunction::Source_Alpha:
		return GL_SRC_ALPHA;

	case RendererBlendFunction::Destination_Alpha:
		return GL_DST_ALPHA;

	case RendererBlendFunction::One_Minus_Source_Alpha:
		return GL_ONE_MINUS_SRC_ALPHA;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

unsigned int TRAP::Graphics::API::OpenGLRenderer::TRAPRendererBlendEquationToOpenGL(const RendererBlendEquation blendEquation)
{
	switch (blendEquation)
	{
	case RendererBlendEquation::Add:
		return GL_FUNC_ADD;

	case RendererBlendEquation::Subtract:
		return GL_FUNC_SUBTRACT;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

unsigned int TRAP::Graphics::API::OpenGLRenderer::TRAPRendererCullModeToOpenGL(const RendererCullMode cullMode)
{
	switch(cullMode)
	{
	case RendererCullMode::NONE:
		return 0;

	case RendererCullMode::Front:
		return GL_FRONT;

	case RendererCullMode::Back:
		return GL_BACK;

	case RendererCullMode::Front_And_Back:
		return GL_FRONT_AND_BACK;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

unsigned int TRAP::Graphics::API::OpenGLRenderer::TRAPRendererFrontFaceToOpenGL(const RendererFrontFace frontFace)
{
	switch(frontFace)
	{
	case RendererFrontFace::Clockwise:
		return GL_CW;

	case RendererFrontFace::Counter_Clockwise:
		return GL_CCW;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

unsigned int TRAP::Graphics::API::OpenGLRenderer::TRAPRendererPrimitiveToOpenGL(const RendererPrimitive primitive)
{
	switch(primitive)
	{
	case RendererPrimitive::Point:
		return GL_POINTS;

	case RendererPrimitive::Line:
		return GL_LINES;

	case RendererPrimitive::Triangle:
		return GL_TRIANGLES;

	case RendererPrimitive::Patch:
		return GL_PATCHES;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void GLAPIENTRY TRAP::Graphics::API::OpenGLRenderer::DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::stringstream ss;
	ss << "[Renderer][OpenGL] [";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		ss << "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		ss << "WINDOW SYSTEM";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		ss << "SHADER COMPILER";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		ss << "THIRD PARTY";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		ss << "APPLICATION";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		ss << "UNKNOWN";
		break;

	default:
		ss << "UNKNOWN";
		break;
	}

	ss << "][";

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		ss << "ERROR";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		ss << "DEPRECATED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		ss << "UNDEFINED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		ss << "PORTABILITY";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		ss << "PERFORMANCE";
		break;

	case GL_DEBUG_TYPE_OTHER:
		ss << "OTHER";
		break;

	case GL_DEBUG_TYPE_MARKER:
		ss << "MARKER";
		break;

	default:
		ss << "UNKNOWN";
		break;
	}

	ss << "] " << id << ' ' << message;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		TP_CRITICAL(ss.str());
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		TP_ERROR(ss.str());
		break;

	case GL_DEBUG_SEVERITY_LOW:
		TP_WARN(ss.str());
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		break;

	default:
		TP_WARN(ss.str());
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::InitDebug()
{
	OpenGLCall(glEnable(GL_DEBUG_OUTPUT));
	OpenGLCall(glDebugMessageCallback(DebugCallback, nullptr));
}
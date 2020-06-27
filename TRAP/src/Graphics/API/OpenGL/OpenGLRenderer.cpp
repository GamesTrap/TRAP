#include "TRAPPCH.h"
#include "OpenGLRenderer.h"

#include "OpenGLCommon.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Textures/Texture.h"

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLRenderer::s_maxCombinedTextureUnits = 0;
std::unordered_map<uint32_t, const TRAP::Graphics::Texture*> TRAP::Graphics::API::OpenGLRenderer::s_boundTextures{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLRenderer::OpenGLRenderer()
	: m_context(OpenGLContext::Get())
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLRenderer::~OpenGLRenderer()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG("[Renderer][OpenGL] Destroying Renderer");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::InitInternal()
{
	TP_PROFILE_FUNCTION();
	
#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	InitDebug();
#endif

	SetClearColor();
	SetDepthTesting(true);
	SetBlend(true);
	SetBlendFunction(RendererBlendFunction::Source_Alpha, RendererBlendFunction::One_Minus_Source_Alpha);
	SetCull(true);
	SetFrontFace(RendererFrontFace::Counter_Clockwise);
	SetCullMode(RendererFaceMode::Back);

	TP_INFO("[Renderer][OpenGL] ----------------------------------");
	TP_INFO("[Renderer][OpenGL] OpenGL:");
	TP_INFO("[Renderer][OpenGL] Version:  ", glGetString(GL_VERSION));
	TP_INFO("[Renderer][OpenGL] GLSL Version: ", glGetString(GL_SHADING_LANGUAGE_VERSION));
	TP_INFO("[Renderer][OpenGL] Vendor:   ", glGetString(GL_VENDOR));
	TP_INFO("[Renderer][OpenGL] Renderer: ", glGetString(GL_RENDERER));
	TP_INFO("[Renderer][OpenGL] ----------------------------------");

	m_rendererTitle = "[OpenGL " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)) + std::string("]"));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::Clear(const RendererBufferType buffer)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glClear(TRAPRendererBufferToOpenGL(buffer)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::Present(const Scope<Window>& window)
{
	TP_PROFILE_FUNCTION();

	OpenGLContext::Present(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetClearColor(const Math::Vec4& color)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glClearColor(color.x, color.y, color.z, color.w));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetDepthTesting(const bool enabled)
{
	TP_PROFILE_FUNCTION();

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

void TRAP::Graphics::API::OpenGLRenderer::SetDepthMasking(const bool enabled)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glDepthMask(enabled));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetDepthFunction(const RendererFunction function)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glDepthFunc(TRAPRendererFunctionToOpenGL(function)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetStencilTesting(const bool enabled)
{
	TP_PROFILE_FUNCTION();

	if(enabled)
	{
		OpenGLCall(glEnable(GL_STENCIL_TEST));
	}
	else
	{
		OpenGLCall(glDisable(GL_STENCIL_TEST));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetStencilMasking(const uint32_t mask)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glStencilMask(mask));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetStencilMaskingSeparate(const RendererFaceMode face, const uint32_t mask)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glStencilMaskSeparate(TRAPRendererFaceModeToOpenGL(face), mask));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetStencilFunction(const RendererFunction function, const int32_t reference, const uint32_t mask)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glStencilFunc(TRAPRendererFunctionToOpenGL(function), reference, mask));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetStencilFunctionSeparate(const RendererFaceMode face, const RendererFunction function, const int32_t reference, const uint32_t mask)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glStencilFuncSeparate(TRAPRendererFaceModeToOpenGL(face), TRAPRendererFunctionToOpenGL(function), reference, mask));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetStencilOperation(const RendererOperation stencilFail, const RendererOperation depthFail, const RendererOperation pass)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glStencilOp(TRAPRendererOperationToOpenGL(stencilFail), TRAPRendererOperationToOpenGL(depthFail), TRAPRendererOperationToOpenGL(pass)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetStencilOperationSeparate(const RendererFaceMode face, const RendererOperation stencilFail, const RendererOperation depthFail, const RendererOperation pass)
{
	TP_PROFILE_FUNCTION();
	
	OpenGLCall(glStencilOpSeparate(TRAPRendererFaceModeToOpenGL(face), TRAPRendererOperationToOpenGL(stencilFail), TRAPRendererOperationToOpenGL(depthFail), TRAPRendererOperationToOpenGL(pass)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetBlend(const bool enabled)
{
	TP_PROFILE_FUNCTION();

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
	TP_PROFILE_FUNCTION();

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
	TP_PROFILE_FUNCTION();

	OpenGLCall(glFrontFace(TRAPRendererFrontFaceToOpenGL(frontFace)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glViewport(x, y, width, height));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glBlendFunc(TRAPRendererBlendFunctionToOpenGL(source), TRAPRendererBlendFunctionToOpenGL(destination)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetBlendFunctionSeparate(const RendererBlendFunction sourceRGB,
                                                                   const RendererBlendFunction sourceAlpha,
                                                                   const RendererBlendFunction destinationRGB,
                                                                   const RendererBlendFunction destinationAlpha)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glBlendFuncSeparate(TRAPRendererBlendFunctionToOpenGL(sourceRGB),
		                           TRAPRendererBlendFunctionToOpenGL(destinationRGB),
		                           TRAPRendererBlendFunctionToOpenGL(sourceAlpha),
		                           TRAPRendererBlendFunctionToOpenGL(destinationAlpha)
	));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetBlendEquation(const RendererBlendEquation blendEquation)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glBlendEquation(TRAPRendererBlendEquationToOpenGL(blendEquation)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetBlendEquationSeparate(const RendererBlendEquation blendEquationRGB, const RendererBlendEquation blendEquationAlpha)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glBlendEquationSeparate(TRAPRendererBlendEquationToOpenGL(blendEquationRGB), TRAPRendererBlendEquationToOpenGL(blendEquationAlpha)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetCullMode(const RendererFaceMode cullMode)
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(glCullFace(TRAPRendererFaceModeToOpenGL(cullMode)));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::SetWireFrame(const bool enabled)
{
	TP_PROFILE_FUNCTION();

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

void TRAP::Graphics::API::OpenGLRenderer::DrawIndexed(const Scope<VertexArray>& vertexArray, const uint32_t indexCount, const RendererPrimitive primitive)
{
	TP_PROFILE_FUNCTION();

	const uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	OpenGLCall(glDrawElementsBaseVertex(TRAPRendererPrimitiveToOpenGL(primitive), count, GL_UNSIGNED_INT, nullptr, 0));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLRenderer::Draw(const Scope<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
	TP_PROFILE_FUNCTION();

	uint32_t count = 0;
	for (const Scope<VertexBuffer>& vertexBuffer : vertexArray->GetVertexBuffers())
		//Amount of indices = Total Vertex count divided by the stride of the data layout
		count += vertexBuffer->GetVertexCount() / vertexBuffer->GetLayout().GetStride();
	
	OpenGLCall(glDrawArrays(TRAPRendererPrimitiveToOpenGL(primitive), 0, count));
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::OpenGLRenderer::GetTitle() const
{
	TP_PROFILE_FUNCTION();

	return m_rendererTitle;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Graphics::API::OpenGLRenderer::GetCurrentGPUUUID()
{
	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::OpenGLRenderer::GetCurrentGPUName()
{
	TP_PROFILE_FUNCTION();

	OpenGLCall(const GLubyte* renderer = glGetString(GL_RENDERER));
	return std::string(reinterpret_cast<const char*>(renderer));
}


//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLRenderer* TRAP::Graphics::API::OpenGLRenderer::Get()
{
	TP_PROFILE_FUNCTION();

	return dynamic_cast<OpenGLRenderer*>(s_Renderer.get());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLRenderer::TRAPRendererBufferToOpenGL(const RendererBufferType buffer)
{
	TP_PROFILE_FUNCTION();

	uint32_t result = 0;

	if (buffer == RendererBufferType::Color)
		result = GL_COLOR_BUFFER_BIT;
	else if (buffer == RendererBufferType::Depth)
		result = GL_DEPTH_BUFFER_BIT;
	else if (buffer == RendererBufferType::Stencil)
		result = GL_STENCIL_BUFFER_BIT;
	else if (buffer == RendererBufferType::Color_Depth)
		result = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	else if (buffer == RendererBufferType::Color_Depth_Stencil)
		result = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
	else if (buffer == RendererBufferType::Depth_Stencil)
		result = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;	

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLRenderer::TRAPRendererBlendFunctionToOpenGL(const RendererBlendFunction function)
{
	TP_PROFILE_FUNCTION();

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

uint32_t TRAP::Graphics::API::OpenGLRenderer::TRAPRendererBlendEquationToOpenGL(const RendererBlendEquation blendEquation)
{
	TP_PROFILE_FUNCTION();

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

uint32_t TRAP::Graphics::API::OpenGLRenderer::TRAPRendererFaceModeToOpenGL(const RendererFaceMode cullMode)
{
	TP_PROFILE_FUNCTION();

	switch(cullMode)
	{
	case RendererFaceMode::Front:
		return GL_FRONT;

	case RendererFaceMode::Back:
		return GL_BACK;

	case RendererFaceMode::Front_And_Back:
		return GL_FRONT_AND_BACK;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLRenderer::TRAPRendererFrontFaceToOpenGL(const RendererFrontFace frontFace)
{
	TP_PROFILE_FUNCTION();

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

uint32_t TRAP::Graphics::API::OpenGLRenderer::TRAPRendererPrimitiveToOpenGL(const RendererPrimitive primitive)
{
	TP_PROFILE_FUNCTION();

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

uint32_t TRAP::Graphics::API::OpenGLRenderer::TRAPRendererFunctionToOpenGL(const RendererFunction function)
{
	TP_PROFILE_FUNCTION();

	switch(function)
	{
	case RendererFunction::Always:
		return GL_ALWAYS;

	case RendererFunction::Never:
		return GL_NEVER;

	case RendererFunction::Less:
		return GL_LESS;

	case RendererFunction::Equal:
		return GL_EQUAL;

	case RendererFunction::Less_Equal:
		return GL_LEQUAL;

	case RendererFunction::Greater:
		return GL_GREATER;

	case RendererFunction::Not_Equal:
		return GL_NOTEQUAL;

	case RendererFunction::Greater_Equal:
		return GL_GEQUAL;
		
	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLRenderer::TRAPRendererOperationToOpenGL(RendererOperation operation)
{
	TP_PROFILE_FUNCTION();

	switch(operation)
	{
	case RendererOperation::Keep:
		return GL_KEEP;

	case RendererOperation::Zero:
		return GL_ZERO;

	case RendererOperation::Replace:
		return GL_REPLACE;

	case RendererOperation::Increment:
		return GL_INCR;

	case RendererOperation::Increment_Wrap:
		return GL_INCR_WRAP;

	case RendererOperation::Decrement:
		return GL_DECR;

	case RendererOperation::Decrement_Wrap:
		return GL_DECR_WRAP;

	case RendererOperation::Invert:
		return GL_INVERT;
		
	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLRenderer::GetMaxTextureUnits()
{
	if(s_maxCombinedTextureUnits == 0)
	{
		OpenGLCall(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, reinterpret_cast<int32_t*>(&s_maxCombinedTextureUnits)));
	}

	return s_maxCombinedTextureUnits;
}

//-------------------------------------------------------------------------------------------------------------------//

std::unordered_map<uint32_t, const TRAP::Graphics::Texture*>& TRAP::Graphics::API::OpenGLRenderer::GetBoundTextures()
{
	return s_boundTextures;
}

//-------------------------------------------------------------------------------------------------------------------//

void GLAPIENTRY TRAP::Graphics::API::OpenGLRenderer::DebugCallback(const GLenum source, const GLenum type, const GLuint id, const GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
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
	OpenGLCall(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
	OpenGLCall(glDebugMessageCallback(DebugCallback, nullptr));
}
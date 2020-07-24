#include "TRAPPCH.h"
#include "OpenGLCommon.h"

void TRAP::Graphics::API::OpenGLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLLogCall(const char* function, const char* file, const int32_t line)
{
	while (const GLenum error = glGetError())
	{
		TP_ERROR(Log::RendererOpenGLPrefix, error, ": ", function, " @[", file, ':', line, ']');

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

GLenum TRAP::Graphics::API::ShaderDataTypeToOpenGLBaseType(const ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return GL_FLOAT;
	case ShaderDataType::Float2:   return GL_FLOAT;
	case ShaderDataType::Float3:   return GL_FLOAT;
	case ShaderDataType::Float4:   return GL_FLOAT;

	case ShaderDataType::Mat3:     return GL_FLOAT;
	case ShaderDataType::Mat4:     return GL_FLOAT;

	case ShaderDataType::Int:      return GL_INT;
	case ShaderDataType::Int2:     return GL_INT;
	case ShaderDataType::Int3:     return GL_INT;
	case ShaderDataType::Int4:     return GL_INT;

	case ShaderDataType::Bool:     return GL_BOOL;

	default:
		TRAP_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
}
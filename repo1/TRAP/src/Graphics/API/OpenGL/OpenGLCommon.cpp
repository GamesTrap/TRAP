#include "TRAPPCH.h"
#include "OpenGLCommon.h"

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
#ifndef _TRAP_OPENGLCOMMON_H_
#define _TRAP_OPENGLCOMMON_H_

#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/BufferLayout.h"

namespace TRAP::Graphics::API
{
	void OpenGLClearError();

	bool OpenGLLogCall(const char* function, const char* file, int32_t line);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::OpenGLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::Graphics::API::OpenGLLogCall(const char* function, const char* file, const int32_t line)
{
	while (const GLenum error = glGetError())
	{
		TP_ERROR("[Renderer][OpenGL] ", error, ": ", function, " @[", file, ':', line, ']');

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP::Graphics
{
#ifdef TRAP_DEBUG
	#define OpenGLCall(x) ::TRAP::Graphics::API::OpenGLClearError(); \
				      x; \
					  ::TRAP::Graphics::API::OpenGLLogCall(#x, __FILE__, __LINE__)
#else
	#define OpenGLCall(x) x;
#endif
}

namespace TRAP::Graphics::API
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
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


}

#endif /*_TRAP_OPENGLCOMMON_H_*/
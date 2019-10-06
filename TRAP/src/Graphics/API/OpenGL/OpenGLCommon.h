#ifndef _TRAP_OPENGLCOMMON_H_
#define _TRAP_OPENGLCOMMON_H_

#include "Graphics/Buffers/BufferLayout.h"

namespace TRAP::Graphics::API
{
	void OpenGLClearError();
	bool OpenGLLogCall(const char* function, const char* file, int32_t line);

	GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type);
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

#endif /*_TRAP_OPENGLCOMMON_H_*/
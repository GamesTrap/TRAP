#pragma once

namespace TRAP::Graphics::API
{
	inline void OpenGLClearError()
	{
		while (glGetError() != GL_NO_ERROR);
	}

	inline bool OpenGLLogCall(const char* function, const char* file, const int line)
	{
		while (const GLenum error = glGetError())
		{
			TP_ERROR("[Renderer][OpenGL] ", error, ": ", function, " @[", file, ':', line, ']');

			return false;
		}

		return true;
	}
}

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
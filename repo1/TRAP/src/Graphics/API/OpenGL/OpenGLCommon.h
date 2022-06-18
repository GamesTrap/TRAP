#ifndef _TRAP_OPENGLCOMMON_H_
#define _TRAP_OPENGLCOMMON_H_

#include "Graphics/Buffers/BufferLayout.h"

namespace TRAP::Graphics::API
{
	GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type);
}

#endif /*_TRAP_OPENGLCOMMON_H_*/
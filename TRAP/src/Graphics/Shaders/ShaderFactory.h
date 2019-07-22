#ifndef _TRAP_SHADERFACTORY_H_
#define _TRAP_SHADERFACTORY_H_

#include "Shader.h"

namespace TRAP::Graphics::ShaderFactory
{
	std::unique_ptr<TRAP::Graphics::API::Shader> PassthroughShader();
}

#endif /*_TRAP_SHADERFACTORY_H_*/
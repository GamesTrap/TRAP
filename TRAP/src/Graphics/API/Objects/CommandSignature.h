#ifndef _TRAP_COMMANDSIGNATURE_H_
#define _TRAP_COMMANDSIGNATURE_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class CommandSignature
	{
	public:
		static TRAP::Ref<CommandSignature> Create(const RendererAPI::CommandSignatureDesc& desc);
		
		virtual ~CommandSignature() = default;
	
	protected:
		CommandSignature() = default;
		
		//No Graphic API independent data
	};
}

#endif /*_TRAP_COMMANDSIGNATURE_H_*/
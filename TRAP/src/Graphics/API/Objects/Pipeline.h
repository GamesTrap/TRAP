#ifndef _TRAP_PIPELINE_H_
#define _TRAP_PIPELINE_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Pipeline
	{
	public:
		static TRAP::Ref<Pipeline> Create(const RendererAPI::PipelineDesc& desc);
		
		Pipeline() = default;
		virtual ~Pipeline() = default;
	
	protected:
		//No Graphic API independent data
	};
}

#endif /*_TRAP_PIPELINE_H_*/
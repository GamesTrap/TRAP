#ifndef _TRAP_PIPELINECACHE_H_
#define _TRAP_PIPELINECACHE_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class PipelineCache
	{
	public:
		static TRAP::Ref<PipelineCache> Create(const RendererAPI::PipelineCacheDesc& desc);
		
		virtual ~PipelineCache() = default;

		virtual void GetPipelineCacheData(std::size_t* size, void* data) const = 0;
	
	protected:
		PipelineCache() = default;
		
		//No Graphic API independent data
	};
}

#endif /*_TRAP_PIPELINECACHE_H_*/
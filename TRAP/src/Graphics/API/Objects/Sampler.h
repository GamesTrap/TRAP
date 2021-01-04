#ifndef _TRAP_SAMPLER_H_
#define _TRAP_SAMPLER_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Sampler
	{
	public:
		static TRAP::Ref<Sampler> Create(const RendererAPI::SamplerDesc& desc);
		
		Sampler() = default;
		virtual ~Sampler() = default;
	
	protected:
		//No Graphic API independent data
	};
}

#endif /*_TRAP_SAMPLER_H_*/
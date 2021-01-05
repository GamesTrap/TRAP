#ifndef _TRAP_DESCRIPTORSET_H_
#define _TRAP_DESCRIPTORSET_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorSet
	{
	public:
		virtual ~DescriptorSet() = default;

		virtual void Update(uint32_t index, uint32_t count, const std::vector<RendererAPI::DescriptorData>& params) = 0;
	
	protected:
		DescriptorSet() = default;
	};
}

#endif /*_TRAP_DESCRIPTORSET_H_*/
#ifndef _TRAP_DESCRIPTORSET_H_
#define _TRAP_DESCRIPTORSET_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorSet
	{
	public:
		virtual ~DescriptorSet();

		virtual void Update(uint32_t index, const std::vector<RendererAPI::DescriptorData>& params) = 0;

	protected:
		DescriptorSet();
	};
}

#endif /*_TRAP_DESCRIPTORSET_H_*/
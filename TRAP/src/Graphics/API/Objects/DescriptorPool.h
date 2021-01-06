#ifndef _TRAP_DESCRIPTORPOOL_H_
#define _TRAP_DESCRIPTORPOOL_H_

#include "DescriptorSet.h"
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorPool
	{
	public:
		static TRAP::Ref<DescriptorPool> Create(uint32_t numDescriptorSets);
		
		virtual ~DescriptorPool() = default;

		virtual void Reset() = 0;
		
		virtual uint32_t GetDescriptorSetsNum() const;

		virtual DescriptorSet* RetrieveDescriptorSet(const RendererAPI::DescriptorSetDesc& desc) = 0;
	
	protected:
		DescriptorPool();
		
		uint32_t m_numDescriptorSets;
	};
}

#endif /*_TRAP_DESCRIPTORPOOL_H_*/
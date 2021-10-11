#ifndef _TRAP_DESCRIPTORPOOL_H_
#define _TRAP_DESCRIPTORPOOL_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorPool
	{
	public:
		static TRAP::Ref<DescriptorPool> Create(uint32_t numDescriptorSets);

		virtual ~DescriptorPool();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		DescriptorPool(const DescriptorPool&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		DescriptorPool& operator=(const DescriptorPool&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		DescriptorPool(DescriptorPool&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		DescriptorPool& operator=(DescriptorPool&&) = default;

		virtual void Reset() = 0;

		virtual uint32_t GetDescriptorSetsNum() const;

		virtual DescriptorSet* RetrieveDescriptorSet(const RendererAPI::DescriptorSetDesc& desc) = 0;

	protected:
		DescriptorPool();

		uint32_t m_numDescriptorSets;
	};
}

#endif /*_TRAP_DESCRIPTORPOOL_H_*/
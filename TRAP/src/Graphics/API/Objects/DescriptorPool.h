#ifndef TRAP_DESCRIPTORPOOL_H
#define TRAP_DESCRIPTORPOOL_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorPool
	{
	public:
		static TRAP::Ref<DescriptorPool> Create(uint32_t numDescriptorSets);

		virtual ~DescriptorPool();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		DescriptorPool(const DescriptorPool&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		DescriptorPool& operator=(const DescriptorPool&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		DescriptorPool(DescriptorPool&&) = default;
		/// <summary>
		/// Move assignment operator.
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

#endif /*TRAP_DESCRIPTORPOOL_H*/
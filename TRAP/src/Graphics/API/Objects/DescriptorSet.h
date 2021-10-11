#ifndef _TRAP_DESCRIPTORSET_H_
#define _TRAP_DESCRIPTORSET_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorSet
	{
	public:
		virtual ~DescriptorSet();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		DescriptorSet(const DescriptorSet&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		DescriptorSet& operator=(const DescriptorSet&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		DescriptorSet(DescriptorSet&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		DescriptorSet& operator=(DescriptorSet&&) = default;


		virtual void Update(uint32_t index, const std::vector<RendererAPI::DescriptorData>& params) = 0;

	protected:
		DescriptorSet();
	};
}

#endif /*_TRAP_DESCRIPTORSET_H_*/
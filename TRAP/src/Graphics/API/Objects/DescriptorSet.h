#ifndef TRAP_DESCRIPTORSET_H
#define TRAP_DESCRIPTORSET_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorSet
	{
	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~DescriptorSet();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		DescriptorSet(const DescriptorSet&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		DescriptorSet& operator=(const DescriptorSet&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		DescriptorSet(DescriptorSet&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		DescriptorSet& operator=(DescriptorSet&&) = default;

		/// <summary>
		/// Update the descriptor set.
		/// </summary>
		/// <param name="index">Index of the set to update.</param>
		/// <param name="params">Data to update.</param>
		virtual void Update(uint32_t index, const std::vector<RendererAPI::DescriptorData>& params) = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		DescriptorSet();
	};
}

#endif /*TRAP_DESCRIPTORSET_H*/
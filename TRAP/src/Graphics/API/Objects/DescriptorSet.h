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
		constexpr virtual ~DescriptorSet() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr DescriptorSet(const DescriptorSet&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr DescriptorSet& operator=(const DescriptorSet&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr DescriptorSet(DescriptorSet&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr DescriptorSet& operator=(DescriptorSet&&) noexcept = default;

		/// <summary>
		/// Update the descriptor set.
		/// </summary>
		/// <param name="index">Index of the set to update.</param>
		/// <param name="params">Data to update.</param>
		virtual void Update(u32 index, const std::vector<RendererAPI::DescriptorData>& params) = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		constexpr DescriptorSet() noexcept = default;
	};
}

#endif /*TRAP_DESCRIPTORSET_H*/
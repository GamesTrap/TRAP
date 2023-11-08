#ifndef TRAP_DESCRIPTORSET_H
#define TRAP_DESCRIPTORSET_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorSet
	{
	public:
		/// @brief Destructor.
		constexpr virtual ~DescriptorSet() = default;

		/// @brief Copy constructor.
		consteval DescriptorSet(const DescriptorSet&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval DescriptorSet& operator=(const DescriptorSet&) noexcept = delete;
		/// @brief Move constructor.
		constexpr DescriptorSet(DescriptorSet&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr DescriptorSet& operator=(DescriptorSet&&) noexcept = default;

		/// @brief Update the descriptor set.
		/// @param index Index of the set to update.
		/// @param params Data to update.
		virtual void Update(u32 index, const std::vector<RendererAPI::DescriptorData>& params) = 0;

	protected:
		/// @brief Constructor.
		constexpr DescriptorSet() noexcept = default;
	};
}

#endif /*TRAP_DESCRIPTORSET_H*/
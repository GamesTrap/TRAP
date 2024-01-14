#ifndef TRAP_DESCRIPTORPOOL_H
#define TRAP_DESCRIPTORPOOL_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorPool
	{
	public:
		/// @brief Create a new descriptor pool.
		/// @param numDescriptorSets Max amount of descriptors sets to manage by the pool.
		/// @return Created descriptor pool.
		[[nodiscard]] static TRAP::Ref<DescriptorPool> Create(u32 numDescriptorSets);

		/// @brief Destructor.
		virtual ~DescriptorPool();

		/// @brief Copy constructor.
		consteval DescriptorPool(const DescriptorPool&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval DescriptorPool& operator=(const DescriptorPool&) noexcept = delete;
		/// @brief Move constructor.
		constexpr DescriptorPool(DescriptorPool&&) noexcept = default;
		/// @brief Move assignment operator.
		DescriptorPool& operator=(DescriptorPool&&) noexcept = default;

		/// @brief Reset the descriptor pool.
		/// @note This implicitly frees all descriptor sets allocated from the pool.
		virtual void Reset() = 0;

		/// @brief Max number of descriptor sets managed by the pool.
		/// @return Number of descriptor sets.
		[[nodiscard]] constexpr u32 GetDescriptorSetsNum() const noexcept;

		/// @brief Retrieve a new descriptor set from description.
		/// @param desc Descriptor set description.
		/// @return New descriptor set.
		[[nodiscard]] virtual TRAP::Scope<DescriptorSet> RetrieveDescriptorSet(const RendererAPI::DescriptorSetDesc& desc) = 0;

	protected:
		/// @brief Constructor.
		explicit DescriptorPool(u32 numDescriptorSets);

		u32 m_numDescriptorSets = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::DescriptorPool::GetDescriptorSetsNum() const noexcept
{
	return m_numDescriptorSets;
}

#endif /*TRAP_DESCRIPTORPOOL_H*/

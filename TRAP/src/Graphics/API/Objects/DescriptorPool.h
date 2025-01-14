#ifndef TRAP_DESCRIPTORPOOL_H
#define TRAP_DESCRIPTORPOOL_H

#include "Core/Types.h"
#include "Core/Base.h"
#include "Utils/SmartPtr.h"
#include "TRAP_Assert.h"

namespace TRAP::Graphics
{
	class DescriptorSet;
	struct DescriptorSetDesc;

	class DescriptorPool
	{
	public:
		/// @brief Create a new descriptor pool.
		/// @param numDescriptorSets Max amount of descriptors sets to be managed by the pool.
		/// @param name Optional: Name for the descriptor pool.
		/// @return Created descriptor pool.
		[[nodiscard]] static TRAP::Ref<DescriptorPool> Create(u32 numDescriptorSets, [[maybe_unused]] const std::string& name = "");

		/// @brief Destructor.
		constexpr virtual ~DescriptorPool();

		/// @brief Copy constructor.
		consteval DescriptorPool(const DescriptorPool&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval DescriptorPool& operator=(const DescriptorPool&) noexcept = delete;
		/// @brief Move constructor.
		constexpr DescriptorPool(DescriptorPool&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr DescriptorPool& operator=(DescriptorPool&&) noexcept = default;

		/// @brief Reset the descriptor pool.
		/// @note This implicitly frees all descriptor sets allocated from the pool.
		virtual void Reset() = 0;

		/// @brief Max number of descriptor sets managed by the pool.
		/// @return Number of descriptor sets.
		[[nodiscard]] constexpr u32 GetDescriptorSetsNum() const noexcept;

		/// @brief Retrieve a new descriptor set from description.
		/// @param desc Descriptor set description.
		/// @return New descriptor set.
		[[nodiscard]] virtual TRAP::Scope<DescriptorSet> RetrieveDescriptorSet(const DescriptorSetDesc& desc) = 0;

	protected:
		/// @brief Constructor.
		/// @param numDescriptorSets Max amount of descriptors sets to be managed by the pool.
		constexpr explicit DescriptorPool(u32 numDescriptorSets);

		u32 m_numDescriptorSets = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::DescriptorPool::~DescriptorPool()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Destroying DescriptorPool");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::DescriptorPool::DescriptorPool(const u32 numDescriptorSets)
	: m_numDescriptorSets(numDescriptorSets)
{
	TRAP_ASSERT(numDescriptorSets > 0, "DescriptorPool::DescriptorPool(): m_numDescriptorSets is 0");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Creating DescriptorPool");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}


//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::DescriptorPool::GetDescriptorSetsNum() const noexcept
{
	return m_numDescriptorSets;
}

#endif /*TRAP_DESCRIPTORPOOL_H*/

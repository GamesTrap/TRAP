#ifndef TRAP_DESCRIPTORPOOL_H
#define TRAP_DESCRIPTORPOOL_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorPool
	{
	public:
		/// <summary>
		/// Create a new descriptor pool.
		/// </summary>
		/// <param name="numDescriptorSets">Max amount of descriptors sets to manage by the pool.</param>
		/// <returns>Created descriptor pool.</returns>
		[[nodiscard]] static TRAP::Ref<DescriptorPool> Create(uint32_t numDescriptorSets);

		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr virtual ~DescriptorPool();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		DescriptorPool(const DescriptorPool&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		DescriptorPool& operator=(const DescriptorPool&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		DescriptorPool(DescriptorPool&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		DescriptorPool& operator=(DescriptorPool&&) noexcept = default;

		/// <summary>
		/// Reset the descriptor pool.
		/// Note: This implicitly frees all descriptor sets allocated from the pool.
		/// </summary>
		virtual void Reset() = 0;

		/// <summary>
		/// Max number of descriptor sets managed by the pool.
		/// </summary>
		/// <returns>
		[[nodiscard]] uint32_t GetDescriptorSetsNum() const noexcept;

		/// <summary>
		/// Retrieve a new descriptor set from description.
		/// </summary>
		/// <param name="desc">Descriptor set description.</param>
		/// <returns>New descriptor set.</returns>
		[[nodiscard]] virtual TRAP::Scope<DescriptorSet> RetrieveDescriptorSet(const RendererAPI::DescriptorSetDesc& desc) = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		DescriptorPool();

		uint32_t m_numDescriptorSets;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::DescriptorPool::~DescriptorPool()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorPoolPrefix, "Destroying DescriptorPool");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

#endif /*TRAP_DESCRIPTORPOOL_H*/
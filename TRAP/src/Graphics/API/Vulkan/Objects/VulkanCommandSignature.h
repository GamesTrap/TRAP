#ifndef TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H
#define TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/CommandSignature.h"

namespace TRAP::Graphics::API
{
	class VulkanCommandSignature final : public CommandSignature
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Command signature description.</param>
		explicit VulkanCommandSignature(const RendererAPI::CommandSignatureDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanCommandSignature() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr VulkanCommandSignature(const VulkanCommandSignature&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanCommandSignature& operator=(const VulkanCommandSignature&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr VulkanCommandSignature(VulkanCommandSignature&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanCommandSignature& operator=(VulkanCommandSignature&&) noexcept = default;

		/// <summary>
		/// Retrieve the draw type used for indirect drawing.
		/// </summary>
		/// <returns>Draw type.</returns>
		[[nodiscard]] constexpr RendererAPI::IndirectArgumentType GetDrawType() const noexcept;
		/// <summary>
		/// Retrieve the byte stride between successive sets of draw parameters.
		/// </summary>
		/// <returns>Byte stride.</returns>
		[[nodiscard]] constexpr uint32_t GetStride() const noexcept;

	private:
		RendererAPI::IndirectArgumentType m_drawType{};
		uint32_t m_stride{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::IndirectArgumentType TRAP::Graphics::API::VulkanCommandSignature::GetDrawType() const noexcept
{
	return m_drawType;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::API::VulkanCommandSignature::GetStride() const noexcept
{
	return m_stride;
}

#endif /*TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H*/
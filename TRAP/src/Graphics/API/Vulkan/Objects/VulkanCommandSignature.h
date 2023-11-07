#ifndef TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H
#define TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/CommandSignature.h"

namespace TRAP::Graphics::API
{
	class VulkanCommandSignature final : public CommandSignature
	{
	public:
		/// @brief Constructor.
		/// @param desc Command signature description.
		explicit VulkanCommandSignature(const RendererAPI::CommandSignatureDesc& desc);
		/// @brief Destructor.
		~VulkanCommandSignature() override;

		/// @brief Copy constructor.
		constexpr VulkanCommandSignature(const VulkanCommandSignature&) noexcept = default;
		/// @brief Copy assignment operator.
		VulkanCommandSignature& operator=(const VulkanCommandSignature&) noexcept = default;
		/// @brief Move constructor.
		constexpr VulkanCommandSignature(VulkanCommandSignature&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanCommandSignature& operator=(VulkanCommandSignature&&) noexcept = default;

		/// @brief Retrieve the draw type used for indirect drawing.
		/// @return Draw type.
		[[nodiscard]] constexpr RendererAPI::IndirectArgumentType GetDrawType() const noexcept;
		/// @brief Retrieve the byte stride between successive sets of draw parameters.
		/// @return Byte stride.
		[[nodiscard]] constexpr u32 GetStride() const noexcept;

	private:
		RendererAPI::IndirectArgumentType m_drawType{};
		u32 m_stride{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::IndirectArgumentType TRAP::Graphics::API::VulkanCommandSignature::GetDrawType() const noexcept
{
	return m_drawType;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanCommandSignature::GetStride() const noexcept
{
	return m_stride;
}

#endif /*TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H*/
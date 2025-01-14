#ifndef TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H
#define TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H

#include "Core/Base.h"
#include "Core/Types.h"
#include "Graphics/API/Objects/CommandSignature.h"
#include "Graphics/API/RendererAPI/Types.h"

namespace TRAP::Graphics::API
{
	class VulkanCommandSignature final : public CommandSignature
	{
	public:
		/// @brief Constructor.
		/// @param desc Command signature description.
		constexpr explicit VulkanCommandSignature(const CommandSignatureDesc& desc);
		/// @brief Destructor.
		constexpr ~VulkanCommandSignature() override;

		/// @brief Copy constructor.
		consteval VulkanCommandSignature(const VulkanCommandSignature&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanCommandSignature& operator=(const VulkanCommandSignature&) noexcept = delete;
		/// @brief Move constructor.
		constexpr VulkanCommandSignature(VulkanCommandSignature&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr VulkanCommandSignature& operator=(VulkanCommandSignature&&) noexcept = default;

		/// @brief Retrieve the draw type used for indirect drawing.
		/// @return Draw type.
		[[nodiscard]] constexpr IndirectArgumentType GetDrawType() const noexcept;
		/// @brief Retrieve the byte stride between successive sets of draw parameters.
		/// @return Byte stride.
		[[nodiscard]] constexpr u32 GetStride() const noexcept;

	private:
		IndirectArgumentType m_drawType{};
		u32 m_stride{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::API::VulkanCommandSignature::VulkanCommandSignature(const CommandSignatureDesc& desc)
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandSignaturePrefix, "Creating CommandSignature");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	for(const auto& argDesc : desc.ArgDescs) //Counting for all types
	{
		switch(argDesc.Type)
		{
		case IndirectArgumentType::IndirectDraw:
			m_drawType = IndirectArgumentType::IndirectDraw;
			m_stride += sizeof(IndirectDrawArguments);
			break;

		case IndirectArgumentType::IndirectDrawIndex:
			m_drawType = IndirectArgumentType::IndirectDrawIndex;
			m_stride += sizeof(IndirectDrawIndexArguments);
			break;

		case IndirectArgumentType::IndirectDispatch:
			m_drawType = IndirectArgumentType::IndirectDispatch;
			m_stride += sizeof(IndirectDispatchArguments);
			break;

		default:
			TRAP_ASSERT(false, "VulkanCommandSignature(): Vulkan runtime only supports IndirectDraw, IndirectDrawIndex and IndirectDispatch "
			                   "at this point!");
			break;
		}
	}

	if (!desc.Packed)
		m_stride = ((m_stride + 16 - 1) / 16) * 16;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::API::VulkanCommandSignature::~VulkanCommandSignature()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandSignaturePrefix, "Destroying CommandSignature");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::IndirectArgumentType TRAP::Graphics::API::VulkanCommandSignature::GetDrawType() const noexcept
{
	return m_drawType;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanCommandSignature::GetStride() const noexcept
{
	return m_stride;
}

#endif /*TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H*/

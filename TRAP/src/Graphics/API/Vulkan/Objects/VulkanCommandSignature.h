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
		VulkanCommandSignature(const VulkanCommandSignature&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanCommandSignature& operator=(const VulkanCommandSignature&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanCommandSignature(VulkanCommandSignature&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanCommandSignature& operator=(VulkanCommandSignature&&) noexcept = default;

		/// <summary>
		/// Retrieve the draw type used for indirect drawing.
		/// </summary>
		/// <returns>Draw type.</returns>
		RendererAPI::IndirectArgumentType GetDrawType() const noexcept;
		/// <summary>
		/// Retrieve the byte stride between successive sets of draw parameters.
		/// </summary>
		/// <returns>Byte stride.</returns>
		uint32_t GetStride() const noexcept;

	private:
		RendererAPI::IndirectArgumentType m_drawType;
		uint32_t m_stride;
	};
}

#endif /*TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H*/
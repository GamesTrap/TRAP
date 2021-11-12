#ifndef TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H
#define TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/CommandSignature.h"

namespace TRAP::Graphics::API
{
	class VulkanCommandSignature final : public CommandSignature
	{
	public:
		explicit VulkanCommandSignature(const RendererAPI::CommandSignatureDesc& desc);
		~VulkanCommandSignature() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanCommandSignature(const VulkanCommandSignature&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanCommandSignature& operator=(const VulkanCommandSignature&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanCommandSignature(VulkanCommandSignature&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanCommandSignature& operator=(VulkanCommandSignature&&) = default;

		RendererAPI::IndirectArgumentType GetDrawType() const;
		uint32_t GetStride() const;

	private:
		RendererAPI::IndirectArgumentType m_drawType;
		uint32_t m_stride;
	};
}

#endif /*TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H*/
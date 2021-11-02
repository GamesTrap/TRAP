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
		/// Default Copy Constructor.
		/// </summary>
		VulkanCommandSignature(const VulkanCommandSignature&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VulkanCommandSignature& operator=(const VulkanCommandSignature&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VulkanCommandSignature(VulkanCommandSignature&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
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
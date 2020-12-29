#ifndef _TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H_
#define _TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanCommandSignature
	{
	public:
		explicit VulkanCommandSignature(const RendererAPI::CommandSignatureDesc& desc);
		~VulkanCommandSignature();

		RendererAPI::IndirectArgumentType GetDrawType() const;
		uint32_t GetStride() const;
		
	private:
		RendererAPI::IndirectArgumentType m_drawType;
		uint32_t m_stride;
	};
}

#endif /*_TRAP_VULKANINDIRECTCOMMANDSIGNATURE_H_*/
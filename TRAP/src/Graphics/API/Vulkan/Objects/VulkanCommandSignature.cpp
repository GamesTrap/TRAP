#include "TRAPPCH.h"
#include "VulkanCommandSignature.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandSignature::VulkanCommandSignature(const RendererAPI::CommandSignatureDesc& desc)
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandSignaturePrefix, "Creating CommandSignature");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	for(const auto& ArgDesc : desc.ArgDescs) //Counting for all types
	{
		switch(ArgDesc.Type)
		{
		case RendererAPI::IndirectArgumentType::IndirectDraw:
			m_drawType = RendererAPI::IndirectArgumentType::IndirectDraw;
			m_stride += sizeof(RendererAPI::IndirectDrawArguments);
			break;

		case RendererAPI::IndirectArgumentType::IndirectDrawIndex:
			m_drawType = RendererAPI::IndirectArgumentType::IndirectDrawIndex;
			m_stride += sizeof(RendererAPI::IndirectDrawIndexArguments);
			break;

		case RendererAPI::IndirectArgumentType::IndirectDispatch:
			m_drawType = RendererAPI::IndirectArgumentType::IndirectDispatch;
			m_stride += sizeof(RendererAPI::IndirectDispatchArguments);
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

TRAP::Graphics::API::VulkanCommandSignature::~VulkanCommandSignature()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandSignaturePrefix, "Destroying CommandSignature");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
}

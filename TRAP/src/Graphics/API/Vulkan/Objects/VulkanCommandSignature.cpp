#include "TRAPPCH.h"
#include "VulkanCommandSignature.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandSignature::VulkanCommandSignature(const CommandSignatureDesc& desc)
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

TRAP::Graphics::API::VulkanCommandSignature::~VulkanCommandSignature()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandSignaturePrefix, "Destroying CommandSignature");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
}

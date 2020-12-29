#include "TRAPPCH.h"
#include "VulkanCommandSignature.h"

TRAP::Graphics::API::VulkanCommandSignature::VulkanCommandSignature(const RendererAPI::CommandSignatureDesc& desc)
	: m_drawType(), m_stride()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandSignaturePrefix, "Creating CommandSignature");
#endif

	for(uint32_t i = 0; i < desc.IndirectArgCount; ++i) //Counting for all types
	{
		switch(desc.ArgDescs[i].Type)
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
			TP_ERROR(false, "Vulkan runtime only supports IndirectDraw, IndirectDrawIndex and IndirectDispatch at this point!");
			break;
		}
	}

	if (!desc.Packed)
		m_stride = ((m_stride + 16 - 1) / 16) * 16;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandSignature::~VulkanCommandSignature()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandSignaturePrefix, "Destroying CommandSignature");
#endif

	m_stride = 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::IndirectArgumentType TRAP::Graphics::API::VulkanCommandSignature::GetDrawType() const
{
	return m_drawType;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanCommandSignature::GetStride() const
{
	return m_stride;
}
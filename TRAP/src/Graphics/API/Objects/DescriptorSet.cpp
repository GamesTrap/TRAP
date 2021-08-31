#include "TRAPPCH.h"
#include "DescriptorSet.h"

TRAP::Graphics::DescriptorSet::DescriptorSet()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorSetPrefix, "Creating DescriptorSet");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::DescriptorSet::~DescriptorSet()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererDescriptorSetPrefix, "Destroying DescriptorSet");
#endif
}
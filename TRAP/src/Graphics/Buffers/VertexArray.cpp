#include "TRAPPCH.h"
#include "VertexArray.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/Vulkan/Buffers/VulkanVertexArray.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexArray> TRAP::Graphics::VertexArray::Create()
{
	TP_PROFILE_FUNCTION();

	switch(API::Context::GetRenderAPI())
	{
	case API::RenderAPI::Vulkan:
		return TRAP::MakeScope<API::VulkanVertexArray>();

	default:
		//Shouldn't be reached
		return nullptr;
	}
}
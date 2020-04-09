#include "TRAPPCH.h"
#include "VertexArray.h"

#include "Graphics/API/Context.h"
#include "Graphics/API/Vulkan/Buffers/VulkanVertexArray.h"
#include "Graphics/API/OpenGL/Buffers/OpenGLVertexArray.h"

const TRAP::Graphics::VertexArray* TRAP::Graphics::VertexArray::s_CurrentlyBound = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::VertexArray> TRAP::Graphics::VertexArray::Create()
{
	TP_PROFILE_FUNCTION();

	switch(API::Context::GetRenderAPI())
	{
	case API::RenderAPI::Vulkan:
		return TRAP::MakeScope<API::VulkanVertexArray>();

	case API::RenderAPI::OpenGL:
		return TRAP::MakeScope<API::OpenGLVertexArray>();

	default:
		//Shouldn't be reached
		return nullptr;
	}
}
#include "TRAPPCH.h"
#include "D3D12Renderer.h"

#include "Graphics/RenderCommand.h"

#ifdef TRAP_PLATFORM_WINDOWS

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Renderer::D3D12Renderer()
	: m_context(D3D12Context::Get())
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Renderer::~D3D12Renderer()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG("[Renderer][D3D12] Destroying Renderer");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::InitInternal()
{
	TP_PROFILE_FUNCTION();
	
	SetDepthTesting(true);
	SetBlendFunction(RendererBlendFunction::Source_Alpha, RendererBlendFunction::One_Minus_Source_Alpha);
	SetBlend(true);

	TP_INFO("[Renderer][D3D12] ----------------------------------");
	TP_INFO("[Renderer][D3D12] DirectX 12.1(D3D12):");
	TP_CRITICAL("[Renderer][D3D12] NOT IMPLEMENTED WIP");
	//Device, etc...
	TP_INFO("[Renderer][D3D12] ----------------------------------");

	m_rendererTitle = "[DirectX 12.1]";
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::Clear(RendererBufferType buffer)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::Present(const Scope<Window>& window)
{
	TP_PROFILE_FUNCTION();
	
	m_context->Present(window);
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetClearColor(const Math::Vec4& color)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetDepthTesting(bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetDepthMasking(bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetDepthFunction(RendererDepthFunction function)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlend(bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetCull(bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetFrontFace(RendererFrontFace frontFace)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetWireFrame(bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendFunctionSeparate(RendererBlendFunction sourceRGB,
	                                                              RendererBlendFunction sourceAlpha,
	                                                              RendererBlendFunction destinationRGB,
	                                                              RendererBlendFunction destinationAlpha)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendEquation(RendererBlendEquation blendEquation)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB, RendererBlendEquation blendEquationAlpha)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetCullMode(RendererCullMode cullMode)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::DrawIndexed(const Scope<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::Draw(const Scope<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::D3D12Renderer::GetTitle() const
{
	TP_PROFILE_FUNCTION();
	
	return m_rendererTitle;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Renderer* TRAP::Graphics::API::D3D12Renderer::Get()
{
	TP_PROFILE_FUNCTION();
	
	return dynamic_cast<D3D12Renderer*>(s_Renderer.get());
}

#endif
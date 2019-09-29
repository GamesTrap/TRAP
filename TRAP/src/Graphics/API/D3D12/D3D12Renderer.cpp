#include "TRAPPCH.h"
#include "D3D12Renderer.h"

#include "Graphics/RenderCommand.h"

#ifdef TRAP_PLATFORM_WINDOWS

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Renderer::D3D12Renderer()
	: m_context(D3D12Context::Get())
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Renderer::~D3D12Renderer()
{
	TP_DEBUG("[Renderer][D3D12] Destroying Renderer");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::InitInternal()
{
	SetDepthTesting(true);
	SetBlendFunction(RendererBlendFunction::Source_Alpha, RendererBlendFunction::One_Minus_Source_Alpha);
	SetBlend(true);

	TP_INFO("[Renderer][D3D12] ----------------------------------");
	TP_INFO("[Renderer][D3D12] DirectX 12.1(D3D12):");
	TP_CRITICAL("[Renderer][D3D12] NOT IMPLEMENTED WIP");
	//Device, etc...
	TP_INFO("[Renderer][D3D12] ----------------------------------");

	//Create Device
	m_rendererTitle = "[DirectX 12.1]";
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::Clear(RendererBufferType buffer)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::Present(const std::unique_ptr<Window>& window)
{
	m_context->Present(window);
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetClearColor(const Math::Vec4& color)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetDepthTesting(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetDepthMasking(bool enabled)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetDepthFunction(RendererDepthFunction function)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlend(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetCull(bool enabled)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetFrontFace(RendererFrontFace frontFace)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetWireFrame(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendFunctionSeparate(RendererBlendFunction sourceRGB,
	                                                              RendererBlendFunction sourceAlpha,
	                                                              RendererBlendFunction destinationRGB,
	                                                              RendererBlendFunction destinationAlpha)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendEquation(RendererBlendEquation blendEquation)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB, RendererBlendEquation blendEquationAlpha)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetCullMode(RendererCullMode cullMode)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::DrawIndexed(const Scope<VertexArray>& vertexArray, const RendererPrimitive primitive)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::Draw(const Scope<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
}

#endif
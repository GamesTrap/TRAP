#include "TRAPPCH.h"
#include "D3D12Renderer.h"

#ifdef TRAP_PLATFORM_WINDOWS

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Renderer::D3D12Renderer()
	: m_context(API::D3D12Context::Get())
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
	SetBlendFunction(RendererBlendFunction::SOURCE_ALPHA, RendererBlendFunction::ONE_MINUS_SOURCE_ALPHA);
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

void TRAP::Graphics::API::D3D12Renderer::ClearInternal(unsigned int buffer)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::PresentInternal(Window* window)
{
	m_context->Present(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetDepthTestingInternal(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendInternal(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetCullInternal(bool enabled)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetFrontFaceInternal(RendererFrontFace frontFace)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetWireFrameInternal(bool enabled)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendFunctionInternal(RendererBlendFunction source, RendererBlendFunction destination)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetBlendEquationInternal(RendererBlendEquation blendEquation)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetViewportInternal(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Renderer::SetCullModeInternal(RendererCullMode cullMode)
{	
}

#endif
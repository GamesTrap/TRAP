#include "TRAPPCH.h"
#include "D3D12Renderer.h"

namespace TRAP::Graphics
{
	D3D12Renderer::D3D12Renderer()
		: m_context(API::D3D12Context::Get())
	{		
	}

	D3D12Renderer::~D3D12Renderer()
	{
		TP_DEBUG("[Renderer][D3D12] Destroying Renderer");
	}

	void D3D12Renderer::InitInternal()
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

	void D3D12Renderer::ClearInternal(unsigned int buffer)
	{		
	}

	void D3D12Renderer::PresentInternal(Window* window)
	{
		m_context->Present(window);
	}

	void D3D12Renderer::SetDepthTestingInternal(bool enabled)
	{		
	}

	void D3D12Renderer::SetBlendInternal(bool enabled)
	{		
	}

	void D3D12Renderer::SetBlendFunctionInternal(RendererBlendFunction source, RendererBlendFunction destination)
	{		
	}

	void D3D12Renderer::SetBlendEquationInternal(RendererBlendEquation blendEquation)
	{		
	}

	void D3D12Renderer::SetViewportInternal(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{		
	}

	std::string_view D3D12Renderer::GetTitleInternal() const
	{
		return m_rendererTitle;
	}
}

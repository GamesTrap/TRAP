#include "TRAPPCH.h"
#include "D3D12Context.h"

namespace TRAP::Graphics::API
{
#define ReleaseCOM(x) \
	    if (x) \
	    { \
		    x->Release(); \
			x = nullptr; \
		}

	D3D12Context::D3D12Context(Window* window)
	{
	}

	D3D12Context::~D3D12Context()
	{
		TP_DEBUG("[Context][D3D12] Destroying Context");
	}

	void D3D12Context::SetVSyncIntervalInternal(unsigned int interval)
	{
	}

	void D3D12Context::Present(Window* window)
	{
	}
}

#include "TRAPPCH.h"
#include "D3D12Context.h"

#ifdef TRAP_PLATFORM_WINDOWS

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP::Graphics::API
{
#define ReleaseCOM(x)     \
	    if (x)            \
	    {                 \
		    x->Release(); \
			x = nullptr;  \
		}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Context::D3D12Context(Window* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12Context::~D3D12Context()
{
	TP_DEBUG("[Context][D3D12] Destroying Context");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Context::SetVSyncIntervalInternal(uint32_t interval)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Context::Present(Window* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12Context::UseInternal(Window* window)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::D3D12Context::IsD3D12Capable()
{
	return true; //TODO Check if D3D12 capable
}

#endif
#ifndef _TRAP_D3D12COMMON_H_
#define _TRAP_D3D12COMMON_H_

#include "TRAPPCH.h"

#ifdef TRAP_PLATFORM_WINDOWS

namespace TRAP::Graphics::API
{
	void CheckD3D12Error(HRESULT result, const char* function, const char* file, int32_t line);
}

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP::Graphics::API
{
#ifdef TRAP_DEBUG
	#define DXCall(x) do { \
					  HRESULT __hr = x; \
     				  ::TRAP::Graphics::API::CheckD3D12Error(__hr, #x, __FILE__, __LINE__); \
					  } while(false)
#else
	#define DXCall(x) x
#endif
}

#endif

#endif /*_TRAP_D3D12COMMON_H_*/
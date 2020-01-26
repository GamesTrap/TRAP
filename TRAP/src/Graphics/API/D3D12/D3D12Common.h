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
#if _cplusplus > 201703L
	#define DXCall(x) std::source_location loc = std::source_location::current();\
					  HRESULT __hr = x; \
     				  ::TRAP::Graphics::API::CheckD3D12Error(__hr, #x, loc.file_name, loc.line);
#else
	#define DXCall(x) HRESULT __hr = x; \
     				  ::TRAP::Graphics::API::CheckD3D12Error(__hr, #x, __FILE__, __LINE__);
#endif
#else
	#define DXCall(x) x
#endif
}

#endif

#endif /*_TRAP_D3D12COMMON_H_*/
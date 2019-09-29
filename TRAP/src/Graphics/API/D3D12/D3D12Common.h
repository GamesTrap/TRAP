#ifndef _TRAP_D3D12COMMON_H_
#define _TRAP_D3D12COMMON_H_

#include "TRAPPCH.h"

#ifdef TRAP_PLATFORM_WINDOWS

namespace TRAP::Graphics::API
{
	void CheckD3D12Error(HRESULT result, const char* function, const char* file, int32_t line);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::CheckD3D12Error(const HRESULT result, const char* function, const char* file, const int32_t line)
{
	if (result == S_OK)
		return;

	switch (result)
	{
	case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
		TRAP_ERROR("[Renderer][D3D12] D3D12_ERROR_DRIVER_VERSION_MISMATCH: ", function, " @[", file, ':', line, ']');
		break;

	case D3D12_ERROR_ADAPTER_NOT_FOUND:
		TRAP_ERROR("[Renderer][D3D12] D3D12_ERROR_ADAPTER_NOT_FOUND: ", function, " @[", file, ':', line, ']');
		break;

	case DXGI_ERROR_INVALID_CALL:
		TRAP_ERROR("[Renderer][D3D12] DXGI_ERROR_INVALID_CALL: ", function, " @[", file, ':', line, ']');
		break;

	case DXGI_ERROR_WAS_STILL_DRAWING:
		TRAP_ERROR("[Renderer][D3D12] DXGI_ERROR_WAS_STILL_DRAWING: ", function, " @[", file, ':', line, ']');
		break;

	case E_FAIL:
		TRAP_ERROR("[Renderer][D3D12] E_FAIL: ", function, " @[", file, ':', line, ']');
		break;

	case E_INVALIDARG:
		TRAP_ERROR("[Renderer][D3D12] E_INVALIDARG: ", function, " @[", file, ':', line, ']');
		break;

	case E_OUTOFMEMORY:
		TRAP_ERROR("[Renderer][D3D12] E_OUTOFMEMORY: ", function, " @[", file, ':', line, ']');
		break;

	case E_NOTIMPL:
		TRAP_ERROR("[Renderer][D3D12] E_NOTIMPL: ", function, " @[", file, ':', line, ']');
		break;

	case S_FALSE:
		TRAP_ERROR("[Renderer][D3D12] S_FALSE: ", function, " @[", file, ':', line, ']');
		break;

	default:
		TRAP_ERROR("[Renderer][D3D12] Unknown error: ", function, " @[", file, ':', line, ']');
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP::Graphics
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
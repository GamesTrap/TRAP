#ifndef _TRAP_D3D12CONTEXT_H_
#define _TRAP_D3D12CONTEXT_H_

#include "Graphics/API/Context.h"

#ifdef TRAP_PLATFORM_WINDOWS

namespace TRAP::Graphics::API
{
	class D3D12Context final : public Context
	{
	public:
		explicit D3D12Context(Window* window);
		~D3D12Context();
		D3D12Context(const D3D12Context&) = default;
		D3D12Context& operator=(const D3D12Context&) = default;
		D3D12Context(D3D12Context&&) = default;
		D3D12Context& operator=(D3D12Context&&) = default;

		void SetVSyncIntervalInternal(uint32_t interval) override;

		void Present(const std::unique_ptr<Window>& window);

		void UseInternal(const std::unique_ptr<Window>& window) override;

		static D3D12Context* Get();

		static bool IsD3D12Capable();
	};
}

#endif

#endif /*_TRAP_D3D12CONTEXT_H_*/
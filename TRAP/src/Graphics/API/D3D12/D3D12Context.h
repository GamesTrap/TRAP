#pragma once

namespace TRAP::Graphics::API
{
	class D3D12Context final : public Context
	{
	public:
		D3D12Context(Window* window);
		~D3D12Context();
		D3D12Context(const D3D12Context&) = default;
		D3D12Context& operator=(const D3D12Context&) = default;
		D3D12Context(D3D12Context&&) = default;
		D3D12Context& operator=(D3D12Context&&) = default;

		void SetVSyncIntervalInternal(unsigned int interval) override;

		void Present(Window* window);

		static D3D12Context* Get() { return dynamic_cast<D3D12Context*>(s_Context.get()); }
	};
}

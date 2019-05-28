#pragma once

namespace TRAP {
	class Window;
}

namespace TRAP::Graphics::API
{
	enum class RenderAPI
	{
		NONE,
		OPENGL,
#ifdef TRAP_PLATFORM_WINDOWS
		D3D12,
#endif
		VULKAN
	};

	class Context
	{
	public:
		Context() = default;
		virtual ~Context() = default;
		Context(const Context&) = default;
		Context& operator=(const Context&) = default;
		Context(Context&&) = default;
		Context& operator=(Context&&) = default;

		static void Create(Window* window);

		static RenderAPI GetRenderAPI() { return s_RenderAPI; }
		static void SetRenderAPI(const RenderAPI api) { s_RenderAPI = api; }

		static void SetVSyncInterval(const unsigned int interval) { if (s_Context) s_Context->SetVSyncIntervalInternal(interval); m_vsyncInterval = interval; }
		static unsigned int GetVSyncInterval() { return m_vsyncInterval; }

	protected:
		static std::unique_ptr<Context> s_Context;
		static RenderAPI s_RenderAPI;

	private:
		virtual void SetVSyncIntervalInternal(unsigned int interval = 0) = 0;

		static unsigned int m_vsyncInterval;
	};
}

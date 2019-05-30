#ifndef _TRAP_CONTEXT_H_
#define _TRAP_CONTEXT_H_

namespace TRAP {
	class Window;
}

namespace TRAP::Graphics::API
{
	enum class RenderAPI
	{
		NONE,
		OPENGL,
		D3D12,
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

		static RenderAPI GetRenderAPI();
		static void SetRenderAPI(RenderAPI api);

		static void SetVSyncInterval(unsigned int interval);
		static unsigned int GetVSyncInterval();

	protected:
		static std::unique_ptr<Context> s_Context;
		static RenderAPI s_RenderAPI;

	private:
		virtual void SetVSyncIntervalInternal(unsigned int interval = 0) = 0;

		static unsigned int m_vsyncInterval;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::RenderAPI TRAP::Graphics::API::Context::GetRenderAPI()
{	
	return s_RenderAPI;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Context::SetRenderAPI(const RenderAPI api)
{
	s_RenderAPI = api;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Context::SetVSyncInterval(const unsigned int interval)
{
	if (s_Context) s_Context->SetVSyncIntervalInternal(interval); m_vsyncInterval = interval;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned TRAP::Graphics::API::Context::GetVSyncInterval()
{
	return m_vsyncInterval;
}

#endif /*_TRAP_CONTEXT_H_*/
#ifndef _TRAP_CONTEXT_H_
#define _TRAP_CONTEXT_H_

#include "Core/Base.h"

namespace TRAP
{
	class Window;
}

namespace TRAP::Graphics::API
{
	enum class RenderAPI
	{
		NONE = 0,
		Vulkan
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
		static void Shutdown();

		static void AutoSelectRenderAPI();
		static void CheckAllRenderAPIs();
		static RenderAPI GetRenderAPI();
		static void SetRenderAPI(RenderAPI api);
		static void SwitchRenderAPI(RenderAPI api);

		static bool IsSupported(RenderAPI api);
		static bool IsVulkanCapable();

		static void SetVSyncInterval(uint32_t interval);

		static void Use(Window* window);

		static RenderAPI s_newRenderAPI;
	protected:
		static Scope<Context> s_Context;
		static RenderAPI s_RenderAPI;

	private:
		virtual void SetVSyncIntervalInternal(uint32_t interval = 0) = 0;
		virtual void UseInternal(Window* window) = 0;

		static bool s_isVulkanCapable;
	};
}

#endif /*_TRAP_CONTEXT_H_*/
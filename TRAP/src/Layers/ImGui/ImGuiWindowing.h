#ifndef _TRAP_IMGUIWINDOWING_H_
#define _TRAP_IMGUIWINDOWING_H_

#include "Window/WindowingAPI.h"

namespace TRAP::INTERNAL
{
	class ImGuiWindowing
	{
	public:
		static bool InitForOpenGL(const std::shared_ptr<WindowingAPI::InternalWindow>& window, bool installCallbacks);
		static void Shutdown();
		static void NewFrame();

	private:
		static std::shared_ptr<WindowingAPI::InternalWindow> s_window; //Main Window
		static WindowingAPI::ContextAPI s_clientAPI;
		static double s_time;
		static std::array<bool, 5> s_mouseJustPressed;
		static std::array<std::shared_ptr<WindowingAPI::InternalCursor>, ImGuiMouseCursor_COUNT> s_mouseCursors;
		static bool s_installedCallbacks;
		static bool s_wantUpdateMonitors;

		//Chain WindowingAPI callbacks for main viewport: our callbacks will call the user's previously installed callbacks, if any.
		static WindowingAPI::MouseButtonFunc s_prevUserCallbackMouseButton;
		static WindowingAPI::ScrollFunc s_prevUserCallbackScroll;
		static WindowingAPI::KeyFunc s_prevUserCallbackKey;
		static WindowingAPI::CharFunc s_prevUserCallbackChar;

		struct ImGuiViewportDataTRAP
		{
			std::shared_ptr<WindowingAPI::InternalWindow> Window = nullptr;
			bool WindowOwned = false;
			int32_t IgnoreWindowSizeEventFrame = -1;

			~ImGuiViewportDataTRAP()
			{
				IM_ASSERT(Window == nullptr);
			}
		};		
		
		static void InitPlatformInterface();
		//static void UpdateMonitors();

		static bool Init(const std::shared_ptr<WindowingAPI::InternalWindow>& window, bool installCallbacks, WindowingAPI::ContextAPI clientAPI);
		static const char* GetClipboardText(void* userData);
		static void SetClipboardText(void* userData, const char* text);
		static void MouseButtonCallback(const std::shared_ptr<WindowingAPI::InternalWindow>& window, Input::MouseButton mouseButton, bool pressed);
		static void ScrollCallback(const std::shared_ptr<WindowingAPI::InternalWindow>& window, double xOffset, double yOffset);
		static void KeyCallback(const std::shared_ptr<WindowingAPI::InternalWindow>& window, Input::Key key, int32_t scanCode, bool pressed);
		static void CharCallback(const std::shared_ptr<WindowingAPI::InternalWindow>& window, uint32_t codePoint);
	};
}

#endif /*_TRAP_IMGUIWINDOWING_H_*/
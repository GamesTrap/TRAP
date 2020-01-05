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
		static void UpdateMousePosAndButtons();
		static void UpdateMouseCursor();
		static void UpdateGamepads();
		static void UpdateMonitors();

		static bool Init(const std::shared_ptr<WindowingAPI::InternalWindow>& window, bool installCallbacks, WindowingAPI::ContextAPI clientAPI);
		static const char* GetClipboardText(void* userData);
		static void SetClipboardText(void* userData, const char* text);
		static void MouseButtonCallback(Ref<WindowingAPI::InternalWindow> window, Input::MouseButton mouseButton, bool pressed);
		static void ScrollCallback(Ref<WindowingAPI::InternalWindow> window, double xOffset, double yOffset);
		static void KeyCallback(Ref<WindowingAPI::InternalWindow> window, Input::Key key, int32_t scanCode, bool pressed);
		static void CharCallback(Ref<WindowingAPI::InternalWindow> window, uint32_t codePoint);
		static void WindowCloseCallback(Ref<WindowingAPI::InternalWindow> window);
		static void WindowPosCallback(Ref<WindowingAPI::InternalWindow> window, int32_t x, int32_t y);
		static void WindowSizeCallback(Ref<WindowingAPI::InternalWindow> window, int32_t width, int32_t height);
		static void CreateWindow(ImGuiViewport* viewport);
		static void DestroyWindow(ImGuiViewport* viewport);
#ifdef TRAP_PLATFORM_WINDOWS
		static WNDPROC s_TRAPWndProc;
		static LRESULT CALLBACK WndProcNoInputs(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
		static void ShowWindow(ImGuiViewport* viewport);
		static ImVec2 GetWindowPos(ImGuiViewport* viewport);
		static void SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
		static ImVec2 GetWindowSize(ImGuiViewport* viewport);
		static void SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
		static void SetWindowTitle(ImGuiViewport* viewport, const char* title);
		static void SetWindowFocus(ImGuiViewport* viewport);
		static bool GetWindowFocus(ImGuiViewport* viewport);
		static bool GetWindowMinimized(ImGuiViewport* viewport);
		static void SetWindowAlpha(ImGuiViewport* viewport, float alpha);
		static void RenderWindow(ImGuiViewport* viewport, void* unused);
		static void SwapBuffers(ImGuiViewport* viewport, void* unused);
#ifdef TRAP_PLATFORM_WINDOWS
		static void SetIMEInputPos(ImGuiViewport* viewport, ImVec2 pos);
#endif
		static int32_t CreateVkSurface(ImGuiViewport* viewport, ImU64 vkInstance, const void* vkAllocator, ImU64* outVkSurface);
		static void MonitorCallback(Ref<WindowingAPI::InternalMonitor>& unused1, bool unused2);
	};
}

#endif /*_TRAP_IMGUIWINDOWING_H_*/
#ifndef _TRAP_WINDOW_H_
#define _TRAP_WINDOW_H_

#include "Graphics/API/Context.h"

#include <glad/glad.h>

#include "Event/Event.h"
#include "ImageLoader/Image.h"
#include "WindowingAPI.h"

namespace TRAP
{
	struct WindowProps;

	//Interface representing a desktop system based window
	class Window final
	{
	public:
		//Used to set Display modes of windows
		enum class DisplayMode
		{
			Windowed,
			Fullscreen,
			Borderless
		};

		//Used to set Cursor mode of windows
		using CursorMode = INTERNAL::WindowingAPI::CursorMode;
		
		//Used to set Cursor type of windows
		using CursorType = INTERNAL::WindowingAPI::CursorType;
		
		using EventCallbackFn = std::function<void(Event&)>;

		explicit Window(const WindowProps& props);
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = default;
		Window& operator=(Window&&) = default;
		~Window();

		static void OnUpdate();
		
		static void Use(const Scope<Window>& window);
		static void Use();
		static uint32_t GetActiveWindows();
		static uint32_t GetMonitors();
		static std::unordered_map<uint32_t, std::string> GetMonitorNames();

		std::string_view GetTitle() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetRefreshRate() const;
		DisplayMode GetDisplayMode() const;
		uint32_t GetMonitor() const;
		std::string GetMonitorName() const;
		uint32_t GetVSyncInterval() const;
		CursorMode GetCursorMode() const;
		bool GetRawMouseInput() const;

		void* GetInternalWindow() const;

		void SetTitle(const std::string& title);
		void SetDisplayMode(const DisplayMode& mode,
			                uint32_t width = 0,
			                uint32_t height = 0,
			                uint32_t refreshRate = 0);
		void SetMonitor(uint32_t monitor = 0);
		void SetVSyncInterval(uint32_t interval);
		void SetCursorMode(const CursorMode& mode);
		void SetCursorType(const CursorType& cursor) const;
		void SetCursorIcon(const Scope<Image>& image) const;
		void SetRawMouseInput(bool enabled);
		void SetIcon() const;
		void SetIcon(const Scope<Image>& image) const;
		void SetEventCallback(const EventCallbackFn& callback);
		void SetResizable(bool enabled) const;
		void SetMinimumSize(uint32_t minWidth, uint32_t minHeight) const;
		void SetMaximumSize(uint32_t maxWidth, uint32_t maxHeight) const;

		bool IsMaximized() const;
		bool IsMinimized() const;
		bool IsResizable() const;
		bool IsVisible() const;
		bool IsFocused() const;
		bool IsDecorated() const;

		void Maximize() const;
		void Minimize() const;
		void RequestAttention() const;
		void Focus() const;
		void Hide() const;
		void Show() const;
		void Restore() const;

	private:
		void Init(const WindowProps& props);
		void Shutdown();
		
		Scope<INTERNAL::WindowingAPI::InternalWindow> m_window;
		INTERNAL::WindowingAPI::InternalMonitor* m_useMonitor; //Stores a reference to the monitor
		static std::unordered_map<uint32_t, INTERNAL::WindowingAPI::VideoMode> s_baseVideoModes; //Stores the underlying video mode being used by the OS for every monitor
		
		struct WindowedModeParams
		{
			int32_t Width, Height, RefreshRate;
			int32_t XPos, YPos;
		} m_oldWindowedParams{};
		
		struct WindowData
		{
			std::string Title;
			int32_t Width{}, Height{}, RefreshRate{}, VSync{};
			DisplayMode displayMode{};
			uint32_t Monitor{};
			CursorMode cursorMode{};
			bool RawMouseInput{};
			std::unordered_map<uint16_t, uint32_t> KeyRepeatCounts;

			EventCallbackFn EventCallback;

			WindowedModeParams* windowModeParams = nullptr;
		} m_data;
		
		static uint32_t s_windows;
		static bool s_WindowingAPIInitialized;
		static std::vector<Window*> s_fullscreenWindows;
	};

	//Used to create new windows
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		uint32_t RefreshRate;
		Graphics::API::RenderAPI RenderAPI;
		Window::DisplayMode displayMode;
		uint32_t Monitor;

		struct Advanced
		{			
			uint32_t VSync = 0;
			bool Resizable = true;
			bool Maximized = false;
			bool Visible = true;
			bool Focused = true;
			bool FocusOnShow = true;
			bool Decorated = true;
			//bool Stereo = false;
			bool RawMouseInput = false;
			Window::CursorMode CursorMode = Window::CursorMode::Normal;

			explicit Advanced(uint32_t vSync = 0,
			                  bool resizable = true,
			                  bool maximized = false,
			                  bool visible = true,
			                  bool focused = true,
			                  bool focusOnShow = true,
			                  bool decorated = true,
			                  bool rawMouseInput = false,
			                  Window::CursorMode cursorMode = Window::CursorMode::Normal);
			
		} advanced{};

		//Sets up properties for new window(s)
		explicit WindowProps(std::string title = "TRAP Engine",
							 uint32_t width = 1280,
							 uint32_t height = 720,
							 uint32_t refreshRate = 60,
							 Window::DisplayMode displayMode = Window::DisplayMode::Windowed,
							 Advanced advanced = Advanced{},
		                     uint32_t monitor = 0);
	};
}

#endif /*_TRAP_WINDOW_H_*/
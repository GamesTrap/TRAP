#ifndef _TRAP_WINDOW_H_
#define _TRAP_WINDOW_H_

#include "Graphics/API/Context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Event/Event.h"
#include "Utils/Logo.h"
#include "ImageLoader/Image.h"

namespace TRAP
{
	//Used to set Display modes of windows
	enum class DisplayMode
	{
		Windowed,
		Fullscreen,
		Borderless
	};

	//Used to create new windows
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		uint32_t RefreshRate;
		uint32_t VSync;
		Graphics::API::RenderAPI RenderAPI;
		DisplayMode Mode;
		uint32_t Monitor;

		//Sets up properties for new window(s)
		explicit WindowProps(std::string title = "TRAP Engine",
			const uint32_t width = 1280,
			const uint32_t height = 720,
			const uint32_t refreshRate = 60,
			const uint32_t vsync = 0,
			const DisplayMode mode = DisplayMode::Windowed,
			const uint32_t monitor = 0)
			: Title(std::move(title)),
			Width(width),
			Height(height),
			RefreshRate(refreshRate),
			VSync(vsync),
			RenderAPI(Graphics::API::Context::GetRenderAPI()),
			Mode(mode),
			Monitor(monitor)
		{
		}
	};

	//Interface representing a desktop system based window
	class Window final
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		explicit Window(const WindowProps& props);
		Window(const Window&) = default;
		Window& operator=(const Window&) = default;
		Window(Window&&) = default;
		Window& operator=(Window&&) = default;
		~Window();

		static void OnUpdate();
		static void Use(Window* window);
		static void Use();
		static uint32_t GetActiveWindows();
		static uint32_t GetMonitors();

		std::string GetTitle() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetRefreshRate() const;
		DisplayMode GetDisplayMode() const;
		uint32_t GetMonitor() const;
		uint32_t GetVSyncInterval() const;

		void* GetNativeWindow() const;

		void SetTitle(const std::string& title);
		void SetDisplayMode(const DisplayMode& mode,
			uint32_t width = 0,
			uint32_t height = 0,
			uint32_t refreshRate = 0);
		void SetMonitor(uint32_t monitor = 0);
		void SetVSyncInterval(uint32_t interval) const;
		void SetIcon() const;
		void SetIcon(const std::unique_ptr<Image>& image) const;
		void SetEventCallback(const EventCallbackFn& callback);

	private:
		void Shutdown();
		void Init(const WindowProps& props);
		
		GLFWwindow* m_window;
		GLFWmonitor* m_useMonitor; //Stores a reference to the monitor
		GLFWvidmode m_baseVideoMode{}; //Stores the underlying video mode being used by the OS

		struct WindowData
		{
			std::string Title;
			uint32_t Width{}, Height{}, RefreshRate{}, VSync{};
			DisplayMode Mode{};
			uint32_t Monitor{};

			EventCallbackFn EventCallback;
		};

		WindowData m_data;

		struct WindowedModeParams
		{
			uint32_t Width, Height, RefreshRate;
			int32_t XPos, YPos;
		} m_oldWindowedParams{};

		static uint32_t s_windows;
	};
}

#endif /*_TRAP_WINDOW_H_*/
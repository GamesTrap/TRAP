#ifndef _TRAP_WINDOW_H_
#define _TRAP_WINDOW_H_

#include "Graphics/API/Context.h"

#include <GLFW/glfw3.h>

#include "Event/Event.h"

namespace TRAP
{
	enum class DisplayMode
	{
		WINDOWED,
		FULLSCREEN,
		BORDERLESS
	};

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		unsigned int RefreshRate;
		unsigned int VSync;
		Graphics::API::RenderAPI RenderAPI;
		DisplayMode Mode;
		unsigned int Monitor;

		explicit WindowProps(std::string title = "TRAP Engine",
		                     const unsigned int width = 1280,
		                     const unsigned int height = 720,
		                     const unsigned int refreshRate = 60,
		                     const unsigned int vsync = 0,
		                     const DisplayMode mode = DisplayMode::WINDOWED,
		                     const unsigned int monitor = 0,
		                     Graphics::API::RenderAPI api = Graphics::API::RenderAPI::VULKAN)
			: Title(std::move(title)),
			  Width(width),
			  Height(height),
			  RefreshRate(refreshRate),
			  VSync(vsync),
			  RenderAPI(api),
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

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		unsigned int GetRefreshRate() const;
		std::string_view GetTitle() const;
		DisplayMode GetDisplayMode() const;
		unsigned int GetMonitor() const;

		//Window attributes
		void SetEventCallback(const EventCallbackFn& callback);
		
		void SetWindowMode(const DisplayMode& mode,
		                   unsigned int width = 0,
		                   unsigned int height = 0,
		                   unsigned int refreshRate = 0);
		void SetMonitor(unsigned int monitor = 0);
		void SetIcon(unsigned int width = 32, unsigned int height = 32, unsigned char* pixels = Utils::INTERNAL::Logo.data()) const; //TODO Replace pixels with TRAP::Image Loader...

		void* GetNativeWindow() const;

		static void Clear();

	private:
		void Init(const WindowProps& props);
		void Shutdown() const;

		GLFWwindow* m_window;
		GLFWmonitor* m_useMonitor; //Stores a reference to the primary monitor
		GLFWvidmode m_baseVideoMode{}; //Stores the underlying video mode being used by the OS

		struct WindowData
		{
			std::string Title;
			unsigned int Width{}, Height{}, RefreshRate{};
			DisplayMode Mode{};
			unsigned int Monitor{};

			EventCallbackFn EventCallback;
		};

		WindowData m_data;

		struct WindowedModeParams
		{
			unsigned int Width, Height, RefreshRate;
			int XPos, YPos;
		};

		WindowedModeParams m_oldWindowedParams{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Window::GetWidth() const
{
	return m_data.Width;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Window::GetHeight() const
{
	return m_data.Height;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Window::GetRefreshRate() const
{
	return m_data.RefreshRate;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Window::GetTitle() const
{
	return m_data.Title;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::DisplayMode TRAP::Window::GetDisplayMode() const
{
	return m_data.Mode;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Window::GetMonitor() const
{
	return m_data.Monitor;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Window::SetEventCallback(const EventCallbackFn& callback)
{
	m_data.EventCallback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void* TRAP::Window::GetNativeWindow() const
{
	return m_window;
}

#endif /*_TRAP_WINDOW_H_*/
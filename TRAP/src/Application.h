#ifndef _TRAP_APPLICATION_H_
#define _TRAP_APPLICATION_H_
#include "TRAPPCH.h"

#include "Layers/ImGui/ImGuiLayer.h"

#include "Config/Config.h"
#include "Layers/LayerStack.h"
#include "ThreadPool/ThreadPool.h"
#include "Utils/Time/Timer.h"

int main();

namespace TRAP
{
	class VFS;

	namespace Events
	{
		class WindowRestoreEvent;
		class WindowMinimizeEvent;
		class WindowLostFocusEvent;
		class WindowFocusEvent;
		class FrameBufferResizeEvent;
		class WindowCloseEvent;
		class KeyPressEvent;
	}

	/// <summary>
	/// CPUInfo is a struct which contains information about the CPU of the system which is used by TRAP Engine.
	/// </summary>
	struct CPUInfo
	{
		std::string Model = "";
		uint32_t Cores = 0;
		uint32_t LogicalCores = 0;
		bool HyperThreaded = false;
	};

	class Application
	{
	public:
		/// <summary>
		/// Enum used to describe endianness.
		/// </summary>
		enum class Endian
		{
			Little = 1,
			Big = 0
		};

		/// <summary>
		/// Enum used to indicate which Window Manager is used by Linux based systems.
		/// </summary>
		enum class LinuxWindowManager
		{
			Unknown,

			X11,
			Wayland
		};

		/// <summary>
		/// Constructor.
		/// </summary>
		Application();
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Application();

		/// <summary>
		/// Deleted Copy Constructor.
		/// </summary>
		Application(const Application&) = delete;
		/// <summary>
		/// Deleted Copy Assignment Operator.
		/// </summary>
		Application& operator=(const Application&) = delete;
		/// <summary>
		/// Deleted Move Constructor.
		/// </summary>
		Application(Application&&) = delete;
		/// <summary>
		/// Deleted Move Assignment Operator.
		/// </summary>
		Application& operator=(Application&&) = delete;

		/// <summary>
		/// Pushes a TRAP::Layer to the TRAP::Applications TRAP::LayerStack.
		/// </summary>
		/// <param name="layer">Scope containing TRAP::Layer to be pushed.</param>
		void PushLayer(Scope<Layer> layer) const;
		/// <summary>
		/// Pushes a TRAP::Layer to the TRAP::Applications TRAP::LayerStack.
		/// </summary>
		/// <param name="overlay">Scope containing TRAP::Layer to be pushed.</param>
		void PushOverlay(Scope<Layer> overlay) const;

		/// <summary>
		/// Get the Engine.cfg TRAP::Utils::Config (Engine.cfg) from the TRAP::Application.
		/// </summary>
		/// <returns>Constant reference to the TRAP::Utils::Config.</returns>
		static const Utils::Config& GetConfig();
		/// <summary>
		/// Get the TRAP::LayerStack from the TRAP::Application.
		/// </summary>
		/// <returns>Reference to the TRAP::LayerStack.</returns>
		static LayerStack& GetLayerStack();
		/// <summary>
		/// Get the default ImGuiLayer
		/// </summary>
		/// <returns>Reference to ImGuiLayer.</returns>
		static ImGuiLayer GetImGuiLayer();

		/// <summary>
		/// Get the current Frames Per Second.
		/// </summary>
		/// <returns>Current Frames Per Second.</returns>
		static uint32_t GetFPS();
		/// <summary>
		/// Get the current FrameTime.
		/// </summary>
		/// <returns>Current FrameTime.</returns>
		static float GetFrameTime();
		/// <summary>
		/// Get the current TimeScale.
		/// </summary>
		/// <returns>Current TimeScale.</returns>
		static float GetTimeScale();
		/// <summary>
		/// Get the current DrawCalls of the frame.
		/// </summary>
		/// <returns>Current DrawCalls of the frame.</returns>
		static uint32_t GetDrawCalls();
		/// <summary>
		/// Add a single DrawCall to the DrawCall counter.
		/// </summary>
		static void AddSingleDrawCall();
		/// <summary>
		/// Get the current TickRate (Default: 100).
		/// </summary>
		/// <returns>Current TickRate.</returns>
		static uint32_t GetTickRate();
		/// <summary>
		/// Set the TickRate.
		/// </summary>
		/// <param name="tickRate">TickRate.</param>
		static void SetTickRate(uint32_t tickRate);
		/// <summary>
		/// Set the TimeScale.
		/// </summary>
		/// <param name="timeScale">TimeScale.</param>
		static void SetTimeScale(float timeScale);

		/// <summary>
		/// Enable or Disable HotShaderReloading.
		/// </summary>
		/// <param name="enabled">Whether to enable or disable HotShaderReloading.</param>
		static void SetHotShaderReloading(bool enabled);
		/// <summary>
		/// Enable or Disable HotTextureReloading.
		/// </summary>
		/// <param name="enabled">Whether to enable or disable HotTextureReloading.</param>
		static void SetHotTextureReloading(bool enabled);

		/// <summary>
		/// Shutdown the Engine.
		/// </summary>
		static void Shutdown();

		/// <summary>
		/// Get the Main Render TRAP::Window.
		/// </summary>
		/// <returns>Constant Reference to a Scope containing the Main Render TRAP::Window.</returns>
		static const Scope<Window>& GetWindow();
		/// <summary>
		/// Get the Time since the Engine was started.
		/// </summary>
		/// <returns>TRAP::Utils::TimeStep containing the passed Time since the Engine was started.</returns>
		static Utils::TimeStep GetTime();
		/// <summary>
		/// Get the endianness of the system.
		/// </summary>
		/// <returns>TRAP::Application::Endian::Little or TRAP::Application::Endian::Big.</returns>
		static Endian GetEndian();
		/// <summary>
		/// Get the Window Manager used by Linux based systems.
		/// </summary>
		/// <returns>TRAP::Application::LinuxWindowManager::X11, TRAP::Application::LinuxWindowManager::Wayland or TRAP::Application::LinuxWindowManager::Unknown(If Window Manager is unknown or system OS is Windows).</returns>
		static LinuxWindowManager GetLinuxWindowManager();
		/// <summary>
		/// Get the TRAP::ThreadPool to be used for small tasks that can be multi-threaded.
		/// </summary>
		/// <returns>Reference to the TRAP::ThreadPool.</returns>
		static ThreadPool& GetThreadPool();

		/// <summary>
		/// Set the clipboard.
		/// </summary>
		/// <param name="string">String to be set.</param>
		static void SetClipboardString(std::string_view string);
		/// <summary>
		/// Get current content of the clipboard.
		/// </summary>
		/// <returns>String containing the clipboards content.</returns>
		static std::string GetClipboardString();

		/// <summary>
		/// Get information about the CPU that runs the Engine.
		/// </summary>
		/// <returns>Constant reference to the TRAP::CPUInfo.</returns>
		static const CPUInfo& GetCPUInfo();

		/// <summary>
		/// Get the id of the main engine thread.
		/// </summary>
		/// <returns>Main thread ID.</returns>
		static std::thread::id GetMainThreadID();

	private:
		/// <summary>
		/// Game/Run Loop.
		/// </summary>
		void Run();

		/// <summary>
		/// Recreate the Main Render Window for use by the specified TRAP::Graphics::API::RenderAPI (needed for switching to/from OpenGL from any other RenderingAPI).
		/// </summary>
		/// <param name="renderAPI">New TRAP::Graphics::API::RenderAPI to be used.</param>
		void ReCreateWindow(Graphics::API::RenderAPI renderAPI);
		/*void ReCreate(Graphics::API::RenderAPI renderAPI) const;*/
		
		/// <summary>
		/// Handles all Events for the Main Render Window and the TRAP::LayerStack.
		/// </summary>
		/// <param name="e">TRAP::Events::Event that occurred.</param>
		void OnEvent(Events::Event& e);
		/// <summary>
		/// Handles Window close events for the Main Render Window.
		/// </summary>
		/// <param name="e">TRAP::Events::WindowCloseEvent that occurred.</param>
		/// <returns>Always true.</returns>
		bool OnWindowClose(Events::WindowCloseEvent& e);
		/// <summary>
		/// Handles Window Frame buffer resizes for the Main Render Window.
		/// </summary>
		/// <param name="e">TRAP::Events::FrameBufferResizeEvent that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnFrameBufferResize(Events::FrameBufferResizeEvent& e);
		/// <summary>
		/// Handles Key presses(ALT+Enter) for the Main Render Window.
		/// </summary>
		/// <param name="e">TRAP::Events::KeyPressEvent that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnKeyPress(Events::KeyPressEvent& e) const;
		/// <summary>
		/// Handles Window Focus for the Main Render Window.
		/// </summary>
		/// <param name="e">TRAP::Events::WindowFocusEvent that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowFocus(Events::WindowFocusEvent& e);
		/// <summary>
		/// Handles Window Lost Focus for the Main Render Window.
		/// </summary>
		/// <param name="e">TRAP::Events::WindowLostFocusEvent that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowLostFocus(Events::WindowLostFocusEvent& e);
		/// <summary>
		/// Handles Window Minimize events for the Main Render Window.
		/// </summary>
		/// <param name="e">TRAP::Events::WindowMinimizeEvent that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowMinimize(Events::WindowMinimizeEvent& e);
		/// <summary>
		/// Handles Window Restore events for the Main Render Window.
		/// </summary>
		/// <param name="e">TRAP::Events::WindowRestoreEvent that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowRestore(Events::WindowRestoreEvent& e);

		/// <summary>
		/// Checks if the environmental variable for Wayland(WAYLAND_DISPLAY or XDG_SESSION_TYPE=wayland" or X11(DISPLAY or XDG_SESSION_TYPE=x11) is set.<br>
		/// Note: If neither Wayland or X11 was found the Engine will shutdown!<br>
		/// NOTE: This function only affects Linux OSes!
		/// </summary>
		void UpdateLinuxWindowManager();

		/// <summary>
		/// ProcessHotReloading is used to offload the HotReloading system onto another Thread.
		/// </summary>
		/// <param name="shaders">Reference to a vector for adding the virtual paths of modified shaders.</param>
		/// <param name="textures">Reference to a vector for adding the virtual paths of modified textures.</param>
		/// <param name="run">Whether to stop running the infinite loop or not.</param>
		static void ProcessHotReloading(std::vector<std::string>& shaders, std::vector<std::string>& textures, const bool& run);
		/// <summary>
		/// Tries to reload every modified shader/texture that was set by ProcessHotReloading.
		/// </summary>
		void UpdateHotReloading();
		Scope<std::thread> m_hotReloadingThread;
		std::vector<std::string> m_hotReloadingShaderPaths;
		std::vector<std::string> m_hotReloadingTexturePaths;
		static std::mutex s_hotReloadingMutex;

		Scope<Window> m_window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_running = true;
		bool m_minimized = false;
		bool m_focused = true;
		std::unique_ptr<LayerStack> m_layerStack;

		Utils::Config m_config;

		std::unique_ptr<Utils::Timer> m_timer;
		uint32_t m_FramesPerSecond;
		float m_FrameTime;
		uint32_t m_drawCalls;
		uint32_t m_fpsLimit;
		uint32_t m_tickRate;
		float m_timeScale;

		Endian m_endian;
		LinuxWindowManager m_linuxWindowManager;

		ThreadPool m_threadPool;

		std::thread::id m_mainThreadID;

		static CPUInfo s_CPU;
		static Application* s_Instance;

		friend int ::main();
		friend class TRAP::VFS;
	};

	//To be defined in CLIENT
	std::unique_ptr<Application> CreateApplication();
}

#endif /*_TRAP_APPLICATION_H_*/
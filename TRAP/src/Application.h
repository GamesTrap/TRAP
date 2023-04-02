#ifndef TRAP_APPLICATION_H
#define TRAP_APPLICATION_H

#include <thread>

#include "Utils/Config/Config.h"
#include "Layers/LayerStack.h"
#include "ThreadPool/ThreadPool.h"
#include "Utils/Time/Timer.h"

#if !defined(DOXYGEN_DOCUMENTATION_BUILD)
int main(int32_t argc, const char* const* argv);
#endif

namespace TRAP
{
	class ImGuiLayer;

	namespace Events
	{
		class WindowRestoreEvent;
		class WindowMinimizeEvent;
		class WindowLostFocusEvent;
		class WindowFocusEvent;
		class FrameBufferResizeEvent;
		class WindowCloseEvent;
		class KeyPressEvent;
		class FileChangeEvent;
	}

	namespace FileSystem
	{
		class FileWatcher;
	}

	namespace Utils
	{
		struct TimeStep;
	}

	class Application
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="gameName">Name of the game.</param>
		/// <param name="appID">Optional: Steam AppID for this application. Default: Invalid AppID.</param>
		explicit Application(std::string gameName, uint32_t appID = 0);
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Application();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Application(const Application&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Application& operator=(const Application&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Application(Application&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Application& operator=(Application&&) = delete;

		/// <summary>
		/// Pushes a layer to the applications layer stack.
		/// </summary>
		/// <param name="layer">Unique ptr containing the layer to be pushed.</param>
		void PushLayer(std::unique_ptr<Layer> layer);
		/// <summary>
		/// Pushes a layer to the applications layer stack.
		/// </summary>
		/// <param name="overlay">Unique ptr containing the layer to be pushed.</param>
		void PushOverlay(std::unique_ptr<Layer> overlay);

		/// <summary>
		/// Get the Engine.cfg config from the application.
		/// </summary>
		/// <returns>Constant reference to the config.</returns>
		[[nodiscard]] static const Utils::Config& GetConfig();
		/// <summary>
		/// Get the layer stack from the application.
		/// </summary>
		/// <returns>Reference to the layer stack.</returns>
		[[nodiscard]] static LayerStack& GetLayerStack();
		/// <summary>
		/// Get the default ImGuiLayer
		/// </summary>
		/// <returns>Reference to ImGuiLayer.</returns>
		[[nodiscard]] static ImGuiLayer& GetImGuiLayer();

		/// <summary>
		/// Get the current frames per second.
		/// </summary>
		/// <returns>Current frames per second.</returns>
		[[nodiscard]] static uint32_t GetFPS();
		/// <summary>
		/// Set a FPS limit.
		/// Note: Valid FPS range is 25 - 500. 0 sets unlimited FPS.
		/// </summary>
		/// <param name="fps">FPS to limit to.</param>
		static void SetFPSLimit(uint32_t fps);
		/// <summary>
		/// Retrieve the current FPS limit.
		/// Note: 0 means unlimited FPS.
		/// </summary>
		/// <returns>Current FPS limit.</returns>
		[[nodiscard]] static uint32_t GetFPSLimit();
		/// <summary>
		/// Get the current CPU frame time.
		/// </summary>
		/// <returns>Current CPU frame time.</returns>
		[[nodiscard]] static float GetCPUFrameTime();
		/// <summary>
		/// Get the current time scale.
		/// </summary>
		/// <returns>Current time scale.</returns>
		[[nodiscard]] static float GetTimeScale();
		/// <summary>
		/// Get the current tick rate (Default: 64).
		/// </summary>
		/// <returns>Current tick rate.</returns>
		[[nodiscard]] static uint32_t GetTickRate();
		/// <summary>
		/// Set the tick rate (fixed update interval for OnTick()).
		/// </summary>
		/// <param name="tickRate">New Tick rate.</param>
		static void SetTickRate(uint32_t tickRate);
		/// <summary>
		/// Set the time scale.
		/// </summary>
		/// <param name="timeScale">Time scale.</param>
		static void SetTimeScale(float timeScale);

		/// <summary>
		/// Sets the RenderAPI to be used on the next start of the engine.
		/// </summary>
		/// <param name="renderAPI">New RenderAPI to be used.</param>
		static void SetNewRenderAPI(Graphics::RenderAPI renderAPI);

		/// <summary>
		/// Shutdown the engine.
		/// </summary>
		static void Shutdown();

		/// <summary>
		/// Get the Main Render window.
		/// </summary>
		/// <returns>Pointer to the main render window.</returns>
		[[nodiscard]] static Window* GetWindow();
		/// <summary>
		/// Get the Time since the Engine was started.
		/// </summary>
		/// <returns>Time step containing the passed time since the engine was started.</returns>
		[[nodiscard]] static Utils::TimeStep GetTime();
		/// <summary>
		/// Get the thread pool to be used for small tasks that can be multi-threaded.
		/// </summary>
		/// <returns>Reference to the thread pool.</returns>
		[[nodiscard]] static ThreadPool& GetThreadPool();

		/// <summary>
		/// Set the clipboard.
		/// </summary>
		/// <param name="string">String to be set.</param>
		static void SetClipboardString(const std::string& string);
		/// <summary>
		/// Get current content of the clipboard.
		/// </summary>
		/// <returns>String containing the clipboards content.</returns>
		[[nodiscard]] static std::string GetClipboardString();

		/// <summary>
		/// Get the id of the main engine thread.
		/// </summary>
		/// <returns>Main thread ID.</returns>
		[[nodiscard]] static std::thread::id GetMainThreadID();

		/// <summary>
		/// Get the name of the game.
		/// </summary>
		[[nodiscard]] static std::string GetGameName();

		/// <summary>
		/// Get the global counter.
		/// The counter is incremented every frame.
		/// Note: This is mainly used for NVIDIA-Reflex support.
		/// </summary>
		/// <returns>Global counter value.</returns>
		[[nodiscard]] static uint64_t GetGlobalCounter();

		/// <summary>
		/// Get the hot reloading file watcher.
		/// </summary>
		/// <returns>TRAP::FileSystem::FileWatcher* if file watcher is running, false otherwise.</returns>
		[[nodiscard]] static TRAP::FileSystem::FileWatcher* GetHotReloadingFileWatcher();
		/// <summary>
		/// Get whether hot reloading is enabled or not.
		/// </summary>
		/// <returns>True if hot reloading is enabled, false otherwise.</returns>
		[[nodiscard]] static bool IsHotReloadingEnabled();
		/// <summary>
		/// Set whether to enable or disable hot reloading.
		/// </summary>
		/// <param name="enable">True to enable hot reloading, false otherwise.</param>
		static void SetHotReloading(bool enable);

	private:
		/// <summary>
		/// Game/Run loop.
		/// </summary>
		void Run();

		/// <summary>
		/// Handles all events for the main render window and the layer stack.
		/// </summary>
		/// <param name="event">Event that occurred.</param>
		void OnEvent(Events::Event& event);
		/// <summary>
		/// Handles window close events for the main render window.
		/// </summary>
		/// <param name="event">Window close event that occurred.</param>
		/// <returns>Always true.</returns>
		bool OnWindowClose(Events::WindowCloseEvent& event) noexcept;
		/// <summary>
		/// Handles window framebuffer resizes for the main render window.
		/// </summary>
		/// <param name="event">Framebuffer resize event that occurred.</param>
		/// <returns>Always false.</returns>
		static bool OnFrameBufferResize(Events::FrameBufferResizeEvent& event);
		/// <summary>
		/// Handles key presses(ALT+Enter) for the main render window.
		/// </summary>
		/// <param name="event">Key press event that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnKeyPress(Events::KeyPressEvent& event) const;
		/// <summary>
		/// Handles window focus for the main render window.
		/// </summary>
		/// <param name="event">Window focus event that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowFocus(Events::WindowFocusEvent& event) noexcept;
		/// <summary>
		/// Handles window lost focus for the main render window.
		/// </summary>
		/// <param name="event">Window lost focus event that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowLostFocus(Events::WindowLostFocusEvent& event) noexcept;
		/// <summary>
		/// Handles window minimize events for the main render window.
		/// </summary>
		/// <param name="event">Window minimize event that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowMinimize(Events::WindowMinimizeEvent& event) noexcept;
		/// <summary>
		/// Handles window restore events for the main render window.
		/// </summary>
		/// <param name="event">Window restore event that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowRestore(Events::WindowRestoreEvent& event) noexcept;

		/// <summary>
		/// Handles file change events for the application.
		/// Used by Texture and Shader HotReloading.
		/// </summary>
		/// <param name="e">File change event that occurred.</param>
		/// <returns>True if event was handled, false otherwise.</returns>
		bool OnFileChangeEvent(const Events::FileChangeEvent& event);

		/// <summary>
		/// Tries to reload every modified shader/texture that was set by the hot reloading file watcher.
		/// </summary>
		void UpdateHotReloading();

		std::vector<std::filesystem::path> m_hotReloadingShaderPaths;
		std::vector<std::filesystem::path> m_hotReloadingTexturePaths;
		TracyLockable(std::mutex, m_hotReloadingMutex);
		std::unique_ptr<FileSystem::FileWatcher> m_hotReloadingFileWatcher;
		bool m_hotReloadingEnabled;

		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_running = true;
		bool m_minimized = false;
		bool m_focused = true;
		LayerStack m_layerStack;

		Utils::Config m_config;

		Utils::Timer m_timer;
		float m_FrameTime;
		uint32_t m_fpsLimit;
		uint32_t m_tickRate;
		float m_timeScale;
		std::string m_gameName;
		//NVIDIA-Reflex
		uint64_t m_globalCounter;

		ThreadPool m_threadPool;

		std::thread::id m_mainThreadID;

		Graphics::RenderAPI m_newRenderAPI;

		static Application* s_Instance; //Singleton instance

		friend int ::main(int32_t argc, const char* const*  argv);
	};

	/// <summary>
	/// This function will be called by TRAP engines main function to initialize the engine.
	/// IT MUST BE IMPLEMENTED BY ANY CLIENT!
	/// </summary>
	/// <returns>A unique pointer containing a TRAP::Application</returns>
	extern std::unique_ptr<Application> CreateApplication(const std::vector<std::string_view>& args);
}

#endif /*TRAP_APPLICATION_H*/
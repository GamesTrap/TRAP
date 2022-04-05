#ifndef TRAP_APPLICATION_H
#define TRAP_APPLICATION_H

#include "Layers/ImGui/ImGuiLayer.h"

#include "Utils/Config/Config.h"
#include "Layers/LayerStack.h"
#include "ThreadPool/ThreadPool.h"
#include "Utils/Time/Timer.h"

int main();

namespace TRAP
{
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

	namespace FS
	{
		class FileWatcher;
	}

	class Application
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="gameName">Name of the game.</param>
		explicit Application(std::string gameName);
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
		/// <param name="layer">Scope containing the layer to be pushed.</param>
		void PushLayer(Scope<Layer> layer) const;
		/// <summary>
		/// Pushes a layer to the applications layer stack.
		/// </summary>
		/// <param name="overlay">Scope containing the layer to be pushed.</param>
		void PushOverlay(Scope<Layer> overlay) const;

		/// <summary>
		/// Get the Engine.cfg config from the application.
		/// </summary>
		/// <returns>Constant reference to the config.</returns>
		static const Utils::Config& GetConfig();
		/// <summary>
		/// Get the layer stack from the application.
		/// </summary>
		/// <returns>Reference to the layer stack.</returns>
		static LayerStack& GetLayerStack();
		/// <summary>
		/// Get the default ImGuiLayer
		/// </summary>
		/// <returns>Reference to ImGuiLayer.</returns>
		static ImGuiLayer GetImGuiLayer();

		/// <summary>
		/// Get the current frames per second.
		/// </summary>
		/// <returns>Current frames per second.</returns>
		static uint32_t GetFPS();
		/// <summary>
		/// Get the current frame time.
		/// </summary>
		/// <returns>Current frame time.</returns>
		static float GetFrameTime();
		/// <summary>
		/// Get the current time scale.
		/// </summary>
		/// <returns>Current time scale.</returns>
		static float GetTimeScale();
		/// <summary>
		/// Get the current tick rate (Default: 100).
		/// </summary>
		/// <returns>Current tick rate.</returns>
		static uint32_t GetTickRate();
		/// <summary>
		/// Set the tick rate.
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
		/// <returns>Constant reference to a Scope containing the main render window.</returns>
		static const Scope<Window>& GetWindow();
		/// <summary>
		/// Get the Time since the Engine was started.
		/// </summary>
		/// <returns>Time step containing the passed time since the engine was started.</returns>
		static Utils::TimeStep GetTime();
		/// <summary>
		/// Get the thread pool to be used for small tasks that can be multi-threaded.
		/// </summary>
		/// <returns>Reference to the thread pool.</returns>
		static ThreadPool& GetThreadPool();

		/// <summary>
		/// Set the clipboard.
		/// </summary>
		/// <param name="string">String to be set.</param>
		static void SetClipboardString(const std::string& string);
		/// <summary>
		/// Get current content of the clipboard.
		/// </summary>
		/// <returns>String containing the clipboards content.</returns>
		static std::string GetClipboardString();

		/// <summary>
		/// Get the id of the main engine thread.
		/// </summary>
		/// <returns>Main thread ID.</returns>
		static std::thread::id GetMainThreadID();

		/// <summary>
		/// Get the name of the game.
		/// </summary>
		static std::string GetGameName();

		/// <summary>
		/// Get the hot reloading file watcher.
		/// </summary>
		/// <returns>TRAP::FS::FileWatcher* if file watcher is running, false otherwise.</returns>
		static TRAP::FS::FileWatcher* GetHotReloadingFileWatcher();
		/// <summary>
		/// Get whether hot reloading is enabled or not.
		/// </summary>
		/// <returns>True if hot reloading is enabled, false otherwise.</returns>
		static bool IsHotReloadingEnabled();
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
		/// <param name="e">Event that occurred.</param>
		void OnEvent(Events::Event& e);
		/// <summary>
		/// Handles window close events for the main render window.
		/// </summary>
		/// <param name="e">Window close event that occurred.</param>
		/// <returns>Always true.</returns>
		bool OnWindowClose(Events::WindowCloseEvent& e);
		/// <summary>
		/// Handles window framebuffer resizes for the main render window.
		/// </summary>
		/// <param name="e">Framebuffer resize event that occurred.</param>
		/// <returns>Always false.</returns>
		static bool OnFrameBufferResize(Events::FrameBufferResizeEvent& e);
		/// <summary>
		/// Handles key presses(ALT+Enter) for the main render window.
		/// </summary>
		/// <param name="e">Key press event that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnKeyPress(Events::KeyPressEvent& e) const;
		/// <summary>
		/// Handles window focus for the main render window.
		/// </summary>
		/// <param name="e">Window focus event that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowFocus(Events::WindowFocusEvent& e);
		/// <summary>
		/// Handles window lost focus for the main render window.
		/// </summary>
		/// <param name="e">Window lost focus event that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowLostFocus(Events::WindowLostFocusEvent& e);
		/// <summary>
		/// Handles window minimize events for the main render window.
		/// </summary>
		/// <param name="e">Window minimize event that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowMinimize(Events::WindowMinimizeEvent& e);
		/// <summary>
		/// Handles window restore events for the main render window.
		/// </summary>
		/// <param name="e">Window restore event that occurred.</param>
		/// <returns>Always false.</returns>
		bool OnWindowRestore(Events::WindowRestoreEvent& e);

		/// <summary>
		/// Tries to reload every modified shader/texture that was set by the hot reloading file watcher.
		/// </summary>
		void UpdateHotReloading();
		bool OnFileChangeEvent(const Events::FileChangeEvent& e);
		std::vector<std::filesystem::path> m_hotReloadingShaderPaths;
		std::vector<std::filesystem::path> m_hotReloadingTexturePaths;
		std::mutex m_hotReloadingMutex;
		Scope<FS::FileWatcher> m_hotReloadingFileWatcher;
		bool m_hotReloadingEnabled;

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
		uint32_t m_fpsLimit;
		uint32_t m_tickRate;
		float m_timeScale;
		std::string m_gameName;

		ThreadPool m_threadPool;

		std::thread::id m_mainThreadID;

		Graphics::RenderAPI m_newRenderAPI;

		static Application* s_Instance;

		friend int ::main();
	};

	//To be defined in CLIENT
	std::unique_ptr<Application> CreateApplication();
}

#endif /*TRAP_APPLICATION_H*/
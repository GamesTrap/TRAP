#ifndef TRAP_APPLICATION_H
#define TRAP_APPLICATION_H

#include <thread>

#include "Layers/ImGui/ImGuiLayer.h"

#include "Utils/Config/Config.h"
#include "Layers/LayerStack.h"
#include "ThreadPool/ThreadPool.h"
#include "Utils/Time/Timer.h"

#if !defined(DOXYGEN_DOCUMENTATION_BUILD)
int main(int32_t argc, char** argv);
#endif

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

	namespace FileSystem
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
		static ImGuiLayer& GetImGuiLayer();

		/// <summary>
		/// Get the current frames per second.
		/// </summary>
		/// <returns>Current frames per second.</returns>
		static uint32_t GetFPS();
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
		static uint32_t GetFPSLimit();
		/// <summary>
		/// Get the current CPU frame time.
		/// </summary>
		/// <returns>Current CPU frame time.</returns>
		static float GetCPUFrameTime();
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
		/// <returns>Pointer to the main render window.</returns>
		static Window* GetWindow();
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
		/// Get the global counter.
		/// The counter is incremented every frame.
		/// Note: This is mainly used for NVIDIA-Reflex support.
		/// </summary>
		/// <returns>Global counter value.</returns>
		static uint64_t GetGlobalCounter();

		/// <summary>
		/// Get the hot reloading file watcher.
		/// </summary>
		/// <returns>TRAP::FileSystem::FileWatcher* if file watcher is running, false otherwise.</returns>
		static TRAP::FileSystem::FileWatcher* GetHotReloadingFileWatcher();
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
		/// Handles file change events for the application.
		/// Used by Texture and Shader HotReloading.
		/// </summary>
		/// <param name="e">File change event that occurred.</param>
		/// <returns>True if event was handled, false otherwise.</returns>
		bool OnFileChangeEvent(const Events::FileChangeEvent& e);

		/// <summary>
		/// Tries to reload every modified shader/texture that was set by the hot reloading file watcher.
		/// </summary>
		void UpdateHotReloading();

		std::vector<std::filesystem::path> m_hotReloadingShaderPaths;
		std::vector<std::filesystem::path> m_hotReloadingTexturePaths;
		std::mutex m_hotReloadingMutex;
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

		friend int ::main(int32_t argc, char** argv);
	};

	//To be defined in CLIENT
	std::unique_ptr<Application> CreateApplication(int32_t argc, char** argv);
}

#endif /*TRAP_APPLICATION_H*/
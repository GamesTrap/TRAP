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
		/// <param name="appID">
		/// Optional: Steam AppID for this application.
		/// If appID is a valid Steam AppID then the SteamAPI for clients will be initialized.
		/// Headless mode: SteamAPI won't be initialized, users should manually initialize using TRAP::Utils::Steam::InitializeServer().
		/// </param>
		explicit Application(std::string gameName, std::optional<uint32_t> appID = std::nullopt);
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
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Get the default ImGuiLayer
		/// </summary>
		/// <returns>Reference to ImGuiLayer.</returns>
		[[nodiscard]] static ImGuiLayer& GetImGuiLayer();
#endif

		/// <summary>
		/// Get the current frames per second.
		/// </summary>
		/// <returns>Current frames per second.</returns>
		[[nodiscard]] static uint32_t GetFPS();
		/// <summary>
		/// Set a FPS limit.
		/// Note: Valid FPS range is 25 - 500. 0 sets unlimited FPS.
		/// </summary>
		/// <param name="targetFPS">FPS to limit to.</param>
		static void SetFPSLimit(uint32_t targetFPS);
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

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		/// <summary>
		/// Get the global counter.
		/// The counter is incremented every frame.
		/// Note: This is mainly used for NVIDIA-Reflex support.
		/// </summary>
		/// <returns>Global counter value.</returns>
		[[nodiscard]] static uint64_t GetGlobalCounter();
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

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
		/// Runs the work for a single iteration of the Run() function.
		/// </summary>
		/// <param name="deltaTime">Delta time.</param>
		/// <param name="tickTimerSeconds">Tick timer in seconds.</param>
		void RunWork(const Utils::TimeStep& deltaTime, float& tickTimerSeconds);

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
		static bool OnKeyPress(Events::KeyPressEvent& event);

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

		/// <summary>
		/// Update the time data of the run loop for a new frame.
		/// </summary>
		/// <param name="time">Timer with elapsed time since start of engine.</param>
		/// <param name="lastFrameTime">Frame time of the last frame.</param>
		/// <param name="tickTimerSeconds"></param>
		/// <param name="timeScale">Scaling for time.</param>
		/// <returns>Delta time between start of the last frame to the current time.</returns>
		static Utils::TimeStep UpdateNewFrameTimeData(const Utils::Timer& time, float& lastFrameTime,
		                                              float& tickTimerSeconds, float timeScale);

		/// <summary>
		/// Limit the FPS to fpsLimit.
		/// </summary>
		/// <param name="fpsLimit">Target FPS to limit to.</param>
		/// <param name="limitTimer">Limiter timer.</param>
		static void LimitFPS(uint32_t fpsLimit, Utils::Timer& limitTimer);
		/// <summary>
		/// Limit the FPS to fpsLimit.
		///
		/// This function is used to limit FPS when the main window is unfocused.
		/// </summary>
		/// <param name="fpsLimit">Target FPS to limit to.</param>
		/// <param name="limitTimer">Limiter timer.</param>
		static void UnfocusedLimitFPS(uint32_t fpsLimit, Utils::Timer& limitTimer);

		/// <summary>
		/// Retrieve the filepath of the engine.cfg file.
		/// </summary>
		/// <returns>Path of the config file.</returns>
		static std::filesystem::path GetTRAPConfigPath();

		/// <summary>
		/// Load the engine.cfg file.
		/// </summary>
		/// <returns>Loaded config.</returns>
		static Utils::Config LoadTRAPConfig();

		/// <summary>
		/// Update the engine.cfg file.
		/// </summary>
		/// <param name="config">Config to update.</param>
		/// <param name="window">Optional: Main Window to query data from.</param>
		/// <param name="fpsLimit">FPS limit to save.</param>
		/// <param name="renderAPI">RenderAPI to save.</param>
		static void UpdateTRAPConfig(Utils::Config& config, const Window* window, uint32_t fpsLimit, Graphics::RenderAPI renderAPI);

		/// <summary>
		/// Save the engine.cfg file.
		/// </summary>
		/// <param name="config">Config to save.</param>
		static void SaveTRAPConfig(Utils::Config& config);

		/// <summary>
		/// Load the advanced window properties from the given config.
		/// </summary>
		/// <param name="config">Config to load data from.</param>
		/// <returns>Advanced Window properties.</returns>
		static WindowProps::AdvancedProps LoadAdvancedWindowProps(const TRAP::Utils::Config& config);

		/// <summary>
		/// Load the window properties from the given config.
		/// </summary>
		/// <param name="config">Config to load data from.</param>
		/// <returns>Window properties.</returns>
		static WindowProps LoadWindowProps(const TRAP::Utils::Config& config);

		/// <summary>
		/// Select the RenderAPI to be used for rendering.
		/// </summary>
		/// <param name="config">Config to load data from.</param>
		/// <returns>RenderAPI to use for rendering.</returns>
		static Graphics::RenderAPI SelectRenderAPI(const TRAP::Utils::Config& config);

		/// <summary>
		/// Initialize the RendererAPI.
		/// </summary>
		/// <param name="gameName">Name of the game.</param>
		/// <param name="renderAPI">RenderAPI to use.</param>
		/// <param name="config">Config to load data from.</param>
		static void InitializeRendererAPI(std::string_view gameName, const TRAP::Graphics::RenderAPI& renderAPI, const TRAP::Utils::Config& config);

		/// <summary>
		/// Create the main window.
		///
		/// Linux: If no known window manager can be found this function will close the engine.
		/// Headless-Mode: This function may not create a window, this is not an error.
		/// </summary>
		/// <param name="winProps">Properties for the window.</param>
		/// <returns>Created main window or nullptr.</returns>
		static std::unique_ptr<TRAP::Window> CreateMainWindow(const TRAP::WindowProps& winProps);

		/// <summary>
		/// Load fallback shaders.
		/// </summary>
		static void LoadFallbackShaders();
		/// <summary>
		/// Load fallback textures.
		/// </summary>
		static void LoadFallbackTextures();
		/// <summary>
		/// Load fallback data.
		///
		/// This currently adds fallback shaders and textures.
		/// </summary>
		static void LoadFallbackData();

		/// <summary>
		/// Apply RendererAPI specific settings from config file.
		/// </summary>
		/// <param name="config">Config to load data from.</param>
		static void ApplyRendererAPISettings(const TRAP::Utils::Config& config);

		/// <summary>
		/// Initialize TRAP::Input.
		/// </summary>
		static void InitializeInput();
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Initialize ImGui Layer.
		/// </summary>
		/// <param name="layerStack">LayerStack to add ImGui to.</param>
		/// <returns>ImGui Layer on success, nullptr otherwise.</returns>
		static TRAP::ImGuiLayer* InitializeImGui(TRAP::LayerStack& layerStack);
#endif /*TRAP_HEADLESS_MODE*/

		//Hot Reloading
		std::vector<std::filesystem::path> m_hotReloadingShaderPaths{};
		std::vector<std::filesystem::path> m_hotReloadingTexturePaths{};
		TracyLockable(std::mutex, m_hotReloadingMutex);
		std::unique_ptr<FileSystem::FileWatcher> m_hotReloadingFileWatcher = nullptr;

		//Layers
		LayerStack m_layerStack{};
		ImGuiLayer* m_ImGuiLayer = nullptr;

		//Main window
		std::unique_ptr<Window> m_window = nullptr;
		bool m_minimized = false;
		bool m_focused = true;

		//NVIDIA-Reflex
		uint64_t m_globalCounter = 0;

		//Multithreading
		ThreadPool m_threadPool;
		std::thread::id m_mainThreadID;

		//Other data
		Utils::Config m_config{};
		Utils::Timer m_timer{};
		float m_FrameTime = 0.0f;
		uint32_t m_fpsLimit = 0;
		uint32_t m_tickRate = 64;
		float m_timeScale = 1.0f;
		std::string m_gameName = "TRAP™";
		bool m_running = true;
		Graphics::RenderAPI m_newRenderAPI = Graphics::RenderAPI::NONE;

		//Singleton instance
		static Application* s_Instance;

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
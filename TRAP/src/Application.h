#ifndef TRAP_APPLICATION_H
#define TRAP_APPLICATION_H

#include <thread>
#include <optional>

#include "Core/Types.h"
#include "Utils/Config/Config.h"
#include "Layers/LayerStack.h"
#include "ThreadPool/ThreadPool.h"
#include "Utils/Time/Timer.h"

#if !defined(DOXYGEN_DOCUMENTATION_BUILD) && !defined(TRAP_UNITTESTS)
int main(i32 argc, const char* const* argv);
#endif /*!DOXYGEN_DOCUMENTATION_BUILD && !TRAP_UNITTESTS*/

namespace TRAP
{
	class ImGuiLayer;

	namespace Events
	{
#ifndef TRAP_HEADLESS_MODE
		class KeyPressEvent;
		class WindowRestoreEvent;
		class WindowMinimizeEvent;
		class WindowLostFocusEvent;
		class WindowFocusEvent;
		class WindowCloseEvent;
		class FrameBufferResizeEvent;
#endif /*TRAP_HEADLESS_MODE*/
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
		/// @brief Constructor.
		/// @param gameName Name of the game.
		/// @param appID Optional: Steam AppID for this application.
		///              If appID is a valid Steam AppID then the SteamAPI for clients will be initialized.
		/// @remark @headless SteamAPI won't be initialized automatically, clients should
        ///                   manually initialize it using TRAP::Utils::Steam::InitializeServer().
		explicit Application(std::string gameName, std::optional<u32> appID = std::nullopt);
		/// @brief Destructor.
		virtual ~Application();

		/// @brief Copy constructor.
		constexpr Application(const Application&) = delete;
		/// @brief Copy assignment operator.
		constexpr Application& operator=(const Application&) = delete;
		/// @brief Move constructor.
		constexpr Application(Application&&) = delete;
		/// @brief Move assignment operator.
		constexpr Application& operator=(Application&&) = delete;

		/// @brief Pushes a layer to the applications layer stack.
		/// @param layer Unique ptr containing the layer to be pushed.
		void PushLayer(std::unique_ptr<Layer> layer);
		/// @brief Pushes a layer to the applications layer stack.
		/// @param overlay Unique ptr containing the layer to be pushed.
		void PushOverlay(std::unique_ptr<Layer> overlay);

		/// @brief Get the Engine.cfg config from the application.
		/// @return Constant reference to the config.
		[[nodiscard]] static const Utils::Config& GetConfig();
		/// @brief Get the layer stack from the application.
		/// @return Reference to the layer stack.
		[[nodiscard]] static LayerStack& GetLayerStack();
#ifndef TRAP_HEADLESS_MODE
		/// @brief Get the default ImGuiLayer.
		/// @return Reference to ImGuiLayer.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static ImGuiLayer& GetImGuiLayer();
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Set a FPS limit.
		/// @param targetFPS FPS to limit to.
		/// @note Valid FPS range is 25 - 500 (values outside this range are clamped). 0 sets unlimited FPS.
		static void SetFPSLimit(u32 targetFPS);
		/// @brief Retrieve the current FPS limit.
		/// @return Current FPS limit.
		/// @note 0 means unlimited FPS.
		[[nodiscard]] static u32 GetFPSLimit();
		/// @brief Set a FPS limit when not focused.
		/// @param targetFPS FPS to limit to.
		/// @note Valid FPS range is 10 - 500 (values outside this range are clamped). 0 sets unlimited FPS.
		static void SetUnfocusedFPSLimit(u32 targetFPS);
		/// @brief Retrieve the current FPS limit when unfocused.
		/// @return Current FPS limit.
		/// @note 0 means unlimited FPS.
		[[nodiscard]] static u32 GetUnfocusedFPSLimit();
		/// @brief Get the current CPU frame time.
		/// @return Current CPU frame time.
		[[nodiscard]] static f32 GetCPUFrameTime();
		/// @brief Get the current time scale.
		/// @return Current time scale.
		[[nodiscard]] static f32 GetTimeScale();
		/// @brief Get the current tick rate (Default: 64).
		/// @return Current tick rate.
		[[nodiscard]] static u32 GetTickRate();
		/// @brief Set the tick rate (fixed update interval for OnTick()).
		/// @param tickRate New Tick rate.
		static void SetTickRate(u32 tickRate);
		/// @brief Set the time scale.
		/// @param timeScale Time scale.
		static void SetTimeScale(f32 timeScale);

		/// @brief Sets the RenderAPI to be used on the next start of the engine.
		/// @param renderAPI New RenderAPI to be used.
		static void SetNewRenderAPI(Graphics::RenderAPI renderAPI);

		/// @brief Shutdown the engine.
		static void Shutdown();

#ifndef TRAP_HEADLESS_MODE
		/// @brief Get the Main Render window.
		/// @return Pointer to the main render window.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static Window* GetWindow();
#endif /*TRAP_HEADLESS_MODE*/
		/// @brief Get the Time since the Engine was started.
		/// @return Time step containing the passed time since the engine was started.
		[[nodiscard]] static Utils::TimeStep GetTime();
		/// @brief Get the thread pool to be used for small tasks that can be multi-threaded.
		/// @return Reference to the thread pool.
		[[nodiscard]] static ThreadPool& GetThreadPool();

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the content of the clipboard.
		/// @param string Content to set the clipboard to.
		/// @remark @headless This function is not available in headless mode.
		static void SetClipboardString(const std::string& string);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Get current content of the clipboard.
		/// @return Content of the clipboard as string.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static std::string GetClipboardString();
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Get the id of the main engine thread.
		/// @return Main thread ID.
		[[nodiscard]] static std::thread::id GetMainThreadID();

		/// @brief Get the name of the game.
		/// @return Name of the game.
		[[nodiscard]] static std::string GetGameName();

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		/// @brief Get the global counter. The counter is incremented every frame.
		/// @return Global counter value.
		/// @remark This function is only available when NVIDIA Reflex SDK is provided.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static u64 GetGlobalCounter();
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

		/// @brief Get the hot reloading file watcher.
		/// @return Reference to TRAP::FileSystem::FileWatcher if file watcher is running, empty optional otherwise.
		[[nodiscard]] static std::optional<std::reference_wrapper<TRAP::FileSystem::FileWatcher>> GetHotReloadingFileWatcher();
		/// @brief Get whether hot reloading is enabled or not.
		/// @return True if hot reloading is enabled, false otherwise.
		[[nodiscard]] static bool IsHotReloadingEnabled();
		/// @brief Set whether to enable or disable hot reloading.
		/// @param enable True to enable hot reloading, false otherwise.
		static void SetHotReloading(bool enable);

	private:
		/// @brief Game/Run loop.
		void Run();
		/// @brief Runs the work for a single iteration of the Run() function.
		/// @param deltaTime Delta time.
		/// @param tickTimerSeconds Tick timer in seconds.
		void RunWork(const Utils::TimeStep& deltaTime, f32& tickTimerSeconds);

		/// @brief Handles all events for the main render window and the layer stack.
		/// @param event Event that occurred.
		void OnEvent(Events::Event& event);
#ifndef TRAP_HEADLESS_MODE
		/// @brief Handles window close events for the main render window.
		/// @param event Window close event that occurred.
		/// @return true.
		/// @remark @headless This function is not available in headless mode.
		bool OnWindowClose(Events::WindowCloseEvent& event) noexcept;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Handles window framebuffer resizes for the main render window.
		/// @param event Framebuffer resize event that occurred.
		/// @return false.
		/// @remark @headless This function is not available in headless mode.
		static bool OnFrameBufferResize(Events::FrameBufferResizeEvent& event);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Handles key presses for the main render window.
		/// @param event Key press event that occurred.
		/// @return false.
		/// @remark @headless This function is not available in headless mode.
		static bool OnKeyPress(Events::KeyPressEvent& event);
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Handles file change events for the application. Used by HotReloading.
		/// @param event File change event that occurred.
		/// @return True if event was handled, false otherwise.
		bool OnFileChangeEvent(const Events::FileChangeEvent& event);

		/// @brief Tries to reload every modified shader/texture that was set by the hot reloading file watcher.
		void UpdateHotReloading();

		/// @brief Update the time data of the run loop for a new frame.
		/// @param time Timer with elapsed time since start of engine.
		/// @param lastFrameTime In/Out: Frame time of the last frame.
		/// @param tickTimerSeconds Out: Tick timer to update in seconds.
		/// @param timeScale Scaling for time.
		/// @return Delta time between start of the last frame to the current time.
		static Utils::TimeStep UpdateNewFrameTimeData(const Utils::Timer& time, f32& lastFrameTime,
		                                              f32& tickTimerSeconds, f32 timeScale);

		/// @brief Limit the FPS to fpsLimit.
		/// @param fpsLimit Target FPS to limit to.
		/// @param limitTimer In/Out: Limiter timer.
		static void LimitFPS(u32 fpsLimit, Utils::Timer& limitTimer);
		/// @brief Limit the FPS to fpsLimit. This function is used to limit FPS when the main window is unfocused.
		/// @param fpsLimit Target FPS to limit to.
		/// @param limitTimer In/Out: Limiter timer.
		static void UnfocusedLimitFPS(u32 fpsLimit, Utils::Timer& limitTimer);

		/// @brief Retrieve the filepath of the engine.cfg file.
		/// @return Path of the engine.cfg config file.
		static std::filesystem::path GetTRAPConfigPath();

		/// @brief Load the engine.cfg file.
		/// @return Loaded and parsed config.
		static Utils::Config LoadTRAPConfig();

#ifndef TRAP_HEADLESS_MODE
		/// @brief Update the engine.cfg file.
		/// @param config Config to update.
		/// @param window Optional: Main Window to query data from.
		/// @param fpsLimit FPS limit to save.
		/// @param unfocusedFPSLimit Unfocused FPS limit to save.
		/// @param renderAPI RenderAPI to save.
		/// @remark @headless This function is not available in headless mode.
		static void UpdateTRAPConfig(Utils::Config& config, const Window* window, u32 fpsLimit,
		                             u32 unfocusedFPSLimit, Graphics::RenderAPI renderAPI);
#else
		/// @brief Update the engine.cfg file.
		/// @param config Config to update.
		/// @param fpsLimit FPS limit to save.
		/// @param unfocusedFPSLimit Unfocused FPS limit to save.
		/// @param renderAPI RenderAPI to save.
		/// @remark This function is only available in headless mode.
		static void UpdateTRAPConfig(Utils::Config& config, u32 fpsLimit, u32 unfocusedFPSLimit,
		                             Graphics::RenderAPI renderAPI);
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Save the engine.cfg file.
		/// @param config Config to save.
		static void SaveTRAPConfig(Utils::Config& config);

#ifndef TRAP_HEADLESS_MODE
		/// @brief Load the advanced window properties from the given config.
		/// @param config Config to load data from.
		/// @return Advanced Window properties.
		/// @remark @headless This function is not available in headless mode.
		static WindowProps::AdvancedProps LoadAdvancedWindowProps(const TRAP::Utils::Config& config);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Load the window properties from the given config.
		/// @param config Config to load data from.
		/// @return Window properties.
		/// @remark @headless This function is not available in headless mode.
		static WindowProps LoadWindowProps(const TRAP::Utils::Config& config);
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Select the RenderAPI to be used for rendering.
		/// @param config Config to load data from.
		/// @return RenderAPI to use for rendering.
		static Graphics::RenderAPI SelectRenderAPI(const TRAP::Utils::Config& config);

		/// @brief Initialize the RendererAPI.
		/// @param gameName Name of the game.
		/// @param renderAPI RenderAPI to use.
		/// @param config Config to load data from.
		static void InitializeRendererAPI(std::string_view gameName, const TRAP::Graphics::RenderAPI& renderAPI, const TRAP::Utils::Config& config);

#ifndef TRAP_HEADLESS_MODE
		/// @brief Create the main window.
		/// @param winProps Properties for the window.
		/// @return Created main window or nullptr.
		/// @remark @linux If no known window manager can be found this function will close the engine.
		/// @remark @headless This function is not available in headless mode.
		static std::unique_ptr<TRAP::Window> CreateMainWindow(const TRAP::WindowProps& winProps);
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// @brief Create the main viewport.
		/// @param config Config to load data from.
		/// @remark This function is only available in headless mode.
		static void CreateMainViewport(const TRAP::Utils::Config& config);
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Load fallback shaders.
		static void LoadFallbackShaders();
		/// @brief Load fallback textures.
		static void LoadFallbackTextures();
		/// @brief Load fallback data.
		static void LoadFallbackData();

		/// @brief Apply RendererAPI specific settings from config file.
		/// @param config Config to load data from.
		static void ApplyRendererAPISettings(const TRAP::Utils::Config& config);

#ifndef TRAP_HEADLESS_MODE
		/// @brief Initialize TRAP::Input.
		/// @remark @headless This function is not available in headless mode.
		static void InitializeInput();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Initialize ImGui Layer.
		/// @param layerStack LayerStack to add ImGui to.
		/// @return ImGui Layer on success, nullptr otherwise.
		/// @remark @headless This function is not available in headless mode.
		static TRAP::ImGuiLayer* InitializeImGui(TRAP::LayerStack& layerStack);
#endif /*TRAP_HEADLESS_MODE*/

		//Hot Reloading
		std::vector<std::filesystem::path> m_hotReloadingShaderPaths{};
		std::vector<std::filesystem::path> m_hotReloadingTexturePaths{};
		TracyLockable(std::mutex, m_hotReloadingMutex);
		std::unique_ptr<FileSystem::FileWatcher> m_hotReloadingFileWatcher = nullptr;

		//Layers
		LayerStack m_layerStack{};
#ifndef TRAP_HEADLESS_MODE
		ImGuiLayer* m_ImGuiLayer = nullptr;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		//Main window
		std::unique_ptr<Window> m_window = nullptr;
#endif /*TRAP_HEADLESS_MODE*/

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		//NVIDIA-Reflex
		u64 m_globalCounter = 0;
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

		//Multithreading
		ThreadPool m_threadPool{Utils::GetCPUInfo().LogicalCores > 1 ? (Utils::GetCPUInfo().LogicalCores - 1) :
	                            std::thread::hardware_concurrency()};
		std::thread::id m_mainThreadID = std::this_thread::get_id();

		//Other data
		Utils::Config m_config{};
		Utils::Timer m_timer{};
		f32 m_FrameTime = 0.0f;
		u32 m_fpsLimit = 0;
		u32 m_unfocusedFPSLimit = 0;
		u32 m_tickRate = 64;
		f32 m_timeScale = 1.0f;
		std::string m_gameName = "TRAP™";
		bool m_running = true;
		Graphics::RenderAPI m_newRenderAPI = Graphics::RenderAPI::NONE;

		//Singleton instance
		inline constinit static Application* s_Instance = nullptr;

#ifndef TRAP_UNITTESTS
		friend int ::main(i32 argc, const char* const* argv);
#endif /*TRAP_UNITTESTS*/
	};

	/// @brief This function will be called by TRAP engines main function to initialize the engine.
	/// @param args Console arguments.
	/// @return A unique pointer containing a TRAP::Application.
	/// @note THIS FUNCTION MUST BE IMPLEMENTED BY THE CLIENT!
	extern std::unique_ptr<Application> CreateApplication(std::span<const std::string_view> args);
}

#endif /*TRAP_APPLICATION_H*/

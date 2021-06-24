#ifndef _TRAP_APPLICATION_H_
#define _TRAP_APPLICATION_H_

#include "Layers/ImGui/ImGuiLayer.h"

#include "Utils/Config/Config.h"
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

	class Application
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="gameName">Name of the game.</param>
		explicit Application(const std::string& gameName);
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
		/// Recreate the Engine for use by the specified TRAP::Graphics::API::RenderAPI (needed for switching RenderingAPIs).
		/// </summary>
		/// <param name="renderAPI">New TRAP::Graphics::API::RenderAPI to be used.</param>
		void ReCreate(Graphics::RenderAPI renderAPI) const;
		
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
		uint32_t m_fpsLimit;
		uint32_t m_tickRate;
		float m_timeScale;
		std::string m_gameName;

		ThreadPool m_threadPool;

		std::thread::id m_mainThreadID;

		Graphics::RenderAPI m_newRenderAPI;

		static Application* s_Instance;

		friend int ::main();
		friend class TRAP::VFS;
	};

	//To be defined in CLIENT
	std::unique_ptr<Application> CreateApplication();
}

#endif /*_TRAP_APPLICATION_H_*/
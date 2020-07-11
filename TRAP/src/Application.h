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
		enum class Endian
		{
			Little = 1,
			Big = 0
		};

		enum class LinuxWindowManager
		{
			Unknown,
			
			X11,
			Wayland
		};

		Application();
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;	

		void PushLayer(Scope<Layer> layer) const;
		void PushOverlay(Scope<Layer> overlay) const;

		static const Utils::Config& GetConfig();
		static LayerStack& GetLayerStack();

		static uint32_t GetFPS();
		static float GetFrameTime();
		static float GetTimeScale();
		static uint32_t GetDrawCalls();
		static void AddSingleDrawCall();
		static uint32_t GetTickRate();
		static void SetTickRate(uint32_t tickRate);
		static void SetTimeScale(float timeScale);

		static void SetHotShaderReloading(bool enabled);
		static void SetHotTextureReloading(bool enabled);

		static void Shutdown();
		
		static const Scope<Window>& GetWindow();
		static Utils::TimeStep GetTime();
		static Endian GetEndian();
		static LinuxWindowManager GetLinuxWindowManager();
		static ThreadPool& GetThreadPool();

		static void SetClipboardString(const std::string& string);
		static std::string GetClipboardString();
		
		void ReCreateWindow(Graphics::API::RenderAPI renderAPI);
		/*void ReCreate(Graphics::API::RenderAPI renderAPI) const;*/
		
	private:
		void Run();
		
		Utils::TimeStep GetTimeInternal() const;

		void OnEvent(Events::Event& e);
		bool OnWindowClose(Events::WindowCloseEvent& e);
		bool OnFrameBufferResize(Events::FrameBufferResizeEvent& e);
		bool OnKeyPress(Events::KeyPressEvent& e) const;
		bool OnWindowFocus(Events::WindowFocusEvent& e);
		bool OnWindowLostFocus(Events::WindowLostFocusEvent& e);
		bool OnWindowMinimize(Events::WindowMinimizeEvent& e);
		bool OnWindowRestore(Events::WindowRestoreEvent& e);

		void UpdateLinuxWindowManager();

		Scope<Window> m_window;
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
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
		
		static Application* s_Instance;
		friend int ::main();
	};

	//To be defined in CLIENT
	std::unique_ptr<Application> CreateApplication();
}

#endif /*_TRAP_APPLICATION_H_*/
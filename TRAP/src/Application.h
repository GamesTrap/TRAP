#ifndef _TRAP_APPLICATION_H_
#define _TRAP_APPLICATION_H_
#include "TRAPPCH.h"

#include "Layers/ImGuiLayer.h"

#include "Window/Window.h"
#include "Config/Config.h"
#include "Layers/LayerStack.h"
#include "Utils/Timer.h"
#include "Event/ApplicationEvent.h"
#include "Utils/Singleton.h"

namespace TRAP
{
	class Application : public Singleton
	{
	public:
		Application();
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Scope<Layer> layer);
		void PushOverlay(Scope<Layer> overlay);

		//const std::unique_ptr<Window>& GetWindow() const;
		Utils::Config* GetConfig();
		LayerStack& GetLayerStack();

		uint32_t GetFPS() const;
		float GetFrameTime() const;
		Utils::TimeStep GetTime() const;
		uint32_t GetDrawCalls() const;
		void AddSingleDrawCall();
		uint32_t GetTickRate() const;
		void SetTickRate(uint32_t);

		static void Shutdown();

		static Application& Get();
		static const std::unique_ptr<Window>& GetWindow();

		void ReCreateWindow(Graphics::API::RenderAPI renderAPI);
		void ReCreate(Graphics::API::RenderAPI renderAPI);

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_window;
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
		bool m_running = true;
		LayerStack m_layerStack;

		Utils::Config m_config;

		std::unique_ptr<Utils::Timer> m_timer;
		uint32_t m_FramesPerSecond;
		float m_FrameTime;
		uint32_t m_drawCalls;
		uint32_t m_fpsLimit;
		uint32_t m_tickRate;

		static Application* s_Instance;
	};

	//To be defined in CLIENT
	std::unique_ptr<Application> CreateApplication();
}

//-------------------------------------------------------------------------------------------------------------------//

//inline const std::unique_ptr<TRAP::Window>& TRAP::Application::GetWindow() const
inline const std::unique_ptr<TRAP::Window>& TRAP::Application::GetWindow()
{
	return Get().m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Utils::Config* TRAP::Application::GetConfig()
{
	return &m_config;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Application::GetFPS() const
{
	return m_FramesPerSecond;
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::Application::GetFrameTime() const
{
	return m_FrameTime;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Application::GetDrawCalls() const
{
	return m_drawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Application::AddSingleDrawCall()
{
	++m_drawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Application::GetTickRate() const
{
	return m_tickRate;
}


//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Application::SetTickRate(const uint32_t tickRate)
{
	m_tickRate = tickRate;
}


//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Application::Shutdown()
{
	Get().m_running = false;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Application& TRAP::Application::Get()
{
	return *s_Instance;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::LayerStack& TRAP::Application::GetLayerStack()
{
	return m_layerStack;
}

#endif /*_TRAP_APPLICATION_H_*/
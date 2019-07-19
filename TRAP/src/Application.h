#ifndef _TRAP_APPLICATION_H_
#define _TRAP_APPLICATION_H_
#include "TRAPPCH.h"

#include "Layers/ImGuiLayer.h"

namespace TRAP
{
	class Application
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

		void PushLayer(std::unique_ptr<Layer> layer);
		void PushOverlay(std::unique_ptr<Layer> overlay);

		Window* GetWindow() const;
		Utils::Config* GetConfig();
		LayerStack& GetLayerStack();

		unsigned int GetFPS() const;
		float GetFrameTime() const;
		Utils::TimeStep GetTime() const;

		void Shutdown();

		static Application& Get();

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
		unsigned int m_FramesPerSecond;
		float m_FrameTime;

		static Application* s_Instance;
	};

	//To be defined in CLIENT
	std::unique_ptr<Application> CreateApplication();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Window* TRAP::Application::GetWindow() const
{
	return m_window.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Utils::Config* TRAP::Application::GetConfig()
{
	return &m_config;
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Application::GetFPS() const
{
	return m_FramesPerSecond;
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::Application::GetFrameTime() const
{
	return m_FrameTime;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Application::Shutdown()
{
	m_running = false;
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
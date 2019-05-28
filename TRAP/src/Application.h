#pragma once
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

		Window* GetWindow() const { return m_window.get(); }
		Utils::Config* GetConfig() { return &m_config; }

		unsigned int GetFPS() const;
		float GetFrameTime() const;

		void Shutdown() { m_running = false; }

		static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_window;
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
		bool m_running = true;
		LayerStack m_layerStack;

		Utils::Config m_config;

		std::unique_ptr<Utils::Timer> m_timer;
		unsigned int m_FramesPerSecond;
		float m_Frametime;

		static Application* s_Instance;
	};

	//To be defined in CLIENT
	std::unique_ptr<Application> CreateApplication();
}

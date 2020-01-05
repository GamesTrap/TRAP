#ifndef _TRAP_APPLICATION_H_
#define _TRAP_APPLICATION_H_
#include "TRAPPCH.h"

#include "Layers/ImGui/ImGuiLayer.h"

#include "Window/Window.h"
#include "Config/Config.h"
#include "Layers/LayerStack.h"
#include "Utils/Timer.h"
#include "Event/ApplicationEvent.h"
#include "Utils/Singleton.h"
#include "Input/Input.h"

namespace TRAP
{
	class KeyPressedEvent;

	class Application : public Singleton
	{		
	public:
		enum class Endian
		{
			Little = 1,
			Big = 0
		};
		
		Application();
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;
		virtual ~Application();

		void Run();		

		void PushLayer(Scope<Layer> layer) const;
		void PushOverlay(Scope<Layer> overlay) const;

		Utils::Config* GetConfig();
		LayerStack& GetLayerStack() const;

		uint32_t GetFPS() const;
		float GetFrameTime() const;		
		uint32_t GetDrawCalls() const;
		void AddSingleDrawCall();
		uint32_t GetTickRate() const;
		void SetTickRate(uint32_t);

		static void Shutdown();

		static Application& Get();
		static const Scope<Window>& GetWindow();
		static Utils::TimeStep GetTime();
		static Endian GetEndian();

		static void SetClipboardString(const std::string& string);
		static std::string GetClipboardString();

		void ReCreateWindow(Graphics::API::RenderAPI renderAPI);
		void ReCreate(Graphics::API::RenderAPI renderAPI) const;

	private:		
		Utils::TimeStep GetTimeInternal() const;

		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPress(KeyPressedEvent& e) const;
		bool OnWindowFocus(WindowFocusEvent& e);
		bool OnWindowLostFocus(WindowLostFocusEvent& e);

		Scope<Window> m_window;
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
		bool m_running = true;
		bool m_minimized = false;
		bool m_focused = true;
		std::unique_ptr<LayerStack> m_layerStack;

		Utils::Config m_config;
		std::unique_ptr<Input> m_input;

		std::unique_ptr<Utils::Timer> m_timer;
		uint32_t m_FramesPerSecond;
		float m_FrameTime;
		uint32_t m_drawCalls;
		uint32_t m_fpsLimit;
		uint32_t m_tickRate;

		Endian m_endian;
		
		static Application* s_Instance;
	};

	//To be defined in CLIENT
	std::unique_ptr<Application> CreateApplication();
}

#endif /*_TRAP_APPLICATION_H_*/
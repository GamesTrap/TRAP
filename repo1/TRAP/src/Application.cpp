#include "TRAPPCH.h"
#include "Application.h"

#include "Core/Base.h"
#include "VFS/VFS.h"
#include "Embed.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/Renderer.h"
#include "Utils/String/String.h"
#include "Utils/Dialogs/MsgBox.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"
#include "Input/Input.h"
#include "Utils/Utils.h"
#include "Window/Monitor.h"

TRAP::Application* TRAP::Application::s_Instance = nullptr;
std::mutex TRAP::Application::s_hotReloadingMutex;
TRAP::CPUInfo TRAP::Application::s_CPU{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::Application()
	: m_timer(std::make_unique<Utils::Timer>()),
	m_FramesPerSecond(0),
	m_FrameTime(0.0f),
	m_drawCalls(0),
	m_fpsLimit(0),
	m_tickRate(100),
	m_timeScale(1.0f),
	m_linuxWindowManager(LinuxWindowManager::Unknown),
	m_threadPool(GetCPUInfo().LogicalCores > 1 ? GetCPUInfo().LogicalCores : std::thread::hardware_concurrency())
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::ApplicationPrefix, "Initializing TRAP Modules...");

	TRAP_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;
	m_mainThreadID = std::this_thread::get_id();

	TP_INFO(Log::ApplicationPrefix, "CPU: ", GetCPUInfo().LogicalCores, "x ", GetCPUInfo().Model);

	//Check if machine is using little-endian or big-endian
	int32_t intVal = 1;
	uint8_t* uVal = reinterpret_cast<uint8_t*>(&intVal);
#if __cplusplus > 201703L
	m_endian = static_cast<Endian>(std::endian::native == std::endian::little);
#else
	m_endian = static_cast<Endian>(uVal[0] == 1);
#endif

	UpdateLinuxWindowManager();

	//TODO Future Remove
	if (GetLinuxWindowManager() == LinuxWindowManager::Wayland)
	{
        TRAP::Utils::Dialogs::MsgBox::Show("Wayland is currently not supported by TRAP! Please use X11 instead", "Wayland unsupported!",
            TRAP::Utils::Dialogs::MsgBox::Style::Error, TRAP::Utils::Dialogs::MsgBox::Buttons::Quit);
		TP_CRITICAL(Log::EngineLinuxWaylandPrefix, "Wayland is currently not supported by TRAP! Please use X11 instead");
		exit(-1);
	}

	VFS::Init();
	if (!m_config.LoadFromFile("Engine.cfg"))
		TP_INFO(Log::ConfigPrefix, "Using default values");
#if defined(TRAP_DEBUG_CONFIGS)
	m_config.Print();
#endif

	uint32_t width = 1280;
	uint32_t height = 720;
	uint32_t refreshRate = 60;
	m_config.Get("Width", width);
	m_config.Get("Height", height);
	m_config.Get("RefreshRate", refreshRate);
	const uint32_t vsync = m_config.Get<uint32_t>("VSync");
	const uint32_t fpsLimit = m_config.Get<uint32_t>("FPSLimit");
	const Window::DisplayMode displayMode = m_config.Get<Window::DisplayMode>("DisplayMode");
	const bool maximized = m_config.Get<bool>("Maximized");
	const uint32_t monitor = m_config.Get<uint32_t>("Monitor");
	const bool rawInput = m_config.Get<bool>("RawMouseInput");
	const Graphics::API::RenderAPI renderAPI = m_config.Get<Graphics::API::RenderAPI>("RenderAPI");

	if (fpsLimit > 0)
	{
		if (fpsLimit >= 25 && fpsLimit <= 500)
			m_fpsLimit = fpsLimit;
		else
			m_fpsLimit = 0;
	}

	Graphics::API::Context::SetRenderAPI(renderAPI);
	m_window = MakeScope<Window>
	(
		WindowProps
		(
			"TRAP Engine",
			width,
			height,
			refreshRate,
			displayMode,
			WindowProps::AdvancedProps
			{
				vsync,
				true,
				maximized,
				true,
				true,
				true,
				true,
				rawInput,
				Window::CursorMode::Normal
			},
			monitor
		)
	);
	m_window->SetEventCallback([this](Events::Event& e) { OnEvent(e); });

	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Fallback", Embed::FallbackVS, Embed::FallbackFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());
	Graphics::TextureManager::Add(Graphics::TextureCube::Create());

	//Initialize Renderer
	Graphics::Renderer::Init();

	m_layerStack = std::make_unique<LayerStack>();

	//Initialize Input for Joysticks
	Input::SetEventCallback([this](Events::Event& e) {OnEvent(e); });
	Input::Init();

	Scope<ImGuiLayer> imguiLayer = TRAP::MakeScope<ImGuiLayer>();
	m_ImGuiLayer = imguiLayer.get();
	m_layerStack->PushOverlay(std::move(imguiLayer));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::~Application()
{
	TP_PROFILE_BEGIN_SESSION("Shutdown", "TRAPProfile-Shutdown.json");

	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::ApplicationPrefix, "Shutting down TRAP Modules...");
	if(m_hotReloadingThread)
	{
		m_hotReloadingThread->join();
		m_hotReloadingThread.reset();
	}
	Input::Shutdown();
	m_layerStack.reset();
	m_config.Set("Width", m_window->GetWidth());
	m_config.Set("Height", m_window->GetHeight());
	m_config.Set("RefreshRate", m_window->GetRefreshRate());
	m_config.Set("VSync", m_window->GetVSyncInterval());
	m_config.Set("FPSLimit", m_fpsLimit);
	m_config.Set("DisplayMode", m_window->GetDisplayMode());
	m_config.Set("Maximized", m_window->IsMaximized());
	m_config.Set("Monitor", m_window->GetMonitor().GetID());
	m_config.Set("RawMouseInput", m_window->GetRawMouseInput());
	m_config.Set("RenderAPI", Graphics::API::Context::GetRenderAPI());
	const std::array<uint8_t, 16> VulkanGPUUUID = Graphics::API::RendererAPI::GetRenderer()->GetCurrentGPUUUID();
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
		m_config.Set("VulkanGPU", Utils::UUIDToString(VulkanGPUUUID));
	else
	{
		if (m_config.Get<std::string_view>("VulkanGPU").empty())
			m_config.Set("VulkanGPU", "");
	}
#if defined(TRAP_DEBUG_CONFIGS)
	m_config.Print();
#endif
	m_config.SaveToFile("Engine.cfg");
	m_window.reset();
	VFS::Shutdown();

	TP_PROFILE_END_SESSION();
};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Run()
{
	TP_PROFILE_FUNCTION();

	float lastFrameTime = 0.0f;
	auto nextFrame = std::chrono::steady_clock::now();
	Utils::Timer tickTimer;

	while (m_running)
	{
		TP_PROFILE_SCOPE("RunLoop");

		if (m_fpsLimit)
			nextFrame += std::chrono::milliseconds(1000 / m_fpsLimit);
		if (!m_focused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
			nextFrame += std::chrono::milliseconds(1000 / 30); //30 FPS

		m_drawCalls = 0;

		Utils::Timer FrameTimeTimer;
		const float time = m_timer->Elapsed();
		const Utils::TimeStep deltaTime{ (time - lastFrameTime) * m_timeScale };
		lastFrameTime = time;

		if (!m_minimized)
		{
			{
				TP_PROFILE_SCOPE("LayerStack OnUpdate");

				for (const auto& layer : *m_layerStack)
					layer->OnUpdate(deltaTime);
			}

			if (tickTimer.ElapsedMilliseconds() > 1000.0f / static_cast<float>(m_tickRate))
			{
				{
					TP_PROFILE_SCOPE("LayerStack OnTick");

					for (const auto& layer : *m_layerStack)
						layer->OnTick();
				}

				tickTimer.Reset();
			}
		}

		Window::Use(m_window);
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
		{
			ImGuiLayer::Begin();
			{
				TP_PROFILE_SCOPE("LayerStack OnImGuiRender");

				for (const auto& layer : *m_layerStack)
					layer->OnImGuiRender();
			}
			ImGuiLayer::End();
		}

		if (!m_minimized)
			Graphics::RenderCommand::Present(m_window);
		TRAP::Window::OnUpdate();

		Graphics::Texture2D::UpdateLoadingTextures();

		if (!m_hotReloadingThread && (VFS::GetHotShaderReloading() || VFS::GetHotTextureReloading()))
			m_hotReloadingThread = TRAP::MakeScope<std::thread>(ProcessHotReloading, std::ref(m_hotReloadingShaderPaths), std::ref(m_hotReloadingTexturePaths), std::ref(m_running));
		UpdateHotReloading();

		if (Graphics::API::Context::s_newRenderAPI != Graphics::API::RenderAPI::NONE && Graphics::API::Context::s_newRenderAPI != Graphics::API::Context::GetRenderAPI())
		{
			//if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL || Graphics::API::Context::s_newRenderAPI == Graphics::API::RenderAPI::OpenGL)
			ReCreateWindow(Graphics::API::Context::s_newRenderAPI);
			/*else
				ReCreate(Graphics::API::Context::s_newRenderAPI);*/

			Graphics::API::Context::SetRenderAPI(Graphics::API::Context::s_newRenderAPI);
		}

		if (!m_minimized)
		{
			//Correct FrameTime when not Focused
			if (m_fpsLimit || (!m_focused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)))
				m_FrameTime = static_cast<float>(std::chrono::milliseconds(1000 / 30).count());
			else
				m_FrameTime = FrameTimeTimer.ElapsedMilliseconds();

			m_FramesPerSecond = static_cast<uint32_t>(1000.0f / m_FrameTime);
		}

		//FPSLimiter
		if (m_fpsLimit || (!m_focused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)))
			std::this_thread::sleep_until(nextFrame);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::OnEvent(Events::Event& e)
{
	TP_PROFILE_FUNCTION();

	Events::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Events::WindowCloseEvent>([this](Events::WindowCloseEvent& event) {return OnWindowClose(event); });
	dispatcher.Dispatch<Events::FrameBufferResizeEvent>([this](Events::FrameBufferResizeEvent& event) {return OnFrameBufferResize(event); });
	dispatcher.Dispatch<Events::KeyPressEvent>([this](Events::KeyPressEvent& event) {return OnKeyPress(event); });
	dispatcher.Dispatch<Events::WindowFocusEvent>([this](Events::WindowFocusEvent& event) {return OnWindowFocus(event); });
	dispatcher.Dispatch<Events::WindowLostFocusEvent>([this](Events::WindowLostFocusEvent& event) {return OnWindowLostFocus(event); });
	dispatcher.Dispatch<Events::WindowMinimizeEvent>([this](Events::WindowMinimizeEvent& event) {return OnWindowMinimize(event); });
	dispatcher.Dispatch<Events::WindowRestoreEvent>([this](Events::WindowRestoreEvent& event) {return OnWindowRestore(event); });

	if (m_layerStack)
	{
		for (auto it = m_layerStack->rbegin(); it != m_layerStack->rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushLayer(Scope<Layer> layer) const
{
	m_layerStack->PushLayer(std::move(layer));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushOverlay(Scope<Layer> overlay) const
{
	m_layerStack->PushOverlay(std::move(overlay));
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Utils::Config& TRAP::Application::GetConfig()
{
	return s_Instance->m_config;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::LayerStack& TRAP::Application::GetLayerStack()
{
	return *s_Instance->m_layerStack;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImGuiLayer TRAP::Application::GetImGuiLayer()
{
	return *s_Instance->m_ImGuiLayer;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetFPS()
{
	return s_Instance->m_FramesPerSecond;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Application::GetFrameTime()
{
	return s_Instance->m_FrameTime;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Application::GetTimeScale()
{
	return s_Instance->m_timeScale;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetDrawCalls()
{
	return s_Instance->m_drawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::AddSingleDrawCall()
{
	++(s_Instance->m_drawCalls);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Application::GetTickRate()
{
	return s_Instance->m_tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetTickRate(const uint32_t tickRate)
{
	s_Instance->m_tickRate = tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetTimeScale(const float timeScale)
{
	s_Instance->m_timeScale = timeScale;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetHotShaderReloading(const bool enabled)
{
	VFS::SetHotShaderReloading(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetHotTextureReloading(const bool enabled)
{
	VFS::SetHotTextureReloading(enabled);
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Shutdown()
{
	s_Instance->m_running = false;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Window>& TRAP::Application::GetWindow()
{
	return s_Instance->m_window;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::TimeStep TRAP::Application::GetTime()
{
	const Utils::TimeStep timeStep(s_Instance->m_timer->Elapsed());

	return timeStep;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::Endian TRAP::Application::GetEndian()
{
	return s_Instance->m_endian;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::LinuxWindowManager TRAP::Application::GetLinuxWindowManager()
{
	return s_Instance->m_linuxWindowManager;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ThreadPool& TRAP::Application::GetThreadPool()
{
	return s_Instance->m_threadPool;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetClipboardString(const std::string_view string)
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::SetClipboardString(string);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Application::GetClipboardString()
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::GetClipboardString();
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::CPUInfo& TRAP::Application::GetCPUInfo()
{
	if (s_CPU.Model.empty())
	{
		auto CPUID = [](const uint32_t funcID, const uint32_t subFuncID)->std::array <uint32_t, 4>
		{
			std::array<uint32_t, 4> regs{};
		#ifdef TRAP_PLATFORM_WINDOWS
			__cpuidex(reinterpret_cast<int32_t*>(regs.data()), static_cast<int32_t>(funcID), static_cast<int32_t>(subFuncID));
		#else
			asm volatile
				("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
					: "a" (funcID), "c" (subFuncID));
		#endif

			return regs;
		};

		std::array<uint32_t, 4> regs = CPUID(0, 0);
		const uint32_t HFS = regs[0];
		//Get Vendor
		const std::string vendorID = std::string(reinterpret_cast<const char*>(&regs[1]), 4) +
			std::string(reinterpret_cast<const char*>(&regs[3]), 4) +
			std::string(reinterpret_cast<const char*>(&regs[2]), 4);
		regs = CPUID(1, 0);
		s_CPU.HyperThreaded = regs[3] & 0x10000000; //Get Hyper-threading

		const std::string upVendorID = Utils::String::ToUpper(vendorID);
		//Get Number of cores
		constexpr int32_t MAX_INTEL_TOP_LVL = 4;
		constexpr uint32_t LVL_TYPE = 0x0000FF00;
		constexpr uint32_t LVL_CORES = 0x0000FFFF;
		if (upVendorID.find("INTEL") != std::string::npos)
		{
			if (HFS >= 11)
			{
				int32_t numSMT = 0;
				for (int32_t lvl = 0; lvl < MAX_INTEL_TOP_LVL; ++lvl)
				{
					std::array<uint32_t, 4> regs1 = CPUID(0x0B, lvl);
					const uint32_t currentLevel = (LVL_TYPE & regs1[2]) >> 8;
					switch (currentLevel)
					{
					case 0x01:
						numSMT = LVL_CORES & regs1[1];
						break;

					case 0x02:
						s_CPU.LogicalCores = LVL_CORES & regs1[1];
						break;

					default:
						break;
					}
				}
				s_CPU.Cores = s_CPU.LogicalCores / numSMT;
			}
			else
			{
				if (HFS >= 1)
				{
					s_CPU.LogicalCores = (regs[1] >> 16) & 0xFF;
					if (HFS >= 4)
					{
						std::array<uint32_t, 4> regs1 = CPUID(4, 0);
						s_CPU.Cores = 1 + (regs1[0] >> 26) & 0x3F;
					}
				}
				if (s_CPU.HyperThreaded)
				{
					if (!(s_CPU.Cores > 1))
					{
						s_CPU.Cores = 1;
						s_CPU.LogicalCores = (s_CPU.LogicalCores >= 2 ? s_CPU.LogicalCores : 2);
					}
				}
				else
					s_CPU.Cores = s_CPU.LogicalCores = 1;
			}
		}
		else if (upVendorID.find("AMD") != std::string::npos)
		{
			uint32_t extFamily;
			if (((regs[0] >> 8) & 0xF) < 0xF)
				extFamily = (regs[0] >> 8) & 0xF;
			else
				extFamily = ((regs[0] >> 8) & 0xF) + ((regs[0] >> 20) & 0xFF);

			if (HFS >= 1)
			{
				s_CPU.LogicalCores = (regs[1] >> 16) & 0xFF;
				std::array<uint32_t, 4> regs1 = CPUID(0x80000000, 0);
				if (regs1[0] >= 8)
				{
					regs1 = CPUID(0x80000008, 0);
					s_CPU.Cores = 1 + (regs1[2] & 0xFF);
				}
			}
			if (s_CPU.HyperThreaded)
			{
				if (!(s_CPU.Cores > 1))
				{
					s_CPU.Cores = 1;
					s_CPU.LogicalCores = (s_CPU.LogicalCores >= 2 ? s_CPU.LogicalCores : 2);
				}
				else if (s_CPU.Cores > 1)
				{
					//Ryzen 3 has SMT flag, but in fact cores count is equal to threads count.
					//Ryzen 5/7 reports twice as many "real" cores (e.g. 16 cores instead of 8) because of SMT.
					//On PPR 17h, page 82:
					//CPUID_Fn8000001E_EBX [Core Identifiers][15:8] is ThreadsPerCore
					//ThreadsPerCore: [...] The number of threads per core is ThreadsPerCore + 1
					std::array<uint32_t, 4> regs1 = CPUID(0x80000000, 0);
					if ((extFamily >= 23) && (regs1[0] >= 30))
					{
						regs1 = CPUID(0x8000001E, 0);
						s_CPU.Cores /= ((regs1[1] >> 8) & 0xFF) + 1;
					}
				}
			}
			else
				s_CPU.Cores = s_CPU.LogicalCores = 1;
		}

		//Get CPU brand string
		for (uint32_t i = 0x80000002; i < 0x80000005; ++i)
		{
			std::array<uint32_t, 4> regs1 = CPUID(i, 0);
			s_CPU.Model += std::string(reinterpret_cast<const char*>(&regs1[0]), 4);
			s_CPU.Model += std::string(reinterpret_cast<const char*>(&regs1[1]), 4);
			s_CPU.Model += std::string(reinterpret_cast<const char*>(&regs1[2]), 4);
			s_CPU.Model += std::string(reinterpret_cast<const char*>(&regs1[3]), 4);
		}
	}

	return s_CPU;
}

//-------------------------------------------------------------------------------------------------------------------//

std::thread::id TRAP::Application::GetMainThreadID()
{
	return s_Instance->m_mainThreadID;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::ReCreateWindow(const Graphics::API::RenderAPI renderAPI)
{
	TP_PROFILE_FUNCTION();

	for (const auto& layer : *m_layerStack)
		layer->OnDetach();
	Graphics::API::Context::SetRenderAPI(renderAPI);

	WindowProps props
	{
		m_window->GetTitle(),
		m_window->GetWidth(),
		m_window->GetHeight(),
		m_window->GetRefreshRate(),
		m_window->GetDisplayMode(),
		WindowProps::AdvancedProps
		{
			m_window->GetVSyncInterval(),
			m_window->IsResizable(),
			m_window->IsMaximized(),
			true,
			true,
			true,
			m_window->IsDecorated(),
			m_window->GetRawMouseInput(),
			m_window->GetCursorMode()
		},
		m_window->GetMonitor().GetID()
	};
	m_window.reset();
	m_window = std::make_unique<Window>(props);
	m_window->SetEventCallback([this](Events::Event& e) {OnEvent(e); });
	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Fallback", Embed::FallbackVS, Embed::FallbackFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());
	Graphics::TextureManager::Add(Graphics::TextureCube::Create());
	//Initialize Renderer
	Graphics::Renderer::Init();

	for (const auto& layer : *m_layerStack)
		layer->OnAttach();
}

//-------------------------------------------------------------------------------------------------------------------//

/*void TRAP::Application::ReCreate(const Graphics::API::RenderAPI renderAPI) const
{
	TP_PROFILE_FUNCTION();

	for (const auto& layer : *m_layerStack)
		layer->OnDetach();
	Graphics::API::Context::SetRenderAPI(renderAPI);

	Graphics::TextureManager::Shutdown();
	Graphics::ShaderManager::Shutdown();
	Graphics::Renderer::Shutdown();
	Graphics::API::RendererAPI::Shutdown();
	Graphics::API::Context::Shutdown();

	Graphics::API::Context::Create(m_window.get());
	Graphics::API::Context::SetVSyncInterval(m_window->GetVSyncInterval());
	Graphics::API::RendererAPI::Init();
	m_window->SetTitle(m_window->GetTitle());
	//Always added as a fallback shader
	Graphics::ShaderManager::Load("Fallback", Embed::FallbackVS, Embed::FallbackFS);
	//Always added as a fallback texture
	Graphics::TextureManager::Add(Graphics::Texture2D::Create());
	Graphics::TextureManager::Add(Graphics::TextureCube::Create());
	//Initialize Renderer
	Graphics::Renderer::Init();

	for (const auto& layer : *m_layerStack)
		layer->OnAttach();
}*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::UpdateLinuxWindowManager()
{
#ifdef TRAP_PLATFORM_LINUX
	std::string wl = "wayland";
	std::string x = "x11";
	std::string session;
	if(std::getenv("XDG_SESSION_TYPE"))
		session = std::getenv("XDG_SESSION_TYPE");
	if (std::getenv("WAYLAND_DISPLAY") || session == wl)
		m_linuxWindowManager = LinuxWindowManager::Wayland;
	else if (std::getenv("DISPLAY") || session == x)
		m_linuxWindowManager = LinuxWindowManager::X11;
	else
	{
		TP_CRITICAL(Log::EngineLinuxPrefix, "Unsupported Window Manager!");
		Utils::Dialogs::MsgBox::Show("Window Manager is unsupported!\nTRAP Engine uses X11 or Wayland\nMake sure the appropriate environment variables are set!", "Unsupported Window Manager", Utils::Dialogs::MsgBox::Style::Error, Utils::Dialogs::MsgBox::Buttons::Quit);
		exit(-1);
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowClose(Events::WindowCloseEvent& e)
{
	m_running = false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnFrameBufferResize(Events::FrameBufferResizeEvent& e)
{
	TP_PROFILE_FUNCTION();

	if (Window::GetActiveWindows() > 1)
		Window::Use();

	Graphics::RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnKeyPress(Events::KeyPressEvent& e) const
{
	if (Window::GetActiveWindows() == 1)
	{
		if ((e.GetKey() == Input::Key::Enter || e.GetKey() == Input::Key::KP_Enter) && Input::IsKeyPressed(Input::Key::Left_ALT) && e.GetRepeatCount() < 1)
		{
			if (m_window->GetDisplayMode() == Window::DisplayMode::Windowed ||
				m_window->GetDisplayMode() == Window::DisplayMode::Borderless)
				m_window->SetDisplayMode(Window::DisplayMode::Fullscreen, 0, 0, 0);
			else if (m_window->GetDisplayMode() == Window::DisplayMode::Fullscreen)
				m_window->SetDisplayMode(Window::DisplayMode::Windowed, 0, 0, 0);
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowFocus(Events::WindowFocusEvent& e)
{
	m_focused = true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowLostFocus(Events::WindowLostFocusEvent& e)
{
	if (Window::GetActiveWindows() == 1)
		m_focused = false;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowMinimize(Events::WindowMinimizeEvent& e)
{
	if (Window::GetActiveWindows() == 1)
		m_minimized = true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowRestore(Events::WindowRestoreEvent& e)
{
	m_minimized = false;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::ProcessHotReloading(std::vector<std::string>& shaders, std::vector<std::string>& textures, const bool& run)
{
	while (run)
	{
		//Update Shaders if needed
		if (VFS::GetHotShaderReloading() && VFS::GetShaderFileWatcher())
		{
			//Check monitoring shader folders for changes and
			//in case of changes run ShaderManager::Reload(virtualPath) (deferred into main thread)
			VFS::GetShaderFileWatcher()->Check([&](const std::filesystem::path& physicalPath,
				const std::string& virtualPath,
				const FileWatcher::FileStatus status) -> void
				{
					//Process only regular files and FileStatus::Modified
					if (!std::filesystem::is_regular_file(physicalPath))
						return;
					if (status == FileWatcher::FileStatus::Erased)
						return;

					const std::string_view suffix = Utils::String::GetSuffixStringView(virtualPath);
					if (suffix == "shader" || suffix == "spirv")
					{
						if (std::find(shaders.begin(), shaders.end(), virtualPath) == shaders.end())
						{
							{
								std::lock_guard<std::mutex> lock(s_hotReloadingMutex);
								shaders.emplace_back(virtualPath);
							}
						}
					}
				});
		}
		//Update Textures if needed
		if (VFS::GetHotTextureReloading() && VFS::GetTextureFileWatcher())
		{
			//Check monitoring texture folders for changes and
			//in case of changes run TextureManager::Reload(virtualPath) (deferred into main thread)
			VFS::GetTextureFileWatcher()->Check([&](const std::filesystem::path& physicalPath,
				const std::string& virtualPath,
				const FileWatcher::FileStatus status) -> void
				{
					//Process only regular files and FileStatus::Modified
					if (!std::filesystem::is_regular_file(physicalPath))
						return;
					if (status == FileWatcher::FileStatus::Erased)
						return;

					const std::string_view suffix = Utils::String::GetSuffixStringView(virtualPath);
					if (suffix == "pgm" || suffix == "ppm" || suffix == "pnm" || suffix == "pam" || suffix == "pfm" ||
						suffix == "tga" || suffix == "icb" || suffix == "vda" || suffix == "vst" || suffix == "bmp" ||
						suffix == "dib" || suffix == "png" || suffix == "hdr" || suffix == "pic")
					{
						if (std::find(textures.begin(), textures.end(), virtualPath) == textures.end())
						{
							{
								std::lock_guard<std::mutex> lock(s_hotReloadingMutex);
								textures.emplace_back(virtualPath);
							}
						}
					}
				});
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::UpdateHotReloading()
{
	//Shader Reloading
	for (const std::string& virtualPath : m_hotReloadingShaderPaths)
	{
		if (Graphics::ShaderManager::ExistsVirtualPath(virtualPath))
		{
			TP_INFO(Log::ShaderManagerPrefix, "Shader Modified Reloading...");
			Graphics::ShaderManager::Reload(virtualPath);
		}
	}
	m_hotReloadingShaderPaths.clear();

	//Texture Reloading
	for (const std::string& virtualPath : m_hotReloadingTexturePaths)
	{
		if (Graphics::TextureManager::ExistsVirtualPath(virtualPath))
		{
			TP_INFO(Log::ShaderManagerPrefix, "Texture Modified Reloading...");
			Graphics::TextureManager::Reload(virtualPath);
		}
	}
	m_hotReloadingTexturePaths.clear();
}
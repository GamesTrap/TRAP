#include "TRAPPCH.h"
#include "Application.h"

#include "Core/Base.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/FileWatcher.h"
#include "Embed.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/Renderer2D.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Textures/SpriteManager.h"
#include "Graphics/Textures/Texture.h"
#include "Graphics/Renderer.h"
#include "Utils/String/String.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"
#include "Events/HotReloadEvent.h"
#include "Events/FileEvent.h"
#include "Input/Input.h"
#include "Utils/Utils.h"
#include "Window/Monitor.h"
#include "Utils/Discord/DiscordGameSDK.h"
#include "Utils/Steam/SteamworksSDK.h"
#include "Utils/Memory.h"
#include "Utils/Time/TimeStep.h"
#include "Layers/ImGui/ImGuiLayer.h"

TRAP::Application* TRAP::Application::s_Instance = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_PLATFORM_WINDOWS) && defined(TRAP_HEADLESS_MODE)
static BOOL WINAPI SIGINTHandlerRoutine(_In_ DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_C_EVENT)
	{
		TRAP::Application::Shutdown();
		return TRUE;
	}
	else if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		TRAP::Application::Shutdown();
		Sleep(10000);
		return TRUE;
	}

	return FALSE;
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#if defined(ENABLE_SINGLE_PROCESS_ONLY) && defined(TRAP_PLATFORM_LINUX)
[[nodiscard]] static bool CheckSingleProcessLinux()
{
	ZoneScoped;

	static int32_t socketFD = -1;
	static int32_t rc = 1;
	static constexpr uint16_t port = 49420; //Just a free (hopefully) random port

	if(socketFD == -1 || rc)
	{
		socketFD = -1;
		rc = 1;

		socketFD = socket(AF_INET, SOCK_DGRAM, 0);
		if(socketFD < 0)
		{
			TP_ERROR(Log::ApplicationPrefix, "Failed to create socket!");
			TP_ERROR(Log::ApplicationPrefix, Utils::String::GetStrError());
			return false;
		}

		sockaddr_in name;
		name.sin_family = AF_INET;
		name.sin_port = port;
		name.sin_addr.s_addr = INADDR_ANY;

		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(name.sin_port);

		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
			TRAP::Utils::Memory::SwapBytes(name.sin_addr.s_addr);

		sockaddr convertedSock = TRAP::Utils::BitCast<sockaddr_in, sockaddr>(name); //Prevent usage of reinterpret_cast
		rc = bind(socketFD, &convertedSock, sizeof(name));
		if(rc < 0)
		{
			TP_ERROR(Log::ApplicationPrefix, "Failed to bind socket!");
			TP_ERROR(Log::ApplicationPrefix, Utils::String::GetStrError());
		}
	}

	return (socketFD != -1 && rc == 0);
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#if defined(ENABLE_SINGLE_PROCESS_ONLY) && defined(TRAP_PLATFORM_WINDOWS)
[[nodiscard]] static bool CheckSingleProcessWindows()
{
	ZoneScoped;

	const HANDLE hMutex = CreateMutex(0, 0, L"TRAP-Engine");
	if(!hMutex) //Error creating mutex
	{
		TP_ERROR(Log::ApplicationPrefix, "Failed to create mutex!");
		TP_ERROR(Log::ApplicationPrefix, Utils::String::GetStrError());
		return false;
	}
	if(hMutex && GetLastError() == ERROR_ALREADY_EXISTS)
		return false;

	return true;
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

static constexpr uint32_t DefaultTickrate = 64;
static constexpr uint32_t DefaultWindowWidth = 1280;
static constexpr uint32_t DefaultWindowHeight = 720;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::Application(std::string gameName, const uint32_t appID)
	: m_hotReloadingEnabled(false),
	  m_FrameTime(0.0f),
	  m_fpsLimit(0),
	  m_tickRate(DefaultTickrate),
	  m_timeScale(1.0f),
	  m_gameName(std::move(gameName)),
	  m_globalCounter(0),
	  m_threadPool(Utils::GetCPUInfo().LogicalCores > 1 ? (Utils::GetCPUInfo().LogicalCores - 1) :
	               std::thread::hardware_concurrency()),
	  m_newRenderAPI(Graphics::RenderAPI::NONE)
{
	ZoneScoped;

	//Register SIGINT callback to capture CTRL+C
#ifdef TRAP_HEADLESS_MODE
#ifdef TRAP_PLATFORM_LINUX
	if(signal(SIGINT, [](int) {TRAP::Application::Shutdown(); }) == SIG_ERR)
	{
		TP_ERROR(Log::ApplicationPrefix, "Failed to register SIGINT callback!");
		TP_ERROR(Log::ApplicationPrefix, Utils::String::GetStrError());
	}
#elif defined(TRAP_PLATFORM_WINDOWS)
	if(!SetConsoleCtrlHandler(SIGINTHandlerRoutine, TRUE))
	{
		TP_ERROR(Log::ApplicationPrefix, "Failed to set SIGINT handler!");
		TP_ERROR(Log::ApplicationPrefix, Utils::String::GetStrError());
	}
#endif
#endif

#ifdef TRACY_ENABLE
	//Set Main Thread name for profiler
	tracy::SetThreadName("Main Thread");
#endif

	//Single process mode
#ifdef ENABLE_SINGLE_PROCESS_ONLY
	bool singleProcess = true;
	#ifdef TRAP_PLATFORM_LINUX
		singleProcess = CheckSingleProcessLinux();
	#elif defined(TRAP_PLATFORM_WINDOWS)
		singleProcess = CheckSingleProcessWindows();
	#endif

	if(!singleProcess)
	{
		Utils::Dialogs::ShowMsgBox("TRAP™ is already running", "A TRAP™ Application is already running!\n"
		                           "Error code: 0x0012", Utils::Dialogs::Style::Error,
								   Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::ApplicationPrefix, "A TRAP™ Application is already running! (0x0012)");
		exit(0x0012);
	}
#endif

	TP_DEBUG(Log::ApplicationPrefix, "Initializing TRAP modules...");

	TRAP_ASSERT(!s_Instance, "Application(): Application already exists!");
	s_Instance = this;
	m_mainThreadID = std::this_thread::get_id();

	TP_INFO(Log::ApplicationPrefix, "CPU: ", Utils::GetCPUInfo().LogicalCores, "x ", Utils::GetCPUInfo().Model);

	FileSystem::Init();

	//Set main log file path
	const auto logFolder = FileSystem::GetGameLogFolderPath();
	if(logFolder)
		TRAP::TRAPLog.SetFilePath(*logFolder / "trap.log");
	else //Fallback to current directory
		TRAP::TRAPLog.SetFilePath("trap.log");

	TRAP::Utils::Steam::Initalize(appID);

	std::filesystem::path cfgPath = "engine.cfg";
#ifndef TRAP_HEADLESS_MODE
	const auto docsFolder = FileSystem::GetGameDocumentsFolderPath();
	if(docsFolder)
		cfgPath = *docsFolder / "engine.cfg";
#endif
	if (!m_config.LoadFromFile(cfgPath))
		TP_INFO(Log::ConfigPrefix, "Using default values");

#ifndef TRAP_HEADLESS_MODE
	uint32_t width = DefaultWindowWidth;
	uint32_t height = DefaultWindowHeight;
	uint32_t refreshRate = DefaultTickrate;
	m_config.Get("Width", width);
	m_config.Get("Height", height);
	m_config.Get("RefreshRate", refreshRate);
	const bool vsync = m_config.Get<bool>("VSync");
	const bool visible = true;
#ifdef NVIDIA_REFLEX_AVAILABLE
	const Graphics::LatencyMode latencyMode = m_config.Get<Graphics::LatencyMode>("NVIDIAReflex");
#endif /*NVIDIA_REFLEX_AVAILABLE*/
#else
	const uint32_t width = 2;
	const uint32_t height = 2;
	const uint32_t refreshRate = 60;
	const bool vsync = true;
	const Window::DisplayMode displayMode = Window::DisplayMode::Windowed;
	const uint32_t monitor = 0;
	const bool maximized = false;
	const bool rawInput = false;
	const bool visible = false;
#endif
	const uint32_t fpsLimit = m_config.Get<uint32_t>("FPSLimit");
#ifndef TRAP_HEADLESS_MODE
	const Window::DisplayMode displayMode = m_config.Get<Window::DisplayMode>("DisplayMode");
	const bool maximized = m_config.Get<bool>("Maximized");
	const uint32_t monitor = m_config.Get<uint32_t>("Monitor");
	const bool rawInput = m_config.Get<bool>("RawMouseInput");
#else
	bool enableGPU = true;
	m_config.Get("EnableGPU", enableGPU);
#endif
	Graphics::RenderAPI renderAPI = m_config.Get<Graphics::RenderAPI>("RenderAPI");

#ifdef TRAP_HEADLESS_MODE
	if(enableGPU)
	{
#endif
	if (renderAPI == Graphics::RenderAPI::NONE || !Graphics::RendererAPI::IsSupported(renderAPI))
		renderAPI = Graphics::RendererAPI::AutoSelectRenderAPI();
#ifdef TRAP_HEADLESS_MODE
	}
	else
		renderAPI = Graphics::RenderAPI::NONE;
#endif

	//Initialize Renderer
#ifdef TRAP_HEADLESS_MODE
	if(renderAPI != Graphics::RenderAPI::NONE)
	{
		//Retrieve RendererAPI values from engine.cfg
		Graphics::AntiAliasing antiAliasing = Graphics::AntiAliasing::Off;
		m_config.Get<TRAP::Graphics::AntiAliasing>("AntiAliasing", antiAliasing);
		Graphics::SampleCount antiAliasingSampleCount = Graphics::SampleCount::Two;
		m_config.Get<TRAP::Graphics::SampleCount>("AntiAliasingQuality", antiAliasingSampleCount);
		std::string anisotropyLevelStr = "16";
		m_config.Get<std::string>("AnisotropyLevel", anisotropyLevelStr);

		Graphics::RendererAPI::Init(m_gameName, renderAPI);
		Graphics::RenderCommand::SetAntiAliasing(antiAliasing, antiAliasingSampleCount);
		if(!anisotropyLevelStr.empty())
		{
			const Graphics::SampleCount anisotropyLevel = (anisotropyLevelStr == "Off") ? Graphics::SampleCount::One : Utils::String::ConvertToType<Graphics::SampleCount>(anisotropyLevelStr);
			Graphics::RenderCommand::SetAnisotropyLevel(anisotropyLevel);
		}
	}
#else
	//Retrieve RendererAPI values from engine.cfg
	Graphics::AntiAliasing antiAliasing = Graphics::AntiAliasing::Off;
	m_config.Get<TRAP::Graphics::AntiAliasing>("AntiAliasing", antiAliasing);
	Graphics::SampleCount antiAliasingSampleCount = Graphics::SampleCount::Two;
	m_config.Get<TRAP::Graphics::SampleCount>("AntiAliasingQuality", antiAliasingSampleCount);
	std::string anisotropyLevelStr = "16";
	m_config.Get<std::string>("AnisotropyLevel", anisotropyLevelStr);

	Graphics::RendererAPI::Init(m_gameName, renderAPI);
	Graphics::RenderCommand::SetAntiAliasing(antiAliasing, antiAliasingSampleCount);
	if(!anisotropyLevelStr.empty())
	{
		const Graphics::SampleCount anisotropyLevel = (anisotropyLevelStr == "Off") ? Graphics::SampleCount::One : Utils::String::ConvertToType<Graphics::SampleCount>(anisotropyLevelStr);
		Graphics::RenderCommand::SetAnisotropyLevel(anisotropyLevel);
	}
#endif

	//Window creation stuff
#ifdef TRAP_PLATFORM_LINUX
	if(TRAP::Utils::GetLinuxWindowManager() != TRAP::Utils::LinuxWindowManager::Unknown)
	{
#endif
		WindowProps::AdvancedProps advWinProps{};
		advWinProps.Maximized = maximized;
		advWinProps.Visible = visible;
		advWinProps.RawMouseInput = rawInput;

		WindowProps winProps{};
		winProps.Title = "TRAP™";
		winProps.Width = width;
		winProps.Height = height;
		winProps.RefreshRate = refreshRate;
		winProps.VSync = vsync;
		winProps.DisplayMode = displayMode;
		winProps.Monitor = monitor;
		winProps.Advanced = advWinProps;

		m_window = std::make_unique<Window>(winProps);
		m_window->SetEventCallback([this](Events::Event& event) { OnEvent(event); });

		//Update Window Title (Debug/RelWithDebInfo)
		if(renderAPI != Graphics::RenderAPI::NONE)
			m_window->SetTitle(m_window->GetTitle() + Graphics::RendererAPI::GetRenderer()->GetTitle());
#ifdef TRAP_PLATFORM_LINUX
	}
#ifdef TRAP_HEADLESS_MODE
	else //Headless without X11 or Wayland
	{
		if(TRAP::Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
			TRAP::Graphics::RendererAPI::GetRenderer()->InitPerWindowData(nullptr);
	}
#endif
#endif

	SetFPSLimit(fpsLimit);

#ifndef TRAP_HEADLESS_MODE
	//NVIDIA Reflex
#ifdef NVIDIA_REFLEX_AVAILABLE
	Graphics::RenderCommand::SetLatencyMode(latencyMode);
	NVSTATS_INIT(0, 0);
#endif /*NVIDIA_REFLEX_AVAILABLE*/
#endif

	if(renderAPI != Graphics::RenderAPI::NONE)
	{
		float renderScale = 1.0f;
		m_config.Get<float>("RenderScale", renderScale);
		Graphics::RenderCommand::SetRenderScale(renderScale);

		//Always added as a fallback shader
		Graphics::ShaderManager::LoadSource("FallbackGraphics", std::string(Embed::FallbackGraphicsShader))->Use();
		Graphics::ShaderManager::LoadSource("FallbackCompute", std::string(Embed::FallbackComputeShader))->Use();

		//Always added as a fallback texture
		Graphics::TextureManager::Add(Graphics::Texture::CreateFallback2D());
		Graphics::TextureManager::Add(Graphics::Texture::CreateFallbackCube());

		//Initialize Renderer
		Graphics::Renderer::Init();
	}

	//Initialize Input for Joysticks
#ifdef TRAP_PLATFORM_LINUX
	if(TRAP::Utils::GetLinuxWindowManager() != TRAP::Utils::LinuxWindowManager::Unknown)
	{
#endif
		Input::SetEventCallback([this](Events::Event& event) {OnEvent(event); });
		Input::Init();
#ifdef TRAP_PLATFORM_LINUX
	}
#endif

#ifndef TRAP_HEADLESS_MODE
	if(Graphics::RendererAPI::GPUSettings.SurfaceSupported &&
	   Graphics::RendererAPI::GPUSettings.PresentSupported)
	{
		Scope<ImGuiLayer> imguiLayer = TRAP::MakeScope<ImGuiLayer>();
		m_ImGuiLayer = imguiLayer.get();
		m_layerStack.PushOverlay(std::move(imguiLayer));
	}
#endif

	TRAP::Utils::Discord::Create();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Application::~Application()
{
	ZoneScoped;
	TP_DEBUG(Log::ApplicationPrefix, "Shutting down TRAP modules...");

	if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
		TRAP::Graphics::RendererAPI::GetRenderer()->WaitIdle();

	m_layerStack.Shutdown();

	TRAP::Utils::Steam::Shutdown();
	TRAP::Utils::Discord::Destroy();
	if(TRAP::Utils::GetLinuxWindowManager() != TRAP::Utils::LinuxWindowManager::Unknown)
		Input::Shutdown();

#ifndef TRAP_HEADLESS_MODE
	if(m_window->GetWidth() > 0)
		m_config.Set("Width", m_window->GetWidth());
	if(m_window->GetHeight() > 0)
		m_config.Set("Height", m_window->GetHeight());
	m_config.Set("RefreshRate", m_window->GetRefreshRate());
	m_config.Set("VSync", m_window->GetVSync());
	m_config.Set("DisplayMode", m_window->GetDisplayMode());
	m_config.Set("Maximized", m_window->IsMaximized());
	m_config.Set("Monitor", m_window->GetMonitor().GetID());
	m_config.Set("RawMouseInput", m_window->GetRawMouseInput());
#else
	m_config.Set("EnableGPU", Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE);
#endif

	m_config.Set("FPSLimit", m_fpsLimit);
	m_config.Set("RenderAPI", (m_newRenderAPI != Graphics::RenderAPI::NONE) ? m_newRenderAPI :
	                                                                          Graphics::RendererAPI::GetRenderAPI());

	if (Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
	{
		//GPU UUID
		std::array<uint8_t, 16> GPUUUID{};
		if(Graphics::RendererAPI::GetNewGPU() != std::array<uint8_t, 16>{}) //Only if UUID is not empty
			GPUUUID = Graphics::RendererAPI::GetNewGPU();
		else
			GPUUUID = Graphics::RendererAPI::GetRenderer()->GetCurrentGPUUUID();
		m_config.Set("VulkanGPU", Utils::UUIDToString(GPUUUID));

		Graphics::AntiAliasing antiAliasing = Graphics::AntiAliasing::Off;
		Graphics::SampleCount antiAliasingSampleCount = Graphics::SampleCount::Two;
		Graphics::RenderCommand::GetAntiAliasing(antiAliasing, antiAliasingSampleCount);
		const Graphics::SampleCount anisotropyLevel = Graphics::RenderCommand::GetAnisotropyLevel();
		m_config.Set("AntiAliasing", antiAliasing);
		m_config.Set("AntiAliasingQuality", antiAliasingSampleCount);
		m_config.Set("AnisotropyLevel", (anisotropyLevel == Graphics::SampleCount::One) ? "Off" : Utils::String::ConvertToString(anisotropyLevel));
		m_config.Set("RenderScale", Graphics::RenderCommand::GetRenderScale());

		//NVIDIA Reflex
#if !defined(TRAP_HEADLESS_MODE) && defined(NVIDIA_REFLEX_AVAILABLE)
		NVSTATS_SHUTDOWN();
		m_config.Set("NVIDIAReflex", Graphics::RenderCommand::GetLatencyMode());
#endif
	}

	std::filesystem::path cfgPath = "engine.cfg";
#ifndef TRAP_HEADLESS_MODE
	const auto docsFolder = FileSystem::GetGameDocumentsFolderPath();
	if(docsFolder)
		cfgPath = *docsFolder / "engine.cfg";
#endif
	m_config.SaveToFile(cfgPath);
	m_window.reset();

	if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
	{
		TP_TRACE(Log::ApplicationPrefix, "Shutting down Renderer");
		Graphics::Renderer::Shutdown();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down SpriteManager");
		Graphics::SpriteManager::Clean();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down TextureManager");
		Graphics::TextureManager::Shutdown();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down ShaderManager");
		Graphics::ShaderManager::Shutdown();
		TP_TRACE(Log::ApplicationPrefix, "Shutting down RendererAPI");
		Graphics::RendererAPI::Shutdown();
	}

	FileSystem::Shutdown();

	s_Instance = nullptr;
};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Run()
{
	ZoneScoped;

	float lastFrameTime = 0.0f;
	float tickTimerSeconds = 0.0f;
	Utils::Timer limiterTimer{};

	while (m_running)
	{
		const Utils::Timer FrameTimeTimer{};
		const float time = m_timer.Elapsed();
		const Utils::TimeStep deltaTime{ (time - lastFrameTime) * m_timeScale };
		tickTimerSeconds += time - lastFrameTime;
		lastFrameTime = time;

		//FPSLimiter
		if(Graphics::RendererAPI::GPUSettings.ReflexSupported)
			Graphics::RendererAPI::GetRenderer()->ReflexSleep();
#ifndef TRACY_ENABLE
		else if ((m_fpsLimit != 0u) || (!m_focused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)))
#else
		else if ((m_fpsLimit != 0u))
#endif
		{
			std::chrono::duration<float, std::milli> limitMs{};
			if(m_fpsLimit != 0u)
				limitMs = std::chrono::duration<float, std::milli>(1000.0f / static_cast<float>(m_fpsLimit) - limiterTimer.ElapsedMilliseconds());
			else //If engine is not focused, set engine to 30 FPS so other applications dont lag
				limitMs = std::chrono::duration<float, std::milli>(1000.0f / 30.0f - limiterTimer.ElapsedMilliseconds());
			std::this_thread::sleep_for(limitMs); //If this is too inaccurate, resort to using nanosleep
			limiterTimer.Reset();
		}

#ifdef NVIDIA_REFLEX_AVAILABLE
		Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, VK_SIMULATION_START);
		Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, VK_INPUT_SAMPLE);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

#ifdef TRAP_PLATFORM_LINUX
		if(TRAP::Utils::GetLinuxWindowManager() != TRAP::Utils::LinuxWindowManager::Unknown)
			TRAP::Window::OnUpdate();
#else
		TRAP::Window::OnUpdate();
#endif

#ifdef NVIDIA_REFLEX_AVAILABLE
		if(Input::IsMouseButtonPressed(Input::MouseButton::Left))
			Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, VK_TRIGGER_FLASH);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

		if (!m_minimized)
		{
			for (const auto& layer : m_layerStack)
				layer->OnUpdate(deltaTime);

			//Timestep of a single tick
			const Utils::TimeStep tickRateTimeStep{1000.0f / static_cast<float>(m_tickRate) / 1000.0f};
			//If we reached at least one fixed time step update
			if (tickTimerSeconds >= tickRateTimeStep)
			{
				static constexpr uint32_t MAX_TICKS_PER_FRAME = 8;

				//Count how many ticks we need to run (this is limited to a maximum of MAX_TICKS_PER_FRAME)
				const uint32_t fixedTimeSteps = TRAP::Math::Min(static_cast<uint32_t>(tickTimerSeconds / tickRateTimeStep), MAX_TICKS_PER_FRAME);
				// TP_TRACE("Ticks: ", fixedTimeSteps);

				// TP_TRACE("Before: ", tickTimerSeconds, "s of tick time remaining");
				//Call OnTick() fixedTimeSteps times.
				for(uint32_t i = 0; i < fixedTimeSteps; ++i)
				{
					for (const auto& layer : m_layerStack)
						layer->OnTick(tickRateTimeStep);

					//Instead of resetting the timer, we just subtract the current tick from the counted time.
					tickTimerSeconds -= tickRateTimeStep;
				}
				// TP_TRACE("After: ", tickTimerSeconds, "s of tick time remaining");
			}

			if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
				Graphics::RendererAPI::OnPostUpdate();

#ifndef TRAP_HEADLESS_MODE
			ImGuiLayer::Begin();
			{
				for (const auto& layer : m_layerStack)
					layer->OnImGuiRender();
			}
			ImGuiLayer::End();
#endif
			if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
			{
				Graphics::RenderCommand::Flush(m_window.get());
				Graphics::Renderer2D::Reset();
			}
		}

		UpdateHotReloading();

		//Needed by Discord Game SDK
		TRAP::Utils::Discord::RunCallbacks();
		//Needed by Steamworks SDK
		TRAP::Utils::Steam::RunCallbacks();

#ifdef NVIDIA_REFLEX_AVAILABLE
		Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, VK_SIMULATION_END);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

		if (!m_minimized)
			m_FrameTime = FrameTimeTimer.ElapsedMilliseconds();

		++m_globalCounter;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::OnEvent(Events::Event& event)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Events::WindowCloseEvent>([this](Events::WindowCloseEvent& wcEvent)
		{
			return OnWindowClose(wcEvent);
		});
	dispatcher.Dispatch<Events::FrameBufferResizeEvent>([](Events::FrameBufferResizeEvent& fbrEvent)
		{
			return OnFrameBufferResize(fbrEvent);
		});
	dispatcher.Dispatch<Events::KeyPressEvent>([this](Events::KeyPressEvent& kpEvent) {return OnKeyPress(kpEvent); });
	dispatcher.Dispatch<Events::WindowFocusEvent>([this](Events::WindowFocusEvent& wfEvent)
		{
			return OnWindowFocus(wfEvent);
		});
	dispatcher.Dispatch<Events::WindowLostFocusEvent>([this](Events::WindowLostFocusEvent& wlfEvent)
		{
			return OnWindowLostFocus(wlfEvent);
		});
	dispatcher.Dispatch<Events::WindowMinimizeEvent>([this](Events::WindowMinimizeEvent& wmEvent)
		{
			return OnWindowMinimize(wmEvent);
		});
	dispatcher.Dispatch<Events::WindowRestoreEvent>([this](Events::WindowRestoreEvent& wrEvent)
		{
			return OnWindowRestore(wrEvent);
		});
	dispatcher.Dispatch<Events::FileChangeEvent>([this](Events::FileChangeEvent& fcEvent)
		{
			return OnFileChangeEvent(fcEvent);
		});

	for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
	{
		if (event.Handled)
			break;
		(*it)->OnEvent(event);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushLayer(std::unique_ptr<Layer> layer)
{
	ZoneScoped;

	m_layerStack.PushLayer(std::move(layer));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::PushOverlay(std::unique_ptr<Layer> overlay)
{
	ZoneScoped;

	m_layerStack.PushOverlay(std::move(overlay));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Utils::Config& TRAP::Application::GetConfig()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_config;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::LayerStack& TRAP::Application::GetLayerStack()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_layerStack;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::ImGuiLayer& TRAP::Application::GetImGuiLayer()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return *(s_Instance->m_ImGuiLayer);
}

//-------------------------------------------------------------------------------------------------------------------//

static constexpr uint32_t MinLimitedFPS = 25u;
static constexpr uint32_t MaxLimitedFPS = 500u;

void TRAP::Application::SetFPSLimit(const uint32_t fps)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(fps == 0)
		s_Instance->m_fpsLimit = 0;
	else
		s_Instance->m_fpsLimit = TRAP::Math::Clamp(fps, MinLimitedFPS, MaxLimitedFPS);

#ifdef NVIDIA_REFLEX_AVAILABLE
	if(TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::NONE &&
	   TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported)
	{
		Graphics::RendererAPI::GetRenderer()->SetReflexFPSLimit(s_Instance->m_fpsLimit);
	}
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Application::GetFPSLimit()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_fpsLimit;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Application::GetCPUFrameTime()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_FrameTime;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Application::GetTimeScale()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_timeScale;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Application::GetTickRate()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetTickRate(const uint32_t tickRate)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	s_Instance->m_tickRate = tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetTimeScale(const float timeScale)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	s_Instance->m_timeScale = timeScale;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetNewRenderAPI(const Graphics::RenderAPI renderAPI)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	s_Instance->m_newRenderAPI = renderAPI;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Shutdown()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	s_Instance->m_running = false;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Window* TRAP::Application::GetWindow()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_window.get();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::TimeStep TRAP::Application::GetTime()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const Utils::TimeStep timeStep(s_Instance->m_timer.Elapsed());

	return timeStep;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::ThreadPool& TRAP::Application::GetThreadPool()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_threadPool;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetClipboardString(const std::string& string)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	INTERNAL::WindowingAPI::SetClipboardString(string);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Application::GetClipboardString()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return INTERNAL::WindowingAPI::GetClipboardString();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::thread::id TRAP::Application::GetMainThreadID()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_mainThreadID;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Application::GetGameName()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_gameName;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint64_t TRAP::Application::GetGlobalCounter()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_globalCounter;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::FileSystem::FileWatcher* TRAP::Application::GetHotReloadingFileWatcher()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(s_Instance->m_hotReloadingFileWatcher)
		return s_Instance->m_hotReloadingFileWatcher.get();

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Application::IsHotReloadingEnabled()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_Instance->m_hotReloadingEnabled;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetHotReloading(const bool enable)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	s_Instance->m_hotReloadingEnabled = enable;

	if(enable && !s_Instance->m_hotReloadingFileWatcher)
	{
		s_Instance->m_hotReloadingFileWatcher = std::make_unique<FileSystem::FileWatcher>("HotReloading", false);
		s_Instance->m_hotReloadingFileWatcher->SetEventCallback([](Events::Event& event) {s_Instance->OnEvent(event); });
	}
	else if(s_Instance->m_hotReloadingFileWatcher)
		s_Instance->m_hotReloadingFileWatcher.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowClose(Events::WindowCloseEvent& event) noexcept
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(event.GetWindow() == m_window.get())
		m_running = false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnFrameBufferResize(Events::FrameBufferResizeEvent& event)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	Graphics::RendererAPI::ResizeSwapChain(event.GetWindow());

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnKeyPress([[maybe_unused]] Events::KeyPressEvent& event) const
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(Window::GetActiveWindows() != 1)
		return false;

#ifndef TRAP_HEADLESS_MODE
	if ((event.GetKey() == Input::Key::Enter || event.GetKey() == Input::Key::KP_Enter) &&
	    Input::IsKeyPressed(Input::Key::Left_ALT) && event.GetRepeatCount() < 1)
	{
		if (m_window->GetDisplayMode() == Window::DisplayMode::Windowed ||
			m_window->GetDisplayMode() == Window::DisplayMode::Borderless)
			m_window->SetDisplayMode(Window::DisplayMode::Fullscreen, 0, 0, 0);
		else if (m_window->GetDisplayMode() == Window::DisplayMode::Fullscreen)
			m_window->SetDisplayMode(Window::DisplayMode::Windowed, 0, 0, 0);

		return true;
	}
#endif

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowFocus([[maybe_unused]] Events::WindowFocusEvent& event) noexcept
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	m_focused = true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowLostFocus([[maybe_unused]] Events::WindowLostFocusEvent& event) noexcept
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (Window::GetActiveWindows() == 1)
		m_focused = false;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowMinimize([[maybe_unused]] Events::WindowMinimizeEvent& event) noexcept
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (Window::GetActiveWindows() == 1)
		m_minimized = true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnWindowRestore([[maybe_unused]] Events::WindowRestoreEvent& event) noexcept
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	m_minimized = false;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::UpdateHotReloading()
{
	ZoneScoped;

	std::vector<std::filesystem::path> shaderPaths;
	std::vector<std::filesystem::path> texturePaths;

	//Hot code
	{
		std::lock_guard lock(m_hotReloadingMutex);
		LockMark(m_hotReloadingMutex);

		//Shader
		shaderPaths = m_hotReloadingShaderPaths;
		m_hotReloadingShaderPaths.clear();

		//Textures
		texturePaths = m_hotReloadingTexturePaths;
		m_hotReloadingTexturePaths.clear();
	}

	//Shader reloading
	for(const auto& path : shaderPaths)
	{
		if(!Graphics::ShaderManager::ExistsPath(path))
			continue;

		TP_INFO(Log::HotReloadingPrefix, "Shader modified reloading...");
		Graphics::RendererAPI::GetRenderer()->WaitIdle();
		Ref<TRAP::Graphics::Shader> shader = Graphics::ShaderManager::Reload(path.string());

		//By binding the fallback shader, we can make sure that the
		//new shader will trigger a pipeline rebuild.
		if(static_cast<bool>(shader->GetShaderStages() & Graphics::RendererAPI::ShaderStage::Compute))
			TRAP::Graphics::ShaderManager::Get("FallbackCompute")->Use();
		else
			TRAP::Graphics::ShaderManager::Get("FallbackGraphics")->Use();

		//Send event
		TRAP::Events::ShaderReloadEvent event(shader);
		OnEvent(event);
	}

	//Texture reloading
	for(const auto& path : texturePaths)
	{
		if(!Graphics::TextureManager::ExistsPath(path))
			continue;

		TP_INFO(Log::HotReloadingPrefix, "Texture modified reloading...");
		Graphics::RendererAPI::GetRenderer()->WaitIdle();
		TRAP::Ref<TRAP::Graphics::Texture> texture = Graphics::TextureManager::Reload(path.string());

		//Send event
		TRAP::Events::TextureReloadEvent event(texture);
		OnEvent(event);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnFileChangeEvent(const Events::FileChangeEvent& event)
{
	ZoneScoped;

	if(event.GetStatus() != FileSystem::FileStatus::Modified)
		return false; //Only handle modified files

	const auto fileEnding = FileSystem::GetFileEnding(event.GetPath());
	if(!fileEnding) //Ignore files without an extension
		return false;

	const std::string fEnding = Utils::String::ToLower(*fileEnding);

	//Is it a texture?
	bool texture = false;
	for(const auto& fmt : Image::SupportedImageFormatSuffixes)
	{
		if(fEnding == fmt)
		{
			texture = true;
			break;
		}
	}

	bool shader = false;
	if(!texture) //Or is it a shader?
	{
		for(const auto& fmt : Graphics::Shader::SupportedShaderFormatSuffixes)
		{
			if(fEnding == fmt)
			{
				shader = true;
				break;
			}
		}
	}

	if(!texture && !shader)
		return false; //Not a texture or shader

	if(texture)
	{
		std::lock_guard lock(m_hotReloadingMutex); //Hot code
		LockMark(m_hotReloadingMutex);

		//Don't add duplicates!
		for(const auto& path : m_hotReloadingTexturePaths)
		{
			if(FileSystem::IsEquivalent(path, event.GetPath()))
				return false;
		}

		m_hotReloadingTexturePaths.push_back(event.GetPath());
	}
	else if(shader)
	{
		std::lock_guard lock(m_hotReloadingMutex); //Hot code
		LockMark(m_hotReloadingMutex);

		//Don't add duplicates!
		for(const auto& path : m_hotReloadingShaderPaths)
		{
			if(FileSystem::IsEquivalent(path, event.GetPath()))
				return false;
		}

		m_hotReloadingShaderPaths.push_back(event.GetPath());
	}

	return false;
}
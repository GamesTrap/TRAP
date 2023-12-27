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
#include "Utils/ErrorCodes/ErrorCodes.h"
#include "Utils/String/String.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"
#include "Events/HotReloadEvent.h"
#include "Events/FileEvent.h"
#include "Input/Input.h"
#include "Utils/Utils.h"
#include "Window/Monitor.h"
#include "Utils/Discord/DiscordGameSDK.h"
#include "Utils/Steam/SteamworksSDK.h"
#include "Utils/Time/TimeStep.h"
#include "Layers/ImGui/ImGuiLayer.h"
#include "Utils/DBus/DBus.h"

#ifdef TRAP_UNITTESTS

TRAP::Application::Application(std::string gameName, [[maybe_unused]] const std::optional<u32> appID)
	: m_gameName(std::move(gameName))
{
	TRAP_ASSERT(!s_Instance, "Application(): Application already exists!");
	s_Instance = this;
}

#else

TRAP::Application::Application(std::string gameName, const std::optional<u32> appID)
	: m_gameName(std::move(gameName))
{
	ZoneScoped;

#ifdef TRAP_HEADLESS_MODE
	TRAP::Utils::RegisterSIGINTCallback();
#endif /*TRAP_HEADLESS_MODE*/

#ifdef TRACY_ENABLE
	//Set Main Thread name for profiler
	tracy::SetThreadName("Main Thread");
#endif /*TRACY_ENABLE*/

	Utils::CheckSingleProcess();
	[[maybe_unused]] const auto _ = Utils::GetLinuxWindowManager(); //On Linux if no known window manager is found this will exit the engine

	TP_DEBUG(Log::ApplicationPrefix, "Initializing TRAP modules...");

	TRAP_ASSERT(!s_Instance, "Application(): Application already exists!");
	s_Instance = this;

	TP_INFO(Log::ApplicationPrefix, "CPU: ", Utils::GetCPUInfo().LogicalCores, "x ", Utils::GetCPUInfo().Model);

	FileSystem::Init();

	//Set main log file path (uses current folder as fallback)
	TRAP::TRAPLog.SetFilePath(FileSystem::GetGameLogFolderPath().ValueOr("") / "trap.log");

	if(appID)
		TRAP::Utils::Steam::InitializeClient(*appID);

	m_config = LoadTRAPConfig();

	const Graphics::RenderAPI renderAPI = SelectRenderAPI(m_config);

	InitializeRendererAPI(m_gameName, renderAPI, m_config);

#ifndef TRAP_HEADLESS_MODE
	m_window = CreateMainWindow(LoadWindowProps(m_config));
#else
	CreateMainViewport(m_config);
#endif /*TRAP_HEADLESS_MODE*/

	if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
	{
		LoadFallbackData();
		ApplyRendererAPISettings(m_config);

		//Initialize Renderer
		Graphics::Renderer::Init();
	}

	SetFPSLimit(m_config.Get<u32>("FPSLimit").value_or(0u));
	SetUnfocusedFPSLimit(m_config.Get<u32>("UnfocusedFPSLimit").value_or(30u));

#ifndef TRAP_HEADLESS_MODE
	InitializeInput();
	m_ImGuiLayer = InitializeImGui(m_layerStack);

	TRAP::Utils::Discord::Create();
#endif /*TRAP_HEADLESS_MODE*/
}

#endif

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_UNITTESTS

TRAP::Application::~Application()
{
}

#else

TRAP::Application::~Application()
{
	ZoneScoped;

	TP_DEBUG(Log::ApplicationPrefix, "Shutting down TRAP modules...");

	if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
		TRAP::Graphics::RendererAPI::GetRenderer()->WaitIdle();

	m_layerStack.Shutdown();

	TRAP::Utils::Steam::Shutdown();
#ifndef TRAP_HEADLESS_MODE
	TRAP::Utils::Discord::Destroy();

	Input::Shutdown();

	UpdateTRAPConfig(m_config, m_window.get(), m_fpsLimit, m_unfocusedFPSLimit, m_newRenderAPI);
#else
	UpdateTRAPConfig(m_config, m_fpsLimit, m_unfocusedFPSLimit, m_newRenderAPI);
#endif /*TRAP_HEADLESS_MODE*/
	SaveTRAPConfig(m_config);

#ifndef TRAP_HEADLESS_MODE
	m_window.reset();
#endif /*TRAP_HEADLESS_MODE*/

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

#ifdef TRAP_PLATFORM_LINUX
	DBus::UnloadSymbols();
#endif /*TRAP_PLATFORM_LINUX*/

	s_Instance = nullptr;
}

#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Run()
{
	ZoneScoped;

	f32 lastFrameTime = 0.0f;
	f32 tickTimerSeconds = 0.0f;
	Utils::Timer limiterTimer{};

	while (m_running)
	{
		const Utils::Timer FrameTimeTimer{};
		const Utils::TimeStep deltaTime = UpdateNewFrameTimeData(m_timer, lastFrameTime, tickTimerSeconds, m_timeScale);

#if defined(TRAP_HEADLESS_MODE) || defined(TRACY_ENABLE)
		if(m_fpsLimit != 0u)
			LimitFPS(m_fpsLimit, limiterTimer);
#else
		if(Window::GetActiveWindows() == 1 && !m_window->IsFocused() &&
		   !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
		{
			UnfocusedLimitFPS(m_unfocusedFPSLimit, limiterTimer);
		}
		else if(m_fpsLimit != 0u)
			LimitFPS(m_fpsLimit, limiterTimer);
#endif

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, PCLSTATS_SIMULATION_START);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		TRAP::Window::OnUpdate();
#endif /*TRAP_HEADLESS_MODE*/

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		if(Input::IsMouseButtonPressed(Input::MouseButton::Left))
			Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, PCLSTATS_TRIGGER_FLASH);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		if(Window::GetActiveWindows() > 1 || !m_window->IsMinimized())
#endif /*TRAP_HEADLESS_MODE*/
		{
			RunWork(deltaTime, tickTimerSeconds);
		}

		UpdateHotReloading();

#ifndef TRAP_HEADLESS_MODE
		//Needed by Discord Game SDK
		TRAP::Utils::Discord::RunCallbacks();
#endif /*TRAP_HEADLESS_MODE*/
		//Needed by Steamworks SDK
		TRAP::Utils::Steam::RunCallbacks();

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, PCLSTATS_SIMULATION_END);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

		m_FrameTime = FrameTimeTimer.ElapsedMilliseconds();

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		++m_globalCounter;
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::RunWork(const Utils::TimeStep& deltaTime, f32& tickTimerSeconds)
{
	for (const auto& layer : m_layerStack)
		layer->OnUpdate(deltaTime);

	//Timestep of a single tick
	const Utils::TimeStep tickRateTimeStep{1000.0f / NumericCast<f32>(m_tickRate) / 1000.0f}; //Tickrate in seconds
	//If we reached at least one fixed time step update
	if (tickTimerSeconds >= tickRateTimeStep)
	{
		static constexpr u32 MAX_TICKS_PER_FRAME = 8;

		//Count how many ticks we need to run (this is limited to a maximum of MAX_TICKS_PER_FRAME)
		const u32 fixedTimeSteps = TRAP::Math::Min(NumericCast<u32>(tickTimerSeconds / tickRateTimeStep), MAX_TICKS_PER_FRAME);
		// TP_TRACE("Ticks: ", fixedTimeSteps);

		// TP_TRACE("Before: ", tickTimerSeconds, "s of tick time remaining");
		//Call OnTick() fixedTimeSteps times.
		for(u32 i = 0; i < fixedTimeSteps; ++i)
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
#endif /*TRAP_HEADLESS_MODE*/

	if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
	{
#ifndef TRAP_HEADLESS_MODE
		Graphics::RenderCommand::Flush(m_window.get());
#else
		Graphics::RenderCommand::Flush();
#endif /*TRAP_HEADLESS_MODE*/
		Graphics::Renderer2D::Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::OnEvent(Events::Event& event)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	Events::EventDispatcher dispatcher(event);
#ifndef TRAP_HEADLESS_MODE
	dispatcher.Dispatch<Events::FrameBufferResizeEvent>(OnFrameBufferResize);
	dispatcher.Dispatch<Events::WindowCloseEvent>(std::bind_front(&Application::OnWindowClose, this));
	dispatcher.Dispatch<Events::KeyPressEvent>(OnKeyPress);
#endif /*TRAP_HEADLESS_MODE*/
	dispatcher.Dispatch<Events::FileChangeEvent>(std::bind_front(&Application::OnFileChangeEvent, this));

	for (auto& it : std::ranges::reverse_view(m_layerStack))
	{
		if (event.Handled)
			break;
		it->OnEvent(event);
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

	TRAP_ASSERT(s_Instance, "Application::GetConfig(): Application is nullptr!");

	return s_Instance->m_config;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::LayerStack& TRAP::Application::GetLayerStack()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetLayerStack(): Application is nullptr!");

	return s_Instance->m_layerStack;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::ImGuiLayer& TRAP::Application::GetImGuiLayer()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetImGuiLayer(): Application is nullptr!");
	TRAP_ASSERT(s_Instance->m_ImGuiLayer != nullptr, "Application::GetImGuiLayer(): ImGuiLayer is nullptr!");

	return *(s_Instance->m_ImGuiLayer);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

static constexpr u32 MinLimitedFPS = 25u;
static constexpr u32 MaxLimitedFPS = 500u;

void TRAP::Application::SetFPSLimit(const u32 targetFPS)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::SetFPSLimit(): Application is nullptr!");

	s_Instance->m_fpsLimit = (targetFPS != 0) ? TRAP::Math::Clamp(targetFPS, MinLimitedFPS, MaxLimitedFPS) : 0u;

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	if(TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::NONE &&
	   TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported)
	{
		Graphics::RendererAPI::GetRenderer()->SetReflexFPSLimit(s_Instance->m_fpsLimit);
	}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Application::GetFPSLimit()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetFPSLimit(): Application is nullptr!");

	return s_Instance->m_fpsLimit;
}

//-------------------------------------------------------------------------------------------------------------------//

static constexpr u32 MinUnfocusedFPS = 10u;

void TRAP::Application::SetUnfocusedFPSLimit(const u32 targetFPS)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::SetFPSLimit(): Application is nullptr!");

	s_Instance->m_unfocusedFPSLimit = (targetFPS != 0) ? TRAP::Math::Clamp(targetFPS, MinUnfocusedFPS, MaxLimitedFPS) : 0u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Application::GetUnfocusedFPSLimit()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetFPSLimit(): Application is nullptr!");

	return s_Instance->m_unfocusedFPSLimit;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Application::GetCPUFrameTime()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetCPUFrameTime(): Application is nullptr!");

	return s_Instance->m_FrameTime;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Application::GetTimeScale()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetTimeScale(): Application is nullptr!");

	return s_Instance->m_timeScale;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Application::GetTickRate()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetTickRate(): Application is nullptr!");

	return s_Instance->m_tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetTickRate(const u32 tickRate)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::SetTickRate(): Application is nullptr!");

	s_Instance->m_tickRate = tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetTimeScale(const f32 timeScale)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::SetTimeScale(): Application is nullptr!");
	TRAP_ASSERT(timeScale >= 0.0f, "Application::SetTimeScale(): timeScale is negative!");

	if(timeScale >= 0.0f)
		s_Instance->m_timeScale = timeScale;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetNewRenderAPI(const Graphics::RenderAPI renderAPI)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::SetNewRenderAPI(): Application is nullptr!");

	s_Instance->m_newRenderAPI = renderAPI;
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::Shutdown()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::Shutdown(): Application is nullptr!");

	s_Instance->m_running = false;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Window* TRAP::Application::GetWindow()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetWindow(): Application is nullptr!");
	TRAP_ASSERT(s_Instance->m_window, "Application::GetWindow(): Window is nullptr!");

	return s_Instance->m_window.get();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::TimeStep TRAP::Application::GetTime()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetTime(): Application is nullptr!");

	return Utils::TimeStep(s_Instance->m_timer.Elapsed());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::ThreadPool& TRAP::Application::GetThreadPool()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetThreadPool(): Application is nullptr!");

	return s_Instance->m_threadPool;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Application::SetClipboardString(const std::string& string)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	INTERNAL::WindowingAPI::SetClipboardString(string);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] std::string TRAP::Application::GetClipboardString()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return INTERNAL::WindowingAPI::GetClipboardString();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::thread::id TRAP::Application::GetMainThreadID()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetMainThreadID(): Application is nullptr!");

	return s_Instance->m_mainThreadID;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Application::GetGameName()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetGameName(): Application is nullptr!");

	return s_Instance->m_gameName;
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
[[nodiscard]] u64 TRAP::Application::GetGlobalCounter()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetGlobalCounter(): Application is nullptr!");

	return s_Instance->m_globalCounter;
}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<std::reference_wrapper<TRAP::FileSystem::FileWatcher>> TRAP::Application::GetHotReloadingFileWatcher()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetHotReloadingFileWatcher(): Application is nullptr!");

	if(s_Instance->m_hotReloadingFileWatcher)
		return *s_Instance->m_hotReloadingFileWatcher;

	return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Application::IsHotReloadingEnabled()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::IsHotReloadingEnabled(): Application is nullptr!");

	return s_Instance->m_hotReloadingFileWatcher != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetHotReloading(const bool enable)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::SetHotReloading(): Application is nullptr!");

	if(enable && !s_Instance->m_hotReloadingFileWatcher)
	{
		s_Instance->m_hotReloadingFileWatcher = std::make_unique<FileSystem::FileWatcher>("HotReloading", false);
		s_Instance->m_hotReloadingFileWatcher->SetEventCallback([](Events::Event& event) {s_Instance->OnEvent(event); });
	}
	else if(!enable && s_Instance->m_hotReloadingFileWatcher)
		s_Instance->m_hotReloadingFileWatcher.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
bool TRAP::Application::OnWindowClose(const Events::WindowCloseEvent& event) noexcept
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(event.GetWindow() == *m_window)
		m_running = false;

	return true;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
bool TRAP::Application::OnFrameBufferResize(const Events::FrameBufferResizeEvent& event)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	Graphics::RendererAPI::ResizeSwapChain(&event.GetWindow());

	return false;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
bool TRAP::Application::OnKeyPress([[maybe_unused]] const Events::KeyPressEvent& event)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if ((event.GetKey() == Input::Key::Enter || event.GetKey() == Input::Key::KP_Enter) &&
	    Input::IsKeyPressed(Input::Key::Left_ALT, &event.GetWindow()))
	{
		if (event.GetWindow().GetDisplayMode() == Window::DisplayMode::Windowed ||
			event.GetWindow().GetDisplayMode() == Window::DisplayMode::Borderless)
		{
			event.GetWindow().SetFullscreen();
		}
		else if (event.GetWindow().GetDisplayMode() == Window::DisplayMode::Fullscreen)
			event.GetWindow().SetWindowed();

		return true;
	}

	return false;
}
#endif /*TRAP_HEADLESS_MODE*/

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
		if((shader->GetShaderStages() & Graphics::RendererAPI::ShaderStage::Compute) != Graphics::RendererAPI::ShaderStage::None)
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

TRAP::Utils::TimeStep TRAP::Application::UpdateNewFrameTimeData(const Utils::Timer& time, f32& lastFrameTime,
                                                                f32& tickTimerSeconds, const f32 timeScale)
{
	const f32 elapsedTime = time.Elapsed();

	const Utils::TimeStep deltaTime{ (elapsedTime - lastFrameTime) * timeScale };

	tickTimerSeconds += elapsedTime - lastFrameTime;
	lastFrameTime = elapsedTime;

	return deltaTime;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::LimitFPS(const u32 fpsLimit, Utils::Timer& limitTimer)
{
	if(fpsLimit == 0u)
		return;

#ifndef TRAP_HEADLESS_MODE
	if(Graphics::RendererAPI::GPUSettings.ReflexSupported)
	{
		Graphics::RendererAPI::GetRenderer()->ReflexSleep();
		return;
	}
#endif /*TRAP_HEADLESS_MODE*/

	const std::chrono::duration<f32, std::milli> limitMs =
		std::chrono::duration<f32, std::milli>(1000.0f / NumericCast<f32>(fpsLimit) - limitTimer.ElapsedMilliseconds());
	std::this_thread::sleep_for(limitMs); //If this is too inaccurate, resort to using nanosleep
	limitTimer.Reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::UnfocusedLimitFPS(const u32 fpsLimit, Utils::Timer& limitTimer)
{
	const std::chrono::duration<f32, std::milli> limitMs =
		std::chrono::duration<f32, std::milli>(1000.0f / NumericCast<f32>(fpsLimit) - limitTimer.ElapsedMilliseconds());
	std::this_thread::sleep_for(limitMs); //If this is too inaccurate, resort to using nanosleep
	limitTimer.Reset();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Application::OnFileChangeEvent(const Events::FileChangeEvent& event)
{
	ZoneScoped;

	if(event.GetStatus() != FileSystem::FileStatus::Modified && event.GetStatus() != FileSystem::FileStatus::Created)
		return false; //Only handle modified and created files

	const auto fileEnding = FileSystem::GetFileEnding(event.GetPath());
	if(!fileEnding) //Ignore files without an extension
		return false;

	const std::string fEnding = Utils::String::ToLower(*fileEnding);

	if(std::ranges::contains(Image::SupportedImageFormatSuffixes, fEnding))
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
	else if(std::ranges::contains(Graphics::Shader::SupportedShaderFormatSuffixes, fEnding))
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

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::Application::GetTRAPConfigPath()
{
#ifdef TRAP_HEADLESS_MODE
	return "engine.cfg";
#else
	return TRAP::FileSystem::GetGameDocumentsFolderPath().ValueOr("") / "engine.cfg";
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Config TRAP::Application::LoadTRAPConfig()
{
	TRAP::Utils::Config config{};

	if (!config.LoadFromFile(GetTRAPConfigPath()))
		TP_INFO(TRAP::Log::ConfigPrefix, "Using default values");

	return config;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Application::UpdateTRAPConfig(Utils::Config& config, const TRAP::Window* const window,
                                         const u32 fpsLimit, const u32 unfocusedFPSLimit,
										 const Graphics::RenderAPI renderAPI)
#else
void TRAP::Application::UpdateTRAPConfig(Utils::Config& config, const u32 fpsLimit,
                                         const u32 unfocusedFPSLimit,
                                         const Graphics::RenderAPI renderAPI)
#endif /*TRAP_HEADLESS_MODE*/
{
#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window != nullptr, "Application::UpdateTRAPConfig(); window is nullptr!");

	if(window != nullptr)
	{
		if(const auto displayMode = window->GetDisplayMode(); displayMode == Window::DisplayMode::Fullscreen || displayMode == Window::DisplayMode::Borderless)
		{
			//Save the unscaled pixel size
			const auto videoMode = window->GetMonitor().GetCurrentVideoMode();
			if(videoMode && videoMode->Width > 0 && videoMode->Height > 0)
			{
				config.Set("Width", videoMode->Width);
				config.Set("Height", videoMode->Height);
			}
		}
		else
		{
			//Save the possibly scaled logical window size
			if(window->GetWidth() > 0)
				config.Set("Width", window->GetWidth());
			if(window->GetHeight() > 0)
				config.Set("Height", window->GetHeight());
		}
		config.Set("RefreshRate", window->GetRefreshRate());
		config.Set("VSync", window->GetVSync());
		config.Set("DisplayMode", window->GetDisplayMode());
		config.Set("Maximized", window->IsMaximized());
		config.Set("Monitor", window->GetMonitor().GetID());
		config.Set("RawMouseInput", window->GetRawMouseInput());
	}
#else
	config.Set("EnableGPU", Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE);
#endif /*TRAP_HEADLESS_MODE*/

	config.Set("FPSLimit", fpsLimit);
	config.Set("UnfocusedFPSLimit", unfocusedFPSLimit);
	config.Set("RenderAPI", (renderAPI != Graphics::RenderAPI::NONE) ? renderAPI :
	                                                                   Graphics::RendererAPI::GetRenderAPI());

	if (Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
	{
#ifdef TRAP_HEADLESS_MODE
		u32 width = 1920;
		u32 height = 1080;
		Graphics::RendererAPI::GetRenderer()->GetResolution(width, height);

		config.Set("Width", width);
		config.Set("Height", height);
#endif /*TRAP_HEADLESS_MODE*/

		//GPU UUID
		TRAP::Utils::UUID GPUUUID{};
		if(Graphics::RendererAPI::GetNewGPU() != TRAP::Utils::UUID{}) //Only if UUID is not empty
			GPUUUID = Graphics::RendererAPI::GetNewGPU();
		else
			GPUUUID = Graphics::RendererAPI::GetRenderer()->GetCurrentGPUUUID();
		config.Set(fmt::format("{}GPU", Graphics::RendererAPI::GetRenderAPI()), Utils::UUIDToString(GPUUUID));

		Graphics::AntiAliasing antiAliasing = Graphics::AntiAliasing::Off;
		Graphics::SampleCount antiAliasingSampleCount = Graphics::SampleCount::Two;
		Graphics::RenderCommand::GetAntiAliasing(antiAliasing, antiAliasingSampleCount);
		const Graphics::SampleCount anisotropyLevel = Graphics::RenderCommand::GetAnisotropyLevel();
		config.Set("AntiAliasing", antiAliasing);
		config.Set("AntiAliasingQuality", antiAliasingSampleCount);
		config.Set("AnisotropyLevel", (anisotropyLevel == Graphics::SampleCount::One) ? "Off" : fmt::format("{}", anisotropyLevel));
		config.Set("RenderScale", Graphics::RenderCommand::GetRenderScale());

		//NVIDIA Reflex
#if !defined(TRAP_HEADLESS_MODE) && defined(NVIDIA_REFLEX_AVAILABLE)
		PCLSTATS_SHUTDOWN();
		config.Set("NVIDIAReflex", Graphics::RenderCommand::GetLatencyMode());
#endif /*!TRAP_HEADLESS_MODE && NVIDIA_REFLEX_AVAILABLE*/
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SaveTRAPConfig(TRAP::Utils::Config& config)
{
	config.SaveToFile(GetTRAPConfigPath());
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
TRAP::WindowProps::AdvancedProps TRAP::Application::LoadAdvancedWindowProps(const TRAP::Utils::Config& config)
{
	TRAP::WindowProps::AdvancedProps props{};

	props.Maximized = config.Get<bool>("Maximized").value_or(false);
	props.Visible = true;
	props.RawMouseInput = config.Get<bool>("RawMouseInput").value_or(false);

	return props;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
TRAP::WindowProps TRAP::Application::LoadWindowProps(const TRAP::Utils::Config& config)
{
	static constexpr u32 DefaultWindowWidth = 1280;
	static constexpr u32 DefaultWindowHeight = 720;
	static constexpr u32 DefaultWindowRefreshRate = 60;

	TRAP::WindowProps props{};

	props.Title = "TRAP™";
	props.Width = config.Get<u32>("Width").value_or(DefaultWindowWidth);
	props.Height = config.Get<u32>("Height").value_or(DefaultWindowHeight);
	props.RefreshRate = config.Get<f64>("RefreshRate").value_or(DefaultWindowRefreshRate);
	props.VSync = config.Get<bool>("VSync").value_or(false);
	props.DisplayMode = config.Get<TRAP::Window::DisplayMode>("DisplayMode").value_or(TRAP::Window::DisplayMode::Windowed);
	const auto monitors = TRAP::Monitor::GetAllMonitors();
	const u32 monitorIdx = config.Get<u32>("Monitor").value_or(0u);
	if(monitors.empty())
		Utils::DisplayError(Utils::ErrorCode::MonitorNoneFound);
	else if(monitorIdx < monitors.size())
		props.Monitor = TRAP::Monitor::GetAllMonitors()[config.Get<u32>("Monitor").value_or(0u)];
	else //Fallback to primary monitor
		props.Monitor = TRAP::Monitor::GetAllMonitors()[0];

	props.Advanced = LoadAdvancedWindowProps(config);

	return props;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderAPI TRAP::Application::SelectRenderAPI(const TRAP::Utils::Config& config)
{
	TRAP::Graphics::RenderAPI renderAPI = config.Get<TRAP::Graphics::RenderAPI>("RenderAPI").value_or(TRAP::Graphics::RenderAPI::NONE);

#ifdef TRAP_HEADLESS_MODE
	const bool enableGPU = config.Get<bool>("EnableGPU").value_or(true);

	if(enableGPU &&
	   (renderAPI == TRAP::Graphics::RenderAPI::NONE || !TRAP::Graphics::RendererAPI::IsSupported(renderAPI)))
	{
		renderAPI = TRAP::Graphics::RendererAPI::AutoSelectRenderAPI();
	}
	else if(!enableGPU)
		renderAPI = TRAP::Graphics::RenderAPI::NONE;
#else
	if (renderAPI == TRAP::Graphics::RenderAPI::NONE || !TRAP::Graphics::RendererAPI::IsSupported(renderAPI))
		renderAPI = TRAP::Graphics::RendererAPI::AutoSelectRenderAPI();
#endif

	return renderAPI;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::InitializeRendererAPI(const std::string_view gameName,
                                              const TRAP::Graphics::RenderAPI& renderAPI,
											  const TRAP::Utils::Config& config)
{
#ifdef TRAP_HEADLESS_MODE
	if(renderAPI == TRAP::Graphics::RenderAPI::NONE)
		return;
#endif /*TRAP_HEADLESS_MODE*/

	TRAP::Graphics::RendererAPI::Init(gameName, renderAPI);

	const TRAP::Graphics::SampleCount antiAliasingSampleCount = config.Get<TRAP::Graphics::SampleCount>("AntiAliasingQuality").value_or(TRAP::Graphics::SampleCount::Two);
	TRAP::Graphics::RenderCommand::SetAntiAliasing(config.Get<TRAP::Graphics::AntiAliasing>("AntiAliasing").value_or(TRAP::Graphics::AntiAliasing::Off),
	                                               antiAliasingSampleCount != TRAP::Graphics::SampleCount::One ?
												   antiAliasingSampleCount : TRAP::Graphics::SampleCount::Two);

	const std::string anisotropyLevelStr = config.Get<std::string>("AnisotropyLevel").value_or("Off");
	if(!anisotropyLevelStr.empty())
	{
		const TRAP::Graphics::SampleCount anisotropyLevel = (Utils::String::CompareAnyCase(anisotropyLevelStr, "Off")) ?
		                                                    TRAP::Graphics::SampleCount::One :
															TRAP::Utils::String::ConvertToType<TRAP::Graphics::SampleCount>(anisotropyLevelStr);
		TRAP::Graphics::RenderCommand::SetAnisotropyLevel(anisotropyLevel);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
std::unique_ptr<TRAP::Window> TRAP::Application::CreateMainWindow(const TRAP::WindowProps& winProps)
{
	std::unique_ptr<TRAP::Window> window = std::make_unique<TRAP::Window>(winProps);
	if(window)
		window->SetEventCallback([](TRAP::Events::Event& event) { s_Instance->OnEvent(event); });

	return window;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Application::CreateMainViewport(const TRAP::Utils::Config& config)
{
	const u32 width = config.Get<u32>("Width").value_or(1920u);
	const u32 height = config.Get<u32>("Height").value_or(1080u);

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::NONE)
		TRAP::Graphics::RendererAPI::GetRenderer()->InitPerViewportData(width, height);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::LoadFallbackShaders()
{
	TRAP_ASSERT(TRAP::Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE,
	            "Application::LoadFallbackShaders(): RenderAPI must not be RenderAPI::NONE!");

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::NONE)
		return;

	TRAP::Graphics::ShaderManager::LoadSource("FallbackGraphics", std::string(TRAP::Embed::FallbackGraphicsShader))->Use();
	TRAP::Graphics::ShaderManager::LoadSource("FallbackCompute", std::string(TRAP::Embed::FallbackComputeShader))->Use();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::LoadFallbackTextures()
{
	TRAP_ASSERT(TRAP::Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE,
	            "Application::LoadFallbackTextures(): RenderAPI must not be RenderAPI::NONE!");

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::NONE)
		return;

	TRAP::Graphics::TextureManager::Add(TRAP::Graphics::Texture::CreateFallback2D());
	TRAP::Graphics::TextureManager::Add(TRAP::Graphics::Texture::CreateFallbackCube());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::LoadFallbackData()
{
	TRAP_ASSERT(TRAP::Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE,
	            "Application::LoadFallbackData(): RenderAPI must not be RenderAPI::NONE!");

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::NONE)
		return;

	LoadFallbackShaders();
	LoadFallbackTextures();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::ApplyRendererAPISettings(const TRAP::Utils::Config& config)
{
	TRAP_ASSERT(TRAP::Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE,
	            "Application::ApplyRendererAPISettings(): RenderAPI must not be RenderAPI::NONE!");

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::NONE)
		return;

#if !defined(TRAP_HEADLESS_MODE) && defined(NVIDIA_REFLEX_AVAILABLE)
	//NVIDIA Reflex
	Graphics::RenderCommand::SetLatencyMode(config.Get<Graphics::LatencyMode>("NVIDIAReflex").value_or(Graphics::LatencyMode::Disabled));
	PCLSTATS_INIT(0);
#endif /*!TRAP_HEADLESS_MODE && NVIDIA_REFLEX_AVAILABLE*/

	const f32 renderScale = config.Get<f32>("RenderScale").value_or(1.0f);
	Graphics::RenderCommand::SetRenderScale(renderScale);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Application::InitializeInput()
{
	Input::SetEventCallback([](Events::Event& event) { s_Instance->OnEvent(event); });
	Input::Init();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
TRAP::ImGuiLayer* TRAP::Application::InitializeImGui(TRAP::LayerStack& layerStack)
{
	Scope<ImGuiLayer> imguiLayer = TRAP::MakeScope<ImGuiLayer>();
	ImGuiLayer* imguiLayerPtr = nullptr;
	if(imguiLayer)
	{
		imguiLayerPtr = imguiLayer.get();
		layerStack.PushOverlay(std::move(imguiLayer));
	}

	return imguiLayerPtr;
}
#endif /*TRAP_HEADLESS_MODE*/
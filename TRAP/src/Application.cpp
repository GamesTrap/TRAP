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

TRAP::Application::Application(std::string gameName, [[maybe_unused]] const std::optional<uint32_t> appID)
	: m_threadPool(Utils::GetCPUInfo().LogicalCores > 1 ? (Utils::GetCPUInfo().LogicalCores - 1) :
	               std::thread::hardware_concurrency()),
	  m_mainThreadID(std::this_thread::get_id()),
	  m_gameName(std::move(gameName))
{
	ZoneScoped;

#ifdef TRAP_HEADLESS_MODE
	TRAP::Utils::RegisterSIGINTCallback();
#endif /*TRAP_HEADLESS_MODE*/

#ifdef TRACY_ENABLE
	//Set Main Thread name for profiler
	tracy::SetThreadName("Main Thread");
#endif

	Utils::CheckSingleProcess();
	Utils::GetLinuxWindowManager(); //On Linux if no known window manager is found this will exit the engine

	TP_DEBUG(Log::ApplicationPrefix, "Initializing TRAP modules...");

	TRAP_ASSERT(!s_Instance, "Application(): Application already exists!");
	s_Instance = this;

	TP_INFO(Log::ApplicationPrefix, "CPU: ", Utils::GetCPUInfo().LogicalCores, "x ", Utils::GetCPUInfo().Model);

	FileSystem::Init();

	//Set main log file path (uses current folder as fallback)
	TRAP::TRAPLog.SetFilePath(FileSystem::GetGameLogFolderPath().value_or("") / "trap.log");

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

	SetFPSLimit(m_config.Get<uint32_t>("FPSLimit"));

#ifndef TRAP_HEADLESS_MODE
	InitializeInput();
	m_ImGuiLayer = InitializeImGui(m_layerStack);

	TRAP::Utils::Discord::Create();
#endif /*TRAP_HEADLESS_MODE*/
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
#ifndef TRAP_HEADLESS_MODE
	TRAP::Utils::Discord::Destroy();

	Input::Shutdown();

	UpdateTRAPConfig(m_config, m_window.get(), m_fpsLimit, m_newRenderAPI);
#else
	UpdateTRAPConfig(m_config, m_fpsLimit, m_newRenderAPI);
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
		const Utils::TimeStep deltaTime = UpdateNewFrameTimeData(m_timer, lastFrameTime, tickTimerSeconds, m_timeScale);

#if defined(TRAP_HEADLESS_MODE) || defined(TRACY_ENABLE)
		if(m_fpsLimit != 0u)
			LimitFPS(m_fpsLimit, limiterTimer);
#else
		if(Window::GetActiveWindows() == 1 && !m_window->IsFocused() &&
		   !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
		{
			UnfocusedLimitFPS(30, limiterTimer);
		}
		else if(m_fpsLimit != 0u)
			LimitFPS(m_fpsLimit, limiterTimer);
#endif

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, PCLSTATS_SIMULATION_START);
		// Graphics::RendererAPI::GetRenderer()->ReflexMarker(m_globalCounter, PCLSTATS_INPUT_SAMPLE); //Deprecated
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

void TRAP::Application::RunWork(const Utils::TimeStep& deltaTime, float& tickTimerSeconds)
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
	dispatcher.Dispatch<Events::FrameBufferResizeEvent>([](Events::FrameBufferResizeEvent& fbrEvent)
		{
			return OnFrameBufferResize(fbrEvent);
		});
	dispatcher.Dispatch<Events::WindowCloseEvent>([this](Events::WindowCloseEvent& wcEvent)
		{
			return OnWindowClose(wcEvent);
		});
	dispatcher.Dispatch<Events::KeyPressEvent>([](Events::KeyPressEvent& kpEvent) {return OnKeyPress(kpEvent); });
#endif /*TRAP_HEADLESS_MODE*/
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

static constexpr uint32_t MinLimitedFPS = 25u;
static constexpr uint32_t MaxLimitedFPS = 500u;

void TRAP::Application::SetFPSLimit(const uint32_t targetFPS)
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

[[nodiscard]] uint32_t TRAP::Application::GetFPSLimit()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetFPSLimit(): Application is nullptr!");

	return s_Instance->m_fpsLimit;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Application::GetCPUFrameTime()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetCPUFrameTime(): Application is nullptr!");

	return s_Instance->m_FrameTime;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Application::GetTimeScale()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetTimeScale(): Application is nullptr!");

	return s_Instance->m_timeScale;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Application::GetTickRate()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetTickRate(): Application is nullptr!");

	return s_Instance->m_tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetTickRate(const uint32_t tickRate)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::SetTickRate(): Application is nullptr!");

	s_Instance->m_tickRate = tickRate;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::SetTimeScale(const float timeScale)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::SetTimeScale(): Application is nullptr!");

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
[[nodiscard]] uint64_t TRAP::Application::GetGlobalCounter()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetGlobalCounter(): Application is nullptr!");

	return s_Instance->m_globalCounter;
}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::FileSystem::FileWatcher* TRAP::Application::GetHotReloadingFileWatcher()
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(s_Instance, "Application::GetHotReloadingFileWatcher(): Application is nullptr!");

	if(s_Instance->m_hotReloadingFileWatcher)
		return s_Instance->m_hotReloadingFileWatcher.get();

	return nullptr;
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
bool TRAP::Application::OnWindowClose(Events::WindowCloseEvent& event) noexcept
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(event.GetWindow() == m_window.get())
		m_running = false;

	return true;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
bool TRAP::Application::OnFrameBufferResize(Events::FrameBufferResizeEvent& event)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(event.GetWindow() != nullptr)
		Graphics::RendererAPI::ResizeSwapChain(event.GetWindow());

	return false;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
bool TRAP::Application::OnKeyPress([[maybe_unused]] Events::KeyPressEvent& event)
{
	ZoneNamed(__tracy, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(event.GetWindow() == nullptr)
		return false;

	if ((event.GetKey() == Input::Key::Enter || event.GetKey() == Input::Key::KP_Enter) &&
	    Input::IsKeyPressed(Input::Key::Left_ALT, event.GetWindow()) && event.GetRepeatCount() < 1)
	{
		if (event.GetWindow()->GetDisplayMode() == Window::DisplayMode::Windowed ||
			event.GetWindow()->GetDisplayMode() == Window::DisplayMode::Borderless)
			event.GetWindow()->SetDisplayMode(Window::DisplayMode::Fullscreen, 0, 0, 0);
		else if (event.GetWindow()->GetDisplayMode() == Window::DisplayMode::Fullscreen)
			event.GetWindow()->SetDisplayMode(Window::DisplayMode::Windowed, 0, 0, 0);

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

TRAP::Utils::TimeStep TRAP::Application::UpdateNewFrameTimeData(const Utils::Timer& time, float& lastFrameTime,
                                                                float& tickTimerSeconds, const float timeScale)
{
	const float elapsedTime = time.Elapsed();

	const Utils::TimeStep deltaTime{ (elapsedTime - lastFrameTime) * timeScale };

	tickTimerSeconds += elapsedTime - lastFrameTime;
	lastFrameTime = elapsedTime;

	return deltaTime;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::LimitFPS(const uint32_t fpsLimit, Utils::Timer& limitTimer)
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

	const std::chrono::duration<float, std::milli> limitMs =
		std::chrono::duration<float, std::milli>(1000.0f / static_cast<float>(fpsLimit) - limitTimer.ElapsedMilliseconds());
	std::this_thread::sleep_for(limitMs); //If this is too inaccurate, resort to using nanosleep
	limitTimer.Reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Application::UnfocusedLimitFPS(const uint32_t fpsLimit, Utils::Timer& limitTimer)
{
	const std::chrono::duration<float, std::milli> limitMs =
		std::chrono::duration<float, std::milli>(1000.0f / static_cast<float>(fpsLimit) - limitTimer.ElapsedMilliseconds());
	std::this_thread::sleep_for(limitMs); //If this is too inaccurate, resort to using nanosleep
	limitTimer.Reset();
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

	bool texture = std::find(Image::SupportedImageFormatSuffixes.begin(),
	                         Image::SupportedImageFormatSuffixes.end(),
							 fEnding) != Image::SupportedImageFormatSuffixes.end();

	bool shader = false;
	if(!texture)
	{
		shader = std::find(Graphics::Shader::SupportedShaderFormatSuffixes.begin(),
	                       Graphics::Shader::SupportedShaderFormatSuffixes.end(),
						   fEnding) != Graphics::Shader::SupportedShaderFormatSuffixes.end();
	}

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

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::Application::GetTRAPConfigPath()
{
#ifdef TRAP_HEADLESS_MODE
	return "engine.cfg";
#else
	return TRAP::FileSystem::GetGameDocumentsFolderPath().value_or("") / "engine.cfg";
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
                                         const uint32_t fpsLimit, const Graphics::RenderAPI renderAPI)
#else
void TRAP::Application::UpdateTRAPConfig(Utils::Config& config, const uint32_t fpsLimit,
                                         const Graphics::RenderAPI renderAPI)
#endif /*TRAP_HEADLESS_MODE*/
{
#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window != nullptr, "Application::UpdateTRAPConfig(); window is nullptr!");

	if(window != nullptr)
	{
		if(window->GetWidth() > 0)
			config.Set("Width", window->GetWidth());
		if(window->GetHeight() > 0)
			config.Set("Height", window->GetHeight());
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
	config.Set("RenderAPI", (renderAPI != Graphics::RenderAPI::NONE) ? renderAPI :
	                                                                   Graphics::RendererAPI::GetRenderAPI());

	if (Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::NONE)
	{
#ifdef TRAP_HEADLESS_MODE
		uint32_t width = 1920;
		uint32_t height = 1080;
		Graphics::RendererAPI::GetRenderer()->GetResolution(width, height);

		config.Set("Width", width);
		config.Set("Height", height);
#endif /*TRAP_HEADLESS_MODE*/

		//GPU UUID
		std::array<uint8_t, 16> GPUUUID{};
		if(Graphics::RendererAPI::GetNewGPU() != std::array<uint8_t, 16>{}) //Only if UUID is not empty
			GPUUUID = Graphics::RendererAPI::GetNewGPU();
		else
			GPUUUID = Graphics::RendererAPI::GetRenderer()->GetCurrentGPUUUID();
		config.Set(Utils::String::ConvertToString(Graphics::RendererAPI::GetRenderAPI()) + "GPU", Utils::UUIDToString(GPUUUID));

		Graphics::AntiAliasing antiAliasing = Graphics::AntiAliasing::Off;
		Graphics::SampleCount antiAliasingSampleCount = Graphics::SampleCount::Two;
		Graphics::RenderCommand::GetAntiAliasing(antiAliasing, antiAliasingSampleCount);
		const Graphics::SampleCount anisotropyLevel = Graphics::RenderCommand::GetAnisotropyLevel();
		config.Set("AntiAliasing", antiAliasing);
		config.Set("AntiAliasingQuality", antiAliasingSampleCount);
		config.Set("AnisotropyLevel", (anisotropyLevel == Graphics::SampleCount::One) ? "Off" : Utils::String::ConvertToString(anisotropyLevel));
		config.Set("RenderScale", Graphics::RenderCommand::GetRenderScale());

		//NVIDIA Reflex
#if !defined(TRAP_HEADLESS_MODE) && defined(NVIDIA_REFLEX_AVAILABLE)
		PCLSTATS_SHUTDOWN();
		config.Set("NVIDIAReflex", Graphics::RenderCommand::GetLatencyMode());
#endif
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

	props.Maximized = config.Get<bool>("Maximized");
	props.Visible = true;
	props.RawMouseInput = config.Get<bool>("RawMouseInput");

	return props;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
TRAP::WindowProps TRAP::Application::LoadWindowProps(const TRAP::Utils::Config& config)
{
	constexpr uint32_t DefaultWindowWidth = 1280;
	constexpr uint32_t DefaultWindowHeight = 720;
	constexpr uint32_t DefaultWindowRefreshRate = 60;

	TRAP::WindowProps props{};

	props.Title = "TRAP™";
	props.Width = DefaultWindowWidth;
	props.Height = DefaultWindowHeight;
	props.RefreshRate = DefaultWindowRefreshRate;
	props.VSync = config.Get<bool>("VSync");
	props.DisplayMode = config.Get<TRAP::Window::DisplayMode>("DisplayMode");
	props.Monitor = config.Get<uint32_t>("Monitor");
	props.Advanced = LoadAdvancedWindowProps(config);

	config.Get("Width", props.Width);
	config.Get("Height", props.Height);
	config.Get("RefreshRate", props.RefreshRate);

	return props;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderAPI TRAP::Application::SelectRenderAPI(const TRAP::Utils::Config& config)
{
	TRAP::Graphics::RenderAPI renderAPI = config.Get<TRAP::Graphics::RenderAPI>("RenderAPI");

#ifdef TRAP_HEADLESS_MODE
	bool enableGPU = true;
	config.Get("EnableGPU", enableGPU);

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

	const TRAP::Graphics::SampleCount antiAliasingSampleCount = config.Get<TRAP::Graphics::SampleCount>("AntiAliasingQuality");
	TRAP::Graphics::RenderCommand::SetAntiAliasing(config.Get<TRAP::Graphics::AntiAliasing>("AntiAliasing"),
	                                               antiAliasingSampleCount != TRAP::Graphics::SampleCount::One ?
												   antiAliasingSampleCount : TRAP::Graphics::SampleCount::Two);

	std::string anisotropyLevelStr = "16";
	config.Get<std::string>("AnisotropyLevel", anisotropyLevelStr);
	if(!anisotropyLevelStr.empty())
	{
		const TRAP::Graphics::SampleCount anisotropyLevel = (anisotropyLevelStr == "Off") ?
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
	{
		window->SetEventCallback([](TRAP::Events::Event& event) { s_Instance->OnEvent(event); });

		//Update Window Title (Debug/RelWithDebInfo)
		if(TRAP::Graphics::RendererAPI::GetRenderAPI() != TRAP::Graphics::RenderAPI::NONE)
			window->SetTitle(window->GetTitle() + TRAP::Graphics::RendererAPI::GetRenderer()->GetTitle());
	}

	return window;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Application::CreateMainViewport(const TRAP::Utils::Config& config)
{
	uint32_t width = 1920;
	uint32_t height = 1080;

	config.Get<uint32_t>("Width", width);
	config.Get<uint32_t>("Height", height);

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
	Graphics::RenderCommand::SetLatencyMode(config.Get<Graphics::LatencyMode>("NVIDIAReflex"));
	PCLSTATS_INIT(0);
#endif /*!TRAP_HEADLESS_MODE && NVIDIA_REFLEX_AVAILABLE*/

	float renderScale = 1.0f;
	config.Get<float>("RenderScale", renderScale);
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
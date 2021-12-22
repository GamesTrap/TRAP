#include "TRAPEditorLayer.h"

#include <Scene/SceneSerializer.h>

TRAPEditorLayer::TRAPEditorLayer()
	: Layer("TRAPEditorLayer"),
	  m_viewportSize(),
	  m_viewportFocused(false),
	  m_viewportHovered(false)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnImGuiRender()
{
	static bool dockspaceOpen = true;
	static bool optFullscreenPersistent = true;
	const bool optFullscreen = optFullscreenPersistent;
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

	//We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	//because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if(optFullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	//When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole,
	//so we ask Begin() to not render a background.
	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	//Important: Note that we proceed event if Begin() returns false (aka window is collapsed).
	//This is because we want to keep our DockSpace() active.
	//If a DockSpace() is inactive, all active windows docked into it will lose their parent and become undocked.
	//We cannot preserve the docking relationship between an active window and an inactive docking,
	//otherwise any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("TRAP Editor DockSpace", &dockspaceOpen, windowFlags);
	ImGui::PopStyleVar();

	if (optFullscreen)
		ImGui::PopStyleVar(2);

	//DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	const float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;
	if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		const ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
	}

	style.WindowMinSize.x = minWinSizeX;

	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			//Disabling fullscreen would allow the window to be moved to the front of other windows,
			//which we can not undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", nullptr, &optFullscreenPersistent);

			if(ImGui::MenuItem("New", "Ctrl+N"))
				NewScene();
			if(ImGui::MenuItem("Open...", "Ctrl+O"))
				OpenScene();
			if(ImGui::MenuItem("Save", "Ctrl+S"))
				SaveScene();
			if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				SaveSceneAs();

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
				TRAP::Application::Shutdown();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	m_sceneGraphPanel.OnImGuiRender();

	ImGui::Begin("Settings");
	ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
	ImGui::Text("GPU: %s", TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGPUName().c_str());
	ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
	ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
	ImGui::Separator();
	const TRAP::Graphics::Renderer2D::Statistics stats = TRAP::Graphics::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("DrawCalls: %u", stats.DrawCalls);
	ImGui::Text("Quads: %u", stats.QuadCount);
	ImGui::Text("Vertices: %u", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %u", stats.GetTotalIndexCount());

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
	ImGui::Begin("Viewport");

	m_viewportFocused = ImGui::IsWindowFocused();
	m_viewportHovered = ImGui::IsWindowHovered();
	TRAP::Application::GetImGuiLayer().BlockEvents(!m_viewportFocused || !m_viewportHovered);

	const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	ImGui::Image(m_renderTarget->GetTexture().get(), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0.0f, 0.0f }, ImVec2{ 1.0f, 1.0f });
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnAttach()
{
	//THIS IS THE TRAP EDITOR!!!11!!1!
	TRAP::Application::GetWindow()->SetTitle("TRAP Editor");

	//Enable Developer features
	TRAP::Application::SetHotReloading(true);

	//Setup Viewport FrameBuffer
	TRAP::Graphics::RendererAPI::RenderTargetDesc desc{};
    m_renderTargetDesc.Width = 1280;
    m_renderTargetDesc.Height = 720;
    m_renderTargetDesc.Depth = 1;
    m_renderTargetDesc.ArraySize = 1;
    m_renderTargetDesc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture;
    m_renderTargetDesc.Format = TRAP::Graphics::API::ImageFormat::B8G8R8A8_UNORM;
    m_renderTargetDesc.StartState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
    m_renderTargetDesc.Name = "Viewport Framebuffer";
	m_renderTarget = TRAP::Graphics::RenderTarget::Create(m_renderTargetDesc);

	m_renderTargetLoadActions.ClearColorValues[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_renderTargetLoadActions.LoadActionsColor[0] = TRAP::Graphics::RendererAPI::LoadActionType::Clear;

	m_activeScene = TRAP::MakeRef<TRAP::Scene>();

#if 0
	//Entity
	auto square = m_activeScene->CreateEntity("Square Entity");
	square.AddComponent<TRAP::SpriteRendererComponent>(TRAP::Math::Vec4{0.0f, 1.0f, 0.0f, 1.0f});

	m_squareEntity = square;

	m_cameraEntity = m_activeScene->CreateEntity("Camera Entity");
	m_cameraEntity.AddComponent<TRAP::CameraComponent>();

	class CameraController : public TRAP::ScriptableEntity
	{
	public:
		void OnCreate() override
		{
			auto& pos = GetComponent<TRAP::TransformComponent>().Position;
			pos.x = TRAP::Utils::Random::Get(-5.0f, 5.0f);
		}

		void OnDestroy() override
		{}

		void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override
		{
			auto& pos = GetComponent<TRAP::TransformComponent>().Position;
			const float speed = 5.0f;

			if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::W))
				pos.y += speed * deltaTime;
			if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::A))
				pos.x -= speed * deltaTime;
			if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::S))
				pos.y -= speed * deltaTime;
			if (TRAP::Input::IsKeyPressed(TRAP::Input::Key::D))
				pos.x += speed * deltaTime;
		}

		void OnTick() override
		{}
	};

	m_cameraEntity.AddComponent<TRAP::NativeScriptComponent>().Bind<CameraController>();
#endif
	m_sceneGraphPanel.SetContext(m_activeScene);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnDetach()
{
	//m_frameBuffer.reset();
	m_activeScene.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Resize Viewport
	if (m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f && //Zero sized framebuffer is invalid
		(m_renderTarget->GetWidth() != static_cast<uint32_t>(m_viewportSize.x) ||
		 m_renderTarget->GetHeight() != static_cast<uint32_t>(m_viewportSize.y)))
	{
		m_renderTargetDesc.Width = static_cast<uint32_t>(m_viewportSize.x);
		m_renderTargetDesc.Height = static_cast<uint32_t>(m_viewportSize.y);
		m_renderTarget = TRAP::Graphics::RenderTarget::Create(m_renderTargetDesc);

		m_activeScene->OnViewportResize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));
	}

	TRAP::Graphics::Renderer2D::ResetStats();

	//Stop RenderPass (necessary for transition)
	TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

	TRAP::Graphics::RendererAPI::RenderTargetBarrier barrier{};
	barrier.RenderTarget = m_renderTarget;
	barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
	barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
	TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);

	//Framebuffer bind
	TRAP::Graphics::RenderCommand::BindRenderTarget(m_renderTarget, nullptr, &m_renderTargetLoadActions);
	TRAP::Graphics::RenderCommand::SetViewport(0, 0, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());
	TRAP::Graphics::RenderCommand::SetScissor(0, 0, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());

	//Update Scene
	m_activeScene->OnUpdate(deltaTime);

	//Stop RenderPass (necessary for transition)
	TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

	//Transition from RenderTarget to ShaderResource
	barrier.RenderTarget = m_renderTarget;
	barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
	barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
	TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnTick()
{
	//Update Scene
	m_activeScene->OnTick();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) {return OnKeyPress(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAPEditorLayer::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
	//Shortcuts
	if (event.GetRepeatCount() > 0)
		return false;

	const bool ctrlPressed = TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Control) || TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Control);
	const bool shiftPressed = TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Shift) || TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Shift);
	switch(event.GetKey())
	{
	case TRAP::Input::Key::N:
	{
		if (ctrlPressed)
			NewScene();

		break;
	}

	case TRAP::Input::Key::O:
	{
		if (ctrlPressed)
			OpenScene();

		break;
	}

	case TRAP::Input::Key::S:
	{
		if (ctrlPressed && shiftPressed)
			SaveSceneAs();
		else if (shiftPressed)
			SaveScene();

		break;
	}

	default:
		break;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::NewScene()
{
	m_activeScene = TRAP::MakeRef<TRAP::Scene>();
	m_activeScene->OnViewportResize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));
	m_sceneGraphPanel.SetContext(m_activeScene);

	m_lastScenePath = "";
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OpenScene()
{
	const std::string path = TRAP::Utils::Dialogs::OpenSingleFile("TRAP Scene", m_lastScenePath.empty() ? "" : m_lastScenePath, { {"TRAP Scene", "*.TRAPScene;*.TPScene"} });
	if (!path.empty())
	{
		m_lastScenePath = path;

		m_activeScene = TRAP::MakeRef<TRAP::Scene>();
		m_activeScene->OnViewportResize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));
		m_sceneGraphPanel.SetContext(m_activeScene);

		TRAP::SceneSerializer serializer(m_activeScene);
		serializer.Deserialize(m_lastScenePath);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::SaveScene()
{
	std::string path;
	if (m_lastScenePath.empty())
		path = TRAP::Utils::Dialogs::SaveFile("TRAP Scene", "MyScene.TRAPScene", { {"TRAP Scene", "*.TRAPScene;*.TPScene"} });

	if (!path.empty())
		m_lastScenePath = path;

	TRAP::SceneSerializer serializer(m_activeScene);
	serializer.Serialize(m_lastScenePath);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::SaveSceneAs()
{
	const std::string path = TRAP::Utils::Dialogs::SaveFile("TRAP Scene", m_lastScenePath.empty() ? "MyScene.TRAPScene" : m_lastScenePath, { {"TRAP Scene", "*.TRAPScene;*.TPScene"} });

	if (!path.empty())
	{
		m_lastScenePath = path;

		TRAP::SceneSerializer serializer(m_activeScene);
		serializer.Serialize(m_lastScenePath);
	}
}
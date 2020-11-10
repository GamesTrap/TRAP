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

			if(ImGui::MenuItem("Serialize"))
			{
				TRAP::SceneSerializer serializer(m_activeScene);
				serializer.Serialize("Test.TRAP");
			}

			if (ImGui::MenuItem("Deserialize"))
			{
				TRAP::SceneSerializer serializer(m_activeScene);
				serializer.Deserialize("Test.TRAP");
			}
			
			if (ImGui::MenuItem("Exit"))
				TRAP::Application::Shutdown();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	m_sceneGraphPanel.OnImGuiRender();
	
	ImGui::Begin("Settings");
	ImGui::Text("CPU: %ix %s", TRAP::Application::GetCPUInfo().LogicalCores, TRAP::Application::GetCPUInfo().Model.c_str());
	ImGui::Text("GPU: %s", TRAP::Graphics::API::RendererAPI::GetRenderer()->GetCurrentGPUName().c_str());
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
	
	const uint64_t textureID = m_frameBuffer->GetColorAttachmentRendererID();
	ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f });
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnAttach()
{
	//THIS IS THE TRAP EDITOR!!!11!!1!
	TRAP::Application::GetWindow()->SetTitle("TRAP Editor");

	//Currently do not allow to use VulkanAPI as it isn't finished yet!
	if (!TRAP::Graphics::API::Context::IsSupported(TRAP::Graphics::API::RenderAPI::OpenGL))
	{
		TP_CRITICAL("[TRAP Editor] TRAP Editor needs OpenGL 4.6 to run!");
		TRAP::Utils::Dialogs::MsgBox::Show("[TRAP Editor] TRAP Editor needs OpenGL 4.6 or newer!",
			"[TRAP Editor] Invalid RenderAPI",
			TRAP::Utils::Dialogs::MsgBox::Style::Error,
			TRAP::Utils::Dialogs::MsgBox::Buttons::Quit);
	}

	//If OpenGL 4.6 is supported switch to it
	if (TRAP::Graphics::API::Context::GetRenderAPI() == TRAP::Graphics::API::RenderAPI::Vulkan)
		TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::OpenGL);

	//Enable Developer features
	TRAP::Application::SetHotShaderReloading(true);
	TRAP::Application::SetHotTextureReloading(true);

	//Setup Viewport FrameBuffer
	const TRAP::Graphics::FrameBufferProps frameBufferProps{ 1280, 720, 1, false };
	m_frameBuffer = TRAP::Graphics::FrameBuffer::Create(frameBufferProps);

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

		void OnUpdate(const TRAP::Utils::TimeStep deltaTime) override
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
	m_frameBuffer.reset();
	m_activeScene.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Resize Viewport
	if (const TRAP::Graphics::FrameBufferProps props = m_frameBuffer->GetProps();
		m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f && //Zero sized framebuffer is invalid
		(props.Width != static_cast<uint32_t>(m_viewportSize.x) || props.Height != static_cast<uint32_t>(m_viewportSize.y)))
	{
		m_frameBuffer->Resize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));

		m_activeScene->OnViewportResize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));
	}
	
	TRAP::Graphics::Renderer2D::ResetStats();
	//Framebuffer
	m_frameBuffer->Bind();
	//Setup
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);
	
	//Update Scene
	m_activeScene->OnUpdate(deltaTime);

	m_frameBuffer->Unbind();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnTick()
{
	//Update Scene
	m_activeScene->OnTick();
}
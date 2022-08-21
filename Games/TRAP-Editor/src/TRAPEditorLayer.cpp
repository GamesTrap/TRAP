#include "TRAPEditorLayer.h"

#include "Application.h"
#include "Graphics/Cameras/Editor/EditorCamera.h"
#include "Graphics/RenderCommand.h"
#include "Input/Input.h"
#include <Scene/SceneSerializer.h>

#include <ImGuizmo.h>
#include <imgui_internal.h>

TRAPEditorLayer::TRAPEditorLayer()
	: Layer("TRAPEditorLayer"),
	  m_viewportSize(),
	  m_viewportFocused(false),
	  m_viewportHovered(false),
	  m_editorCamera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f),
	  m_gizmoType(-1), m_allowViewportCameraEvents(false), m_startedCameraMovement(false)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnImGuiRender()
{
	static bool dockspaceOpen = true;
	static bool optFullscreenPersistent = true;
	const bool optFullscreen = optFullscreenPersistent;
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

	if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
	   (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !m_startedCameraMovement))
	{
		ImGui::FocusWindow(GImGui->HoveredWindow);
		if(TRAP::Application::GetWindow()->GetCursorMode() != TRAP::Window::CursorMode::Normal)
			TRAP::Application::GetWindow()->SetCursorMode(TRAP::Window::CursorMode::Normal);
	}

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

	//Important: Note that we proceed even if Begin() returns false (aka window is collapsed).
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

#if 0
	ImGui::Separator();

	ImGui::Text("Editor Camera Debug:");
	ImGui::Text("Distance: %f", m_editorCamera.GetDistance());
	ImGui::Text("Focal Point: %f, %f, %f", m_editorCamera.GetFocalPoint().x, m_editorCamera.GetFocalPoint().y, m_editorCamera.GetFocalPoint().z);
	ImGui::Text("Up Dir: %f, %f, %f", m_editorCamera.GetUpDirection().x, m_editorCamera.GetUpDirection().y, m_editorCamera.GetUpDirection().z);
	ImGui::Text("Strafe Dir: %f, %f, %f", m_editorCamera.GetRightDirection().x, m_editorCamera.GetRightDirection().y, m_editorCamera.GetRightDirection().z);
	ImGui::Text("Yaw: %f", m_editorCamera.GetYaw());
	ImGui::Text("Yaw Delta: %f", m_editorCamera.m_yawDelta);
	ImGui::Text("Pitch: %f", m_editorCamera.GetPitch());
	ImGui::Text("Pitch Delta: %f", m_editorCamera.m_pitchDelta);
	ImGui::Text("Position: (%f, %f, %f)", m_editorCamera.GetPosition().x, m_editorCamera.GetPosition().y, m_editorCamera.GetPosition().z);
	ImGui::Text("Position Delta: (%f, %f, %f)", m_editorCamera.m_positionDelta.x, m_editorCamera.m_positionDelta.y, m_editorCamera.m_positionDelta.z);
	const TRAP::Math::Mat4 m = m_editorCamera.GetViewMatrix();
	ImGui::Text("View matrix: [%f, %f, %f, %f]",  m[0].x, m[0].y, m[0].z, m[0].w);
	ImGui::Text("		       [%f, %f, %f, %f]",  m[1].x, m[1].y, m[1].z, m[1].w);
	ImGui::Text("		       [%f, %f, %f, %f]",  m[2].x, m[2].y, m[2].z, m[2].w);
	ImGui::Text("		       [%f, %f, %f, %f]",  m[3].x, m[3].y, m[3].z, m[3].w);
#endif

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
	ImGui::Begin("Viewport");

	m_viewportFocused = ImGui::IsWindowFocused();
	m_viewportHovered = ImGui::IsWindowHovered();
	TRAP::Application::GetImGuiLayer().BlockEvents(m_viewportHovered && (TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Right) ||
	                                                                           TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Middle)));

	const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	ImGui::Image(m_renderTarget->GetTexture(), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0.0f, 0.0f }, ImVec2{ 1.0f, 1.0f });

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 minBound = ImGui::GetWindowPos();

	ImVec2 maxBound = {minBound.x + windowSize.x, minBound.y + windowSize.y};
	m_allowViewportCameraEvents = (ImGui::IsMouseHoveringRect(minBound, maxBound) && m_viewportFocused) || m_startedCameraMovement;

	//Gizmos
	TRAP::Entity selectedEntity = m_sceneGraphPanel.GetSelectedEntity(); //TODO Mouse picking (currently only works via selection in the scene graph)
	if(selectedEntity && m_gizmoType != -1 &&
	   !TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Right) &&
	   !TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Middle))
	{
		//Camera
		// auto cameraEntity = m_activeScene->GetPrimaryCameraEntity(); //Run time camera
		// if(cameraEntity)
		// {
			// const auto& camera = cameraEntity.GetComponent<TRAP::CameraComponent>().Camera;
			// const TRAP::Math::Mat4& cameraProj = camera.GetProjectionMatrix();
			// TRAP::Math::Mat4 cameraView = TRAP::Math::Inverse(cameraEntity.GetComponent<TRAP::TransformComponent>().GetTransform());
			const TRAP::Math::Mat4& cameraProj = m_editorCamera.GetProjectionMatrix();
			TRAP::Math::Mat4 cameraView = m_editorCamera.GetViewMatrix();

			//Entity transform
			auto& tc = selectedEntity.GetComponent<TRAP::TransformComponent>();
			TRAP::Math::Mat4 transform = tc.GetTransform();

			// ImGuizmo::SetOrthographic(camera.GetProjectionType() == TRAP::SceneCamera::ProjectionType::Orthographic); //TODO 2D mode
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			const float windowWidth = static_cast<float>(ImGui::GetWindowWidth());
			const float windowHeight = static_cast<float>(ImGui::GetWindowHeight());
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			//Snapping
			const bool snap = TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Control) ||
			                  TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Control);
			float snapValue = 0.5f;
			if(m_gizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			std::array<float, 3> snapValues = {snapValue, snapValue, snapValue};

			ImGuizmo::Manipulate(&cameraView[0].x, &cameraProj[0].x,
			                     static_cast<ImGuizmo::OPERATION>(m_gizmoType), ImGuizmo::LOCAL,
								 &transform[0].x, nullptr, snap ? snapValues.data() : nullptr);

			if(ImGuizmo::IsUsing())
			{
				TRAP::Math::Vec3 position, rotation, scale;
				TRAP::Math::Decompose(transform, position, rotation, scale);
				{
					const TRAP::Math::Vec3 deltaRotation = rotation - tc.Rotation;
					tc.Position = position;
					tc.Rotation += deltaRotation;
					tc.Scale = scale;
				}
			}
		// }
	}

	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnAttach()
{
	//THIS IS THE TRAP EDITOR!!!11!!1!
	TRAP::Application::GetWindow()->SetTitle("TRAP™ Editor");

	//Set Discord stuff
	TRAP::Utils::Discord::SetActivity({"trapwhitelogo2048x2048", "TRAP™ Editor", "TRAP™ Editor", "Developed by TrappedGames"});

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

	m_editorCamera = TRAP::Graphics::EditorCamera(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

	m_sceneGraphPanel.SetContext(m_activeScene);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnDetach()
{
	m_renderTarget.reset();
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

		m_editorCamera.SetViewportSize(m_viewportSize.x, m_viewportSize.y);

		m_activeScene->OnViewportResize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));
	}

	m_editorCamera.SetActive(m_allowViewportCameraEvents);
	m_editorCamera.OnUpdate(deltaTime);

	if((TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Right) ||
		TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Middle)) &&
	   !m_startedCameraMovement && m_viewportFocused && m_viewportHovered)
	{
		m_startedCameraMovement = true;
	}

	if(!TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Right) &&
	   !TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Middle))
		m_startedCameraMovement = false;

	TRAP::Graphics::Renderer2D::ResetStats();

	//Stop RenderPass (necessary for transition)
	TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

	TRAP::Graphics::RendererAPI::RenderTargetBarrier barrier{};
	barrier.RenderTarget = m_renderTarget;
	barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource;
	barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
	TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);

	//Framebuffer bind
	TRAP::Graphics::RenderCommand::BindRenderTarget(m_renderTarget, nullptr, &m_renderTargetLoadActions);
	TRAP::Graphics::RenderCommand::SetViewport(0, 0, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());
	TRAP::Graphics::RenderCommand::SetScissor(0, 0, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());

	//Update Scene
	m_activeScene->OnUpdateEditor(deltaTime, m_editorCamera);

	//Stop RenderPass (necessary for transition)
	TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

	//Transition from RenderTarget to PixelShaderResource
	barrier.RenderTarget = m_renderTarget;
	barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
	barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource;
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
	if(m_allowViewportCameraEvents)
		m_editorCamera.OnEvent(event);

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

	//TODO Make these shortcuts customizable
	if(!TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Right) && !ImGuizmo::IsUsing())
	{
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

		//Gizmos
		case TRAP::Input::Key::Q:
		{
				m_gizmoType = -1;
			break;
		}
		case TRAP::Input::Key::W:
		{
			m_gizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case TRAP::Input::Key::E:
		{
			m_gizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		case TRAP::Input::Key::R:
		{
			m_gizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

		default:
			break;
		}
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
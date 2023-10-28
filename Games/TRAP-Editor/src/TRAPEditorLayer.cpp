#include "TRAPEditorLayer.h"

#include <imgui_internal.h>

TRAPEditorLayer::TRAPEditorLayer()
	: Layer("TRAPEditorLayer")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnImGuiRender()
{
	constinit static bool dockspaceOpen = true;
	constinit static bool optFullscreenPersistent = true;
	const bool optFullscreen = optFullscreenPersistent;
	constinit static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

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
		const ImGuiViewport* const viewport = ImGui::GetMainViewport();
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
	if ((dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) != 0)
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
	const f32 minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;
	if((io.ConfigFlags & ImGuiConfigFlags_DockingEnable) != 0)
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
	ImGui::Text("GPU: %s", TRAP::Graphics::RenderCommand::GetGPUName().c_str());
    ImGui::Text("CPU FPS: %u", TRAP::Graphics::RenderCommand::GetCPUFPS());
    ImGui::Text("GPU FPS: %u", TRAP::Graphics::RenderCommand::GetGPUFPS());
    ImGui::Text("CPU FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetCPUFrameTime());
    ImGui::Text("GPU Graphics FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime());
    ImGui::Text("GPU Compute FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime());
	ImGui::Separator();
	const TRAP::Graphics::Renderer2D::Statistics stats = TRAP::Graphics::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("DrawCalls: %u", stats.DrawCalls);
	ImGui::Text("Quads: %u", stats.QuadCount);
	ImGui::Text("Circles: %u", stats.CircleCount);
	ImGui::Text("Lines: %u", stats.LineCount);
	ImGui::Text("Vertices: %u", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %u", stats.GetTotalIndexCount());
	ImGui::Separator();
	ImGui::Checkbox("Show physics colliders", &m_showPhysicsColliders);

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
	ImGui::Begin("Viewport");

	const ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
	const ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	const ImVec2 viewportOffset = ImGui::GetWindowPos(); //Includes menu bar
	std::get<0>(m_viewportBounds) = {viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
	std::get<1>(m_viewportBounds) = {viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

	m_viewportFocused = ImGui::IsWindowFocused();
	m_viewportHovered = ImGui::IsWindowHovered();
	TRAP::Application::GetImGuiLayer().BlockEvents(m_viewportHovered && (TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Right) ||
	                                                                           TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Middle)));

	const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	ImGui::Image(m_renderTarget->GetTexture(), ImVec2{ m_viewportSize.x(), m_viewportSize.y() }, ImVec2{ 0.0f, 0.0f }, ImVec2{ 1.0f, 1.0f });

	m_allowViewportCameraEvents = (ImGui::IsMouseHoveringRect(ImVec2(std::get<0>(m_viewportBounds).x(), std::get<0>(m_viewportBounds).y()),
	                                                          ImVec2(std::get<1>(m_viewportBounds).x(), std::get<1>(m_viewportBounds).y())) && m_viewportFocused) || m_startedCameraMovement;

	//Gizmos
	TRAP::Entity selectedEntity = m_sceneGraphPanel.GetSelectedEntity();
	if(selectedEntity && m_enableGizmo)
	{
		//Camera
		auto cameraEntity = m_activeScene->GetPrimaryCameraEntity(); //Run time camera
		TRAP::Math::Mat4 cameraProj{};
		TRAP::Math::Mat4 cameraView{};
		if(m_sceneState == SceneState::Edit || !cameraEntity)
		{
			cameraProj = m_editorCamera.GetProjectionMatrix();
			cameraView = m_editorCamera.GetViewMatrix();
		}
		else if(m_sceneState == SceneState::Play)
		{
			const auto& camera = cameraEntity.GetComponent<TRAP::CameraComponent>().Camera;
			cameraProj = camera.GetProjectionMatrix();
			cameraView = TRAP::Math::Inverse(cameraEntity.GetComponent<TRAP::TransformComponent>().GetTransform());
		}

		//Entity transform
		auto& tc = selectedEntity.GetComponent<TRAP::TransformComponent>();
		TRAP::Math::Mat4 transform = tc.GetTransform();

		// ImGuizmo::SetOrthographic(camera.GetProjectionType() == TRAP::SceneCamera::ProjectionType::Orthographic); //TODO 2D mode
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(std::get<0>(m_viewportBounds).x(), std::get<0>(m_viewportBounds).y(),
							std::get<1>(m_viewportBounds).x() - std::get<0>(m_viewportBounds).x(),
							std::get<1>(m_viewportBounds).y() - std::get<0>(m_viewportBounds).y());

		//Snapping
		const bool snap = TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Control) ||
							TRAP::Input::IsKeyPressed(TRAP::Input::Key::Right_Control);
		f32 snapValue = 0.5f;
		if(m_gizmoType == ImGuizmo::OPERATION::ROTATE)
			snapValue = 45.0f;

		const std::array<f32, 3> snapValues = {snapValue, snapValue, snapValue};

		//Disable gizmo while entity was just changed and the left mouse button is still pressed
		ImGuizmo::Enable(!m_entityChanged || m_leftMouseBtnRepeatCount == 0);

		const bool manipulated = ImGuizmo::Manipulate(&std::get<0>(cameraView).x(), &std::get<0>(cameraProj).x(),
														static_cast<ImGuizmo::OPERATION>(m_gizmoType),
														ImGuizmo::LOCAL, &std::get<0>(transform).x(),
														nullptr, snap ? snapValues.data() : nullptr);

		if (manipulated &&
			!TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Right) &&
			!TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Middle))
		{
			TRAP::Math::Vec3 position{}, rotation{}, scale{};
			if(TRAP::Math::Decompose(transform, position, rotation, scale))
			{
				const TRAP::Math::Vec3 deltaRotation = rotation - tc.Rotation;
				tc.Position = position;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();

	UIToolbar();

	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnAttach()
{
	//THIS IS THE TRAP EDITOR!!!11!!1!
	TRAP::Application::GetWindow()->SetTitle("TRAP™ Editor");

	//Copy Default editor layout (imgui.ini) if it doesn't exist
	auto gameDocsFolder = TRAP::FileSystem::GetGameDocumentsFolderPath();
	std::filesystem::path imguiIniPath;
	if(gameDocsFolder)
	{
		imguiIniPath = *gameDocsFolder / "imgui.ini";
		if(!TRAP::FileSystem::Exists(imguiIniPath))
		{
			if(TRAP::FileSystem::Copy("./Resources/Layouts/Default.ini", imguiIniPath))
				ImGui::LoadIniSettingsFromDisk(imguiIniPath.string().c_str());
		}
	}

	//Set Discord stuff
	TRAP::Utils::Discord::SetActivity({"trapwhitelogo2048x2048", "TRAP™ Editor", "TRAP™ Editor", "Developed by TrappedGames"});

	//Enable Developer features
	TRAP::Application::SetHotReloading(true);

	m_iconPlay = TRAP::Graphics::Texture::CreateFromFile("./Resources/Icons/Play.png", TRAP::Graphics::TextureType::Texture2D);
	m_iconStop = TRAP::Graphics::Texture::CreateFromFile("./Resources/Icons/Stop.png", TRAP::Graphics::TextureType::Texture2D);

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
    m_renderTargetDesc.Name = "Viewport ID Framebuffer";
	m_renderTargetDesc.Format = TRAP::Graphics::API::ImageFormat::R32_SINT;
	m_IDRenderTarget = TRAP::Graphics::RenderTarget::Create(m_renderTargetDesc);

	std::get<0>(m_renderTargetLoadActions.ClearColorValues) = { 0.1, 0.1, 0.1, 1.0 };
	std::get<0>(m_renderTargetLoadActions.LoadActionsColor) = TRAP::Graphics::RendererAPI::LoadActionType::Clear;
	std::get<1>(m_renderTargetLoadActions.ClearColorValues) = {-1.0, 0.0, 0.0, 0.0};
	std::get<1>(m_renderTargetLoadActions.LoadActionsColor) = TRAP::Graphics::RendererAPI::LoadActionType::Clear;

	//Setup Mouse Picking Buffer
	m_mousePickBufferDesc.Flags = TRAP::Graphics::RendererAPI::BufferCreationFlags::PersistentMap |
	                              TRAP::Graphics::RendererAPI::BufferCreationFlags::NoDescriptorViewCreation;
	m_mousePickBufferDesc.Size = NumericCast<u64>(m_renderTargetDesc.Width) * m_renderTargetDesc.Height * sizeof(i32);
	m_mousePickBufferDesc.QueueType = TRAP::Graphics::RendererAPI::QueueType::Graphics;
	m_mousePickBufferDesc.MemoryUsage = TRAP::Graphics::RendererAPI::ResourceMemoryUsage::GPUToCPU;
	m_mousePickBufferDesc.StartState = TRAP::Graphics::RendererAPI::ResourceState::CopyDestination;
	m_mousePickBufferDesc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::RWBuffer;
	m_mousePickBufferDesc.Name = "Viewport ID Buffer";
	m_mousePickBuffer = TRAP::Graphics::Buffer::Create(m_mousePickBufferDesc);

	m_editorScene = TRAP::MakeRef<TRAP::Scene>();
	m_activeScene = m_editorScene;
	m_sceneGraphPanel.SetContext(m_activeScene);

	m_editorCamera = TRAP::Graphics::EditorCamera(30.0f, 16.0f / 9.0f, 0.1f);

	TRAP::Graphics::RenderCommand::SetCullMode(TRAP::Graphics::CullMode::None); //Culling
		TRAP::Graphics::RenderCommand::SetBlendConstant(TRAP::Graphics::BlendConstant::SrcAlpha,
														TRAP::Graphics::BlendConstant::One,
														TRAP::Graphics::BlendConstant::OneMinusSrcAlpha,
														TRAP::Graphics::BlendConstant::OneMinusSrcAlpha); //Blending

	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Resize Viewport
	if (m_viewportSize.x() > 0.0f && m_viewportSize.y() > 0.0f && //Zero sized framebuffer is invalid
		(m_renderTarget->GetWidth() != NumericCast<u32>(m_viewportSize.x()) ||
		 m_renderTarget->GetHeight() != NumericCast<u32>(m_viewportSize.y())))
	{
		//Update RenderTargets
		m_renderTargetDesc.Width = NumericCast<u32>(m_viewportSize.x());
		m_renderTargetDesc.Height = NumericCast<u32>(m_viewportSize.y());
    	m_renderTargetDesc.Format = TRAP::Graphics::API::ImageFormat::B8G8R8A8_UNORM;
    	m_renderTargetDesc.Name = "Viewport Framebuffer";
		m_renderTarget = TRAP::Graphics::RenderTarget::Create(m_renderTargetDesc);
    	m_renderTargetDesc.Name = "Viewport ID Framebuffer";
    	m_renderTargetDesc.Format = TRAP::Graphics::API::ImageFormat::R32_SINT;
		m_IDRenderTarget = TRAP::Graphics::RenderTarget::Create(m_renderTargetDesc);

		//Update Mouse Picking Buffer
		m_mousePickBufferDesc.Size = NumericCast<u64>(m_renderTargetDesc.Width) * m_renderTargetDesc.Height * sizeof(i32);
		m_mousePickBuffer = TRAP::Graphics::Buffer::Create(m_mousePickBufferDesc);

		m_editorCamera.SetViewportSize(m_viewportSize.x(), m_viewportSize.y());

		m_activeScene->OnViewportResize(NumericCast<u32>(m_viewportSize.x()), NumericCast<u32>(m_viewportSize.y()));
	}

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
	TRAP::Graphics::RendererAPI::RenderTargetBarrier IDBarrier = barrier;
	IDBarrier.RenderTarget = m_IDRenderTarget;
	TRAP::Graphics::RenderCommand::RenderTargetBarriers({barrier, IDBarrier});

	//Framebuffer bind
	TRAP::Graphics::RenderCommand::BindRenderTargets({ m_renderTarget, m_IDRenderTarget }, nullptr, &m_renderTargetLoadActions);
	TRAP::Graphics::RenderCommand::SetViewport(0, 0, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());
	TRAP::Graphics::RenderCommand::SetScissor(0, 0, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());

	//Update Scene
	switch(m_sceneState)
	{
	case SceneState::Edit:
	{
		m_editorCamera.SetActive(m_allowViewportCameraEvents);
		m_editorCamera.OnUpdate(deltaTime);

		m_activeScene->OnUpdateEditor(deltaTime, m_editorCamera);
		break;
	}

	case SceneState::Play:
	{
		m_activeScene->OnUpdateRuntime(deltaTime);
		break;
	}

	// case SceneState::Pause:
	{
		// break;
	}

	// case SceneState::Simulate:
	{
		// break;
	}

	default:
		break;
	}

	OnOverlayRender();

	//Stop RenderPass (necessary for transition)
	TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

	//Transition from RenderTarget to PixelShaderResource
	barrier.RenderTarget = m_renderTarget;
	barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
	barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource;
	IDBarrier = barrier;
	IDBarrier.RenderTarget = m_IDRenderTarget;
	TRAP::Graphics::RenderCommand::RenderTargetBarriers({barrier, IDBarrier});

	MousePicking();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnTick(const TRAP::Utils::TimeStep& deltaTime)
{
	//Update Scene
	switch(m_sceneState)
	{
	case SceneState::Edit:
	{
		break;
	}

	case SceneState::Play:
	{
		m_activeScene->OnTick(deltaTime);
		m_activeScene->OnTickRuntime(deltaTime);
		break;
	}

	default:
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnEvent(TRAP::Events::Event& event)
{
	if(m_allowViewportCameraEvents)
		m_editorCamera.OnEvent(event);

	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(this, &TRAPEditorLayer::OnKeyPress);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAPEditorLayer::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	//Shortcuts

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
			else if (ctrlPressed)
				SaveScene();

			break;
		}

		//Entity Shortcuts
		case TRAP::Input::Key::D: //TODO Also duplicate if left clicking on a translation gizmoe while holding shift
		{
			if(ctrlPressed)
				DuplicateEntity();
			break;
		}
		case TRAP::Input::Key::Delete:
		{
			DeleteEntity();
			break;
		}

		//Gizmos
		case TRAP::Input::Key::Q:
		{
			m_enableGizmo = false;
			break;
		}
		case TRAP::Input::Key::W:
		{
			m_gizmoType = ImGuizmo::OPERATION::TRANSLATE;
			m_enableGizmo = true;
			break;
		}
		case TRAP::Input::Key::E:
		{
			m_gizmoType = ImGuizmo::OPERATION::ROTATE;
			m_enableGizmo = true;
			break;
		}
		case TRAP::Input::Key::R:
		{
			m_gizmoType = ImGuizmo::OPERATION::SCALE;
			m_enableGizmo = true;
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
	m_activeScene->OnViewportResize(NumericCast<u32>(m_viewportSize.x()), NumericCast<u32>(m_viewportSize.y()));
	m_sceneGraphPanel.SetContext(m_activeScene);

	m_lastScenePath = "";
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OpenScene()
{
	if(m_sceneState != SceneState::Edit)
		OnSceneStop();

	const std::string path = TRAP::Utils::Dialogs::OpenSingleFile("TRAP Scene", m_lastScenePath.empty() ? "" : m_lastScenePath, { {"TRAP Scene", "*.TRAPScene;*.TPScene"} });
	if (!path.empty())
	{
		m_lastScenePath = path;

		TRAP::Ref<TRAP::Scene> newScene = TRAP::MakeRef<TRAP::Scene>();
		TRAP::SceneSerializer serializer(newScene);
		if(serializer.Deserialize(m_lastScenePath))
		{
			m_editorScene = newScene;
			m_editorScene->OnViewportResize(NumericCast<u32>(m_viewportSize.x()), NumericCast<u32>(m_viewportSize.y()));
			m_sceneGraphPanel.SetContext(m_editorScene);

			m_activeScene = m_editorScene;
		}
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

	SerializeScene(m_activeScene, m_lastScenePath);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::SaveSceneAs()
{
	const std::string path = TRAP::Utils::Dialogs::SaveFile("TRAP Scene", m_lastScenePath.empty() ? "MyScene.TRAPScene" : m_lastScenePath, { {"TRAP Scene", "*.TRAPScene;*.TPScene"} });

	if (!path.empty())
	{
		m_lastScenePath = path;
		SerializeScene(m_activeScene, m_lastScenePath);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnOverlayRender()
{
	static constexpr f32 zIndex = 0.001f;
	f32 colliderProjectionZ = 0.0f;
	if(m_sceneState == SceneState::Play && m_activeScene->GetPrimaryCameraEntity() != TRAP::Entity())
	{
		TRAP::Entity camera = m_activeScene->GetPrimaryCameraEntity();
		const auto& transformComponent = camera.GetComponent<TRAP::TransformComponent>();

		//Get forward direction from camera
		const TRAP::Math::Quat orientation = TRAP::Math::Quat(transformComponent.Rotation);
		const TRAP::Math::Vec3 forwardDirection = orientation * TRAP::Math::ZAxis<f32>();
		colliderProjectionZ = forwardDirection.z() * zIndex;

		TRAP::Graphics::Renderer2D::BeginScene(camera.GetComponent<TRAP::CameraComponent>().Camera,
		                                       camera.GetComponent<TRAP::TransformComponent>().GetTransform());
	}
	else
	{
		colliderProjectionZ = m_editorCamera.GetForwardDirection().z() * zIndex;
		TRAP::Graphics::Renderer2D::BeginScene(m_editorCamera);
	}

	if(m_showPhysicsColliders)
	{
		//Box Collider 2D visualization
		{
			auto view = m_activeScene->GetAllEntitiesWithComponents<TRAP::TransformComponent, TRAP::BoxCollider2DComponent>();
			for(auto entity : view)
			{
				auto [transform, boxCollider] = view.get<TRAP::TransformComponent, TRAP::BoxCollider2DComponent>(entity);

				const TRAP::Math::Vec3 position = transform.Position + TRAP::Math::Vec3(boxCollider.Offset, -colliderProjectionZ);
				const f32 rotation = transform.Rotation.z();
				const TRAP::Math::Vec3 scale = transform.Scale * TRAP::Math::Vec3(boxCollider.Size * 2.0f, 1.0f);

				const TRAP::Math::Mat4 trans = TRAP::Math::Translate(position) *
											TRAP::Math::Rotate(rotation, {0.0f, 0.0f, 1.0f}) *
											TRAP::Math::Scale(scale);

				TRAP::Graphics::Renderer2D::DrawRect(trans, TRAP::Math::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
			}
		}

		//Circle Collider 2D visualization
		{
			auto view = m_activeScene->GetAllEntitiesWithComponents<TRAP::TransformComponent, TRAP::CircleCollider2DComponent>();
			for(auto entity : view)
			{
				auto [transform, circleCollider] = view.get<TRAP::TransformComponent, TRAP::CircleCollider2DComponent>(entity);

				const TRAP::Math::Vec3 position = transform.Position + TRAP::Math::Vec3(circleCollider.Offset, -colliderProjectionZ);
				const TRAP::Math::Vec3 scale = transform.Scale * TRAP::Math::Vec3(circleCollider.Radius * 2.0f);

				const TRAP::Math::Mat4 trans = TRAP::Math::Translate(position) * TRAP::Math::Scale(scale);

				TRAP::Graphics::Renderer2D::DrawCircle(trans, TRAP::Math::Vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.01f);
			}
		}
	}

	TRAP::Graphics::Renderer2D::EndScene();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::MousePicking()
{
	if(!m_viewportHovered)
		return;

	if(TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Left) && m_leftMouseBtnRepeatCount == 0)
	{
		++m_leftMouseBtnRepeatCount;

		//Get mouse position relative to viewport
		ImVec2 mousePos = ImGui::GetMousePos();
		mousePos.x -= std::get<0>(m_viewportBounds).x();
		mousePos.y -= std::get<0>(m_viewportBounds).y();
		const TRAP::Math::Vec2 viewportSize = std::get<1>(m_viewportBounds) - std::get<0>(m_viewportBounds);
		u32 mouseX = NumericCast<u32>(mousePos.x);
		u32 mouseY = NumericCast<u32>(mousePos.y);

		//Out of viewport bounds check (maybe redundant, doesnt hurt)
		if(mouseX >= NumericCast<u32>(viewportSize.x()) || mouseY >= NumericCast<u32>(viewportSize.y()))
			return;

		//Copy the RenderTarget contents to our CPU visible buffer
		TRAP::Graphics::API::SyncToken sync{};
		TRAP::Graphics::RendererAPI::TextureCopyDesc copyDesc{};
		copyDesc.Texture = m_IDRenderTarget->GetTexture();
		copyDesc.Buffer = m_mousePickBuffer;
		copyDesc.TextureState = TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource;
		copyDesc.QueueType = TRAP::Graphics::RendererAPI::QueueType::Graphics;
		TRAP::Graphics::RendererAPI::GetResourceLoader()->CopyResource(copyDesc, &sync);
		TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForToken(&sync);

		//Bring the RenderTarget back into its original state
		TRAP::Graphics::RenderCommand::Transition(m_IDRenderTarget->GetTexture(),
		                                          TRAP::Graphics::RendererAPI::ResourceState::CopySource,
												  TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource);

		//Read pixel of the CPU visible buffer
		TRAP::Graphics::RendererAPI::ReadRange range{0, NumericCast<u64>(m_renderTargetDesc.Width) * m_renderTargetDesc.Height * sizeof(i32)};
		m_mousePickBuffer->MapBuffer(&range);

		//Get our wanted value
		const i32 id = *(static_cast<i32*>(m_mousePickBuffer->GetCPUMappedAddress()) + (mouseY * m_renderTargetDesc.Width + mouseX));
		if(m_activeScene)
		{
			const TRAP::Entity entity{static_cast<entt::entity>(id), m_activeScene.get()};
			if(entity != m_sceneGraphPanel.GetSelectedEntity() && !ImGuizmo::IsUsing())
			{
				if ((!ImGuizmo::IsOver() && entity != TRAP::Entity()) || id != -1)
				{
					m_sceneGraphPanel.SetSelectedEntity(entity);
					m_entityChanged = true;
				}
			}
		}
		TP_TRACE("Mouse = {", mouseX, "}, {", mouseY, "}, ID = {", id, "}");

		m_mousePickBuffer->UnMapBuffer();
	}
	else if(TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Left))
		++m_leftMouseBtnRepeatCount;
	else
	{
		m_leftMouseBtnRepeatCount = 0;
		m_entityChanged = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::DeleteEntity()
{
	if(m_sceneState != SceneState::Edit)
		return;

	TRAP::Entity selectedEntity = m_sceneGraphPanel.GetSelectedEntity();
	if(selectedEntity)
	{
		m_sceneGraphPanel.SetSelectedEntity(TRAP::Entity(entt::null, m_editorScene.get()));
		m_editorScene->DestroyEntity(selectedEntity);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::DuplicateEntity()
{
	if(m_sceneState != SceneState::Edit)
		return;

	TRAP::Entity selectedEntity = m_sceneGraphPanel.GetSelectedEntity();
	if(selectedEntity)
		m_editorScene->DuplicateEntity(selectedEntity);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::SerializeScene(TRAP::Ref<TRAP::Scene> scene, const std::filesystem::path& path)
{
	TRAP::SceneSerializer serializer(std::move(scene));
	serializer.Serialize(path);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::UIToolbar()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	const auto& colors = ImGui::GetStyle().Colors;
	const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
	const auto& buttonActive = colors[ImGuiCol_ButtonActive];
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

	ImGui::Begin("##toolbar", nullptr, /*ImGuiWindowFlags_NoDecoration | */ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		const f32 size = ImGui::GetWindowHeight() - 4.0f;
		TRAP::Ref<TRAP::Graphics::Texture> icon = m_sceneState == SceneState::Edit ? m_iconPlay : m_iconStop;
		ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if(ImGui::ImageButton(icon, ImVec2(size, size), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)))
		{
			if(m_sceneState == SceneState::Edit)
				OnScenePlay();
			else if(m_sceneState == SceneState::Play)
				OnSceneStop();
			// else if(m_sceneState == SceneState::Pause)
			// 	OnScenePause();
			// else if(m_sceneState == SceneState::Simulate)
			// 	OnSceneSimulate();
		}
	}

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(3);

	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnScenePlay()
{
	m_sceneState = SceneState::Play;

	m_activeScene = TRAP::Scene::Copy(m_editorScene);
	m_activeScene->OnRuntimeStart();

	m_sceneGraphPanel.SetContext(m_activeScene);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAPEditorLayer::OnSceneStop()
{
	m_activeScene->OnRuntimeStop();

	m_sceneState = SceneState::Edit;

	m_activeScene = m_editorScene;

	m_sceneGraphPanel.SetContext(m_activeScene);
}

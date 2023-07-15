#include "Sandbox2D.h"
#include "Application.h"
#include "Events/FileEvent.h"
#include "FileSystem/FileWatcher.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"),
	m_wireFrame(false),
	m_cameraController(TRAP::Application::GetWindow()->GetAspectRatio(), true),
	m_frameTimeHistory()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnImGuiRender()
{
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::Begin("Performance", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Performance");
	ImGui::Separator();
    ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
	ImGui::Text("GPU: %s", TRAP::Graphics::RenderCommand::GetGPUName().c_str());
    ImGui::Text("CPU FPS: %u", TRAP::Graphics::RenderCommand::GetCPUFPS());
    ImGui::Text("GPU FPS: %u", TRAP::Graphics::RenderCommand::GetGPUFPS());
    ImGui::Text("CPU FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetCPUFrameTime());
    ImGui::Text("GPU Graphics FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime());
    ImGui::Text("GPU Compute FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime());
	ImGui::PlotLines("", m_frameTimeHistory.data(), NumericCast<int32_t>(m_frameTimeHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
	ImGui::NewLine();
	ImGui::Image(TRAP::Graphics::TextureManager::Get2D("TRAP"), ImVec2(100, 100));
	ImGui::End();

	const TRAP::Graphics::Renderer2D::Statistics stats = TRAP::Graphics::Renderer2D::GetStats();
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::Begin("Renderer2D Stats:", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                               ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Renderer2D Stats");
	ImGui::Separator();
	ImGui::Text("DrawCalls: %u", stats.DrawCalls);
	ImGui::Text("Quads: %u", stats.QuadCount);
	ImGui::Text("Circles: %u", stats.CircleCount);
	ImGui::Text("Lines: %u", stats.LineCount);
	ImGui::Text("Vertices: %u", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %u", stats.GetTotalIndexCount());
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnAttach()
{
	TRAP::Application::SetHotReloading(true);
	TRAP::Application::GetWindow()->SetTitle("Sandbox2D");

	//Load Textures
	TRAP::Graphics::TextureManager::Load("TRAP", "./Assets/Textures/TRAPWhiteLogo2048x2048.png")->AwaitLoading();
	m_spriteSheet = TRAP::Graphics::TextureManager::Load("Controls", "./Assets/Textures/tilemap_packed.png");
	m_spriteSheet->AwaitLoading();

	TRAP::Graphics::RenderCommand::SetDepthTesting(true);
	TRAP::Graphics::RenderCommand::SetBlendConstant(TRAP::Graphics::BlendConstant::SrcAlpha,
													TRAP::Graphics::BlendConstant::One,
													TRAP::Graphics::BlendConstant::OneMinusSrcAlpha,
													TRAP::Graphics::BlendConstant::OneMinusSrcAlpha);

	TRAP::Graphics::SpriteManager::CreateFromCoords("AButton", m_spriteSheet, {4.0f, 0.0f}, {16.0f, 16.0f});
	TRAP::Graphics::SpriteManager::CreateFromCoords("BButton", m_spriteSheet, {5.0f, 0.0f}, {16.0f, 16.0f});
	TRAP::Graphics::SpriteManager::CreateFromCoords("EnterKey", m_spriteSheet, {32.0f, 10.0f}, {16.0f, 16.0f}, {2, 2});
	TRAP::Graphics::SpriteManager::CreateFromPixels("Circle", m_spriteSheet, {432.0f, 304.0f}, {16.0f, 16.0f});
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnDetach()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Update
	m_cameraController.OnUpdate(deltaTime);

	//Render
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::ClearBuffer::Color_Depth);

	TRAP::Graphics::Renderer2D::ResetStats();
	TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());
	{
		float size = 5.0f;
		for(float y = -size; y < size; y += 0.5f)
		{
			for(float x = -size; x < size; x += 0.5f)
			{
				TRAP::Math::Vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };

				// const TRAP::Math::Mat4 trans = TRAP::Math::Translate(TRAP::Math::Vec3(x, y, 0.0f)) *
		        //                                Mat4Cast(TRAP::Math::Quat(Radians(TRAP::Math::Vec3(0.0f)))) *
				// 						       TRAP::Math::Scale(TRAP::Math::Vec3(1.0f));
				// TRAP::Graphics::Renderer2D::DrawCircle(trans, color, 1.0f, 0.005f);
				TRAP::Graphics::Renderer2D::DrawQuad({{x, y, 0.0f}, {}, {0.45f, 0.45f, 1.0f}}, color);
			}
		}

		TRAP::Graphics::Renderer2D::DrawQuad({ {-1.0f, 0.0f, -0.1f}, {}, {0.8f, 0.8f, 1.0f} }, { 0.8f, 0.2f, 0.3f, 1.0f });
		TRAP::Graphics::Renderer2D::DrawQuad({ {0.5f, -0.5f, -0.1f}, {}, {0.5f, 0.75f, 1.0f} }, { 0.2f, 0.3f, 0.8f, 1.0f });
		TRAP::Graphics::Renderer2D::DrawQuad({ {0.2f, 0.5f, -0.1f}, {0.0f, 0.0f, TRAP::Application::GetTime() * -50.0f }, {1.0f, 1.0f, 1.0f} },
			                                 { 0.2f, 0.8f, 0.3f, 1.0f }, TRAP::Graphics::TextureManager::Get2D("TRAP"));

		TRAP::Graphics::Renderer2D::DrawQuad({ {-3.0f, 0.5f, 0.0f}, {}, {0.5f, 0.5f, 0.5f} }, TRAP::Graphics::SpriteManager::Get("EnterKey"));
		TRAP::Graphics::Renderer2D::DrawQuad({ {-2.5f, 0.0f, 0.0f}, {}, {0.5f, 0.5f, 0.5f} }, TRAP::Graphics::SpriteManager::Get("AButton"));
		TRAP::Graphics::Renderer2D::DrawQuad({ {-2.0f, 0.0f, 0.0f}, {}, {0.5f, 0.5f, 0.5f} }, TRAP::Graphics::SpriteManager::Get("BButton"));
		TRAP::Graphics::Renderer2D::DrawQuad({ {-2.0f, -0.5f, 0.0f}, {}, {0.5f, 0.5f, 0.5f} }, TRAP::Graphics::SpriteManager::Get("Circle"));
	}
	TRAP::Graphics::Renderer2D::EndScene();

	//Update FPS & FrameTime history
	if (m_updateFPSTimer.Elapsed() >= 0.025f)
	{
		m_updateFPSTimer.Reset();
		constinit static std::size_t frameTimeIndex = 0;
		if (frameTimeIndex < m_frameTimeHistory.size() - 1)
		{
			m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
			frameTimeIndex++;
		}
		else
		{
			std::shift_left(m_frameTimeHistory.begin(), m_frameTimeHistory.end(), 1);
			m_frameTimeHistory.back() = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnEvent(TRAP::Events::Event& event)
{
	m_cameraController.OnEvent(event);

	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(this, &Sandbox2D::OnKeyPress);
}

//-------------------------------------------------------------------------------------------------------------------//

bool Sandbox2D::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	if (event.GetKey() == TRAP::Input::Key::F10) //Enable/Disable WireFrame Mode
	{
		m_wireFrame = !m_wireFrame;
		TRAP::Graphics::RenderCommand::SetFillMode(m_wireFrame ? TRAP::Graphics::FillMode::Line :
			                                                     TRAP::Graphics::FillMode::Solid);
	}

	if (event.GetKey() == TRAP::Input::Key::F11)
		TRAP::Utils::Dialogs::ShowMsgBox("Just a prank bro!", "Critical Error");

	return true;
}

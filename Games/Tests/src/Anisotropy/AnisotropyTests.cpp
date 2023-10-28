#include "AnisotropyTests.h"

namespace
{
	constexpr std::array<const char*, 5> anisotropyLevels{"Off", "2x", "4x", "8x", "16x"};
}

//-------------------------------------------------------------------------------------------------------------------//

AnisotropyTests::AnisotropyTests()
	: Layer("Anisotropy")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void AnisotropyTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Anisotropy");

	TRAP::Graphics::TextureManager::Load("Test", "Assets/Textures/testequi.png")->AwaitLoading();

	//Camera setup
    m_camera.SetPerspective(TRAP::Math::Radians(45.0f), 0.01f);
    m_camera.SetViewportSize(TRAP::Application::GetWindow()->GetFrameBufferSize().x(),
                             TRAP::Application::GetWindow()->GetFrameBufferSize().y());

	//Use a bilinear sampler for Renderer2D (makes anisotropic filtering stand out more than point filtering)
	TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc{};
	samplerDesc.MinFilter = TRAP::Graphics::FilterType::Linear;
	samplerDesc.MagFilter = TRAP::Graphics::FilterType::Linear;
	samplerDesc.MipMapMode = TRAP::Graphics::MipMapMode::Linear;
	samplerDesc.AddressU = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.AddressV = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.AddressW = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.EnableAnisotropy = true;
	TRAP::Ref<TRAP::Graphics::Sampler> sampler = TRAP::Graphics::Sampler::Create(samplerDesc);
	TRAP::Graphics::Renderer2D::SetCustomSampler(sampler);

	m_maxAnisotropyLevelIdx = NumericCast<u32>(TRAP::Math::Ceil(TRAP::Math::Sqrt(TRAP::Graphics::RendererAPI::GPUSettings.MaxAnisotropy)));
	if(TRAP::Graphics::RenderCommand::GetAnisotropyLevel() == TRAP::Graphics::SampleCount::One)
		m_currAnisotropyLevelIdx = 0;
	else
		m_currAnisotropyLevelIdx = NumericCast<u32>(TRAP::Math::Ceil(TRAP::Math::Sqrt(static_cast<f32>(TRAP::Graphics::RenderCommand::GetAnisotropyLevel()))));
}

//-------------------------------------------------------------------------------------------------------------------//

void AnisotropyTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	TRAP::Graphics::Renderer2D::BeginScene(m_camera, TRAP::Math::Mat4(1.0f));
	TRAP::Graphics::Renderer2D::DrawQuad({ {0.0f, 0.0f, -7.5f}, {-85.0f, 0.0f, 0.0f}, {3.0f, 30.0f, 2.0f} }, TRAP::Graphics::TextureManager::Get2D("Test"));
	TRAP::Graphics::Renderer2D::EndScene();

	//Simple performance metrics
	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[Anisotropy] FPS: ", TRAP::Graphics::RenderCommand::GetCPUFPS());
		TP_INFO("[Anisotropy] CPU FrameTime: ", TRAP::Graphics::RenderCommand::GetCPUFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void AnisotropyTests::OnImGuiRender()
{
	ImGui::Begin("Anisotropy", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                                 ImGuiWindowFlags_AlwaysAutoResize);

	if(m_maxAnisotropyLevelIdx == 0)
		ImGui::Text("Anisotropic Filtering is not supported by this device!");
	else
	{
		if(ImGui::BeginCombo("Anisotropy Level", anisotropyLevels[m_currAnisotropyLevelIdx]))
		{
			for(usize i = 0; i < anisotropyLevels.size(); i++)
			{
				const bool isSelected = m_currAnisotropyLevelIdx == NumericCast<u32>(i);
				if(ImGui::Selectable(anisotropyLevels[i], isSelected, (i > m_maxAnisotropyLevelIdx) ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None))
				{
					m_currAnisotropyLevelIdx = NumericCast<u32>(i);
					if(m_currAnisotropyLevelIdx == 0)
						TRAP::Graphics::RenderCommand::SetAnisotropyLevel(TRAP::Graphics::SampleCount::One);
					else
					{
						const TRAP::Graphics::SampleCount samples = static_cast<TRAP::Graphics::SampleCount>(TRAP::Math::Pow(2.0f, NumericCast<f32>(m_currAnisotropyLevelIdx)));
						TRAP::Graphics::RenderCommand::SetAnisotropyLevel(samples);
					}
				}

				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}

	ImGui::Text("Press ESC to close");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void AnisotropyTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(this, &AnisotropyTests::OnKeyPress);
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>(this, &AnisotropyTests::OnFrameBufferResize);
}

//-------------------------------------------------------------------------------------------------------------------//

bool AnisotropyTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool AnisotropyTests::OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& e)
{
	m_camera.SetViewportSize(e.GetWidth(), e.GetHeight());

	return false;
}

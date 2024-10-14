#include "AnisotropyTests.h"

namespace
{
	[[nodiscard]] constexpr std::string SampleCountToStringForUI(const TRAP::Graphics::SampleCount sample)
	{
		if(sample == TRAP::Graphics::SampleCount::One)
			return "Off";

		return fmt::format("{}x", sample);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void DrawAnisotropyComboBox()
	{
		static constexpr std::array<TRAP::Graphics::SampleCount, 5u> anisotropySamples
		{
			TRAP::Graphics::SampleCount::One,
			TRAP::Graphics::SampleCount::Two,
			TRAP::Graphics::SampleCount::Four,
			TRAP::Graphics::SampleCount::Eight,
			TRAP::Graphics::SampleCount::Sixteen
		};

		const TRAP::Graphics::SampleCount maxSupportedAnisotropy = static_cast<TRAP::Graphics::SampleCount>(TRAP::Graphics::RendererAPI::GPUSettings.MaxAnisotropy);

		if(maxSupportedAnisotropy == TRAP::Graphics::SampleCount::One)
		{
			ImGui::Text("Anisotropic Filtering is not supported by this device!");
			return;
		}

		const TRAP::Graphics::SampleCount currentAnisotropy = TRAP::Graphics::RenderCommand::GetAnisotropyLevel();

		if(ImGui::BeginCombo("Anisotropy Level", SampleCountToStringForUI(currentAnisotropy).c_str()))
		{
			for(const auto anisotropySample : anisotropySamples)
			{
				const bool isSelected = anisotropySample == currentAnisotropy;

				const bool isAnisotropyLevelSupported = anisotropySample <= maxSupportedAnisotropy;
				if(ImGui::Selectable(SampleCountToStringForUI(anisotropySample).c_str(), isSelected,
									 isAnisotropyLevelSupported ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled))
				{
					TRAP::Graphics::RenderCommand::SetAnisotropyLevel(anisotropySample);
				}

				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}
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
	static const TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc
	{
		.MinFilter = TRAP::Graphics::FilterType::Linear,
		.MagFilter = TRAP::Graphics::FilterType::Linear,
		.MipMapMode = TRAP::Graphics::MipMapMode::Linear,
		.AddressU = TRAP::Graphics::AddressMode::Repeat,
		.AddressV = TRAP::Graphics::AddressMode::Repeat,
		.AddressW = TRAP::Graphics::AddressMode::Repeat,
		.EnableAnisotropy = true
	};
	const TRAP::Ref<TRAP::Graphics::Sampler> sampler = TRAP::Graphics::Sampler::Create(samplerDesc);
	TRAP::Graphics::Renderer2D::SetCustomSampler(sampler);
}

//-------------------------------------------------------------------------------------------------------------------//

void AnisotropyTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	TRAP::Graphics::Renderer2D::BeginScene(m_camera, TRAP::Math::Mat4(1.0f));
	TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.0f, 0.0f, -7.5f}, .Rotation={-85.0f, 0.0f, 0.0f}, .Scale={3.0f, 30.0f, 2.0f} }, TRAP::Graphics::TextureManager::Get2D("Test"));
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
	DrawAnisotropyComboBox();
	ImGui::Text("Press ESC to close");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

bool AnisotropyTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e) const
{
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	return false;
}

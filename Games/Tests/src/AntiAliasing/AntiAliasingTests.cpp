#include "AntiAliasingTests.h"

#include <ImageLoader/PortableMaps/PPMImage.h>

namespace
{
	[[nodiscard]] TRAP::Graphics::AntiAliasing DrawAntiAliasingMethodComboBox(TRAP::Graphics::AntiAliasing currentAAMethod)
	{
		static constexpr std::array<TRAP::Graphics::AntiAliasing, 2u> aaMethods
		{
			TRAP::Graphics::AntiAliasing::Off,
			TRAP::Graphics::AntiAliasing::MSAA
		};

		TRAP::Graphics::AntiAliasing newAAMethod = currentAAMethod;
		if(ImGui::BeginCombo("Anti aliasing", fmt::format("{}", currentAAMethod).c_str()))
		{
			for(const auto aaMethod : aaMethods)
			{
				const bool isSelected = aaMethod == currentAAMethod;

				if(ImGui::Selectable(fmt::format("{}", aaMethod).c_str(), isSelected))
					newAAMethod = aaMethod;

				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return newAAMethod;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::string SampleCountToStringForUI(const TRAP::Graphics::SampleCount sample)
	{
		if(sample == TRAP::Graphics::SampleCount::One)
			return "Off";

		return fmt::format("{}x", sample);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Graphics::SampleCount DrawAntiAliasingSampleComboBox(const TRAP::Graphics::AntiAliasing currentAAMethod, const TRAP::Graphics::SampleCount currentSample)
	{
		static constexpr std::array<TRAP::Graphics::SampleCount, 4u> aaSamples
		{
			TRAP::Graphics::SampleCount::Two,
			TRAP::Graphics::SampleCount::Four,
			TRAP::Graphics::SampleCount::Eight,
			TRAP::Graphics::SampleCount::Sixteen
		};

		if(currentAAMethod == TRAP::Graphics::AntiAliasing::Off)
			return TRAP::Graphics::SampleCount::Two;

		const TRAP::Graphics::SampleCount maxMSAASamples = TRAP::Graphics::RendererAPI::GPUSettings.MaxMSAASampleCount;

		TRAP::Graphics::SampleCount newAASample = currentSample;
		if(ImGui::BeginCombo("Quality", SampleCountToStringForUI(currentSample).c_str()))
		{
			for(const auto aaSample : aaSamples)
			{
				const bool isSelected = aaSample == currentSample;

				const bool isAASampleSupported = aaSample <= maxMSAASamples;
				if(ImGui::Selectable(SampleCountToStringForUI(aaSample).c_str(), isSelected,
									 isAASampleSupported ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled))
				{
					newAASample = aaSample;
				}

				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return newAASample;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void DrawAntiAliasingComboBox()
	{
		TRAP::Graphics::AntiAliasing currentAntiAliasing = TRAP::Graphics::AntiAliasing::Off;
		TRAP::Graphics::SampleCount currentAntiAliasingSamples = TRAP::Graphics::SampleCount::One;
		TRAP::Graphics::RenderCommand::GetAntiAliasing(currentAntiAliasing, currentAntiAliasingSamples);

		const auto newAAMethod = DrawAntiAliasingMethodComboBox(currentAntiAliasing);
		const auto newSample = DrawAntiAliasingSampleComboBox(currentAntiAliasing, currentAntiAliasingSamples);

		if(newAAMethod != currentAntiAliasing || newSample != currentAntiAliasingSamples)
			TRAP::Graphics::RenderCommand::SetAntiAliasing(newAAMethod, newSample);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

AntiAliasingTests::AntiAliasingTests()
	: Layer("AntiAliasing")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("AntiAliasing");
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	const f32 angle = TRAP::Application::GetTime().GetSeconds() * TRAP::Math::PI<f32>();

	TRAP::Graphics::Renderer2D::BeginScene(m_camera);
	TRAP::Graphics::Renderer2D::DrawQuad({ {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {0.25f, 0.25f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::DrawQuad({ {-0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {0.25f, 0.25f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::DrawQuad({ { 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {0.25f, 0.25f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::DrawQuad({ { 0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {0.25f, 0.25f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::DrawQuad({ { 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {0.25f, 0.25f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::EndScene();

	//Simple performance metrics
	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[AntiAliasing] FPS: ", TRAP::Graphics::RenderCommand::GetCPUFPS());
		TP_INFO("[AntiAliasing] CPU FrameTime: ", TRAP::Graphics::RenderCommand::GetCPUFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnImGuiRender()
{
	ImGui::Begin("AntiAliasing", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                      ImGuiWindowFlags_AlwaysAutoResize);

	DrawAntiAliasingComboBox();

	ImGui::Text("Press ESC to close");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

bool AntiAliasingTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();
	if (e.GetKey() == TRAP::Input::Key::P && TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Control))
	{
        //Screenshot
	    const TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
		if(testImage)
	    	TRAP::INTERNAL::PPMImage::Save(*testImage, "antialiasing.ppm");
	}

	return false;
}

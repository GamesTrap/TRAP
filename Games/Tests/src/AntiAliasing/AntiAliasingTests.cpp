#include "AntiAliasingTests.h"

#include <ImageLoader/PortableMaps/PPMImage.h>

namespace
{
	struct SampleData
	{
		const char* Name;
		TRAP::Graphics::SampleCount Samples;
	};

	constexpr std::array<SampleData, 4> Samples
	{
		{
			{ "x2", TRAP::Graphics::SampleCount::Two },
			{ "x4", TRAP::Graphics::SampleCount::Four },
			{ "x8", TRAP::Graphics::SampleCount::Eight },
			{ "x16", TRAP::Graphics::SampleCount::Sixteen }
		}
	};
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

	TRAP::Graphics::RenderCommand::GetAntiAliasing(m_antiAliasing, m_sampleCount);

	for(u32 i = std::to_underlying(TRAP::Graphics::AntiAliasing::Off); i < (std::to_underlying(TRAP::Graphics::AntiAliasing::MSAA) + 1); ++i)
		m_antiAliasingMethods.emplace_back(static_cast<TRAP::Graphics::AntiAliasing>(i));
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

	bool updateAA = false;
	if(ImGui::BeginCombo("Anti aliasing", fmt::format("{}", m_antiAliasing).c_str()))
	{
		for(usize i = 0; i < m_antiAliasingMethods.size(); ++i)
		{
			const bool isSelected = std::cmp_equal(std::to_underlying(m_antiAliasing), i);
			if(ImGui::Selectable(fmt::format("{}", m_antiAliasingMethods[i]).c_str(), isSelected))
			{
				updateAA = true;
				m_antiAliasing = m_antiAliasingMethods[i];
			}
			if(isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if(m_antiAliasing != TRAP::Graphics::AntiAliasing::Off)
	{
		u32 currSample = 0;
		for(u32 i = 0; i < Samples.size(); ++i)
		{
			if(Samples[i].Samples == m_sampleCount)
			{
				currSample = i;
				break;
			}
		}

		if(ImGui::BeginCombo("Quality", Samples[currSample].Name))
		{
			for(auto Sample : Samples)
			{
				const bool isSelected = (m_sampleCount == Sample.Samples);
				ImGuiSelectableFlags flags = ImGuiSelectableFlags_None;
				if(m_antiAliasing == TRAP::Graphics::AntiAliasing::MSAA && Sample.Samples > TRAP::Graphics::RendererAPI::GPUSettings.MaxMSAASampleCount)
					flags = ImGuiSelectableFlags_Disabled;
				if(ImGui::Selectable(Sample.Name, isSelected, flags))
				{
					updateAA = true;
					m_sampleCount = Sample.Samples;
				}
				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}

	if(updateAA)
		TRAP::Graphics::RenderCommand::SetAntiAliasing(m_antiAliasing, m_sampleCount);

	ImGui::Text("Press ESC to close");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&AntiAliasingTests::OnKeyPress, this));
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>(std::bind_front(&AntiAliasingTests::OnFrameBufferResize, this));
}

//-------------------------------------------------------------------------------------------------------------------//

bool AntiAliasingTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();
	if (e.GetKey() == TRAP::Input::Key::P && TRAP::Input::IsKeyPressed(TRAP::Input::Key::Left_Control))
	{
        //Screenshot
	    TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
		if(testImage)
	    	TRAP::INTERNAL::PPMImage::Save(testImage.get(), "antialiasing.ppm");
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool AntiAliasingTests::OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& e)
{
	m_camera.SetProjection(-e.GetAspectRatio(),
		                   e.GetAspectRatio(),
		                   -1.0f, 1.0f, -1.0f, 1.0f);

	return false;
}
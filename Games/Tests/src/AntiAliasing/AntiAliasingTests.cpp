#include "AntiAliasingTests.h"

#include <ImageLoader/PortableMaps/PPMImage.h>

AntiAliasingTests::AntiAliasingTests()
	: Layer("AntiAliasing"), m_antiAliasing(), m_sampleCount(),
	  m_camera(-TRAP::Application::GetWindow()->GetAspectRatio(),
	           TRAP::Application::GetWindow()->GetAspectRatio(),
	           -1.0f, 1.0f, -1.0f, 1.0f)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("AntiAliasing");

	TRAP::Graphics::RenderCommand::GetAntiAliasing(m_antiAliasing, m_sampleCount);

	for(uint32_t i = std::to_underlying(TRAP::Graphics::AntiAliasing::Off); i < (std::to_underlying(TRAP::Graphics::AntiAliasing::MSAA) + 1); ++i)
		m_antiAliasingMethods.emplace_back(static_cast<TRAP::Graphics::AntiAliasing>(i));
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnDetach()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	const float angle = TRAP::Application::GetTime().GetSeconds() * TRAP::Math::PI<float>();

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
	if(ImGui::BeginCombo("Anti aliasing", TRAP::Utils::String::ConvertToString(m_antiAliasing).c_str()))
	{
		for(std::size_t i = 0; i < m_antiAliasingMethods.size(); ++i)
		{
			const bool isSelected = (NumericCast<std::size_t>(std::to_underlying(m_antiAliasing)) == i);
			if(ImGui::Selectable(TRAP::Utils::String::ConvertToString(m_antiAliasingMethods[i]).c_str(), isSelected))
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
		uint32_t currSample = 0;
		for(uint32_t i = 0; i < Samples.size(); ++i)
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
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(this, &AntiAliasingTests::OnKeyPress);
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>(this, &AntiAliasingTests::OnFrameBufferResize);
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
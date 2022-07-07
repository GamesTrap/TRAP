#include "AntiAliasingTests.h"

AntiAliasingTests::AntiAliasingTests()
	: Layer("AntiAliasing"), m_fpsTimer(), m_antiAliasing(), m_sampleCount(TRAP::Graphics::SampleCount::Two),
	  m_camera(-(static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) /
	             static_cast<float>(TRAP::Application::GetWindow()->GetHeight())),
	           static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) /
			   static_cast<float>(TRAP::Application::GetWindow()->GetHeight()),
	           -1.0f, 1.0f, -1.0f, 1.0f)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("AntiAliasing");

	TRAP::Graphics::RenderCommand::GetAntiAliasing(m_antiAliasing, m_sampleCount);
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnDetach()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnUpdate(const TRAP::Utils::TimeStep& /*deltaTime*/)
{
	const float angle = TRAP::Application::GetTime().GetSeconds() * TRAP::Math::PI<float>();

	TRAP::Graphics::Renderer2D::BeginScene(m_camera);
	TRAP::Graphics::Renderer2D::DrawQuad({ {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {1.0f, 1.0f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::DrawQuad({ { 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {1.0f, 1.0f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::EndScene();

	//Simple performance metrics
	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[AntiAliasing] FPS: ", TRAP::Graphics::Renderer::GetFPS());
		TP_INFO("[AntiAliasing] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnImGuiRender()
{
	ImGui::Begin("AntiAliasing", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                      ImGuiWindowFlags_AlwaysAutoResize);

	bool updateAA = false;

	constexpr std::array<const char*, 2> antiAliasingMethods{"Off", "MSAA"};
	static int32_t currentAA = static_cast<uint32_t>(m_antiAliasing);
	if(ImGui::Combo("Anti aliasing", &currentAA, antiAliasingMethods.data(), static_cast<int32_t>(antiAliasingMethods.size())))
	{
		if(currentAA == 0)
			m_antiAliasing = TRAP::Graphics::AntiAliasing::Off;
		else if(currentAA == 1)
			m_antiAliasing = TRAP::Graphics::AntiAliasing::MSAA;

		updateAA = true;
	}

	if(currentAA != 0)
	{
		static int32_t maxSupportedQualitySize = static_cast<int32_t>((TRAP::Math::Log(static_cast<float>(TRAP::Graphics::RendererAPI::GPUSettings.MaxMSAASampleCount)) / TRAP::Math::Log(2.0f)));
		constexpr std::array<const char*, 4> sampleCounts{"x2", "x4", "x8", "x16"};
		static int32_t currentAAQuality = TRAP::Math::Log(static_cast<float>(m_sampleCount)) / TRAP::Math::Log(2.0f) - 1;
		if(currentAAQuality == -1)
			currentAAQuality = 0;
		if(ImGui::Combo("Anti aliasing quality", &currentAAQuality, sampleCounts.data(), maxSupportedQualitySize))
		{
			if(currentAAQuality == 0)
				m_sampleCount = TRAP::Graphics::SampleCount::Two;
			else if(currentAAQuality == 1)
				m_sampleCount = TRAP::Graphics::SampleCount::Four;
			else if(currentAAQuality == 2)
				m_sampleCount = TRAP::Graphics::SampleCount::Eight;
			else if(currentAAQuality == 3)
				m_sampleCount = TRAP::Graphics::SampleCount::Sixteen;

			updateAA = true;
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
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
	{
		return OnKeyPress(e);
	});
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>([this](TRAP::Events::FrameBufferResizeEvent& e)
	{
		return OnFrameBufferResize(e);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

bool AntiAliasingTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool AntiAliasingTests::OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& e)
{
	m_camera.SetProjection(-(static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight())),
		                   static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight()),
		                   -1.0f, 1.0f, -1.0f, 1.0f);

	return false;
}
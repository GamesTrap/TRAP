#include "VRSTests.h"

VRSTests::VRSTests()
	: Layer("Variable Rate Shading"),
	m_cameraController(static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::GetWindow()->GetHeight()), true),
    m_shadingRate(), m_supportedShadingRates(), m_supportedShadingRatesStrings()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VRSTests::OnImGuiRender()
{
	ImGui::Begin("Variable Rate Shading", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                               ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
    ImGui::Separator();
    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerDraw))
    {
        ImGui::Text("Per Draw Shading Rate");
        static int32_t selectedShadingRate = 0;
        if(ImGui::Combo("##Shading rate", &selectedShadingRate, m_supportedShadingRatesStrings.data(), m_supportedShadingRatesStrings.size()))
        {
            if(m_supportedShadingRates[selectedShadingRate] != m_shadingRate)
                m_shadingRate = m_supportedShadingRates[selectedShadingRate];
        }
    }
    else
        ImGui::Text("Variable Rate Shading is not supported on this GPU");

    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void VRSTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Variable Rate Shading");

    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Full))
    {
        m_supportedShadingRates.emplace_back(TRAP::Graphics::ShadingRate::Full);
        m_supportedShadingRatesStrings.emplace_back("1x1");
    }
    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Half))
    {
        m_supportedShadingRates.emplace_back(TRAP::Graphics::ShadingRate::Half);
        m_supportedShadingRatesStrings.emplace_back("2x2");
    }
    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Quarter))
    {
        m_supportedShadingRates.emplace_back(TRAP::Graphics::ShadingRate::Quarter);
        m_supportedShadingRatesStrings.emplace_back("4x4");
    }
    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Eighth))
    {
        m_supportedShadingRates.emplace_back(TRAP::Graphics::ShadingRate::Eighth);
        m_supportedShadingRatesStrings.emplace_back("8x8");
    }
    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::OneXTwo))
    {
        m_supportedShadingRates.emplace_back(TRAP::Graphics::ShadingRate::OneXTwo);
        m_supportedShadingRatesStrings.emplace_back("1x2");
    }
    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXOne))
    {
        m_supportedShadingRates.emplace_back(TRAP::Graphics::ShadingRate::TwoXOne);
        m_supportedShadingRatesStrings.emplace_back("2x1");
    }
    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXFour))
    {
        m_supportedShadingRates.emplace_back(TRAP::Graphics::ShadingRate::TwoXFour);
        m_supportedShadingRatesStrings.emplace_back("2x4");
    }
    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::FourXTwo))
    {
        m_supportedShadingRates.emplace_back(TRAP::Graphics::ShadingRate::FourXTwo);
        m_supportedShadingRatesStrings.emplace_back("4x2");
    }

    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerDraw))
        m_shadingRate = TRAP::Graphics::ShadingRate::Full;

	//Load Textures
    TRAP::Graphics::TextureManager::Load("TRAP", "./Assets/Textures/TRAPWhiteLogo2048x2048.png")->AwaitLoading();
	TRAP::Graphics::RenderCommand::SetDepthTesting(true);
	TRAP::Graphics::RenderCommand::SetBlendConstant(TRAP::Graphics::BlendConstant::SrcAlpha, TRAP::Graphics::BlendConstant::OneMinusSrcAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

void VRSTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Update
	m_cameraController.OnUpdate(deltaTime);

	//Render
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::ClearBuffer::Color_Depth);
    if(m_shadingRate != TRAP::Graphics::ShadingRate::NotSupported)
    {
        TRAP::Graphics::RenderCommand::SetShadingRate(m_shadingRate, nullptr,
                                                      TRAP::Graphics::ShadingRateCombiner::Max,
                                                      TRAP::Graphics::ShadingRateCombiner::Max);
    }

	TRAP::Graphics::Renderer2D::ResetStats();
	TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());
    TRAP::Graphics::Renderer2D::DrawQuad({ {}, {0.0f, 0.0f, TRAP::Application::GetTime() * -50.0f }, {1.0f, 1.0f, 1.0f} },
                                         { 0.2f, 0.8f, 0.3f, 1.0f }, TRAP::Graphics::TextureManager::Get2D("TRAP"));
	TRAP::Graphics::Renderer2D::EndScene();
}

//-------------------------------------------------------------------------------------------------------------------//

void VRSTests::OnEvent(TRAP::Events::Event& event)
{
	m_cameraController.OnEvent(event);

	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool VRSTests::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
	if (event.GetKey() == TRAP::Input::Key::Escape && event.GetRepeatCount() < 1)
        TRAP::Application::Shutdown();

	return true;
}
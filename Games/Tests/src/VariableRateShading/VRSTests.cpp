#include "VRSTests.h"

VRSTests::VRSTests()
	: Layer("Variable Rate Shading"),
	m_cameraController(TRAP::Application::GetWindow()->GetAspectRatio(), true),
    m_shadingRateTexture(nullptr), m_shadingRate(), m_supportedShadingRates(), m_supportedShadingRatesStrings(),
    m_supportsPerDrawVRS(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerDraw)),
    m_supportsPerTileVRS(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerTile)),
    m_perDrawActive(true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VRSTests::OnImGuiRender()
{
	ImGui::Begin("Variable Rate Shading", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                               ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
    ImGui::Separator();

    if(ImGui::BeginCombo("Shading Rate Mode", m_perDrawActive ? "Per Draw" : "Per Tile"))
    {
        if(m_supportsPerDrawVRS)
        {
            if(ImGui::Selectable("Per Draw", m_perDrawActive))
                m_perDrawActive = true;
        }
        if(m_supportsPerTileVRS)
        {
            if(ImGui::Selectable("Per Tile", !m_perDrawActive))
            {
                m_perDrawActive = false;

                TRAP::Graphics::RenderCommand::SetShadingRate(m_shadingRateTexture);
            }
        }

        ImGui::EndCombo();
    }

    if(m_supportsPerDrawVRS && m_perDrawActive)
    {
        static int32_t selectedPerDrawShadingRate = 0;
        if(ImGui::Combo("##Shading rate", &selectedPerDrawShadingRate, m_supportedShadingRatesStrings.data(), static_cast<int32_t>(m_supportedShadingRatesStrings.size())))
        {
            if(m_supportedShadingRates[selectedPerDrawShadingRate] != m_shadingRate)
                m_shadingRate = m_supportedShadingRates[selectedPerDrawShadingRate];
        }
    }

    if(!m_supportsPerDrawVRS && !m_supportsPerTileVRS)
        ImGui::Text("Variable Rate Shading is not supported on this GPU");

    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void VRSTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Variable Rate Shading");

    const auto fbSize = TRAP::Application::GetWindow()->GetFrameBufferSize();
	m_shadingRateTexture = CreateShadingRateTexture(fbSize.x, fbSize.y);

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

    if(m_perDrawActive)
    {
        TRAP::Graphics::RenderCommand::SetShadingRate(m_shadingRate,
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
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>([this](TRAP::Events::FrameBufferResizeEvent& e) { return OnFrameBufferResize(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool VRSTests::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
	if (event.GetKey() == TRAP::Input::Key::Escape && event.GetRepeatCount() < 1)
        TRAP::Application::Shutdown();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool VRSTests::OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& event)
{
    m_shadingRateTexture = CreateShadingRateTexture(event.GetWidth(), event.GetHeight());

    if(!m_perDrawActive)
        TRAP::Graphics::RenderCommand::SetShadingRate(m_shadingRateTexture);

    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> VRSTests::CreateShadingRateTexture(const uint32_t framebufferWidth,
                                                                      const uint32_t framebufferHeight)
{
    //Create empty texture for shading rate
	TRAP::Graphics::RendererAPI::TextureDesc texDesc{};
	texDesc.Width = static_cast<uint32_t>(TRAP::Math::Ceil(static_cast<float>(framebufferWidth) / static_cast<float>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelWidth)));
	texDesc.Height = static_cast<uint32_t>(TRAP::Math::Ceil(static_cast<float>(framebufferHeight) / static_cast<float>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelHeight)));
	texDesc.Format = TRAP::Graphics::API::ImageFormat::R8_UINT;
	texDesc.StartState = TRAP::Graphics::ResourceState::ShadingRateSource;
	texDesc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture;
	texDesc.Name = "Shading Rate Texture";
	TRAP::Ref<TRAP::Graphics::Texture> shadingRateTex = TRAP::Graphics::Texture::CreateCustom(texDesc);
	shadingRateTex->AwaitLoading();

    //Fill texture with lowest supported shading rate
	uint8_t lowestShadingRate = 0;
	if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Eighth))
		lowestShadingRate = (8 >> 1) | (8 << 1);
	else if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Quarter))
		lowestShadingRate =	(4 >> 1) | (4 << 1);
	else if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Half))
		lowestShadingRate = (2 >> 1) | (2 << 1);
	else if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXFour))
		lowestShadingRate = (2 >> 1) | (4 << 1);
	else if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::FourXTwo))
		lowestShadingRate = (4 >> 1) | (2 << 1);
	else if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::OneXTwo))
		lowestShadingRate = (1 >> 1) | (2 << 1);
	else if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXOne))
		lowestShadingRate = (2 >> 1) | (1 << 1);

	std::vector<uint8_t> pixels(texDesc.Width * texDesc.Height, lowestShadingRate);
	shadingRateTex->Update(pixels.data(), static_cast<uint32_t>(pixels.size()) * sizeof(uint8_t));
	shadingRateTex->AwaitLoading();
	TRAP::Graphics::RenderCommand::Transition(shadingRateTex, TRAP::Graphics::ResourceState::ShaderResource, TRAP::Graphics::ResourceState::ShadingRateSource);

    return shadingRateTex;
}
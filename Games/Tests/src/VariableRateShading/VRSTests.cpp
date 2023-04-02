#include "VRSTests.h"

VRSTests::VRSTests()
	: Layer("Variable Rate Shading"),
	m_cameraController(TRAP::Application::GetWindow()->GetAspectRatio(), true),
    m_shadingRateTexture(nullptr), m_shadingRate(),
    m_currRenderScale(1.0f),
    m_supportsPerDrawVRS(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerDraw)),
    m_supportsPerTileVRS(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerTile)),
    m_perDrawActive(true), m_visualizeShadingRate(false)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VRSTests::OnImGuiRender()
{
	ImGui::Begin("Variable Rate Shading", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                               ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
    ImGui::Separator();

    if((m_supportsPerDrawVRS || m_supportsPerTileVRS))
    {
        if(ImGui::BeginCombo("Shading Rate Mode", m_perDrawActive ? "Per Draw" : "Per Tile"))
        {
            if(ImGui::Selectable("Per Draw", m_perDrawActive, m_supportsPerDrawVRS ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled))
                m_perDrawActive = true;
            if(ImGui::Selectable("Per Tile", !m_perDrawActive, m_supportsPerTileVRS ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled))
            {
                m_perDrawActive = false;

                TRAP::Graphics::RenderCommand::SetShadingRate(m_shadingRateTexture);
            }

            ImGui::EndCombo();
        }
    }

    if(m_supportsPerDrawVRS && m_perDrawActive)
    {
        static int32_t selectedPerDrawShadingRate = 0;
        if(ImGui::BeginCombo("##Shading rate", m_shadingRates[selectedPerDrawShadingRate].Name.c_str()))
        {
            for(int32_t i = 0; i < static_cast<int32_t>(m_shadingRates.size()); ++i)
            {
                bool isSelected = i == selectedPerDrawShadingRate;
                if(ImGui::Selectable(m_shadingRates[i].Name.c_str(), isSelected, m_shadingRates[i].Supported ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled))
                {
                    selectedPerDrawShadingRate = i;
                    m_shadingRate = m_shadingRates[selectedPerDrawShadingRate].ShadingRate;
                }
            }

            ImGui::EndCombo();
        }
    }

    if((m_supportsPerDrawVRS || m_supportsPerTileVRS))
        ImGui::Checkbox("Visualize Shading Rate", &m_visualizeShadingRate);

    if(!m_supportsPerDrawVRS && !m_supportsPerTileVRS)
        ImGui::Text("Variable Rate Shading is not supported on this GPU");

    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void VRSTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Variable Rate Shading");

    m_currRenderScale = TRAP::Graphics::RenderCommand::GetRenderScale();

    if(m_supportsPerTileVRS)
    {
        const auto texRes = TRAP::Graphics::RendererAPI::GetInternalRenderResolution(TRAP::Application::GetWindow());
        m_shadingRateTexture = CreateShadingRateTexture(texRes.x, texRes.y);
    }

    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::Full, "1x1", static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Full)});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::OneXTwo, "1x2", static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::OneXTwo)});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::TwoXOne, "2x1", static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXOne)});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::Half, "2x2", static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Half)});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::TwoXFour, "2x4", static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXFour)});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::FourXTwo, "4x2", static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::FourXTwo)});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::Quarter, "4x4", static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Quarter)});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::Eighth, "8x8", static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Eighth)});

    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerDraw))
        m_shadingRate = TRAP::Graphics::ShadingRate::Full;

	//Load Textures
    TRAP::Graphics::TextureManager::Load("TRAP", "./Assets/Textures/TRAPWhiteLogo2048x2048.png")->AwaitLoading();

    //Load Shader
    if(m_supportsPerDrawVRS || m_supportsPerTileVRS)
        TRAP::Graphics::ShaderManager::LoadFile("ShadingRateVisualizer", "./Assets/Shaders/shadingratevisualizer.shader");

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

	TRAP::Graphics::RenderCommand::SetDepthTesting(true);
		TRAP::Graphics::RenderCommand::SetBlendConstant(TRAP::Graphics::BlendConstant::SrcAlpha,
														TRAP::Graphics::BlendConstant::One,
														TRAP::Graphics::BlendConstant::OneMinusSrcAlpha,
														TRAP::Graphics::BlendConstant::OneMinusSrcAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

void VRSTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
    if(m_supportsPerTileVRS && TRAP::Graphics::RenderCommand::GetRenderScale() != m_currRenderScale)
    {
        m_currRenderScale = TRAP::Graphics::RenderCommand::GetRenderScale();

        const auto texRes = TRAP::Graphics::RendererAPI::GetInternalRenderResolution(TRAP::Application::GetWindow());
        m_shadingRateTexture = CreateShadingRateTexture(texRes.x, texRes.y);
    }

	//Update
	m_cameraController.OnUpdate(deltaTime);

	//Render
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::ClearBuffer::Color_Depth);

    if(m_supportsPerDrawVRS && m_perDrawActive)
    {
        TRAP::Graphics::RenderCommand::SetShadingRate(m_shadingRate,
                                                      TRAP::Graphics::ShadingRateCombiner::Max,
                                                      TRAP::Graphics::ShadingRateCombiner::Max);
    }
    else if(m_supportsPerTileVRS && !m_perDrawActive)
        TRAP::Graphics::RenderCommand::SetShadingRate(m_shadingRateTexture);

    TRAP::Graphics::Renderer2D::ResetStats();
	TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());
    TRAP::Graphics::Renderer2D::DrawQuad({ {}, {0.0f, 0.0f, TRAP::Application::GetTime() * -50.0f }, {2.0f, 2.0f, 1.0f} },
                                         { 0.2f, 0.8f, 0.3f, 1.0f }, TRAP::Graphics::TextureManager::Get2D("TRAP"));
	TRAP::Graphics::Renderer2D::EndScene();

    if((m_supportsPerDrawVRS || m_supportsPerTileVRS) && m_visualizeShadingRate)
    {
        //Fullscreen pass
        TRAP::Graphics::ShaderManager::Get("ShadingRateVisualizer")->Use();
        TRAP::Graphics::RenderCommand::Draw(3);
    }
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

bool VRSTests::OnFrameBufferResize([[maybe_unused]] TRAP::Events::FrameBufferResizeEvent& event)
{
    if(m_supportsPerTileVRS)
    {
        const auto texRes = TRAP::Graphics::RendererAPI::GetInternalRenderResolution(TRAP::Application::GetWindow());
        m_shadingRateTexture = CreateShadingRateTexture(texRes.x, texRes.y);
    }

    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

struct ShadingRateData
{
    TRAP::Graphics::ShadingRate Rate;
    uint8_t V;
    uint8_t H;
};

TRAP::Ref<TRAP::Graphics::RenderTarget> VRSTests::CreateShadingRateTexture(const uint32_t framebufferWidth,
                                                                           const uint32_t framebufferHeight)
{
    //Create empty texture for shading rate
    TRAP::Graphics::RendererAPI::RenderTargetDesc rTDesc{};
    rTDesc.Width = static_cast<uint32_t>(TRAP::Math::Ceil(static_cast<float>(framebufferWidth) / static_cast<float>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelWidth)));
	rTDesc.Height = static_cast<uint32_t>(TRAP::Math::Ceil(static_cast<float>(framebufferHeight) / static_cast<float>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelHeight)));
	rTDesc.Format = TRAP::Graphics::API::ImageFormat::R8_UINT;
	rTDesc.StartState = TRAP::Graphics::ResourceState::ShadingRateSource;
	rTDesc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture;
    rTDesc.Depth = 1;
    rTDesc.ArraySize = 1;
    rTDesc.MipLevels = 1;
    rTDesc.SampleCount = TRAP::Graphics::SampleCount::One;
	rTDesc.Name = "Shading Rate Texture";

    TRAP::Ref<TRAP::Graphics::RenderTarget> shadingRateTex = TRAP::Graphics::RenderTarget::Create(rTDesc);

	// TRAP::Graphics::RendererAPI::TextureDesc texDesc{};
	// texDesc.Width = static_cast<uint32_t>(TRAP::Math::Ceil(static_cast<float>(framebufferWidth) / static_cast<float>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelWidth)));
	// texDesc.Height = static_cast<uint32_t>(TRAP::Math::Ceil(static_cast<float>(framebufferHeight) / static_cast<float>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelHeight)));
	// texDesc.Format = TRAP::Graphics::API::ImageFormat::R8_UINT;
	// texDesc.StartState = TRAP::Graphics::ResourceState::ShadingRateSource;
	// texDesc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture;
	// texDesc.Name = "Shading Rate Texture";
	// TRAP::Ref<TRAP::Graphics::Texture> shadingRateTex = TRAP::Graphics::Texture::CreateCustom(texDesc);
	// shadingRateTex->AwaitLoading();

    std::vector<ShadingRateData> supportedShadingRates{};
    if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Eighth))
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::Eighth, 8u >> 1u, 8u << 1u});
	if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Quarter))
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::Quarter, 4u >> 1u, 4u << 1u});
	if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXFour))
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::TwoXFour, 2u >> 1u, 4u << 1u});
	if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::FourXTwo))
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::FourXTwo, 4u >> 1u, 2u << 1u});
	if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Half))
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::Half, 2u >> 1u, 2u << 1u});
	if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::OneXTwo))
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::OneXTwo, 1u >> 1u, 2u << 1u});
	if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXOne))
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::TwoXOne, 2u >> 1u, 1u << 1u});
	if(static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Full))
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::Full, 0, 0});


    //Fill texture with lowest supported shading rate
	std::vector<uint8_t> pixels(static_cast<uint64_t>(rTDesc.Width) * rTDesc.Height, supportedShadingRates[0].V | supportedShadingRates[0].H);

    //Create a circular pattern from the available list of fragment shadring rates with decreasing sampling rates outwards (max. range, pattern)
    //See https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/extensions/fragment_shading_rate/fragment_shading_rate.cpp
    std::map<float, uint8_t> patternLookup{};
    float range = 25.0f / static_cast<float>(supportedShadingRates.size());
    float currentRange = 8.0f;
    for(std::size_t i = supportedShadingRates.size() - 1; i > 0; --i)
    {
        patternLookup[currentRange] = supportedShadingRates[i].V | supportedShadingRates[i].H;
        currentRange += range;
    }

    uint8_t* pixelData = pixels.data();
    for(uint32_t y = 0; y < rTDesc.Height; ++y)
    {
        for(uint32_t x = 0; x < rTDesc.Width; ++x)
        {
            const float deltaX = (static_cast<float>(rTDesc.Width) / 2.0f - static_cast<float>(x)) / static_cast<float>(rTDesc.Width) * 100.0f;
            const float deltaY = (static_cast<float>(rTDesc.Height) / 2.0f - static_cast<float>(y)) / static_cast<float>(rTDesc.Height) * 100.0f;
            const float distance = TRAP::Math::Sqrt(deltaX * deltaX + deltaY * deltaY);
            for(auto pattern : patternLookup)
            {
                if(distance < pattern.first)
                {
                    *pixelData = pattern.second;
                    break;
                }
            }

            ++pixelData;
        }
    }

    shadingRateTex->GetTexture()->Update(pixels.data(), static_cast<uint32_t>(pixels.size()) * sizeof(uint8_t));
    shadingRateTex->GetTexture()->AwaitLoading();
	TRAP::Graphics::RenderCommand::Transition(shadingRateTex->GetTexture(), TRAP::Graphics::ResourceState::ShaderResource, TRAP::Graphics::ResourceState::ShadingRateSource);

	// shadingRateTex->Update(pixels.data(), static_cast<uint32_t>(pixels.size()) * sizeof(uint8_t));
	// shadingRateTex->AwaitLoading();
	// TRAP::Graphics::RenderCommand::Transition(shadingRateTex, TRAP::Graphics::ResourceState::ShaderResource, TRAP::Graphics::ResourceState::ShadingRateSource);

    return shadingRateTex;
}
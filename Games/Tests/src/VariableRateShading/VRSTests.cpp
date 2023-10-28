#include "VRSTests.h"

VRSTests::VRSTests()
	: Layer("Variable Rate Shading")
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
        constinit static usize selectedPerDrawShadingRate = 0;
        if(ImGui::BeginCombo("##Shading rate", m_shadingRates[selectedPerDrawShadingRate].Name.c_str()))
        {
            for(usize i = 0; i < m_shadingRates.size(); ++i)
            {
                bool isSelected = (i == selectedPerDrawShadingRate);
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
        m_shadingRateTexture = CreateShadingRateTexture(texRes.x(), texRes.y());
    }

    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::Full, "1x1", (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Full) != TRAP::Graphics::ShadingRate::NotSupported});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::OneXTwo, "1x2", (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::OneXTwo) != TRAP::Graphics::ShadingRate::NotSupported});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::TwoXOne, "2x1", (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXOne) != TRAP::Graphics::ShadingRate::NotSupported});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::Half, "2x2", (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Half) != TRAP::Graphics::ShadingRate::NotSupported});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::TwoXFour, "2x4", (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXFour) != TRAP::Graphics::ShadingRate::NotSupported});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::FourXTwo, "4x2", (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::FourXTwo) != TRAP::Graphics::ShadingRate::NotSupported});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::Quarter, "4x4", (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Quarter) != TRAP::Graphics::ShadingRate::NotSupported});
    m_shadingRates.push_back({TRAP::Graphics::ShadingRate::Eighth, "8x8", (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Eighth) != TRAP::Graphics::ShadingRate::NotSupported});

    if((TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerDraw) != TRAP::Graphics::RendererAPI::ShadingRateCaps::NotSupported)
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
        m_shadingRateTexture = CreateShadingRateTexture(texRes.x(), texRes.y());
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
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(this, &VRSTests::OnKeyPress);
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>(this, &VRSTests::OnFrameBufferResize);
}

//-------------------------------------------------------------------------------------------------------------------//

bool VRSTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	if (event.GetKey() == TRAP::Input::Key::Escape)
        TRAP::Application::Shutdown();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool VRSTests::OnFrameBufferResize([[maybe_unused]] const TRAP::Events::FrameBufferResizeEvent& event)
{
    if(m_supportsPerTileVRS)
    {
        const auto texRes = TRAP::Graphics::RendererAPI::GetInternalRenderResolution(TRAP::Application::GetWindow());
        m_shadingRateTexture = CreateShadingRateTexture(texRes.x(), texRes.y());
    }

    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

struct ShadingRateData
{
    TRAP::Graphics::ShadingRate Rate;
    u8 V;
    u8 H;
};

TRAP::Ref<TRAP::Graphics::RenderTarget> VRSTests::CreateShadingRateTexture(const u32 framebufferWidth,
                                                                           const u32 framebufferHeight)
{
    //Create empty texture for shading rate
    TRAP::Graphics::RendererAPI::RenderTargetDesc rTDesc{};
    rTDesc.Width = NumericCast<u32>(TRAP::Math::Ceil(NumericCast<f32>(framebufferWidth) / NumericCast<f32>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelWidth)));
	rTDesc.Height = NumericCast<u32>(TRAP::Math::Ceil(NumericCast<f32>(framebufferHeight) / NumericCast<f32>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelHeight)));
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
	// texDesc.Width = NumericCast<u32>(TRAP::Math::Ceil(NumericCast<f32>(framebufferWidth) / NumericCast<f32>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelWidth)));
	// texDesc.Height = NumericCast<u32>(TRAP::Math::Ceil(NumericCast<f32>(framebufferHeight) / NumericCast<f32>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelHeight)));
	// texDesc.Format = TRAP::Graphics::API::ImageFormat::R8_UINT;
	// texDesc.StartState = TRAP::Graphics::ResourceState::ShadingRateSource;
	// texDesc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture;
	// texDesc.Name = "Shading Rate Texture";
	// TRAP::Ref<TRAP::Graphics::Texture> shadingRateTex = TRAP::Graphics::Texture::CreateCustom(texDesc);
	// shadingRateTex->AwaitLoading();

    std::vector<ShadingRateData> supportedShadingRates{};
    if((TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Eighth) != TRAP::Graphics::ShadingRate::NotSupported)
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::Eighth, 8u >> 1u, 8u << 1u});
	if((TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Quarter) != TRAP::Graphics::ShadingRate::NotSupported)
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::Quarter, 4u >> 1u, 4u << 1u});
	if((TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXFour) != TRAP::Graphics::ShadingRate::NotSupported)
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::TwoXFour, 2u >> 1u, 4u << 1u});
	if((TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::FourXTwo) != TRAP::Graphics::ShadingRate::NotSupported)
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::FourXTwo, 4u >> 1u, 2u << 1u});
	if((TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Half) != TRAP::Graphics::ShadingRate::NotSupported)
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::Half, 2u >> 1u, 2u << 1u});
	if((TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::OneXTwo) != TRAP::Graphics::ShadingRate::NotSupported)
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::OneXTwo, 1u >> 1u, 2u << 1u});
	if((TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXOne) != TRAP::Graphics::ShadingRate::NotSupported)
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::TwoXOne, 2u >> 1u, 1u << 1u});
	if((TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Full) != TRAP::Graphics::ShadingRate::NotSupported)
		supportedShadingRates.push_back({TRAP::Graphics::ShadingRate::Full, 0, 0});


    //Fill texture with lowest supported shading rate
	std::vector<u8> pixels(NumericCast<u64>(rTDesc.Width) * rTDesc.Height, supportedShadingRates[0].V | supportedShadingRates[0].H);

    //Create a circular pattern from the available list of fragment shadring rates with decreasing sampling rates outwards (max. range, pattern)
    //See https://github.com/KhronosGroup/Vulkan-Samples/blob/master/samples/extensions/fragment_shading_rate/fragment_shading_rate.cpp
    std::map<f32, u8> patternLookup{};
    const f32 range = 25.0f / NumericCast<f32>(supportedShadingRates.size());
    f32 currentRange = 8.0f;
    for(usize i = supportedShadingRates.size() - 1; i > 0; --i)
    {
        patternLookup[currentRange] = supportedShadingRates[i].V | supportedShadingRates[i].H;
        currentRange += range;
    }

    u8* pixelData = pixels.data();
    for(u32 y = 0; y < rTDesc.Height; ++y)
    {
        for(u32 x = 0; x < rTDesc.Width; ++x)
        {
            const f32 deltaX = (NumericCast<f32>(rTDesc.Width) / 2.0f - NumericCast<f32>(x)) / NumericCast<f32>(rTDesc.Width) * 100.0f;
            const f32 deltaY = (NumericCast<f32>(rTDesc.Height) / 2.0f - NumericCast<f32>(y)) / NumericCast<f32>(rTDesc.Height) * 100.0f;
            const f32 distance = TRAP::Math::Sqrt(deltaX * deltaX + deltaY * deltaY);
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

    shadingRateTex->GetTexture()->Update(pixels.data(), NumericCast<u32>(pixels.size()) * sizeof(u8));
    shadingRateTex->GetTexture()->AwaitLoading();
	TRAP::Graphics::RenderCommand::Transition(shadingRateTex->GetTexture(), TRAP::Graphics::ResourceState::ShaderResource, TRAP::Graphics::ResourceState::ShadingRateSource);

	// shadingRateTex->Update(pixels.data(), NumericCast<u32>(pixels.size()) * sizeof(u8));
	// shadingRateTex->AwaitLoading();
	// TRAP::Graphics::RenderCommand::Transition(shadingRateTex, TRAP::Graphics::ResourceState::ShaderResource, TRAP::Graphics::ResourceState::ShadingRateSource);

    return shadingRateTex;
}
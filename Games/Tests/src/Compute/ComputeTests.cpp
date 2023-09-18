#include "ComputeTests.h"

ComputeTests::ComputeTests()
    : Layer("ComputeTests")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void ComputeTests::OnAttach()
{
    TRAP::Application::SetHotReloading(true);
    TRAP::Application::GetWindow()->SetTitle("Async Compute Test");

    //Load Quad vertices
    m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(QuadVerticesIndexed.data(),
                                                          QuadVerticesIndexed.size() *
                                                          sizeof(float), TRAP::Graphics::UpdateFrequency::Static);
    const TRAP::Graphics::VertexBufferLayout layout =
    {
        { TRAP::Graphics::ShaderDataType::Float3, "Pos" },
        { TRAP::Graphics::ShaderDataType::Float2, "UV" }
    };
    m_vertexBuffer->SetLayout(layout);
    m_vertexBuffer->AwaitLoading();

    //Load Quad indices
    m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(QuadIndices.data(),
                                                        QuadIndices.size() *
                                                        sizeof(uint16_t), TRAP::Graphics::UpdateFrequency::Static);
    m_indexBuffer->AwaitLoading();

    //Load Texture
    m_colTex = TRAP::Graphics::TextureManager::Load("ColorTextureUAV", "./Assets/Textures/vulkanlogo.png", TRAP::Graphics::TextureCreationFlags::Storage);
    m_colTex->AwaitLoading();
    TRAP::Graphics::RenderCommand::Transition(m_colTex, TRAP::Graphics::ResourceState::UnorderedAccess, TRAP::Graphics::ResourceState::ShaderResource);

    //Create empty Texture
    TRAP::Graphics::TextureManager::Add(TRAP::Graphics::Texture::CreateEmpty("ComputeTargetUAV",
                                        m_colTex->GetWidth(), m_colTex->GetHeight(), m_colTex->GetBitsPerPixel(),
                                        m_colTex->GetColorFormat(), TRAP::Graphics::TextureType::Texture2D,
                                        TRAP::Graphics::TextureCreationFlags::Storage));
    m_compTex = TRAP::Graphics::TextureManager::Get2D("ComputeTargetUAV");
    m_compTex->AwaitLoading();
    TRAP::Graphics::RenderCommand::Transition(m_compTex, TRAP::Graphics::ResourceState::UnorderedAccess, TRAP::Graphics::ResourceState::ShaderResource);

    //Load Shader
    TRAP::Graphics::ShaderManager::LoadFile("Texture", "./Assets/Shaders/testtextureseperate.shader");
    TRAP::Graphics::ShaderManager::LoadFile("ComputeSharpen", "./Assets/Shaders/sharpen.compute.shader");
    TRAP::Graphics::ShaderManager::LoadFile("ComputeEmboss", "./Assets/Shaders/emboss.compute.shader");
    TRAP::Graphics::ShaderManager::LoadFile("ComputeEdgeDetect", "./Assets/Shaders/edgedetect.compute.shader");

    TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc{};
    samplerDesc.AddressU = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.AddressV = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.AddressW = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.MagFilter = TRAP::Graphics::FilterType::Linear;
	samplerDesc.MinFilter = TRAP::Graphics::FilterType::Linear;
	samplerDesc.MipMapMode = TRAP::Graphics::MipMapMode::Linear;
    samplerDesc.EnableAnisotropy = false;
    m_textureSampler = TRAP::Graphics::Sampler::Create(samplerDesc);

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void ComputeTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
    TRAP::Graphics::ShaderManager::Get("Texture")->UseSampler(0, 1, m_textureSampler.get());

    //-------------------------------------------------------------------------------------------------------------------//
    //Async compute Stuff------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    if(m_reset && !m_disabled)
    {
        m_reset = false;

        TRAP::Ref<TRAP::Graphics::Shader> shader = nullptr;
        if(m_sharpen)
            shader = TRAP::Graphics::ShaderManager::Get("ComputeSharpen");
        else if(m_emboss)
            shader = TRAP::Graphics::ShaderManager::Get("ComputeEmboss");
        else if(m_edgedetect)
            shader = TRAP::Graphics::ShaderManager::Get("ComputeEdgeDetect");

        //Transition textures (to use as storage images)
        TRAP::Graphics::RendererAPI::TextureBarrier barrier{};
        barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
        barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess;
        barrier.Texture = m_colTex.get();
        TRAP::Graphics::RenderCommand::TextureBarrier(barrier, TRAP::Graphics::QueueType::Compute);
        barrier.Texture = m_compTex.get();
        TRAP::Graphics::RenderCommand::TextureBarrier(barrier, TRAP::Graphics::QueueType::Compute);

        //ALWAYS Bind descriptors (textures, buffers, etc) before binding the shader (pipeline)!
        //else you will get a black texture (learned this the hard way, thx NVIDIA driver :C)
        shader->UseTexture(1, 0, m_colTex);
        shader->UseTexture(1, 1, m_compTex);
        shader->Use();

        //Set push constants
        static constexpr float brightness = 1.0f;
        TRAP::Graphics::RenderCommand::SetPushConstants("BrightnessRootConstant", &brightness, sizeof(brightness),
                                                        TRAP::Graphics::QueueType::Compute);

        //Dispatch work
        TRAP::Graphics::RenderCommand::Dispatch({m_compTex->GetWidth(), m_compTex->GetHeight(), 1});

        //Transition textures (to use as sampled images)
        barrier = {};
        barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess;
        barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
        barrier.Texture = m_colTex.get();
        TRAP::Graphics::RenderCommand::TextureBarrier(barrier, TRAP::Graphics::QueueType::Compute);
        barrier.Texture = m_compTex.get();
        TRAP::Graphics::RenderCommand::TextureBarrier(barrier, TRAP::Graphics::QueueType::Compute);
    }

    //-------------------------------------------------------------------------------------------------------------------//
    //Graphics Stuff-----------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    m_vertexBuffer->Use();
    m_indexBuffer->Use();

    //Use shader
    const auto texShader = TRAP::Graphics::ShaderManager::Get("Texture");
    if(m_disabled)
        texShader->UseTexture(1, 0, m_colTex, TRAP::Application::GetWindow());
    else
        texShader->UseTexture(1, 0, m_compTex, TRAP::Application::GetWindow());
    texShader->Use();

    //Render Quad
    TRAP::Graphics::RenderCommand::DrawIndexed(m_indexBuffer->GetCount());

    //Update FPS & FrameTime history
    if (m_titleTimer.Elapsed() >= 0.025f)
    {
        m_titleTimer.Reset();
        constinit static std::size_t frameTimeIndex = 0;
        if (frameTimeIndex < m_frameTimeHistory.size() - 1)
        {
            m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
            frameTimeIndex++;
        }
        else
        {
            std::shift_left(m_frameTimeHistory.begin(), m_frameTimeHistory.end(), 1);
            m_frameTimeHistory.back() = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void ComputeTests::OnImGuiRender()
{
    ImGui::Begin("Async Compute Test");
    ImGui::Text("Press ESC to close");
    ImGui::Separator();
    ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
	ImGui::Text("GPU: %s", TRAP::Graphics::RenderCommand::GetGPUName().c_str());
    ImGui::Text("CPU FPS: %u", TRAP::Graphics::RenderCommand::GetCPUFPS());
    ImGui::Text("GPU FPS: %u", TRAP::Graphics::RenderCommand::GetGPUFPS());
    ImGui::Text("CPU FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetCPUFrameTime());
    ImGui::Text("GPU Graphics FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime());
    ImGui::Text("GPU Compute FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime());
    ImGui::PlotLines("", m_frameTimeHistory.data(), NumericCast<int32_t>(m_frameTimeHistory.size()), 0, nullptr, 0,
                     33, ImVec2(200, 50));
    ImGui::Separator();
    static constexpr std::array<std::string_view, 4> shaders{"Disabled", "Sharpen", "Emboss", "Edge Detection"};
    static constexpr std::array<const char*, 4> shadersC{std::get<0>(shaders).data(), std::get<1>(shaders).data(), std::get<2>(shaders).data(), std::get<3>(shaders).data()};
    constinit static int32_t currentItem = 0;
    int32_t oldItem = currentItem;
    ImGui::Combo("##Compute shader", &currentItem, shadersC.data(), shadersC.size());
    if(currentItem != oldItem)
    {
        m_reset = true;

        m_disabled = false;
        m_sharpen = false;
        m_emboss = false;
        m_edgedetect = false;

        if(currentItem == 0)
            m_disabled = true;
        else if(currentItem == 1)
            m_sharpen = true;
        else if(currentItem == 2)
            m_emboss = true;
        else if(currentItem == 3)
            m_edgedetect = true;
    }
    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void ComputeTests::OnEvent(TRAP::Events::Event& event)
{
    TRAP::Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(this, &ComputeTests::OnKeyPress);
}

//-------------------------------------------------------------------------------------------------------------------//

bool ComputeTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
{
    if(e.GetKey() == TRAP::Input::Key::Escape)
        TRAP::Application::Shutdown();

    return false;
}

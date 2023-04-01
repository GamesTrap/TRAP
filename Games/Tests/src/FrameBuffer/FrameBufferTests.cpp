#include "FrameBufferTests.h"
#include "Graphics/RenderCommand.h"

FrameBufferTests::FrameBufferTests()
    : Layer("FrameBuffer"),
      m_vertexBuffer(nullptr),
      m_indexBuffer(nullptr),
	  m_shader(nullptr),
      m_texture(nullptr),
      m_renderTarget(nullptr),
      m_resolveTarget(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void FrameBufferTests::OnAttach()
{
    TRAP::Application::GetWindow()->SetTitle("FrameBuffer");

    //Load Quad vertices
    m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(QuadVerticesIndexed.data(),
                                                          static_cast<uint32_t>(QuadVerticesIndexed.size()) *
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
                                                        static_cast<uint16_t>(QuadIndices.size()) *
                                                        sizeof(uint16_t), TRAP::Graphics::UpdateFrequency::Static);
    m_indexBuffer->AwaitLoading();

    //Load Texture
    m_texture = TRAP::Graphics::TextureManager::Load("vulkanlogo", "./Assets/Textures/vulkanlogotransparent2.png");
    m_texture->AwaitLoading();

    //Load Shader
    m_shader = TRAP::Graphics::ShaderManager::LoadFile("TextureTest", "./Assets/Shaders/testtextureseperate.shader");

    TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc{};
    samplerDesc.AddressU = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.AddressV = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.AddressW = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.MagFilter = TRAP::Graphics::FilterType::Linear;
	samplerDesc.MinFilter = TRAP::Graphics::FilterType::Linear;
    samplerDesc.EnableAnisotropy = false;
	samplerDesc.CompareFunc = TRAP::Graphics::CompareMode::Never;
	samplerDesc.MipLodBias = 0.0f;
	samplerDesc.MipMapMode = TRAP::Graphics::MipMapMode::Linear;
    m_textureSampler = TRAP::Graphics::Sampler::Create(samplerDesc);

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

    TRAP::Graphics::AntiAliasing aaMethod = TRAP::Graphics::AntiAliasing::Off;
    TRAP::Graphics::SampleCount aaSamples = TRAP::Graphics::SampleCount::One;
    TRAP::Graphics::RenderCommand::GetAntiAliasing(aaMethod, aaSamples);
    m_MSAAEnabled = aaMethod == TRAP::Graphics::AntiAliasing::MSAA;

    TRAP::Graphics::RendererAPI::RenderTargetDesc desc{};
    desc.Width = m_texture->GetWidth() / 2;
    desc.Height = m_texture->GetHeight() / 2;
    desc.Depth = 1;
    desc.ArraySize = 1;
    desc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture;
    desc.ClearValue = TRAP::Graphics::RendererAPI::Color{0.0, 0.0, 0.0, 1.0};
    desc.Format = TRAP::Graphics::API::ImageFormat::B8G8R8A8_UNORM;
    desc.StartState = TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource;
    desc.SampleCount = m_MSAAEnabled ? aaSamples : TRAP::Graphics::SampleCount::One;
    desc.SampleQuality = 0;
    desc.Name = "Test Framebuffer";
    m_renderTarget = TRAP::Graphics::RenderTarget::Create(desc);

    desc.SampleCount = TRAP::Graphics::SampleCount::One;
    m_resolveTarget = TRAP::Graphics::RenderTarget::Create(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

void FrameBufferTests::OnDetach()
{
    //Maybe needs GPU idle
    m_resolveTarget.reset();
    m_renderTarget.reset();

    m_textureSampler.reset();
    m_indexBuffer.reset();
    m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void FrameBufferTests::OnUpdate(const TRAP::Utils::TimeStep&)
{
    m_shader->UseSampler(0, 1, m_textureSampler.get());

    //Stop RenderPass (necessary for transition)
    TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

    TRAP::Graphics::RendererAPI::RenderTargetBarrier barrier{};
    barrier.RenderTarget = m_renderTarget;
    barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource;
    barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
    TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);
    if(m_MSAAEnabled)
    {
        barrier.RenderTarget = m_resolveTarget;
        TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);
    }

    //Bind render target/framebuffer for draw
    TRAP::Graphics::RenderCommand::BindRenderTarget(m_renderTarget);
    TRAP::Graphics::RenderCommand::SetViewport(0, 0, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());
    TRAP::Graphics::RenderCommand::SetScissor(0, 0, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());

    //Bind geometry and shader
    m_vertexBuffer->Use();
    m_indexBuffer->Use();
    m_shader->UseTexture(1, 0, m_texture);
    m_shader->Use();

    //Render Quad
    TRAP::Graphics::RenderCommand::DrawIndexed(m_indexBuffer->GetCount());

    if(m_MSAAEnabled)
        TRAP::Graphics::RenderCommand::MSAAResolvePass(m_renderTarget, m_resolveTarget);

    //Stop RenderPass (necessary for transition)
    TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

    //Transition from RenderTarget to PixelShaderResource
    barrier.RenderTarget = m_renderTarget;
    barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
    barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource;
    TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);
    if(m_MSAAEnabled)
    {
        barrier.RenderTarget = m_resolveTarget;
        TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);
    }

    //Update FPS & FrameTime history
    if (m_titleTimer.Elapsed() >= 0.025f)
    {
        m_titleTimer.Reset();
        static int frameTimeIndex = 0;
        if (frameTimeIndex < static_cast<int>(m_frameTimeHistory.size() - 1))
        {
            m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
            frameTimeIndex++;
        }
        else
        {
            std::move(m_frameTimeHistory.begin() + 1, m_frameTimeHistory.end(), m_frameTimeHistory.begin());
            m_frameTimeHistory[m_frameTimeHistory.size() - 1] = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void FrameBufferTests::OnImGuiRender()
{
    ImGui::Begin("FrameBuffer");
    ImGui::Text("Press ESC to close");
    ImGui::Separator();
    ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
	ImGui::Text("GPU: %s", TRAP::Graphics::RenderCommand::GetGPUName().c_str());
    ImGui::Text("CPU FPS: %u", TRAP::Graphics::RenderCommand::GetCPUFPS());
    ImGui::Text("GPU FPS: %u", TRAP::Graphics::RenderCommand::GetGPUFPS());
    ImGui::Text("CPU FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetCPUFrameTime());
    ImGui::Text("GPU Graphics FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime());
    ImGui::Text("GPU Compute FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime());
    ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0,
                        33, ImVec2(200, 50));
    ImGui::End();

    ImGui::Begin("COLOR_B8G8R8A8_UNORM_Framebuffer", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
                                                                ImGuiWindowFlags_NoCollapse |
                                                                ImGuiWindowFlags_NoResize);
    if(m_MSAAEnabled)
    {
        ImGui::Image(m_resolveTarget->GetTexture(), ImVec2(static_cast<float>(m_resolveTarget->GetWidth()),
                     static_cast<float>(m_resolveTarget->GetHeight())), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
    }
    else
    {
        ImGui::Image(m_renderTarget->GetTexture(), ImVec2(static_cast<float>(m_renderTarget->GetWidth()),
                     static_cast<float>(m_renderTarget->GetHeight())), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
    }
    ImGui::Text("Vulkan and the Vulkan logo are registered trademarks of the Khronos Group Inc.");
    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void FrameBufferTests::OnEvent(TRAP::Events::Event& event)
{
    TRAP::Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([](TRAP::Events::KeyPressEvent& e){return OnKeyPress(e);});
}

//-------------------------------------------------------------------------------------------------------------------//

bool FrameBufferTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
    if(e.GetKey() == TRAP::Input::Key::Escape)
        TRAP::Application::Shutdown();

    return false;
}
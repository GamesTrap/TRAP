#include "FrameBufferTests.h"

namespace
{
    constexpr std::array<f32, 5ull * 4u> QuadVerticesIndexed
	{
		//XYZ UV
		-1.0f, -1.0f, 0.0f,    0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,    1.0f, 1.0f,
		 1.0f,  1.0f, 0.0f,    1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,    0.0f, 0.0f
	};

    constexpr std::array<u16, 6u> QuadIndices
	{
		0u, 1u, 2u, 2u, 3u, 0u
	};

    [[nodiscard]] TRAP::Ref<TRAP::Graphics::RenderTarget> BuildRenderTarget(const u32 width, const u32 height,
                                                                            const TRAP::Graphics::SampleCount sampleCount,
                                                                            const std::string& name)
    {
        TRAP::Graphics::RendererAPI::RenderTargetDesc desc{};
        desc.Width = width;
        desc.Height = height;
        desc.Depth = 1u;
        desc.ArraySize = 1u;
        desc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture;
        desc.ClearValue = TRAP::Graphics::RendererAPI::Color{.Red=0.0, .Green=0.0, .Blue=0.0, .Alpha=1.0};
        desc.Format = TRAP::Graphics::API::ImageFormat::B8G8R8A8_UNORM;
        desc.StartState = TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource;
        desc.SampleCount = sampleCount;
        desc.SampleQuality = 0u;
        desc.Name = name;
        return TRAP::Graphics::RenderTarget::Create(desc);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

FrameBufferTests::FrameBufferTests()
    : Layer("FrameBuffer")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void FrameBufferTests::OnAttach()
{
    TRAP::Application::GetWindow()->SetTitle("FrameBuffer");

    //Load Quad vertices
    m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(QuadVerticesIndexed, TRAP::Graphics::UpdateFrequency::Static);
    const TRAP::Graphics::VertexBufferLayout layout =
    {
        { TRAP::Graphics::ShaderDataType::Float3, "Pos" },
        { TRAP::Graphics::ShaderDataType::Float2, "UV" }
    };
    m_vertexBuffer->SetLayout(layout);
    m_vertexBuffer->AwaitLoading();

    //Load Quad indices
    m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(QuadIndices, TRAP::Graphics::UpdateFrequency::Static);
    m_indexBuffer->AwaitLoading();

    //Load Texture
    m_texture = TRAP::Graphics::TextureManager::Load("vulkanlogo", "./Assets/Textures/vulkanlogotransparent2.png");
    m_texture->AwaitLoading();

    //Load Shader
    m_shader = TRAP::Graphics::ShaderManager::LoadFile("TextureTest", "./Assets/Shaders/testtextureseperate.shader", TRAP::Graphics::ShaderType::Graphics);

    static constexpr TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc
    {
        .MinFilter = TRAP::Graphics::FilterType::Linear,
        .MagFilter = TRAP::Graphics::FilterType::Linear,
        .MipMapMode = TRAP::Graphics::MipMapMode::Linear,
        .AddressU = TRAP::Graphics::AddressMode::Repeat,
        .AddressV = TRAP::Graphics::AddressMode::Repeat,
        .AddressW = TRAP::Graphics::AddressMode::Repeat,
        .EnableAnisotropy = false,
        .CompareFunc = TRAP::Graphics::CompareMode::Never
    };
    m_textureSampler = TRAP::Graphics::Sampler::Create(samplerDesc);

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

    TRAP::Graphics::AntiAliasing aaMethod = TRAP::Graphics::AntiAliasing::Off;
    TRAP::Graphics::SampleCount aaSamples = TRAP::Graphics::SampleCount::One;
    TRAP::Graphics::RenderCommand::GetAntiAliasing(aaMethod, aaSamples);
    m_MSAAEnabled = (aaMethod == TRAP::Graphics::AntiAliasing::MSAA);

    m_renderTarget = BuildRenderTarget(m_texture->GetWidth() / 2u, m_texture->GetHeight() / 2u, aaSamples, "Intermediate RenderTarget (FrameBufferTests)");
    m_resolveTarget = BuildRenderTarget(m_texture->GetWidth() / 2u, m_texture->GetHeight() / 2u, TRAP::Graphics::SampleCount::One, "MSAA Resolve RenderTarget (FrameBufferTests)");
}

//-------------------------------------------------------------------------------------------------------------------//

void FrameBufferTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
    TRAP::Graphics::AntiAliasing aaMethod = TRAP::Graphics::AntiAliasing::Off;
    TRAP::Graphics::SampleCount aaSamples = TRAP::Graphics::SampleCount::One;
    TRAP::Graphics::RenderCommand::GetAntiAliasing(aaMethod, aaSamples);
    if(m_MSAAEnabled != (aaMethod == TRAP::Graphics::AntiAliasing::MSAA))
        m_renderTarget = BuildRenderTarget(m_texture->GetWidth() / 2u, m_texture->GetHeight() / 2u, aaSamples, "Intermediate RenderTarget (FrameBufferTests)");
    m_MSAAEnabled = (aaMethod == TRAP::Graphics::AntiAliasing::MSAA);

    m_shader->UseSampler(0u, 1u, *m_textureSampler);

    //Stop RenderPass (necessary for transition)
    TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

    TRAP::Graphics::RendererAPI::RenderTargetBarrier barrier
    {
        .RenderTarget = *m_renderTarget,
        .CurrentState = TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource,
        .NewState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget
    };
    TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);
    if(m_MSAAEnabled)
    {
        barrier.RenderTarget = *m_resolveTarget;
        TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);
    }

    //Bind render target/framebuffer for draw
    TRAP::Graphics::RenderCommand::BindRenderTarget(m_renderTarget.get());
    TRAP::Graphics::RenderCommand::SetViewport(0u, 0u, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());
    TRAP::Graphics::RenderCommand::SetScissor(0u, 0u, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());

    //Bind geometry and shader
    m_vertexBuffer->Use();
    m_indexBuffer->Use();
    m_shader->UseTexture(1u, 0u, *m_texture);
    m_shader->Use();

    //Render Quad
    TRAP::Graphics::RenderCommand::DrawIndexed(m_indexBuffer->GetCount());

    if(m_MSAAEnabled)
        TRAP::Graphics::RenderCommand::MSAAResolvePass(*m_renderTarget, *m_resolveTarget);

    //Stop RenderPass (necessary for transition)
    TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

    //Transition from RenderTarget to PixelShaderResource
    barrier.RenderTarget = *m_renderTarget;
    barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
    barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource;
    TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);
    if(m_MSAAEnabled)
    {
        barrier.RenderTarget = *m_resolveTarget;
        TRAP::Graphics::RenderCommand::RenderTargetBarrier(barrier);
    }

    //Update FPS & FrameTime history
    if (m_titleTimer.Elapsed() >= 0.025f)
    {
        m_titleTimer.Reset();
        constinit static usize frameTimeIndex = 0u;
        if (frameTimeIndex < m_frameTimeHistory.size() - 1u)
        {
            m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
            ++frameTimeIndex;
        }
        else
        {
            std::shift_left(m_frameTimeHistory.begin(), m_frameTimeHistory.end(), 1);
            m_frameTimeHistory.back() = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
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
    ImGui::PlotLines("##frametimeHistory", m_frameTimeHistory.data(), NumericCast<i32>(m_frameTimeHistory.size()),
                     0, nullptr, 0.0f, 33.0f, ImVec2(200.0f, 50.0f));
    ImGui::End();

    ImGui::Begin("COLOR_B8G8R8A8_UNORM_Framebuffer", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
                                                              ImGuiWindowFlags_NoCollapse |
                                                              ImGuiWindowFlags_NoResize);
    if(m_MSAAEnabled)
    {
        ImGui::Image(m_resolveTarget->GetTexture(), ImVec2(NumericCast<f32>(m_resolveTarget->GetWidth()),
                     NumericCast<f32>(m_resolveTarget->GetHeight())), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
    }
    else
    {
        ImGui::Image(m_renderTarget->GetTexture(), ImVec2(NumericCast<f32>(m_renderTarget->GetWidth()),
                     NumericCast<f32>(m_renderTarget->GetHeight())), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
    }
    ImGui::Text("Vulkan and the Vulkan logo are registered trademarks of the Khronos Group Inc.");
    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

bool FrameBufferTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
{
    if(e.GetKey() == TRAP::Input::Key::Escape)
        TRAP::Application::Shutdown();

    return false;
}

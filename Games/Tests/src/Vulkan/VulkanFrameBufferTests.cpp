#include "VulkanFrameBufferTests.h"

#include "Graphics/API/Objects/CommandBuffer.h"
#include "Graphics/API/Objects/RenderTarget.h"
#include "Graphics/API/Objects/SwapChain.h"
#include "Graphics/Textures/TextureBase.h"

VulkanFrameBufferTests::VulkanFrameBufferTests()
    : Layer("VulkanFrameBufferTests"),
      m_vertexBuffer(nullptr),
      m_indexBuffer(nullptr),
	  m_shader(nullptr),
      m_texture(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanFrameBufferTests::OnAttach()
{
    TRAP::Application::GetWindow()->SetTitle("Vulkan FrameBuffer Test");
    TRAP::Application::GetWindow()->SetResizable(false);

    //Load Quad vertices
    m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_quadVerticesIndexed.data(),
                                                          static_cast<uint32_t>(m_quadVerticesIndexed.size()) *
                                                          sizeof(float), TRAP::Graphics::UpdateFrequency::Static);
    const TRAP::Graphics::VertexBufferLayout layout =
    {
        { TRAP::Graphics::ShaderDataType::Float3, "Pos" },
        { TRAP::Graphics::ShaderDataType::Float2, "UV" }
    };
    m_vertexBuffer->SetLayout(layout);
    m_vertexBuffer->AwaitLoading();

    //Load Quad indices
    m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(m_quadIndices.data(),
                                                        static_cast<uint16_t>(m_quadIndices.size()) *
                                                        sizeof(uint16_t), TRAP::Graphics::UpdateFrequency::Static);
    m_indexBuffer->AwaitLoading();

    //Load Texture
    m_texture = TRAP::Graphics::TextureManager::Load("vulkanlogo", "./Assets/Textures/vulkanlogotransparent2.png").get();
    m_texture->AwaitLoading();

    //Load Shader
    m_shader = TRAP::Graphics::ShaderManager::LoadFile("VKTextureTest", "./Assets/Shaders/testtextureseperate.shader").get();

    TRAP::Graphics::SamplerDesc samplerDesc{};
    samplerDesc.AddressU = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.AddressV = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.AddressW = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.MagFilter = TRAP::Graphics::FilterType::Linear;
	samplerDesc.MinFilter = TRAP::Graphics::FilterType::Linear;
	samplerDesc.MaxAnisotropy = 0.0f;
	samplerDesc.CompareFunc = TRAP::Graphics::CompareMode::Never;
	samplerDesc.MipLodBias = 0.0f;
	samplerDesc.MipMapMode = TRAP::Graphics::MipMapMode::Linear;
    m_textureSampler = TRAP::Graphics::Sampler::Create(samplerDesc);

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

    //Use static shader resources
    m_shader->UseTexture(0, 0, m_texture);
    m_shader->UseSampler(0, 1, m_textureSampler.get());

    TRAP::Graphics::RendererAPI::RenderTargetDesc desc{};
    desc.Width = m_texture->GetWidth() / 2;
    desc.Height = m_texture->GetHeight() / 2;
    desc.Depth = 1;
    desc.ArraySize = 1;
    desc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture;
    desc.ClearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    desc.Format = TRAP::Graphics::API::ImageFormat::B8G8R8A8_UNORM;
    desc.StartState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
    desc.SampleCount = TRAP::Graphics::RendererAPI::SampleCount::SampleCount1;
    desc.SampleQuality = 0;
    desc.Name = "Test Framebuffer";
    m_renderTarget = TRAP::Graphics::RenderTarget::Create(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanFrameBufferTests::OnDetach()
{
    //Maybe needs GPU idle
    m_renderTarget.reset();

    m_textureSampler.reset();
    m_indexBuffer.reset();
    m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanFrameBufferTests::OnUpdate(const TRAP::Utils::TimeStep&)
{
    if(!m_renderedFrame) //As this is a static scene just render once
    {
        //TODO Should be unnecessary
        const auto& winData = TRAP::Graphics::RendererAPI::GetMainWindowData();
        uint32_t imgIndex = TRAP::Graphics::RendererAPI::GetCurrentImageIndex(TRAP::Application::GetWindow().get());

        //Bind render target/framebuffer for draw
        TRAP::Graphics::RenderCommand::BindRenderTarget(m_renderTarget);
        TRAP::Graphics::RenderCommand::SetViewport(0, 0, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());
        TRAP::Graphics::RenderCommand::SetScissor(0, 0, m_renderTarget->GetWidth(), m_renderTarget->GetHeight());

        //Bind geometry and shader
        m_vertexBuffer->Use();
        m_indexBuffer->Use();
        TRAP::Graphics::ShaderManager::Get("VKTextureTest")->Use();

        //Render Quad
        TRAP::Graphics::RenderCommand::DrawIndexed(m_indexBuffer->GetCount());

        //Unbind render target/framebuffer for transition
        TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

        //Transition from RenderTarget to ShaderResource
        TRAP::Graphics::RendererAPI::RenderTargetBarrier barrier{};
        barrier.RenderTarget = m_renderTarget;
        barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
        barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
        winData->GraphicCommandBuffers[imgIndex]->ResourceBarrier({}, {}, {barrier}); //TODO Via RenderCommand

        m_renderedFrame = true;
    }

    //Update FPS & FrameTime history
    if (m_titleTimer.Elapsed() >= 0.025f)
    {
        m_titleTimer.Reset();
        static int frameTimeIndex = 0;
        if (frameTimeIndex < static_cast<int>(m_frameTimeHistory.size() - 1))
        {
            m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::Renderer::GetFrameTime();
            frameTimeIndex++;
        }
        else
        {
            std::move(m_frameTimeHistory.begin() + 1, m_frameTimeHistory.end(), m_frameTimeHistory.begin());
            m_frameTimeHistory[m_frameTimeHistory.size() - 1] = TRAP::Graphics::Renderer::GetFrameTime();
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanFrameBufferTests::OnImGuiRender()
{
    ImGui::Begin("Vulkan FrameBuffer Test");
    ImGui::Text("Press ESC to close");
    ImGui::Separator();
    ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
    ImGui::Text("GPU: %s", TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGPUName().c_str());
    ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
    ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
    ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0,
                        33, ImVec2(200, 50));
    ImGui::End();

    if(m_renderedFrame)
    {
        ImGui::Begin("COLOR_B8G8R8A8_UNORM_Framebuffer", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
                                                                  ImGuiWindowFlags_NoCollapse |
                                                                  ImGuiWindowFlags_NoResize);
        ImGui::Image(m_renderTarget->GetTexture().get(), ImVec2(m_renderTarget->GetWidth(), m_renderTarget->GetHeight()), ImVec2(0, 0), ImVec2(1, 1));
        ImGui::Text("Vulkan and the Vulkan logo are registered trademarks of the Khronos Group Inc.");
        ImGui::End();
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanFrameBufferTests::OnEvent(TRAP::Events::Event& event)
{
    TRAP::Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e){return OnKeyPress(e);});
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanFrameBufferTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
    if(e.GetKey() == TRAP::Input::Key::Escape)
        TRAP::Application::Shutdown();

    return false;
}
#include "VulkanComputeTests.h"

#include <Graphics/API/Objects/SwapChain.h>

VulkanComputeTests::VulkanComputeTests()
    : Layer("VulkanComputeTests"),
      m_vertexBuffer(nullptr),
      m_indexBuffer(nullptr),
	  m_texShader(nullptr),
      m_colorTextureUAV(nullptr),
      m_computeTarget(nullptr),
      m_vulkanLogo(nullptr),
      m_disabled(true),
      m_sharpen(false),
      m_emboss(false),
      m_edgedetect(false)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanComputeTests::OnAttach()
{
    TRAP::Application::GetWindow()->SetTitle("Vulkan Async Compute Test");

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

    //Load Image
    m_vulkanLogo = TRAP::Image::LoadFromFile("./Assets/Textures/vulkanlogo.png");

    //Load Texture    {
    //TODO Texture Loading bool to indicate UAV usage
    //Currently UAV Textures require manual loading
    m_colorTextureUAV = PrepareTextureTarget(m_vulkanLogo->GetWidth(), m_vulkanLogo->GetHeight());
    m_colorTextureUAV->Update(m_vulkanLogo->GetPixelData(), m_vulkanLogo->GetPixelDataSize());
    m_colorTextureUAV->AwaitLoading();

    //TODO Make this a RenderCommand
    TRAP::Graphics::RendererAPI::GetRenderer()->Transition(m_colorTextureUAV.get(),
                                                            TRAP::Graphics::RendererAPI::ResourceState::ShaderResource,
                                                            TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess);

    //Load Shader
    m_texShader = TRAP::Graphics::ShaderManager::LoadFile("Texture", "./Assets/Shaders/testtextureseperate.shader");
    TRAP::Graphics::ShaderManager::LoadFile("ComputeSharpen", "./Assets/Shaders/sharpen.compute.shader");
    TRAP::Graphics::ShaderManager::LoadFile("ComputeEmboss", "./Assets/Shaders/emboss.compute.shader");
    TRAP::Graphics::ShaderManager::LoadFile("ComputeEdgeDetect", "./Assets/Shaders/edgedetect.compute.shader");

    TRAP::Graphics::SamplerDesc samplerDesc{};
    samplerDesc.AddressU = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.AddressV = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.AddressW = TRAP::Graphics::AddressMode::Repeat;
	samplerDesc.MagFilter = TRAP::Graphics::FilterType::Linear;
	samplerDesc.MinFilter = TRAP::Graphics::FilterType::Linear;
	samplerDesc.MipMapMode = TRAP::Graphics::MipMapMode::Linear;
    m_textureSampler = TRAP::Graphics::Sampler::Create(samplerDesc);

    //Create empty Texture
    m_computeTarget = PrepareTextureTarget(m_colorTextureUAV->GetWidth(), m_colorTextureUAV->GetHeight());

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

    m_texShader->UseSampler(0, 1, m_textureSampler.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanComputeTests::OnDetach()
{
    m_textureSampler.reset();
    m_indexBuffer.reset();
    m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanComputeTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
    //-------------------------------------------------------------------------------------------------------------------//
    //Async compute Stuff------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    //TODO VulkanInits VulkanImageCreateInfo set sharing mode back to exclusive

    if(!m_disabled)
    {
        TRAP::Graphics::Shader* shader = nullptr;
        if(m_sharpen)
            shader = TRAP::Graphics::ShaderManager::Get("ComputeSharpen");
        else if(m_emboss)
            shader = TRAP::Graphics::ShaderManager::Get("ComputeEmboss");
        else if(m_edgedetect)
            shader = TRAP::Graphics::ShaderManager::Get("ComputeEdgeDetect");

        //Bind compute shader
        shader->Use(TRAP::Application::GetWindow());

        //Set shader descriptors
        shader->UseTexture(1, 0, m_colorTextureUAV.get(), TRAP::Application::GetWindow()); //TODO Update Shader class
        shader->UseTexture(1, 1, m_computeTarget.get(), TRAP::Application::GetWindow());

        //Dispatch compute work (work groups are retrieved through automatic reflection)
        //TODO Make this a RenderCommand
        TRAP::Graphics::RendererAPI::GetRenderer()->Dispatch({m_colorTextureUAV->GetWidth(),
                                                              m_colorTextureUAV->GetHeight(),
                                                              1},
                                                             TRAP::Application::GetWindow());
    }

    //-------------------------------------------------------------------------------------------------------------------//
    //Graphics Stuff-----------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    //Stop RenderPass (necessary for ownership transfer)
    TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

    //Layout transition
    TRAP::Graphics::RendererAPI::TextureBarrier barrier{};
    barrier.Texture = m_computeTarget.get();
    barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess;
    barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
    TRAP::Graphics::RenderCommand::TextureBarrier(barrier);
    barrier.Texture = m_colorTextureUAV.get();
    TRAP::Graphics::RenderCommand::TextureBarrier(barrier);

    const auto& mainWinData = TRAP::Graphics::RendererAPI::GetRenderer()->GetMainWindowData();
    TRAP::Graphics::RenderCommand::BindRenderTarget(mainWinData.SwapChain->GetRenderTargets()[mainWinData.CurrentSwapChainImageIndex]);

    m_vertexBuffer->Use();
    m_indexBuffer->Use();

    //Use shader
    m_texShader->Use();
    if(m_disabled)
        m_texShader->UseTexture(1, 0, m_colorTextureUAV.get(), TRAP::Application::GetWindow());
    else
        m_texShader->UseTexture(1, 0, m_computeTarget.get(), TRAP::Application::GetWindow());

    //Render Quad
    TRAP::Graphics::RenderCommand::DrawIndexed(m_indexBuffer->GetCount());

    //Stop RenderPass (necessary for layout transition)
    TRAP::Graphics::RenderCommand::BindRenderTarget(nullptr);

    //Layout transition
    barrier = {};
    barrier.Texture = m_computeTarget.get();
    barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
    barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess;
    TRAP::Graphics::RenderCommand::TextureBarrier(barrier);
    barrier.Texture = m_colorTextureUAV.get();
    TRAP::Graphics::RenderCommand::TextureBarrier(barrier);

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

void VulkanComputeTests::OnImGuiRender()
{
    ImGui::Begin("Vulkan Compute Test");
    ImGui::Text("Press ESC to close");
    ImGui::Separator();
    ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
    ImGui::Text("GPU: %s", TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGPUName().c_str());
    ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
    ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
    ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0,
                     33, ImVec2(200, 50));
    ImGui::Separator();
    const std::array<std::string, 4> shaders{"Disabled", "Sharpen", "Emboss", "Edge Detection"};
    static std::string currentItem = shaders[0];
    ImGui::Text("Compute shader: ");
    if(ImGui::BeginCombo("##Compute shader", currentItem.c_str()))
    {
        for(uint32_t n = 0; n < shaders.size(); ++n)
        {
            bool isSelected = (currentItem == shaders[n]);
            if(ImGui::Selectable(shaders[n].c_str(), isSelected))
                currentItem = shaders[n];
            if(isSelected)
            {
                ImGui::SetItemDefaultFocus();

                m_disabled = false;
                m_sharpen = false;
                m_emboss = false;
                m_edgedetect = false;
            }
        }
        ImGui::EndCombo();

        if(currentItem == shaders[0])
            m_disabled = true;
        else if(currentItem == shaders[1])
            m_sharpen = true;
        else if(currentItem == shaders[2])
            m_emboss = true;
        else if(currentItem == shaders[3])
            m_edgedetect = true;
    }
    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanComputeTests::OnEvent(TRAP::Events::Event& event)
{
    TRAP::Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e){return OnKeyPress(e);});
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanComputeTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
    if(e.GetKey() == TRAP::Input::Key::Escape)
        TRAP::Application::Shutdown();
    else if(e.GetKey() == TRAP::Input::Key::E)
        m_emboss = !m_emboss;

    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture> VulkanComputeTests::PrepareTextureTarget(const uint32_t width, const uint32_t height)
{
    //Create empty Texture
    TRAP::Graphics::RendererAPI::TextureDesc texDesc{};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.Format = TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM;
    texDesc.StartState = TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess;
    texDesc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture | TRAP::Graphics::RendererAPI::DescriptorType::RWTexture;

    TRAP::Scope<TRAP::Graphics::Texture> texTarget = TRAP::Graphics::Texture::CreateCustom(texDesc);
    texTarget->AwaitLoading();

    return texTarget;
}
#include "VulkanTextureTests.h"

VulkanTextureTests::VulkanTextureTests()
    : Layer("VulkanTextureTests"),
      m_vertexBuffer(nullptr),
      m_indexBuffer(nullptr),
      m_cycleMips(false),
      m_currentMipLevel(0),
      m_maxMipLevel(0),
      m_updateTexture(false),
	  m_shader(nullptr),
      m_texture(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnAttach()
{
    TRAP::Application::GetWindow()->SetTitle("Vulkan Texture Test");

    //Mount Shader & Texture Folders
    TRAP::VFS::MountShaders("Assets/Shaders");
    TRAP::VFS::MountTextures("Assets/Textures");

    //Load Quad vertices
    m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_quadVerticesIndexed.data(),
                                                          static_cast<uint32_t>(m_quadVerticesIndexed.size()) *
                                                          sizeof(float), TRAP::Graphics::UpdateFrequency::None);
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
                                                        sizeof(uint16_t), TRAP::Graphics::UpdateFrequency::None);
    m_indexBuffer->AwaitLoading();

    //Load Images
    m_vulkanLogo = TRAP::Image::LoadFromFile("/Textures/vulkanlogo.png");
    m_vulkanLogoTransparent = TRAP::Image::LoadFromFile("/Textures/vulkanlogoTransparent.png");

    //Load Texture
    m_texture = TRAP::Graphics::TextureManager::Load("vulkanlogo", m_vulkanLogo).get();
    m_texture->AwaitLoading();
    m_maxMipLevel = m_texture->GetMipLevels();

    //Load Shader
    std::vector<TRAP::Graphics::Shader::Macro> macros{};
    macros.emplace_back(TRAP::Graphics::Shader::Macro{"MAX_TEXTURE_MIP_LEVELS", std::to_string(m_maxMipLevel)});
    m_shader = TRAP::Graphics::ShaderManager::LoadFile("VKTextureTest", "/shaders/testtextureseperate.shader",
                                                       &macros).get();

    TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc{};
    samplerDesc.AddressU = TRAP::Graphics::RendererAPI::AddressMode::Repeat;
	samplerDesc.AddressV = TRAP::Graphics::RendererAPI::AddressMode::Repeat;
	samplerDesc.AddressW = TRAP::Graphics::RendererAPI::AddressMode::Repeat;
	samplerDesc.MagFilter = TRAP::Graphics::RendererAPI::FilterType::Linear;
	samplerDesc.MinFilter = TRAP::Graphics::RendererAPI::FilterType::Linear;
	samplerDesc.MaxAnisotropy = 0.0f;
	samplerDesc.CompareFunc = TRAP::Graphics::RendererAPI::CompareMode::Never;
	samplerDesc.MipLodBias = 0.0f;
	samplerDesc.MipMapMode = TRAP::Graphics::RendererAPI::MipMapMode::Linear;
    m_textureSampler = TRAP::Graphics::Sampler::Create(samplerDesc);

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

    m_shader->UseTexture(0, 0, m_texture);
    m_shader->UseSampler(0, 1, m_textureSampler.get());

    //Bind buffers
    m_vertexBuffer->Use();
    m_indexBuffer->Use();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnDetach()
{
    m_textureSampler.reset();
    m_indexBuffer.reset();
    m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
    m_vertexBuffer->Use();
    m_indexBuffer->Use();

    if(m_cycleMips)
    {
        static float time = 0.0f;
        time += deltaTime.GetMilliseconds();
        if(time > 1000.0f)
        {
            m_currentMipLevel = (m_currentMipLevel + 1) % m_maxMipLevel;
            time = 0.0f;
        }
    }
    else
        m_currentMipLevel = 0;

    if(m_updateTexture)
    {
        m_updateTexture = false;
        m_currentTexture = (m_currentTexture + 1) % 2;

        if(m_currentTexture == 0)
            m_texture->Update(m_vulkanLogo->GetPixelData(), m_vulkanLogo->GetPixelDataSize());
        else if(m_currentTexture == 1)
            m_texture->Update(m_vulkanLogoTransparent->GetPixelData(), m_vulkanLogoTransparent->GetPixelDataSize());
    }

	//Bind shader
    TRAP::Graphics::ShaderManager::Get("VKTextureTest")->Use();
	//TODO Currently shader needs to be bound in orderd to set push constants
	//TODO PushConstant binding should use a shader as input or better Shader should have a function for
    //binding push constants

    //Upload mip level index
    //TODO Use Shaders RootSignature internally instead of GraphicsPipelineDescs RootSignature
    TRAP::Graphics::RenderCommand::SetPushConstants("SamplerRootConstant", &m_currentMipLevel);

    //Render Quad
    TRAP::Graphics::RenderCommand::DrawIndexed(m_indexBuffer->GetCount());
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnImGuiRender()
{
    ImGui::Begin("Vulkan Texture Test");
    ImGui::Text("Press ESC to close");
    ImGui::Text("Current Mip Level: %i", m_currentMipLevel);
    ImGui::Text("Cycle Mip Levels (F1): %s", m_cycleMips ? "Enabled" : "Disabled");
    ImGui::Text("Update Texture (F2)");
    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnEvent(TRAP::Events::Event& event)
{
    TRAP::Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e){return OnKeyPress(e);});
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanTextureTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
    if(e.GetKey() == TRAP::Input::Key::F1)
    {
        m_cycleMips = !m_cycleMips;
        TP_TRACE("[VulkanTextureTests] Mipmap cycling: ", m_cycleMips ? "On" : "Off");
    }
    else if(e.GetKey() == TRAP::Input::Key::F2)
    {
        m_updateTexture = true;
        TP_TRACE("[VulkanTextureTests] Updating Texture");
    }
    else if(e.GetKey() == TRAP::Input::Key::Escape)
        TRAP::Application::Shutdown();

    return false;
}
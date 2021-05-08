#include "VulkanTextureTests.h"

#include <Graphics/API/Objects/DescriptorPool.h>
#include <Graphics/API/Objects/DescriptorSet.h>

VulkanTextureTests::VulkanTextureTests()
    : Layer("VulkanTextureTests"),
      m_vertexBuffer(nullptr),
      m_indexBuffer(nullptr),
      m_texture(nullptr),
      m_descriptorSet(nullptr),
      m_currentMipLevel(0),
      m_maxMipLevel(0),
      m_textureSamplers()
{}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnAttach()
{
    TRAP::Application::GetWindow()->SetTitle("Vulkan Texture Test");

    //Mount Shader & Texture Folders
    TRAP::VFS::MountShaders("Assets/Shaders");
    TRAP::VFS::MountTextures("Assets/Textures");

    //Setup VertexBuffer and BufferLayout
    m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_quadVerticesIndexed.data(), static_cast<uint32_t>(m_quadVerticesIndexed.size()) * sizeof(float), TRAP::Graphics::BufferUsage::Static);
    const TRAP::Graphics::BufferLayout layout =
    {
        { TRAP::Graphics::ShaderDataType::Float3, "Pos" },
        { TRAP::Graphics::ShaderDataType::Float2, "UV" }
    };
    m_vertexBuffer->SetLayout(layout);
    m_vertexBuffer->AwaitLoading();

    //Setup IndexBuffer
    m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(m_quadIndices.data(), static_cast<uint16_t>(m_quadIndices.size()) * sizeof(uint16_t), TRAP::Graphics::BufferUsage::Static);
    m_indexBuffer->AwaitLoading();

    //////////////////////////////////////
	//INTERNAL CODE USE AT YOUR OWN RISK//
	//////////////////////////////////////
    //Load Texture
    TRAP::Graphics::RendererAPI::TextureLoadDesc textureLoadDesc{};
    textureLoadDesc.Filepaths[0] = "/Textures/vulkanlogo.png";
    TRAP::Graphics::API::SyncToken token{};
    textureLoadDesc.Texture = &m_texture;
    TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(textureLoadDesc, &token);
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForToken(&token);
    m_maxMipLevel = m_texture->GetMipLevels();
    m_textureSamplers.resize(m_maxMipLevel);
	//////////////////////////////////////

    //Load Shader
    std::vector<TRAP::Graphics::Shader::Macro> macros{};
    macros.emplace_back(TRAP::Graphics::Shader::Macro{"MAX_TEXTURE_MIP_LEVELS", std::to_string(m_maxMipLevel)});
    TRAP::Graphics::ShaderManager::LoadFile("VKTextureTest", "/shaders/testtextureseperate.shader", &macros);

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
	samplerDesc.ForceMipLevel = true;
    samplerDesc.MipLevel = 0.0f;
    for(uint32_t i = 0; i < m_maxMipLevel; ++i)
    {
        samplerDesc.MipLevel = static_cast<float>(i);
        m_textureSamplers[i] = TRAP::Graphics::Sampler::Create(samplerDesc);
    }

	//TODO Test Runtime Texture Update

    //Just in case
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

    //Bind everything
    m_vertexBuffer->Use();
    m_indexBuffer->Use();
    TRAP::Graphics::ShaderManager::Get("VKTextureTest")->Use();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnDetach()
{
    m_textureSamplers.clear();
    m_texture.reset();
    m_indexBuffer.reset();
    m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
    m_vertexBuffer->Use();
    m_indexBuffer->Use();

    //////////////////////////////////////
	//INTERNAL CODE USE AT YOUR OWN RISK//
	//////////////////////////////////////
    if(!m_descriptorSet)
    {
        TRAP::Graphics::RendererAPI::DescriptorSetDesc desc{};
        desc.RootSignature = TRAP::Graphics::RendererAPI::GetGraphicsRootSignature();
        desc.UpdateFrequency = TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::None;
        desc.MaxSets = 1;
        m_descriptorSet = TRAP::Graphics::RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(desc);

        TRAP::Graphics::RendererAPI::DescriptorData param{};
        param.Name = "Texture";
        param.Resource = std::vector<TRAP::Graphics::API::VulkanTexture*>{m_texture.get()};
        param.Count = 1;

        TRAP::Graphics::RendererAPI::DescriptorData paramSamplers{};
        paramSamplers.Name = "Samplers";
        std::vector<TRAP::Graphics::Sampler*> samplers(m_maxMipLevel, nullptr);
        for(uint32_t i = 0; i < samplers.size(); ++i)
            samplers[i] = m_textureSamplers[i].get();
        paramSamplers.Resource = samplers;
        paramSamplers.Count = m_textureSamplers.size();

        m_descriptorSet->Update(0, {param, paramSamplers});
    }
    TRAP::Graphics::RendererAPI::GetRenderer()->BindDescriptorSet(*m_descriptorSet, 0);
	//////////////////////////////////////

    if(m_cycleMips)
    {
        static float time = 0.0f;
        time += deltaTime.GetMilliseconds();
        if(time > 1000.0f)
        {
            m_currentMipLevel = ++m_currentMipLevel % m_maxMipLevel;
            time = 0.0f;
        }
    }
    else
        m_currentMipLevel = 0;

    //Upload mip level index
    TRAP::Graphics::RendererAPI::GetRenderer()->BindPushConstants("SamplerRootConstant", &m_currentMipLevel);

    //Render Quad
    TRAP::Graphics::RendererAPI::GetRenderer()->DrawIndexed(m_indexBuffer->GetCount());
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnImGuiRender()
{
    ImGui::Begin("Vulkan Texture Test");
    ImGui::Text("Current Mip Level: %i", m_currentMipLevel);
    ImGui::Text("Cycle Mip Levels (F1): %s", m_cycleMips ? "Enabled" : "Disabled");
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
    else if(e.GetKey() == TRAP::Input::Key::Escape)
        TRAP::Application::Shutdown();

    return false;
}
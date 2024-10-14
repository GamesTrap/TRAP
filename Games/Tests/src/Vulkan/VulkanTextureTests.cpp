#include "VulkanTextureTests.h"

namespace
{
    constexpr std::array<f32, 5ull * 4u> QuadVerticesIndexed
	{
		//XYZ UV
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    0.0f, 0.0f
	};

    constexpr std::array<u16, 6u> QuadIndices
	{
		0u, 1u, 2u, 2u, 3u, 0u
	};
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnAttach()
{
    TRAP::Application::GetWindow()->SetTitle("Vulkan Texture Test");

    //Load Quad vertices
    m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(QuadVerticesIndexed,
                                                          TRAP::Graphics::UpdateFrequency::Static);
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

    //Load Images
    m_vulkanLogo = TRAP::Image::LoadFromFile("./Assets/Textures/vulkanlogo.png");
    m_vulkanLogoTransparent = TRAP::Image::LoadFromFile("./Assets/Textures/vulkanlogoTransparent.png");

    //Load Texture
    m_texture = TRAP::Graphics::TextureManager::Load("vulkanlogo", *m_vulkanLogo);
    m_texture->AwaitLoading();
    m_maxMipLevel = m_texture->GetMipLevels();

    //Load Shader
    m_shader = TRAP::Graphics::ShaderManager::LoadFile("VKTextureTest", "./Assets/Shaders/testtextureseperatelod.shader", TRAP::Graphics::ShaderType::Graphics);

    static const TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc
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
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTextureTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
    m_shader->UseTexture(0u, 0u, *m_texture);
    m_shader->UseSampler(0u, 1u, *m_textureSampler);

    m_vertexBuffer->Use();
    m_indexBuffer->Use();

    if(m_cycleMips)
    {
        constinit static f32 time = 0.0f;
        time += deltaTime.GetMilliseconds();
        if(time > 1000.0f)
        {
            m_currentMipLevel = (m_currentMipLevel + 1u) % m_maxMipLevel;
            time = 0.0f;
        }
    }
    else
        m_currentMipLevel = 0u;

    if(m_updateTexture)
    {
        m_updateTexture = false;
        m_currentTexture = (m_currentTexture + 1u) % 2u;

        if(m_currentTexture == 0u)
            m_texture->Update(m_vulkanLogo->GetPixelData());
        else if(m_currentTexture == 1u)
            m_texture->Update(m_vulkanLogoTransparent->GetPixelData());
    }

	//Use shader
    TRAP::Graphics::ShaderManager::GetGraphics("VKTextureTest")->Use(); //Always bind shader before settings push constants

    //Use Push Constants
    //Upload mip level index
    TRAP::Graphics::RenderCommand::SetPushConstants("SamplerRootConstant", m_currentMipLevel);

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

bool VulkanTextureTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
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

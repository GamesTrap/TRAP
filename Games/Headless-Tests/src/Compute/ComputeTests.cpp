#include "ComputeTests.h"

#include <span>
#include <ImageLoader/PortableMaps/PPMImage.h>

namespace
{
    constexpr std::array<f32, 5ull * 4> QuadVerticesIndexed
	{
		//XYZ UV
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    0.0f, 0.0f
	};

    constexpr std::array<u16, 6> QuadIndices
	{
		0, 1, 2, 2, 3, 0
	};
}

//-------------------------------------------------------------------------------------------------------------------//

void ComputeTests::OnAttach()
{
    if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::NONE)
        return;

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
    m_colTex = TRAP::Graphics::TextureManager::Load("ColorTextureUAV", "./Assets/Textures/vulkanlogo.png", TRAP::Graphics::TextureCreationFlags::Storage);
    m_colTex->AwaitLoading();

    //Create empty Texture
    TRAP::Graphics::TextureManager::Add(TRAP::Graphics::Texture::Create2D("ComputeTargetUAV",
                                        m_colTex->GetWidth(), m_colTex->GetHeight(), m_colTex->GetBitsPerPixel(),
                                        m_colTex->GetColorFormat(),
                                        TRAP::Graphics::TextureCreationFlags::Storage));
    m_compTex = TRAP::Graphics::TextureManager::Get2D("ComputeTargetUAV");
    m_compTex->AwaitLoading();

    //Load Shader
    TRAP::Graphics::ShaderManager::LoadFile("Texture", "./Assets/Shaders/testtextureseperate.shader", TRAP::Graphics::ShaderType::Graphics);
    TRAP::Graphics::ShaderManager::LoadFile("ComputeEmboss", "./Assets/Shaders/emboss.compute.shader", TRAP::Graphics::ShaderType::Compute);

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
    if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::NONE)
        return;

    TRAP::Graphics::ShaderManager::GetGraphics("Texture")->UseSampler(0, 1, *m_textureSampler);

    constinit static u32 frames = 0;
    if(frames == 3)
    {

        //Screenshot
	    TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
        if(testImage)
	        TRAP::INTERNAL::PPMImage::Save(testImage.get(), "compute.ppm");

        TRAP::Application::Shutdown();
        return;
    }

    //-------------------------------------------------------------------------------------------------------------------//
    //Async compute Stuff------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    constinit static bool once = true;
    if(once)
    {
        once = false;

        const auto shader = TRAP::Graphics::ShaderManager::GetCompute("ComputeEmboss");

        //Set shader descriptors
        shader->UseTexture(1, 0, *m_colTex);
        shader->UseTexture(1, 1, *m_compTex);
        //Bind compute shader
        shader->Use();

        static constexpr f32 brightness = 1.0f;
        TRAP::Graphics::RenderCommand::SetPushConstants("BrightnessRootConstant", brightness,
                                                        TRAP::Graphics::QueueType::Compute);

        //Dispatch compute work (local thread group sizes are retrieved through automatic reflection)
        TRAP::Graphics::RenderCommand::Dispatch({m_compTex->GetWidth(), m_compTex->GetHeight(), 1});

        //Transition textures (to use as sampled images)
        TRAP::Graphics::RendererAPI::TextureBarrier barrier
        {
            .Texture = *m_colTex,
            .CurrentState = TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess,
            .NewState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource
        };
        TRAP::Graphics::RenderCommand::TextureBarrier(barrier, TRAP::Graphics::QueueType::Compute);
        barrier.Texture = *m_compTex;
        TRAP::Graphics::RenderCommand::TextureBarrier(barrier, TRAP::Graphics::QueueType::Compute);
    }

    //-------------------------------------------------------------------------------------------------------------------//
    //Graphics Stuff-----------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    m_vertexBuffer->Use();
    m_indexBuffer->Use();

    //Use shader
    const auto texShader = TRAP::Graphics::ShaderManager::GetGraphics("Texture");
    texShader->UseTexture(1, 0, *m_compTex);
    texShader->Use();

    //Render Quad
    TRAP::Graphics::RenderCommand::DrawIndexed(m_indexBuffer->GetCount());

    ++frames;
}

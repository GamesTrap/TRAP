#include "ComputeTests.h"

enum class ComputeShader : u8
{
    Off,
    Sharpen,
    Emboss,
    EdgeDetection
};

template<>
struct fmt::formatter<ComputeShader>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const ComputeShader computeShader,
                                                fmt::format_context& ctx)
    {
        std::string result{};
        switch(computeShader)
        {
        case ComputeShader::Off:
            result = "Off";
            break;
        case ComputeShader::Sharpen:
            result = "Sharpen";
            break;
        case ComputeShader::Emboss:
            result = "Emboss";
            break;
        case ComputeShader::EdgeDetection:
            result = "Edge detection";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", result);
    }
};

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

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

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] bool DrawComputeShaderComboBox(ComputeShader& outSelectedComputeShader)
	{
        static constexpr std::array<ComputeShader, 4u> shaders
        {
            ComputeShader::Off,
            ComputeShader::Sharpen,
            ComputeShader::Emboss,
            ComputeShader::EdgeDetection
        };

        static ComputeShader currentShader = ComputeShader::Off;

        bool hasChanged = false;
		if(ImGui::BeginCombo("Compute shader", fmt::format("{}", currentShader).c_str()))
		{
			for(const auto computeShader : shaders)
			{
				const bool isSelected = currentShader == computeShader;

				if(ImGui::Selectable(fmt::format("{}", computeShader).c_str(), isSelected))
                {
                    hasChanged = currentShader != computeShader;
                    currentShader = computeShader;
                    outSelectedComputeShader = currentShader;
                }

				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

        return hasChanged;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

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
    TRAP::Graphics::RenderCommand::Transition(m_colTex, TRAP::Graphics::ResourceState::UnorderedAccess, TRAP::Graphics::ResourceState::ShaderResource);

    //Create empty Texture
    TRAP::Graphics::TextureManager::Add(TRAP::Graphics::Texture::Create2D("ComputeTargetUAV",
                                        m_colTex->GetWidth(), m_colTex->GetHeight(), m_colTex->GetBitsPerPixel(),
                                        m_colTex->GetColorFormat(),
                                        TRAP::Graphics::TextureCreationFlags::Storage));
    m_compTex = TRAP::Graphics::TextureManager::Get2D("ComputeTargetUAV");
    m_compTex->AwaitLoading();
    TRAP::Graphics::RenderCommand::Transition(m_compTex, TRAP::Graphics::ResourceState::UnorderedAccess, TRAP::Graphics::ResourceState::ShaderResource);

    //Load Shader
    TRAP::Graphics::ShaderManager::LoadFile("Texture", "./Assets/Shaders/testtextureseperate.shader", TRAP::Graphics::ShaderType::Graphics);
    TRAP::Graphics::ShaderManager::LoadFile("ComputeSharpen", "./Assets/Shaders/sharpen.compute.shader", TRAP::Graphics::ShaderType::Compute);
    TRAP::Graphics::ShaderManager::LoadFile("ComputeEmboss", "./Assets/Shaders/emboss.compute.shader", TRAP::Graphics::ShaderType::Compute);
    TRAP::Graphics::ShaderManager::LoadFile("ComputeEdgeDetect", "./Assets/Shaders/edgedetect.compute.shader", TRAP::Graphics::ShaderType::Compute);

    static constexpr TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc
    {
        .MinFilter = TRAP::Graphics::FilterType::Linear,
        .MagFilter = TRAP::Graphics::FilterType::Linear,
        .MipMapMode = TRAP::Graphics::MipMapMode::Linear,
        .AddressU = TRAP::Graphics::AddressMode::Repeat,
        .AddressV = TRAP::Graphics::AddressMode::Repeat,
        .AddressW = TRAP::Graphics::AddressMode::Repeat,
        .EnableAnisotropy = false
    };
    m_textureSampler = TRAP::Graphics::Sampler::Create(samplerDesc);

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void ComputeTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
    TRAP::Graphics::ShaderManager::GetGraphics("Texture")->UseSampler(0u, 1u, *m_textureSampler);

    //-------------------------------------------------------------------------------------------------------------------//
    //Async compute Stuff------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    if(m_reset && !m_disabled)
    {
        m_reset = false;

        TRAP::Ref<TRAP::Graphics::Shader> shader = nullptr;
        if(m_sharpen)
            shader = TRAP::Graphics::ShaderManager::GetCompute("ComputeSharpen");
        else if(m_emboss)
            shader = TRAP::Graphics::ShaderManager::GetCompute("ComputeEmboss");
        else if(m_edgedetect)
            shader = TRAP::Graphics::ShaderManager::GetCompute("ComputeEdgeDetect");

        //Transition textures (to use as storage images)
        TRAP::Graphics::RendererAPI::TextureBarrier barrier
        {
            .Texture = *m_colTex,
            .CurrentState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource,
            .NewState = TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess
        };
        TRAP::Graphics::RenderCommand::TextureBarrier(barrier, TRAP::Graphics::QueueType::Compute);
        barrier.Texture = *m_compTex;
        TRAP::Graphics::RenderCommand::TextureBarrier(barrier, TRAP::Graphics::QueueType::Compute);

        //ALWAYS Bind descriptors (textures, buffers, etc) before binding the shader (pipeline)!
        //else you will get a black texture (learned this the hard way, thx NVIDIA driver :C)
        shader->UseTexture(1u, 0u, *m_colTex);
        shader->UseTexture(1u, 1u, *m_compTex);
        shader->Use();

        //Set push constants
        static constexpr f32 brightness = 1.0f;
        TRAP::Graphics::RenderCommand::SetPushConstants("BrightnessRootConstant", brightness,
                                                        TRAP::Graphics::QueueType::Compute);

        //Dispatch work
        TRAP::Graphics::RenderCommand::Dispatch({m_compTex->GetWidth(), m_compTex->GetHeight(), 1u});

        //Transition textures (to use as sampled images)
        barrier.CurrentState = TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess;
        barrier.NewState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
        barrier.Texture = *m_colTex;
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
    if(m_disabled)
        texShader->UseTexture(1u, 0u, *m_colTex);
    else
        texShader->UseTexture(1u, 0u, *m_compTex);
    texShader->Use();

    //Render Quad
    TRAP::Graphics::RenderCommand::DrawIndexed(m_indexBuffer->GetCount());

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
    ImGui::PlotLines("##frametimeHistory", m_frameTimeHistory.data(), NumericCast<i32>(m_frameTimeHistory.size()),
                     0, nullptr, 0.0f, 33.0f, ImVec2(200.0f, 50.0f));
    ImGui::Separator();

    ComputeShader selectedComputeShader = ComputeShader::Off;
    if(DrawComputeShaderComboBox(selectedComputeShader))
    {
        m_reset = true;

        m_disabled = false;
        m_sharpen = false;
        m_emboss = false;
        m_edgedetect = false;

        switch(selectedComputeShader)
        {
        case ComputeShader::Off:
            m_disabled = true;
            break;
        case ComputeShader::Sharpen:
            m_sharpen = true;
            break;
        case ComputeShader::Emboss:
            m_emboss = true;
            break;
        case ComputeShader::EdgeDetection:
            m_edgedetect = true;
            break;
        }
    }
    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

bool ComputeTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
{
    if(e.GetKey() == TRAP::Input::Key::Escape)
        TRAP::Application::Shutdown();

    return false;
}

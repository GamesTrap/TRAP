#include "VulkanTests.h"

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/Objects/CommandBuffer.h"
#include "Graphics/API/Objects/Pipeline.h"
#include "Graphics/API/Objects/PipelineCache.h"
#include "Graphics/API/Objects/RenderTarget.h"
#include "Graphics/API/Objects/RootSignature.h"
#include "Graphics/API/Objects/SwapChain.h"

VulkanTests::VulkanTests()
	: Layer("VulkanTests"),
	  m_window(nullptr),
	  m_defaultShader(nullptr),
	  m_defaultPipeline(nullptr),
	  m_defaultPipelineCache(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Vulkan Test");

	TRAP::VFS::MountShaders("Assets/Shaders");
	
	TRAP::Application::GetWindow()->SetTitle("Vulkan Multi-Window Test");

	TRAP::WindowProps windowProps
	{
		"Vulkan Test",
		200,
		200,
		60,
		false,
		TRAP::Window::DisplayMode::Windowed,
		TRAP::WindowProps::AdvancedProps
		{
			true,
			false,
			true,
			true,
			true,
			true,
			false,
			TRAP::Window::CursorMode::Normal
		},
		0
	};
	m_window = TRAP::MakeScope<TRAP::Window>(windowProps);
	m_window->SetEventCallback([this](TRAP::Events::Event& e) { OnEvent(e); });
	TRAP::Graphics::RendererAPI::GetRenderer()->SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f }, m_window.get());

	//TODO
	TRAP::Graphics::RendererAPI::PipelineCacheLoadDesc cacheDesc{};
	cacheDesc.VirtualOrPhysicalPath = "Assets/Cache/VkClearScreen.cache";
	m_defaultPipelineCache = TRAP::Graphics::PipelineCache::Create(cacheDesc);

	m_defaultShader = TRAP::Graphics::ShaderManager::LoadFile("/shaders/test.shader").get();
	
	//RootSignature
	TRAP::Graphics::RendererAPI::RootSignatureDesc rootDesc{};
	rootDesc.Shaders = { m_defaultShader };
	m_rootSignature = TRAP::Graphics::RootSignature::Create(rootDesc);
	///////////////////////////////////////////////////////

	TRAP::Graphics::RendererAPI::VertexLayout vertexLayout{};
	vertexLayout.AttributeCount = 2;
	vertexLayout.Attributes[0].Semantic = TRAP::Graphics::RendererAPI::ShaderSemantic::Position;
	vertexLayout.Attributes[0].Format = TRAP::Graphics::RendererAPI::ImageFormat::R32G32B32_SFLOAT;
	vertexLayout.Attributes[0].Binding = 0;
	vertexLayout.Attributes[0].Location = 0;
	vertexLayout.Attributes[0].Offset = 0;
	vertexLayout.Attributes[1].Semantic = TRAP::Graphics::RendererAPI::ShaderSemantic::Color;
	vertexLayout.Attributes[1].Format = TRAP::Graphics::RendererAPI::ImageFormat::R32G32B32_SFLOAT;
	vertexLayout.Attributes[1].Binding = 0;
	vertexLayout.Attributes[1].Location = 1;
	vertexLayout.Attributes[1].Offset = 3 * sizeof(float);

	TRAP::Ref<TRAP::Graphics::SwapChain> swapChain = TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentSwapChain(TRAP::Application::GetWindow().get());
	
	//TODO
	TRAP::Graphics::RendererAPI::PipelineDesc desc{};
	if (m_defaultPipelineCache)
		desc.Cache = m_defaultPipelineCache;
	desc.Type = TRAP::Graphics::RendererAPI::PipelineType::Graphics;
	desc.Pipeline = TRAP::Graphics::RendererAPI::GraphicsPipelineDesc();
	TRAP::Graphics::RendererAPI::GraphicsPipelineDesc& pipelineSettings = std::get<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>(desc.Pipeline);
	pipelineSettings.PrimitiveTopology = TRAP::Graphics::RendererAPI::PrimitiveTopology::TriangleList;
	pipelineSettings.RenderTargetCount = 1;
	pipelineSettings.ColorFormats = { swapChain->GetRenderTargets()[0]->GetImageFormat(), swapChain->GetRenderTargets()[1]->GetImageFormat(), swapChain->GetRenderTargets()[2]->GetImageFormat() };
	pipelineSettings.SampleCount = swapChain->GetRenderTargets()[0]->GetSampleCount();
	pipelineSettings.SampleQuality = swapChain->GetRenderTargets()[0]->GetSampleQuality();
	pipelineSettings.RootSignature = m_rootSignature;
	pipelineSettings.ShaderProgram = m_defaultShader;
	pipelineSettings.VertexLayout = &vertexLayout;
	m_defaultPipeline = TRAP::Graphics::Pipeline::Create(desc);
	////////////////////////////////////////////////////////////

	//Triangle Vertex Buffer
	TRAP::Graphics::RendererAPI::BufferLoadDesc loadDesc{};
	loadDesc.Desc.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::VertexBuffer;
	loadDesc.Desc.MemoryUsage = TRAP::Graphics::RendererAPI::ResourceMemoryUsage::GPUOnly;
	loadDesc.Desc.Size = m_triangleVertices.size() * sizeof(float);
	loadDesc.Data = m_triangleVertices.data();
	TRAP::Graphics::RendererAPI::GetResourceLoader()->AddResource(loadDesc, nullptr);

	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
	
	m_triangleVertexBuffer = loadDesc.Buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnDetach()
{
	m_window.reset();
	
	m_triangleVertexBuffer.reset();
	
	m_defaultPipeline.reset(); //TODO
	m_rootSignature.reset(); //TODO

	//TODO
	if (m_defaultPipelineCache)
	{
		m_defaultPipelineCache->Save("Assets/Cache/VkClearScreen.cache");
		m_defaultPipelineCache.reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	if (m_window)
		TRAP::Graphics::RenderCommand::Present(m_window);

	TRAP::Graphics::CommandBuffer* cmd = TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGraphicCommandBuffer(TRAP::Application::GetWindow().get());

	if(cmd)
	{
		TRAP::Graphics::RendererAPI::GetRenderer()->Clear(TRAP::Graphics::RendererAPI::ClearFlags::Color, TRAP::Graphics::RendererAPI::ClearValue{ {0.0f, 1.0f, 0.0f, 1.0f} });

		cmd->BindPipeline(m_defaultPipeline);
		cmd->BindVertexBuffer({ m_triangleVertexBuffer }, { 6 * sizeof(float) }, {});
		cmd->Draw(3, 0);
	}

	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[Sandbox] FPS: ", TRAP::Graphics::Renderer::GetFPS());
		TP_INFO("[Sandbox] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::WindowCloseEvent>([this](TRAP::Events::WindowCloseEvent& e) { return OnWindowClose(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanTests::OnWindowClose(TRAP::Events::WindowCloseEvent& e)
{
	if (e.GetTitle() == m_window->GetTitle())
		m_window.reset();
	
	return true;
}
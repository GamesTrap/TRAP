#include "VulkanTests.h"

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/Objects/CommandBuffer.h"
#include "Graphics/API/Objects/CommandPool.h"
#include "Graphics/API/Objects/Fence.h"
#include "Graphics/API/Objects/Pipeline.h"
#include "Graphics/API/Objects/PipelineCache.h"
#include "Graphics/API/Objects/Queue.h"
#include "Graphics/API/Objects/RenderTarget.h"
#include "Graphics/API/Objects/RootSignature.h"
#include "Graphics/API/Objects/Semaphore.h"
#include "Graphics/API/Objects/SwapChain.h"

VulkanTests::VulkanTests()
	: Layer("VulkanTests"),
	  m_graphicsQueue(nullptr),
	  m_cmds(),
	  m_swapChain(nullptr),
	  m_imageAcquiredSemaphore(nullptr),
	  m_defaultShader(nullptr),
	  m_defaultPipeline(nullptr),
	  m_defaultPipelineCache(nullptr),
	  m_frameIndex(0),
	  m_resChanged(false)//, m_window(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Vulkan Clear Screen Test");

	TRAP::VFS::MountShaders("Assets/Shaders");
	
	//TRAP::Application::GetWindow()->SetTitle("Vulkan Multi-Window Test");

	/*TRAP::WindowProps windowProps
	{
		"Vulkan Test",
		200,
		200,
		60,
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
	m_window->SetEventCallback([this](TRAP::Events::Event& e) { OnEvent(e); });*/
	
	//Vulkan Initialization
	TRAP::Graphics::RendererAPI::QueueDesc queueDesc{};
	queueDesc.Type = TRAP::Graphics::RendererAPI::QueueType::Graphics;
	queueDesc.Flag = TRAP::Graphics::RendererAPI::QueueFlag::InitMicroprofile;
	m_graphicsQueue = TRAP::Graphics::Queue::Create(queueDesc);

	for(uint32_t i = 0; i < ImageCount; ++i)
	{
		m_cmdPools[i] = TRAP::Graphics::CommandPool::Create({ m_graphicsQueue, false });
		m_cmds[i] = m_cmdPools[i]->AllocateCommandBuffer(false);

		m_renderCompleteFences[i] = TRAP::Graphics::Fence::Create();
		m_renderCompleteSemaphores[i] = TRAP::Graphics::Semaphore::Create();
	}
	m_imageAcquiredSemaphore = TRAP::Graphics::Semaphore::Create();

	TRAP::Graphics::RendererAPI::PipelineCacheLoadDesc cacheDesc{};
	cacheDesc.VirtualOrPhysicalPath = "Assets/Cache/VkClearScreen.cache";
	m_defaultPipelineCache = TRAP::Graphics::PipelineCache::Create(cacheDesc);

	m_defaultShader = TRAP::Graphics::ShaderManager::LoadFile("/shaders/test.shader").get();
	
	//RootSignature
	TRAP::Graphics::RendererAPI::RootSignatureDesc rootDesc{};
	rootDesc.Shaders = { m_defaultShader };
	m_rootSignature = TRAP::Graphics::RootSignature::Create(rootDesc);
	
	TRAP::Graphics::RendererAPI::SwapChainDesc swapChainDesc{};
	swapChainDesc.Window = TRAP::Application::GetWindow().get();
	swapChainDesc.PresentQueues = { m_graphicsQueue };
	swapChainDesc.Width = TRAP::Application::GetWindow()->GetWidth();
	swapChainDesc.Height = TRAP::Application::GetWindow()->GetHeight();
	swapChainDesc.ImageCount = ImageCount;
	swapChainDesc.ColorFormat = TRAP::Graphics::SwapChain::GetRecommendedSwapchainFormat(false);
	swapChainDesc.EnableVSync = false;
	swapChainDesc.ColorClearValue = { {0.1f, 0.1f, 0.1f, 1.0f} };
	m_swapChain = TRAP::Graphics::SwapChain::Create(swapChainDesc);
	
	if (!m_swapChain)
		TRAP::Application::Shutdown();

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
	
	TRAP::Graphics::RendererAPI::PipelineDesc desc{};
	if (m_defaultPipelineCache)
		desc.Cache = m_defaultPipelineCache;
	desc.Type = TRAP::Graphics::RendererAPI::PipelineType::Graphics;
	desc.Pipeline = TRAP::Graphics::RendererAPI::GraphicsPipelineDesc();
	TRAP::Graphics::RendererAPI::GraphicsPipelineDesc& pipelineSettings = std::get<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>(desc.Pipeline);
	pipelineSettings.PrimitiveTopology = TRAP::Graphics::RendererAPI::PrimitiveTopology::TriangleList;
	pipelineSettings.RenderTargetCount = 1;
	pipelineSettings.ColorFormats = { m_swapChain->GetRenderTargets()[0]->GetImageFormat(), m_swapChain->GetRenderTargets()[1]->GetImageFormat(), m_swapChain->GetRenderTargets()[2]->GetImageFormat() };
	pipelineSettings.SampleCount = m_swapChain->GetRenderTargets()[0]->GetSampleCount();
	pipelineSettings.SampleQuality = m_swapChain->GetRenderTargets()[0]->GetSampleQuality();
	pipelineSettings.RootSignature = m_rootSignature;
	pipelineSettings.ShaderProgram = m_defaultShader;
	pipelineSettings.VertexLayout = &vertexLayout;
	m_defaultPipeline = TRAP::Graphics::Pipeline::Create(desc);

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
	//m_window.reset();

	m_graphicsQueue->WaitQueueIdle();
	
	m_triangleVertexBuffer.reset();
	
	m_defaultPipeline.reset();
	m_swapChain.reset();
	m_rootSignature.reset();
	m_imageAcquiredSemaphore.reset();

	for(uint32_t i = ImageCount - 1; i > 0; i--)
	{
		m_renderCompleteSemaphores[i].reset();
		m_renderCompleteFences[i].reset();
		m_cmdPools[i]->FreeCommandBuffer(m_cmds[i]);
		m_cmdPools[i].reset();
	}

	m_graphicsQueue.reset();

	if (m_defaultPipelineCache)
	{
		m_defaultPipelineCache->Save("Assets/Cache/VkClearScreen.cache");
		m_defaultPipelineCache.reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//TRAP::Window::Use();
	
	//Render
	//TRAP::Graphics::RenderCommand::SetClearColor();
	//TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);

	/*if (m_window)
	{
		TRAP::Window::Use(m_window);
		
		//Render
		TRAP::Graphics::RenderCommand::SetClearColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);

		TRAP::Window::Use();
		
		TRAP::Graphics::RenderCommand::Present(m_window);
	}*/

	if(m_resChanged)
	{
		m_swapChain.reset();

		TRAP::Graphics::RendererAPI::SwapChainDesc swapChainDesc{};
		swapChainDesc.Window = TRAP::Application::GetWindow().get();
		swapChainDesc.PresentQueues = { m_graphicsQueue };
		swapChainDesc.Width = TRAP::Application::GetWindow()->GetWidth();
		swapChainDesc.Height = TRAP::Application::GetWindow()->GetHeight();
		swapChainDesc.ImageCount = ImageCount;
		swapChainDesc.ColorFormat = TRAP::Graphics::SwapChain::GetRecommendedSwapchainFormat(false);
		swapChainDesc.EnableVSync = false;
		m_swapChain = TRAP::Graphics::SwapChain::Create(swapChainDesc);

		if (!m_swapChain)
			TRAP::Application::Shutdown();

		m_resChanged = false;
	}

	const uint32_t swapChainImageIndex = m_swapChain->AcquireNextImage(m_imageAcquiredSemaphore, nullptr);

	const TRAP::Ref<TRAP::Graphics::RenderTarget> renderTarget = m_swapChain->GetRenderTargets()[swapChainImageIndex];
	const TRAP::Ref<TRAP::Graphics::Semaphore> renderCompleteSemaphore = m_renderCompleteSemaphores[m_frameIndex];
	TRAP::Ref<TRAP::Graphics::Fence> renderCompleteFence = m_renderCompleteFences[m_frameIndex];

	//Stall if CPU is running "Swap Chain Buffer Count" frames ahead of GPU
	if (renderCompleteFence->GetStatus() == TRAP::Graphics::RendererAPI::FenceStatus::Incomplete)
		renderCompleteFence->Wait();

	//Reset cmd pool for this frame
	m_cmdPools[m_frameIndex]->Reset();

	TRAP::Graphics::CommandBuffer* cmd = m_cmds[m_frameIndex];
	cmd->Begin();
	
	TRAP::Graphics::RendererAPI::RenderTargetBarrier barrier = { renderTarget, TRAP::Graphics::RendererAPI::ResourceState::Present, TRAP::Graphics::RendererAPI::ResourceState::RenderTarget };
	cmd->ResourceBarrier({}, {}, { barrier });

	TRAP::Graphics::RendererAPI::LoadActionsDesc loadActions{};
	loadActions.LoadActionsColor[0] = TRAP::Graphics::RendererAPI::LoadActionType::Clear;
	loadActions.ClearColorValues[0].R = 0.1f;
	loadActions.ClearColorValues[0].G = 0.1f;
	loadActions.ClearColorValues[0].B = 0.1f;
	loadActions.ClearColorValues[0].A = 1.0f;
	cmd->BindRenderTargets({ renderTarget }, nullptr, &loadActions, {}, {}, -1, -1);
	cmd->SetViewport(0.0f, 0.0f, static_cast<float>(renderTarget->GetWidth()), static_cast<float>(renderTarget->GetHeight()), 0.0f, 1.0f);
	cmd->SetScissor(0, 0, renderTarget->GetWidth(), renderTarget->GetHeight());

	cmd->BindPipeline(m_defaultPipeline);
	cmd->BindVertexBuffer({ m_triangleVertexBuffer }, { 6 * sizeof(float) }, {});
	cmd->Draw(3, 0);
	
	cmd->BindRenderTargets({}, nullptr, nullptr, {}, {}, -1, -1);

	cmd->SetViewport(0.0f, 0.0f, static_cast<float>(TRAP::Application::GetWindow()->GetWidth()), static_cast<float>(TRAP::Application::GetWindow()->GetHeight()), 0.0f, 1.0f);
	cmd->SetScissor(0, 0, TRAP::Application::GetWindow()->GetWidth(), TRAP::Application::GetWindow()->GetHeight());

	barrier = { renderTarget, TRAP::Graphics::RendererAPI::ResourceState::RenderTarget, TRAP::Graphics::RendererAPI::ResourceState::Present };
	cmd->ResourceBarrier({}, {}, { barrier });
	
	cmd->End();

	TRAP::Graphics::RendererAPI::QueueSubmitDesc submitDesc{};
	submitDesc.Cmds = { cmd };
	submitDesc.SignalSemaphores = { renderCompleteSemaphore };
	submitDesc.WaitSemaphores = { m_imageAcquiredSemaphore };
	submitDesc.SignalFence = renderCompleteFence;
	m_graphicsQueue->Submit(submitDesc);

	TRAP::Graphics::RendererAPI::QueuePresentDesc presentDesc{};
	presentDesc.Index = static_cast<uint8_t>(swapChainImageIndex);
	presentDesc.WaitSemaphores = { renderCompleteSemaphore };
	presentDesc.SwapChain = m_swapChain;
	presentDesc.SubmitDone = true;
	if (m_graphicsQueue->Present(presentDesc) != TRAP::Graphics::RendererAPI::PresentStatus::Success)
		TRAP::Application::Shutdown();

	m_frameIndex = (m_frameIndex + 1) % ImageCount;

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
	//dispatcher.Dispatch<TRAP::Events::WindowCloseEvent>([this](TRAP::Events::WindowCloseEvent& e) { return OnWindowClose(e); });
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>([this](TRAP::Events::FrameBufferResizeEvent& e) { return OnWindowResize(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

/*bool VulkanTests::OnWindowClose(TRAP::Events::WindowCloseEvent& e)
{
	if (e.GetTitle() == m_window->GetTitle())
		m_window.reset();
	
	return true;
}*/

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanTests::OnWindowResize(TRAP::Events::FrameBufferResizeEvent& e)
{
	m_resChanged = true;
	
	return false;
}
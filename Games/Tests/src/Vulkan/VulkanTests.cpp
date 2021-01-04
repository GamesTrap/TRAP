#include "VulkanTests.h"

#include "Graphics/API/Vulkan/Objects/VulkanRootSignature.h"
#include "Graphics/API/Vulkan/Objects/VulkanPipeline.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Objects/VulkanShader.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"
#include "Graphics/API/Vulkan/Objects/VulkanFence.h"
#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Objects/VulkanCommandPool.h"
#include "Graphics/API/Vulkan/Objects/VulkanRenderTarget.h"

VulkanTests::VulkanTests()
	: Layer("VulkanTests"),
	  m_graphicsQueue(nullptr),
	  m_cmds(),
	  m_swapChain(nullptr),
	  m_imageAcquiredSemaphore(nullptr),
	  m_defaultShader(nullptr),
	  m_defaultPipeline(nullptr),
	  m_frameIndex(0) //, m_window(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Vulkan Clear Screen Test");
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

	auto* vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>(TRAP::Graphics::RendererAPI::GetRenderer().get());
	
	//Vulkan Initialization
	TRAP::Graphics::RendererAPI::QueueDesc queueDesc{};
	queueDesc.Type = TRAP::Graphics::RendererAPI::QueueType::Graphics;
	queueDesc.Flag = TRAP::Graphics::RendererAPI::QueueFlag::InitMicroprofile;
	m_graphicsQueue = TRAP::MakeRef<TRAP::Graphics::API::VulkanQueue>(vkRenderer->GetDevice(), queueDesc);

	for(uint32_t i = 0; i < ImageCount; ++i)
	{
		m_cmdPools[i] = TRAP::MakeRef<TRAP::Graphics::API::VulkanCommandPool>(TRAP::Graphics::RendererAPI::CommandPoolDesc{ m_graphicsQueue, false });
		m_cmds[i] = m_cmdPools[i]->AllocateCommandBuffer(false);

		m_renderCompleteFences[i] = TRAP::MakeRef<TRAP::Graphics::API::VulkanFence>(vkRenderer->GetDevice());
		m_renderCompleteSemaphores[i] = TRAP::MakeRef<TRAP::Graphics::API::VulkanSemaphore>(vkRenderer->GetDevice());
	}
	m_imageAcquiredSemaphore = TRAP::MakeRef<TRAP::Graphics::API::VulkanSemaphore>(vkRenderer->GetDevice());

	const std::vector<uint8_t> vertSource = TRAP::VFS::ReadFile("Assets/Shaders/test.vert.spv");
	const std::vector<uint8_t> fragSource = TRAP::VFS::ReadFile("Assets/Shaders/test.frag.spv");
	TRAP::Graphics::RendererAPI::BinaryShaderDesc shaderDesc{};
	shaderDesc.Vertex = TRAP::Graphics::RendererAPI::BinaryShaderStageDesc{ vertSource, "main" };
	shaderDesc.Fragment = TRAP::Graphics::RendererAPI::BinaryShaderStageDesc{ fragSource, "main" };
	shaderDesc.Stages = TRAP::Graphics::RendererAPI::ShaderStage::Vertex | TRAP::Graphics::RendererAPI::ShaderStage::Fragment;
	m_defaultShader = TRAP::MakeRef<TRAP::Graphics::API::VulkanShader>(vkRenderer->GetDevice(), shaderDesc);

	//RootSignature
	TRAP::Graphics::RendererAPI::RootSignatureDesc rootDesc{};
	rootDesc.Shaders = { m_defaultShader };
	m_rootSignature = TRAP::MakeRef<TRAP::Graphics::API::VulkanRootSignature>(rootDesc);
	
	TRAP::Graphics::RendererAPI::SwapChainDesc swapChainDesc{};
	swapChainDesc.WindowHandle = static_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>(TRAP::Application::GetWindow()->GetInternalWindow());
	swapChainDesc.PresentQueues = { m_graphicsQueue };
	swapChainDesc.Width = TRAP::Application::GetWindow()->GetWidth();
	swapChainDesc.Height = TRAP::Application::GetWindow()->GetHeight();
	swapChainDesc.ImageCount = ImageCount;
	swapChainDesc.ColorFormat = TRAP::Graphics::API::GetRecommendedSwapchainFormat(false);
	swapChainDesc.EnableVSync = false;
	m_swapChain = TRAP::MakeRef<TRAP::Graphics::API::VulkanSwapChain>(vkRenderer->GetInstance(), vkRenderer->GetDevice(), vkRenderer->GetVMA(), swapChainDesc);
	
	if (!m_swapChain)
		TRAP::Application::Shutdown();

	TRAP::Graphics::RendererAPI::PipelineDesc desc{};
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
	m_defaultPipeline = TRAP::MakeRef<TRAP::Graphics::API::VulkanPipeline>(vkRenderer->GetDevice(), desc);
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnDetach()
{
	//m_window.reset();

	m_graphicsQueue->WaitQueueIdle();

	m_defaultPipeline.reset();
	m_defaultPipeline = nullptr;
	
	m_swapChain.reset();
	m_swapChain = nullptr;

	m_rootSignature.reset(); //Not deleting (reference count)
	m_rootSignature = nullptr;
	
	m_defaultShader.reset(); //Not deleting (reference count)
	m_defaultShader = nullptr;

	m_imageAcquiredSemaphore.reset();
	m_imageAcquiredSemaphore = nullptr;

	for(uint32_t i = ImageCount - 1; i > 0; i--)
	{
		m_renderCompleteSemaphores[i].reset();
		m_renderCompleteSemaphores[i] = nullptr;

		m_renderCompleteFences[i].reset();
		m_renderCompleteFences[i] = nullptr;

		m_cmdPools[i]->FreeCommandBuffer(m_cmds[i]);
		m_cmds[i] = nullptr;

		m_cmdPools[i].reset();
		m_cmdPools[i] = nullptr;
	}

	m_graphicsQueue.reset();
	m_graphicsQueue = nullptr;
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

	uint32_t swapChainImageIndex = m_swapChain->AcquireNextImage(m_imageAcquiredSemaphore, nullptr);

	TRAP::Ref<TRAP::Graphics::API::VulkanRenderTarget> renderTarget = m_swapChain->GetRenderTargets()[swapChainImageIndex];
	TRAP::Ref<TRAP::Graphics::API::VulkanSemaphore> renderCompleteSemaphore = m_renderCompleteSemaphores[m_frameIndex];
	TRAP::Ref<TRAP::Graphics::API::VulkanFence> renderCompleteFence = m_renderCompleteFences[m_frameIndex];

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
	loadActions.ClearColorValues[0].R = 0.5f;
	loadActions.ClearColorValues[0].G = 0.75f;
	loadActions.ClearColorValues[0].B = 1.0f;
	loadActions.ClearColorValues[0].A = 1.0f;
	cmd->BindRenderTargets({ renderTarget }, nullptr, &loadActions, {}, {}, -1, -1);
	cmd->SetViewport(0.0f, 0.0f, static_cast<float>(renderTarget->GetWidth()), static_cast<float>(renderTarget->GetHeight()), 0.0f, 1.0f);
	cmd->SetScissor(0, 0, renderTarget->GetWidth(), renderTarget->GetHeight());

	cmd->BindPipeline(m_defaultPipeline);
	
	cmd->BindRenderTargets({}, nullptr, nullptr, {}, {}, -1, -1);

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
	m_graphicsQueue->Present(presentDesc);

	m_frameIndex = (m_frameIndex + 1) % ImageCount;

	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[Sandbox] FPS: ", TRAP::Graphics::Renderer::GetFPS());
		TP_INFO("[Sandbox] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

/*void VulkanTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::WindowCloseEvent>([this](TRAP::Events::WindowCloseEvent& e) { return OnWindowClose(e); });
}*/

//-------------------------------------------------------------------------------------------------------------------//

/*bool VulkanTests::OnWindowClose(TRAP::Events::WindowCloseEvent& e)
{
	if (e.GetTitle() == m_window->GetTitle())
		m_window.reset();
	
	return true;
}*/
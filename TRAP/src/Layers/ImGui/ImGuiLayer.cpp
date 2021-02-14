#include "TRAPPCH.h"
#include "ImGuiLayer.h"

#include <examples/imgui_impl_vulkan.h>

#include "Application.h"
#include "Embed.h"
#include "Window/WindowingAPI.h"
#include "ImGuiWindowing.h"
#include "Graphics/API/Objects/CommandBuffer.h"
#include "Graphics/API/Objects/CommandPool.h"
#include "Graphics/API/Objects/Fence.h"
#include "Graphics/API/Objects/PipelineCache.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Objects/VulkanDevice.h"
#include "Graphics/API/Objects/Queue.h"
#include "Graphics/API/Vulkan/Objects/VulkanCommandBuffer.h"
#include "Graphics/API/Vulkan/Objects/VulkanInits.h"
#include "Graphics/API/Vulkan/Objects/VulkanInstance.h"
#include "Graphics/API/Vulkan/Objects/VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/Objects/VulkanPipelineCache.h"
#include "VFS/VFS.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImGuiLayer::ImGuiLayer()
	: Layer("ImGuiLayer"), m_blockEvents(true), m_imguiPipelineCache(nullptr), m_imguiDescriptorPool(nullptr)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnAttach()
{
	TP_PROFILE_FUNCTION();

	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //Enable Multi-Viewport / Platform Windows

	const auto contentScale = Application::GetWindow()->GetContentScale();
	float scaleFactor = 1.0f;
	if (contentScale.x > 1.0f || contentScale.y > 1.0f)
		scaleFactor = contentScale.x;

	ImFontConfig fontConfig;
	fontConfig.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromMemoryTTF(Embed::OpenSansBoldTTFData.data(), static_cast<int32_t>(Embed::OpenSansBoldTTFData.size()), scaleFactor * 18.0f, &fontConfig);
	io.FontDefault = io.Fonts->AddFontFromMemoryTTF(Embed::OpenSansTTFData.data(), static_cast<int32_t>(Embed::OpenSansTTFData.size()), scaleFactor * 18.0f, &fontConfig);
	
	//Setup Dear ImGui style
	ImGui::StyleColorsDark();

	//When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to
	ImGuiStyle& style = ImGui::GetStyle();

	style.ScaleAllSizes(scaleFactor);
	
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
	}

	SetDarkThemeColors();

	INTERNAL::WindowingAPI::InternalWindow* window = static_cast<INTERNAL::WindowingAPI::InternalWindow*>(Application::GetWindow()->GetInternalWindow());

	//Setup Platform/Renderer bindings
	const TRAP::Scope<TRAP::Graphics::RendererAPI::PerWindowData>& winData = TRAP::Graphics::RendererAPI::GetMainWindowData();

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		TRAP::INTERNAL::ImGuiWindowing::InitForVulkan(window, true);
		
		TP_TRACE(Log::ImGuiPrefix, "Vulkan Init...");
		const TRAP::Graphics::API::VulkanRenderer* renderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>(TRAP::Graphics::RendererAPI::GetRenderer().get());

		VkDescriptorPoolCreateInfo poolInfo = Graphics::API::VulkanInits::DescriptorPoolCreateInfo(m_descriptorPoolSizes, 1000);
		VkCall(vkCreateDescriptorPool(renderer->GetDevice()->GetVkDevice(), &poolInfo, nullptr, &m_imguiDescriptorPool));
		
		TRAP::Graphics::RendererAPI::PipelineCacheLoadDesc cacheDesc{};
		cacheDesc.VirtualOrPhysicalPath = TRAP::VFS::GetTempFolderPath() + "TRAP/ImGui.cache";
		m_imguiPipelineCache = TRAP::Graphics::PipelineCache::Create(cacheDesc);
		
		//This initializes ImGui for Vulkan
		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = renderer->GetInstance()->GetVkInstance();
		initInfo.PhysicalDevice = renderer->GetDevice()->GetPhysicalDevice()->GetVkPhysicalDevice();
		initInfo.Device = renderer->GetDevice()->GetVkDevice();
		initInfo.QueueFamily = renderer->GetDevice()->GetGraphicsQueueFamilyIndex();
		initInfo.Queue = dynamic_cast<TRAP::Graphics::API::VulkanQueue*>(winData->GraphicQueue.get())->GetVkQueue();
		initInfo.PipelineCache = dynamic_cast<TRAP::Graphics::API::VulkanPipelineCache*>(m_imguiPipelineCache.get())->GetVkPipelineCache();
		initInfo.DescriptorPool = m_imguiDescriptorPool;
		initInfo.Allocator = nullptr;
		initInfo.MinImageCount = winData->ImageCount;
		initInfo.ImageCount = winData->ImageCount;
		initInfo.CheckVkResultFn = [](const VkResult res) {VkCall(res); };
		
		ImGui_ImplVulkan_Init(&initInfo, dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>(winData->GraphicCommandBuffers[winData->ImageIndex])->GetActiveVkRenderPass());

		//Execute a GPU command to upload ImGui font textures
		TRAP::Graphics::CommandBuffer* cmd = winData->GraphicCommandPools[winData->ImageIndex]->AllocateCommandBuffer(false);
		cmd->Begin();
		ImGui_ImplVulkan_CreateFontsTexture(dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>(cmd)->GetVkCommandBuffer());
		cmd->End();
		TRAP::Ref<TRAP::Graphics::Fence> submitFence = TRAP::Graphics::Fence::Create();
		TRAP::Graphics::RendererAPI::QueueSubmitDesc submitDesc{};
		submitDesc.Cmds = { cmd };
		submitDesc.SignalFence = submitFence;
		winData->GraphicQueue->Submit(submitDesc);
		submitFence->Wait();
		submitFence.reset();
		winData->GraphicCommandPools[winData->ImageIndex]->FreeCommandBuffer(cmd);

		//Clear font textures from CPU data
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		TP_TRACE(Log::ImGuiPrefix, "Vulkan Init Finished");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnDetach()
{
	TP_PROFILE_FUNCTION();

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		TP_TRACE(Log::ImGuiPrefix, "Vulkan Shutdown...");
		m_imguiPipelineCache->Save(TRAP::VFS::GetTempFolderPath() + "TRAP/ImGui.cache");
		m_imguiPipelineCache.reset();
		const TRAP::Graphics::API::VulkanRenderer* renderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>(TRAP::Graphics::RendererAPI::GetRenderer().get());
		vkDestroyDescriptorPool(renderer->GetDevice()->GetVkDevice(), m_imguiDescriptorPool, nullptr);
		TP_TRACE(Log::ImGuiPrefix, "Vulkan Shutdown Finished");
		
		ImGui_ImplVulkan_Shutdown();
		INTERNAL::ImGuiWindowing::Shutdown();
	}

	ImGui::DestroyContext();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnEvent(Events::Event& event)
{
	if (m_blockEvents)
	{
		ImGuiIO& io = ImGui::GetIO();
		event.Handled |= event.IsInCategory(Events::EventCategory::Mouse) & io.WantCaptureMouse;
		event.Handled |= event.IsInCategory(Events::EventCategory::Keyboard) & io.WantCaptureKeyboard;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::Begin()
{
	TP_PROFILE_FUNCTION();

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		ImGui_ImplVulkan_NewFrame();
		INTERNAL::ImGuiWindowing::NewFrame();
	}

	ImGui::NewFrame();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::End()
{
	TP_PROFILE_FUNCTION();

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(Application::GetWindow()->GetWidth()), static_cast<float>(Application::GetWindow()->GetHeight()));

	//Rendering
	ImGui::Render();
	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		const TRAP::Scope<TRAP::Graphics::RendererAPI::PerWindowData>& winData = TRAP::Graphics::RendererAPI::GetMainWindowData();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>(winData->GraphicCommandBuffers[winData->ImageIndex])->GetVkCommandBuffer());
	}

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::BlockEvents(const bool block)
{
	m_blockEvents = block;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::SetDarkThemeColors()
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);

	//Headers
	colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

	//Buttons
	colors[ImGuiCol_Button] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

	//Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

	//Tabs
	colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.3805f, 0.381f, 1.0f);
	colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.2805f, 0.281f, 1.0f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

	//Title BG
	colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
}
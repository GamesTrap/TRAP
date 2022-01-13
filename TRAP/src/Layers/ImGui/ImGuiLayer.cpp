#include "TRAPPCH.h"
#include "ImGuiLayer.h"

#include "ImGuiVulkanBackend.h"

#include "Application.h"
#include "Embed.h"
#include "Window/WindowingAPI.h"
#include "ImGuiWindowing.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureBase.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/CommandBuffer.h"
#include "Graphics/API/Objects/SwapChain.h"
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
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"
#include "Graphics/API/Vulkan/Objects/VulkanSampler.h"
#include "FS/FS.h"

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

	//Set imgui.ini path
	m_imguiIniPath = (TRAP::FS::GetDocumentsFolderPath() / "TRAP" / TRAP::Application::GetGameName() / "imgui.ini").generic_string();
	io.IniFilename = m_imguiIniPath.c_str();

	const auto contentScale = Application::GetWindow()->GetContentScale();
	float scaleFactor = 1.0f;
	if (contentScale.x > 1.0f || contentScale.y > 1.0f)
		scaleFactor = contentScale.x;

	ImFontConfig fontConfig;
	fontConfig.FontDataOwnedByAtlas = false;
	//While looking like UB Casting const void* to void* in this instance is okay because the memory will only be copied by ImGui because of the above line
	io.Fonts->AddFontFromMemoryTTF((void*)Embed::OpenSansBoldTTFData.data(),
	                               static_cast<int32_t>(Embed::OpenSansBoldTTFData.size()),
								   scaleFactor * 18.0f, &fontConfig);
	io.FontDefault = io.Fonts->AddFontFromMemoryTTF((void*)Embed::OpenSansTTFData.data(),
	                                                static_cast<int32_t>(Embed::OpenSansTTFData.size()),
													scaleFactor * 18.0f, &fontConfig);

	//Setup Dear ImGui style
	ImGui::StyleColorsDark();

	//When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to
	ImGuiStyle& style = ImGui::GetStyle();

	style.ScaleAllSizes(scaleFactor);

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		style.WindowRounding = 0.0f;

	SetDarkThemeColors();

	INTERNAL::WindowingAPI::InternalWindow* window = static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	(
		Application::GetWindow()->GetInternalWindow()
	);

	//Setup Platform/Renderer bindings
	const TRAP::Scope<TRAP::Graphics::RendererAPI::PerWindowData>& winData = TRAP::Graphics::RendererAPI::GetMainWindowData();

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		TRAP::INTERNAL::ImGuiWindowing::InitForVulkan(window, true);

		TP_TRACE(Log::ImGuiPrefix, "Vulkan init...");
		const TRAP::Graphics::API::VulkanRenderer* renderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>
		(
			TRAP::Graphics::RendererAPI::GetRenderer().get()
		);

		VkDescriptorPoolCreateInfo poolInfo = Graphics::API::VulkanInits::DescriptorPoolCreateInfo(m_descriptorPoolSizes,
		                                                                                           1000);
		VkCall(vkCreateDescriptorPool(renderer->GetDevice()->GetVkDevice(), &poolInfo, nullptr,
		                              &m_imguiDescriptorPool));

		TRAP::Graphics::RendererAPI::PipelineCacheLoadDesc cacheDesc{};
		cacheDesc.Path = TRAP::FS::GetGameTempFolderPath() / "ImGui.cache";
		m_imguiPipelineCache = TRAP::Graphics::PipelineCache::Create(cacheDesc);

		//This initializes ImGui for Vulkan
		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = renderer->GetInstance()->GetVkInstance();
		initInfo.PhysicalDevice = renderer->GetDevice()->GetPhysicalDevice()->GetVkPhysicalDevice();
		initInfo.Device = renderer->GetDevice()->GetVkDevice();
		initInfo.QueueFamily = renderer->GetDevice()->GetGraphicsQueueFamilyIndex();
		initInfo.Queue = dynamic_cast<TRAP::Graphics::API::VulkanQueue*>
		(
			TRAP::Graphics::RendererAPI::GetGraphicsQueue().get()
		)->GetVkQueue();
		initInfo.PipelineCache = dynamic_cast<TRAP::Graphics::API::VulkanPipelineCache*>
		(
			m_imguiPipelineCache.get()
		)->GetVkPipelineCache();
		initInfo.DescriptorPool = m_imguiDescriptorPool;
		initInfo.Allocator = nullptr;
		initInfo.MinImageCount = TRAP::Graphics::RendererAPI::ImageCount;
		initInfo.ImageCount = TRAP::Graphics::RendererAPI::ImageCount;
		initInfo.CheckVkResultFn = [](const VkResult res) {VkCall(res); };

		ImGui_ImplVulkan_Init(&initInfo, dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
		(
			winData->GraphicCommandBuffers[winData->ImageIndex]
		)->GetActiveVkRenderPass());

		ImGui_ImplVulkan_UploadFontsTexture();

		//Clear font textures from CPU data
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		TP_TRACE(Log::ImGuiPrefix, "Finished Vulkan init");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnDetach()
{
	TP_PROFILE_FUNCTION();

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		TP_TRACE(Log::ImGuiPrefix, "Vulkan shutdown...");
		m_imguiPipelineCache->Save(TRAP::FS::GetGameTempFolderPath() / "ImGui.cache");
		m_imguiPipelineCache.reset();
		const TRAP::Graphics::API::VulkanRenderer* renderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>
		(
			TRAP::Graphics::RendererAPI::GetRenderer().get()
		);
		vkDestroyDescriptorPool(renderer->GetDevice()->GetVkDevice(), m_imguiDescriptorPool, nullptr);
		TP_TRACE(Log::ImGuiPrefix, "Finished Vulkan shutdown");

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
		//Bind SwapChain RenderTarget if no RenderPass is active
		const auto& winData = TRAP::Graphics::RendererAPI::GetMainWindowData();
		const auto* vkCmdBuffer = dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
		(
			winData->GraphicCommandBuffers[winData->ImageIndex]
		);
		if(vkCmdBuffer->GetActiveVkRenderPass() == VK_NULL_HANDLE)
			TRAP::Graphics::RenderCommand::BindRenderTarget(winData->SwapChain->GetRenderTargets()[winData->ImageIndex]);
		ImGui_ImplVulkan_NewFrame();
	}

	if(Graphics::RendererAPI::GetRenderAPI() != Graphics::RenderAPI::Headless)
		INTERNAL::ImGuiWindowing::NewFrame();

	ImGui::NewFrame();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::End()
{
	TP_PROFILE_FUNCTION();

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(Application::GetWindow()->GetWidth()),
	                        static_cast<float>(Application::GetWindow()->GetHeight()));

	//Rendering
	ImGui::Render();
	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		const TRAP::Scope<TRAP::Graphics::RendererAPI::PerWindowData>& winData = TRAP::Graphics::RendererAPI::GetMainWindowData();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
		                                dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
										(
											winData->GraphicCommandBuffers[winData->ImageIndex]
										)->GetVkCommandBuffer());
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

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::Image(TRAP::Graphics::Texture2D* image, TRAP::Graphics::Sampler* sampler, const ImVec2& size,
                  const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
	{
		auto* vkImage = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(image->GetTexture().get());
		auto* vkSampler = dynamic_cast<TRAP::Graphics::API::VulkanSampler*>(sampler);
		ImTextureID texID = ImGui_ImplVulkan_AddTexture(vkSampler->GetVkSampler(), vkImage->GetSRVVkImageView(),
		                                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui::Image(texID, size, uv0, uv1, tint_col, border_col);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::Image(TRAP::Graphics::Texture2D* image, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1,
			      const ImVec4& tint_col, const ImVec4& border_col)
{
	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
	{
		auto* vkImage = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(image->GetTexture().get());
		ImTextureID texID = ImGui_ImplVulkan_AddTexture(TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultSampler->GetVkSampler(),
		                                                      vkImage->GetSRVVkImageView(),
		                                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui::Image(texID, size, uv0, uv1, tint_col, border_col);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::Image(TRAP::Graphics::TextureBase* image, TRAP::Graphics::Sampler* sampler, const ImVec2& size,
                  const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
	{
		auto* vkImage = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(image);
		auto* vkSampler = dynamic_cast<TRAP::Graphics::API::VulkanSampler*>(sampler);
		ImTextureID texID = ImGui_ImplVulkan_AddTexture(vkSampler->GetVkSampler(), vkImage->GetSRVVkImageView(),
		                                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui::Image(texID, size, uv0, uv1, tint_col, border_col);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::Image(TRAP::Graphics::TextureBase* image, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1,
			      const ImVec4& tint_col, const ImVec4& border_col)
{
	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
	{
		auto* vkImage = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(image);
		ImTextureID texID = ImGui_ImplVulkan_AddTexture(TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultSampler->GetVkSampler(),
		                                                      vkImage->GetSRVVkImageView(),
		                                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui::Image(texID, size, uv0, uv1, tint_col, border_col);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

ImFont* ImGui::AddFontFromFileTTF(const std::string_view filename, const float sizePixels,
							      const ImFontConfig* fontCfgTemplate, const ImWchar* glyphRanges)
{
	//Add font like normally
	ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.data(), sizePixels, fontCfgTemplate, glyphRanges);

	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		ImGui_ImplVulkan_UploadFontsTexture();

	//Clear font textures from CPU data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	return font;
}

//-------------------------------------------------------------------------------------------------------------------//

ImFont* ImGui::AddFontFromMemoryTTF(void* fontData, const int32_t fontSize, const float sizePixels,
								    const ImFontConfig* fontCfg, const ImWchar* glyphRanges)
{
	//Add font like normally
	ImFont* font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(fontData, fontSize, sizePixels, fontCfg, glyphRanges);

	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		ImGui_ImplVulkan_UploadFontsTexture();

	//Clear font textures from CPU data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	return font;
}
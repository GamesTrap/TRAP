#include "TRAPPCH.h"
#include "ImGuiLayer.h"

#include "ImGuiVulkanBackend.h"

#include "Application.h"
#include "Embed.h"
#include "Window/WindowingAPI.h"
#include "ImGuiWindowing.h"
#include "Graphics/Textures/Texture.h"
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
#include "FileSystem/FileSystem.h"

#include <imgui.h>
#include <imgui_internal.h>

//-------------------------------------------------------------------------------------------------------------------//

struct InputTextCallbackUserData
{
	std::string* Str;
	ImGuiInputTextCallback ChainCallback;
	void* ChainCallbackUserData;
};

//-------------------------------------------------------------------------------------------------------------------//

static int32_t InputTextCallback(ImGuiInputTextCallbackData* const data)
{
	const InputTextCallbackUserData* const userData = static_cast<InputTextCallbackUserData*>(data->UserData);
	if(data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		//Resize string callback
		//If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		std::string* str = userData->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = &(*str)[0];
	}
	else if(userData->ChainCallback)
	{
		//Forward to user callback, if any
		data->UserData = userData->ChainCallbackUserData;
		return userData->ChainCallback(data);
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImGuiLayer::ImGuiLayer()
	: Layer("ImGuiLayer"), m_blockEvents(true), m_imguiPipelineCache(nullptr), m_imguiDescriptorPool(nullptr)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnAttach()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //Enable Multi-Viewport / Platform Windows

	//Set imgui.ini path
	const auto docsFolder = TRAP::FileSystem::GetGameDocumentsFolderPath();
	if(docsFolder)
		m_imguiIniPath = (*docsFolder / "imgui.ini").u8string();
	else //Fallback
		m_imguiIniPath = "imgui.ini";
	io.IniFilename = m_imguiIniPath.c_str();

	const auto contentScale = Application::GetWindow()->GetContentScale();
	float scaleFactor = 1.0f;
	if (contentScale.x > 1.0f || contentScale.y > 1.0f)
		scaleFactor = contentScale.x;

	ImFontConfig fontConfig;
	fontConfig.FontDataOwnedByAtlas = false; //This makes the const_cast below safe.
	io.Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(Embed::OpenSansBoldTTFData.data()),
	                               static_cast<int32_t>(Embed::OpenSansBoldTTFData.size()),
								   scaleFactor * 18.0f, &fontConfig);
	io.FontDefault = io.Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(Embed::OpenSansTTFData.data()),
	                                                static_cast<int32_t>(Embed::OpenSansTTFData.size()),
													scaleFactor * 18.0f, &fontConfig);

	//Setup Dear ImGui style
	ImGui::StyleColorsDark();

	//When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to
	ImGuiStyle& style = ImGui::GetStyle();

	style.ScaleAllSizes(scaleFactor);

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	SetDarkThemeColors();

	INTERNAL::WindowingAPI::InternalWindow* const window = static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	(
		Application::GetWindow()->GetInternalWindow()
	);

	//Setup Platform/Renderer bindings
	const auto& winData = TRAP::Graphics::RendererAPI::GetWindowData(TRAP::Application::GetWindow());

	TP_TRACE(Log::ImGuiPrefix, "Init...");
	TRAP::INTERNAL::ImGuiWindowing::Init(window, true, Graphics::RendererAPI::GetRenderAPI());
	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		const TRAP::Graphics::API::VulkanRenderer* const renderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>
		(
			TRAP::Graphics::RendererAPI::GetRenderer()
		);

		VkDescriptorPoolCreateInfo poolInfo = Graphics::API::VulkanInits::DescriptorPoolCreateInfo(m_descriptorPoolSizes,
		                                                                                           1000);
		VkCall(vkCreateDescriptorPool(renderer->GetDevice()->GetVkDevice(), &poolInfo, nullptr,
		                              &m_imguiDescriptorPool));

		const auto tempFolder = TRAP::FileSystem::GetGameTempFolderPath();
		if(tempFolder)
		{
			TRAP::Graphics::RendererAPI::PipelineCacheLoadDesc cacheDesc{};
			cacheDesc.Path = *tempFolder / "ImGui.cache";
			m_imguiPipelineCache = TRAP::Graphics::PipelineCache::Create(cacheDesc);
		}
		else //Create empty cache as fallback
			m_imguiPipelineCache = TRAP::Graphics::PipelineCache::Create(TRAP::Graphics::RendererAPI::PipelineCacheDesc{});

		TRAP::Graphics::AntiAliasing aaMethod = TRAP::Graphics::AntiAliasing::Off;
		TRAP::Graphics::SampleCount aaSamples = TRAP::Graphics::SampleCount::One;
		TRAP::Graphics::RenderCommand::GetAntiAliasing(aaMethod, aaSamples);

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
		initInfo.Subpass = 0;
		initInfo.MinImageCount = TRAP::Graphics::RendererAPI::ImageCount;
		initInfo.ImageCount = TRAP::Graphics::RendererAPI::ImageCount;
		initInfo.MSAASamples = aaMethod == TRAP::Graphics::AntiAliasing::MSAA ? static_cast<VkSampleCountFlagBits>(aaSamples) : VK_SAMPLE_COUNT_1_BIT;
		initInfo.Allocator = nullptr;
		initInfo.CheckVkResultFn = [](const VkResult res) {VkCall(res); };

		ImGui_ImplVulkan_Init(&initInfo, dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
		(
			winData.GraphicCommandBuffers[winData.ImageIndex]
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
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		TP_TRACE(Log::ImGuiPrefix, "Vulkan shutdown...");
		Graphics::RendererAPI::GetRenderer()->WaitIdle();
		const auto tempFolder = TRAP::FileSystem::GetGameTempFolderPath();
		if(tempFolder)
			m_imguiPipelineCache->Save(*tempFolder / "ImGui.cache");
		m_imguiPipelineCache.reset();
		// const TRAP::Graphics::API::VulkanRenderer* const renderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>
		// (
		// 	TRAP::Graphics::RendererAPI::GetRenderer()
		// );
		if(m_imguiDescriptorPool)
		{
			//Gets cleard by ImGui_ImplVulkan_Shutdown()
			// vkDestroyDescriptorPool(renderer->GetDevice()->GetVkDevice(), m_imguiDescriptorPool, nullptr);
			m_imguiDescriptorPool = nullptr;
		}
		TP_TRACE(Log::ImGuiPrefix, "Finished Vulkan shutdown");

		ImGui_ImplVulkan_Shutdown();
		INTERNAL::ImGuiWindowing::Shutdown();
	}

	ImGui::DestroyContext();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnEvent(Events::Event& event)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (m_blockEvents)
	{
		const ImGuiIO& io = ImGui::GetIO();
		event.Handled |= event.IsInCategory(Events::EventCategory::Mouse) & io.WantCaptureMouse;
		event.Handled |= event.IsInCategory(Events::EventCategory::Keyboard) & io.WantCaptureKeyboard;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::SetMSAASamples(const uint32_t sampleCount)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
		ImGui_ImplVulkan_SetMSAASamples(static_cast<VkSampleCountFlagBits>(sampleCount));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::Begin()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		//Bind SwapChain RenderTarget if no RenderPass is active
		const auto& winData = TRAP::Graphics::RendererAPI::GetWindowData(TRAP::Application::GetWindow());
		const auto* const vkCmdBuffer = dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
		(
			winData.GraphicCommandBuffers[winData.ImageIndex]
		);
		if(vkCmdBuffer->GetActiveVkRenderPass() == VK_NULL_HANDLE)
		{
			TRAP::Graphics::AntiAliasing aaMethod = TRAP::Graphics::AntiAliasing::Off;
			TRAP::Graphics::SampleCount aaSamples = TRAP::Graphics::SampleCount::One;
			TRAP::Graphics::RenderCommand::GetAntiAliasing(aaMethod, aaSamples);

			if(aaMethod == TRAP::Graphics::RendererAPI::AntiAliasing::MSAA) //MSAA
				TRAP::Graphics::RenderCommand::BindRenderTarget(winData.SwapChain->GetRenderTargetsMSAA()[winData.ImageIndex]);
			else //No MSAA
				TRAP::Graphics::RenderCommand::BindRenderTarget(winData.SwapChain->GetRenderTargets()[winData.ImageIndex]);
		}
		ImGui_ImplVulkan_NewFrame();
	}

	INTERNAL::ImGuiWindowing::NewFrame();

	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::End()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(Application::GetWindow()->GetFrameBufferSize().x),
	                        static_cast<float>(Application::GetWindow()->GetFrameBufferSize().y));

	//Rendering
	ImGui::Render();
	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		const auto& winData = TRAP::Graphics::RendererAPI::GetWindowData(TRAP::Application::GetWindow());
		if(!Application::GetWindow()->IsMinimized())
		{
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
											dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
											(
												winData.GraphicCommandBuffers[winData.ImageIndex]
											)->GetVkCommandBuffer());
		}
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
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	m_blockEvents = block;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::SetDarkThemeColors()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

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

void ImGui::Image(TRAP::Ref<TRAP::Graphics::Texture> image, const TRAP::Graphics::Sampler* const sampler, const ImVec2& size,
                  const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	TRAP_ASSERT(image != nullptr, "Image is nullptr!");
	TRAP_ASSERT(image->GetType() == TRAP::Graphics::TextureType::Texture2D, "Image is not a Texture2D!");

	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
	{
		const auto vkImage = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(image);
		const auto* const vkSampler = dynamic_cast<const TRAP::Graphics::API::VulkanSampler*>(sampler);
		const ImTextureID texID = ImGui_ImplVulkan_AddTexture(vkSampler->GetVkSampler(), vkImage->GetSRVVkImageView(),
		                                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui::Image(texID, size, uv0, uv1, tint_col, border_col);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::Image(TRAP::Ref<TRAP::Graphics::Texture> image, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1,
			      const ImVec4& tint_col, const ImVec4& border_col)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	TRAP_ASSERT(image != nullptr, "Image is nullptr!");
	TRAP_ASSERT(image->GetType() == TRAP::Graphics::TextureType::Texture2D, "Image is not a Texture2D!");

	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
	{
		const auto vkImage = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(image);
		const ImTextureID texID = ImGui_ImplVulkan_AddTexture(TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultSampler->GetVkSampler(),
		                                                      vkImage->GetSRVVkImageView(),
		                                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui::Image(texID, size, uv0, uv1, tint_col, border_col);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGui::ImageButton(TRAP::Ref<TRAP::Graphics::Texture> image, const ImVec2& size, const ImVec2& uv0,
                        const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	TRAP_ASSERT(image != nullptr, "Image is nullptr!");
	TRAP_ASSERT(image->GetType() == TRAP::Graphics::TextureType::Texture2D, "Image is not a Texture2D!");

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
	{
		const auto vkImage = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(image);
		const ImTextureID texID = ImGui_ImplVulkan_AddTexture(TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultSampler->GetVkSampler(),
												              vkImage->GetSRVVkImageView(),
		                                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		VkImageView imgView = vkImage->GetSRVVkImageView();
		std::size_t imgViewHash = 0;
		TRAP::Utils::HashCombine(imgViewHash, imgView);
		const ImGuiID imgViewID = static_cast<uint32_t>(imgViewHash);

		return ImGui::ImageButtonEx(imgViewID, texID, size, uv0, uv1, ImVec2(frame_padding, frame_padding), bg_col, tint_col);
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGui::InputText(const std::string_view label, std::string* str, ImGuiInputTextFlags flags,
                      const ImGuiInputTextCallback callback, void* userData)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallbackUserData cbUserData{str, callback, userData};

	return InputText(label.data(), &(*str)[0], str->capacity() + 1, flags, InputTextCallback, &cbUserData);
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGui::InputTextMultiline(const std::string_view label, std::string* str, const ImVec2& size,
                               ImGuiInputTextFlags flags, const ImGuiInputTextCallback callback, void* userData)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallbackUserData cbUserData{str, callback, userData};

	return InputTextMultiline(label.data(), &(*str)[0], str->capacity() + 1, size, flags, InputTextCallback, &cbUserData);
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGui::InputTextWithHint(const std::string_view label, const std::string_view hint, std::string* str,
                              ImGuiInputTextFlags flags, const ImGuiInputTextCallback callback, void* userData)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallbackUserData cbUserData{str, callback, userData};

	return InputTextWithHint(label.data(), hint.data(), &(*str)[0], str->capacity() + 1, flags, InputTextCallback, &cbUserData);
}

//-------------------------------------------------------------------------------------------------------------------//

ImFont* ImGui::AddFontFromFileTTF(const std::string_view filename, const float sizePixels,
							      const ImFontConfig* const fontCfgTemplate, const ImWchar* const glyphRanges)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	//Add font like normally
	ImFont* const font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.data(), sizePixels, fontCfgTemplate, glyphRanges);

	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		ImGui_ImplVulkan_UploadFontsTexture();

	//Clear font textures from CPU data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	return font;
}

//-------------------------------------------------------------------------------------------------------------------//

ImFont* ImGui::AddFontFromMemoryTTF(void* fontData, const int32_t fontSize, const float sizePixels,
								    const ImFontConfig* const fontCfg, const ImWchar* const glyphRanges)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	//Add font like normally
	ImFont* const font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(fontData, fontSize, sizePixels, fontCfg, glyphRanges);

	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		ImGui_ImplVulkan_UploadFontsTexture();

	//Clear font textures from CPU data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	return font;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGui::IsInputEnabled()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const auto& io = ImGui::GetIO();
	return (io.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0 &&
	       (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::SetInputEnabled(const bool enable)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	auto& io = ImGui::GetIO();

	if(enable)
	{
		io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		io.ConfigFlags &= ~ImGuiConfigFlags_NavNoCaptureKeyboard;
	}
	else
	{
		io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
		io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
	}
}

#include "TRAPPCH.h"
#include "ImGuiLayer.h"

#ifndef TRAP_HEADLESS_MODE

#include "ImGuiVulkanBackend.h"

#include "Application.h"
#include "Embed.h"
#include "Utils/ErrorCodes/ErrorCodes.h"
#include "Window/WindowingAPI.h"
#include "ImGuiWindowing.h"
#include "Graphics/Textures/Texture.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/CommandBuffer.h"
#include "Graphics/API/Objects/SwapChain.h"
#include "Graphics/API/Objects/PipelineCache.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Objects/VulkanDevice.h"
#include "Graphics/API/Vulkan/Objects/VulkanCommandBuffer.h"
#include "Graphics/API/Vulkan/Objects/VulkanPipelineCache.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"
#include "Graphics/API/Vulkan/Objects/VulkanSampler.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"
#include "FileSystem/FileSystem.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace
{
	#ifdef _MSC_VER
	#pragma warning(disable: 4702)
	#endif /*_MSC_VER*/

	//Circumvent cert-err58-cpp
	[[nodiscard]] const std::vector<VkDescriptorPoolSize>& GlobalDescriptorPoolSizes()
	{
		try
		{
			static const std::vector<VkDescriptorPoolSize> DescriptorPoolSizes =
			{
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
			};

			return DescriptorPoolSizes;
		}
		catch (...)
		{
			TP_CRITICAL(TRAP::Log::ImGuiPrefix, "ImGuiVulkanBackend::GlobalDescriptorPoolSizes(): std::vector constructor threw an exception!");
			throw std::runtime_error("ImGuiVulkanBackend::GlobalDescriptorPoolSizes(): std::vector constructor threw an exception!");
			std::terminate();
		}
	}

	#ifdef _MSC_VER
	#pragma warning(default: 4702)
	#endif /*_MSC_VER*/
}

//-------------------------------------------------------------------------------------------------------------------//

struct InputTextCallbackUserData
{
	std::string* Str = nullptr;
	ImGuiInputTextCallback ChainCallback = nullptr;
	void* ChainCallbackUserData = nullptr;
};

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	[[nodiscard]] i32 InputTextCallback(ImGuiInputTextCallbackData* const data)
	{
		TRAP_ASSERT(data != nullptr, "ImGuiLayer::InputTextCallback(): data is nullptr!");
		if(data == nullptr)
			return 0;

		const InputTextCallbackUserData* const userData = static_cast<InputTextCallbackUserData*>(data->UserData);
		TRAP_ASSERT(userData != nullptr, "ImGuiLayer::InputTextCallback(): userData is nullptr!");
		if(userData == nullptr)
			return 0;

		if(data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			//Resize string callback
			//If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
			TRAP_ASSERT(userData->Str != nullptr, "ImGuiLayer::InputTextCallback(): userData->Str is nullptr!");
			std::string& str = *userData->Str;
			TRAP_ASSERT(data->Buf == str.c_str(), "ImGuiLayer::InputTextCallback(): String pointers data->Buf and str.c_str() aren't the same!");
			str.resize(NumericCast<usize>(data->BufTextLen));
			data->Buf = str.data();
		}
		else if(userData->ChainCallback != nullptr)
		{
			//Forward to user callback, if any
			data->UserData = userData->ChainCallbackUserData;
			return userData->ChainCallback(data);
		}

		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnAttach()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //Enable Docking
#ifdef TRAP_PLATFORM_WINDOWS
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //Enable Multi-Viewport / Platform Windows
#elif defined(TRAP_PLATFORM_LINUX)
	//On Wayland we are unable to track global window and mouse positions, so we disable multi-viewport support
	if(Utils::GetLinuxWindowManager() != Utils::LinuxWindowManager::Wayland)
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //Enable Multi-Viewport / Platform Windows
#endif

	//Set imgui.ini path
	const auto docsFolder = TRAP::FileSystem::GetGameDocumentsFolderPath();
	if(docsFolder)
		m_imguiIniPath = (*docsFolder / "imgui.ini").generic_string();
	else //Fallback
		m_imguiIniPath = "imgui.ini";
	io.IniFilename = m_imguiIniPath.c_str();

	const auto contentScale = Application::GetWindow()->GetContentScale();
	f32 scaleFactor = 1.0f;
	if (contentScale.x() > 1.0f || contentScale.y() > 1.0f)
		scaleFactor = contentScale.x();

	ImFontConfig fontConfig;
	fontConfig.FontDataOwnedByAtlas = false; //This makes the const_cast below safe.
	io.Fonts->AddFontFromMemoryTTF(const_cast<u8*>(Embed::OpenSansBoldTTFData.data()),
	                               NumericCast<i32>(Embed::OpenSansBoldTTFData.size()),
								   scaleFactor * 18.0f, &fontConfig);
	io.FontDefault = io.Fonts->AddFontFromMemoryTTF(const_cast<u8*>(Embed::OpenSansTTFData.data()),
	                                                NumericCast<i32>(Embed::OpenSansTTFData.size()),
													scaleFactor * 18.0f, &fontConfig);

	//Setup Dear ImGui style
	ImGui::StyleColorsDark();

	//When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to
	ImGuiStyle& style = ImGui::GetStyle();

	style.ScaleAllSizes(scaleFactor);

	if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	SetDarkThemeColors();
	SetImGuizmoStyle();

	INTERNAL::WindowingAPI::InternalWindow* const window = static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	(
		Application::GetWindow()->GetInternalWindow()
	);

	//Setup Platform/Renderer bindings
	const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData(*TRAP::Application::GetWindow());

	TP_TRACE(Log::ImGuiPrefix, "Init...");
	if(!TRAP::INTERNAL::ImGuiWindowing::Init(window, true, Graphics::RendererAPI::GetRenderAPI()))
		Utils::DisplayError(Utils::ErrorCode::ImGuiFailedInitialization);

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		const TRAP::Graphics::API::VulkanRenderer* const renderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>
		(
			TRAP::Graphics::RendererAPI::GetRenderer()
		);
		TRAP_ASSERT(renderer != nullptr, "ImGuiLayer::OnAttach(): renderer is nullptr!");

		const auto tempFolder = TRAP::FileSystem::GetGameTempFolderPath();
		if(tempFolder)
		{
			const TRAP::Graphics::RendererAPI::PipelineCacheLoadDesc cacheDesc{.Path = *tempFolder / "ImGui.cache",
			                                                                   .Flags = TRAP::Graphics::RendererAPI::PipelineCacheFlags::None,
																			   .Name = "Pipeline cache (Pipeline: \"ImGui\")"};
			m_imguiPipelineCache = TRAP::Graphics::PipelineCache::Create(cacheDesc);
		}
		else //Create empty cache as fallback
		{
			TRAP::Graphics::RendererAPI::PipelineCacheDesc cacheDesc{};
			cacheDesc.Name = "Pipeline cache (Pipeline: \"ImGui\")";
			m_imguiPipelineCache = TRAP::Graphics::PipelineCache::Create(cacheDesc);
		}

		TRAP::Graphics::AntiAliasing aaMethod = TRAP::Graphics::AntiAliasing::Off;
		TRAP::Graphics::SampleCount aaSamples = TRAP::Graphics::SampleCount::Two;
		TRAP::Graphics::RenderCommand::GetAntiAliasing(aaMethod, aaSamples);

		//This initializes ImGui for Vulkan
		const auto graphicsQueue = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanQueue>
		(
			TRAP::Graphics::RendererAPI::GetGraphicsQueue()
		);
		const auto pipelineCache = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanPipelineCache>
		(
			m_imguiPipelineCache
		);

		const auto* cmdBuffer = dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
		(
			viewportData.GraphicCommandBuffers[viewportData.ImageIndex]
		);
		TRAP_ASSERT(cmdBuffer != nullptr, "ImGuiLayer::OnAttach(): cmdBuffer is nullptr!");

		const ImGui::INTERNAL::Vulkan::InitInfo initInfo
		{
			.Instance = renderer->GetInstance(),
			.Device = renderer->GetDevice(),
			.Queue = graphicsQueue,
			.DescriptorPoolSizes = GlobalDescriptorPoolSizes(),
			.DescriptorPool = VK_NULL_HANDLE,
			.RenderPass = cmdBuffer->GetActiveVkRenderPass(),
			.MinImageCount = TRAP::Graphics::RendererAPI::ImageCount,
			.ImageCount = TRAP::Graphics::RendererAPI::ImageCount,
			.MSAASamples = aaMethod == TRAP::Graphics::AntiAliasing::MSAA ? static_cast<VkSampleCountFlagBits>(aaSamples) : VK_SAMPLE_COUNT_1_BIT,
			.PipelineCache = pipelineCache,
			.UseDynamicRendering = false,
			.PipelineRenderingCreateInfo = {},
			.Allocator = nullptr,
			.CheckVkResultFn = [](const VkResult res){VkCall(res);}
		};

		ImGui::INTERNAL::Vulkan::Init(initInfo);

		TP_TRACE(Log::ImGuiPrefix, "Finished Vulkan init");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnDetach()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		TP_TRACE(Log::ImGuiPrefix, "Vulkan shutdown...");
		Graphics::RendererAPI::GetRenderer()->WaitIdle();
		const auto tempFolder = TRAP::FileSystem::GetGameTempFolderPath();
		if(tempFolder)
			m_imguiPipelineCache->Save(*tempFolder / "ImGui.cache");
		m_imguiPipelineCache.reset();
		TP_TRACE(Log::ImGuiPrefix, "Finished Vulkan shutdown");

		ImGui::INTERNAL::Vulkan::Shutdown();
		INTERNAL::ImGuiWindowing::Shutdown();
	}

	ImGui::DestroyContext();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnEvent(Events::Event& event)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	if (m_blockEvents)
	{
		const ImGuiIO& io = ImGui::GetIO();
		event.Handled |= event.IsInCategory(Events::EventCategory::Mouse) & io.WantCaptureMouse;
		event.Handled |= event.IsInCategory(Events::EventCategory::Keyboard) & io.WantCaptureKeyboard;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::Begin()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		//Bind SwapChain RenderTarget (this also updates the used RenderPass)
		const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData(*TRAP::Application::GetWindow());
		auto* const vkCmdBuffer = dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
		(
			viewportData.GraphicCommandBuffers[viewportData.ImageIndex]
		);
		TRAP_ASSERT(vkCmdBuffer != nullptr, "ImGuiLayer::Begin(): vkCmdBuffer is nullptr!");
		TRAP::Graphics::AntiAliasing aaMethod = TRAP::Graphics::AntiAliasing::Off;
		TRAP::Graphics::SampleCount aaSamples = TRAP::Graphics::SampleCount::Two;
		TRAP::Graphics::RenderCommand::GetAntiAliasing(aaMethod, aaSamples);

		TRAP::Ref<TRAP::Graphics::RenderTarget> rT = nullptr;

		if(aaMethod == TRAP::Graphics::RendererAPI::AntiAliasing::MSAA && viewportData.RenderScale == 1.0f) //MSAA and no RenderScale
			rT = viewportData.InternalRenderTargets[viewportData.CurrentSwapChainImageIndex];
		else
			rT = viewportData.SwapChain->GetRenderTargets()[viewportData.CurrentSwapChainImageIndex];

		//Cant use TRAP::Graphics::RenderCommand::StartRenderPass() here, because it would also bind the shading rate image
		vkCmdBuffer->BindRenderTargets({ *rT }, nullptr, nullptr, nullptr, nullptr,
		                               std::numeric_limits<u32>::max(), std::numeric_limits<u32>::max());

		//Only apply MSAA if no RenderScale is used (else it got already resolved to a non-MSAA texture)
		if(aaMethod == TRAP::Graphics::AntiAliasing::MSAA && viewportData.RenderScale == 1.0f)
			ImGui::INTERNAL::Vulkan::SetMSAASamples(static_cast<VkSampleCountFlagBits>(aaSamples));
		else
			ImGui::INTERNAL::Vulkan::SetMSAASamples(VK_SAMPLE_COUNT_1_BIT);

		ImGui::INTERNAL::Vulkan::NewFrame();
	}

	INTERNAL::ImGuiWindowing::NewFrame();

	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::End()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(NumericCast<f32>(Application::GetWindow()->GetFrameBufferSize().x()),
	                        NumericCast<f32>(Application::GetWindow()->GetFrameBufferSize().y()));

	//Rendering
	ImGui::Render();
	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData(*TRAP::Application::GetWindow());
		if(!Application::GetWindow()->IsMinimized())
		{
			const auto* cmdBuffer = dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
				(
					viewportData.GraphicCommandBuffers[viewportData.ImageIndex]
				);

			TRAP_ASSERT(ImGui::GetDrawData() != nullptr, "ImGuiLayer::End(): ImGui::GetDrawData() is nullptr!");
			TRAP_ASSERT(cmdBuffer != nullptr, "ImGuiLayer::End(): cmdBuffer is nullptr!");
			if(const ImDrawData* const drawData = ImGui::GetDrawData(); cmdBuffer != nullptr && drawData != nullptr)
				ImGui::INTERNAL::Vulkan::RenderDrawData(*drawData, *cmdBuffer);
		}
	}

	if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::SetDarkThemeColors()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

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
	colors[ImGuiCol_TabSelected] = ImVec4(0.28f, 0.2805f, 0.281f, 1.0f);
	colors[ImGuiCol_TabDimmed] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
	colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

	//Title BG
	colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::SetImGuizmoStyle()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	ImGuizmo::Style& style = ImGuizmo::GetStyle();

	static constexpr f32 ScaleFactor = 2.0f;

	style.TranslationLineThickness *= ScaleFactor;
	style.TranslationLineArrowSize *= ScaleFactor;
	style.RotationLineThickness *= ScaleFactor;
	style.RotationOuterLineThickness *= ScaleFactor;
	style.ScaleLineThickness *= ScaleFactor;
	style.ScaleLineCircleSize *= ScaleFactor;

	style.Colors[ImGuizmo::COLOR::DIRECTION_X] = ImVec4(0.847f, 0.369f, 0.443f, 1.000f);
	style.Colors[ImGuizmo::COLOR::DIRECTION_Y] = ImVec4(0.145f, 0.666f, 0.145f, 1.000f);
	style.Colors[ImGuizmo::COLOR::DIRECTION_Z] = ImVec4(0.173f, 0.325f, 0.800f, 1.000f);
	style.Colors[ImGuizmo::COLOR::PLANE_X] = ImVec4(0.847f, 0.408f, 0.478f, 1.000f);
	style.Colors[ImGuizmo::COLOR::PLANE_Y] = ImVec4(0.333f, 0.671f, 0.333f, 1.000f);
	style.Colors[ImGuizmo::COLOR::PLANE_Z] = ImVec4(0.259f, 0.404f, 0.851f, 1.000f);
	style.Colors[ImGuizmo::COLOR::SELECTION] = ImVec4(0.800f, 0.666f, 0.126f, 1.000f);
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::Image(const TRAP::Ref<TRAP::Graphics::Texture>& image, const TRAP::Ref<TRAP::Graphics::Sampler>& sampler,
                  const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col,
				  const ImVec4& border_col)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	TRAP_ASSERT(sampler != nullptr, "ImGui::Image(): Sampler is nullptr!");
	TRAP_ASSERT(image != nullptr, "ImGui::Image(): Image is nullptr!");
	TRAP_ASSERT(image->GetType() == TRAP::Graphics::TextureType::Texture2D, "ImGui::Image(): Image is not a Texture2D!");

	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
	{
		const auto vkImage = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(image);
		const auto vkSampler = dynamic_pointer_cast<TRAP::Graphics::API::VulkanSampler>(sampler);
		ImTextureID texID = ImGui::INTERNAL::Vulkan::AddTexture(vkSampler, vkImage,
		                                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui::Image(texID, size, uv0, uv1, tint_col, border_col);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::Image(const TRAP::Ref<TRAP::Graphics::Texture>& image, const ImVec2& size, const ImVec2& uv0,
                  const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	TRAP_ASSERT(image != nullptr, "ImGui::Image(): Image is nullptr!");
	TRAP_ASSERT(image->GetType() == TRAP::Graphics::TextureType::Texture2D, "ImGui::Image(): Image is not a Texture2D!");

	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
	{
		const auto vkImage = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(image);
		ImTextureID texID = ImGui::INTERNAL::Vulkan::AddTexture(TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultSampler,
		                                                        vkImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui::Image(texID, size, uv0, uv1, tint_col, border_col);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGui::ImageButton(const TRAP::Ref<TRAP::Graphics::Texture>& image, const ImVec2& size,
                        const ImVec2& uv0, const ImVec2& uv1, const ImVec4& bg_col, const ImVec4& tint_col)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	TRAP_ASSERT(image != nullptr, "ImGui::ImageButton(): Image is nullptr!");
	TRAP_ASSERT(image->GetType() == TRAP::Graphics::TextureType::Texture2D, "ImGui::ImageButton(): Image is not a Texture2D!");

	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
	{
		const auto vkImage = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(image);
		ImTextureID texID = ImGui::INTERNAL::Vulkan::AddTexture(TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultSampler,
												                vkImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		VkImageView imgView = vkImage->GetSRVVkImageView();
		usize imgViewHash = 0;
		TRAP::Utils::HashCombine(imgViewHash, imgView);
		const ImGuiID imgViewID = static_cast<ImGuiID>(imgViewHash); //Intended narrowing

		return ImGui::ImageButtonEx(imgViewID, texID, size, uv0, uv1, bg_col, tint_col);
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGui::InputText(const std::string_view label, std::string* const str, ImGuiInputTextFlags flags,
                      const ImGuiInputTextCallback callback, void* const userData)
{
	TRAP_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0, "ImGui::InputText(): Missing callback resize flag!");
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallbackUserData cbUserData{str, callback, userData};

	return InputText(label.data(), str->data(), str->capacity() + 1, flags, InputTextCallback, &cbUserData);
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGui::InputTextMultiline(const std::string_view label, std::string* const str, const ImVec2& size,
                               ImGuiInputTextFlags flags, const ImGuiInputTextCallback callback,
							   void* const userData)
{
	TRAP_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0, "ImGui::InputTextMultiline(): Missing callback resize flag!");
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallbackUserData cbUserData{str, callback, userData};

	return InputTextMultiline(label.data(), str->data(), str->capacity() + 1, size, flags, InputTextCallback, &cbUserData);
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImGui::InputTextWithHint(const std::string_view label, const std::string_view hint, std::string* const str,
                              ImGuiInputTextFlags flags, const ImGuiInputTextCallback callback, void* const userData)
{
	TRAP_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0, "ImGui::InputTextWithHint(): Missing callback resize flag!");
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallbackUserData cbUserData{str, callback, userData};

	return InputTextWithHint(label.data(), hint.data(), str->data(), str->capacity() + 1, flags, InputTextCallback, &cbUserData);
}

//-------------------------------------------------------------------------------------------------------------------//

ImFont* ImGui::AddFontFromFileTTF(const std::string_view filename, const f32 sizePixels,
							      const ImFontConfig* const fontCfgTemplate, const ImWchar* const glyphRanges)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	//Add font like normally
	ImFont* const font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.data(), sizePixels, fontCfgTemplate, glyphRanges);

	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		ImGui::INTERNAL::Vulkan::CreateFontsTexture();

	return font;
}

//-------------------------------------------------------------------------------------------------------------------//

ImFont* ImGui::AddFontFromMemoryTTF(const std::span<const u8> fontData, const f32 sizePixels,
								    const ImFontConfig* const fontCfg, const ImWchar* const glyphRanges)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	//Add font like normally
	ImFont* const font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<u8*>(fontData.data()),
	                                                                NumericCast<i32>(fontData.size_bytes()),
																	sizePixels, fontCfg, glyphRanges);

	if (TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
		ImGui::INTERNAL::Vulkan::CreateFontsTexture();

	return font;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool ImGui::IsInputEnabled()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	const auto& io = ImGui::GetIO();
	return (io.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0 &&
	       (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::SetInputEnabled(const bool enable)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

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

#endif /*TRAP_HEADLESS_MODE*/
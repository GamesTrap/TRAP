#include "TRAPPCH.h"
#include "ImGuiLayer.h"

#include <examples/imgui_impl_vulkan.h>

#include "Application.h"
#include "Window/WindowingAPI.h"
#include "ImGuiWindowing.h"
#include "Embed.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImGuiLayer::ImGuiLayer()
	: Layer("ImGuiLayer"), m_blockEvents(true)
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
	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
		//ImGui_ImplGlfw_InitForVulkan(window, false);
		//ImGui_ImplVulkan_Init();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnDetach()
{
	TP_PROFILE_FUNCTION();

	if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
	{
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
	/*if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());*/

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		INTERNAL::WindowingAPI::InternalWindow* backupCurrentContext = nullptr;
		if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
		{
			//Save current context here
		}

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		if (Graphics::RendererAPI::GetRenderAPI() == Graphics::RenderAPI::Vulkan)
		{
			//Load saved context here
		}
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
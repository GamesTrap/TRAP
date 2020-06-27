#include "TRAPPCH.h"
#include "ImGuiLayer.h"

#include "ImGuiOpenGL4.h"
#include <examples/imgui_impl_vulkan.h>

#include "Application.h"
#include "Graphics/API/Context.h"
#include "Window/WindowingAPI.h"
#include "ImGuiWindowing.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImGuiLayer::ImGuiLayer()
	: Layer("ImGuiLayer")
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

	//Setup Dear ImGui style
	ImGui::StyleColorsDark();

	//When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	INTERNAL::WindowingAPI::InternalWindow* window = static_cast<INTERNAL::WindowingAPI::InternalWindow*>(Application::GetWindow()->GetInternalWindow());

	//Setup Platform/Renderer bindings
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
	{
		//ImGui_ImplGlfw_InitForVulkan(window, false);
		//ImGui_ImplVulkan_Init();
	}
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
	{
		INTERNAL::ImGuiWindowing::InitForOpenGL(window, true);
		ImGuiOpenGL4Init();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnDetach()
{
	TP_PROFILE_FUNCTION();

	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
	{
		ImGui_ImplVulkan_Shutdown();
		INTERNAL::ImGuiWindowing::Shutdown();
	}
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
	{
		ImGuiOpenGL4Shutdown();
		INTERNAL::ImGuiWindowing::Shutdown();
	}

	ImGui::DestroyContext();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnEvent(Events::Event& event)
{
	ImGuiIO& io = ImGui::GetIO();
	event.Handled |= event.IsInCategory(Events::EventCategory::Mouse) & io.WantCaptureMouse;
	event.Handled |= event.IsInCategory(Events::EventCategory::Keyboard) & io.WantCaptureKeyboard;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::Begin()
{
	TP_PROFILE_FUNCTION();

	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
	{
		ImGui_ImplVulkan_NewFrame();
		INTERNAL::ImGuiWindowing::NewFrame();
	}
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
	{
		ImGuiOpenGL4NewFrame();
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
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
		ImGuiOpenGL4RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		INTERNAL::WindowingAPI::InternalWindow* backupCurrentContext = nullptr;
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
		{
			//Save current context here
		}
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
			backupCurrentContext = INTERNAL::WindowingAPI::GetCurrentContext();

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
		{
			//Load saved context here
		}
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
			INTERNAL::WindowingAPI::MakeContextCurrent(backupCurrentContext);
	}
}
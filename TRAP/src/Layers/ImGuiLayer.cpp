#include "TRAPPCH.h"
#include "ImGuiLayer.h"

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_vulkan.h>
#ifdef TRAP_PLATFORM_WINDOWS
#include <examples/imgui_impl_win32.h>
#include <examples/imgui_impl_dx12.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include "Application.h"
#include "Graphics/API/Context.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImGuiLayer::ImGuiLayer()
	: Layer("ImGuiLayer")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnAttach()
{
	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //Enable Multi-Viewport / Platform Windows

	//Setup Dear ImGui style
	ImGui::StyleColorsDark();

	//WHen viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());

	//Setup Platform/Renderer bindings
/*#ifdef TRAP_PLATFORM_WINDOWS
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::D3D12)
	{
		ImGui_ImplWin32_Init(glfwGetWin32Window(window));
		//ImGui_ImplDX12_Init();
	}
#endif*/
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
	{
		ImGui_ImplGlfw_InitForVulkan(window, false);
		//ImGui_ImplVulkan_Init();
	}
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
	{
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460 core");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::OnDetach()
{
/*#ifdef TRAP_PLATFORM_WINDOWS
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::D3D12)
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
	}
#endif*/
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

	ImGui::DestroyContext();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::Begin()
{
/*#ifdef TRAP_PLATFORM_WINDOWS
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::D3D12)
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
	}
#endif*/
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	ImGui::NewFrame();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::ImGuiLayer::End()
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(Application::Get().GetWindow()->GetWidth()), static_cast<float>(Application::Get().GetWindow()->GetHeight()));

	//Rendering
	ImGui::Render();
/*#ifdef TRAP_PLATFORM_WINDOWS
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::D3D12)
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData());
#endif
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());*/
	if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{

		GLFWwindow* backupCurrentContext = nullptr;
/*#ifdef TRAP_PLATFORM_WINDOWS
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::D3D12)
		{
			//Save current context here
		}
#endif*/
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
		{
			//Save current context here
		}
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
			backupCurrentContext = glfwGetCurrentContext();

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

/*#ifdef TRAP_PLATFORM_WINDOWS
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::D3D12)
		{
			//Load saved context here
		}
#endif*/
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::Vulkan)
		{
			//Load saved context here
		}
		if (Graphics::API::Context::GetRenderAPI() == Graphics::API::RenderAPI::OpenGL)
			glfwMakeContextCurrent(backupCurrentContext);
	}
}
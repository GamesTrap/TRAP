#include "RendererAPIInfo.h"

#include <Graphics/API/Vulkan/VulkanRenderer.h>
#include <Graphics/API/Vulkan/Objects/VulkanDevice.h>
#include <Graphics/API/Vulkan/Objects/VulkanInstance.h>

RendererAPIInfo::RendererAPIInfo()
	: Layer("RendererAPIInfo")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void RendererAPIInfo::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("RendererAPI Info");
}

//-------------------------------------------------------------------------------------------------------------------//

void RendererAPIInfo::OnImGuiRender()
{
    const std::string GPUName = TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGPUName();
    const std::array<uint8_t, 16> GPUUUID = TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGPUUUID();
    std::string GPUUUIDStr = TRAP::Utils::UUIDToString(GPUUUID);
    const std::string renderAPI = TRAP::Utils::String::ConvertToString(TRAP::Graphics::RendererAPI::GetRenderAPI());

    ImGui::Begin("RendererAPI Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
	                                   ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Current RenderAPI: %s", renderAPI.c_str());
    ImGui::Separator();
    ImGui::Text("Current GPU:");
    ImGui::Text("Name: %s", GPUName.c_str());
    ImGui::Text("UUID: %s", GPUUUIDStr.c_str());
    ImGui::NewLine();
    ImGui::Text("Supported RenderAPIs:");
    ImGui::Text("Vulkan: %s", TRAP::Graphics::RendererAPI::IsVulkanCapable() ? "Supported" : "Not Supported");
    ImGui::NewLine();
    ImGui::Text("Supported Features:");
    ImGui::Text("MultiDrawIndirect: %s", TRAP::Graphics::RendererAPI::GPUSettings.MultiDrawIndirect ? "Supported" : "Not Supported");
    ImGui::Text("Tessellation: %s", TRAP::Graphics::RendererAPI::GPUSettings.TessellationSupported ? "Supported" : "Not Supported");
    ImGui::Text("Geometry Shader: %s", TRAP::Graphics::RendererAPI::GPUSettings.GeometryShaderSupported ? "Supported" : "Not Supported");
    ImGui::Text("Surface: %s", TRAP::Graphics::RendererAPI::GPUSettings.SurfaceSupported ? "Supported" : "Not Supported");
    ImGui::Text("Present: %s", TRAP::Graphics::RendererAPI::GPUSettings.PresentSupported ? "Supported" : "Not Supported");
    ImGui::Text("Max Anisotropy: %i", static_cast<int32_t>(TRAP::Graphics::RendererAPI::GPUSettings.MaxAnisotropy));
    if(TRAP::Graphics::RendererAPI::GetRenderer()->GetAllGPUs().size() > 1)
    {
        ImGui::Separator();
        ImGui::Text("Other supported GPUs:");
        for(const auto& [name, uuid] : TRAP::Graphics::RendererAPI::GetRenderer()->GetAllGPUs())
        {
            if(name == GPUName)
                continue;

            ImGui::Text("Name: %s", name.c_str());
        }
        ImGui::Separator();
    }
    ImGui::End();

    if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
    {
        TRAP::Graphics::API::VulkanRenderer* vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>(TRAP::Graphics::RendererAPI::GetRenderer());
        TRAP::Graphics::API::VulkanInstance* vkInstance = vkRenderer->GetInstance().get();
        TRAP::Graphics::API::VulkanDevice* vkDev = vkRenderer->GetDevice().get();
        const std::vector<std::string> instLayers = vkInstance->GetUsedInstanceLayers();
        const std::vector<std::string> instExts = vkInstance->GetUsedInstanceExtensions();
        const std::vector<std::string> devExts = vkDev->GetUsedPhysicalDeviceExtensions();

        ImGui::Begin("Vulkan Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
	                                   ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Used Instance Layers:");
        for(const std::string& str : instLayers)
            ImGui::Text("%s", str.c_str());
        ImGui::Separator();
        ImGui::Text("Used Instance Extensions:");
        for(const std::string& str : instExts)
            ImGui::Text("%s", str.c_str());
        ImGui::Separator();
        ImGui::Text("Used Physical Device Extensions:");
        for(const std::string& str : devExts)
            ImGui::Text("%s", str.c_str());
        ImGui::End();
    }
}

#include "RendererAPIInfo.h"

#include <Graphics/API/Vulkan/VulkanRenderer.h>
#include <Graphics/API/Vulkan/Objects/VulkanDevice.h>
#include <Graphics/API/Vulkan/Objects/VulkanInstance.h>
#include <Graphics/API/Vulkan/Objects/VulkanPhysicalDevice.h>

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
    const std::string GPUName = TRAP::Graphics::RenderCommand::GetGPUName();
    const std::array<uint8_t, 16> GPUUUID = TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGPUUUID();
    const std::string GPUUUIDStr = TRAP::Utils::UUIDToString(GPUUUID);
    const std::string renderAPI = TRAP::Utils::String::ConvertToString(TRAP::Graphics::RendererAPI::GetRenderAPI());

    ImGui::Begin("RendererAPI Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
	                                   ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Current RenderAPI: %s", renderAPI.c_str());
    ImGui::Separator();
    ImGui::Text("Current GPU:");
    ImGui::Text("Name: %s", GPUName.c_str());
    ImGui::Text("UUID: %s", GPUUUIDStr.c_str());
    ImGui::NewLine();
    ImGui::Text("RenderAPIs:");
    ImGui::Text("Vulkan: %s", TRAP::Graphics::RendererAPI::IsVulkanCapable() ? "Supported" : "Not Supported");
    ImGui::NewLine();
    ImGui::Text("Supported Features:");
    ImGui::Text("MultiDrawIndirect: %s", TRAP::Graphics::RendererAPI::GPUSettings.MultiDrawIndirectSupported ? "Supported" : "Not Supported");
    ImGui::Text("Tessellation: %s", TRAP::Graphics::RendererAPI::GPUSettings.TessellationSupported ? "Supported" : "Not Supported");
    ImGui::Text("Geometry Shader: %s", TRAP::Graphics::RendererAPI::GPUSettings.GeometryShaderSupported ? "Supported" : "Not Supported");
    ImGui::Text("Surface: %s", TRAP::Graphics::RendererAPI::GPUSettings.SurfaceSupported ? "Supported" : "Not Supported");
    ImGui::Text("Present: %s", TRAP::Graphics::RendererAPI::GPUSettings.PresentSupported ? "Supported" : "Not Supported");
    ImGui::Text("Max Anisotropy: %u", NumericCast<uint32_t>(TRAP::Graphics::RendererAPI::GPUSettings.MaxAnisotropy));
    bool vrsPerDraw = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerDraw) != TRAP::Graphics::RendererAPI::ShadingRateCaps::NotSupported;
    bool vrsPerTile = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerTile) != TRAP::Graphics::RendererAPI::ShadingRateCaps::NotSupported;
    bool vrsMin = static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCombiner & TRAP::Graphics::RendererAPI::ShadingRateCombiner::Min);
    bool vrsMax = static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCombiner & TRAP::Graphics::RendererAPI::ShadingRateCombiner::Max);
    bool vrsSum = static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCombiner & TRAP::Graphics::RendererAPI::ShadingRateCombiner::Sum);
    bool vrs1x1 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::RendererAPI::ShadingRate::Full) != TRAP::Graphics::RendererAPI::ShadingRate::NotSupported;
    bool vrs2x2 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::RendererAPI::ShadingRate::Half) != TRAP::Graphics::RendererAPI::ShadingRate::NotSupported;
    bool vrs4x4 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::RendererAPI::ShadingRate::Quarter) != TRAP::Graphics::RendererAPI::ShadingRate::NotSupported;
    bool vrs8x8 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::RendererAPI::ShadingRate::Eighth) != TRAP::Graphics::RendererAPI::ShadingRate::NotSupported;
    bool vrs1x2 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::RendererAPI::ShadingRate::OneXTwo) != TRAP::Graphics::RendererAPI::ShadingRate::NotSupported;
    bool vrs2x1 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::RendererAPI::ShadingRate::TwoXOne) != TRAP::Graphics::RendererAPI::ShadingRate::NotSupported;
    bool vrs2x4 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::RendererAPI::ShadingRate::TwoXFour) != TRAP::Graphics::RendererAPI::ShadingRate::NotSupported;
    bool vrs4x2 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::RendererAPI::ShadingRate::FourXTwo) != TRAP::Graphics::RendererAPI::ShadingRate::NotSupported;
    ImGui::Text("Variable Rate Shading (VRS): %s", vrsPerDraw ? "Supported" : "Not Supported");
    ImGui::Text("    Per Draw: %s", vrsPerDraw ? "Supported" : "Not Supported");
    ImGui::Text("    Per Tile: %s", vrsPerTile ? "Supported" : "Not Supported");
    ImGui::Text("    Combiner:");
    ImGui::Text("        Passthrough: %s", vrsPerDraw ? "Supported" : "Not Supported");
    ImGui::Text("        Override: %s", vrsPerDraw ? "Supported" : "Not Supported");
    ImGui::Text("        Min: %s", vrsMin ? "Supported" : "Not Supported");
    ImGui::Text("        Max: %s", vrsMax ? "Supported" : "Not Supported");
    ImGui::Text("        Sum: %s", vrsSum ? "Supported" : "Not Supported");
    ImGui::Text("    Rates:");
    ImGui::Text("        1x1: %s", vrs1x1 ? "Supported" : "Not Supported");
    ImGui::Text("        2x2: %s", vrs2x2 ? "Supported" : "Not Supported");
    ImGui::Text("        4x4: %s", vrs4x4 ? "Supported" : "Not Supported");
    ImGui::Text("        8x8: %s", vrs8x8 ? "Supported" : "Not Supported");
    ImGui::Text("        1x2: %s", vrs1x2 ? "Supported" : "Not Supported");
    ImGui::Text("        2x1: %s", vrs2x1 ? "Supported" : "Not Supported");
    ImGui::Text("        2x4: %s", vrs2x4 ? "Supported" : "Not Supported");
    ImGui::Text("        4x2: %s", vrs4x2 ? "Supported" : "Not Supported");
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
        const TRAP::Graphics::API::VulkanRenderer* const vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>(TRAP::Graphics::RendererAPI::GetRenderer());
        const TRAP::Graphics::API::VulkanInstance* const vkInstance = vkRenderer->GetInstance().get();
        const TRAP::Graphics::API::VulkanDevice* const vkDev = vkRenderer->GetDevice().get();
        TRAP::Graphics::API::VulkanPhysicalDevice* const vkPhysicalDev = vkDev->GetPhysicalDevice();
        const std::vector<std::string> instLayers = vkInstance->GetUsedInstanceLayers();
        const std::vector<std::string> instExts = vkInstance->GetUsedInstanceExtensions();
        const std::vector<std::string> devExts = vkDev->GetUsedPhysicalDeviceExtensions();

        ImGui::Begin("Vulkan Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
	                                   ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Used Instance Layers:");
        for(const std::string& str : instLayers)
        {
            const auto layerProps = TRAP::Graphics::API::VulkanInstance::GetInstanceLayerProperties(str);
            if(layerProps)
            {
                ImGui::Text("%s (%s) Spec: %i.%i.%i.%i Rev: %i", str.c_str(), layerProps->description,
                            VK_API_VERSION_MAJOR(layerProps->specVersion), VK_API_VERSION_MINOR(layerProps->specVersion),
                            VK_API_VERSION_PATCH(layerProps->specVersion), VK_API_VERSION_VARIANT(layerProps->specVersion),
                            layerProps->implementationVersion);
            }
            else
                ImGui::Text("%s", str.c_str());
        }
        ImGui::Separator();
        ImGui::Text("Used Instance Extensions:");
        for(const std::string& str : instExts)
        {
            const auto extensionProps = TRAP::Graphics::API::VulkanInstance::GetInstanceExtensionProperties(str);
            if(extensionProps)
            {
                ImGui::Text("%s Spec: %i.%i.%i.%i", str.c_str(), VK_API_VERSION_MAJOR(extensionProps->specVersion),
                            VK_API_VERSION_MINOR(extensionProps->specVersion), VK_API_VERSION_PATCH(extensionProps->specVersion),
                            VK_API_VERSION_VARIANT(extensionProps->specVersion));
            }
            else
                ImGui::Text("%s", str.c_str());
        }
        ImGui::Separator();
        ImGui::Text("Used Physical Device Extensions:");
        for(const std::string& str : devExts)
        {
            const auto extensionProps = vkPhysicalDev->GetPhysicalDeviceExtensionProperties(str);
            if(extensionProps)
            {
                ImGui::Text("%s Spec: %i.%i.%i.%i", str.c_str(), VK_API_VERSION_MAJOR(extensionProps->specVersion),
                            VK_API_VERSION_MINOR(extensionProps->specVersion), VK_API_VERSION_PATCH(extensionProps->specVersion),
                            VK_API_VERSION_VARIANT(extensionProps->specVersion));
            }
            else
                ImGui::Text("%s", str.c_str());
        }
        ImGui::End();
    }
}

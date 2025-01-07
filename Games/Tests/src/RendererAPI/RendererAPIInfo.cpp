#include "RendererAPIInfo.h"

#include <Graphics/API/Vulkan/VulkanRenderer.h>
#include <Graphics/API/Vulkan/Objects/VulkanDevice.h>
#include <Graphics/API/Vulkan/Objects/VulkanInstance.h>
#include <Graphics/API/Vulkan/Objects/VulkanPhysicalDevice.h>

void RendererAPIInfo::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("RendererAPI Info");
}

//-------------------------------------------------------------------------------------------------------------------//

void RendererAPIInfo::OnImGuiRender()
{
    const std::string GPUName = TRAP::Graphics::RenderCommand::GetGPUName();
    const TRAP::Utils::UUID GPUUUID = TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGPUUUID();
    const std::string GPUUUIDStr = TRAP::Utils::UUIDToString(GPUUUID);
    const std::string renderAPI = fmt::format("{}", TRAP::Graphics::RendererAPI::GetRenderAPI());

    ImGui::Begin("RendererAPI Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
	                                   ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextFmt("Current RenderAPI: {}", renderAPI);
    ImGui::Separator();
    ImGui::TextFmt("Current GPU:");
    ImGui::TextFmt("Name: {}", GPUName);
    ImGui::TextFmt("UUID: {}", GPUUUIDStr);
    ImGui::NewLine();
    ImGui::TextFmt("RenderAPIs:");
    ImGui::TextFmt("Vulkan: {}", TRAP::Graphics::RendererAPI::IsVulkanCapable() ? "Supported" : "Not Supported");
    ImGui::NewLine();
    ImGui::TextFmt("Supported Features:");
    ImGui::TextFmt("MultiDrawIndirect: {}", TRAP::Graphics::RendererAPI::GPUSettings.MultiDrawIndirectSupported ? "Supported" : "Not Supported");
    ImGui::TextFmt("Tessellation: {}", TRAP::Graphics::RendererAPI::GPUSettings.TessellationSupported ? "Supported" : "Not Supported");
    ImGui::TextFmt("Geometry Shader: {}", TRAP::Graphics::RendererAPI::GPUSettings.GeometryShaderSupported ? "Supported" : "Not Supported");
    ImGui::TextFmt("Surface: {}", TRAP::Graphics::RendererAPI::GPUSettings.SurfaceSupported ? "Supported" : "Not Supported");
    ImGui::TextFmt("Present: {}", TRAP::Graphics::RendererAPI::GPUSettings.PresentSupported ? "Supported" : "Not Supported");
    ImGui::TextFmt("Max Anisotropy: {}", NumericCast<u32>(TRAP::Graphics::RendererAPI::GPUSettings.MaxAnisotropy));
    bool vrsPerDraw = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::ShadingRateCaps::PerDraw) != TRAP::Graphics::ShadingRateCaps::NotSupported;
    bool vrsPerTile = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::ShadingRateCaps::PerTile) != TRAP::Graphics::ShadingRateCaps::NotSupported;
    bool vrsMin = static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCombiner & TRAP::Graphics::ShadingRateCombiner::Min);
    bool vrsMax = static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCombiner & TRAP::Graphics::ShadingRateCombiner::Max);
    bool vrsSum = static_cast<bool>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCombiner & TRAP::Graphics::ShadingRateCombiner::Sum);
    bool vrs1x1 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Full) != TRAP::Graphics::ShadingRate::NotSupported;
    bool vrs2x2 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Half) != TRAP::Graphics::ShadingRate::NotSupported;
    bool vrs4x4 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Quarter) != TRAP::Graphics::ShadingRate::NotSupported;
    bool vrs8x8 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::Eighth) != TRAP::Graphics::ShadingRate::NotSupported;
    bool vrs1x2 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::OneXTwo) != TRAP::Graphics::ShadingRate::NotSupported;
    bool vrs2x1 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXOne) != TRAP::Graphics::ShadingRate::NotSupported;
    bool vrs2x4 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::TwoXFour) != TRAP::Graphics::ShadingRate::NotSupported;
    bool vrs4x2 = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRates & TRAP::Graphics::ShadingRate::FourXTwo) != TRAP::Graphics::ShadingRate::NotSupported;
    ImGui::TextFmt("Variable Rate Shading (VRS): {}", vrsPerDraw ? "Supported" : "Not Supported");
    ImGui::TextFmt("    Per Draw: {}", vrsPerDraw ? "Supported" : "Not Supported");
    ImGui::TextFmt("    Per Tile: {}", vrsPerTile ? "Supported" : "Not Supported");
    ImGui::TextFmt("    Combiner:");
    ImGui::TextFmt("        Passthrough: {}", vrsPerDraw ? "Supported" : "Not Supported");
    ImGui::TextFmt("        Override: {}", vrsPerDraw ? "Supported" : "Not Supported");
    ImGui::TextFmt("        Min: {}", vrsMin ? "Supported" : "Not Supported");
    ImGui::TextFmt("        Max: {}", vrsMax ? "Supported" : "Not Supported");
    ImGui::TextFmt("        Sum: {}", vrsSum ? "Supported" : "Not Supported");
    ImGui::TextFmt("    Rates:");
    ImGui::TextFmt("        1x1: {}", vrs1x1 ? "Supported" : "Not Supported");
    ImGui::TextFmt("        2x2: {}", vrs2x2 ? "Supported" : "Not Supported");
    ImGui::TextFmt("        4x4: {}", vrs4x4 ? "Supported" : "Not Supported");
    ImGui::TextFmt("        8x8: {}", vrs8x8 ? "Supported" : "Not Supported");
    ImGui::TextFmt("        1x2: {}", vrs1x2 ? "Supported" : "Not Supported");
    ImGui::TextFmt("        2x1: {}", vrs2x1 ? "Supported" : "Not Supported");
    ImGui::TextFmt("        2x4: {}", vrs2x4 ? "Supported" : "Not Supported");
    ImGui::TextFmt("        4x2: {}", vrs4x2 ? "Supported" : "Not Supported");
    if(TRAP::Graphics::RendererAPI::GetRenderer()->GetAllGPUs().size() > 1u)
    {
        ImGui::Separator();
        ImGui::TextFmt("Other supported GPUs:");
        for(const auto& [name, uuid] : TRAP::Graphics::RendererAPI::GetRenderer()->GetAllGPUs())
        {
            if(name == GPUName)
                continue;

            ImGui::TextFmt("Name: {}", name);
        }
        ImGui::Separator();
    }
    ImGui::End();

    if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::Vulkan)
    {
        const TRAP::Graphics::API::VulkanRenderer* const vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer*>(TRAP::Graphics::RendererAPI::GetRenderer());
        const TRAP::Graphics::API::VulkanInstance* const vkInstance = vkRenderer->GetInstance().get();
        const TRAP::Graphics::API::VulkanDevice* const vkDev = vkRenderer->GetDevice().get();
        const TRAP::Graphics::API::VulkanPhysicalDevice& vkPhysicalDev = vkDev->GetPhysicalDevice();
        const std::vector<std::string> instLayers = vkInstance->GetUsedInstanceLayers();
        const std::vector<std::string> instExts = vkInstance->GetUsedInstanceExtensions();
        const std::vector<std::string> devExts = vkDev->GetUsedPhysicalDeviceExtensions();

        ImGui::Begin("Vulkan Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
	                                         ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::TextFmt("Used Instance Layers:");
        for(const std::string& str : instLayers)
        {
            const auto layerProps = TRAP::Graphics::API::VulkanInstance::GetInstanceLayerProperties(str);
            if(layerProps)
            {
                ImGui::TextFmt("{} ({}) Spec: {}.{}.{}.{} Rev: {}", str, layerProps->description,
                               VK_API_VERSION_MAJOR(layerProps->specVersion), VK_API_VERSION_MINOR(layerProps->specVersion),
                               VK_API_VERSION_PATCH(layerProps->specVersion), VK_API_VERSION_VARIANT(layerProps->specVersion),
                               layerProps->implementationVersion);
            }
            else
                ImGui::TextFmt("{}", str);
        }
        ImGui::Separator();
        ImGui::TextFmt("Used Instance Extensions:");
        for(const std::string& str : instExts)
        {
            const auto extensionProps = TRAP::Graphics::API::VulkanInstance::GetInstanceExtensionProperties(str);
            if(extensionProps)
            {
                ImGui::TextFmt("{} Spec: {}.{}.{}.{}", str, VK_API_VERSION_MAJOR(extensionProps->specVersion),
                               VK_API_VERSION_MINOR(extensionProps->specVersion), VK_API_VERSION_PATCH(extensionProps->specVersion),
                               VK_API_VERSION_VARIANT(extensionProps->specVersion));
            }
            else
                ImGui::TextFmt("{}", str);
        }
        ImGui::Separator();
        ImGui::TextFmt("Used Physical Device Extensions:");
        for(const std::string& str : devExts)
        {
            const auto extensionProps = vkPhysicalDev.GetPhysicalDeviceExtensionProperties(str);
            if(extensionProps)
            {
                ImGui::TextFmt("{} Spec: {}.{}.{}.{}", str, VK_API_VERSION_MAJOR(extensionProps->specVersion),
                               VK_API_VERSION_MINOR(extensionProps->specVersion), VK_API_VERSION_PATCH(extensionProps->specVersion),
                               VK_API_VERSION_VARIANT(extensionProps->specVersion));
            }
            else
                ImGui::TextFmt("{}", str);
        }
        ImGui::End();
    }
}

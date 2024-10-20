/*
The MIT License(MIT)

Copyright(c) 2014 - 2024 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Modified by: Jan "GamesTrap" Schuerkamp
*/

#ifndef TRAP_IMGUIVULKANBACKEND_H
#define TRAP_IMGUIVULKANBACKEND_H

#ifndef TRAP_HEADLESS_MODE

#include <vector>

// dear imgui: Renderer Backend for Vulkan

// Implemented features:
//  [x] Renderer: User texture binding. Use 'VkDescriptorSet' as ImTextureID. Read the FAQ about ImTextureID! See https://github.com/ocornut/imgui/pull/914 for discussions.
//  [X] Renderer: Large meshes support (64k+ vertices) with 16-bit indices.
//  [X] Renderer: Expose selected render state for draw callbacks to use. Access in '(ImGui_ImplXXXX_RenderState*)GetPlatformIO().Renderer_RenderState'.
//  [x] Renderer: Multi-viewport / platform windows. With issues (flickering when creating a new viewport).

#include <imgui.h>

#include "Core/Base.h"
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"

namespace TRAP::Graphics::API
{
    class VulkanInstance;
    class VulkanDevice;
    class VulkanQueue;
    class VulkanPipelineCache;
    class VulkanSampler;
    class VulkanCommandBuffer;
    class VulkanTexture;
};

namespace ImGui::INTERNAL::Vulkan
{
    /// @brief Initialization data, for ImGui_ImplVulkan_Init()
	/// @remark @headless This struct is not available in headless mode.
    /// @note - VkDescriptorPool should be created with a pool size large enough to hold an ImGui VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER descriptor.
    ///       - When using dynamic rendering, set UseDynamicRendering=true and fill PipelineRenderingCreateInfo structure.
    struct InitInfo
    {
        using PFN_CheckVkResultFn = void(*)(VkResult err);

        TRAP::Ref<TRAP::Graphics::API::VulkanInstance> Instance = nullptr;
        TRAP::Ref<TRAP::Graphics::API::VulkanDevice> Device = nullptr;
        TRAP::Ref<TRAP::Graphics::API::VulkanQueue> Queue = nullptr;
        std::vector<VkDescriptorPoolSize> DescriptorPoolSizes{};
        VkDescriptorPool DescriptorPool = VK_NULL_HANDLE; //See requirements in note above
        VkRenderPass RenderPass = VK_NULL_HANDLE; //Ignored if using dynamic rendering
        u32 MinImageCount = 2u;                   // >= 2
        u32 ImageCount = 2u;                      // >= MinImageCount
        VkSampleCountFlagBits MSAASamples = VK_SAMPLE_COUNT_1_BIT; // 0 defaults to VK_SAMPLE_COUNT_1_BIT

        //(Optional)
        TRAP::Ref<TRAP::Graphics::API::VulkanPipelineCache> PipelineCache = nullptr;

        //(Optional) Dynamic Rendering
        //Need to explicitly enable VK_KHR_dynamic_rendering extension to use this, even for Vulkan 1.3.
        bool UseDynamicRendering = false;
        VkPipelineRenderingCreateInfoKHR PipelineRenderingCreateInfo{};

        //(Optional) Allocation, Debugging
        const VkAllocationCallbacks* Allocator = nullptr;
        PFN_CheckVkResultFn CheckVkResultFn = nullptr;
    };

    // Called by user code
                  void        Init(const InitInfo& info);
                  void        Shutdown();
                  void        NewFrame();
                  void        RenderDrawData(const ImDrawData& draw_data, const TRAP::Graphics::API::VulkanCommandBuffer& command_buffer, VkPipeline pipeline = VK_NULL_HANDLE);
                  void        CreateFontsTexture();
                  /// @note You probably never need to call this, as it is called by CreateFontsTexture() and Shutdown().
                  void        DestroyFontsTexture();
    [[nodiscard]] ImTextureID AddTexture(const TRAP::Ref<TRAP::Graphics::API::VulkanSampler>& sampler, const TRAP::Ref<TRAP::Graphics::API::VulkanTexture>& image, VkImageLayout image_layout);
                  void        RemoveTexture(const TRAP::Ref<TRAP::Graphics::API::VulkanTexture>& image_view);
                  ImTextureID UpdateTextureInfo(VkDescriptorSet descriptorSet, const TRAP::Graphics::API::VulkanSampler& sampler, VkImageView image_view, VkImageLayout image_layout);
                  void        ClearCache() noexcept;
                  void        SetMSAASamples(VkSampleCountFlagBits sampleCount);

                  void        RenderWindow(ImGuiViewport* viewport, [[maybe_unused]] void* render_arg);

    /// @brief [BETA] Selected render state data shared with callbacks.
    ///        This is temporarily stored in io.BackendRendererRenderState during the ImGui_ImplVulkan_RenderDrawData() call.
    struct ImGui_ImplVulkan_RenderState
    {
        const TRAP::Graphics::API::VulkanCommandBuffer* CommandBuffer;
        VkPipeline Pipeline;
        VkPipelineLayout PipelineLayout;
    };
};

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_IMGUIVULKANBACKEND_H*/

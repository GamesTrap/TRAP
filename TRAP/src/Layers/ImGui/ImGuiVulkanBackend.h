/*
The MIT License(MIT)

Copyright(c) 2014 - 2023 Omar Cornut

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

#include <optional>

// dear imgui: Renderer Backend for Vulkan
// This needs to be used along with a Platform Backend (e.g. GLFW, SDL, Win32, custom..)

// Implemented features:
//  [X] Renderer: Large meshes support (64k+ vertices) with 16-bit indices.
//  [x] Renderer: Multi-viewport / platform windows. With issues (flickering when creating a new viewport).

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// The aim of imgui_impl_vulkan.h/.cpp is to be usable in your engine without any modification.
// IF YOU FEEL YOU NEED TO MAKE ANY CHANGE TO THIS CODE, please share them and your feedback at https://github.com/ocornut/imgui/

// Important note to the reader who wish to integrate imgui_impl_vulkan.cpp/.h in their own engine/app.
// - Common ImGui_ImplVulkan_XXX functions and structures are used to interface with imgui_impl_vulkan.cpp/.h.
//   You will use those if you want to use this rendering backend in your engine/app.
// - Helper ImGui_ImplVulkanH_XXX functions and structures are only used by this example (main.cpp) and by
//   the backend itself (imgui_impl_vulkan.cpp), but should PROBABLY NOT be used by your own engine/app code.
// Read comments in imgui_impl_vulkan.h.

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
//ImGUI
#include <imgui.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"

// Initialization data, for ImGui_ImplVulkan_Init()
// [Please zero-clear before use!]
struct ImGui_ImplVulkan_InitInfo
{
    using PFN_CheckVkResultFn = void(*)(VkResult err);

    VkInstance                      Instance = VK_NULL_HANDLE;
    VkPhysicalDevice                PhysicalDevice = VK_NULL_HANDLE;
    VkDevice                        Device = VK_NULL_HANDLE;
    uint32_t                        QueueFamily = 0;
    VkQueue                         Queue = VK_NULL_HANDLE;
    VkPipelineCache                 PipelineCache = VK_NULL_HANDLE;
    VkDescriptorPool                DescriptorPool = VK_NULL_HANDLE;
    uint32_t                        Subpass = 0;
    uint32_t                        MinImageCount = 2;                   // >= 2
    uint32_t                        ImageCount = 2;                      // >= MinImageCount
    VkSampleCountFlagBits           MSAASamples = VK_SAMPLE_COUNT_1_BIT; // >= VK_SAMPLE_COUNT_1_BIT (0 -> default to VK_SAMPLE_COUNT_1_BIT)

    //Dynamic Rendering (Optional)
    bool UseDynamicRendering = false; //Need to explicitly enable VK_KHR_dynamic_rendering extension to use this, even for Vulkan 1.3.
    VkFormat ColorAttachmentFormat = VK_FORMAT_UNDEFINED; //Required for dynamic rendering

    //Allocation, Debugging
    const VkAllocationCallbacks* Allocator = nullptr;
    PFN_CheckVkResultFn CheckVkResultFn = nullptr;
};

// Called by user code
              IMGUI_IMPL_API void        ImGui_ImplVulkan_Init(const ImGui_ImplVulkan_InitInfo* info, VkRenderPass render_pass);
              IMGUI_IMPL_API void        ImGui_ImplVulkan_Shutdown();
              IMGUI_IMPL_API void        ImGui_ImplVulkan_NewFrame();
              IMGUI_IMPL_API void        ImGui_ImplVulkan_RenderDrawData(const ImDrawData* draw_data, VkCommandBuffer command_buffer, VkPipeline pipeline = VK_NULL_HANDLE);
              // IMGUI_IMPL_API bool        ImGui_ImplVulkan_CreateFontsTexture(VkCommandBuffer command_buffer);
              // IMGUI_IMPL_API void        ImGui_ImplVulkan_DestroyFontsTexture();
              IMGUI_IMPL_API void        ImGui_ImplVulkan_UploadFontsTexture();
              IMGUI_IMPL_API void        ImGui_ImplVulkan_DestroyFontUploadObjects();
              IMGUI_IMPL_API void        ImGui_ImplVulkan_SetMinImageCount(uint32_t min_image_count); // To override MinImageCount after initialization (e.g. if swap chain is recreated)
[[nodiscard]] IMGUI_IMPL_API ImTextureID ImGui_ImplVulkan_AddTexture(VkSampler sampler, VkImageView image_view, VkImageLayout image_layout);
              IMGUI_IMPL_API void        ImGui_ImplVulkan_RemoveTexture(VkImageView image_view);
              IMGUI_IMPL_API ImTextureID ImGui_ImplVulkan_UpdateTextureInfo(VkDescriptorSet descriptorSet, VkSampler sampler, VkImageView image_view, VkImageLayout image_layout);
              IMGUI_IMPL_API void        ImGui_ImplVulkan_ClearCache() noexcept;
              IMGUI_IMPL_API void        ImGui_ImplVulkan_SetMSAASamples(VkSampleCountFlagBits sampleCount);

//-------------------------------------------------------------------------
// Internal / Miscellaneous Vulkan Helpers
// (Used by example's main.cpp. Used by multi-viewport features. PROBABLY NOT used by your own engine/app.)
//-------------------------------------------------------------------------
// You probably do NOT need to use or care about those functions.
// Those functions only exist because:
//   1) they facilitate the readability and maintenance of the multiple main.cpp examples files.
//   2) the multi-viewport / platform window implementation needs them internally.
// Generally we avoid exposing any kind of superfluous high-level helpers in the bindings,
// but it is too much code to duplicate everywhere so we exceptionally expose them.
//
// Your engine/app will likely _already_ have code to setup all that stuff (swap chain, render pass, frame buffers, etc.).
// You may read this code to learn about Vulkan, but it is recommended you use you own custom tailored code to do equivalent work.
// (The ImGui_ImplVulkanH_XXX functions do not interact with any of the state used by the regular ImGui_ImplVulkan_XXX functions)
//-------------------------------------------------------------------------

struct ImGui_ImplVulkanH_Window;

// Helpers
              IMGUI_IMPL_API void                   ImGui_ImplVulkanH_CreateOrResizeWindow(VkInstance instance, VkPhysicalDevice physical_device, VkDevice device, ImGui_ImplVulkanH_Window* wnd, uint32_t queue_family, const VkAllocationCallbacks* allocator, int32_t w, int32_t h, uint32_t min_image_count);
              IMGUI_IMPL_API void                   ImGui_ImplVulkanH_DestroyWindow(VkInstance instance, VkDevice device, ImGui_ImplVulkanH_Window* wnd, const VkAllocationCallbacks* allocator);
[[nodiscard]] IMGUI_IMPL_API VkSurfaceFormatKHR     ImGui_ImplVulkanH_SelectSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkFormat* request_formats, int32_t request_formats_count, VkColorSpaceKHR request_color_space);
[[nodiscard]] IMGUI_IMPL_API VkPresentModeKHR       ImGui_ImplVulkanH_SelectPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkPresentModeKHR* request_modes, int32_t request_modes_count);
[[nodiscard]] IMGUI_IMPL_API std::optional<int32_t> ImGui_ImplVulkanH_GetMinImageCountFromPresentMode(VkPresentModeKHR present_mode);

// Helper structure to hold the data needed by one rendering frame
// (Used by example's main.cpp. Used by multi-viewport features. Probably NOT used by your own engine/app.)
// [Please zero-clear before use!]
struct ImGui_ImplVulkanH_Frame
{
    VkCommandPool       CommandPool = VK_NULL_HANDLE;
    VkCommandBuffer     CommandBuffer = VK_NULL_HANDLE;
    VkFence             Fence = VK_NULL_HANDLE;
    VkImage             Backbuffer = VK_NULL_HANDLE;
    VkImageView         BackbufferView = VK_NULL_HANDLE;
    VkFramebuffer       Framebuffer = VK_NULL_HANDLE;
};

struct ImGui_ImplVulkanH_FrameSemaphores
{
    VkSemaphore         ImageAcquiredSemaphore = VK_NULL_HANDLE;
    VkSemaphore         RenderCompleteSemaphore = VK_NULL_HANDLE;
};

// Helper structure to hold the data needed by one rendering context into one OS window
// (Used by example's main.cpp. Used by multi-viewport features. Probably NOT used by your own engine/app.)
struct ImGui_ImplVulkanH_Window
{
    int                 Width{};
    int                 Height{};
    VkSwapchainKHR      Swapchain = VK_NULL_HANDLE;
    VkSurfaceKHR        Surface = VK_NULL_HANDLE;
    VkSurfaceFormatKHR  SurfaceFormat{};
    VkPresentModeKHR    PresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
    VkRenderPass        RenderPass = VK_NULL_HANDLE;
    VkPipeline          Pipeline = VK_NULL_HANDLE; // The window pipeline may uses a different VkRenderPass than the one passed in ImGui_ImplVulkan_InitInfo
    bool                UseDynamicRendering = false;
    bool                ClearEnable = true;
    VkClearValue        ClearValue{};
    uint32_t            FrameIndex{}; // Current frame being rendered to (0 <= FrameIndex < FrameInFlightCount)
    uint32_t            ImageCount{}; // Number of simultaneous in-flight frames (returned by vkGetSwapchainImagesKHR, usually derived from min_image_count)
    uint32_t            SemaphoreIndex{}; // Current set of swapchain wait semaphores we're using (needs to be distinct from per frame data)
    ImGui_ImplVulkanH_Frame*            Frames = nullptr;
    ImGui_ImplVulkanH_FrameSemaphores*  FrameSemaphores = nullptr;
};

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_IMGUIVULKANBACKEND_H*/

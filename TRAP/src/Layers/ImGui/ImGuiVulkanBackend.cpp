/*
The MIT License (MIT)

Copyright (c) 2014-2023 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Modified by Jan "GamesTrap" Schuerkamp
*/

#include "TRAPPCH.h"
#include "ImGuiVulkanBackend.h"

#ifndef TRAP_HEADLESS_MODE

#include "Application.h"
#include "Maths/Math.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/Objects/VulkanCommandPool.h"
#include "Graphics/API/Vulkan/Objects/VulkanCommandBuffer.h"
#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"
#include "Graphics/API/Vulkan/Objects/VulkanFence.h"
#include "Graphics/API/Vulkan/Objects/VulkanDevice.h"
#include "Graphics/API/Vulkan/Objects/VulkanInstance.h"
#include "Graphics/API/Vulkan/Objects/VulkanPipelineCache.h"
#include "Graphics/API/Vulkan/Objects/VulkanSampler.h"
#include "Graphics/API/Vulkan/Objects/VulkanBuffer.h"
#include "Graphics/API/Vulkan/Objects/VulkanRenderTarget.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"
#include "Graphics/API/Vulkan/Objects/VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Objects/Fence.h"
#include "Graphics/Textures/Texture.h"

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

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2023-XX-XX: Platform: Added support for multiple windows via the ImGuiPlatformIO interface.
//  2023-07-04: Vulkan: Added optional support for VK_KHR_dynamic_rendering. User needs to set init_info->UseDynamicRendering = true and init_info->ColorAttachmentFormat.
//  2023-01-02: Vulkan: Fixed sampler passed to ImGui_ImplVulkan_AddTexture() not being honored + removed a bunch of duplicate code.
//  2022-10-11: Using 'nullptr' instead of 'NULL' as per our switch to C++11.
//  2022-10-04: Vulkan: Added experimental ImGui_ImplVulkan_RemoveTexture() for api symetry. (#914, #5738).
//  2021-10-15: Vulkan: Call vkCmdSetScissor() at the end of render a full-viewport to reduce likehood of issues with people using VK_DYNAMIC_STATE_SCISSOR in their app without calling vkCmdSetScissor() explicitly every frame.
//  2021-06-29: Reorganized backend to pull data from a single structure to facilitate usage with multiple-contexts (all g_XXXX access changed to bd->XXXX).
//  2021-03-22: Vulkan: Fix mapped memory validation error when buffer sizes are not multiple of VkPhysicalDeviceLimits::nonCoherentAtomSize.
//  2021-02-18: Vulkan: Change blending equation to preserve alpha in output buffer.
//  2021-01-27: Vulkan: Added support for custom function load and IMGUI_IMPL_VULKAN_NO_PROTOTYPES by using ImGui_ImplVulkan_LoadFunctions().
//  2020-11-11: Vulkan: Added support for specifying which subpass to reference during VkPipeline creation.
//  2020-09-07: Vulkan: Added VkPipeline parameter to ImGui_ImplVulkan_RenderDrawData (default to one passed to ImGui_ImplVulkan_Init).
//  2020-05-04: Vulkan: Fixed crash if initial frame has no vertices.
//  2020-04-26: Vulkan: Fixed edge case where render callbacks wouldn't be called if the ImDrawData didn't have vertices.
//  2019-08-01: Vulkan: Added support for specifying multisample count. Set ImGui_ImplVulkan_InitInfo::MSAASamples to one of the VkSampleCountFlagBits values to use, default is non-multisampled as before.
//  2019-05-29: Vulkan: Added support for large mesh (64K+ vertices), enable ImGuiBackendFlags_RendererHasVtxOffset flag.
//  2019-04-30: Vulkan: Added support for special ImDrawCallback_ResetRenderState callback to reset render state.
//  2019-04-04: *BREAKING CHANGE*: Vulkan: Added ImageCount/MinImageCount fields in ImGui_ImplVulkan_InitInfo, required for initialization (was previously a hard #define IMGUI_VK_QUEUED_FRAMES 2). Added ImGui_ImplVulkan_SetMinImageCount().
//  2019-04-04: Vulkan: Added VkInstance argument to ImGui_ImplVulkanH_CreateWindow() optional helper.
//  2019-04-04: Vulkan: Avoid passing negative coordinates to vkCmdSetScissor, which debug validation layers do not like.
//  2019-04-01: Vulkan: Support for 32-bit index buffer (#define ImDrawIdx unsigned int).
//  2019-02-16: Vulkan: Viewport and clipping rectangles correctly using draw_data->FramebufferScale to allow retina display.
//  2018-11-30: Misc: Setting up io.BackendRendererName so it can be displayed in the About Window.
//  2018-08-25: Vulkan: Fixed mishandled VkSurfaceCapabilitiesKHR::maxImageCount=0 case.
//  2018-06-22: Inverted the parameters to ImGui_ImplVulkan_RenderDrawData() to be consistent with other backends.
//  2018-06-08: Misc: Extracted imgui_impl_vulkan.cpp/.h away from the old combined GLFW+Vulkan example.
//  2018-06-08: Vulkan: Use draw_data->DisplayPos and draw_data->DisplaySize to setup projection matrix and clipping rectangle.
//  2018-03-03: Vulkan: Various refactor, created a couple of ImGui_ImplVulkanH_XXX helper that the example can use and that viewport support will use.
//  2018-03-01: Vulkan: Renamed ImGui_ImplVulkan_Init_Info to ImGui_ImplVulkan_InitInfo and fields to match more closely Vulkan terminology.
//  2018-02-16: Misc: Obsoleted the io.RenderDrawListsFn callback, ImGui_ImplVulkan_Render() calls ImGui_ImplVulkan_RenderDrawData() itself.
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.
//  2017-05-15: Vulkan: Fix scissor offset being negative. Fix new Vulkan validation warnings. Set required depth member for buffer image copy.
//  2016-11-13: Vulkan: Fix validation layer warnings and errors and redeclare gl_PerVertex.
//  2016-10-18: Vulkan: Add location decorators & change to use structs as in/out in glsl, update embedded spv (produced with glslangValidator -x). Null the released resources.
//  2016-08-27: Vulkan: Fix Vulkan example for use when a depth buffer is active.

using namespace ImGui::INTERNAL::Vulkan;

namespace
{
    //-----------------------------------------------------------------------------
    // SHADERS
    //-----------------------------------------------------------------------------

    // glsl_shader.vert, compiled with:
    // # glslangValidator -V -x -o glsl_shader.vert.u32 glsl_shader.vert
    /*
    #version 450 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aUV;
    layout(location = 2) in vec4 aColor;
    layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

    out gl_PerVertex { vec4 gl_Position; };
    layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

    void main()
    {
        Out.Color = aColor;
        Out.UV = aUV;
        gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
    }
    */
    constexpr std::array<u32, 324> glsl_shader_vert_spv =
    {
        0x07230203,0x00010000,0x00080001,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
        0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
        0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
        0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
        0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
        0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
        0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
        0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
        0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
        0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
        0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
        0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
        0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
        0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
        0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
        0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
        0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
        0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
        0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
        0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
        0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
        0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
        0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
        0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
        0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
        0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
        0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
        0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
        0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
        0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
        0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
        0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
        0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
        0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
        0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
        0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
        0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
        0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
        0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
        0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
        0x0000002d,0x0000002c,0x000100fd,0x00010038
    };

    //-------------------------------------------------------------------------------------------------------------------//

    // glsl_shader.frag, compiled with:
    // # glslangValidator -V -x -o glsl_shader.frag.u32 glsl_shader.frag
    /*
    #version 450 core
    layout(location = 0) out vec4 fColor;
    layout(set=0, binding=0) uniform sampler2D sTexture;
    layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
    void main()
    {
        fColor = In.Color * texture(sTexture, In.UV.st);
    }
    */
    constexpr std::array<u32, 193> glsl_shader_frag_spv =
    {
        0x07230203,0x00010000,0x00080001,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
        0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
        0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
        0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
        0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
        0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
        0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
        0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
        0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
        0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
        0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
        0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
        0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
        0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
        0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
        0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
        0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
        0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
        0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
        0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
        0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
        0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
        0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
        0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
        0x00010038
    };

    //-------------------------------------------------------------------------
    // Internal / Miscellaneous Vulkan Helpers
    // (Used by multi-viewport features.)
    //-------------------------------------------------------------------------

    // Helper structure to hold the data needed by one rendering frame
    // (Used by multi-viewport features.)
    struct ImGui_ImplVulkanH_Frame
    {
        TRAP::Scope<TRAP::Graphics::API::VulkanCommandPool> CommandPool = nullptr;
        TRAP::Graphics::API::VulkanCommandBuffer* CommandBuffer = nullptr;
        TRAP::Ref<TRAP::Graphics::API::VulkanFence> Fence = nullptr;
        TRAP::Ref<TRAP::Graphics::API::VulkanRenderTarget> Backbuffer = nullptr;
        VkFramebuffer       Framebuffer = VK_NULL_HANDLE;
    };

    struct ImGui_ImplVulkanH_FrameSemaphores
    {
        TRAP::Ref<TRAP::Graphics::API::VulkanSemaphore> ImageAcquiredSemaphore = nullptr;
        TRAP::Ref<TRAP::Graphics::API::VulkanSemaphore> RenderCompleteSemaphore = nullptr;
    };

    // Helper structure to hold the data needed by one rendering context into one OS window
    // (Used by multi-viewport features.)
    struct ImGui_ImplVulkanH_Window
    {
        i32             Width{};
        i32             Height{};
        VkSwapchainKHR      Swapchain = VK_NULL_HANDLE;
        VkSurfaceKHR        Surface = VK_NULL_HANDLE;
        VkSurfaceFormatKHR  SurfaceFormat{};
        VkPresentModeKHR    PresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
        VkRenderPass        RenderPass = VK_NULL_HANDLE;
        VkPipeline          Pipeline = VK_NULL_HANDLE; // The window pipeline may uses a different VkRenderPass than the one passed in ImGui_ImplVulkan_InitInfo
        bool                UseDynamicRendering = false;
        bool                ClearEnable = true;
        VkClearValue        ClearValue{};
        u32            FrameIndex{}; // Current frame being rendered to (0 <= FrameIndex < FrameInFlightCount)
        u32            ImageCount{}; // Number of simultaneous in-flight frames (returned by vkGetSwapchainImagesKHR, usually derived from min_image_count)
        u32            SemaphoreIndex{}; // Current set of swapchain wait semaphores we're using (needs to be distinct from per frame data)
        std::vector<ImGui_ImplVulkanH_Frame>            Frames{};
        std::vector<ImGui_ImplVulkanH_FrameSemaphores>  FrameSemaphores{};
    };

    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    struct VulkanTextureCacheEntry
    {
        TRAP::Ref<TRAP::Graphics::API::VulkanSampler> Sampler;
        VkImageLayout ImageLayout;
        VkDescriptorSet DescriptorSet;
    };
    std::unordered_map<TRAP::Ref<TRAP::Graphics::API::VulkanTexture>, VulkanTextureCacheEntry> s_VulkanTextureCache{};

    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    // Reusable buffers used for rendering 1 current in-flight frame, for RenderDrawData()
    struct ImGui_ImplVulkanH_FrameRenderBuffers
    {
        TRAP::Ref<TRAP::Graphics::API::VulkanBuffer> VertexBuffer = nullptr;
        TRAP::Ref<TRAP::Graphics::API::VulkanBuffer> IndexBuffer = nullptr;
    };

    //-------------------------------------------------------------------------------------------------------------------//

    // Each viewport will hold 1 ImGui_ImplVulkanH_WindowRenderBuffers
    struct ImGui_ImplVulkanH_WindowRenderBuffers
    {
        u32            Index = 0;
        u32            Count = 0;
        std::vector<ImGui_ImplVulkanH_FrameRenderBuffers> FrameRenderBuffers{};
    };

    //-------------------------------------------------------------------------------------------------------------------//

    // For multi-viewport support:
    // Helper structure we store in the void* RendererUserData field of each ImGuiViewport to easily retrieve our backend data.
    struct ImGui_ImplVulkan_ViewportData
    {
        bool                                    WindowOwned = false;
        ImGui_ImplVulkanH_Window                Window{};             // Used by secondary viewports only
        ImGui_ImplVulkanH_WindowRenderBuffers   RenderBuffers{};      // Used by all viewports
    };

    //-------------------------------------------------------------------------------------------------------------------//

    // Vulkan data
    struct ImGui_ImplVulkan_Data
    {
        InitInfo                    VulkanInitInfo{};
        VkRenderPass                RenderPass = VK_NULL_HANDLE;
        VkPipelineCreateFlags       PipelineCreateFlags{};
        VkDescriptorSetLayout       DescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout            PipelineLayout = VK_NULL_HANDLE;
        VkDescriptorSet             DescriptorSet = VK_NULL_HANDLE;
        VkPipeline                  Pipeline = VK_NULL_HANDLE;
        VkShaderModule              ShaderModuleVert = VK_NULL_HANDLE;
        VkShaderModule              ShaderModuleFrag = VK_NULL_HANDLE;

        // Font data
        TRAP::Ref<TRAP::Graphics::API::VulkanSampler> FontSampler = nullptr;
        TRAP::Ref<TRAP::Graphics::API::VulkanTexture> FontImage = nullptr;
        VkDescriptorSet             FontDescriptorSet = VK_NULL_HANDLE;
        TRAP::Ref<TRAP::Graphics::API::VulkanBuffer> UploadBuffer = nullptr;

        // Render buffers for main window
        ImGui_ImplVulkanH_WindowRenderBuffers MainWindowRenderBuffers{};

        std::vector<VkDescriptorPool> DescriptorPools;
    };

    //-------------------------------------------------------------------------------------------------------------------//
    //Forward Declarations-----------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] ImGui_ImplVulkan_Data* GetBackendData();
    void CheckVkResult(VkResult err);
    void SetupRenderState(const ImDrawData& draw_data, VkPipeline pipeline,
                          const TRAP::Graphics::API::VulkanCommandBuffer& command_buffer,
                          const ImGui_ImplVulkanH_FrameRenderBuffers& rb, i32 fb_width, i32 fb_height);
    void CreateShaderModules(const TRAP::Graphics::API::VulkanDevice& device,
                             const VkAllocationCallbacks* allocator);
    void CreatePipeline(const TRAP::Graphics::API::VulkanDevice& device, const VkAllocationCallbacks* allocator,
                        const TRAP::Graphics::API::VulkanPipelineCache* pipelineCache,
                        VkRenderPass renderPass, VkSampleCountFlagBits MSAASamples, VkPipeline& pipeline);
    void CreateFontsTexture(const TRAP::Graphics::API::VulkanCommandBuffer& command_buffer);
    void DestroyFontsTexture();
    void CreateDeviceObjects();
    void DestroyDeviceObjects();

    [[nodiscard]] VkSurfaceFormatKHR SelectSurfaceFormat(const TRAP::Graphics::API::VulkanPhysicalDevice& physical_device,
                                                         VkSurfaceKHR surface, std::span<const VkFormat> request_formats,
                                                         VkColorSpaceKHR request_color_space);
    [[nodiscard]] VkPresentModeKHR SelectPresentMode(const TRAP::Graphics::API::VulkanPhysicalDevice& physical_device,
                                                     VkSurfaceKHR surface, std::span<const VkPresentModeKHR> request_modes);
    void CreateWindowCommandBuffers(ImGui_ImplVulkanH_Window& wd, const TRAP::Ref<TRAP::Graphics::API::VulkanQueue>& queue);
    [[nodiscard]] std::optional<i32> GetMinImageCountFromPresentMode(VkPresentModeKHR present_mode);
    void CreateWindowSwapChain(const TRAP::Graphics::API::VulkanDevice& device,
                               ImGui_ImplVulkanH_Window& wd, const VkAllocationCallbacks* allocator,
                               i32 w, i32 h, u32 min_image_count);
    void CreateOrResizeWindow(const TRAP::Graphics::API::VulkanDevice& device,
                              ImGui_ImplVulkanH_Window& wnd, const TRAP::Ref<TRAP::Graphics::API::VulkanQueue>& queue_family,
                              const VkAllocationCallbacks* allocator, i32 w, i32 h, u32 min_image_count);
    void DestroyWindow(const TRAP::Graphics::API::VulkanInstance& instance,
                       const TRAP::Graphics::API::VulkanDevice& device, ImGui_ImplVulkanH_Window& wnd,
                       const VkAllocationCallbacks* allocator);
    void DestroyFrame(const TRAP::Graphics::API::VulkanDevice& device, ImGui_ImplVulkanH_Frame& fd,
                      const VkAllocationCallbacks* allocator);
    void CreateNewDescriptorPool();
    void DestroyAllViewportsRenderBuffers();

    void CreateWindow(ImGuiViewport* viewport);
    void DestroyWindow(ImGuiViewport* viewport);
    void SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
    void RenderWindow(ImGuiViewport* viewport, [[maybe_unused]] void* render_arg);
    void SwapBuffers(ImGuiViewport* viewport, [[maybe_unused]] void* render_arg);

    void InitPlatformInterface();
    void ShutdownPlatformInterface();

    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] ImGui_ImplVulkan_Data* GetBackendData()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

        const ImGuiContext* context = ImGui::GetCurrentContext();
        const ImGuiIO& io = ImGui::GetIO();

        TRAP_ASSERT(ImGui::GetCurrentContext() != nullptr, "ImGuiVulkanBackend::GetBackendData(): ImGui context is nullptr!");
        TRAP_ASSERT(io.BackendRendererUserData != nullptr, "ImGuiVulkanBackend::GetBackendData(): BackendRendererUserData is nullptr!");
        return context != nullptr ? static_cast<ImGui_ImplVulkan_Data*>(io.BackendRendererUserData) : nullptr;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CheckVkResult(const VkResult err)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        if (bd == nullptr)
            return;

        const InitInfo& v = bd->VulkanInitInfo;
        if (v.CheckVkResultFn != nullptr)
            v.CheckVkResultFn(err);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void SetupRenderState(const ImDrawData& draw_data, VkPipeline pipeline,
                          const TRAP::Graphics::API::VulkanCommandBuffer& command_buffer,
                          const ImGui_ImplVulkanH_FrameRenderBuffers& rb, const i32 fb_width,
                          const i32 fb_height)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();

        // Bind pipeline
        {
            vkCmdBindPipeline(command_buffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        }

        // Bind Vertex And Index Buffer:
        if (draw_data.TotalVtxCount > 0)
        {
            command_buffer.BindVertexBuffer({rb.VertexBuffer}, {sizeof(TRAP::Math::Vec2) + sizeof(TRAP::Math::Vec2) + sizeof(TRAP::Math::Vec4)}, {0});
            const auto indexType = sizeof(ImDrawIdx) == 2 ? TRAP::Graphics::RendererAPI::IndexType::UInt16 : TRAP::Graphics::RendererAPI::IndexType::UInt32;
            command_buffer.BindIndexBuffer(rb.IndexBuffer, indexType, 0);
        }

        // Setup viewport:
        command_buffer.SetViewport(0.0f, 0.0f, NumericCast<f32>(fb_width), NumericCast<f32>(-fb_height), 0.0f, 1.0f);

        // Setup scale and translation:
        // Our visible imgui space lies from draw_data->DisplayPps (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
        {
            const std::array<f32, 2> scale{2.0f / draw_data.DisplaySize.x, 2.0f / draw_data.DisplaySize.y};
            const std::array<f32, 2> translate{-1.0f - draw_data.DisplayPos.x * std::get<0>(scale), -1.0f - draw_data.DisplayPos.y * std::get<1>(scale)};
            vkCmdPushConstants(command_buffer.GetVkCommandBuffer(), bd->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(f32) * 0, sizeof(f32) * 2, scale.data());
            vkCmdPushConstants(command_buffer.GetVkCommandBuffer(), bd->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(f32) * 2, sizeof(f32) * 2, translate.data());
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateShaderModules(const TRAP::Graphics::API::VulkanDevice& device,
                             const VkAllocationCallbacks* const allocator)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        // Create the shader modules
        ImGui_ImplVulkan_Data* const bd = GetBackendData();
        if (bd->ShaderModuleVert == VK_NULL_HANDLE)
        {
            static constexpr VkShaderModuleCreateInfo vert_info
            {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .codeSize = glsl_shader_vert_spv.size() * sizeof(decltype(glsl_shader_vert_spv)::value_type),
                .pCode = glsl_shader_vert_spv.data()
            };
            const VkResult err = vkCreateShaderModule(device.GetVkDevice(), &vert_info, allocator, &bd->ShaderModuleVert);
            CheckVkResult(err);
        }
        if (bd->ShaderModuleFrag == VK_NULL_HANDLE)
        {
            static constexpr VkShaderModuleCreateInfo frag_info
            {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .codeSize = glsl_shader_frag_spv.size() * sizeof(decltype(glsl_shader_frag_spv)::value_type),
                .pCode = glsl_shader_frag_spv.data()
            };
            const VkResult err = vkCreateShaderModule(device.GetVkDevice(), &frag_info, allocator, &bd->ShaderModuleFrag);
            CheckVkResult(err);
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreatePipeline(const TRAP::Graphics::API::VulkanDevice& device,
                        const VkAllocationCallbacks* const allocator,
                        const TRAP::Graphics::API::VulkanPipelineCache* const pipelineCache,
                        VkRenderPass renderPass, const VkSampleCountFlagBits MSAASamples, VkPipeline& pipeline)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        CreateShaderModules(device, allocator);

        const std::array<VkPipelineShaderStageCreateInfo, 2> stage
        {
            TRAP::Graphics::API::VulkanInits::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, bd->ShaderModuleVert, "main"),
            TRAP::Graphics::API::VulkanInits::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, bd->ShaderModuleFrag, "main")
        };

        static constexpr VkVertexInputBindingDescription binding_desc
        {
            .binding = 0,
            .stride = sizeof(ImDrawVert),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        };

        static constexpr std::array<VkVertexInputAttributeDescription, 3> attribute_desc
        {
            VkVertexInputAttributeDescription
            {
                .location = 0,
                .binding = binding_desc.binding,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = IM_OFFSETOF(ImDrawVert, pos)
            },
            VkVertexInputAttributeDescription
            {
                .location = 1,
                .binding = binding_desc.binding,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = IM_OFFSETOF(ImDrawVert, uv)
            },
            VkVertexInputAttributeDescription
            {
                .location = 2,
                .binding = binding_desc.binding,
                .format = VK_FORMAT_R8G8B8A8_UNORM,
                .offset = IM_OFFSETOF(ImDrawVert, col)
            }
        };

        const VkPipelineVertexInputStateCreateInfo vertex_info = TRAP::Graphics::API::VulkanInits::PipelineVertexInputStateCreateInfo(1, &binding_desc, 3, attribute_desc.data());
        static constexpr VkPipelineInputAssemblyStateCreateInfo ia_info = TRAP::Graphics::API::VulkanInits::PipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
        static constexpr VkPipelineViewportStateCreateInfo viewport_info = TRAP::Graphics::API::VulkanInits::PipelineViewportStateCreateInfo();

        static constexpr VkPipelineRasterizationStateCreateInfo raster_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f
        };

        const VkPipelineMultisampleStateCreateInfo ms_info = TRAP::Graphics::API::VulkanInits::PipelineMultisampleStateCreateInfo((MSAASamples != 0) ? MSAASamples : VK_SAMPLE_COUNT_1_BIT, false, 0.0f);

        static constexpr VkPipelineColorBlendAttachmentState color_attachment
        {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = static_cast<u32>(VK_COLOR_COMPONENT_R_BIT) | static_cast<u32>(VK_COLOR_COMPONENT_G_BIT) |
                              static_cast<u32>(VK_COLOR_COMPONENT_B_BIT) | static_cast<u32>(VK_COLOR_COMPONENT_A_BIT)
        };

        static constexpr VkPipelineDepthStencilStateCreateInfo depth_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = VK_FALSE,
            .depthWriteEnable = VK_FALSE,
            .depthCompareOp = VK_COMPARE_OP_NEVER,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .front = VkStencilOpState{.failOp = VK_STENCIL_OP_KEEP, .passOp = VK_STENCIL_OP_KEEP, .depthFailOp = VK_STENCIL_OP_KEEP,
                                      .compareOp = VK_COMPARE_OP_NEVER, .compareMask = 0, .writeMask = 0, .reference = 0},
            .back = VkStencilOpState{.failOp = VK_STENCIL_OP_KEEP, .passOp = VK_STENCIL_OP_KEEP, .depthFailOp = VK_STENCIL_OP_KEEP,
                                      .compareOp = VK_COMPARE_OP_NEVER, .compareMask = 0, .writeMask = 0, .reference = 0},
            .minDepthBounds =  0.0f,
            .maxDepthBounds = 0.0f
        };

        const std::vector<VkPipelineColorBlendAttachmentState> color_attachments{ color_attachment };
        const VkPipelineColorBlendStateCreateInfo blend_info = TRAP::Graphics::API::VulkanInits::PipelineColorBlendStateCreateInfo(color_attachments);

        const std::vector<VkDynamicState> dynamic_states{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        const VkPipelineDynamicStateCreateInfo dynamic_state = TRAP::Graphics::API::VulkanInits::PipelineDynamicStateCreateInfo(dynamic_states);

        const VkGraphicsPipelineCreateInfo info = TRAP::Graphics::API::VulkanInits::GraphicsPipelineCreateInfo(2, stage.data(), vertex_info, ia_info, viewport_info, raster_info, ms_info, depth_info, blend_info, dynamic_state, bd->PipelineLayout, renderPass);

    #ifdef IMGUI_IMPL_VULKAN_HAY_DYNAMIC_RENDERING
        const VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
            .pNext = nullptr,
            .viewMask = 0,
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &bd->VulkanInitInfo.ColorAttachmentFormat,
            .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED
        };
        if(bd->VulkanInitInfo.UseDynamicRendering)
        {
            info.pNext = &pipelineRenderingCreateInfo;
            info.renderPass = VK_NULL_HANDLE; //Just make sure it's actually nullptr
        }
    #endif

        const VkResult err = vkCreateGraphicsPipelines(device.GetVkDevice(),
                                                       pipelineCache != nullptr ? pipelineCache->GetVkPipelineCache() : VK_NULL_HANDLE,
                                                       1, &info, allocator, &pipeline);
        CheckVkResult(err);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateFontsTexture(const TRAP::Graphics::API::VulkanCommandBuffer& command_buffer)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplVulkan_Data* const bd = GetBackendData();

        u8* pixels = nullptr;
        i32 width = 0, height = 0;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        const usize upload_size = NumericCast<usize>(width) * NumericCast<usize>(height) * 4u * sizeof(char);

        // Create the Image:
        bd->FontImage = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(TRAP::Graphics::TextureType::Texture2D);
        const TRAP::Graphics::RendererAPI::TextureDesc fontDesc
        {
            .Flags = TRAP::Graphics::RendererAPI::TextureCreationFlags::None,
            .Width = NumericCast<u32>(width),
            .Height = NumericCast<u32>(height),
            .Depth = 1,
            .ArraySize = 1,
            .SampleCount = TRAP::Graphics::RendererAPI::SampleCount::One,
            .SampleQuality = 1,
            .Format = TRAP::Graphics::API::ImageFormat::R8G8B8A8_UNORM,
            .ClearValue = {},
            .StartState = TRAP::Graphics::RendererAPI::ResourceState::Undefined,
            .Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture,
            .NativeHandle = nullptr,
            .Name = "ImGui Font Texture",
            .VkSamplerYcbcrConversionInfo = nullptr
        };
        bd->FontImage->Init(fontDesc);

        // Create the Descriptor Set:
        bd->FontDescriptorSet = static_cast<VkDescriptorSet>(AddTexture(bd->FontSampler, bd->FontImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

        // Create the Upload Buffer:
        {
            const TRAP::Graphics::RendererAPI::BufferDesc bufferDesc
            {
                .Size = upload_size,
                .Alignment = 0,
                .MemoryUsage = TRAP::Graphics::RendererAPI::ResourceMemoryUsage::CPUToGPU,
                .Flags = TRAP::Graphics::RendererAPI::BufferCreationFlags::HostVisible,
                .QueueType = TRAP::Graphics::RendererAPI::QueueType::Graphics,
                .StartState = TRAP::Graphics::RendererAPI::ResourceState::Undefined,
                .FirstElement = 0,
                .ElementCount = 0,
                .StructStride = 0,
                .ICBDrawType = {},
                .ICBMaxVertexBufferBind = 0,
                .ICBMaxFragmentBufferBind = 0,
                .Format = TRAP::Graphics::API::ImageFormat::Undefined,
                .Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Undefined,
                .Name = "ImGui UploadBuffer"
            };
            bd->UploadBuffer = TRAP::MakeRef<TRAP::Graphics::API::VulkanBuffer>(bufferDesc);
        }

        // Upload to Buffer:
        {
            u8* map = nullptr;
            const TRAP::Graphics::RendererAPI::ReadRange readRange{.Offset = 0, .Range = upload_size};
            bd->UploadBuffer->MapBuffer(&readRange);
            map = reinterpret_cast<u8*>(bd->UploadBuffer->GetCPUMappedAddress());
            std::copy_n(pixels, upload_size, map);
            bd->UploadBuffer->UnMapBuffer();
        }

        // Copy to Image:
        {
            std::vector<TRAP::Graphics::RendererAPI::TextureBarrier> fontImageBarriers
            {
                TRAP::Graphics::RendererAPI::TextureBarrier
                {
                    .Texture = dynamic_cast<TRAP::Graphics::Texture*>(bd->FontImage.get()),
                    .CurrentState = TRAP::Graphics::RendererAPI::ResourceState::Undefined,
                    .NewState = TRAP::Graphics::RendererAPI::ResourceState::CopyDestination,
                    .BeginOnly = false,
                    .EndOnly = false,
                    .Acquire = false,
                    .Release = false,
                    .QueueType = TRAP::Graphics::RendererAPI::QueueType::Graphics,
                    .SubresourceBarrier = false,
                    .MipLevel = 0,
                    .ArrayLayer = 0
                }
            };
            command_buffer.ResourceBarrier(nullptr, &fontImageBarriers, nullptr);

            //TODO This currently only supports tightly packed (using image extent) textures
            const TRAP::Graphics::RendererAPI::SubresourceDataDesc subresourceDesc
            {
                .SrcOffset = 0,
                .MipLevel = 0,
                .ArrayLayer = 0,
                .RowPitch = NumericCast<u32>(width) * bd->FontImage->GetBytesPerPixel(),
                .SlicePitch = 0
            };
            command_buffer.UpdateSubresource(bd->FontImage.get(), dynamic_pointer_cast<TRAP::Graphics::Buffer>(bd->UploadBuffer), subresourceDesc);

            fontImageBarriers[0].CurrentState = TRAP::Graphics::RendererAPI::ResourceState::CopyDestination;
            fontImageBarriers[0].NewState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
            command_buffer.ResourceBarrier(nullptr, &fontImageBarriers, nullptr);
        }

        // Store our identifier
        io.Fonts->SetTexID(static_cast<ImTextureID>(bd->FontDescriptorSet));
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyFontsTexture()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        ImGui_ImplVulkan_Data* const bd = GetBackendData();

        bd->FontImage.reset();
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateDeviceObjects()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        ImGui_ImplVulkan_Data* const bd = GetBackendData();
        const InitInfo& v = bd->VulkanInitInfo;
        VkResult err = VK_SUCCESS;

        if (bd->FontSampler == nullptr)
        {
            // Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling.
            static constexpr TRAP::Graphics::RendererAPI::SamplerDesc samplerDesc
            {
                .MinFilter = TRAP::Graphics::RendererAPI::FilterType::Linear,
                .MagFilter = TRAP::Graphics::RendererAPI::FilterType::Linear,
                .MipMapMode = TRAP::Graphics::RendererAPI::MipMapMode::Linear,
                .AddressU = TRAP::Graphics::RendererAPI::AddressMode::Repeat,
                .AddressV = TRAP::Graphics::RendererAPI::AddressMode::Repeat,
                .AddressW = TRAP::Graphics::RendererAPI::AddressMode::Repeat,
                .MipLodBias = 0.0f,
                .SetLodRange = true,
                .MinLod = -1000.0f,
                .MaxLod = 1000.0f,
                .EnableAnisotropy = true,
                .OverrideAnisotropyLevel = 1.0f,
                .CompareFunc = TRAP::Graphics::RendererAPI::CompareMode::Never,
                .SamplerConversionDesc = {}
            };
            bd->FontSampler = TRAP::MakeRef<TRAP::Graphics::API::VulkanSampler>(samplerDesc);
        }

        if (bd->DescriptorSetLayout == nullptr)
        {
            static constexpr VkDescriptorSetLayoutBinding binding
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            };
            static constexpr VkDescriptorSetLayoutCreateInfo info
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .bindingCount = 1,
                .pBindings = &binding
            };
            err = vkCreateDescriptorSetLayout(v.Device->GetVkDevice(), &info, v.Allocator, &bd->DescriptorSetLayout);
            CheckVkResult(err);
        }

        if (bd->PipelineLayout == nullptr)
        {
            // Constants: we are using 'vec2 offset' and 'vec2 scale' instead of a full 3d projection matrix
            static constexpr VkPushConstantRange push_constants
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = sizeof(f32) * 0,
                .size = sizeof(f32) * 4
            };
            VkDescriptorSetLayout set_layout = bd->DescriptorSetLayout;
            const VkPipelineLayoutCreateInfo layout_info = TRAP::Graphics::API::VulkanInits::PipelineLayoutCreateInfo(1, &set_layout, 1, &push_constants);
            err = vkCreatePipelineLayout(v.Device->GetVkDevice(), &layout_info, v.Allocator, &bd->PipelineLayout);
            CheckVkResult(err);
        }

        CreatePipeline(*v.Device, v.Allocator, v.PipelineCache.get(), bd->RenderPass, v.MSAASamples, bd->Pipeline);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyDeviceObjects()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        ImGui_ImplVulkan_Data* const bd = GetBackendData();
        const InitInfo& v = bd->VulkanInitInfo;
        DestroyAllViewportsRenderBuffers();
        DestroyFontUploadObjects();

        if (bd->ShaderModuleVert != nullptr)     { vkDestroyShaderModule(v.Device->GetVkDevice(), bd->ShaderModuleVert, v.Allocator); bd->ShaderModuleVert = VK_NULL_HANDLE; }
        if (bd->ShaderModuleFrag != nullptr)     { vkDestroyShaderModule(v.Device->GetVkDevice(), bd->ShaderModuleFrag, v.Allocator); bd->ShaderModuleFrag = VK_NULL_HANDLE; }
        bd->FontImage.reset();
        bd->FontSampler.reset();
        if (bd->DescriptorSetLayout != nullptr)  { vkDestroyDescriptorSetLayout(v.Device->GetVkDevice(), bd->DescriptorSetLayout, v.Allocator); bd->DescriptorSetLayout = VK_NULL_HANDLE; }
        if (bd->PipelineLayout != nullptr)       { vkDestroyPipelineLayout(v.Device->GetVkDevice(), bd->PipelineLayout, v.Allocator); bd->PipelineLayout = VK_NULL_HANDLE; }
        if (bd->Pipeline != nullptr)             { vkDestroyPipeline(v.Device->GetVkDevice(), bd->Pipeline, v.Allocator); bd->Pipeline = VK_NULL_HANDLE; }
    }

    //-------------------------------------------------------------------------
    // Internal / Miscellaneous Vulkan Helpers
    // (Used by example's main.cpp. Used by multi-viewport features. PROBABLY NOT used by your own app.)
    //-------------------------------------------------------------------------
    // You probably do NOT need to use or care about those functions.
    // Those functions only exist because:
    //   1) they facilitate the readability and maintenance of the multiple main.cpp examples files.
    //   2) the upcoming multi-viewport feature will need them internally.
    // Generally we avoid exposing any kind of superfluous high-level helpers in the backends,
    // but it is too much code to duplicate everywhere so we exceptionally expose them.
    //
    // Your engine/app will likely _already_ have code to setup all that stuff (swap chain, render pass, frame buffers, etc.).
    // You may read this code to learn about Vulkan, but it is recommended you use you own custom tailored code to do equivalent work.
    // (The ImGui_ImplVulkanH_XXX functions do not interact with any of the state used by the regular ImGui_ImplVulkan_XXX functions)
    //-------------------------------------------------------------------------

    [[nodiscard]] VkSurfaceFormatKHR SelectSurfaceFormat(const TRAP::Graphics::API::VulkanPhysicalDevice& physical_device,
                                                         VkSurfaceKHR surface, const std::span<const VkFormat> request_formats,
                                                         const VkColorSpaceKHR request_color_space)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(!request_formats.empty(), "ImGuiVulkanBackend::SelectSurfaceFormat(): request_formats is empty!");

        // Per Spec Format and View Format are expected to be the same unless VK_IMAGE_CREATE_MUTABLE_BIT was set at image creation
        // Assuming that the default behavior is without setting this bit, there is no need for separate Swapchain image and image view format
        // Additionally several new color spaces were introduced with Vulkan Spec v1.0.40,
        // hence we must make sure that a format with the mostly available color space, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, is found and used.
        u32 avail_count = 0;
        CheckVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device.GetVkPhysicalDevice(), surface, &avail_count, nullptr));
        ImVector<VkSurfaceFormatKHR> avail_format;
        avail_format.resize(NumericCast<i32>(avail_count));
        CheckVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device.GetVkPhysicalDevice(), surface, &avail_count, avail_format.Data));

        // First check if only one format, VK_FORMAT_UNDEFINED, is available, which would imply that any format is available
        if (avail_format.size() == 1)
        {
            if (avail_format[0].format == VK_FORMAT_UNDEFINED)
            {
                return VkSurfaceFormatKHR
                {
                    .format = request_formats[0],
                    .colorSpace = request_color_space
                };
            }

            // No point in searching another format
            return avail_format[0];
        }

        // Request several formats, the first found will be used
        for (const VkFormat request_format : request_formats)
        {
            for(const VkSurfaceFormatKHR& surface_format : avail_format)
            {
                if (surface_format.format == request_format && surface_format.colorSpace == request_color_space)
                    return surface_format;
            }
        }

        // If none of the requested image formats could be found, use the first available
        return avail_format[0];
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] VkPresentModeKHR SelectPresentMode(const TRAP::Graphics::API::VulkanPhysicalDevice& physical_device,
                                                     VkSurfaceKHR surface,
                                                     const std::span<const VkPresentModeKHR> request_modes)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(!request_modes.empty(), "ImGuiVulkanBackend::SelectPresentMode(): request_modes is empty!");

        // Request a certain mode and confirm that it is available. If not use VK_PRESENT_MODE_FIFO_KHR which is mandatory
        u32 avail_count = 0;
        CheckVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device.GetVkPhysicalDevice(), surface, &avail_count, nullptr));
        ImVector<VkPresentModeKHR> avail_modes;
        avail_modes.resize(NumericCast<i32>(avail_count));
        CheckVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device.GetVkPhysicalDevice(), surface, &avail_count, avail_modes.Data));

        for (const VkPresentModeKHR request_mode : request_modes)
        {
            for (const VkPresentModeKHR avail_mode : avail_modes)
            {
                if (request_mode == avail_mode)
                    return request_mode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR; // Always available
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateWindowCommandBuffers(ImGui_ImplVulkanH_Window& wd,
                                    const TRAP::Ref<TRAP::Graphics::API::VulkanQueue>& queue)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        // Create Command Buffers
        for (u32 i = 0; i < wd.ImageCount; i++)
        {
            ImGui_ImplVulkanH_Frame& fd = wd.Frames[i];
            ImGui_ImplVulkanH_FrameSemaphores& fsd = wd.FrameSemaphores[i];
            const TRAP::Graphics::RendererAPI::CommandPoolDesc cmdPoolDesc
            {
                .Queue = queue,
                .Transient = true
            };
            fd.CommandPool = TRAP::MakeScope<TRAP::Graphics::API::VulkanCommandPool>(cmdPoolDesc);
            fd.CommandBuffer = dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>(fd.CommandPool->AllocateCommandBuffer(false));

            fd.Fence = TRAP::MakeRef<TRAP::Graphics::API::VulkanFence>();

            fsd.ImageAcquiredSemaphore = TRAP::MakeRef<TRAP::Graphics::API::VulkanSemaphore>();
            fsd.RenderCompleteSemaphore = TRAP::MakeRef<TRAP::Graphics::API::VulkanSemaphore>();
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] std::optional<i32> GetMinImageCountFromPresentMode(const VkPresentModeKHR present_mode)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return 3;
        if (present_mode == VK_PRESENT_MODE_FIFO_KHR || present_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
            return 2;
        if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            return 1;

        TRAP_ASSERT(false, "ImGuiVulkanBackend::GetMinImageCountFromPresentMode(): Unable to retrieve min image count from present mode!");
        return std::nullopt;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    // Also destroy old swap chain and in-flight frames data, if any.
    void CreateWindowSwapChain(const TRAP::Graphics::API::VulkanDevice& device, ImGui_ImplVulkanH_Window& wd,
                               const VkAllocationCallbacks* const allocator, const i32 w, const i32 h,
                               u32 min_image_count)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        VkResult err = VK_SUCCESS;
        VkSwapchainKHR old_swapchain = wd.Swapchain;
        wd.Swapchain = VK_NULL_HANDLE;
        device.WaitIdle();
        CheckVkResult(err);

        // We don't use DestroyWindow() because we want to preserve the old swapchain to create the new one.
        // Destroy old Framebuffer
        for (u32 i = 0; i < wd.ImageCount; i++)
            DestroyFrame(device, wd.Frames[i], allocator);
        wd.Frames.clear();
        wd.FrameSemaphores.clear();
        wd.ImageCount = 0;
        if (wd.RenderPass != nullptr)
            vkDestroyRenderPass(device.GetVkDevice(), wd.RenderPass, allocator);
        if (wd.Pipeline != nullptr)
            vkDestroyPipeline(device.GetVkDevice(), wd.Pipeline, allocator);

        // If min image count was not specified, request different count of images dependent on selected present mode
        if (min_image_count == 0)
            min_image_count = NumericCast<u32>(GetMinImageCountFromPresentMode(wd.PresentMode).value_or(1u));

        // Create Swapchain
        {
            VkSwapchainCreateInfoKHR info = TRAP::Graphics::API::VulkanInits::SwapchainCreateInfoKHR(wd.Surface, min_image_count, wd.SurfaceFormat, {}, VK_SHARING_MODE_EXCLUSIVE, {}, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, wd.PresentMode);
            info.oldSwapchain = old_swapchain;

            VkSurfaceCapabilitiesKHR cap;
            err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysicalDevice()->GetVkPhysicalDevice(), wd.Surface, &cap);
            CheckVkResult(err);
            if (info.minImageCount < cap.minImageCount)
                info.minImageCount = cap.minImageCount;
            else if (cap.maxImageCount != 0 && info.minImageCount > cap.maxImageCount)
                info.minImageCount = cap.maxImageCount;

            if (cap.currentExtent.width == 0xFFFFFFFF)
            {
                wd.Width = w;
                wd.Height = h;

                info.imageExtent.width = NumericCast<u32>(w);
                info.imageExtent.height = NumericCast<u32>(h);
            }
            else
            {
                wd.Width = NumericCast<i32>(cap.currentExtent.width);
                wd.Height = NumericCast<i32>(cap.currentExtent.height);

                info.imageExtent.width = cap.currentExtent.width;
                info.imageExtent.height = cap.currentExtent.height;
            }
            err = vkCreateSwapchainKHR(device.GetVkDevice(), &info, allocator, &wd.Swapchain);
            CheckVkResult(err);
            err = vkGetSwapchainImagesKHR(device.GetVkDevice(), wd.Swapchain, &wd.ImageCount, nullptr);
            CheckVkResult(err);
            TRAP_ASSERT(wd.ImageCount >= min_image_count, "ImGuiVulkanBackend::CreateWindowSwapChain(): wd.ImageCount must be greater than or equal to min_image_count!");
            std::vector<VkImage> backbuffers(wd.ImageCount);
            err = vkGetSwapchainImagesKHR(device.GetVkDevice(), wd.Swapchain, &wd.ImageCount, backbuffers.data());
            CheckVkResult(err);

            TRAP_ASSERT(wd.Frames.empty(), "ImGuiVulkanBackend::CreateWindowSwapChain(): wd.Frames is not empty!");
            TRAP_ASSERT(wd.FrameSemaphores.empty(), "ImGuiVulkanBackend::CreateWindowSwapChain(): wd.FrameSemaphores is not empty!");
            wd.Frames.resize(wd.ImageCount);
            wd.FrameSemaphores.resize(wd.ImageCount);
            for (u32 i = 0; i < wd.ImageCount; i++)
            {
                const TRAP::Graphics::RendererAPI::RenderTargetDesc rtDesc
                {
                    .Flags = TRAP::Graphics::RendererAPI::TextureCreationFlags::None,
                    .Width = NumericCast<u32>(wd.Width),
                    .Height = NumericCast<u32>(wd.Height),
                    .Depth = 1,
                    .ArraySize = 1,
                    .MipLevels = 0,
                    .SampleCount = TRAP::Graphics::RendererAPI::SampleCount::One,
                    .Format = TRAP::Graphics::API::ImageFormatFromVkFormat(wd.SurfaceFormat.format),
                    .StartState = TRAP::Graphics::RendererAPI::ResourceState::Present,
                    .ClearValue = {},
                    .SampleQuality = 0,
                    .Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Undefined,
                    .Name = fmt::format("ImGui Backbuffer {}", i),
                    .NativeHandle = backbuffers[i]
                };
                wd.Frames[i].Backbuffer = TRAP::MakeRef<TRAP::Graphics::API::VulkanRenderTarget>(rtDesc);
            }
        }
        if (old_swapchain != nullptr)
            vkDestroySwapchainKHR(device.GetVkDevice(), old_swapchain, allocator);

        // Create the Render Pass
        if(!wd.UseDynamicRendering)
        {
            const VkAttachmentDescription attachment = TRAP::Graphics::API::VulkanInits::AttachmentDescription(wd.SurfaceFormat.format, VK_SAMPLE_COUNT_1_BIT,
                                                                                                               wd.ClearEnable ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                                                                                               VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                                                                               VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            static constexpr VkAttachmentReference attachmentRef
            {
                .attachment = 0,
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };

            const std::vector<VkAttachmentDescription> attachmentDescs{attachment};
	        const std::vector<VkAttachmentReference> attachmentRefs{attachmentRef};
	        const VkSubpassDescription subpass = TRAP::Graphics::API::VulkanInits::SubPassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, {}, attachmentRefs);
            const VkRenderPassCreateInfo info = TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(attachmentDescs, subpass);

            err = vkCreateRenderPass(device.GetVkDevice(), &info, allocator, &wd.RenderPass);
            CheckVkResult(err);

            //Secondary viewports in multi-viewport mode may want to create their own pipelines.
            //This fixes the RenderPass incompatibility coming from the bd->RenderPass
            //which was first passed on to ImGui_ImplVulkan_Init().
            CreatePipeline(device, allocator, VK_NULL_HANDLE, wd.RenderPass, VK_SAMPLE_COUNT_1_BIT, wd.Pipeline);
        }

        // Create Framebuffer
        if(!wd.UseDynamicRendering)
        {
            VkFramebufferCreateInfo info = TRAP::Graphics::API::VulkanInits::FramebufferCreateInfo(wd.RenderPass, {}, NumericCast<u32>(wd.Width), NumericCast<u32>(wd.Height), 1);
            info.attachmentCount = 1;
            for (u32 i = 0; i < wd.ImageCount; i++)
            {
                ImGui_ImplVulkanH_Frame& fd = wd.Frames[i];
                VkImageView backbufferView = fd.Backbuffer->GetVkImageView();
                info.pAttachments = &backbufferView;
                err = vkCreateFramebuffer(device.GetVkDevice(), &info, allocator, &fd.Framebuffer);
                CheckVkResult(err);
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    // Create or resize window
    void CreateOrResizeWindow(const TRAP::Graphics::API::VulkanDevice& device,
                              ImGui_ImplVulkanH_Window& wnd,
                              const TRAP::Ref<TRAP::Graphics::API::VulkanQueue>& queue,
                              const VkAllocationCallbacks* const allocator, const i32 w, const i32 h,
                              const u32 min_image_count)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(queue != nullptr, "ImGuiVulkanBackend::CreateOrResizeWindow(): queue is nullptr!");

        CreateWindowSwapChain(device, wnd, allocator, w, h, min_image_count);
        CreateWindowCommandBuffers(wnd, queue);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyWindow(const TRAP::Graphics::API::VulkanInstance& instance,
                       const TRAP::Graphics::API::VulkanDevice& device, ImGui_ImplVulkanH_Window& wnd,
                       const VkAllocationCallbacks* const allocator)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        device.WaitIdle(); // FIXME: We could wait on the Queue if we had the queue in wd-> (otherwise VulkanH functions can't use globals)
        //vkQueueWaitIdle(bd.Queue);

        for (u32 i = 0; i < wnd.ImageCount; i++)
            DestroyFrame(device, wnd.Frames[i], allocator);
        wnd.Frames.clear();
        wnd.FrameSemaphores.clear();
        vkDestroyPipeline(device.GetVkDevice(), wnd.Pipeline, allocator);
        vkDestroyRenderPass(device.GetVkDevice(), wnd.RenderPass, allocator);
        vkDestroySwapchainKHR(device.GetVkDevice(), wnd.Swapchain, allocator);
        vkDestroySurfaceKHR(instance.GetVkInstance(), wnd.Surface, allocator);

        wnd = {};
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyFrame(const TRAP::Graphics::API::VulkanDevice& device, ImGui_ImplVulkanH_Frame& fd,
                      const VkAllocationCallbacks* const allocator)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        fd.Fence.reset();
        fd.CommandPool.reset();
        fd.CommandBuffer = nullptr;

        fd.Backbuffer.reset();
        vkDestroyFramebuffer(device.GetVkDevice(), fd.Framebuffer, allocator);
        fd.Framebuffer = VK_NULL_HANDLE;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateNewDescriptorPool()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        ImGui_ImplVulkan_Data* const bd = GetBackendData();
        InitInfo& v = bd->VulkanInitInfo;

        const VkDescriptorPoolCreateInfo poolInfo = TRAP::Graphics::API::VulkanInits::DescriptorPoolCreateInfo(v.DescriptorPoolSizes,
                                                                                                               1000);

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        CheckVkResult(vkCreateDescriptorPool(v.Device->GetVkDevice(), &poolInfo, nullptr, &descriptorPool));

        bd->DescriptorPools.push_back(descriptorPool);
        v.DescriptorPool = descriptorPool;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyAllViewportsRenderBuffers()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        for(ImGuiViewport* viewport : platform_io.Viewports)
        {
            if(viewport == nullptr)
                continue;

            if (ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(viewport->RendererUserData); vd)
                vd->RenderBuffers = {};
        }
    }

    //--------------------------------------------------------------------------------------------------------
    // MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
    // This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports simultaneously.
    // If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
    //--------------------------------------------------------------------------------------------------------

    void CreateWindow(ImGuiViewport* const viewport)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(viewport != nullptr, "ImGuiVulkanBackend::CreateWindow(): viewport is nullptr!");

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        ImGui_ImplVulkan_ViewportData* const vd = IM_NEW(ImGui_ImplVulkan_ViewportData)();
        viewport->RendererUserData = vd;
        ImGui_ImplVulkanH_Window& wd = vd->Window;
        const InitInfo& v = bd->VulkanInitInfo;

        // Create surface
        const ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        const VkResult err = static_cast<VkResult>(platform_io.Platform_CreateVkSurface(viewport, std::bit_cast<ImU64>(v.Instance->GetVkInstance()), static_cast<const void*>(v.Allocator), reinterpret_cast<ImU64*>(&wd.Surface)));
        CheckVkResult(err);

        // Check for WSI support
        VkBool32 res = VK_SUCCESS;
        CheckVkResult(vkGetPhysicalDeviceSurfaceSupportKHR(v.Device->GetPhysicalDevice()->GetVkPhysicalDevice(), v.Queue->GetQueueFamilyIndex(), wd.Surface, &res));
        if (res != VK_TRUE)
        {
            TRAP_ASSERT(false, "ImGuiVulkanBackend::CreateWindow(): VulkanPhysicalDevice has no WSI support!");
            return;
        }

        // Select Surface Format
        static constexpr std::array<VkFormat, 4> requestSurfaceImageFormat{ VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
        static constexpr VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        wd.SurfaceFormat = SelectSurfaceFormat(*v.Device->GetPhysicalDevice(), wd.Surface, requestSurfaceImageFormat, requestSurfaceColorSpace);

        // Select Present Mode
        // FIXME-VULKAN: Even thought mailbox seems to get us maximum framerate with a single window, it halves framerate with a second window etc. (w/ Nvidia and SDK 1.82.1)
        static constexpr std::array<VkPresentModeKHR, 3> present_modes{ VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
        wd.PresentMode = SelectPresentMode(*v.Device->GetPhysicalDevice(), wd.Surface, present_modes);

        // Create SwapChain, RenderPass, Framebuffer, etc.
        wd.ClearEnable = (viewport->Flags & ImGuiViewportFlags_NoRendererClear) == 0;
        wd.UseDynamicRendering = v.UseDynamicRendering;
        CreateOrResizeWindow(*v.Device, wd, v.Queue, v.Allocator, NumericCast<i32>(viewport->Size.x), NumericCast<i32>(viewport->Size.y), v.MinImageCount);
        vd->WindowOwned = true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyWindow(ImGuiViewport* const viewport)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(viewport != nullptr, "ImGuiVulkanBackend::DestroyWindow(): viewport is nullptr!");
        if(viewport == nullptr)
            return;

        // The main viewport (owned by the application) will always have RendererUserData == nullptr since we didn't create the data for it.
        if (ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(viewport->RendererUserData); vd)
        {
            const ImGui_ImplVulkan_Data* const bd = GetBackendData();
            const InitInfo& v = bd->VulkanInitInfo;
            if (vd->WindowOwned)
                DestroyWindow(*v.Instance, *v.Device, vd->Window, v.Allocator);
            vd->RenderBuffers = {};
            IM_DELETE(vd);
        }
        viewport->RendererUserData = nullptr;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void SetWindowSize(ImGuiViewport* const viewport, const ImVec2 size)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(viewport != nullptr, "ImGuiVulkanBackend::SetWindowSize(): viewport is nullptr!");
        if(viewport == nullptr)
            return;

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        if(ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(viewport->RendererUserData); vd)
        {
            const InitInfo& v = bd->VulkanInitInfo;
            vd->Window.ClearEnable = (viewport->Flags & ImGuiViewportFlags_NoRendererClear) == 0;
            CreateOrResizeWindow(*v.Device, vd->Window, v.Queue, v.Allocator, NumericCast<i32>(size.x), NumericCast<i32>(size.y), v.MinImageCount);
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void RenderWindow(ImGuiViewport* const viewport, [[maybe_unused]] void* const render_arg)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(viewport != nullptr, "ImGuiVulkanBackend::RenderWindow(): viewport is nullptr!");
        if(viewport == nullptr)
            return;

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(viewport->RendererUserData);
        TRAP_ASSERT(vd != nullptr, "ImGuiVulkanBackend::RenderWindow(): vd is nullptr!");
        if(vd == nullptr)
            return;

        ImGui_ImplVulkanH_Window& wd = vd->Window;
        const InitInfo& v = bd->VulkanInitInfo;
        VkResult err = VK_SUCCESS;

        const ImGui_ImplVulkanH_FrameSemaphores& fsd = wd.FrameSemaphores[wd.SemaphoreIndex];
        {
            {
                err = vkAcquireNextImageKHR(v.Device->GetVkDevice(), wd.Swapchain, std::numeric_limits<u64>::max(), fsd.ImageAcquiredSemaphore->GetVkSemaphore(), VK_NULL_HANDLE, &wd.FrameIndex);
                CheckVkResult(err);
            }

            const ImGui_ImplVulkanH_Frame& fd = wd.Frames[wd.FrameIndex];

            fd.Fence->Wait();
            {
                fd.CommandPool->Reset();
                fd.CommandBuffer->Begin();
            }
            {
                static constexpr ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
                std::copy_n(&clear_color.x, 4, &wd.ClearValue.color.float32[0]);
            }
    #ifdef IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING
            if(v.UseDynamicRendering)
            {
                //Transition swapchain image to a layout suitable for drawing.
                std::vector<TRAP::Graphics::RendererAPI::TextureBarrier> barriers
                {
                    TRAP::Graphics::RendererAPI::TextureBarrier
                    {
                        .Texture = dynamic_cast<TRAP::Graphics::Texture*>(fd.Backbuffer->GetTexture().get()),
                        .CurrentState = TRAP::Graphics::RendererAPI::ResourceState::Undefined,
                        .NewState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource,
                        .BeginOnly = false,
                        .EndOnly = false,
                        .Acquire = false,
                        .Release = false,
                        .QueueType = TRAP::Graphics::RendererAPI::QueueType::Graphics,
                        .SubresourceBarrier = false,
                        .MipLevel = 0,
                        .ArrayLayer = 0
                    }
                };
                fd.CommandBuffer->ResourceBarrier(nullptr, &barriers, nullptr);

                const VkRenderingAttachmentInfo attachmentInfo =
                {
                    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                    .pNext = nullptr,
                    .imageView = fd.Backbuffer->GetVkImageView(),
                    .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    .resolveMode = VK_RESOLVE_MODE_NONE,
                    .resolveImageView = VK_NULL_HANDLE,
                    .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                    .clearValue = wd.ClearValue
                };

                const VkRenderingInfo renderingInfo
                {
                    .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
                    .pNext = nullptr,
                    .flags = 0,
                    .renderArea = VkRect2D{.offset = VkOffset2D{.x = 0, .y = 0},
                                           .extent = VkExtent2D{.width = NumericCast<u32>(wd.Width), .height = NumericCast<u32>(wd.Height)}},
                    .layerCount = 1,
                    .viewMask = 0,
                    .colorAttachmentCount = 1,
                    .pColorAttachments = &attachmentInfo,
                    .pDepthAttachment = nullptr,
                    .pStencilAttachment = nullptr
                };

                vkCmdBeginRenderingKHR(fd.CommandBuffer->GetVkCommandBuffer(), &renderingInfo);
            }
            else
    #endif /*IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING*/
            {
                const VkRect2D renderArea
                {
                    .offset{.x = 0, .y = 0},
                    .extent{.width = NumericCast<u32>(wd.Width), .height = NumericCast<u32>(wd.Height)}
                };
                const VkRenderPassBeginInfo info = TRAP::Graphics::API::VulkanInits::RenderPassBeginInfo(wd.RenderPass, fd.Framebuffer,
                                                                                                        renderArea, (viewport->Flags & ImGuiViewportFlags_NoRendererClear) != 0 ? std::vector<VkClearValue>{} : std::vector<VkClearValue>{wd.ClearValue});

                vkCmdBeginRenderPass(fd.CommandBuffer->GetVkCommandBuffer(), &info, VK_SUBPASS_CONTENTS_INLINE);
            }
        }

        const ImGui_ImplVulkanH_Frame& fd = wd.Frames[wd.FrameIndex];

        TRAP_ASSERT(viewport->DrawData != nullptr, "ImGuiVulkanBackend::RenderWindow(): viewport->DrawData is nullptr!");
        TRAP_ASSERT(fd.CommandBuffer != nullptr, "ImGuiVulkanBackend::RenderWindow(): fd.CommandBuffer is nullptr!");
        RenderDrawData(*viewport->DrawData, *fd.CommandBuffer, wd.Pipeline);

        {
    #ifdef IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING
            if(v.UseDynamicRendering)
            {
                vkCmdEndRenderingKHR(fd.CommandBuffer->GetVkCommandBuffer());

                //Transition image to a layout suitable for presentation
                std::vector<TRAP::Graphics::RendererAPI::TextureBarrier> barriers
                {
                    TRAP::Graphics::RendererAPI::TextureBarrier
                    {
                        .Texture = dynamic_cast<TRAP::Graphics::Texture*>(fd.Backbuffer->GetTexture().get()),
                        .CurrentState = TRAP::Graphics::RendererAPI::ResourceState::ShaderResource,
                        .NewState = TRAP::Graphics::RendererAPI::ResourceState::Present,
                        .BeginOnly = false,
                        .EndOnly = false,
                        .Acquire = false,
                        .Release = false,
                        .QueueType = TRAP::Graphics::RendererAPI::QueueType::Graphics,
                        .SubresourceBarrier = false,
                        .MipLevel = 0,
                        .ArrayLayer = 0
                    }
                };
                fd.CommandBuffer->ResourceBarrier(nullptr, &barriers, nullptr);
            }
            else
    #endif /*IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING*/
            {
                vkCmdEndRenderPass(fd.CommandBuffer->GetVkCommandBuffer());
            }
            {
                static constexpr VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                VkSemaphore imageAcquiredSemaphore = fsd.ImageAcquiredSemaphore->GetVkSemaphore();
                VkSemaphore renderCompleteSemaphore = fsd.RenderCompleteSemaphore->GetVkSemaphore();
                VkCommandBuffer cmdBuffer = fd.CommandBuffer->GetVkCommandBuffer();
                const VkSubmitInfo info
                {
                    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    .pNext = nullptr,
                    .waitSemaphoreCount = 1,
                    .pWaitSemaphores = &imageAcquiredSemaphore,
                    .pWaitDstStageMask = &wait_stage,
                    .commandBufferCount = 1,
                    .pCommandBuffers = &cmdBuffer,
                    .signalSemaphoreCount = 1,
                    .pSignalSemaphores = &renderCompleteSemaphore
                };

                VkFence fence = fd.Fence->GetVkFence();

                fd.CommandBuffer->End();
                err = vkResetFences(v.Device->GetVkDevice(), 1, &fence);
                CheckVkResult(err);
                err = vkQueueSubmit(v.Queue->GetVkQueue(), 1, &info, fence);
                CheckVkResult(err);
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void SwapBuffers(ImGuiViewport* const viewport, [[maybe_unused]] void* const render_arg)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(viewport != nullptr, "ImGuiVulkanBackend::SwapBuffers(): viewport is nullptr!");
        if(viewport == nullptr)
            return;

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();

        ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(viewport->RendererUserData);
        TRAP_ASSERT(vd != nullptr, "ImGuiVulkanBackend::SwapBuffers(): vd is nullptr!");
        if(vd == nullptr)
            return;

        ImGui_ImplVulkanH_Window& wd = vd->Window;
        const InitInfo& v = bd->VulkanInitInfo;

        VkResult err = VK_SUCCESS;
        u32 present_index = wd.FrameIndex;

        const ImGui_ImplVulkanH_FrameSemaphores& fsd = wd.FrameSemaphores[wd.SemaphoreIndex];
        const std::vector<VkSemaphore> waitSemaphores{fsd.RenderCompleteSemaphore->GetVkSemaphore()};
        const VkPresentInfoKHR info = TRAP::Graphics::API::VulkanInits::PresentInfo(waitSemaphores, wd.Swapchain, present_index);

        err = vkQueuePresentKHR(v.Queue->GetVkQueue(), &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
            CreateOrResizeWindow(*v.Device, vd->Window, v.Queue, v.Allocator, NumericCast<i32>(viewport->Size.x), NumericCast<i32>(viewport->Size.y), v.MinImageCount);
        else
            CheckVkResult(err);

        wd.FrameIndex = (wd.FrameIndex + 1) % wd.ImageCount;         // This is for the next vkWaitForFences()
        wd.SemaphoreIndex = (wd.SemaphoreIndex + 1) % wd.ImageCount; // Now we can use the next set of semaphores
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void InitPlatformInterface()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        if ((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
            TRAP_ASSERT(platform_io.Platform_CreateVkSurface != nullptr, "ImGuiVulkanBackend::InitPlatformInterface(): Platform needs to setup the CreateVkSurface handler!");
        platform_io.Renderer_CreateWindow = CreateWindow;
        platform_io.Renderer_DestroyWindow = DestroyWindow;
        platform_io.Renderer_SetWindowSize = SetWindowSize;
        platform_io.Renderer_RenderWindow = RenderWindow;
        platform_io.Renderer_SwapBuffers = SwapBuffers;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void ShutdownPlatformInterface()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        ImGui::DestroyPlatformWindows();
    }
}

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

// Render function
void ImGui::INTERNAL::Vulkan::RenderDrawData(const ImDrawData& draw_data,
                                             const TRAP::Graphics::API::VulkanCommandBuffer& command_buffer,
                                             VkPipeline pipeline)
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    const i32 fb_width = NumericCast<i32>(draw_data.DisplaySize.x * draw_data.FramebufferScale.x);
    const i32 fb_height = NumericCast<i32>(draw_data.DisplaySize.y * draw_data.FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    const ImGui_ImplVulkan_Data* const bd = GetBackendData();
    const InitInfo& v = bd->VulkanInitInfo;
    if (pipeline == VK_NULL_HANDLE)
        pipeline = bd->Pipeline;

    // Allocate array to store enough vertex/index buffers. Each unique viewport gets its own storage.
    ImGui_ImplVulkan_ViewportData* const viewport_renderer_data = static_cast<ImGui_ImplVulkan_ViewportData*>(draw_data.OwnerViewport->RendererUserData);
    TRAP_ASSERT(viewport_renderer_data != nullptr, "ImGuiVulkanBackend::RenderDrawData(): viewport_renderer_data is nullptr!");
    if(viewport_renderer_data == nullptr)
        return;

    ImGui_ImplVulkanH_WindowRenderBuffers& wrb = viewport_renderer_data->RenderBuffers;
    if (wrb.FrameRenderBuffers.empty())
    {
        wrb.Index = 0;
        wrb.Count = v.ImageCount;
        wrb.FrameRenderBuffers.resize(wrb.Count);
    }
    TRAP_ASSERT(wrb.Count == v.ImageCount, "ImGuiVulkanBackend::RenderDrawData(): wrb->Count doesnt match v->ImageCount!");
    wrb.Index = (wrb.Index + 1) % wrb.Count;
    ImGui_ImplVulkanH_FrameRenderBuffers& rb = wrb.FrameRenderBuffers[wrb.Index];

    if (draw_data.TotalVtxCount > 0)
    {
        // Create or resize the vertex/index buffers
        const usize vertex_size = NumericCast<usize>(draw_data.TotalVtxCount) * sizeof(ImDrawVert);
        const usize index_size = NumericCast<usize>(draw_data.TotalIdxCount) * sizeof(ImDrawIdx);
        if (rb.VertexBuffer == nullptr || rb.VertexBuffer->GetSize() < vertex_size)
        {
            const TRAP::Graphics::RendererAPI::BufferDesc bufferDesc
            {
                .Size = vertex_size,
                .Alignment = 0,
                .MemoryUsage = TRAP::Graphics::RendererAPI::ResourceMemoryUsage::CPUToGPU,
                .Flags = TRAP::Graphics::RendererAPI::BufferCreationFlags::HostVisible,
                .QueueType = TRAP::Graphics::RendererAPI::QueueType::Graphics,
                .StartState = TRAP::Graphics::RendererAPI::ResourceState::Undefined,
                .FirstElement = 0,
                .ElementCount = 0,
                .StructStride = 0,
                .ICBDrawType = {},
                .ICBMaxVertexBufferBind = 0,
                .ICBMaxFragmentBufferBind = 0,
                .Format = TRAP::Graphics::API::ImageFormat::Undefined,
                .Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::VertexBuffer,
                .Name = "ImGui VertexBuffer"
            };
            rb.VertexBuffer = TRAP::MakeRef<TRAP::Graphics::API::VulkanBuffer>(bufferDesc);
        }
        if (rb.IndexBuffer == VK_NULL_HANDLE || rb.IndexBuffer->GetSize() < index_size)
        {
            const TRAP::Graphics::RendererAPI::BufferDesc bufferDesc
            {
                .Size = index_size,
                .Alignment = 0,
                .MemoryUsage = TRAP::Graphics::RendererAPI::ResourceMemoryUsage::CPUToGPU,
                .Flags = TRAP::Graphics::RendererAPI::BufferCreationFlags::HostVisible,
                .QueueType = TRAP::Graphics::RendererAPI::QueueType::Graphics,
                .StartState = TRAP::Graphics::RendererAPI::ResourceState::Undefined,
                .FirstElement = 0,
                .ElementCount = 0,
                .StructStride = 0,
                .ICBDrawType = {},
                .ICBMaxVertexBufferBind = 0,
                .ICBMaxFragmentBufferBind = 0,
                .Format = TRAP::Graphics::API::ImageFormat::Undefined,
                .Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::IndexBuffer,
                .Name = "ImGui IndexBuffer"
            };
            rb.IndexBuffer = TRAP::MakeRef<TRAP::Graphics::API::VulkanBuffer>(bufferDesc);
        }

        // Upload vertex/index data into a single contiguous GPU buffer
        const TRAP::Graphics::RendererAPI::ReadRange vtxReadRange{.Offset = 0, .Range = vertex_size};
        const TRAP::Graphics::RendererAPI::ReadRange idxReadRange{.Offset = 0, .Range = index_size};
        rb.VertexBuffer->MapBuffer(&vtxReadRange);
        rb.IndexBuffer->MapBuffer(&idxReadRange);
        ImDrawVert* vtx_dst = reinterpret_cast<ImDrawVert*>(rb.VertexBuffer->GetCPUMappedAddress());
        ImDrawIdx* idx_dst = reinterpret_cast<ImDrawIdx*>(rb.IndexBuffer->GetCPUMappedAddress());
        for (i32 n = 0; n < draw_data.CmdListsCount; n++)
        {
            if(const ImDrawList* const cmd_list = draw_data.CmdLists[n]; cmd_list)
            {
                std::copy_n(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size, vtx_dst);
                std::copy_n(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size, idx_dst);
                vtx_dst += cmd_list->VtxBuffer.Size;
                idx_dst += cmd_list->IdxBuffer.Size;
            }
        }
        rb.IndexBuffer->UnMapBuffer();
        rb.VertexBuffer->UnMapBuffer();
    }

    // Setup desired Vulkan state
    SetupRenderState(draw_data, pipeline, command_buffer, rb, fb_width, fb_height);

    // Will project scissor/clipping rectangles into framebuffer space
    const ImVec2 clip_off = draw_data.DisplayPos;         // (0,0) unless using multi-viewports
    const ImVec2 clip_scale = draw_data.FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    i32 global_vtx_offset = 0;
    u32 global_idx_offset = 0;
    for (i32 n = 0; n < draw_data.CmdListsCount; n++)
    {
        const ImDrawList* const cmd_list = draw_data.CmdLists[n];
        if(cmd_list == nullptr)
            continue;

        for (i32 cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd& pcmd = cmd_list->CmdBuffer[cmd_i];
            if (pcmd.UserCallback != nullptr)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd.UserCallback == ImDrawCallback_ResetRenderState)
                    SetupRenderState(draw_data, pipeline, command_buffer, rb, fb_width, fb_height);
                else
                    pcmd.UserCallback(cmd_list, &pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd.ClipRect.x - clip_off.x) * clip_scale.x, (pcmd.ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd.ClipRect.z - clip_off.x) * clip_scale.x, (pcmd.ClipRect.w - clip_off.y) * clip_scale.y);

                // Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
                if (clip_min.x < 0.0f)
                    clip_min.x = 0.0f;
                if (clip_min.y < 0.0f)
                    clip_min.y = 0.0f;
                if (clip_max.x > NumericCast<f32>(fb_width))
                    clip_max.x = NumericCast<f32>(fb_width);
                if (clip_max.y > NumericCast<f32>(fb_height))
                    clip_max.y = NumericCast<f32>(fb_height);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply scissor/clipping rectangle
                command_buffer.SetScissor(NumericCast<u32>(clip_min.x), NumericCast<u32>(clip_min.y), NumericCast<u32>(TRAP::Math::Abs(clip_max.x - clip_min.x)), NumericCast<u32>(TRAP::Math::Abs(clip_max.y - clip_min.y)));

                // Bind descriptorset with font or user texture
                VkDescriptorSet descSet = static_cast<VkDescriptorSet>(pcmd.TextureId);
                if constexpr(sizeof(ImTextureID) < sizeof(ImU64))
                {
                    // We don't support texture switches if ImTextureID hasn't been redefined to be 64-bit.
                    // Do a flaky check that other textures haven't been used.
                    TRAP_ASSERT(pcmd.TextureId == static_cast<ImTextureID>(bd->FontDescriptorSet), "ImGuiVulkanBackend::RenderDrawData(): ImTextureID must be at least 64 bits!");
                    descSet = bd->FontDescriptorSet;
                }
                vkCmdBindDescriptorSets(command_buffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, bd->PipelineLayout, 0, 1, &descSet, 0, nullptr);

                // Draw
                command_buffer.DrawIndexed(pcmd.ElemCount, pcmd.IdxOffset + global_idx_offset, NumericCast<i32>(pcmd.VtxOffset) + global_vtx_offset);
            }
        }
        global_idx_offset += NumericCast<u32>(cmd_list->IdxBuffer.Size);
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Note: at this point both vkCmdSetViewport() and vkCmdSetScissor() have been called.
    // Our last values will leak into user/application rendering IF:
    // - Your app uses a pipeline with VK_DYNAMIC_STATE_VIEWPORT or VK_DYNAMIC_STATE_SCISSOR dynamic state
    // - And you forgot to call vkCmdSetViewport() and vkCmdSetScissor() yourself to explicitly set that state.
    // If you use VK_DYNAMIC_STATE_VIEWPORT or VK_DYNAMIC_STATE_SCISSOR you are responsible for setting the values before rendering.
    // In theory we should aim to backup/restore those values but I am not sure this is possible.
    // We perform a call to vkCmdSetScissor() to set back a full viewport which is likely to fix things for 99% users but technically this is not perfect. (See github #4644)
    command_buffer.SetScissor(0, 0, NumericCast<u32>(fb_width), NumericCast<u32>(fb_height));
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::INTERNAL::Vulkan::UploadFontsTexture()
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    //Destroy old font
    DestroyFontsTexture();

    const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData(TRAP::Application::GetWindow());
    //Execute a GPU command to upload ImGui font textures
    TRAP::Graphics::CommandBuffer* const cmd = viewportData.GraphicCommandPools[viewportData.ImageIndex]->AllocateCommandBuffer(false);
    TRAP_ASSERT(cmd != nullptr, "ImGuiVulkanBackend::UploadFontsTexture(): cmd is nullptr!");
    cmd->Begin();
    CreateFontsTexture(*dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>(cmd));
    cmd->End();

    const TRAP::Ref<TRAP::Graphics::Fence> submitFence = TRAP::Graphics::Fence::Create();
    const TRAP::Graphics::RendererAPI::QueueSubmitDesc submitDesc
    {
        .Cmds = {cmd},
        .SignalFence = submitFence,
        .WaitSemaphores = {},
        .SignalSemaphores = {}
    };

    ImGui_ImplVulkan_Data* const bd = GetBackendData();
    const InitInfo& v = bd->VulkanInitInfo;

    v.Queue->Submit(submitDesc);
    submitFence->Wait();
    viewportData.GraphicCommandPools[viewportData.ImageIndex]->FreeCommandBuffer(cmd);
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::INTERNAL::Vulkan::DestroyFontUploadObjects()
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    ImGui_ImplVulkan_Data* const bd = GetBackendData();
    if (bd->UploadBuffer != VK_NULL_HANDLE)
        bd->UploadBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::INTERNAL::Vulkan::Init(const InitInfo& info, VkRenderPass render_pass)
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    ImGuiIO& io = ImGui::GetIO();
    TRAP_ASSERT(io.BackendRendererUserData == nullptr, "ImGuiVulkanBackend::Init(): Renderer backend is already initialized!");

    // Setup backend capabilities flags
    ImGui_ImplVulkan_Data* const bd = IM_NEW(ImGui_ImplVulkan_Data)();
    io.BackendRendererUserData = bd;
    io.BackendRendererName = "TRAP_Vulkan";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

    TRAP_ASSERT(info.Instance != nullptr, "ImGuiVulkanBackend::Init(): info.Instance is nullptr!");
    TRAP_ASSERT(info.Device != nullptr, "ImGuiVulkanBackend::Init(): info.Device is nullptr!");
    TRAP_ASSERT(info.Queue != nullptr, "ImGuiVulkanBackend::Init(): info.Queue is VK_NULL_HANDLE!");
    TRAP_ASSERT(!info.DescriptorPoolSizes.empty(), "ImGuiVulkanBackend::Init(): info.DescriptorPoolSizes must not be empty!");
    TRAP_ASSERT(info.MinImageCount >= 2, "ImGuiVulkanBackend::Init(): info.MinImageCount must be at least 2!");
    TRAP_ASSERT(info.ImageCount >= info.MinImageCount, "ImGuiVulkanBackend::Init(): info.ImageCount muste be equivalent to info.MinImageCount or bigger!");
    if(!info.UseDynamicRendering)
        TRAP_ASSERT(render_pass != VK_NULL_HANDLE, "ImGuiVulkanBackend::Init(): info.RenderPass is VK_NULL_HANDLE!");

    bd->VulkanInitInfo = info;
    bd->RenderPass = render_pass;

    CreateDeviceObjects();

    if(info.DescriptorPool != VK_NULL_HANDLE)
        bd->DescriptorPools.push_back(info.DescriptorPool);
    else
        CreateNewDescriptorPool();

    // Our render function expect RendererUserData to be storing the window render buffer we need (for the main viewport we won't use ->Window)
    ImGuiViewport* const main_viewport = ImGui::GetMainViewport();
    TRAP_ASSERT(main_viewport != nullptr, "ImGuiVulkanBackend::Init(): main_viewport is nullptr!");
    main_viewport->RendererUserData = IM_NEW(ImGui_ImplVulkan_ViewportData)();

    if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
        InitPlatformInterface();
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::INTERNAL::Vulkan::Shutdown()
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    ImGui_ImplVulkan_Data* const bd = GetBackendData();

    TRAP_ASSERT(bd != nullptr, "ImGuiVulkanBackend::Shutdown(): Renderer backend is already shutdown!");
    ImGuiIO& io = ImGui::GetIO();
    const InitInfo& v = bd->VulkanInitInfo;

    // First destroy objects in all viewports
    DestroyDeviceObjects();

    for(VkDescriptorPool& descPool : bd->DescriptorPools)
        vkDestroyDescriptorPool(v.Device->GetVkDevice(), descPool, v.Allocator);
    bd->DescriptorPools.clear();

    // Manually delete main viewport render data in-case we haven't initialized for viewports
    ImGuiViewport* const main_viewport = ImGui::GetMainViewport();
    TRAP_ASSERT(main_viewport != nullptr, "ImGuiVulkanBackend::Shutdown(): main_viewport is nullptr!");
    if(main_viewport != nullptr)
    {
        if (ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(main_viewport->RendererUserData); vd)
            IM_DELETE(vd);
    }
    main_viewport->RendererUserData = nullptr;

    // Clean up windows
    ShutdownPlatformInterface();

    ClearCache();

    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    IM_DELETE(bd);
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::INTERNAL::Vulkan::NewFrame()
{
    ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    [[maybe_unused]] const ImGui_ImplVulkan_Data* const bd = GetBackendData();
    TRAP_ASSERT(bd != nullptr, "ImGuiVulkanBackend::NewFrame(): Renderer backend hasn't been initialized!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ImTextureID ImGui::INTERNAL::Vulkan::AddTexture(const TRAP::Ref<TRAP::Graphics::API::VulkanSampler>& sampler,
                                                              const TRAP::Ref<TRAP::Graphics::API::VulkanTexture>& image,
                                                              const VkImageLayout image_layout)
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    TRAP_ASSERT(sampler != nullptr, "ImGuiVulkanBackend::AddTexture(): sampler is nullptr!");
    TRAP_ASSERT(image != nullptr, "ImGuiVulkanBackend::AddTexture(): image is nullptr!");

    const auto it = s_VulkanTextureCache.find(image);
    if(it != s_VulkanTextureCache.end())
        return reinterpret_cast<ImTextureID>(it->second.DescriptorSet);

    VulkanTextureCacheEntry& vulkanDetails = s_VulkanTextureCache[image];
    vulkanDetails.Sampler = sampler;
    vulkanDetails.ImageLayout = image_layout;

    const ImGui_ImplVulkan_Data* const bd = GetBackendData();
    const InitInfo& v = bd->VulkanInitInfo;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

    // Create Descriptor Set:
    {
        VkDescriptorSetAllocateInfo allocInfo = TRAP::Graphics::API::VulkanInits::DescriptorSetAllocateInfo(v.DescriptorPool, bd->DescriptorSetLayout);
        VkResult err = vkAllocateDescriptorSets(v.Device->GetVkDevice(), &allocInfo, &descriptorSet);
        // CheckVkResult(err);

        if(descriptorSet == VK_NULL_HANDLE)
        {
            CreateNewDescriptorPool();

            //Try again now with the new pool
            allocInfo.descriptorPool = v.DescriptorPool;
            err = vkAllocateDescriptorSets(v.Device->GetVkDevice(), &allocInfo, &descriptorSet);
            CheckVkResult(err);
        }
    }

    vulkanDetails.DescriptorSet = descriptorSet;
    UpdateTextureInfo(vulkanDetails.DescriptorSet, *vulkanDetails.Sampler, image->GetSRVVkImageView(), vulkanDetails.ImageLayout);

    return reinterpret_cast<ImTextureID>(vulkanDetails.DescriptorSet);
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::INTERNAL::Vulkan::RemoveTexture(const TRAP::Ref<TRAP::Graphics::API::VulkanTexture>& image)
{
    const auto it = s_VulkanTextureCache.find(image);

    if(it == s_VulkanTextureCache.end())
        return;

    s_VulkanTextureCache.erase(it);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ImTextureID ImGui::INTERNAL::Vulkan::UpdateTextureInfo(VkDescriptorSet descriptorSet,
                                                                     const TRAP::Graphics::API::VulkanSampler& sampler,
                                                                     VkImageView image_view,
                                                                     const VkImageLayout image_layout)
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    const ImGui_ImplVulkan_Data* const bd = GetBackendData();
    const InitInfo& v = bd->VulkanInitInfo;

    // Update Descriptor Set:
    {
        const VkDescriptorImageInfo imageInfo
        {
            .sampler = sampler.GetVkSampler(),
            .imageView = image_view,
            .imageLayout = image_layout
        };
        const VkWriteDescriptorSet writeDesc
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = descriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfo,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
        };
        vkUpdateDescriptorSets(v.Device->GetVkDevice(), 1, &writeDesc, 0, nullptr);
    }

    return reinterpret_cast<ImTextureID>(descriptorSet);
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::INTERNAL::Vulkan::ClearCache() noexcept
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    s_VulkanTextureCache.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui::INTERNAL::Vulkan::SetMSAASamples(const VkSampleCountFlagBits sampleCount)
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData(TRAP::Application::GetWindow());
    ImGui_ImplVulkan_Data* const bd = GetBackendData();
    InitInfo& v = bd->VulkanInitInfo;

    if(sampleCount == v.MSAASamples)
        return;

    v.MSAASamples = sampleCount;
    bd->RenderPass = dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
		(
			viewportData.GraphicCommandBuffers[viewportData.ImageIndex]
		)->GetActiveVkRenderPass();

    //Delete old pipeline
    if (bd->Pipeline != nullptr)
    {
        v.Device->WaitIdle();
        vkDestroyPipeline(v.Device->GetVkDevice(), bd->Pipeline, v.Allocator);
        bd->Pipeline = VK_NULL_HANDLE;
    }

    //Create new pipeline
    CreatePipeline(*v.Device, v.Allocator, v.PipelineCache.get(), bd->RenderPass, v.MSAASamples, bd->Pipeline);
}

#endif /*TRAP_HEADLESS_MODE*/

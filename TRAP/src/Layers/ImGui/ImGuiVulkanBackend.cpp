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
#include "Graphics/API/Vulkan/Objects/VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Objects/Fence.h"
#include "Utils/ErrorCodes/ErrorCodes.h"

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
    constexpr std::array<uint32_t, 324> glsl_shader_vert_spv =
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
    constexpr std::array<uint32_t, 193> glsl_shader_frag_spv =
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
    // (Used by multi-viewport features.)
    struct ImGui_ImplVulkanH_Window
    {
        int32_t             Width{};
        int32_t             Height{};
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

    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    struct VkDetails
    {
        VkSampler Sampler;
        VkImageView ImageView;
        VkImageLayout ImageLayout;
        VkDescriptorSet DescriptorSet;
    };
    std::unordered_map<std::uintptr_t, VkDetails> s_VulkanTextureCache{};

    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    // Reusable buffers used for rendering 1 current in-flight frame, for ImGui_ImplVulkan_RenderDrawData()
    // [Please zero-clear before use!]
    struct ImGui_ImplVulkanH_FrameRenderBuffers
    {
        VkDeviceMemory      VertexBufferMemory = VK_NULL_HANDLE;
        VkDeviceMemory      IndexBufferMemory = VK_NULL_HANDLE;
        VkDeviceSize        VertexBufferSize = 0;
        VkDeviceSize        IndexBufferSize = 0;
        VkBuffer            VertexBuffer = VK_NULL_HANDLE;
        VkBuffer            IndexBuffer = VK_NULL_HANDLE;
    };

    //-------------------------------------------------------------------------------------------------------------------//

    // Each viewport will hold 1 ImGui_ImplVulkanH_WindowRenderBuffers
    // [Please zero-clear before use!]
    struct ImGui_ImplVulkanH_WindowRenderBuffers
    {
        uint32_t            Index = 0;
        uint32_t            Count = 0;
        ImGui_ImplVulkanH_FrameRenderBuffers*   FrameRenderBuffers = nullptr;
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
        ImGui_ImplVulkan_InitInfo   VulkanInitInfo{};
        VkRenderPass                RenderPass = VK_NULL_HANDLE;
        VkDeviceSize                BufferMemoryAlignment = 256;
        VkPipelineCreateFlags       PipelineCreateFlags{};
        VkDescriptorSetLayout       DescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout            PipelineLayout = VK_NULL_HANDLE;
        VkDescriptorSet             DescriptorSet = VK_NULL_HANDLE;
        VkPipeline                  Pipeline = VK_NULL_HANDLE;
        VkShaderModule              ShaderModuleVert = VK_NULL_HANDLE;
        VkShaderModule              ShaderModuleFrag = VK_NULL_HANDLE;

        // Font data
        VkSampler                   FontSampler = VK_NULL_HANDLE;
        VkDeviceMemory              FontMemory = VK_NULL_HANDLE;
        VkImage                     FontImage = VK_NULL_HANDLE;
        VkImageView                 FontView = VK_NULL_HANDLE;
        VkDescriptorSet             FontDescriptorSet = VK_NULL_HANDLE;
        VkDeviceMemory              UploadBufferMemory = VK_NULL_HANDLE;
        VkBuffer                    UploadBuffer = VK_NULL_HANDLE;

        // Render buffers for main window
        ImGui_ImplVulkanH_WindowRenderBuffers MainWindowRenderBuffers{};

        std::vector<VkDescriptorPool> DescriptorPools;
    };

    //-------------------------------------------------------------------------------------------------------------------//
    //Forward Declarations-----------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] ImGui_ImplVulkan_Data* GetBackendData();
    [[nodiscard]] uint32_t RetrieveMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits);
    void CheckVkResult(VkResult err);
    void CreateOrResizeBuffer(VkBuffer& buffer, VkDeviceMemory& buffer_memory, VkDeviceSize& p_buffer_size,
                              std::size_t new_size, VkBufferUsageFlagBits usage);
    void SetupRenderState(const ImDrawData* draw_data, VkPipeline pipeline, VkCommandBuffer command_buffer,
                          const ImGui_ImplVulkanH_FrameRenderBuffers* rb, int32_t fb_width, int32_t fb_height);
    void CreateShaderModules(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                             const VkAllocationCallbacks* allocator);
    void CreatePipeline(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                         const VkAllocationCallbacks* allocator,
                                         const TRAP::Ref<TRAP::Graphics::API::VulkanPipelineCache>& pipelineCache,
                                         VkRenderPass renderPass, VkSampleCountFlagBits MSAASamples,
                                         VkPipeline* pipeline);
    void CreateFontsTexture(VkCommandBuffer command_buffer);
    void DestroyFontsTexture();
    void CreateDeviceObjects();
    void DestroyDeviceObjects();

    [[nodiscard]] VkSurfaceFormatKHR SelectSurfaceFormat(const TRAP::Graphics::API::VulkanPhysicalDevice* physical_device,
                                                         VkSurfaceKHR surface, const VkFormat* request_formats,
                                                         int32_t request_formats_count,
                                                         VkColorSpaceKHR request_color_space);
    [[nodiscard]] VkPresentModeKHR SelectPresentMode(const TRAP::Graphics::API::VulkanPhysicalDevice* physical_device,
                                                     VkSurfaceKHR surface, const VkPresentModeKHR* request_modes,
                                                     int32_t request_modes_count);
    void CreateWindowCommandBuffers(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                    const ImGui_ImplVulkanH_Window* wd, uint32_t queue_family,
                                    const VkAllocationCallbacks* allocator);
    [[nodiscard]] std::optional<int32_t> GetMinImageCountFromPresentMode(VkPresentModeKHR present_mode);
    void CreateWindowSwapChain(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                               ImGui_ImplVulkanH_Window* wd, const VkAllocationCallbacks* allocator,
                               int32_t w, int32_t h, uint32_t min_image_count);
    void CreateOrResizeWindow(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                              ImGui_ImplVulkanH_Window* wnd, uint32_t queue_family,
                              const VkAllocationCallbacks* allocator, int32_t w, int32_t h, uint32_t min_image_count);
    void DestroyWindow(const TRAP::Ref<TRAP::Graphics::API::VulkanInstance>& instance,
                       const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device, ImGui_ImplVulkanH_Window* wnd,
                       const VkAllocationCallbacks* allocator);
    void DestroyFrame(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device, ImGui_ImplVulkanH_Frame* fd,
                      const VkAllocationCallbacks* allocator);
    void DestroyFrameSemaphores(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                ImGui_ImplVulkanH_FrameSemaphores* fsd, const VkAllocationCallbacks* allocator);
    void DestroyFrameRenderBuffers(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                   ImGui_ImplVulkanH_FrameRenderBuffers* buffers,
                                   const VkAllocationCallbacks* allocator);
    void DestroyWindowRenderBuffers(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                    ImGui_ImplVulkanH_WindowRenderBuffers* buffers,
                                    const VkAllocationCallbacks* allocator);
    void CreateNewDescriptorPool();
    void DestroyAllViewportsRenderBuffers(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                                            const VkAllocationCallbacks* allocator);

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

        return ImGui::GetCurrentContext() != nullptr ? static_cast<ImGui_ImplVulkan_Data*>(ImGui::GetIO().BackendRendererUserData) : nullptr;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] uint32_t RetrieveMemoryType(const VkMemoryPropertyFlags properties, const uint32_t type_bits)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;

        VkPhysicalDeviceMemoryProperties prop;
        vkGetPhysicalDeviceMemoryProperties(v->Device->GetPhysicalDevice()->GetVkPhysicalDevice(), &prop);
        for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
        {
            if ((prop.memoryTypes[i].propertyFlags & properties) == properties && ((type_bits & BIT(i)) != 0u))
                return i;
        }

		TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanNoMatchingMemoryTypeFound);
        return 0;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CheckVkResult(const VkResult err)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        if (bd == nullptr)
            return;
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;
        if (v->CheckVkResultFn != nullptr)
            v->CheckVkResultFn(err);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateOrResizeBuffer(VkBuffer& buffer, VkDeviceMemory& buffer_memory, VkDeviceSize& p_buffer_size,
                              const std::size_t new_size, const VkBufferUsageFlagBits usage)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        ImGui_ImplVulkan_Data* const bd = GetBackendData();
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;
        VkResult err = VK_SUCCESS;

        if (buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(v->Device->GetVkDevice(), buffer, v->Allocator);
        if (buffer_memory != VK_NULL_HANDLE)
            vkFreeMemory(v->Device->GetVkDevice(), buffer_memory, v->Allocator);

        const VkDeviceSize vertex_buffer_size_aligned = ((new_size - 1) / bd->BufferMemoryAlignment + 1) * bd->BufferMemoryAlignment;
        const VkBufferCreateInfo buffer_info = TRAP::Graphics::API::VulkanInits::BufferCreateInfo(vertex_buffer_size_aligned, usage);
        err = vkCreateBuffer(v->Device->GetVkDevice(), &buffer_info, v->Allocator, &buffer);
        CheckVkResult(err);

        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(v->Device->GetVkDevice(), buffer, &req);
        bd->BufferMemoryAlignment = (bd->BufferMemoryAlignment > req.alignment) ? bd->BufferMemoryAlignment : req.alignment;
        const VkMemoryAllocateInfo alloc_info = TRAP::Graphics::API::VulkanInits::MemoryAllocateInfo(req.size, RetrieveMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits));
        err = vkAllocateMemory(v->Device->GetVkDevice(), &alloc_info, v->Allocator, &buffer_memory);
        CheckVkResult(err);

        err = vkBindBufferMemory(v->Device->GetVkDevice(), buffer, buffer_memory, 0);
        CheckVkResult(err);
        p_buffer_size = req.size;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void SetupRenderState(const ImDrawData* const draw_data, VkPipeline pipeline, VkCommandBuffer command_buffer,
                          const ImGui_ImplVulkanH_FrameRenderBuffers* const rb, const int32_t fb_width,
                          const int32_t fb_height)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();

        // Bind pipeline
        {
            vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        }

        // Bind Vertex And Index Buffer:
        if (draw_data->TotalVtxCount > 0)
        {
            VkBuffer vertex_buffers = rb->VertexBuffer;
            static constexpr VkDeviceSize vertex_offset = 0;
            vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffers, &vertex_offset);
            vkCmdBindIndexBuffer(command_buffer, rb->IndexBuffer, 0, sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
        }

        // Setup viewport:
        {
            const VkViewport viewport
            {
                .x = 0,
                .y = 0,
                .width = NumericCast<float>(fb_width),
                .height = NumericCast<float>(fb_height),
                .minDepth = 0.0f,
                .maxDepth = 1.0f
            };
            vkCmdSetViewport(command_buffer, 0, 1, &viewport);
        }

        // Setup scale and translation:
        // Our visible imgui space lies from draw_data->DisplayPps (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
        {
            const std::array<float, 2> scale{2.0f / draw_data->DisplaySize.x, 2.0f / draw_data->DisplaySize.y};
            const std::array<float, 2> translate{-1.0f - draw_data->DisplayPos.x * std::get<0>(scale), -1.0f - draw_data->DisplayPos.y * std::get<1>(scale)};
            vkCmdPushConstants(command_buffer, bd->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 0, sizeof(float) * 2, scale.data());
            vkCmdPushConstants(command_buffer, bd->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 2, sizeof(float) * 2, translate.data());
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateShaderModules(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
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
            const VkResult err = vkCreateShaderModule(device->GetVkDevice(), &vert_info, allocator, &bd->ShaderModuleVert);
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
            const VkResult err = vkCreateShaderModule(device->GetVkDevice(), &frag_info, allocator, &bd->ShaderModuleFrag);
            CheckVkResult(err);
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreatePipeline(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                        const VkAllocationCallbacks* const allocator,
                        const TRAP::Ref<TRAP::Graphics::API::VulkanPipelineCache>& pipelineCache,
                        VkRenderPass renderPass, const VkSampleCountFlagBits MSAASamples, VkPipeline* const pipeline)
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
            .colorWriteMask = static_cast<uint32_t>(VK_COLOR_COMPONENT_R_BIT) | static_cast<uint32_t>(VK_COLOR_COMPONENT_G_BIT) |
                              static_cast<uint32_t>(VK_COLOR_COMPONENT_B_BIT) | static_cast<uint32_t>(VK_COLOR_COMPONENT_A_BIT)
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

        const VkResult err = vkCreateGraphicsPipelines(device->GetVkDevice(),
                                                       pipelineCache ? pipelineCache->GetVkPipelineCache() : VK_NULL_HANDLE,
                                                       1, &info, allocator, pipeline);
        CheckVkResult(err);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateFontsTexture(VkCommandBuffer command_buffer)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplVulkan_Data* const bd = GetBackendData();
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;

        unsigned char* pixels = nullptr;
        int width = 0, height = 0;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        const std::size_t upload_size = NumericCast<std::size_t>(width) * NumericCast<std::size_t>(height) * 4u * sizeof(char);

        VkResult err = VK_SUCCESS;

        // Create the Image:
        {
            const VkImageCreateInfo info = TRAP::Graphics::API::VulkanInits::ImageCreateInfo(VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM,
                                                                                            NumericCast<uint32_t>(width), NumericCast<uint32_t>(height),
                                                                                            1, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
                                                                                            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
            err = vkCreateImage(v->Device->GetVkDevice(), &info, v->Allocator, &bd->FontImage);
            CheckVkResult(err);
            VkMemoryRequirements req;
            vkGetImageMemoryRequirements(v->Device->GetVkDevice(), bd->FontImage, &req);
            const VkMemoryAllocateInfo alloc_info = TRAP::Graphics::API::VulkanInits::MemoryAllocateInfo(req.size, RetrieveMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits));
            err = vkAllocateMemory(v->Device->GetVkDevice(), &alloc_info, v->Allocator, &bd->FontMemory);
            CheckVkResult(err);
            err = vkBindImageMemory(v->Device->GetVkDevice(), bd->FontImage, bd->FontMemory, 0);
            CheckVkResult(err);
        }

        // Create the Image View:
        {
            const VkImageViewCreateInfo info = TRAP::Graphics::API::VulkanInits::ImageViewCreateInfo(bd->FontImage, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 1, 1);
            err = vkCreateImageView(v->Device->GetVkDevice(), &info, v->Allocator, &bd->FontView);
            CheckVkResult(err);
        }

        // Create the Descriptor Set:
        bd->FontDescriptorSet = static_cast<VkDescriptorSet>(ImGui_ImplVulkan_AddTexture(bd->FontSampler, bd->FontView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

        // Create the Upload Buffer:
        {
            const VkBufferCreateInfo buffer_info = TRAP::Graphics::API::VulkanInits::BufferCreateInfo(upload_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
            err = vkCreateBuffer(v->Device->GetVkDevice(), &buffer_info, v->Allocator, &bd->UploadBuffer);
            CheckVkResult(err);
            VkMemoryRequirements req;
            vkGetBufferMemoryRequirements(v->Device->GetVkDevice(), bd->UploadBuffer, &req);
            bd->BufferMemoryAlignment = (bd->BufferMemoryAlignment > req.alignment) ? bd->BufferMemoryAlignment : req.alignment;
            const VkMemoryAllocateInfo alloc_info = TRAP::Graphics::API::VulkanInits::MemoryAllocateInfo(req.size, RetrieveMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits));
            err = vkAllocateMemory(v->Device->GetVkDevice(), &alloc_info, v->Allocator, &bd->UploadBufferMemory);
            CheckVkResult(err);
            err = vkBindBufferMemory(v->Device->GetVkDevice(), bd->UploadBuffer, bd->UploadBufferMemory, 0);
            CheckVkResult(err);
        }

        // Upload to Buffer:
        {
            char* map = nullptr;
            err = vkMapMemory(v->Device->GetVkDevice(), bd->UploadBufferMemory, 0, upload_size, 0, reinterpret_cast<void**>(&map));
            CheckVkResult(err);
            std::copy_n(pixels, upload_size, map);

            const VkMappedMemoryRange range
            {
                .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                .pNext = nullptr,
                .memory = bd->UploadBufferMemory,
                .offset = 0,
                .size = upload_size
            };

            err = vkFlushMappedMemoryRanges(v->Device->GetVkDevice(), 1, &range);
            CheckVkResult(err);
            vkUnmapMemory(v->Device->GetVkDevice(), bd->UploadBufferMemory);
        }

        // Copy to Image:
        {
            static constexpr VkImageSubresourceRange subRange
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            };

            const VkImageMemoryBarrier copy_barrier
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = VK_ACCESS_NONE_KHR,
                .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = bd->FontImage,
                .subresourceRange = subRange
            };

            vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &copy_barrier);

            static constexpr VkImageSubresourceLayers layers
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            };
            const VkBufferImageCopy region = TRAP::Graphics::API::VulkanInits::ImageCopy(0, NumericCast<uint32_t>(width), NumericCast<uint32_t>(height), 1, layers);
            vkCmdCopyBufferToImage(command_buffer, bd->UploadBuffer, bd->FontImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

            const VkImageMemoryBarrier use_barrier
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = bd->FontImage,
                .subresourceRange = subRange
            };

            vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &use_barrier);
        }

        // Store our identifier
        io.Fonts->SetTexID(static_cast<ImTextureID>(bd->FontDescriptorSet));
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyFontsTexture()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;

        if(bd->FontImage != nullptr)
            vkDestroyImage(v->Device->GetVkDevice(), bd->FontImage, nullptr);
        if(bd->FontMemory != nullptr)
            vkFreeMemory(v->Device->GetVkDevice(), bd->FontMemory, nullptr);
        if(bd->FontView != nullptr)
            vkDestroyImageView(v->Device->GetVkDevice(), bd->FontView, nullptr);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateDeviceObjects()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        ImGui_ImplVulkan_Data* const bd = GetBackendData();
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;
        VkResult err = VK_SUCCESS;

        if (bd->FontSampler == nullptr)
        {
            // Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling.
            static constexpr VkSamplerCreateInfo info = TRAP::Graphics::API::VulkanInits::SamplerCreateInfo(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR,
                                                                                                            VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                                                                            VK_SAMPLER_ADDRESS_MODE_REPEAT, 0.0f, -1000.0f, 1000.0f, 1.0f,
                                                                                                            VK_COMPARE_OP_NEVER);
            err = vkCreateSampler(v->Device->GetVkDevice(), &info, v->Allocator, &bd->FontSampler);
            CheckVkResult(err);
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
            err = vkCreateDescriptorSetLayout(v->Device->GetVkDevice(), &info, v->Allocator, &bd->DescriptorSetLayout);
            CheckVkResult(err);
        }

        if (bd->PipelineLayout == nullptr)
        {
            // Constants: we are using 'vec2 offset' and 'vec2 scale' instead of a full 3d projection matrix
            static constexpr VkPushConstantRange push_constants
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = sizeof(float) * 0,
                .size = sizeof(float) * 4
            };
            VkDescriptorSetLayout set_layout = bd->DescriptorSetLayout;
            const VkPipelineLayoutCreateInfo layout_info = TRAP::Graphics::API::VulkanInits::PipelineLayoutCreateInfo(1, &set_layout, 1, &push_constants);
            err = vkCreatePipelineLayout(v->Device->GetVkDevice(), &layout_info, v->Allocator, &bd->PipelineLayout);
            CheckVkResult(err);
        }

        CreatePipeline(v->Device, v->Allocator, v->PipelineCache, bd->RenderPass, v->MSAASamples, &bd->Pipeline);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyDeviceObjects()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        ImGui_ImplVulkan_Data* const bd = GetBackendData();
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;
        DestroyAllViewportsRenderBuffers(v->Device, v->Allocator);
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        if (bd->ShaderModuleVert != nullptr)     { vkDestroyShaderModule(v->Device->GetVkDevice(), bd->ShaderModuleVert, v->Allocator); bd->ShaderModuleVert = VK_NULL_HANDLE; }
        if (bd->ShaderModuleFrag != nullptr)     { vkDestroyShaderModule(v->Device->GetVkDevice(), bd->ShaderModuleFrag, v->Allocator); bd->ShaderModuleFrag = VK_NULL_HANDLE; }
        if (bd->FontView != nullptr)             { vkDestroyImageView(v->Device->GetVkDevice(), bd->FontView, v->Allocator); bd->FontView = VK_NULL_HANDLE; }
        if (bd->FontImage != nullptr)            { vkDestroyImage(v->Device->GetVkDevice(), bd->FontImage, v->Allocator); bd->FontImage = VK_NULL_HANDLE; }
        if (bd->FontMemory != nullptr)           { vkFreeMemory(v->Device->GetVkDevice(), bd->FontMemory, v->Allocator); bd->FontMemory = VK_NULL_HANDLE; }
        if (bd->FontSampler != nullptr)          { vkDestroySampler(v->Device->GetVkDevice(), bd->FontSampler, v->Allocator); bd->FontSampler = VK_NULL_HANDLE; }
        if (bd->DescriptorSetLayout != nullptr)  { vkDestroyDescriptorSetLayout(v->Device->GetVkDevice(), bd->DescriptorSetLayout, v->Allocator); bd->DescriptorSetLayout = VK_NULL_HANDLE; }
        if (bd->PipelineLayout != nullptr)       { vkDestroyPipelineLayout(v->Device->GetVkDevice(), bd->PipelineLayout, v->Allocator); bd->PipelineLayout = VK_NULL_HANDLE; }
        if (bd->Pipeline != nullptr)             { vkDestroyPipeline(v->Device->GetVkDevice(), bd->Pipeline, v->Allocator); bd->Pipeline = VK_NULL_HANDLE; }
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

    [[nodiscard]] VkSurfaceFormatKHR SelectSurfaceFormat(const TRAP::Graphics::API::VulkanPhysicalDevice* const physical_device,
                                                         VkSurfaceKHR surface, const VkFormat* const request_formats,
                                                         const int32_t request_formats_count,
                                                         const VkColorSpaceKHR request_color_space)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(request_formats != nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkanH_SelectSurfaceFormat(): request_formats is nullptr!");
        TRAP_ASSERT(request_formats_count > 0, "ImGuiVulkanBackend::ImGui_ImplVulkanH_SelectSurfaceFormat(): request_formats_count is 0!");

        // Per Spec Format and View Format are expected to be the same unless VK_IMAGE_CREATE_MUTABLE_BIT was set at image creation
        // Assuming that the default behavior is without setting this bit, there is no need for separate Swapchain image and image view format
        // Additionally several new color spaces were introduced with Vulkan Spec v1.0.40,
        // hence we must make sure that a format with the mostly available color space, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, is found and used.
        uint32_t avail_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device->GetVkPhysicalDevice(), surface, &avail_count, nullptr);
        ImVector<VkSurfaceFormatKHR> avail_format;
        avail_format.resize(NumericCast<int32_t>(avail_count));
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device->GetVkPhysicalDevice(), surface, &avail_count, avail_format.Data);

        // First check if only one format, VK_FORMAT_UNDEFINED, is available, which would imply that any format is available
        if (avail_count == 1)
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
        for (int request_i = 0; request_i < request_formats_count; request_i++)
        {
            for (int32_t avail_i = 0; NumericCast<uint32_t>(avail_i) < avail_count; avail_i++)
            {
                if (avail_format[avail_i].format == request_formats[request_i] && avail_format[avail_i].colorSpace == request_color_space)
                    return avail_format[avail_i];
            }
        }

        // If none of the requested image formats could be found, use the first available
        return avail_format[0];
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] VkPresentModeKHR SelectPresentMode(const TRAP::Graphics::API::VulkanPhysicalDevice* const physical_device,
                                                     VkSurfaceKHR surface, const VkPresentModeKHR* const request_modes,
                                                     const int32_t request_modes_count)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(request_modes != nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkanH_SelectPresentMode(): request_modes is nullptr!");
        TRAP_ASSERT(request_modes_count > 0, "ImGuiVulkanBackend::ImGui_ImplVulkanH_SelectPresentMode(): request_modes_count is 0!");

        // Request a certain mode and confirm that it is available. If not use VK_PRESENT_MODE_FIFO_KHR which is mandatory
        uint32_t avail_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device->GetVkPhysicalDevice(), surface, &avail_count, nullptr);
        ImVector<VkPresentModeKHR> avail_modes;
        avail_modes.resize(NumericCast<int32_t>(avail_count));
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device->GetVkPhysicalDevice(), surface, &avail_count, avail_modes.Data);

        for (int32_t request_i = 0; request_i < request_modes_count; request_i++)
        {
            for (int32_t avail_i = 0; avail_i < NumericCast<int32_t>(avail_count); avail_i++)
            {
                if (request_modes[request_i] == avail_modes[avail_i])
                    return request_modes[request_i];
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR; // Always available
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateWindowCommandBuffers(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                    const ImGui_ImplVulkanH_Window* const wd, const uint32_t queue_family,
                                    const VkAllocationCallbacks* const allocator)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        TRAP_ASSERT(device != VK_NULL_HANDLE, "ImGuiVulkanBackend::ImGui_ImplVulkanH_CreateWindowCommandBuffers(): device is nullptr!");

        // Create Command Buffers
        VkResult err = VK_SUCCESS;
        for (uint32_t i = 0; i < wd->ImageCount; i++)
        {
            ImGui_ImplVulkanH_Frame* const fd = &wd->Frames[i];
            ImGui_ImplVulkanH_FrameSemaphores* const fsd = &wd->FrameSemaphores[i];
            {
                VkCommandPoolCreateInfo info = TRAP::Graphics::API::VulkanInits::CommandPoolCreateInfo(queue_family);
                info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                err = vkCreateCommandPool(device->GetVkDevice(), &info, allocator, &fd->CommandPool);
                CheckVkResult(err);
            }
            {
                const VkCommandBufferAllocateInfo info = TRAP::Graphics::API::VulkanInits::CommandBufferAllocateInfo(fd->CommandPool, false);
                err = vkAllocateCommandBuffers(device->GetVkDevice(), &info, &fd->CommandBuffer);
                CheckVkResult(err);
            }
            {
                VkFenceCreateInfo info = TRAP::Graphics::API::VulkanInits::FenceCreateInfo();
                info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
                err = vkCreateFence(device->GetVkDevice(), &info, allocator, &fd->Fence);
                CheckVkResult(err);
            }
            {
                static constexpr VkSemaphoreCreateInfo info = TRAP::Graphics::API::VulkanInits::SemaphoreCreateInfo();
                err = vkCreateSemaphore(device->GetVkDevice(), &info, allocator, &fsd->ImageAcquiredSemaphore);
                CheckVkResult(err);
                err = vkCreateSemaphore(device->GetVkDevice(), &info, allocator, &fsd->RenderCompleteSemaphore);
                CheckVkResult(err);
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] std::optional<int32_t> GetMinImageCountFromPresentMode(const VkPresentModeKHR present_mode)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return 3;
        if (present_mode == VK_PRESENT_MODE_FIFO_KHR || present_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
            return 2;
        if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            return 1;

        TRAP_ASSERT(false, "ImGuiVulkanBackend::ImGui_ImplVulkanH_GetMinImageCountFromPresentMode(): Unable to retrieve min image count from present mode!");
        return std::nullopt;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    // Also destroy old swap chain and in-flight frames data, if any.
    void CreateWindowSwapChain(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                               ImGui_ImplVulkanH_Window* const wd, const VkAllocationCallbacks* const allocator,
                               const int32_t w, const int32_t h, uint32_t min_image_count)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        VkResult err = VK_SUCCESS;
        VkSwapchainKHR old_swapchain = wd->Swapchain;
        wd->Swapchain = VK_NULL_HANDLE;
        err = vkDeviceWaitIdle(device->GetVkDevice());
        CheckVkResult(err);

        // We don't use ImGui_ImplVulkanH_DestroyWindow() because we want to preserve the old swapchain to create the new one.
        // Destroy old Framebuffer
        for (uint32_t i = 0; i < wd->ImageCount; i++)
        {
            DestroyFrame(device, &wd->Frames[i], allocator);
            DestroyFrameSemaphores(device, &wd->FrameSemaphores[i], allocator);
        }
        IM_FREE(wd->Frames);
        IM_FREE(wd->FrameSemaphores);
        wd->Frames = nullptr;
        wd->FrameSemaphores = nullptr;
        wd->ImageCount = 0;
        if (wd->RenderPass != nullptr)
            vkDestroyRenderPass(device->GetVkDevice(), wd->RenderPass, allocator);
        if (wd->Pipeline != nullptr)
            vkDestroyPipeline(device->GetVkDevice(), wd->Pipeline, allocator);

        // If min image count was not specified, request different count of images dependent on selected present mode
        if (min_image_count == 0)
            min_image_count = NumericCast<uint32_t>(GetMinImageCountFromPresentMode(wd->PresentMode).value_or(1u));

        // Create Swapchain
        {
            VkSwapchainCreateInfoKHR info = TRAP::Graphics::API::VulkanInits::SwapchainCreateInfoKHR(wd->Surface, min_image_count, wd->SurfaceFormat, {}, VK_SHARING_MODE_EXCLUSIVE, 0, {}, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, wd->PresentMode);
            info.oldSwapchain = old_swapchain;

            VkSurfaceCapabilitiesKHR cap;
            err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->GetVkPhysicalDevice(), wd->Surface, &cap);
            CheckVkResult(err);
            if (info.minImageCount < cap.minImageCount)
                info.minImageCount = cap.minImageCount;
            else if (cap.maxImageCount != 0 && info.minImageCount > cap.maxImageCount)
                info.minImageCount = cap.maxImageCount;

            if (cap.currentExtent.width == 0xffffffff)
            {
                wd->Width = w;
                wd->Height = h;

                info.imageExtent.width = NumericCast<uint32_t>(w);
                info.imageExtent.height = NumericCast<uint32_t>(h);
            }
            else
            {
                wd->Width = NumericCast<int32_t>(cap.currentExtent.width);
                wd->Height = NumericCast<int32_t>(cap.currentExtent.height);

                info.imageExtent.width = cap.currentExtent.width;
                info.imageExtent.height = cap.currentExtent.height;
            }
            err = vkCreateSwapchainKHR(device->GetVkDevice(), &info, allocator, &wd->Swapchain);
            CheckVkResult(err);
            err = vkGetSwapchainImagesKHR(device->GetVkDevice(), wd->Swapchain, &wd->ImageCount, nullptr);
            CheckVkResult(err);
            std::array<VkImage, 16> backbuffers{};
            TRAP_ASSERT(wd->ImageCount >= min_image_count, "ImGuiVulkanBackend::ImGui_ImplVulkanH_CreateWindowSwapChain(): wd->ImageCount must be greater than or equal to min_image_count!");
            TRAP_ASSERT(wd->ImageCount < backbuffers.size(), "ImGuiVulkanBackend::ImGui_ImplVulkanH_CreateWindowSwapChain(): wd->ImageCount must be smaller than backbuffers.size()!");
            err = vkGetSwapchainImagesKHR(device->GetVkDevice(), wd->Swapchain, &wd->ImageCount, backbuffers.data());
            CheckVkResult(err);

            TRAP_ASSERT(wd->Frames == nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkanH_CreateWindowSwapChain(): wd->Frames is nullptr!");
            wd->Frames = static_cast<ImGui_ImplVulkanH_Frame*>(IM_ALLOC(sizeof(ImGui_ImplVulkanH_Frame) * wd->ImageCount));
            wd->FrameSemaphores = static_cast<ImGui_ImplVulkanH_FrameSemaphores*>(IM_ALLOC(sizeof(ImGui_ImplVulkanH_FrameSemaphores) * wd->ImageCount));
            std::fill_n(wd->Frames, wd->ImageCount, ImGui_ImplVulkanH_Frame());
            std::fill_n(wd->FrameSemaphores, wd->ImageCount, ImGui_ImplVulkanH_FrameSemaphores());
            for (uint32_t i = 0; i < wd->ImageCount; i++)
                wd->Frames[i].Backbuffer = backbuffers[i];
        }
        if (old_swapchain != nullptr)
            vkDestroySwapchainKHR(device->GetVkDevice(), old_swapchain, allocator);

        // Create the Render Pass
        if(!wd->UseDynamicRendering)
        {
            const VkAttachmentDescription attachment = TRAP::Graphics::API::VulkanInits::AttachmentDescription(wd->SurfaceFormat.format, VK_SAMPLE_COUNT_1_BIT,
                                                                                                               wd->ClearEnable ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                                                                                               VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                                                                               VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

            std::vector<VkAttachmentDescription> attachmentDescs{attachment};
            const VkRenderPassCreateInfo info = TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(attachmentDescs);

            err = vkCreateRenderPass(device->GetVkDevice(), &info, allocator, &wd->RenderPass);
            CheckVkResult(err);

            //Secondary viewports in multi-viewport mode may want to create their own pipelines.
            //This fixes the RenderPass incompatibility coming from the bd->RenderPass
            //which was first passed on to ImGui_ImplVulkan_Init().
            CreatePipeline(device, allocator, VK_NULL_HANDLE, wd->RenderPass, VK_SAMPLE_COUNT_1_BIT, &wd->Pipeline);
        }

        // Create The Image Views
        {
            VkImageViewCreateInfo info = TRAP::Graphics::API::VulkanInits::ImageViewCreateInfo(VK_NULL_HANDLE, VK_IMAGE_VIEW_TYPE_2D,  wd->SurfaceFormat.format, 1, 1);
            for (uint32_t i = 0; i < wd->ImageCount; i++)
            {
                ImGui_ImplVulkanH_Frame* const fd = &wd->Frames[i];
                info.image = fd->Backbuffer;
                err = vkCreateImageView(device->GetVkDevice(), &info, allocator, &fd->BackbufferView);
                CheckVkResult(err);
            }
        }

        // Create Framebuffer
        if(!wd->UseDynamicRendering)
        {
            VkFramebufferCreateInfo info = TRAP::Graphics::API::VulkanInits::FramebufferCreateInfo(wd->RenderPass, {}, NumericCast<uint32_t>(wd->Width), NumericCast<uint32_t>(wd->Height), 1);
            info.attachmentCount = 1;
            for (uint32_t i = 0; i < wd->ImageCount; i++)
            {
                ImGui_ImplVulkanH_Frame* const fd = &wd->Frames[i];
                info.pAttachments = &fd->BackbufferView;
                err = vkCreateFramebuffer(device->GetVkDevice(), &info, allocator, &fd->Framebuffer);
                CheckVkResult(err);
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    // Create or resize window
    void CreateOrResizeWindow(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                              ImGui_ImplVulkanH_Window* const wnd, const uint32_t queue_family,
                              const VkAllocationCallbacks* const allocator, const int32_t w, const int32_t h,
                              const uint32_t min_image_count)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        CreateWindowSwapChain(device, wnd, allocator, w, h, min_image_count);
        CreateWindowCommandBuffers(device, wnd, queue_family, allocator);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyWindow(const TRAP::Ref<TRAP::Graphics::API::VulkanInstance>& instance,
                       const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device, ImGui_ImplVulkanH_Window* const wnd,
                       const VkAllocationCallbacks* const allocator)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        vkDeviceWaitIdle(device->GetVkDevice()); // FIXME: We could wait on the Queue if we had the queue in wd-> (otherwise VulkanH functions can't use globals)
        //vkQueueWaitIdle(bd->Queue);

        for (uint32_t i = 0; i < wnd->ImageCount; i++)
        {
            DestroyFrame(device, &wnd->Frames[i], allocator);
            DestroyFrameSemaphores(device, &wnd->FrameSemaphores[i], allocator);
        }
        IM_FREE(wnd->Frames);
        IM_FREE(wnd->FrameSemaphores);
        wnd->Frames = nullptr;
        wnd->FrameSemaphores = nullptr;
        vkDestroyPipeline(device->GetVkDevice(), wnd->Pipeline, allocator);
        vkDestroyRenderPass(device->GetVkDevice(), wnd->RenderPass, allocator);
        vkDestroySwapchainKHR(device->GetVkDevice(), wnd->Swapchain, allocator);
        vkDestroySurfaceKHR(instance->GetVkInstance(), wnd->Surface, allocator);

        *wnd = ImGui_ImplVulkanH_Window();
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyFrame(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device, ImGui_ImplVulkanH_Frame* const fd,
                      const VkAllocationCallbacks* const allocator)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        vkDestroyFence(device->GetVkDevice(), fd->Fence, allocator);
        vkFreeCommandBuffers(device->GetVkDevice(), fd->CommandPool, 1, &fd->CommandBuffer);
        vkDestroyCommandPool(device->GetVkDevice(), fd->CommandPool, allocator);
        fd->Fence = VK_NULL_HANDLE;
        fd->CommandBuffer = VK_NULL_HANDLE;
        fd->CommandPool = VK_NULL_HANDLE;

        vkDestroyImageView(device->GetVkDevice(), fd->BackbufferView, allocator);
        fd->BackbufferView = VK_NULL_HANDLE;
        vkDestroyFramebuffer(device->GetVkDevice(), fd->Framebuffer, allocator);
        fd->Framebuffer = VK_NULL_HANDLE;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyFrameSemaphores(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                ImGui_ImplVulkanH_FrameSemaphores* const fsd,
                                const VkAllocationCallbacks* const allocator)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        vkDestroySemaphore(device->GetVkDevice(), fsd->ImageAcquiredSemaphore, allocator);
        vkDestroySemaphore(device->GetVkDevice(), fsd->RenderCompleteSemaphore, allocator);
        fsd->ImageAcquiredSemaphore = fsd->RenderCompleteSemaphore = VK_NULL_HANDLE;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyFrameRenderBuffers(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                   ImGui_ImplVulkanH_FrameRenderBuffers* const buffers,
                                   const VkAllocationCallbacks* const allocator)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        if (buffers->VertexBuffer != nullptr) { vkDestroyBuffer(device->GetVkDevice(), buffers->VertexBuffer, allocator); buffers->VertexBuffer = VK_NULL_HANDLE; }
        if (buffers->VertexBufferMemory != nullptr) { vkFreeMemory(device->GetVkDevice(), buffers->VertexBufferMemory, allocator); buffers->VertexBufferMemory = VK_NULL_HANDLE; }
        if (buffers->IndexBuffer != nullptr) { vkDestroyBuffer(device->GetVkDevice(), buffers->IndexBuffer, allocator); buffers->IndexBuffer = VK_NULL_HANDLE; }
        if (buffers->IndexBufferMemory != nullptr) { vkFreeMemory(device->GetVkDevice(), buffers->IndexBufferMemory, allocator); buffers->IndexBufferMemory = VK_NULL_HANDLE; }
        buffers->VertexBufferSize = 0;
        buffers->IndexBufferSize = 0;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyWindowRenderBuffers(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                    ImGui_ImplVulkanH_WindowRenderBuffers* const buffers,
                                    const VkAllocationCallbacks* const allocator)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        for (uint32_t n = 0; n < buffers->Count; n++)
            DestroyFrameRenderBuffers(device, &buffers->FrameRenderBuffers[n], allocator);
        IM_FREE(buffers->FrameRenderBuffers);
        buffers->FrameRenderBuffers = nullptr;
        buffers->Index = 0;
        buffers->Count = 0;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void CreateNewDescriptorPool()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        ImGui_ImplVulkan_Data* const bd = GetBackendData();
        ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;

        const VkDescriptorPoolCreateInfo poolInfo = TRAP::Graphics::API::VulkanInits::DescriptorPoolCreateInfo(v->DescriptorPoolSizes,
                                                                                                               1000);

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkCall(vkCreateDescriptorPool(v->Device->GetVkDevice(), &poolInfo, nullptr, &descriptorPool));

        bd->DescriptorPools.push_back(descriptorPool);
        v->DescriptorPool = descriptorPool;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyAllViewportsRenderBuffers(const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device,
                                          const VkAllocationCallbacks* const allocator)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        for (int n = 0; n < platform_io.Viewports.Size; n++)
        {
            if (ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(platform_io.Viewports[n]->RendererUserData))
                DestroyWindowRenderBuffers(device, &vd->RenderBuffers, allocator);
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

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        ImGui_ImplVulkan_ViewportData* const vd = IM_NEW(ImGui_ImplVulkan_ViewportData)();
        viewport->RendererUserData = vd;
         ImGui_ImplVulkanH_Window* const wd = &vd->Window;
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;

        // Create surface
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        const VkResult err = static_cast<VkResult>(platform_io.Platform_CreateVkSurface(viewport, std::bit_cast<ImU64>(v->Instance->GetVkInstance()), static_cast<const void*>(v->Allocator), reinterpret_cast<ImU64*>(&wd->Surface)));
        CheckVkResult(err);

        // Check for WSI support
        VkBool32 res = VK_SUCCESS;
        vkGetPhysicalDeviceSurfaceSupportKHR(v->Device->GetPhysicalDevice()->GetVkPhysicalDevice(), v->Queue->GetQueueFamilyIndex(), wd->Surface, &res);
        if (res != VK_TRUE)
        {
            TRAP_ASSERT(false, "ImGuiVulkanBackend::ImGui_ImplVulkan_CreateWindow(): VulkanPhysicalDevice has no WSI support!");
            return;
        }

        // Select Surface Format
        constexpr std::array<VkFormat, 4> requestSurfaceImageFormat{ VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
        constexpr VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        wd->SurfaceFormat = SelectSurfaceFormat(v->Device->GetPhysicalDevice(), wd->Surface, requestSurfaceImageFormat.data(), NumericCast<int32_t>(requestSurfaceImageFormat.size()), requestSurfaceColorSpace);

        // Select Present Mode
        // FIXME-VULKAN: Even thought mailbox seems to get us maximum framerate with a single window, it halves framerate with a second window etc. (w/ Nvidia and SDK 1.82.1)
        constexpr std::array<VkPresentModeKHR, 3> present_modes{ VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
        wd->PresentMode = SelectPresentMode(v->Device->GetPhysicalDevice(), wd->Surface, present_modes.data(), NumericCast<int32_t>(present_modes.size()));

        // Create SwapChain, RenderPass, Framebuffer, etc.
        wd->ClearEnable = (viewport->Flags & ImGuiViewportFlags_NoRendererClear) == 0;
        wd->UseDynamicRendering = v->UseDynamicRendering;
        CreateOrResizeWindow(v->Device, wd, v->Queue->GetQueueFamilyIndex(), v->Allocator, NumericCast<int32_t>(viewport->Size.x), NumericCast<int32_t>(viewport->Size.y), v->MinImageCount);
        vd->WindowOwned = true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void DestroyWindow(ImGuiViewport* const viewport)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        // The main viewport (owned by the application) will always have RendererUserData == nullptr since we didn't create the data for it.
        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        if (ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(viewport->RendererUserData))
        {
            const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;
            if (vd->WindowOwned)
                DestroyWindow(v->Instance, v->Device, &vd->Window, v->Allocator);
            DestroyWindowRenderBuffers(v->Device, &vd->RenderBuffers, v->Allocator);
            IM_DELETE(vd);
        }
        viewport->RendererUserData = nullptr;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void SetWindowSize(ImGuiViewport* const viewport, const ImVec2 size)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(viewport->RendererUserData);
        if (vd == nullptr) // This is nullptr for the main viewport (which is left to the user/app to handle)
            return;
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;
        vd->Window.ClearEnable = (viewport->Flags & ImGuiViewportFlags_NoRendererClear) == 0;
        CreateOrResizeWindow(v->Device, &vd->Window, v->Queue->GetQueueFamilyIndex(), v->Allocator, NumericCast<int32_t>(size.x), NumericCast<int32_t>(size.y), v->MinImageCount);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void RenderWindow(ImGuiViewport* const viewport, [[maybe_unused]] void* const render_arg)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(viewport->RendererUserData);
        ImGui_ImplVulkanH_Window* const wd = &vd->Window;
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;
        VkResult err = VK_SUCCESS;

        const ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
        const ImGui_ImplVulkanH_FrameSemaphores* const fsd = &wd->FrameSemaphores[wd->SemaphoreIndex];
        {
            {
                err = vkAcquireNextImageKHR(v->Device->GetVkDevice(), wd->Swapchain, UINT64_MAX, fsd->ImageAcquiredSemaphore, VK_NULL_HANDLE, &wd->FrameIndex);
                CheckVkResult(err);
                fd = &wd->Frames[wd->FrameIndex];
            }
            for (;;)
            {
                err = vkWaitForFences(v->Device->GetVkDevice(), 1, &fd->Fence, VK_TRUE, 100);
                if (err == VK_SUCCESS) break;
                if (err == VK_TIMEOUT) continue;
                CheckVkResult(err);
            }
            {
                err = vkResetCommandPool(v->Device->GetVkDevice(), fd->CommandPool, 0);
                CheckVkResult(err);
                VkCommandBufferBeginInfo info = TRAP::Graphics::API::VulkanInits::CommandBufferBeginInfo();
                info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
                CheckVkResult(err);
            }
            {
                static constexpr ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
                std::copy_n(&clear_color.x, 4, &wd->ClearValue.color.float32[0]);
            }
    #ifdef IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING
            if(v->UseDynamicRendering)
            {
                //Transition swapchain image to a layout suitable for drawing.
                const VkImageMemoryBarrier barrier
                {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = VK_ACCESS_NONE_KHR,
                    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    .srcQueueFamilyIndex = 0,
                    .dstQueueFamilyIndex = 0,
                    .image = fd->Backbuffer,
                    .subresourceRange = VkImageSubresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}
                };
                vkCmdPipelineBarrier(fd->CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0,
                                    nullptr, 0, nullptr, 1, &barrier);

                const VkRenderingAttachmentInfo attachmentInfo =
                {
                    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                    .pNext = nullptr,
                    .imageView = fd->BackbufferView,
                    .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    .resolveMode = VK_RESOLVE_MODE_NONE,
                    .resolveImageView = VK_NULL_HANDLE,
                    .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                    .clearValue = wd->ClearValue
                };

                const VkRenderingInfo renderingInfo
                {
                    .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
                    .pNext = nullptr,
                    .flags = 0,
                    .renderArea = VkRect2D{.offset = VkOffset2D{.x = 0, .y = 0},
                                           .extent = VkExtent2D{.width = NumericCast<uint32_t>(wd->Width), .height = NumericCast<uint32_t>(wd->Height)}},
                    .layerCount = 1,
                    .viewMask = 0,
                    .colorAttachmentCount = 1,
                    .pColorAttachments = &attachmentInfo,
                    .pDepthAttachment = nullptr,
                    .pStencilAttachment = nullptr
                };

                vkCmdBeginRenderingKHR(fd->CommandBuffer, &renderingInfo);
            }
            else
    #endif /*IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING*/
            {
                const VkRect2D renderArea
                {
                    .offset{.x = 0, .y = 0},
                    .extent{.width = NumericCast<uint32_t>(wd->Width), .height = NumericCast<uint32_t>(wd->Height)}
                };
                const VkRenderPassBeginInfo info = TRAP::Graphics::API::VulkanInits::RenderPassBeginInfo(wd->RenderPass, fd->Framebuffer,
                                                                                                        renderArea, (viewport->Flags & ImGuiViewportFlags_NoRendererClear) != 0 ? std::vector<VkClearValue>{} : std::vector<VkClearValue>{wd->ClearValue});

                vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
            }
        }

        ImGui_ImplVulkan_RenderDrawData(viewport->DrawData, fd->CommandBuffer, wd->Pipeline);

        {
    #ifdef IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING
            if(v->UseDynamicRendering)
            {
                vkCmdEndRenderingKHR(fd->CommandBuffer);

                //Transition image to a layout suitable for presentation
                const VkImageMemoryBarrier barrier
                {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_NONE_KHR,
                    .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    .srcQueueFamilyIndex = 0,
                    .dstQueueFamilyIndex = 0,
                    .image = fd->Backbuffer,
                    .subresourceRange = VkImageSubresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 0}
                };
                vkCmdPipelineBarrier(fd->CommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
            }
            else
    #endif /*IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING*/
            {
                vkCmdEndRenderPass(fd->CommandBuffer);
            }
            {
                constexpr VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                const VkSubmitInfo info
                {
                    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    .pNext = nullptr,
                    .waitSemaphoreCount = 1,
                    .pWaitSemaphores = &fsd->ImageAcquiredSemaphore,
                    .pWaitDstStageMask = &wait_stage,
                    .commandBufferCount = 1,
                    .pCommandBuffers = &fd->CommandBuffer,
                    .signalSemaphoreCount = 1,
                    .pSignalSemaphores = &fsd->RenderCompleteSemaphore
                };

                err = vkEndCommandBuffer(fd->CommandBuffer);
                CheckVkResult(err);
                err = vkResetFences(v->Device->GetVkDevice(), 1, &fd->Fence);
                CheckVkResult(err);
                err = vkQueueSubmit(v->Queue->GetVkQueue(), 1, &info, fd->Fence);
                CheckVkResult(err);
            }
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void SwapBuffers(ImGuiViewport* const viewport, [[maybe_unused]] void* const render_arg)
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

        const ImGui_ImplVulkan_Data* const bd = GetBackendData();
        ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(viewport->RendererUserData);
        ImGui_ImplVulkanH_Window* const wd = &vd->Window;
        const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;

        VkResult err = VK_SUCCESS;
        uint32_t present_index = wd->FrameIndex;

        const ImGui_ImplVulkanH_FrameSemaphores* const fsd = &wd->FrameSemaphores[wd->SemaphoreIndex];
        const std::vector<VkSemaphore> waitSemaphores{fsd->RenderCompleteSemaphore};
        const VkPresentInfoKHR info = TRAP::Graphics::API::VulkanInits::PresentInfo(waitSemaphores, wd->Swapchain, present_index);

        err = vkQueuePresentKHR(v->Queue->GetVkQueue(), &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
            CreateOrResizeWindow(v->Device, &vd->Window, v->Queue->GetQueueFamilyIndex(), v->Allocator, NumericCast<int32_t>(viewport->Size.x), NumericCast<int32_t>(viewport->Size.y), v->MinImageCount);
        else
            CheckVkResult(err);

        wd->FrameIndex = (wd->FrameIndex + 1) % wd->ImageCount;         // This is for the next vkWaitForFences()
        wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void InitPlatformInterface()
    {
        ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        if ((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
            TRAP_ASSERT(platform_io.Platform_CreateVkSurface != nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkan_InitPlatformInterface(): Platform needs to setup the CreateVkSurface handler!");
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
void ImGui_ImplVulkan_RenderDrawData(const ImDrawData* const draw_data, VkCommandBuffer command_buffer, VkPipeline pipeline)
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    const int32_t fb_width = NumericCast<int32_t>(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    const int32_t fb_height = NumericCast<int32_t>(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    const ImGui_ImplVulkan_Data* const bd = GetBackendData();
    const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;
    if (pipeline == VK_NULL_HANDLE)
        pipeline = bd->Pipeline;

    // Allocate array to store enough vertex/index buffers. Each unique viewport gets its own storage.
    ImGui_ImplVulkan_ViewportData* const viewport_renderer_data = static_cast<ImGui_ImplVulkan_ViewportData*>(draw_data->OwnerViewport->RendererUserData);
    TRAP_ASSERT(viewport_renderer_data != nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkan_RenderDrawData(): viewport_renderer_data is nullptr!");
    ImGui_ImplVulkanH_WindowRenderBuffers* const wrb = &viewport_renderer_data->RenderBuffers;
    if (wrb->FrameRenderBuffers == nullptr)
    {
        wrb->Index = 0;
        wrb->Count = v->ImageCount;
        wrb->FrameRenderBuffers = static_cast<ImGui_ImplVulkanH_FrameRenderBuffers*>(IM_ALLOC(sizeof(ImGui_ImplVulkanH_FrameRenderBuffers) * wrb->Count));
        std::fill_n(wrb->FrameRenderBuffers, wrb->Count, ImGui_ImplVulkanH_FrameRenderBuffers());
    }
    TRAP_ASSERT(viewport_renderer_data != nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkan_RenderDrawData(): wrb->Count doesnt match v->ImageCount!");
    wrb->Index = (wrb->Index + 1) % wrb->Count;
    ImGui_ImplVulkanH_FrameRenderBuffers* const rb = &wrb->FrameRenderBuffers[wrb->Index];

    if (draw_data->TotalVtxCount > 0)
    {
        // Create or resize the vertex/index buffers
        const std::size_t vertex_size = NumericCast<std::size_t>(draw_data->TotalVtxCount) * sizeof(ImDrawVert);
        const std::size_t index_size = NumericCast<std::size_t>(draw_data->TotalIdxCount) * sizeof(ImDrawIdx);
        if (rb->VertexBuffer == VK_NULL_HANDLE || rb->VertexBufferSize < vertex_size)
            CreateOrResizeBuffer(rb->VertexBuffer, rb->VertexBufferMemory, rb->VertexBufferSize, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        if (rb->IndexBuffer == VK_NULL_HANDLE || rb->IndexBufferSize < index_size)
            CreateOrResizeBuffer(rb->IndexBuffer, rb->IndexBufferMemory, rb->IndexBufferSize, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

        // Upload vertex/index data into a single contiguous GPU buffer
        ImDrawVert* vtx_dst = nullptr;
        ImDrawIdx* idx_dst = nullptr;
        VkResult err = vkMapMemory(v->Device->GetVkDevice(), rb->VertexBufferMemory, 0, rb->VertexBufferSize, 0, reinterpret_cast<void**>(&vtx_dst));
        CheckVkResult(err);
        err = vkMapMemory(v->Device->GetVkDevice(), rb->IndexBufferMemory, 0, rb->IndexBufferSize, 0, reinterpret_cast<void**>(&idx_dst));
        CheckVkResult(err);
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* const cmd_list = draw_data->CmdLists[n];
            std::copy_n(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size, vtx_dst);
            std::copy_n(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size, idx_dst);
            vtx_dst += cmd_list->VtxBuffer.Size;
            idx_dst += cmd_list->IdxBuffer.Size;
        }
        const std::array<VkMappedMemoryRange, 2> range
        {
            VkMappedMemoryRange
            {
                .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                .pNext = nullptr,
                .memory = rb->VertexBufferMemory,
                .offset = 0,
                .size = VK_WHOLE_SIZE
            },
            VkMappedMemoryRange
            {
                .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                .pNext = nullptr,
                .memory = rb->IndexBufferMemory,
                .offset = 0,
                .size = VK_WHOLE_SIZE
            }
        };
        err = vkFlushMappedMemoryRanges(v->Device->GetVkDevice(), 2, range.data());
        CheckVkResult(err);
        vkUnmapMemory(v->Device->GetVkDevice(), rb->VertexBufferMemory);
        vkUnmapMemory(v->Device->GetVkDevice(), rb->IndexBufferMemory);
    }

    // Setup desired Vulkan state
    SetupRenderState(draw_data, pipeline, command_buffer, rb, fb_width, fb_height);

    // Will project scissor/clipping rectangles into framebuffer space
    const ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    const ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int32_t global_vtx_offset = 0;
    uint32_t global_idx_offset = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* const cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* const pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    SetupRenderState(draw_data, pipeline, command_buffer, rb, fb_width, fb_height);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);

                // Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
                if (clip_min.x < 0.0f)
                    clip_min.x = 0.0f;
                if (clip_min.y < 0.0f)
                    clip_min.y = 0.0f;
                if (clip_max.x > NumericCast<float>(fb_width))
                    clip_max.x = NumericCast<float>(fb_width);
                if (clip_max.y > NumericCast<float>(fb_height))
                    clip_max.y = NumericCast<float>(fb_height);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply scissor/clipping rectangle
                const VkRect2D scissor
                {
                    .offset = VkOffset2D{.x = NumericCast<int32_t>(clip_min.x), .y = NumericCast<int32_t>(clip_min.y)},
                    .extent = VkExtent2D{.width = NumericCast<uint32_t>(TRAP::Math::Abs(clip_max.x - clip_min.x)),
                                         .height = NumericCast<uint32_t>(TRAP::Math::Abs(clip_max.y - clip_min.y))}
                };
                vkCmdSetScissor(command_buffer, 0, 1, &scissor);

                // Bind descriptorset with font or user texture
                VkDescriptorSet descSet = static_cast<VkDescriptorSet>(pcmd->TextureId);
                if constexpr(sizeof(ImTextureID) < sizeof(ImU64))
                {
                    // We don't support texture switches if ImTextureID hasn't been redefined to be 64-bit.
                    // Do a flaky check that other textures haven't been used.
                    TRAP_ASSERT(pcmd->TextureId == static_cast<ImTextureID>(bd->FontDescriptorSet), "ImGuiVulkanBackend::ImGui_ImplVulkan_RenderDrawData(): ImTextureID must be at least 64 bits!");
                    descSet = bd->FontDescriptorSet;
                }
                vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, bd->PipelineLayout, 0, 1, &descSet, 0, nullptr);

                // Draw
                vkCmdDrawIndexed(command_buffer, pcmd->ElemCount, 1u, pcmd->IdxOffset + global_idx_offset, NumericCast<int32_t>(pcmd->VtxOffset) + global_vtx_offset, 0u);
            }
        }
        global_idx_offset += NumericCast<uint32_t>(cmd_list->IdxBuffer.Size);
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Note: at this point both vkCmdSetViewport() and vkCmdSetScissor() have been called.
    // Our last values will leak into user/application rendering IF:
    // - Your app uses a pipeline with VK_DYNAMIC_STATE_VIEWPORT or VK_DYNAMIC_STATE_SCISSOR dynamic state
    // - And you forgot to call vkCmdSetViewport() and vkCmdSetScissor() yourself to explicitly set that state.
    // If you use VK_DYNAMIC_STATE_VIEWPORT or VK_DYNAMIC_STATE_SCISSOR you are responsible for setting the values before rendering.
    // In theory we should aim to backup/restore those values but I am not sure this is possible.
    // We perform a call to vkCmdSetScissor() to set back a full viewport which is likely to fix things for 99% users but technically this is not perfect. (See github #4644)
    const VkRect2D scissor{.offset = VkOffset2D{.x = 0, .y = 0}, .extent = VkExtent2D{ .width = NumericCast<uint32_t>(fb_width), .height = NumericCast<uint32_t>(fb_height) } };
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui_ImplVulkan_UploadFontsTexture()
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    //Destroy old font
    DestroyFontsTexture();

    const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData(TRAP::Application::GetWindow());
    //Execute a GPU command to upload ImGui font textures
    TRAP::Graphics::CommandBuffer* const cmd = viewportData.GraphicCommandPools[viewportData.ImageIndex]->AllocateCommandBuffer(false);
    cmd->Begin();
    CreateFontsTexture(dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
                                        (cmd)->GetVkCommandBuffer());
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
    const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;

    v->Queue->Submit(submitDesc);
    submitFence->Wait();
    viewportData.GraphicCommandPools[viewportData.ImageIndex]->FreeCommandBuffer(cmd);
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui_ImplVulkan_DestroyFontUploadObjects()
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    ImGui_ImplVulkan_Data* const bd = GetBackendData();
    const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;
    if (bd->UploadBuffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(v->Device->GetVkDevice(), bd->UploadBuffer, v->Allocator);
        bd->UploadBuffer = VK_NULL_HANDLE;
    }
    if (bd->UploadBufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(v->Device->GetVkDevice(), bd->UploadBufferMemory, v->Allocator);
        bd->UploadBufferMemory = VK_NULL_HANDLE;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui_ImplVulkan_Init(const ImGui_ImplVulkan_InitInfo* const info, VkRenderPass render_pass)
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    ImGuiIO& io = ImGui::GetIO();
    TRAP_ASSERT(io.BackendRendererUserData == nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkan_Init(): Renderer backend is already initialized!");

    // Setup backend capabilities flags
    ImGui_ImplVulkan_Data* const bd = IM_NEW(ImGui_ImplVulkan_Data)();
    io.BackendRendererUserData = bd;
    io.BackendRendererName = "TRAP_Vulkan";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

    TRAP_ASSERT(info->Instance != nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkan_Init(): info->Instance is nullptr!");
    TRAP_ASSERT(info->Device != nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkan_Init(): info->Device is nullptr!");
    TRAP_ASSERT(info->Queue != nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkan_Init(): info->Queue is VK_NULL_HANDLE!");
    TRAP_ASSERT(!info->DescriptorPoolSizes.empty(), "ImGuiVulkanBackend::ImGui_ImplVulkan_Init(): info->DescriptorPoolSizes must not be empty!");
    TRAP_ASSERT(info->MinImageCount >= 2, "ImGuiVulkanBackend::ImGui_ImplVulkan_Init(): info->MinImageCount must be at least 2!");
    TRAP_ASSERT(info->ImageCount >= info->MinImageCount, "ImGuiVulkanBackend::ImGui_ImplVulkan_Init(): info->ImageCount muste be equivalent to info->MinImageCount or bigger!");
    if(!info->UseDynamicRendering)
        TRAP_ASSERT(render_pass != VK_NULL_HANDLE, "ImGuiVulkanBackend::ImGui_ImplVulkan_Init(): info->RenderPass is VK_NULL_HANDLE!");

    bd->VulkanInitInfo = *info;
    bd->RenderPass = render_pass;

    CreateDeviceObjects();

    if(info->DescriptorPool != VK_NULL_HANDLE)
        bd->DescriptorPools.push_back(info->DescriptorPool);
    else
        CreateNewDescriptorPool();

    // Our render function expect RendererUserData to be storing the window render buffer we need (for the main viewport we won't use ->Window)
    ImGuiViewport* const main_viewport = ImGui::GetMainViewport();
    main_viewport->RendererUserData = IM_NEW(ImGui_ImplVulkan_ViewportData)();

    if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
        InitPlatformInterface();
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui_ImplVulkan_Shutdown()
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    ImGui_ImplVulkan_Data* const bd = GetBackendData();

    TRAP_ASSERT(bd != nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkan_Shutdown(): Renderer backend is already shutdown!");
    ImGuiIO& io = ImGui::GetIO();
    const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;

    // First destroy objects in all viewports
    DestroyDeviceObjects();

    for(VkDescriptorPool& descPool : bd->DescriptorPools)
        vkDestroyDescriptorPool(v->Device->GetVkDevice(), descPool, v->Allocator);
    bd->DescriptorPools.clear();

    // Manually delete main viewport render data in-case we haven't initialized for viewports
    ImGuiViewport* const main_viewport = ImGui::GetMainViewport();
    if (ImGui_ImplVulkan_ViewportData* const vd = static_cast<ImGui_ImplVulkan_ViewportData*>(main_viewport->RendererUserData))
        IM_DELETE(vd);
    main_viewport->RendererUserData = nullptr;

    // Clean up windows
    ShutdownPlatformInterface();

    ImGui_ImplVulkan_ClearCache();

    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    IM_DELETE(bd);
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui_ImplVulkan_NewFrame()
{
    ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    [[maybe_unused]] const ImGui_ImplVulkan_Data* const bd = GetBackendData();
    TRAP_ASSERT(bd != nullptr, "ImGuiVulkanBackend::ImGui_ImplVulkan_NewFrame(): Renderer backend hasn't been initialized!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ImTextureID ImGui_ImplVulkan_AddTexture(VkSampler sampler, VkImageView image_view, const VkImageLayout image_layout)
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    const auto it = s_VulkanTextureCache.find(reinterpret_cast<std::uintptr_t>(image_view));
    if(it != s_VulkanTextureCache.end())
        return reinterpret_cast<ImTextureID>(it->second.DescriptorSet);

    VkDetails& vulkanDetails = s_VulkanTextureCache[reinterpret_cast<std::uintptr_t>(image_view)];
    vulkanDetails.Sampler = sampler;
    vulkanDetails.ImageView = image_view;
    vulkanDetails.ImageLayout = image_layout;

    const ImGui_ImplVulkan_Data* const bd = GetBackendData();
    const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

    // Create Descriptor Set:
    {
        VkDescriptorSetAllocateInfo allocInfo = TRAP::Graphics::API::VulkanInits::DescriptorSetAllocateInfo(v->DescriptorPool, bd->DescriptorSetLayout);
        VkResult err = vkAllocateDescriptorSets(v->Device->GetVkDevice(), &allocInfo, &descriptorSet);
        // check_vk_result(err);

        if(descriptorSet == VK_NULL_HANDLE)
        {
            CreateNewDescriptorPool();

            //Try again now with the new pool
            allocInfo.descriptorPool = v->DescriptorPool;
            err = vkAllocateDescriptorSets(v->Device->GetVkDevice(), &allocInfo, &descriptorSet);
            CheckVkResult(err);
        }
    }

    vulkanDetails.DescriptorSet = descriptorSet;
    ImGui_ImplVulkan_UpdateTextureInfo(vulkanDetails.DescriptorSet, vulkanDetails.Sampler, vulkanDetails.ImageView, vulkanDetails.ImageLayout);

    return reinterpret_cast<ImTextureID>(vulkanDetails.DescriptorSet);
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui_ImplVulkan_RemoveTexture(VkImageView image_view)
{
    const auto it = s_VulkanTextureCache.find(reinterpret_cast<std::uintptr_t>(image_view));

    if(it == s_VulkanTextureCache.end())
        return;

    s_VulkanTextureCache.erase(it);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ImTextureID ImGui_ImplVulkan_UpdateTextureInfo(VkDescriptorSet descriptorSet, VkSampler sampler,
                                                             VkImageView image_view, const VkImageLayout image_layout)
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    const ImGui_ImplVulkan_Data* const bd = GetBackendData();
    const ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;

    // Update Descriptor Set:
    {
        const VkDescriptorImageInfo imageInfo
        {
            .sampler = sampler,
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
        vkUpdateDescriptorSets(v->Device->GetVkDevice(), 1, &writeDesc, 0, nullptr);
    }

    return reinterpret_cast<ImTextureID>(descriptorSet);
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui_ImplVulkan_ClearCache() noexcept
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

    s_VulkanTextureCache.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

void ImGui_ImplVulkan_SetMSAASamples(const VkSampleCountFlagBits sampleCount)
{
    ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData(TRAP::Application::GetWindow());
    ImGui_ImplVulkan_Data* const bd = GetBackendData();
    ImGui_ImplVulkan_InitInfo* const v = &bd->VulkanInitInfo;

    if(sampleCount == v->MSAASamples)
        return;

    v->MSAASamples = sampleCount;
    bd->RenderPass = dynamic_cast<TRAP::Graphics::API::VulkanCommandBuffer*>
		(
			viewportData.GraphicCommandBuffers[viewportData.ImageIndex]
		)->GetActiveVkRenderPass();

    //Delete old pipeline
    if (bd->Pipeline != nullptr)
    {
        vkDestroyPipeline(v->Device->GetVkDevice(), bd->Pipeline, v->Allocator);
        bd->Pipeline = VK_NULL_HANDLE;
    }

    //Create new pipeline
    CreatePipeline(v->Device, v->Allocator, v->PipelineCache, bd->RenderPass, v->MSAASamples, &bd->Pipeline);
}

#endif /*TRAP_HEADLESS_MODE*/

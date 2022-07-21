#ifndef TRAP_VULKANRENDERER_H
#define TRAP_VULKANRENDERER_H

#include <thread>

#include "Graphics/API/RendererAPI.h"
#include "Maths/Math.h"
#include "Window/WindowingAPI.h"
#include "Graphics/API/ShaderReflection.h"

namespace TRAP::Graphics
{
	class Texture;
}

namespace TRAP::Graphics::API
{
	class VulkanSampler;
	class VulkanRenderTarget;
	class VulkanBuffer;
	class VulkanMemoryAllocator;
	class VulkanFrameBuffer;
	class VulkanRenderPass;
	class VulkanDescriptorPool;
	class VulkanPhysicalDevice;
	class VulkanDevice;
	class VulkanInstance;
	class VulkanDebug;
	class VulkanCommandPool;
	class VulkanCommandBuffer;
	class VulkanFence;
	class VulkanQueue;

	class VulkanRenderer final : public RendererAPI
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		explicit VulkanRenderer();
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanRenderer() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanRenderer(const VulkanRenderer&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanRenderer(VulkanRenderer&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanRenderer& operator=(VulkanRenderer&&) = delete;

		/// <summary>
		/// Initialize the internal Vulkan renderer.
		/// </summary>
		/// <param name="gameName">Name of the game.</param>
		void InitInternal(const std::string_view gameName) override;

		/// <summary>
		/// Present to the given window.
		/// </summary>
		/// <param name="window">Window to present.</param>
		void Present(Window* window) const override;

		/// <summary>
		/// Dispatch to the given window.
		/// </summary>
		/// <param name="workGroupElements">
		/// Amount of elements for each work group.
		/// These values will be divided by the shader's work group size and rounded up.
		/// </param>
		/// <param name="window">Window to Dispatch.</param>
		void Dispatch(std::array<uint32_t, 3> workGroupElements, Window* window) const override;
		//TODO DispatchIndirect

		/// <summary>
		/// Set the VSync state for the given window.
		/// </summary>
		/// <param name="vsync">Enable or disable VSync.</param>
		/// <param name="window">Window to set VSync for. Default: Main Window.</param>
		void SetVSync(bool vsync, Window* window) const override;

		/// <summary>
		/// Set the clear color to be used by the given window.
		/// </summary>
		/// <param name="color">New clear color.</param>
		/// <param name="window">Window to set clear color for. Default: Main Window.</param>
		void SetClearColor(const Math::Vec4& color, Window* window) const override;
		/// <summary>
		/// Set the clear depth value to be used by the given window.
		/// </summary>
		/// <param name="depth">New clear depth value. Must be between 0.0f and 1.0f</param>
		/// <param name="window">Window to set clear depth value for. Default: Main Window.</param>
		void SetClearDepth(float depth, Window* window) const override;
		/// <summary>
		/// Set the clear stencil value to be used by the given window.
		/// </summary>
		/// <param name="stencil">New clear stencil value.</param>
		/// <param name="window">Window to set clear stencil value for. Default: Main Window.</param>
		void SetClearStencil(uint32_t stencil, Window* window) const override;
#ifdef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the resolution of the render targets used by the given window.
		///
		/// Note: This function is only available in Headless mode.
		/// </summary>
		/// <param name="width">New width.</param>
		/// <param name="height">New height.</param>
		/// <param name="window">Window to set resolution for. Default: Main Window.</param>
		void SetResolution(uint32_t width, uint32_t height, Window* window) const override;
#endif
		/// <summary>
		/// Enable or disable depth testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth testing.</param>
		/// <param name="window">Window to set depth testing for. Default: Main Window.</param>
		void SetDepthTesting(bool enabled, Window* window) const override;
		/// <summary>
		/// Enable or disable depth writing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth writing.</param>
		/// <param name="window">Window to set depth writing for. Default: Main Window.</param>
		void SetDepthWriting(bool enabled, Window* window) const override;
		/// <summary>
		/// Set the depth function for the given window.
		/// </summary>
		/// <param name="function">Function to use for depth testing.</param>
		/// <param name="window">Window to set depth function for. Default: Main Window.</param>
		void SetDepthFunction(CompareMode function, Window* window) const override;
		/// <summary>
		/// Set the depth action to perform when depth testing fails for the given window.
		/// </summary>
		/// <param name="front">Depth action to perform when depth testing fails.</param>
		/// <param name="back">Depth action to perform when depth testing fails.</param>
		/// <param name="window">Window to set the depth fail action for. Default: Main Window.</param>
		void SetDepthFail(StencilOp front, StencilOp back, Window* window) const override;
		/// <summary>
		/// Set the depth bias (scalar factor to add to each fragments depth value) for the given window.
		/// </summary>
		/// <param name="depthBias">Depth bias.</param>
		/// <param name="window">Window to set the depth bias for. Default: Main Window.</param>
		void SetDepthBias(int32_t depthBias, Window* window) const override;
		/// <summary>
		/// Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation) for the given window.
		/// </summary>
		/// <param name="factor">Depth bias slope factor.</param>
		/// <param name="window">Window to set the depth bias slope factor for. Default: Main Window.</param>
		void SetDepthBiasSlopeFactor(float factor, Window* window) const override;
		/// <summary>
		/// Enable or disable stencil testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable stencil testing.</param>
		/// <param name="window">Window to set stencil testing for. Default: Main Window.</param>
		void SetStencilTesting(bool enabled, Window* window) const override;
		/// <summary>
		/// Set the stencil action to perform when stencil testing fails for the given window.
		/// </summary>
		/// <param name="front">Stencil action to perform when stencil testing fails.</param>
		/// <param name="back">Stencil action to perform when stencil testing fails.</param>
		/// <param name="window">Window to set the stencil fail action for. Default: Main Window.</param>
		void SetStencilFail(StencilOp front, StencilOp back, Window* window) const override;
		/// <summary>
		/// Set the stencil action to perform when stencil testing and depth testing passes for the given window.
		/// </summary>
		/// <param name="front">Stencil action to perform when passed.</param>
		/// <param name="back">Stencil action to perform when passed.</param>
		/// <param name="window">Window to set the stencil pass action for. Default: Main Window.</param>
		void SetStencilPass(StencilOp front, StencilOp back, Window* window) const override;
		/// <summary>
		/// Set the stencil functions for the given window.
		/// </summary>
		/// <param name="front">Function to use on the front for stencil testing.</param>
		/// <param name="back">Function to use on the back for stencil testing.</param>
		/// <param name="window">Window to set stencil functions for. Default: Main Window.</param>
		void SetStencilFunction(CompareMode front, CompareMode back, Window* window) const override;
		/// <summary>
		/// Set the stencil mask for the given window.
		/// </summary>
		/// <param name="read">Select the bits of the stencil values to test.</param>
		/// <param name="write">Select the bits of the stencil values updated by the stencil test.</param>
		/// <param name="window">Window to set stencil mask for. Default: Main Window.</param>
		void SetStencilMask(uint8_t read, uint8_t write, Window* window) const override;
		/// <summary>
		/// Set the cull mode for the given window.
		/// </summary>
		/// <param name="mode">Cull mode to use.</param>
		/// <param name="window">Window to set cull mode for. Default: Main Window.</param>
		void SetCullMode(CullMode mode, Window* window) const override;
		/// <summary>
		/// Set the fill mode for the given window.
		/// </summary>
		/// <param name="mode">Fill mode to use.</param>
		/// <param name="window">Window to set fill mode for. Default: Main Window.</param>
		void SetFillMode(FillMode mode, Window* window) const override;
		/// <summary>
		/// Set the primitive topology for the given window.
		/// </summary>
		/// <param name="topology">Primitive topology to use.</param>
		/// <param name="window">Window to set primitive topology for. Default: Main Window.</param>
		void SetPrimitiveTopology(PrimitiveTopology topology, Window* window) const override;
		/// <summary>
		/// Set the front face winding order for the given window.
		/// </summary>
		/// <param name="face">Front face winding order to use.</param>
		/// <param name="window">Window to set front face winding order for. Default: Main Window.</param>
		void SetFrontFace(FrontFace face, Window* window) const override;
		/// <summary>
		/// Set the blend mode for the given window.
		/// </summary>
		/// <param name="modeRGB">Blend mode to use for the RGB channels.</param>
		/// <param name="modeAlpha">Blend mode to use for the alpha channel.</param>
		/// <param name="window">Window to set the blend mode for. Default: Main Window.</param>
		void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, Window* window) const override;
		/// <summary>
		/// Set the blend constants/factors for the given window.
		/// </summary>
		/// <param name="sourceRGB">Specifies how the red, green, and blue blending factors are computed.</param>
		/// <param name="sourceAlpha">Specifies how the alpha source blending factor is computed.</param>
		/// <param name="destinationRGB">Specifies how the red, green, and blue destination blending factors are computed.</param>
		/// <param name="destinationAlpha">Specified how the alpha destination blending factor is computed.</param>
		/// <param name="window">Window to set the blend constants for. Default: Main Window.</param>
		void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
							  BlendConstant destinationRGB, BlendConstant destinationAlpha,
							  Window* window) const override;
		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="texture">Unused by Vulkan.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		/// <param name="window">Window to set the shading rate for. Default: Main Window.</param>
		void SetShadingRate(ShadingRate shadingRate,
							TRAP::Graphics::Texture* texture,
							ShadingRateCombiner postRasterizerRate,
							ShadingRateCombiner finalRate, Window* window = nullptr) const override;
		/// <summary>
		/// Set the anti aliasing method and the sample count for the window.
		/// Use AntiAliasing::Off and SampleCount::One to disable anti aliasing.
		/// </summary>
		/// <param name="antiAliasing">Anti aliasing method to use.</param>
		/// <param name="sampleCount">Sample count to use.</param>
		/// <param name="window">Window to set anti aliasing for. Default: Main Window.</param>
		void SetAntiAliasing(AntiAliasing antiAliasing, SampleCount sampleCount, Window* window = nullptr) const override;
		/// <summary>
		/// Retrieve the anti aliasing method and the sample count of the window.
		/// </summary>
		/// <param name="outAntiAliasing">Output: Used anti aliasing method.</param>
		/// <param name="outSampleCount">Output: Used sample count.</param>
		/// <param name="window">Window to get anti aliasing from. Default: Main Window.</param>
		void GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount, Window* window = nullptr) const override;

		/// <summary>
		/// Clear the given window's render target.
		/// </summary>
		/// <param name="clearType">Type of buffer to clear.</param>
		/// <param name="window">Window to clear. Default: Main Window.</param>
		void Clear(ClearBufferType clearType, Window* window) const override;

		/// <summary>
		/// Set viewport size for the given window.
		/// </summary>
		/// <param name="x">X coordinate of the top left corner of the viewport.</param>
		/// <param name="y">Y coordinate of the top left corner of the viewport.</param>
		/// <param name="width">New viewport width.</param>
		/// <param name="height">New viewport height.</param>
		/// <param name="minDepth">New min depth value. Default: 0.0f.</param>
		/// <param name="maxDepth">New max depth value. Default: 1.0f.</param>
		/// <param name="window">Window to set viewport for. Default: Main Window.</param>
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, float minDepth,
		                 float maxDepth, Window* window) const override;
		/// <summary>
		/// Set scissor size for the given window.
		/// </summary>
		/// <param name="x">Upper left corner.</param>
		/// <param name="y">Upper left corner.</param>
		/// <param name="width">New scissor width.</param>
		/// <param name="height">New scissor height.</param>
		/// <param name="window">Window to set scissor size for. Default: Main Window.</param>
		void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Window* window) const override;

		/// <summary>
		/// Draw non-indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		void Draw(uint32_t vertexCount, uint32_t firstVertex, Window* window) const override;
		/// <summary>
		/// Draw indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		void DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t firstVertex,
		                 Window* window) const override;
		/// <summary>
		/// Draw non-indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
		                   uint32_t firstInstance, Window* window) const override;
		/// <summary>
		/// Draw indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
		                          uint32_t firstIndex, uint32_t firstInstance,
							      uint32_t firstVertex, Window* window) const override;

		/// <summary>
		/// Bind shader on the given window.
		/// </summary>
		/// <param name="shader">Shader to bind.</param>
		/// <param name="window">Window to bind the shader for. Default: Main Window.</param>
		void BindShader(Shader* shader, Window* window) const;
		/// <summary>
		/// Bind vertex buffer on the given window.
		/// </summary>
		/// <param name="vBuffer">Vertex buffer to bind.</param>
		/// <param name="layout">Layout of the vertex buffer.</param>
		/// <param name="window">Window to bind the vertex buffer for. Default: Main Window.</param>
		void BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer, const VertexBufferLayout& layout,
		                      Window* window) const override;
		/// <summary>
		/// Bind an index buffer on the given window.
		/// </summary>
		/// <param name="iBuffer">Index buffer to bind.</param>
		/// <param name="indexType">Data type used by the index buffer.</param>
		/// <param name="window">Window to bind the vertex buffer for. Default: Main Window.</param>
		void BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer, IndexType indexType,
		                     Window* window ) const override;
		/// <summary>
		/// Bind a descriptor set on the given window.
		/// </summary>
		/// <param name="dSet">Descriptor set to bind.</param>
		/// <param name="index">Index for which descriptor set to bind.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the descriptor set for. Default: Main Window.</param>
		void BindDescriptorSet(DescriptorSet& dSet, uint32_t index, QueueType queueType, Window* window) const override;
		/// <summary>
		/// Bind push constant buffer data on the given window.
		/// Note: There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// </summary>
		/// <param name="name">Name of the push constant block.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the push constants for. Default: Main Window.</param>
		void BindPushConstants(const char* name, const void* constantsData, QueueType queueType, Window* window) const override;
		/// <summary>
		/// Bind push constant buffer data on the given window.
		/// </summary>
		/// <param name="paramIndex">Index of the push constant block in the RootSignatures descriptors array.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the push constants for. Default: Main Window.</param>
		void BindPushConstantsByIndex(uint32_t paramIndex, const void* constantsData, QueueType queueType,
		                              Window* window) const override;
		/// <summary>
		/// Bind render target(s) on the given window.
		///
		/// Note: This functions ends the currently running render pass and starts a new one.
		/// </summary>
		/// <param name="colorTarget">Color render target to bind.</param>
		/// <param name="depthStencil">Optional depth stencil target to bind. Default: nullptr.</param>
		/// <param name="loadActions">Optional load actions for each render target. Default: nullptr.</param>
		/// <param name="colorArraySlices">Optional color array slices for each render target. Default: nullptr.</param>
		/// <param name="colorMipSlices">Optional color mip slices for each render target. Default: nullptr.</param>
		/// <param name="depthArraySlice">Optional depth array slice for the depth stencil target. Default: -1.</param>
		/// <param name="depthMipSlice">Optional depth mip slice for the depth stencil target. Default: -1.</param>
		/// <param name="window">Window to bind the render target(s) for. Default: Main Window.</param>
		void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                      const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
							  const RendererAPI::LoadActionsDesc* loadActions,
							  std::vector<uint32_t>* colorArraySlices,
							  std::vector<uint32_t>* colorMipSlices,
							  uint32_t depthArraySlice, uint32_t depthMipSlice,
							  Window* window) const override;
		/// <summary>
		/// Bind render target(s) on the given window.
		///
		/// Note: This functions ends the currently running render pass and starts a new one.
		/// </summary>
		/// <param name="colorTargets">Color render target(s) to bind.</param>
		/// <param name="depthStencil">Optional depth stencil target to bind. Default: nullptr.</param>
		/// <param name="loadActions">Optional load actions for each render target. Default: nullptr.</param>
		/// <param name="colorArraySlices">Optional color array slices for each render target. Default: nullptr.</param>
		/// <param name="colorMipSlices">Optional color mip slices for each render target. Default: nullptr.</param>
		/// <param name="depthArraySlice">Optional depth array slice for the depth stencil target. Default: -1.</param>
		/// <param name="depthMipSlice">Optional depth mip slice for the depth stencil target. Default: -1.</param>
		/// <param name="window">Window to bind the render target(s) for. Default: Main Window.</param>
		void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                       const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
							   const RendererAPI::LoadActionsDesc* loadActions,
							   std::vector<uint32_t>* colorArraySlices,
							   std::vector<uint32_t>* colorMipSlices,
							   uint32_t depthArraySlice, uint32_t depthMipSlice,
							   Window* window) const override;

		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="bufferBarrier">Buffer barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barrier for. Default: Main Window.</param>
		void ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier, QueueType queueType,
		                           Window* window) const override;
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="bufferBarriers">Buffer barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barriers for. Default: Main Window.</param>
		void ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
									QueueType queueType, Window* window) const override;
		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="textureBarrier">Texture barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barrier for. Default: Main Window.</param>
		void ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier, QueueType queueType,
		                            Window* window) const override;
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="textureBarriers">Texture barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barriers for. Default: Main Window.</param>
		void ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
									 QueueType queueType, Window* window) const override;
		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="renderTargetBarrier">Render target barrier.</param>
		/// <param name="window">Window to add the barrier for. Default: Main Window.</param>
		void ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
		                                 Window* window ) const override;
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="renderTargetBarriers">Render target barriers.</param>
		/// <param name="window">Window to add the barriers for. Default: Main Window.</param>
		void ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
								          Window* window) const override;

		/// <summary>
		/// Retrieve the renderer title.
		/// Example title: "[Vulkan 1.3.0]".
		/// </summary>
		/// <returns>Renderer title.</returns>
		std::string GetTitle() const override;
		/// <summary>
		/// Retrieve whether VSync is enabled or not for the given window.
		/// </summary>
		/// <param name="window">Window to retrieve VSync for. Default: Main Window.</param>
		/// <returns>True if VSync is enabled, false otherwise.</returns>
		bool GetVSync(Window* window) const override;

		/// <summary>
		/// Retrieve the currently used GPUs UUID.
		/// </summary>
		/// <returns>GPU's UUID.</returns>
		std::array<uint8_t, 16> GetCurrentGPUUUID() const override;
		/// <summary>
		/// Retrieve the name of the currently used GPU.
		/// </summary>
		/// <returns>GPU's name.</returns>
		std::string GetCurrentGPUName() const override;
		/// <summary>
		/// Retrieve a list of all supported GPUs.
		/// The list contains the GPUs name and UUID.
		/// </summary>
		/// <returns>List of all supported GPUs.</returns>
		std::vector<std::pair<std::string, std::array<uint8_t, 16>>> GetAllGPUs() const override;

		/// <summary>
		/// Capture a screenshot of the last presented frame.
		/// </summary>
		/// <param name="window">Window to capture screenshot on. Default: Main Window.</param>
		/// <returns>Captured screenshot as TRAP::Image on success, Black 1x1 TRAP::Image otherwise.</returns>
		TRAP::Scope<TRAP::Image> CaptureScreenshot(Window* window) const override;

		/// <summary>
		/// Initialize the internal rendering data of the given window.
		/// </summary>
		/// <param name="window">Window to initialize the internal rendering data for.</param>
		void InitPerWindowData(Window* window) const override;
		/// <summary>
		/// Remove the internal rendering data of the given window.
		/// </summary>
		/// <param name="window">Window to remove the internal rendering data from.</param>
		void RemovePerWindowData(Window* window) const override;

		void WaitIdle() const override;

		//Instance Extensions
		static bool s_debugUtilsExtension;
		static bool s_debugReportExtension;
		static bool s_validationFeaturesExtension;
		static bool s_swapchainColorSpaceExtension;

		//Device Extensions
		static bool s_shaderDrawParameters;
		static bool s_fragmentShaderInterlockExtension;
		static bool s_drawIndirectCountExtension;
		static bool s_descriptorIndexingExtension;
		static bool s_rayTracingExtension;
		static bool s_samplerYcbcrConversionExtension;
		static bool s_bufferDeviceAddressExtension;
		static bool s_memoryBudgetExtension;
		static bool s_maintenance4Extension;
		static bool s_externalMemory;
		static bool s_shadingRate;

		static bool s_debugMarkerSupport;

		static bool s_externalMemoryWin32Extension;
		static bool s_externalFenceWin32Extension;
		static bool s_externalSemaphoreWin32Extension;

		static struct GPUCapBits
		{
			std::array<bool, static_cast<uint32_t>(ImageFormat::IMAGE_FORMAT_COUNT)> CanShaderReadFrom{};
			std::array<bool, static_cast<uint32_t>(ImageFormat::IMAGE_FORMAT_COUNT)> CanShaderWriteTo{};
			std::array<bool, static_cast<uint32_t>(ImageFormat::IMAGE_FORMAT_COUNT)> CanRenderTargetWriteTo{};
		} s_GPUCapBits;

		struct SizeOffset
		{
			uint32_t Size;
			uint32_t Offset;
		};

		union DescriptorUpdateData
		{
			VkDescriptorImageInfo ImageInfo;
			VkDescriptorBufferInfo BufferInfo;
			VkBufferView BufferView;
		};

		/// <summary>
		/// Description for framebuffer creation.
		/// </summary>
		struct FrameBufferDesc
		{
			//Render pass to use
			TRAP::Ref<VulkanRenderPass> RenderPass;
			//Color render targets to use
			std::vector<TRAP::Ref<RenderTarget>> RenderTargets;
			//Depth/Stencil render target to use
			TRAP::Ref<RenderTarget> DepthStencil;
			//Array layer to use from color render targets
			std::vector<uint32_t> ColorArraySlices;
			//Mip level to use from color render targets
			std::vector<uint32_t> ColorMipSlices;
			//Array layer of depth/stencil render target to use
			uint32_t DepthArraySlice;
			//Mip level of depth/stencil render target to use
			uint32_t DepthMipSlice;
		};

		/// <summary>
		/// Description for render pass creation.
		/// </summary>
		struct RenderPassDesc
		{
			std::vector<ImageFormat> ColorFormats;
			std::vector<LoadActionType> LoadActionsColor;
			std::vector<StoreActionType> StoreActionsColor;
			uint32_t RenderTargetCount;
			TRAP::Graphics::RendererAPI::SampleCount SampleCount;
			ImageFormat DepthStencilFormat;
			LoadActionType LoadActionDepth;
			LoadActionType LoadActionStencil;
			StoreActionType StoreActionDepth;
			StoreActionType StoreActionStencil;
		};

		/// <summary>
		/// Struct holding data about a descriptor set layout.
		/// </summary>
		struct UpdateFrequencyLayoutInfo
		{
			//Array of all bindings in the descriptor set
			std::vector<VkDescriptorSetLayoutBinding> Bindings{};
			//Array of all descriptors in this descriptor set
			std::vector<DescriptorInfo*> Descriptors{};
			//Array of all descriptors marked as dynamic in this descriptor set
			//(applicable to DescriptorType::UniformBuffer)
			std::vector<DescriptorInfo*> DynamicDescriptors{};
			//Hash map to get index of the descriptor in the root signature
			std::unordered_map<DescriptorInfo*, uint64_t> DescriptorIndexMap{};
		};

		/// <summary>
		/// Create default resources to be used as null descriptors in case user does not specify some descriptors
		/// </summary>
		struct NullDescriptors
		{
			std::array<TRAP::Scope<TRAP::Graphics::Texture>,
			           static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount)> DefaultTextureSRV;
			std::array<TRAP::Scope<TRAP::Graphics::Texture>,
			           static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount)> DefaultTextureUAV;
			TRAP::Ref<VulkanBuffer> DefaultBufferSRV;
			TRAP::Ref<VulkanBuffer> DefaultBufferUAV;
			TRAP::Ref<VulkanSampler> DefaultSampler;
			std::mutex SubmitMutex;

			//Unlike DirectX 12, Vulkan textures start in undefined layout.
			//With this, we transition them to the specified layout so app code doesn't
			//have to worry about this
			std::mutex InitialTransitionMutex;
			TRAP::Ref<VulkanQueue> InitialTransitionQueue;
			TRAP::Ref<VulkanCommandPool> InitialTransitionCmdPool;
			VulkanCommandBuffer* InitialTransitionCmd;
			TRAP::Ref<VulkanFence> InitialTransitionFence;
		};
		static TRAP::Scope<NullDescriptors> s_NullDescriptors;
		static std::vector<VkPipelineColorBlendAttachmentState> DefaultBlendAttachments;
		static VkPipelineRasterizationStateCreateInfo DefaultRasterizerDesc;
		static VkPipelineDepthStencilStateCreateInfo DefaultDepthDesc;
		static VkPipelineColorBlendStateCreateInfo DefaultBlendDesc;

		/// <summary>
		/// Transition the given texture from undefined resource state to the given resource state.
		/// </summary>
		/// <param name="texture">Texture to transition.</param>
		/// <param name="startState">Start state for the texture.</param>
		static void UtilInitialTransition(TRAP::Graphics::Texture* texture, RendererAPI::ResourceState startState);

		//Per Thread Render Pass synchronization logic
		//Render-passes are not exposed to the engine code since they are not available on all APIs
		//This map takes care of hashing a render pass based on the render targets passed to
		//CommandBuffer::Begin()
		using RenderPassMap = std::unordered_map<uint64_t, TRAP::Ref<VulkanRenderPass>>;
		using RenderPassMapNode = RenderPassMap::value_type;
		using RenderPassMapIt = RenderPassMap::iterator;
		using FrameBufferMap = std::unordered_map<uint64_t, TRAP::Ref<VulkanFrameBuffer>>;
		using FrameBufferMapNode = FrameBufferMap::value_type;
		using FrameBufferMapIt = FrameBufferMap::iterator;

		/// <summary>
		/// Retrieve the render pass hash map.
		/// </summary>
		/// <returns>Render pass hash map.</returns>
		static RenderPassMap& GetRenderPassMap();
		/// <summary>
		/// Retrieve the framebuffer hash map.
		/// </summary>
		/// <returns>Framebuffer hash map.</returns>
		static FrameBufferMap& GetFrameBufferMap();

		/// <summary>
		/// Retrieve the vulkan instance used by the renderer.
		/// </summary>
		/// <returns>Vulkan instance.</returns>
		TRAP::Ref<VulkanInstance> GetInstance() const;
		/// <summary>
		/// Retrieve the vulkan device used by the renderer.
		/// </summary>
		/// <returns>Vulkan device.</returns>
		TRAP::Ref<VulkanDevice> GetDevice() const;
		/// <summary>
		/// Retrieve the vulkan memory allocator used by the renderer.
		/// </summary>
		/// <returns>Vulkan memory allocator.</returns>
		TRAP::Ref<VulkanMemoryAllocator> GetVMA() const;

		/// <summary>
		/// Utility to retrieve a pipeline.
		/// If a pipeline with the same description was already created,
		/// it will be retrieved from the cache.
		/// </summary>
		/// <param name="desc">Pipeline description.</param>
		/// <returns>Pipeline.</returns>
		static const TRAP::Ref<Pipeline>& GetPipeline(PipelineDesc& desc);

	private:
		/// <summary>
		/// Utility to retrieve the pixel data of a render target from the GPU.
		/// </summary>
		/// <param name="renderTarget">Render target to retrieve the pixel data from.</param>
		/// <param name="currResState">Current resource state of the render target.</param>
		/// <param name="outPixelData">Output pixel data.</param>
		static void MapRenderTarget(const TRAP::Ref<RenderTarget>& renderTarget, ResourceState currResState, void* outPixelData);

		/// <summary>
		/// Retrieve the list of instance layers to use.
		/// </summary>
		/// <returns>List of instance layers.</returns>
		static std::vector<std::string> SetupInstanceLayers();
		/// <summary>
		/// Retrieve the list of instance extensions to use.
		///
		/// 1. Surface extension(s) (Mandatory when not using Headless mode).
		/// 2. Debug extension (if supported) (Only active when ENABLE_GRAPHICS_DEBUG is defined).
		/// 3. HDR extension (if supported).
		/// 4. VR extension(s) (if supported).
		/// </summary>
		/// <returns>List of instance extensions.</returns>
		static std::vector<std::string> SetupInstanceExtensions();
		/// <summary>
		/// Retrieve the list of device extensions to use.
		///
		/// 1. Swapchain extension (Mandatory)
		/// 2. Indirect drawing extension (if supported).
		/// 3. Fragment shader interlock extension (if supported).
		/// 4. Descriptor indexing extension (if supported).
		/// 5. RayTracing extensions (if supported).
		/// 6. YCbCr conversion extension (if supported).
		/// 7. Shader draw parameters extension (if supported).
		/// </summary>
		/// <param name="physicalDevice">Physical device to use.</param>
		/// <returns>List of device extensions.</returns>
		static std::vector<std::string> SetupDeviceExtensions(VulkanPhysicalDevice* physicalDevice);

		/// <summary>
		/// Add the default resources to the renderer.
		/// </summary>
		static void AddDefaultResources();
		/// <summary>
		/// Remove the default resources from the renderer.
		/// </summary>
		static void RemoveDefaultResources();

		/// <summary>
		/// Start recording the graphics pipeline.
		/// </summary>
		/// <param name="p">Per window data used for recording.</param>
		static void StartGraphicRecording(PerWindowData* const p);
		/// <summary>
		/// Stop recording the graphics pipeline.
		/// </summary>
		/// <param name="p">Per window data used for recording.</param>
		static void EndGraphicRecording(PerWindowData* const p);

		/// <summary>
		/// Start recording the compute pipeline.
		/// </summary>
		/// <param name="p">Per window data used for recording.</param>
		static void StartComputeRecording(PerWindowData* const p);
		/// <summary>
		/// Stop recording the compute pipeline.
		/// </summary>
		/// <param name="p">Per window data used for recording.</param>
		static void EndComputeRecording(PerWindowData* const p);

		/// <summary>
		/// Resolve the MSAA pass.
		/// Needed to transfer MSAA rendered image data to a presentable non-MSAA target.
		/// </summary>
		/// <param name="p">Per window data used for recording.</param>
		static void MSAAResolvePass(PerWindowData* const p);

		std::string m_rendererTitle;

		TRAP::Ref<VulkanInstance> m_instance;
		TRAP::Scope<VulkanDebug> m_debug;
		TRAP::Ref<VulkanDevice> m_device;
		TRAP::Ref<VulkanMemoryAllocator> m_vma;

		//RenderPass map per thread (this will make lookups lock free and we only need a lock when inserting
		//a RenderPass Map for the first time)
		static std::unordered_map<std::thread::id, RenderPassMap> s_renderPassMap;
		//FrameBuffer map per thread (this will make lookups lock free and we only need a lock when inserting
		//a FrameBuffer Map for the first time)
		static std::unordered_map<std::thread::id, FrameBufferMap> s_frameBufferMap;
		static std::mutex s_renderPassMutex;

		static std::vector<std::pair<std::string, std::array<uint8_t, 16>>> s_usableGPUs;
		static std::unordered_map<uint64_t, TRAP::Ref<Pipeline>> s_pipelines;
		static std::unordered_map<uint64_t, TRAP::Ref<PipelineCache>> s_pipelineCaches;

		static VulkanRenderer* s_renderer;
	};
}

#endif /*TRAP_VULKANRENDERER_H*/

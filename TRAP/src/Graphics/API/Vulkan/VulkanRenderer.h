#ifndef TRAP_VULKANRENDERER_H
#define TRAP_VULKANRENDERER_H

#include <thread>

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
		explicit VulkanRenderer() noexcept;
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
		VulkanRenderer(VulkanRenderer&&) noexcept = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanRenderer& operator=(VulkanRenderer&&) noexcept = delete;

		/// <summary>
		/// Initialize the internal Vulkan renderer.
		/// </summary>
		/// <param name="gameName">Name of the game.</param>
		void InitInternal(std::string_view gameName) override;

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Flush renderer for the given window.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Presents the rendered image to the screen.
		/// 4. Starts graphics and compute recording for the next frame.
		/// </summary>
		/// <param name="window">Window to flush.</param>
		void Flush(const Window* window) const override;
#else
		/// <summary>
		/// Flush renderer.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Starts graphics and compute recording for the next frame.
		/// </summary>
		void Flush() const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Dispatch to the given window.
		/// </summary>
		/// <param name="workGroupElements">
		/// Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// </param>
		/// <param name="window">Window to Dispatch.</param>
		void Dispatch(std::array<uint32_t, 3> workGroupElements, const Window* window) const override;
#else
		/// <summary>
		/// Dispatch.
		/// </summary>
		/// <param name="workGroupElements">
		/// Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// </param>
		void Dispatch(std::array<uint32_t, 3> workGroupElements) const override;
#endif /*TRAP_HEADLESS_MODE*/
		//TODO DispatchIndirect

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the VSync state for the given window.
		/// </summary>
		/// <param name="vsync">Enable or disable VSync.</param>
		/// <param name="window">Window to set VSync for.</param>
		void SetVSync(bool vsync, const Window* window) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the FPS limit for NVIDIA-Reflex.
		/// Note: This function affects all windows.
		/// Note: Do not call this function in user code! Use TRAP::Application::SetFPSLimit() instead.
		///       This function is only used internally for NVIDIA-Reflex.
		/// </summary>
		/// <param name="limit">FPS target to limit to.</param>
		void SetReflexFPSLimit(uint32_t limit) override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the render scale for the given window.
		/// Note: This functon takes effect on the next frame.
		/// </summary>
		/// <param name="scale">Render scale value (valid range: 0.5f-1.0f inclusive).</param>
		/// <param name="window">Window to set render scale for.</param>
		void SetRenderScale(float scale, const Window* window) const override;
#else
		/// <summary>
		/// Set the render scale.
		/// Note: This functon takes effect on the next frame.
		/// </summary>
		/// <param name="scale">Render scale value (valid range: 0.5f-1.0f inclusive).</param>
		void SetRenderScale(float scale) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the used render scale value of the given window.
		/// </summary>
		/// <param name="window">Window to retrieve render scale from.</param>
		/// <returns>Render scale (between 0.5f and 2.0f inclusive).</returns>
		[[nodiscard]] float GetRenderScale(const Window* window) const override;
#else
		/// <summary>
		/// Retrieve the used render scale value.
		/// </summary>
		/// <returns>Render scale (between 0.5f and 2.0f inclusive).</returns>
		[[nodiscard]] float GetRenderScale() const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the clear color to be used by the given window.
		/// </summary>
		/// <param name="color">New clear color.</param>
		/// <param name="window">Window to set clear color for.</param>
		void SetClearColor(const Color& color /*= { 0.1f, 0.1f, 0.1f, 1.0f }*/,
		                   const Window* window) const override;
#else
		/// <summary>
		/// Set the clear color.
		/// </summary>
		/// <param name="color">New clear color.</param>
		void SetClearColor(const Color& color /*= { 0.1f, 0.1f, 0.1f, 1.0f }*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the clear depth value to be used by the given window.
		/// </summary>
		/// <param name="depth">New clear depth value. Must be between 0.0f and 1.0f</param>
		/// <param name="window">Window to set clear depth value for.</param>
		void SetClearDepth(float depth /*= 0.0f*/, const Window* window) const override;
#else
		/// <summary>
		/// Set the clear depth value.
		/// </summary>
		/// <param name="depth">New clear depth value. Must be between 0.0f and 1.0f</param>
		void SetClearDepth(float depth /*= 0.0f*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the clear stencil value to be used by the given window.
		/// </summary>
		/// <param name="stencil">New clear stencil value.</param>
		/// <param name="window">Window to set clear stencil value for.</param>
		void SetClearStencil(uint32_t stencil /*= 0*/, const Window* window) const override;
#else
		/// <summary>
		/// Set the clear stencil value.
		/// </summary>
		/// <param name="stencil">New clear stencil value.</param>
		void SetClearStencil(uint32_t stencil /*= 0*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the resolution of the render targets.
		/// </summary>
		/// <param name="width">New width.</param>
		/// <param name="height">New height.</param>
		void SetResolution(uint32_t width, uint32_t height) const override;
#endif
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Enable or disable depth testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth testing.</param>
		/// <param name="window">Window to set depth testing for.</param>
		void SetDepthTesting(bool enabled, const Window* window) const override;
#else
		/// <summary>
		/// Enable or disable depth testing.
		/// </summary>
		/// <param name="enabled">Enable or disable depth testing.</param>
		void SetDepthTesting(bool enabled) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Enable or disable depth writing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth writing.</param>
		/// <param name="window">Window to set depth writing for.</param>
		void SetDepthWriting(bool enabled, const Window* window) const override;
#else
		/// <summary>
		/// Enable or disable depth writing.
		/// </summary>
		/// <param name="enabled">Enable or disable depth writing.</param>
		void SetDepthWriting(bool enabled) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the depth function for the given window.
		/// </summary>
		/// <param name="function">Function to use for depth testing.</param>
		/// <param name="window">Window to set depth function for.</param>
		void SetDepthFunction(CompareMode function, const Window* window) const override;
#else
		/// <summary>
		/// Set the depth function.
		/// </summary>
		/// <param name="function">Function to use for depth testing.</param>
		void SetDepthFunction(CompareMode function) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the depth action to perform when depth testing fails for the given window.
		/// </summary>
		/// <param name="front">Depth action to perform when depth testing fails.</param>
		/// <param name="back">Depth action to perform when depth testing fails.</param>
		/// <param name="window">Window to set the depth fail action for.</param>
		void SetDepthFail(StencilOp front, StencilOp back, const Window* window) const override;
#else
		/// <summary>
		/// Set the depth action to perform when depth testing fails.
		/// </summary>
		/// <param name="front">Depth action to perform when depth testing fails.</param>
		/// <param name="back">Depth action to perform when depth testing fails.</param>
		void SetDepthFail(StencilOp front, StencilOp back) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the depth bias (scalar factor to add to each fragments depth value) for the given window.
		/// </summary>
		/// <param name="depthBias">Depth bias.</param>
		/// <param name="window">Window to set the depth bias for.</param>
		void SetDepthBias(int32_t depthBias, const Window* window) const override;
#else
		/// <summary>
		/// Set the depth bias (scalar factor to add to each fragments depth value).
		/// </summary>
		/// <param name="depthBias">Depth bias.</param>
		void SetDepthBias(int32_t depthBias) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation) for the given window.
		/// </summary>
		/// <param name="factor">Depth bias slope factor.</param>
		/// <param name="window">Window to set the depth bias slope factor for.</param>
		void SetDepthBiasSlopeFactor(float factor, const Window* window) const override;
#else
		/// <summary>
		/// Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation).
		/// </summary>
		/// <param name="factor">Depth bias slope factor.</param>
		void SetDepthBiasSlopeFactor(float factor) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Enable or disable stencil testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable stencil testing.</param>
		/// <param name="window">Window to set stencil testing for.</param>
		void SetStencilTesting(bool enabled, const Window* window) const override;
#else
		/// <summary>
		/// Enable or disable stencil testing.
		/// </summary>
		/// <param name="enabled">Enable or disable stencil testing.</param>
		void SetStencilTesting(bool enabled) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the stencil action to perform when stencil testing fails for the given window.
		/// </summary>
		/// <param name="front">Stencil action to perform when stencil testing fails.</param>
		/// <param name="back">Stencil action to perform when stencil testing fails.</param>
		/// <param name="window">Window to set the stencil fail action for.</param>
		void SetStencilFail(StencilOp front, StencilOp back, const Window* window) const override;
#else
		/// <summary>
		/// Set the stencil action to perform when stencil testing fails.
		/// </summary>
		/// <param name="front">Stencil action to perform when stencil testing fails.</param>
		/// <param name="back">Stencil action to perform when stencil testing fails.</param>
		void SetStencilFail(StencilOp front, StencilOp back) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the stencil action to perform when stencil testing and depth testing passes for the given window.
		/// </summary>
		/// <param name="front">Stencil action to perform when passed.</param>
		/// <param name="back">Stencil action to perform when passed.</param>
		/// <param name="window">Window to set the stencil pass action for.</param>
		void SetStencilPass(StencilOp front, StencilOp back, const Window* window) const override;
#else
		/// <summary>
		/// Set the stencil action to perform when stencil testing and depth testing passes.
		/// </summary>
		/// <param name="front">Stencil action to perform when passed.</param>
		/// <param name="back">Stencil action to perform when passed.</param>
		void SetStencilPass(StencilOp front, StencilOp back) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the stencil functions for the given window.
		/// </summary>
		/// <param name="front">Function to use on the front for stencil testing.</param>
		/// <param name="back">Function to use on the back for stencil testing.</param>
		/// <param name="window">Window to set stencil functions for.</param>
		void SetStencilFunction(CompareMode front, CompareMode back, const Window* window) const override;
#else
		/// <summary>
		/// Set the stencil functions.
		/// </summary>
		/// <param name="front">Function to use on the front for stencil testing.</param>
		/// <param name="back">Function to use on the back for stencil testing.</param>
		void SetStencilFunction(CompareMode front, CompareMode back) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the stencil mask for the given window.
		/// </summary>
		/// <param name="read">Select the bits of the stencil values to test.</param>
		/// <param name="write">Select the bits of the stencil values updated by the stencil test.</param>
		/// <param name="window">Window to set stencil mask for.</param>
		void SetStencilMask(uint8_t read, uint8_t write, const Window* window) const override;
#else
		/// <summary>
		/// Set the stencil mask.
		/// </summary>
		/// <param name="read">Select the bits of the stencil values to test.</param>
		/// <param name="write">Select the bits of the stencil values updated by the stencil test.</param>
		void SetStencilMask(uint8_t read, uint8_t write) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the cull mode for the given window.
		/// </summary>
		/// <param name="mode">Cull mode to use.</param>
		/// <param name="window">Window to set cull mode for.</param>
		void SetCullMode(CullMode mode, const Window* window) const override;
#else
		/// <summary>
		/// Set the cull mode.
		/// </summary>
		/// <param name="mode">Cull mode to use.</param>
		void SetCullMode(CullMode mode) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the fill mode for the given window.
		/// </summary>
		/// <param name="mode">Fill mode to use.</param>
		/// <param name="window">Window to set fill mode for.</param>
		void SetFillMode(FillMode mode, const Window* window) const override;
#else
		/// <summary>
		/// Set the fill mode.
		/// </summary>
		/// <param name="mode">Fill mode to use.</param>
		void SetFillMode(FillMode mode) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the primitive topology for the given window.
		/// </summary>
		/// <param name="topology">Primitive topology to use.</param>
		/// <param name="window">Window to set primitive topology for.</param>
		void SetPrimitiveTopology(PrimitiveTopology topology, const Window* window) const override;
#else
		/// <summary>
		/// Set the primitive topology.
		/// </summary>
		/// <param name="topology">Primitive topology to use.</param>
		void SetPrimitiveTopology(PrimitiveTopology topology) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the front face winding order for the given window.
		/// </summary>
		/// <param name="face">Front face winding order to use.</param>
		/// <param name="window">Window to set front face winding order for.</param>
		void SetFrontFace(FrontFace face, const Window* window) const override;
#else
		/// <summary>
		/// Set the front face winding order.
		/// </summary>
		/// <param name="face">Front face winding order to use.</param>
		void SetFrontFace(FrontFace face) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the blend mode for the given window.
		/// </summary>
		/// <param name="modeRGB">Blend mode to use for the RGB channels.</param>
		/// <param name="modeAlpha">Blend mode to use for the alpha channel.</param>
		/// <param name="window">Window to set the blend mode for.</param>
		void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, const Window* window) const override;
#else
		/// <summary>
		/// Set the blend mode.
		/// </summary>
		/// <param name="modeRGB">Blend mode to use for the RGB channels.</param>
		/// <param name="modeAlpha">Blend mode to use for the alpha channel.</param>
		void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the blend constants/factors for the given window.
		/// </summary>
		/// <param name="sourceRGB">Specifies how the red, green, and blue blending factors are computed.</param>
		/// <param name="sourceAlpha">Specifies how the alpha source blending factor is computed.</param>
		/// <param name="destinationRGB">Specifies how the red, green, and blue destination blending factors are computed.</param>
		/// <param name="destinationAlpha">Specified how the alpha destination blending factor is computed.</param>
		/// <param name="window">Window to set the blend constants for.</param>
		void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                  BlendConstant destinationRGB, BlendConstant destinationAlpha,
							  const Window* window) const override;
#else
		/// <summary>
		/// Set the blend constants/factors.
		/// </summary>
		/// <param name="sourceRGB">Specifies how the red, green, and blue blending factors are computed.</param>
		/// <param name="sourceAlpha">Specifies how the alpha source blending factor is computed.</param>
		/// <param name="destinationRGB">Specifies how the red, green, and blue destination blending factors are computed.</param>
		/// <param name="destinationAlpha">Specified how the alpha destination blending factor is computed.</param>
		void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                  BlendConstant destinationRGB, BlendConstant destinationAlpha) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		/// <param name="window">Window to set the shading rate for.</param>
		void SetShadingRate(ShadingRate shadingRate,
		                    ShadingRateCombiner postRasterizerRate,
							ShadingRateCombiner finalRate, const Window* window) const override;
#else
		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		void SetShadingRate(ShadingRate shadingRate,
		                    ShadingRateCombiner postRasterizerRate,
							ShadingRateCombiner finalRate) const override;
#endif /*TRAP_HEADLESS_MODE*/
		//TODO EXPERIMENTAL
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the pipeline fragment shading rate via texture.
		/// </summary>
		/// <param name="texture">
		/// Shading rate texture to use.
		/// Note: The texture must be in ResourceState::ShadingRateSource.
		/// </param>
		/// <param name="window">Window to set shading rate for.</param>
		void SetShadingRate(Ref<RenderTarget> texture, const Window* window) const override;
#else
		/// <summary>
		/// Set the pipeline fragment shading rate via texture.
		/// </summary>
		/// <param name="texture">
		/// Shading rate texture to use.
		/// Note: The texture must be in ResourceState::ShadingRateSource.
		/// </param>
		void SetShadingRate(Ref<RenderTarget> texture) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Clear the given window's render target.
		/// </summary>
		/// <param name="clearType">Type of buffer to clear.</param>
		/// <param name="window">Window to clear.</param>
		void Clear(ClearBufferType clearType, const Window* window) const override;
#else
		/// <summary>
		/// Clear the given render target.
		/// </summary>
		/// <param name="clearType">Type of buffer to clear.</param>
		void Clear(ClearBufferType clearType) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set viewport size for the given window.
		/// </summary>
		/// <param name="x">X coordinate of the top left corner of the viewport.</param>
		/// <param name="y">Y coordinate of the top left corner of the viewport.</param>
		/// <param name="width">New viewport width.</param>
		/// <param name="height">New viewport height.</param>
		/// <param name="minDepth">New min depth value. Default: 0.0f.</param>
		/// <param name="maxDepth">New max depth value. Default: 1.0f.</param>
		/// <param name="window">Window to set viewport for.</param>
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, float minDepth /*= 0.0f*/,
		                 float maxDepth /*= 1.0f*/, const Window* window) const override;
#else
		/// <summary>
		/// Set viewport size.
		/// </summary>
		/// <param name="x">X coordinate of the top left corner of the viewport.</param>
		/// <param name="y">Y coordinate of the top left corner of the viewport.</param>
		/// <param name="width">New viewport width.</param>
		/// <param name="height">New viewport height.</param>
		/// <param name="minDepth">New min depth value. Default: 0.0f.</param>
		/// <param name="maxDepth">New max depth value. Default: 1.0f.</param>
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, float minDepth /*= 0.0f*/,
		                 float maxDepth /*= 1.0f*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set scissor size for the given window.
		/// </summary>
		/// <param name="x">Upper left corner.</param>
		/// <param name="y">Upper left corner.</param>
		/// <param name="width">New scissor width.</param>
		/// <param name="height">New scissor height.</param>
		/// <param name="window">Window to set scissor size for.</param>
		void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
		                const Window* window) const override;
#else
		/// <summary>
		/// Set scissor size.
		/// </summary>
		/// <param name="x">Upper left corner.</param>
		/// <param name="y">Upper left corner.</param>
		/// <param name="width">New scissor width.</param>
		/// <param name="height">New scissor height.</param>
		void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw non-indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for.</param>
		void Draw(uint32_t vertexCount, uint32_t firstVertex /*= 0*/, const Window* window) const override;
#else
		/// <summary>
		/// Draw non-indexed, non-instanced geometry.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		void Draw(uint32_t vertexCount, uint32_t firstVertex /*= 0*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for.</param>
		void DrawIndexed(uint32_t indexCount, uint32_t firstIndex /*= 0*/, uint32_t firstVertex /*= 0*/,
		                 const Window* window) const override;
#else
		/// <summary>
		/// Draw indexed, non-instanced geometry.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		void DrawIndexed(uint32_t indexCount, uint32_t firstIndex /*= 0*/, uint32_t firstVertex /*= 0*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw non-indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		/// <param name="window">Window to draw for.</param>
		void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex /*= 0*/,
		                   uint32_t firstInstance /*= 0*/, const Window* window) const override;
#else
		/// <summary>
		/// Draw non-indexed, instanced geometry.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex /*= 0*/,
		                   uint32_t firstInstance /*= 0*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for.</param>
		void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
		                          uint32_t firstIndex /*= 0*/, uint32_t firstInstance /*= 0*/,
								  uint32_t firstVertex /*= 0*/, const Window* window) const override;
#else
		/// <summary>
		/// Draw indexed, instanced geometry.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
		                          uint32_t firstIndex /*= 0*/, uint32_t firstInstance /*= 0*/,
								  uint32_t firstVertex /*= 0*/) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind shader on the given window.
		/// </summary>
		/// <param name="shader">Shader to bind.</param>
		/// <param name="window">Window to bind the shader for.</param>
		void BindShader(Shader* shader, const Window* window) const;
#else
		/// <summary>
		/// Bind shader on the given window.
		/// </summary>
		/// <param name="shader">Shader to bind.</param>
		void BindShader(Shader* shader) const;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind vertex buffer on the given window.
		/// </summary>
		/// <param name="vBuffer">Vertex buffer to bind.</param>
		/// <param name="layout">Layout of the vertex buffer.</param>
		/// <param name="window">Window to bind the vertex buffer for.</param>
		void BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer, const VertexBufferLayout& layout,
		                      const Window* window) const override;
#else
		/// <summary>
		/// Bind vertex buffer.
		/// </summary>
		/// <param name="vBuffer">Vertex buffer to bind.</param>
		/// <param name="layout">Layout of the vertex buffer.</param>
		void BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer, const VertexBufferLayout& layout) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind an index buffer on the given window.
		/// </summary>
		/// <param name="iBuffer">Index buffer to bind.</param>
		/// <param name="indexType">Data type used by the index buffer.</param>
		/// <param name="window">Window to bind the vertex buffer for.</param>
		void BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer, IndexType indexType,
		                     const Window* window) const override;
#else
		/// <summary>
		/// Bind an index buffer.
		/// </summary>
		/// <param name="iBuffer">Index buffer to bind.</param>
		/// <param name="indexType">Data type used by the index buffer.</param>
		void BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer, IndexType indexType) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind a descriptor set on the given window.
		/// </summary>
		/// <param name="dSet">Descriptor set to bind.</param>
		/// <param name="index">Index for which descriptor set to bind.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the descriptor set for.</param>
		void BindDescriptorSet(DescriptorSet& dSet, uint32_t index,
		                       QueueType queueType /*= QueueType::Graphics*/,
							   const Window* window) const override;
#else
		/// <summary>
		/// Bind a descriptor set.
		/// </summary>
		/// <param name="dSet">Descriptor set to bind.</param>
		/// <param name="index">Index for which descriptor set to bind.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		void BindDescriptorSet(DescriptorSet& dSet, uint32_t index,
		                       QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind push constant buffer data on the given window.
		/// Note: There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// </summary>
		/// <param name="name">Name of the push constant block.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the push constants for.</param>
		void BindPushConstants(const char* name, const void* constantsData,
		                       QueueType queueType /*= QueueType::Graphics*/,
							   const Window* window) const override;
#else
		/// <summary>
		/// Bind push constant buffer data.
		/// Note: There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// </summary>
		/// <param name="name">Name of the push constant block.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		void BindPushConstants(const char* name, const void* constantsData,
		                       QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind push constant buffer data on the given window.
		/// </summary>
		/// <param name="paramIndex">Index of the push constant block in the RootSignatures descriptors array.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the push constants for.</param>
		void BindPushConstantsByIndex(uint32_t paramIndex, const void* constantsData,
									  QueueType queueType /*= QueueType::Graphics*/,
									  const Window* window) const override;
#else
		/// <summary>
		/// Bind push constant buffer data.
		/// </summary>
		/// <param name="paramIndex">Index of the push constant block in the RootSignatures descriptors array.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		void BindPushConstantsByIndex(uint32_t paramIndex, const void* constantsData,
									  QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
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
		/// <param name="window">Window to bind the render target(s) for.</param>
		void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                      const TRAP::Ref<Graphics::RenderTarget>& depthStencil /*= nullptr*/,
							  const RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
							  std::vector<uint32_t>* colorArraySlices /*= nullptr*/,
							  std::vector<uint32_t>* colorMipSlices /*= nullptr*/,
							  uint32_t depthArraySlice /*= -1*/, uint32_t depthMipSlice /*= -1*/,
							  const Window* window) const override;
#else
		/// <summary>
		/// Bind render target(s).
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
		void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                      const TRAP::Ref<Graphics::RenderTarget>& depthStencil /*= nullptr*/,
							  const RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
							  std::vector<uint32_t>* colorArraySlices /*= nullptr*/,
							  std::vector<uint32_t>* colorMipSlices /*= nullptr*/,
							  uint32_t depthArraySlice /*= -1*/, uint32_t depthMipSlice /*= -1*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
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
		/// <param name="window">Window to bind the render target(s) for.</param>
		void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                       const TRAP::Ref<Graphics::RenderTarget>& depthStencil /*= nullptr*/,
							   const RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
							   std::vector<uint32_t>* colorArraySlices /*= nullptr*/,
							   std::vector<uint32_t>* colorMipSlices /*= nullptr*/,
							   uint32_t depthArraySlice /*= -1*/, uint32_t depthMipSlice /*= -1*/,
							   const Window* window) const override;
#else
		/// <summary>
		/// Bind render target(s).
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
		void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                       const TRAP::Ref<Graphics::RenderTarget>& depthStencil /*= nullptr*/,
							   const RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
							   std::vector<uint32_t>* colorArraySlices /*= nullptr*/,
							   std::vector<uint32_t>* colorMipSlices /*= nullptr*/,
							   uint32_t depthArraySlice /*= -1*/, uint32_t depthMipSlice /*= -1*/) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="bufferBarrier">Buffer barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barrier for.</param>
		void ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
								   QueueType queueType /*= QueueType::Graphics*/,
								   const Window* window) const override;
#else
		/// <summary>
		/// Add a resource barrier (memory dependency).
		/// </summary>
		/// <param name="bufferBarrier">Buffer barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		void ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
								   QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="bufferBarriers">Buffer barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barriers for.</param>
		void ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
									QueueType queueType /*= QueueType::Graphics*/,
									const Window* window) const override;
#else
		/// <summary>
		/// Add resource barriers (memory dependencies).
		/// </summary>
		/// <param name="bufferBarriers">Buffer barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		void ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
									QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="textureBarrier">Texture barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barrier for.</param>
		void ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
									QueueType queueType /*= QueueType::Graphics*/,
									const Window* window) const override;
#else
		/// <summary>
		/// Add a resource barrier (memory dependency).
		/// </summary>
		/// <param name="textureBarrier">Texture barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		void ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
									QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="textureBarriers">Texture barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barriers for.</param>
		void ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
									 QueueType queueType /*= QueueType::Graphics*/,
									 const Window* window) const override;
#else
		/// <summary>
		/// Add resource barriers (memory dependencies).
		/// </summary>
		/// <param name="textureBarriers">Texture barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		void ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
									 QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="renderTargetBarrier">Render target barrier.</param>
		/// <param name="window">Window to add the barrier for.</param>
		void ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
									     const Window* window) const override;
#else
		/// <summary>
		/// Add a resource barrier (memory dependency).
		/// </summary>
		/// <param name="renderTargetBarrier">Render target barrier.</param>
		void ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="renderTargetBarriers">Render target barriers.</param>
		/// <param name="window">Window to add the barriers for.</param>
		void ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
									      const Window* window) const override;
#else
		/// <summary>
		/// Add resource barriers (memory dependencies).
		/// </summary>
		/// <param name="renderTargetBarriers">Render target barriers.</param>
		void ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// NVIDIA-Reflex Sleep/synchronize.
		/// </summary>
		void ReflexSleep() const override;
		/// <summary>
		/// NVIDIA-Reflex latency marker.
		/// </summary>
		/// <param name="frame">Frame to set marker for. Must be unique for each frame!</param>
		/// <param name="marker">Enum value of the marker to set.</param>
		void ReflexMarker(uint32_t frame, uint32_t marker) const override;
#ifdef NVIDIA_REFLEX_AVAILABLE
		/// <summary>
		/// Retrieve the latency report from NVIDIA-Reflex.
		/// </summary>
		/// <returns>Latency report.</returns>
		[[nodiscard]] NVLL_VK_LATENCY_RESULT_PARAMS ReflexGetLatency() const override;
#endif /*NVIDIA_REFLEX_AVAILABLE*/
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Retrieve the renderer title.
		/// Example title: "[Vulkan 1.3.0]".
		/// </summary>
		/// <returns>Renderer title.</returns>
		[[nodiscard]] std::string GetTitle() const noexcept override;

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve whether VSync is enabled or not for the given window.
		/// </summary>
		/// <param name="window">Window to retrieve VSync for.</param>
		/// <returns>True if VSync is enabled, false otherwise.</returns>
		[[nodiscard]] bool GetVSync(const Window* window) const override;
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Retrieve the currently used GPUs UUID.
		/// </summary>
		/// <returns>GPU's UUID.</returns>
		[[nodiscard]] std::array<uint8_t, 16> GetCurrentGPUUUID() const noexcept override;
		/// <summary>
		/// Retrieve the name of the currently used GPU.
		/// </summary>
		/// <returns>GPU's name.</returns>
		[[nodiscard]] std::string GetCurrentGPUName() const noexcept override;
		/// <summary>
		/// Retrieve the vendor of the currently used GPU.
		/// </summary>
		/// <returns>GPU vendor.</returns>
		[[nodiscard]] GPUVendor GetCurrentGPUVendor() const noexcept override;
		/// <summary>
		/// Retrieve a list of all supported GPUs.
		/// The list contains the GPUs name and UUID.
		/// </summary>
		/// <returns>List of all supported GPUs.</returns>
		[[nodiscard]] std::vector<std::pair<std::string, std::array<uint8_t, 16>>> GetAllGPUs() const override;

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Capture a screenshot of the last presented frame.
		/// </summary>
		/// <param name="window">Window to capture screenshot on.</param>
		/// <returns>Captured screenshot as TRAP::Image on success, Black 1x1 TRAP::Image otherwise.</returns>
		[[nodiscard]] TRAP::Scope<TRAP::Image> CaptureScreenshot(const Window* window) const override;
#else
		/// <summary>
		/// Capture a screenshot of the last presented frame.
		/// </summary>
		/// <returns>Captured screenshot as TRAP::Image on success, Black 1x1 TRAP::Image otherwise.</returns>
		[[nodiscard]] TRAP::Scope<TRAP::Image> CaptureScreenshot() const override;
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Resolve a MSAA render target to a non MSAA render target.
		/// Needed to transfer MSAA rendered image data to a presentable non-MSAA target.
		///
		/// Note: source and destination must be in ResourceState::RenderTarget.
		/// </summary>
		/// <param name="source">Source MSAA render target to resolve.</param>
		/// <param name="destination">Destination non MSAA render target to resolve into.</param>
		/// <param name="cmd">CommadBuffer to resolve on.</param>
		void MSAAResolvePass(TRAP::Ref<RenderTarget> source, TRAP::Ref<RenderTarget> destination,
		                     CommandBuffer* cmd) const override;

		/// <summary>
		/// Update the internal RenderTargets used for render scaling.
		/// </summary>
		/// <param name="viewportData">PerViewportData to update.</param>
		void UpdateInternalRenderTargets(PerViewportData* viewportData) const;

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Scale image from internal resolution to the final output resolution.
		///
		/// Note: source and destination must be in ResourceState::RenderTarget.
		/// </summary>
		/// <param name="source">Source render target to resolve.</param>
		/// <param name="destination">Destination render target to resolve into.</param>
		/// <param name="window">Window to do the scaling pass on.</param>
		void RenderScalePass(TRAP::Ref<RenderTarget> source,
							 TRAP::Ref<RenderTarget> destination,
		                     const Window* window) const override;
#else
		/// <summary>
		/// Scale image from internal resolution to the final output resolution.
		///
		/// Note: source and destination must be in ResourceState::RenderTarget.
		/// </summary>
		/// <param name="source">Source render target to resolve.</param>
		/// <param name="destination">Destination render target to resolve into.</param>
		void RenderScalePass(TRAP::Ref<RenderTarget> source,
							 TRAP::Ref<RenderTarget> destination) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the latency mode.
		/// Note: Only LatencyMode::Disabled is supported everywhere.
		///       Other LatencyModes are only available on Windows 10 or
		///       newer with NVIDIA hardware.
		/// </summary>
		/// <param name="mode">LatencyMode to set.</param>
		/// <param name="window">Window to set latency mode for.</param>
		/// <returns>True on success, false otherwise.</returns>
		void SetLatencyMode(LatencyMode mode, const Window* window) override;
		/// <summary>
		/// Retrieve the currently used latency mode.
		/// Note: This may differ from the requested mode set with SetLatencyMode().
		/// </summary>
		/// <param name="window">Window to retrieve latency mode for.</param>
		/// <returns>Used latency mode.</returns>
		[[nodiscard]] LatencyMode GetLatencyMode(const Window* window) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Initialize the internal rendering data of the given window.
		/// </summary>
		/// <param name="window">Window to initialize the internal rendering data for.</param>
		void InitPerViewportData(Window* window) const override;
#else
		/// <summary>
		/// Initialize the internal rendering data.
		/// </summary>
		void InitPerViewportData() const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Remove the internal rendering data of the given window.
		/// </summary>
		/// <param name="window">Window to remove the internal rendering data from.</param>
		void RemovePerViewportData(const Window* window) const override;
#else
		/// <summary>
		/// Remove the internal rendering data.
		/// </summary>
		void RemovePerViewportData() const override;
#endif /*TRAP_HEADLESS_MODE*/

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
		static bool s_timelineSemaphore;
		static bool s_multiView;
		static bool s_renderPass2;

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
			//Shading rate texture to use
			TRAP::Ref<RenderTarget> ShadingRate;
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
			ImageFormat ShadingRateFormat;
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
			std::array<TRAP::Ref<TRAP::Graphics::Texture>,
			           static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount)> DefaultTextureSRV;
			std::array<TRAP::Ref<TRAP::Graphics::Texture>,
			           static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount)> DefaultTextureUAV;
			TRAP::Ref<VulkanBuffer> DefaultBufferSRV;
			TRAP::Ref<VulkanBuffer> DefaultBufferUAV;
			TRAP::Ref<VulkanSampler> DefaultSampler;
			TracyLockable(std::mutex, SubmitMutex);

			//Unlike DirectX 12, Vulkan textures start in undefined layout.
			//With this, we transition them to the specified layout so app code doesn't
			//have to worry about this
			TracyLockable(std::mutex, InitialTransitionMutex);
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
		static void UtilInitialTransition(const Ref<TRAP::Graphics::Texture>& texture, RendererAPI::ResourceState startState);

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
		[[nodiscard]] static RenderPassMap& GetRenderPassMap();
		/// <summary>
		/// Retrieve the framebuffer hash map.
		/// </summary>
		/// <returns>Framebuffer hash map.</returns>
		[[nodiscard]] static FrameBufferMap& GetFrameBufferMap();

		/// <summary>
		/// Retrieve the vulkan instance used by the renderer.
		/// </summary>
		/// <returns>Vulkan instance.</returns>
		[[nodiscard]] TRAP::Ref<VulkanInstance> GetInstance() const noexcept;
		/// <summary>
		/// Retrieve the vulkan device used by the renderer.
		/// </summary>
		/// <returns>Vulkan device.</returns>
		[[nodiscard]] TRAP::Ref<VulkanDevice> GetDevice() const noexcept;
		/// <summary>
		/// Retrieve the vulkan memory allocator used by the renderer.
		/// </summary>
		/// <returns>Vulkan memory allocator.</returns>
		[[nodiscard]] TRAP::Ref<VulkanMemoryAllocator> GetVMA() const noexcept;

		/// <summary>
		/// Utility to retrieve a pipeline.
		/// If a pipeline with the same description was already created,
		/// it will be retrieved from the cache.
		/// </summary>
		/// <param name="desc">Pipeline description.</param>
		/// <returns>Pipeline.</returns>
		[[nodiscard]] static const TRAP::Ref<Pipeline>& GetPipeline(PipelineDesc& desc);

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
		[[nodiscard]] static std::vector<std::string> SetupInstanceLayers();
		/// <summary>
		/// Retrieve the list of instance extensions to use.
		///
		/// 1. Surface extension(s) (Mandatory when not using Headless mode).
		/// 2. Debug extension (if supported) (Only active when ENABLE_GRAPHICS_DEBUG is defined).
		/// 3. HDR extension (if supported).
		/// 4. VR extension(s) (if supported).
		/// </summary>
		/// <returns>List of instance extensions.</returns>
		[[nodiscard]] static std::vector<std::string> SetupInstanceExtensions();
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
		[[nodiscard]] static std::vector<std::string> SetupDeviceExtensions(VulkanPhysicalDevice* physicalDevice);

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
		static void StartGraphicRecording(PerViewportData* p);
		/// <summary>
		/// Stop recording the graphics pipeline.
		/// </summary>
		/// <param name="p">Per window data used for recording.</param>
		static void EndGraphicRecording(PerViewportData* p);

		/// <summary>
		/// Start recording the compute pipeline.
		/// </summary>
		/// <param name="p">Per window data used for recording.</param>
		static void StartComputeRecording(PerViewportData* p);
		/// <summary>
		/// Stop recording the compute pipeline.
		/// </summary>
		/// <param name="p">Per window data used for recording.</param>
		static void EndComputeRecording(PerViewportData* p);

		/// <summary>
		/// Present the currently recorded frame.
		/// </summary>
		/// <param name="p">Per widow data used for presentation.</param>
		static void Present(PerViewportData* p);

		/// <summary>
		/// Start frame time profiling of the GPU.
		/// </summary>
		/// <param name="type">Queue type to profile.</param>
		/// <param name="p">Per window data to profile for.</param>
		static void BeginGPUFrameProfile(QueueType type, const PerViewportData* p);
		/// <summary>
		/// End frame time profiling of the GPU.
		/// </summary>
		/// <param name="type">Queue type to profile.</param>
		/// <param name="p">Per window data to profile for.</param>
		static void EndGPUFrameProfile(QueueType type, const PerViewportData* p);
		/// <summary>
		/// Retrieve the result from GPU frame time profiling.
		/// </summary>
		/// <param name="type">Queue type to profile.</param>
		/// <param name="p">Per window data to profile for.</param>
		[[nodiscard]] static float ResolveGPUFrameProfile(QueueType type, const PerViewportData* p);

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
		inline static TracyLockable(std::mutex, s_renderPassMutex);

		static std::vector<std::pair<std::string, std::array<uint8_t, 16>>> s_usableGPUs;
		static std::unordered_map<uint64_t, TRAP::Ref<Pipeline>> s_pipelines;
		static std::unordered_map<uint64_t, TRAP::Ref<PipelineCache>> s_pipelineCaches;

		static VulkanRenderer* s_renderer;
	};
}

#endif /*TRAP_VULKANRENDERER_H*/

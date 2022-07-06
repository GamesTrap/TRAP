#ifndef TRAP_RENDERCOMMAND_H
#define TRAP_RENDERCOMMAND_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	/// <summary>
	/// Different buffers for clearing.
	/// </summary>
	using ClearBuffer = RendererAPI::ClearBufferType;
	/// <summary>
	/// Different compare modes for depth and stencil testing.
	/// </summary>
	using CompareMode = RendererAPI::CompareMode;
	/// <summary>
	/// Different stencil operation options.
	/// </summary>
	using StencilOperation = RendererAPI::StencilOp;
	/// <summary>
	/// Different cull modes.
	/// </summary>
	using CullMode = RendererAPI::CullMode;
	/// <summary>
	/// Different fill modes.
	/// </summary>
	using FillMode = RendererAPI::FillMode;
	/// <summary>
	/// Different primitive topologies.
	/// </summary>
	using PrimitiveTopology = RendererAPI::PrimitiveTopology;
	/// <summary>
	/// Different front face options.
	/// </summary>
	using FrontFace = RendererAPI::FrontFace;
	/// <summary>
	/// Different blend modes.
	/// </summary>
	using BlendMode = RendererAPI::BlendMode;
	/// <summary>
	/// Different blend constants.
	/// </summary>
	using BlendConstant = RendererAPI::BlendConstant;
	/// <summary>
	/// Different shading rates.
	/// </summary>
	using ShadingRate = RendererAPI::ShadingRate;
	/// <summary>
	/// Different shading rate combiners.
	/// </summary>
	using ShadingRateCombiner = RendererAPI::ShadingRateCombiner;
	/// <summary>
	/// Different queue types.
	/// </summary>
	using QueueType = RendererAPI::QueueType;
	/// <summary>
	/// Different resource states.
	/// </summary>
	using ResourceState = RendererAPI::ResourceState;
	/// <summary>
	/// Different anti aliasing methods.
	/// </summary>
	using AntiAliasing = RendererAPI::AntiAliasing;
	/// <summary>
	/// Different sample counts for anti aliasing.
	/// </summary>
	using SampleCount = RendererAPI::SampleCount;

	/// <summary>
	/// Utility class for high level rendering commands.
	/// User with knowledge about OpenGL should be familiar with most of these commands.
	///
	/// Note: Most of these functions are just shortcuts for RendererAPI::* functions.
	/// </summary>
	class RenderCommand
	{
	public:
		/// <summary>
		/// Present frame to given window.
		///
		/// Note: TRAP::Application automatically presents to the main window.
		/// </summary>
		/// <param name="window">Window to present to.</param>
		static void Present(Window* window);

		//VSync functions

		/// <summary>
		/// Set VSync for the given window.
		/// </summary>
		/// <param name="vsync">Enable or disable VSync.</param>
		/// <param name="window">Window to set VSync for. Default: Main Window.</param>
		static void SetVSync(bool vsync, Window* window = nullptr);
		/// <summary>
		/// Retrieve VSync state of the given window.
		/// </summary>
		/// <param name="window">Window to get VSync state from. Default: Main Window.</param>
		/// <returns>True if VSync is enabled, false otherwise.</returns>
		static bool GetVSync(Window* window = nullptr);

		//Clear functions

		/// <summary>
		/// Clear the RenderTarget of the given window.
		/// </summary>
		/// <param name="buffer">Type of buffer to clear.</param>
		/// <param name="window">Window to clear RenderTarget for. Default: Main Window.</param>
		static void Clear(ClearBuffer buffer, Window* window = nullptr);
		/// <summary>
		/// Set the clear color for the given window.
		/// </summary>
		/// <param name="color">New clear color. Default: Very dark gray.</param>
		/// <param name="window">Window to set clear color for. Default: Main Window.</param>
		static void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }, Window* window = nullptr);
		/// <summary>
		/// Set the clear depth value for the given window.
		/// </summary>
		/// <param name="depth">New depth value (range [0,1]). Default: 1.</param>
		/// <param name="window">Window to set clear depth value for. Default: Main Window.</param>
		static void SetClearDepth(float depth = 1.0f, Window* window = nullptr);
		/// <summary>
		/// Set the clear stencil value for the given window.
		/// </summary>
		/// <param name="stencil">New stencil value. Default: 0.</param>
		/// <param name="window">Window to set clear stencil value for. Default: Main Window.</param>
		static void SetClearStencil(uint32_t stencil = 0, Window* window = nullptr);

		//Depth functions

		/// <summary>
		/// Enable or disable depth testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth testing.</param>
		/// <param name="window">Window to set depth testing for. Default: Main Window.</param>
		static void SetDepthTesting(bool enabled, Window* window = nullptr);
		/// <summary>
		/// Enable or disable depth writing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth writing.</param>
		/// <param name="window">Window to set depth writing for. Default: Main Window.</param>
		static void SetDepthWriting(bool enabled, Window* window = nullptr);
		/// <summary>
		/// Set depth function for the given window.
		/// </summary>
		/// <param name="compareMode">Depth compare mode. Default: CompareMode::Less.</param>
		/// <param name="window">Window to set depth function for. Default: Main Window.</param>
		static void SetDepthFunction(CompareMode compareMode = CompareMode::Less, Window* window = nullptr);
		/// <summary>
		/// Set operation for when depth testing fails for the given window.
		/// </summary>
		/// <param name="front">Depth/Stencil operation.</param>
		/// <param name="back">Depth/Stencil operation.</param>
		/// <param name="window">Window to set depth fail operation for. Default: Main Window.</param>
		static void SetDepthFail(StencilOperation front, StencilOperation back, Window* window = nullptr);
		/// <summary>
		/// Set depth bias constant factor for the given window.
		/// </summary>
		/// <param name="bias">Depth bias constant factor.</param>
		/// <param name="window">Window to set depth bias for. Default: Main Window.</param>
		static void SetDepthBias(int32_t bias, Window* window = nullptr);
		/// <summary>
		/// Set depth bias slope factor for the given window.
		/// </summary>
		/// <param name="slopeFactor">Depth bias slope factor.</param>
		/// <param name="window">Window to set depth bias for. Default: Main Window.</param>
		static void SetDepthBiasSlopeFactor(float slopeFactor, Window* window = nullptr);

		//Stencil functions

		/// <summary>
		/// Enable or disable stencil testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable stencil testing.</param>
		/// <param name="window">Window to set stencil testing for. Default: Main Window.</param>
		static void SetStencilTesting(bool enabled, Window* window = nullptr);
		/// <summary>
		/// Set operation for when stencil testing fails for the given window.
		/// </summary>
		/// <param name="front">Stencil operation.</param>
		/// <param name="back">Stencil operation.</param>
		/// <param name="window">Window to set stencil fail operation for. Default: Main Window.</param>
		static void SetStencilFail(StencilOperation front, StencilOperation back, Window* window = nullptr);
		/// <summary>
		/// Set operation for when stencil testing passes for the given window.
		/// </summary>
		/// <param name="front">Stencil operation.</param>
		/// <param name="back">Stencil operation.</param>
		/// <param name="window">Window to set stencil pass operation for. Default: Main Window.</param>
		static void SetStencilPass(StencilOperation front, StencilOperation back, Window* window = nullptr);
		/// <summary>
		/// Set stencil function for the given window.
		/// </summary>
		/// <param name="front">Compare mode.</param>
		/// <param name="back">Compare mode.</param>
		/// <param name="window">Window to set stencil function for. Default: Main Window.</param>
		static void SetStencilFunction(CompareMode front, CompareMode back, Window* window = nullptr);
		/// <summary>
		/// Set stencil mask for the given window.
		/// </summary>
		/// <param name="read">Bits to read/compare.</param>
		/// <param name="write">Bits to write/update.</param>
		/// <param name="window">Window to set stencil mask for. Default: Main Window.</param>
		static void SetStencilMask(uint8_t read, uint8_t write, Window* window = nullptr);

		//Miscellaneous functions

		/// <summary>
		/// Set cull mode for the given window.
		/// </summary>
		/// <param name="cullMode">Cull mode.</param>
		/// <param name="window">Window to set cull mode for. Default: Main Window.</param>
		static void SetCullMode(CullMode cullMode, Window* window = nullptr);
		/// <summary>
		/// Set fill mode for the given window.
		/// </summary>
		/// <param name="fillMode">Fill mode.</param>
		/// <param name="window">Window to set fill mode for. Default: Main Window.</param>
		static void SetFillMode(FillMode fillMode, Window* window = nullptr);
		/// <summary>
		/// Set primitive topology for the given window.
		/// </summary>
		/// <param name="topology">Primitive topology.</param>
		/// <param name="window">Window to set primitive topology for. Default: Main Window.</param>
		static void SetPrimitiveTopology(PrimitiveTopology topology, Window* window = nullptr);
		/// <summary>
		/// Set front face for the given window.
		/// </summary>
		/// <param name="face">Front face.</param>
		/// <param name="window">Window to set front face for. Default: Main Window.</param>
		static void SetFrontFace(FrontFace face, Window* window = nullptr);
		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="texture">Unused by Vulkan.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		/// <param name="window">Window to set shading rate for. Default: Main Window.</param>
		static void SetShadingRate(ShadingRate shadingRate,
						           Texture* texture,
		                           ShadingRateCombiner postRasterizerRate,
							       ShadingRateCombiner finalRate, Window* window = nullptr);
		/// <summary>
		/// Set the anti aliasing method and the sample count for the window.
		/// Use AntiAliasing::Off and SampleCount::SampleCount1 to disable anti aliasing.
		/// </summary>
		/// <param name="antiAliasing">Anti aliasing method to use.</param>
		/// <param name="sampleCount">Sample count to use.</param>
		/// <param name="window">Window to set anti aliasing for. Default: Main Window.</param>
		static void SetAntiAliasing(AntiAliasing antiAliasing, SampleCount sampleCount, Window* window = nullptr);

		//Blending functions

		/// <summary>
		/// Set blend mode for the given window.
		/// </summary>
		/// <param name="modeRGB">Blend mode for RGB.</param>
		/// <param name="modeAlpha">Blend mode for alpha.</param>
		/// <param name="window">Window to set blend mode for. Default: Main Window.</param>
		static void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, Window* window = nullptr);
		/// <summary>
		/// Set combined blend constant for the given window.
		/// </summary>
		/// <param name="sourceRGBA">Blend constant for source RGBA.</param>
		/// <param name="destinationRGBA">Blend constant for destination RGBA.</param>
		/// <param name="window">Window to set blend constant for. Default: Main Window.</param>
		static void SetBlendConstant(BlendConstant sourceRGBA, BlendConstant destinationRGBA,
									 Window* window = nullptr);
		/// <summary>
		/// Set separate blend constant for the given window.
		/// </summary>
		/// <param name="sourceRGB">Blend constant for source RGB.</param>
		/// <param name="sourceAlpha">Blend constant for source alpha.</param>
		/// <param name="destinationRGB">Blend constant for destination RGB.</param>
		/// <param name="destinationAlpha">Blend constant for destination alpha.</param>
		/// <param name="window">Window to set blend constant for. Default: Main Window.</param>
		static void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
									 BlendConstant destinationRGB, BlendConstant destinationAlpha,
									 Window* window = nullptr);

		//Viewport/Scissor functions

		/// <summary>
		/// Set viewport size for the given window.
		/// </summary>
		/// <param name="x">Upper left corner. Default: 0.</param>
		/// <param name="y">Upper left corner. Default: 0.</param>
		/// <param name="width">Viewport width.</param>
		/// <param name="height">Viewport height.</param>
		/// <param name="window">Window to set viewport size for. Default: Main Window.</param>
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Window* window = nullptr);
		/// <summary>
		/// Set scissor size for the given window.
		/// </summary>
		/// <param name="x">Upper left corner. Default: 0.</param>
		/// <param name="y">Upper left corner. Default: 0.</param>
		/// <param name="width">Scissor width.</param>
		/// <param name="height">Scissor height.</param>
		/// <param name="window">Window to set scissor size for. Default: Main Window.</param>
		static void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Window* window = nullptr);
#ifdef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set RenderTarget resolution for the given window.
		/// Note: Only usable in Headless mode.
		/// </summary>
		/// <param name="width">Width.</param>
		/// <param name="height">Height.</param>
		/// <param name="window">Window to set resolution for. Default: Main Window.</param>
		static void SetResolution(uint32_t width, uint32_t height, Window* window = nullptr);
#endif

		//Drawing functions

		/// <summary>
		/// Draw non-indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void Draw(uint32_t vertexCount, uint32_t firstVertex = 0, Window* window = nullptr);
		/// <summary>
		/// Draw indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void DrawIndexed(uint32_t indexCount, uint32_t firstIndex = 0, uint32_t firstVertex = 0,
		                        Window* window = nullptr);
		/// <summary>
		/// Draw non-indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw.</param>
		/// <param name="firstInstance">Index of the first instance to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex = 0,
		                          uint32_t firstInstance = 0, Window* window = nullptr);
		/// <summary>
		/// Draw indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw.</param>
		/// <param name="firstInstance">Index of the first instance to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex = 0,
		                                 uint32_t firstInstance = 0, uint32_t firstVertex = 0,
										 Window* window = nullptr);

		//Compute functions

		/// <summary>
		/// Dispatch compute work on the given window.
		/// </summary>
		/// <param name="workGroupElementSizes">
		/// Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// </param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void Dispatch(const std::array<uint32_t, 3>& workGroupElementSizes, Window* window = nullptr);

		//TODO DispatchIndirect

		//CommandBuffer functions

		/// <summary>
		/// Set push constant for the given window.
		/// Note: Minimum guaranteed size is 128 bytes.
		/// </summary>
		/// <param name="name">Name of the push constant.</param>
		/// <param name="data">Data to set push constant to.</param>
		/// <param name="queueType">Queue type on which to perform the operation. Default: Graphics.</param>
		/// <param name="window">Window to set push constant for. Default: Main Window.</param>
		static void SetPushConstants(const char* name, const void* data,
		                             QueueType queueType = QueueType::Graphics, Window* window = nullptr);
		// static void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		//                              const TRAP::Ref<Graphics::RenderTarget>& depthStencil = nullptr,
		// 							 const RendererAPI::LoadActionsDesc* loadActions = nullptr,
		// 							 std::vector<uint32_t>* colorArraySlices = nullptr,
		// 							 std::vector<uint32_t>* colorMipSlices = nullptr,
		// 							 uint32_t depthArraySlice = -1, uint32_t depthMipSlice = -1,
		// 							 Window* window = nullptr);
		// static void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		//                               const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
		// 							  const RendererAPI::LoadActionsDesc* loadActions = nullptr,
		// 							  std::vector<uint32_t>* colorArraySlices = nullptr,
		// 							  std::vector<uint32_t>* colorMipSlices = nullptr,
		// 							  uint32_t depthArraySlice = -1, uint32_t depthMipSlice = -1,
		// 							  Window* window = nullptr);

		/// <summary>
		/// Set render target for the given window.
		/// </summary>
		/// <param name="colorTarget">Color target to set.</param>
		/// <param name="depthStencil">Depth stencil target to set.</param>
		/// <param name="loadActions">Load actions.</param>
		/// <param name="window">Window to set render target for. Default: Main Window.</param>
		static void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil = nullptr,
									 const RendererAPI::LoadActionsDesc* loadActions = nullptr,
									 Window* window = nullptr);
		/// <summary>
		/// Set multiple render targets for the given window.
		/// </summary>
		/// <param name="colorTargets">Color targets to set.</param>
		/// <param name="depthStencil">Depth stencil target to set.</param>
		/// <param name="loadActions">Load actions.</param>
		/// <param name="window">Window to set render target for. Default: Main Window.</param>
		static void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                              const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
									  const RendererAPI::LoadActionsDesc* loadActions = nullptr,
									  Window* window = nullptr);
		/// <summary>
		/// Start a new render pass for the given window.
		///
		/// Note: This will bind the render target for the current frame again.
		/// </summary>
		/// <param name="window">Window to start render pass for. Default: Main Window.</param>
		static void StartRenderPass(Window* window = nullptr);
		/// <summary>
		/// Stop the running render pass for the given window.
		/// </summary>
		/// <param name="window">Window to stop render pass for. Default: Main Window.</param>
		static void StopRenderPass(Window* window = nullptr);
		/// <summary>
		/// Buffer barrier used to synchronize and transition the buffer.
		/// </summary>
		/// <param name="bufferBarrier">Buffer barrier to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition buffer for. Default: Main Window.</param>
		static void BufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
		                          QueueType queueType = QueueType::Graphics, Window* window = nullptr);
		/// <summary>
		/// Buffer barrier used to synchronize and transition multiple buffers.
		/// </summary>
		/// <param name="bufferBarriers">Buffer barriers to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition buffers for. Default: Main Window.</param>
		static void BufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
								   QueueType queueType = QueueType::Graphics, Window* window = nullptr);
		/// <summary>
		/// Texture barrier used to synchronize and transition the texture.
		/// </summary>
		/// <param name="textureBarrier">Texture barrier to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition texture for. Default: Main Window.</param>
		static void TextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
		                           QueueType queueType = QueueType::Graphics, Window* window = nullptr);
		/// <summary>
		/// Texture barrier used to synchronize and transition multiple textures.
		/// </summary>
		/// <param name="textureBarriers">Texture barriers to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition textures for. Default: Main Window.</param>
		static void TextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
		                            QueueType queueType = QueueType::Graphics, Window* window = nullptr);
		/// <summary>
		/// RenderTarget barrier used to synchronize and transition the RenderTarget.
		/// </summary>
		/// <param name="renderTargetBarrier">RenderTarget barrier to use.</param>
		/// <param name="window">Window to sync and transition RenderTarget for. Default: Main Window.</param>
		static void RenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier, Window* window = nullptr);
		/// <summary>
		/// RenderTarget barrier used to synchronize and transition multiple RenderTargets.
		/// </summary>
		/// <param name="renderTargetBarriers">RenderTarget barriers to use.</param>
		/// <param name="window">Window to sync and transition RenderTargets for. Default: Main Window.</param>
		static void RenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers, Window* window = nullptr);

		//Utility

		/// <summary>
		/// Take a screenshot of the RenderTarget from the given window.
		/// </summary>
		/// <param name="window">Window to take screenshot from. Default: Main Window.</param>
		/// <returns>Captured screenshot.</returns>
		static TRAP::Scope<TRAP::Image> CaptureScreenshot(Window* window = nullptr);

		/// <summary>
		/// Transition a texture from old layout to the new layout.
		/// The transition happens immediately and is guaranteed to be complete when the function returns.
		/// </summary>
		/// <param name="texture">Texture to transition layout.</param>
		/// <param name="oldLayout">Current resource state of the given texture.</param>
		/// <param name="newLayout">New resource state for the given texture.</param>
		/// <param name="queueType">Queue type on which to perform the transition. Default: Graphics.</param>
		static void Transition(Texture* texture, RendererAPI::ResourceState oldLayout,
		                       RendererAPI::ResourceState newLayout,
		                       QueueType queueType = QueueType::Graphics);
	};
}

#endif /*TRAP_RENDERCOMMAND_H*/
#ifndef TRAP_RENDERCOMMAND_H
#define TRAP_RENDERCOMMAND_H

#include "Application.h"

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
	/// Different latency modes.
	/// </summary>
	using LatencyMode = RendererAPI::LatencyMode;
	/// <summary>
	/// Different texture filtering modes.
	/// </summary>
	using FilterType = TRAP::Graphics::RendererAPI::FilterType;
	/// <summary>
	/// Different texture addressing modes.
	/// </summary>
	using AddressMode = TRAP::Graphics::RendererAPI::AddressMode;
	/// <summary>
	/// Different texture mip mapping modes.
	/// </summary>
	using MipMapMode = TRAP::Graphics::RendererAPI::MipMapMode;
	/// <summary>
	/// Different update frequencies.
	/// </summary>
	using UpdateFrequency = RendererAPI::DescriptorUpdateFrequency;

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
		/// Flush renderer for the given window.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Presents the rendered image to the screen.
		/// 4. Starts graphics and compute recording for the next frame.
		///
		/// Note: TRAP::Application automatically flushes for the main window.
		/// </summary>
		/// <param name="window">Window to flush renderer. Default: Main Window.</param>
		static void Flush(const Window* const window = TRAP::Application::GetWindow());

		//VSync functions

		/// <summary>
		/// Set VSync for the given window.
		/// </summary>
		/// <param name="vsync">Enable or disable VSync.</param>
		/// <param name="window">Window to set VSync for. Default: Main Window.</param>
		static void SetVSync(bool vsync, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Retrieve VSync state of the given window.
		/// </summary>
		/// <param name="window">Window to get VSync state from. Default: Main Window.</param>
		/// <returns>True if VSync is enabled, false otherwise.</returns>
		static bool GetVSync(const Window* const window = TRAP::Application::GetWindow());

		//Clear functions

		/// <summary>
		/// Clear the RenderTarget of the given window.
		/// </summary>
		/// <param name="buffer">Type of buffer to clear.</param>
		/// <param name="window">Window to clear RenderTarget for. Default: Main Window.</param>
		static void Clear(ClearBuffer buffer, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set the clear color for the given window.
		/// </summary>
		/// <param name="color">New clear color. Default: Very dark gray.</param>
		/// <param name="window">Window to set clear color for. Default: Main Window.</param>
		static void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set the clear depth value for the given window.
		/// </summary>
		/// <param name="depth">New depth value (range [0,1]). Default: 1.</param>
		/// <param name="window">Window to set clear depth value for. Default: Main Window.</param>
		static void SetClearDepth(float depth = 0.0f, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set the clear stencil value for the given window.
		/// </summary>
		/// <param name="stencil">New stencil value. Default: 0.</param>
		/// <param name="window">Window to set clear stencil value for. Default: Main Window.</param>
		static void SetClearStencil(uint32_t stencil = 0, const Window* const window = TRAP::Application::GetWindow());

		//Depth functions

		/// <summary>
		/// Enable or disable depth testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth testing.</param>
		/// <param name="window">Window to set depth testing for. Default: Main Window.</param>
		static void SetDepthTesting(bool enabled, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Enable or disable depth writing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth writing.</param>
		/// <param name="window">Window to set depth writing for. Default: Main Window.</param>
		static void SetDepthWriting(bool enabled, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set depth function for the given window.
		/// </summary>
		/// <param name="compareMode">Depth compare mode. Default: CompareMode::Less.</param>
		/// <param name="window">Window to set depth function for. Default: Main Window.</param>
		static void SetDepthFunction(CompareMode compareMode = CompareMode::Less, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set operation for when depth testing fails for the given window.
		/// </summary>
		/// <param name="front">Depth/Stencil operation.</param>
		/// <param name="back">Depth/Stencil operation.</param>
		/// <param name="window">Window to set depth fail operation for. Default: Main Window.</param>
		static void SetDepthFail(StencilOperation front, StencilOperation back, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set depth bias constant factor for the given window.
		/// </summary>
		/// <param name="bias">Depth bias constant factor.</param>
		/// <param name="window">Window to set depth bias for. Default: Main Window.</param>
		static void SetDepthBias(int32_t bias, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set depth bias slope factor for the given window.
		/// </summary>
		/// <param name="slopeFactor">Depth bias slope factor.</param>
		/// <param name="window">Window to set depth bias for. Default: Main Window.</param>
		static void SetDepthBiasSlopeFactor(float slopeFactor, const Window* const window = TRAP::Application::GetWindow());

		//Stencil functions

		/// <summary>
		/// Enable or disable stencil testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable stencil testing.</param>
		/// <param name="window">Window to set stencil testing for. Default: Main Window.</param>
		static void SetStencilTesting(bool enabled, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set operation for when stencil testing fails for the given window.
		/// </summary>
		/// <param name="front">Stencil operation.</param>
		/// <param name="back">Stencil operation.</param>
		/// <param name="window">Window to set stencil fail operation for. Default: Main Window.</param>
		static void SetStencilFail(StencilOperation front, StencilOperation back, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set operation for when stencil testing passes for the given window.
		/// </summary>
		/// <param name="front">Stencil operation.</param>
		/// <param name="back">Stencil operation.</param>
		/// <param name="window">Window to set stencil pass operation for. Default: Main Window.</param>
		static void SetStencilPass(StencilOperation front, StencilOperation back, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set stencil function for the given window.
		/// </summary>
		/// <param name="front">Compare mode.</param>
		/// <param name="back">Compare mode.</param>
		/// <param name="window">Window to set stencil function for. Default: Main Window.</param>
		static void SetStencilFunction(CompareMode front, CompareMode back, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set stencil mask for the given window.
		/// </summary>
		/// <param name="read">Bits to read/compare.</param>
		/// <param name="write">Bits to write/update.</param>
		/// <param name="window">Window to set stencil mask for. Default: Main Window.</param>
		static void SetStencilMask(uint8_t read, uint8_t write, const Window* const window = TRAP::Application::GetWindow());

		//Miscellaneous functions

		/// <summary>
		/// Set cull mode for the given window.
		/// </summary>
		/// <param name="cullMode">Cull mode.</param>
		/// <param name="window">Window to set cull mode for. Default: Main Window.</param>
		static void SetCullMode(CullMode cullMode, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set fill mode for the given window.
		/// </summary>
		/// <param name="fillMode">Fill mode.</param>
		/// <param name="window">Window to set fill mode for. Default: Main Window.</param>
		static void SetFillMode(FillMode fillMode, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set primitive topology for the given window.
		/// </summary>
		/// <param name="topology">Primitive topology.</param>
		/// <param name="window">Window to set primitive topology for. Default: Main Window.</param>
		static void SetPrimitiveTopology(PrimitiveTopology topology, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set front face for the given window.
		/// </summary>
		/// <param name="face">Front face.</param>
		/// <param name="window">Window to set front face for. Default: Main Window.</param>
		static void SetFrontFace(FrontFace face, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		/// <param name="window">Window to set shading rate for. Default: Main Window.</param>
		static void SetShadingRate(ShadingRate shadingRate,
		                           ShadingRateCombiner postRasterizerRate,
							       ShadingRateCombiner finalRate, const Window* const window = TRAP::Application::GetWindow());
		//TODO EXPERIMENTAL
		/// <summary>
		/// Set the pipeline fragment shading rate via texture.
		/// </summary>
		/// <param name="texture">
		/// Shading rate texture to use.
		/// Note: The texture must be in ResourceState::ShadingRateSource.
		/// </param>
		/// <param name="window">Window to set shading rate for. Default: Main Window.</param>
		static void SetShadingRate(Ref<Texture> texture, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set the anti aliasing method and the sample count.
		/// Use AntiAliasing::Off to disable anti aliasing.
		///
		/// Note: This will not affect the currently recorded frame.
		/// </summary>
		/// <param name="antiAliasing">Anti aliasing method to use.</param>
		/// <param name="sampleCount">Sample count to use.</param>
		static void SetAntiAliasing(AntiAliasing antiAliasing, SampleCount sampleCount);
		/// <summary>
		/// Retrieve the currently used anti aliasing method and the sample count.
		/// </summary>
		/// <param name="outAntiAliasing">Output: Used anti aliasing method.</param>
		/// <param name="outSampleCount">Output: Used sample count.</param>
		static void GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount);
		/// <summary>
		/// Retrieve the currently used anisotropy level.
		/// </summary>
		/// <returns>Used anisotropy level.</returns>
		static SampleCount GetAnisotropyLevel();
		/// <summary>
		/// Set the anisotropy level.
		/// A value of SampleCount::One effectively disables anisotropic filtering.
		///
		/// Note: Samplers need to be recreated in order to use the new anisotropy level.
		/// </summary>
		/// <param name="anisotropyLevel">Anisotropy level to use.</param>
		static void SetAnisotropyLevel(SampleCount anisotropyLevel);

		//Blending functions

		/// <summary>
		/// Set blend mode for the given window.
		/// </summary>
		/// <param name="modeRGB">Blend mode for RGB.</param>
		/// <param name="modeAlpha">Blend mode for alpha.</param>
		/// <param name="window">Window to set blend mode for. Default: Main Window.</param>
		static void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set combined blend constant for the given window.
		/// </summary>
		/// <param name="sourceRGBA">Blend constant for source RGBA.</param>
		/// <param name="destinationRGBA">Blend constant for destination RGBA.</param>
		/// <param name="window">Window to set blend constant for. Default: Main Window.</param>
		static void SetBlendConstant(BlendConstant sourceRGBA, BlendConstant destinationRGBA,
									 const Window* const window = TRAP::Application::GetWindow());
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
									 const Window* const window = TRAP::Application::GetWindow());

		//Viewport/Scissor functions

		/// <summary>
		/// Set viewport size for the given window.
		/// </summary>
		/// <param name="x">Upper left corner. Default: 0.</param>
		/// <param name="y">Upper left corner. Default: 0.</param>
		/// <param name="width">Viewport width.</param>
		/// <param name="height">Viewport height.</param>
		/// <param name="window">Window to set viewport size for. Default: Main Window.</param>
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Set scissor size for the given window.
		/// </summary>
		/// <param name="x">Upper left corner. Default: 0.</param>
		/// <param name="y">Upper left corner. Default: 0.</param>
		/// <param name="width">Scissor width.</param>
		/// <param name="height">Scissor height.</param>
		/// <param name="window">Window to set scissor size for. Default: Main Window.</param>
		static void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Window* const window = TRAP::Application::GetWindow());
#ifdef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set RenderTarget resolution for the given window.
		/// Note: Only usable in Headless mode.
		/// </summary>
		/// <param name="width">Width.</param>
		/// <param name="height">Height.</param>
		/// <param name="window">Window to set resolution for. Default: Main Window.</param>
		static void SetResolution(uint32_t width, uint32_t height, const Window* const window = TRAP::Application::GetWindow());
#endif

		//Drawing functions

		/// <summary>
		/// Draw non-indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void Draw(uint32_t vertexCount, uint32_t firstVertex = 0, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Draw indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void DrawIndexed(uint32_t indexCount, uint32_t firstIndex = 0, uint32_t firstVertex = 0,
		                        const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Draw non-indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw.</param>
		/// <param name="firstInstance">Index of the first instance to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex = 0,
		                          uint32_t firstInstance = 0, const Window* const window = TRAP::Application::GetWindow());
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
										 const Window* const window = TRAP::Application::GetWindow());

		//Compute functions

		/// <summary>
		/// Dispatch compute work on the given window.
		/// </summary>
		/// <param name="workGroupElementSizes">
		/// Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// </param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void Dispatch(const std::array<uint32_t, 3>& workGroupElementSizes, const Window* const window = TRAP::Application::GetWindow());

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
		                             QueueType queueType = QueueType::Graphics, const Window* const window = TRAP::Application::GetWindow());

		// static void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		//                              const TRAP::Ref<Graphics::RenderTarget>& depthStencil = nullptr,
		// 							 const RendererAPI::LoadActionsDesc* loadActions = nullptr,
		// 							 std::vector<uint32_t>* colorArraySlices = nullptr,
		// 							 std::vector<uint32_t>* colorMipSlices = nullptr,
		// 							 uint32_t depthArraySlice = -1, uint32_t depthMipSlice = -1,
		// 							 const Window* const window = TRAP::Application::GetWindow());
		// static void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		//                               const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
		// 							  const RendererAPI::LoadActionsDesc* loadActions = nullptr,
		// 							  std::vector<uint32_t>* colorArraySlices = nullptr,
		// 							  std::vector<uint32_t>* colorMipSlices = nullptr,
		// 							  uint32_t depthArraySlice = -1, uint32_t depthMipSlice = -1,
		// 							  const Window* const window = TRAP::Application::GetWindow());

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
									 const Window* const window = TRAP::Application::GetWindow());
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
									  const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Start a new render pass for the given window.
		///
		/// Note: This will bind the render target for the current frame again.
		/// </summary>
		/// <param name="window">Window to start render pass for. Default: Main Window.</param>
		static void StartRenderPass(const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Stop the running render pass for the given window.
		/// </summary>
		/// <param name="window">Window to stop render pass for. Default: Main Window.</param>
		static void StopRenderPass(const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Buffer barrier used to synchronize and transition the buffer.
		/// </summary>
		/// <param name="bufferBarrier">Buffer barrier to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition buffer for. Default: Main Window.</param>
		static void BufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
		                          QueueType queueType = QueueType::Graphics, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Buffer barrier used to synchronize and transition multiple buffers.
		/// </summary>
		/// <param name="bufferBarriers">Buffer barriers to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition buffers for. Default: Main Window.</param>
		static void BufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
								   QueueType queueType = QueueType::Graphics, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Texture barrier used to synchronize and transition the texture.
		/// </summary>
		/// <param name="textureBarrier">Texture barrier to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition texture for. Default: Main Window.</param>
		static void TextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
		                           QueueType queueType = QueueType::Graphics, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Texture barrier used to synchronize and transition multiple textures.
		/// </summary>
		/// <param name="textureBarriers">Texture barriers to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition textures for. Default: Main Window.</param>
		static void TextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
		                            QueueType queueType = QueueType::Graphics, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// RenderTarget barrier used to synchronize and transition the RenderTarget.
		/// </summary>
		/// <param name="renderTargetBarrier">RenderTarget barrier to use.</param>
		/// <param name="window">Window to sync and transition RenderTarget for. Default: Main Window.</param>
		static void RenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// RenderTarget barrier used to synchronize and transition multiple RenderTargets.
		/// </summary>
		/// <param name="renderTargetBarriers">RenderTarget barriers to use.</param>
		/// <param name="window">Window to sync and transition RenderTargets for. Default: Main Window.</param>
		static void RenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers, const Window* const window = TRAP::Application::GetWindow());

		//Utility

		/// <summary>
		/// Take a screenshot of the RenderTarget from the given window.
		/// </summary>
		/// <param name="window">Window to take screenshot from. Default: Main Window.</param>
		/// <returns>Captured screenshot.</returns>
		static TRAP::Scope<TRAP::Image> CaptureScreenshot(const Window* const window = TRAP::Application::GetWindow());

		/// <summary>
		/// Transition a texture from old layout to the new layout.
		/// The transition happens immediately and is guaranteed to be complete when the function returns.
		/// </summary>
		/// <param name="texture">Texture to transition layout.</param>
		/// <param name="oldLayout">Current resource state of the given texture.</param>
		/// <param name="newLayout">New resource state for the given texture.</param>
		/// <param name="queueType">Queue type on which to perform the transition. Default: Graphics.</param>
		static void Transition(Ref<Texture> texture, RendererAPI::ResourceState oldLayout,
		                       RendererAPI::ResourceState newLayout,
		                       QueueType queueType = QueueType::Graphics);

		/// <summary>
		/// Resolve an MSAA render target to a non MSAA render target.
		/// Needed to transfer MSAA rendered image data to a presentable non-MSAA target.
		///
		/// Note: source and destination must be in ResourceState::RenderTarget.
		/// </summary>
		/// <param name="source">Source MSAA render target to resolve.</param>
		/// <param name="destination">Destination non MSAA render target to resolve into.</param>
		/// <param name="window">Window to do the resolve pass on.</param>
		static void MSAAResolvePass(TRAP::Ref<RenderTarget> source, TRAP::Ref<RenderTarget> destination,
		                            const Window* const window = TRAP::Application::GetWindow());

		/// <summary>
		/// Retrieve the CPU side frames per second.
		/// </summary>
		/// <returns>CPU frames per second.</returns>
		static uint32_t GetCPUFPS();
		/// <summary>
		/// Retrieve the GPU side frames per second.
		/// </summary>
		/// <param name="window">Window to get frames per second from.</param>
		/// <returns>GPU frames per second.</returns>
		static uint32_t GetGPUFPS(const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Retrieve the CPU side frame time.
		/// </summary>
		/// <returns>CPU frame time in milliseconds.</returns>
		static float GetCPUFrameTime();
		/// <summary>
		/// Retrieve the GPU side frame time for the graphics queue.
		/// </summary>
		/// <param name="window">Window to get frame time from.</param>
		/// <returns>GPU Graphics frame time in milliseconds.</returns>
		static float GetGPUGraphicsFrameTime(const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Retrieve the GPU side frame time for the compute queue.
		/// </summary>
		/// <param name="window">Window to get frame time from.</param>
		/// <returns>GPU Compute frame time in milliseconds.</returns>
		static float GetGPUComputeFrameTime(const Window* const window = TRAP::Application::GetWindow());

		/// <summary>
		/// Retrieve the name of the GPU that is currently used by the RendererAPI.
		/// </summary>
		/// <returns>GPU name.</returns>
		static std::string GetGPUName();

		/// <summary>
		/// Set the latency mode.
		/// Note: Only LatencyMode::Disabled is supported everywhere.
		///       Other LatencyModes are only available on Windows 10 or
		///       newer with NVIDIA hardware.
		/// </summary>
		/// <param name="mode">LatencyMode to set.</param>
		/// <param name="window">Window to set latency mode for.</param>
		/// <returns>True on success, false otherwise.</returns>
		static void SetLatencyMode(LatencyMode mode, const Window* const window = TRAP::Application::GetWindow());
		/// <summary>
		/// Retrieve the currently used latency mode.
		/// Note: This may differ from the requested mode set with SetLatencyMode().
		/// </summary>
		/// <param name="window">Window to retrieve latency mode for.</param>
		/// <returns>Used latency mode.</returns>
		static LatencyMode GetLatencyMode(const Window* const window = TRAP::Application::GetWindow());
	};
}

#endif /*TRAP_RENDERCOMMAND_H*/
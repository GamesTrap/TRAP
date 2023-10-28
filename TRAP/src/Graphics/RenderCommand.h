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
#ifndef TRAP_HEADLESS_MODE
	/// <summary>
	/// Different latency modes.
	/// </summary>
	using LatencyMode = RendererAPI::LatencyMode;
#endif /*TRAP_HEADLESS_MODE*/
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
#ifndef TRAP_HEADLESS_MODE
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
		static void Flush(const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Flush renderer.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Starts graphics and compute recording for the next frame.
		/// </summary>
		static void Flush();
#endif /*TRAP_HEADLESS_MODE*/

		//VSync functions

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set VSync for the given window.
		/// </summary>
		/// <param name="vsync">Enable or disable VSync.</param>
		/// <param name="window">Window to set VSync for. Default: Main Window.</param>
		static void SetVSync(bool vsync, const Window* window = TRAP::Application::GetWindow());
		/// <summary>
		/// Retrieve VSync state of the given window.
		/// </summary>
		/// <param name="window">Window to get VSync state from. Default: Main Window.</param>
		/// <returns>True if VSync is enabled, false otherwise.</returns>
		[[nodiscard]] static bool GetVSync(const Window* window = TRAP::Application::GetWindow());
#endif /*TRAP_HEADLESS_MODE*/

		//Clear functions

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Clear the RenderTarget of the given window.
		/// </summary>
		/// <param name="clearType">Type of buffer to clear.</param>
		/// <param name="window">Window to clear RenderTarget for. Default: Main Window.</param>
		static void Clear(ClearBuffer clearType, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Clear the given render target.
		/// </summary>
		/// <param name="clearType">Type of buffer to clear.</param>
		static void Clear(ClearBuffer clearType);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the clear color for the given window.
		/// </summary>
		/// <param name="color">New clear color. Default: Very dark gray.</param>
		/// <param name="window">Window to set clear color for. Default: Main Window.</param>
		static void SetClearColor(const RendererAPI::Color& color = { 0.1, 0.1, 0.1, 1.0 }, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the clear color.
		/// </summary>
		/// <param name="color">New clear color.</param>
		static void SetClearColor(const RendererAPI::Color& color = { 0.1, 0.1, 0.1, 1.0 });
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the clear depth value for the given window.
		/// </summary>
		/// <param name="depth">New clear depth value (range [0,1]). Default: 1.</param>
		/// <param name="window">Window to set clear depth value for. Default: Main Window.</param>
		static void SetClearDepth(f32 depth = 0.0f, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the clear depth value.
		/// </summary>
		/// <param name="depth">New clear depth value. Must be between 0.0f and 1.0f</param>
		static void SetClearDepth(f32 depth = 0.0f);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the clear stencil value for the given window.
		/// </summary>
		/// <param name="stencil">New clear stencil value. Default: 0.</param>
		/// <param name="window">Window to set clear stencil value for. Default: Main Window.</param>
		static void SetClearStencil(u32 stencil = 0, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the clear stencil value.
		/// </summary>
		/// <param name="stencil">New clear stencil value.</param>
		static void SetClearStencil(u32 stencil = 0);
#endif /*TRAP_HEADLESS_MODE*/

		//Depth functions

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Enable or disable depth testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth testing.</param>
		/// <param name="window">Window to set depth testing for. Default: Main Window.</param>
		static void SetDepthTesting(bool enabled, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Enable or disable depth testing.
		/// </summary>
		/// <param name="enabled">Enable or disable depth testing.</param>
		static void SetDepthTesting(bool enabled);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Enable or disable depth writing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth writing.</param>
		/// <param name="window">Window to set depth writing for. Default: Main Window.</param>
		static void SetDepthWriting(bool enabled, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Enable or disable depth writing.
		/// </summary>
		/// <param name="enabled">Enable or disable depth writing.</param>
		static void SetDepthWriting(bool enabled);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set depth function for the given window.
		/// </summary>
		/// <param name="compareMode">Depth compare mode. Default: CompareMode::Less.</param>
		/// <param name="window">Window to set depth function for. Default: Main Window.</param>
		static void SetDepthFunction(CompareMode compareMode = CompareMode::Less, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the depth function.
		/// </summary>
		/// <param name="compareMode">Depth compare mode. Default: CompareMode::Less.</param>
		static void SetDepthFunction(CompareMode compareMode = CompareMode::Less);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set operation for when depth testing fails for the given window.
		/// </summary>
		/// <param name="front">Depth/Stencil operation.</param>
		/// <param name="back">Depth/Stencil operation.</param>
		/// <param name="window">Window to set depth fail operation for. Default: Main Window.</param>
		static void SetDepthFail(StencilOperation front, StencilOperation back, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the depth action to perform when depth testing fails.
		/// </summary>
		/// <param name="front">Depth action to perform when depth testing fails.</param>
		/// <param name="back">Depth action to perform when depth testing fails.</param>
		static void SetDepthFail(StencilOperation front, StencilOperation back);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set depth bias constant factor for the given window.
		/// </summary>
		/// <param name="bias">Depth bias constant factor.</param>
		/// <param name="window">Window to set depth bias for. Default: Main Window.</param>
		static void SetDepthBias(i32 bias, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the depth bias (scalar factor to add to each fragments depth value).
		/// </summary>
		/// <param name="bias">Depth bias constant factor.</param>
		static void SetDepthBias(i32 bias);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set depth bias slope factor for the given window.
		/// </summary>
		/// <param name="slopeFactor">Depth bias slope factor.</param>
		/// <param name="window">Window to set depth bias for. Default: Main Window.</param>
		static void SetDepthBiasSlopeFactor(f32 slopeFactor, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation).
		/// </summary>
		/// <param name="slopeFactor">Depth bias slope factor.</param>
		static void SetDepthBiasSlopeFactor(f32 slopeFactor);
#endif /*TRAP_HEADLESS_MODE*/

		//Stencil functions

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Enable or disable stencil testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable stencil testing.</param>
		/// <param name="window">Window to set stencil testing for. Default: Main Window.</param>
		static void SetStencilTesting(bool enabled, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Enable or disable stencil testing.
		/// </summary>
		/// <param name="enabled">Enable or disable stencil testing.</param>
		static void SetStencilTesting(bool enabled);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set operation for when stencil testing fails for the given window.
		/// </summary>
		/// <param name="front">Stencil operation.</param>
		/// <param name="back">Stencil operation.</param>
		/// <param name="window">Window to set stencil fail operation for. Default: Main Window.</param>
		static void SetStencilFail(StencilOperation front, StencilOperation back, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the stencil action to perform when stencil testing fails.
		/// </summary>
		/// <param name="front">Stencil action to perform when stencil testing fails.</param>
		/// <param name="back">Stencil action to perform when stencil testing fails.</param>
		static void SetStencilFail(StencilOperation front, StencilOperation back);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set operation for when stencil testing passes for the given window.
		/// </summary>
		/// <param name="front">Stencil operation.</param>
		/// <param name="back">Stencil operation.</param>
		/// <param name="window">Window to set stencil pass operation for. Default: Main Window.</param>
		static void SetStencilPass(StencilOperation front, StencilOperation back, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set operation for when stencil testing passes.
		/// </summary>
		/// <param name="front">Stencil action to perform when passed.</param>
		/// <param name="back">Stencil action to perform when passed.</param>
		static void SetStencilPass(StencilOperation front, StencilOperation back);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set stencil function for the given window.
		/// </summary>
		/// <param name="front">Compare mode.</param>
		/// <param name="back">Compare mode.</param>
		/// <param name="window">Window to set stencil function for. Default: Main Window.</param>
		static void SetStencilFunction(CompareMode front, CompareMode back, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the stencil functions.
		/// </summary>
		/// <param name="front">Function to use on the front for stencil testing.</param>
		/// <param name="back">Function to use on the back for stencil testing.</param>
		static void SetStencilFunction(CompareMode front, CompareMode back);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set stencil mask for the given window.
		/// </summary>
		/// <param name="read">Bits to read/compare.</param>
		/// <param name="write">Bits to write/update.</param>
		/// <param name="window">Window to set stencil mask for. Default: Main Window.</param>
		static void SetStencilMask(u8 read, u8 write, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the stencil mask.
		/// </summary>
		/// <param name="read">Select the bits of the stencil values to test.</param>
		/// <param name="write">Select the bits of the stencil values updated by the stencil test.</param>
		static void SetStencilMask(u8 read, u8 write);
#endif /*TRAP_HEADLESS_MODE*/

		//Miscellaneous functions

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set cull mode for the given window.
		/// </summary>
		/// <param name="cullMode">Cull mode.</param>
		/// <param name="window">Window to set cull mode for. Default: Main Window.</param>
		static void SetCullMode(CullMode cullMode, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the cull mode.
		/// </summary>
		/// <param name="cullMode">Cull mode to use.</param>
		static void SetCullMode(CullMode cullMode);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set fill mode for the given window.
		/// </summary>
		/// <param name="fillMode">Fill mode.</param>
		/// <param name="window">Window to set fill mode for. Default: Main Window.</param>
		static void SetFillMode(FillMode fillMode, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the fill mode.
		/// </summary>
		/// <param name="fillMode">Fill mode to use.</param>
		static void SetFillMode(FillMode fillMode);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set primitive topology for the given window.
		/// </summary>
		/// <param name="topology">Primitive topology.</param>
		/// <param name="window">Window to set primitive topology for. Default: Main Window.</param>
		static void SetPrimitiveTopology(PrimitiveTopology topology, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the primitive topology.
		/// </summary>
		/// <param name="topology">Primitive topology to use.</param>
		static void SetPrimitiveTopology(PrimitiveTopology topology);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set front face for the given window.
		/// </summary>
		/// <param name="face">Front face.</param>
		/// <param name="window">Window to set front face for. Default: Main Window.</param>
		static void SetFrontFace(FrontFace face, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the front face winding order.
		/// </summary>
		/// <param name="face">Front face winding order to use.</param>
		static void SetFrontFace(FrontFace face);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		/// <param name="window">Window to set shading rate for. Default: Main Window.</param>
		static void SetShadingRate(ShadingRate shadingRate,
		                           ShadingRateCombiner postRasterizerRate,
							       ShadingRateCombiner finalRate, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		static void SetShadingRate(ShadingRate shadingRate,
		                           ShadingRateCombiner postRasterizerRate,
							       ShadingRateCombiner finalRate);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// TODO EXPERIMENTAL
		/// Set the pipeline fragment shading rate via texture.
		/// </summary>
		/// <param name="texture">
		/// Shading rate texture to use.
		/// Note: The texture must be in ResourceState::ShadingRateSource.
		/// </param>
		/// <param name="window">Window to set shading rate for. Default: Main Window.</param>
		static void SetShadingRate(Ref<RenderTarget> texture, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the pipeline fragment shading rate via texture.
		/// </summary>
		/// <param name="texture">
		/// Shading rate texture to use.
		/// Note: The texture must be in ResourceState::ShadingRateSource.
		/// </param>
		static void SetShadingRate(Ref<RenderTarget> texture);
#endif /*TRAP_HEADLESS_MODE*/
		/// <summary>
		/// Set the anti aliasing method and the sample count.
		/// Use AntiAliasing::Off to disable anti aliasing.
		///
		/// Note: This will not affect the currently recorded frame.
		/// </summary>
		/// <param name="antiAliasing">Anti aliasing method to use.</param>
		/// <param name="sampleCount">Sample count to use.</param>
		static void SetAntiAliasing(AntiAliasing antiAliasing, SampleCount sampleCount);
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the currently used anti aliasing method and the sample count.
		/// </summary>
		/// <param name="outAntiAliasing">Output: Used anti aliasing method.</param>
		/// <param name="outSampleCount">Output: Used sample count.</param>
		/// <param name="window">Window to get anti aliasing from. Default: Main Window.</param>
		static void GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount, const Window* window = TRAP::Application::GetWindow()) noexcept;
#else
		/// <summary>
		/// Retrieve the currently used anti aliasing method and the sample count.
		/// </summary>
		/// <param name="outAntiAliasing">Output: Used anti aliasing method.</param>
		/// <param name="outSampleCount">Output: Used sample count.</param>
		static void GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount) noexcept;
#endif /*TRAP_HEADLESS_MODE*/
		/// <summary>
		/// Retrieve the currently used anisotropy level.
		/// </summary>
		/// <returns>Used anisotropy level.</returns>
		[[nodiscard]] static SampleCount GetAnisotropyLevel() noexcept;
		/// <summary>
		/// Set the anisotropy level.
		/// A value of SampleCount::One effectively disables anisotropic filtering.
		///
		/// Note: Samplers need to be recreated in order to use the new anisotropy level.
		/// </summary>
		/// <param name="anisotropyLevel">Anisotropy level to use.</param>
		static void SetAnisotropyLevel(SampleCount anisotropyLevel);

		//Blending functions

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set blend mode for the given window.
		/// </summary>
		/// <param name="modeRGB">Blend mode for RGB.</param>
		/// <param name="modeAlpha">Blend mode for alpha.</param>
		/// <param name="window">Window to set blend mode for. Default: Main Window.</param>
		static void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the blend mode.
		/// </summary>
		/// <param name="modeRGB">Blend mode to use for the RGB channels.</param>
		/// <param name="modeAlpha">Blend mode to use for the alpha channel.</param>
		static void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set combined blend constant for the given window.
		/// </summary>
		/// <param name="sourceRGBA">Blend constant for source RGBA.</param>
		/// <param name="destinationRGBA">Blend constant for destination RGBA.</param>
		/// <param name="window">Window to set blend constant for. Default: Main Window.</param>
		static void SetBlendConstant(BlendConstant sourceRGBA, BlendConstant destinationRGBA,
									 const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set combined blend constant.
		/// </summary>
		/// <param name="sourceRGBA">Blend constant for source RGBA.</param>
		/// <param name="destinationRGBA">Blend constant for destination RGBA.</param>
		static void SetBlendConstant(BlendConstant sourceRGBA, BlendConstant destinationRGBA);
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
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
									 const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the blend constants/factors.
		/// </summary>
		/// <param name="sourceRGB">Specifies how the red, green, and blue blending factors are computed.</param>
		/// <param name="sourceAlpha">Specifies how the alpha source blending factor is computed.</param>
		/// <param name="destinationRGB">Specifies how the red, green, and blue destination blending factors are computed.</param>
		/// <param name="destinationAlpha">Specified how the alpha destination blending factor is computed.</param>
		static void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                         BlendConstant destinationRGB, BlendConstant destinationAlpha);
#endif /*TRAP_HEADLESS_MODE*/

		//Viewport/Scissor functions

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set viewport size for the given window.
		/// </summary>
		/// <param name="x">Upper left corner. Default: 0.</param>
		/// <param name="y">Upper left corner. Default: 0.</param>
		/// <param name="width">Viewport width.</param>
		/// <param name="height">Viewport height.</param>
		/// <param name="window">Window to set viewport size for. Default: Main Window.</param>
		static void SetViewport(u32 x, u32 y, u32 width, u32 height, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set viewport size.
		/// </summary>
		/// <param name="x">X coordinate of the top left corner of the viewport. Default: 0.</param>
		/// <param name="y">Y coordinate of the top left corner of the viewport. Default: 0.</param>
		/// <param name="width">Viewport width.</param>
		/// <param name="height">Viewport height.</param>
		static void SetViewport(u32 x, u32 y, u32 width, u32 height);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set scissor size for the given window.
		/// </summary>
		/// <param name="x">Upper left corner. Default: 0.</param>
		/// <param name="y">Upper left corner. Default: 0.</param>
		/// <param name="width">Scissor width.</param>
		/// <param name="height">Scissor height.</param>
		/// <param name="window">Window to set scissor size for. Default: Main Window.</param>
		static void SetScissor(u32 x, u32 y, u32 width, u32 height, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set scissor size.
		/// </summary>
		/// <param name="x">Upper left corner. Default: 0.</param>
		/// <param name="y">Upper left corner. Default: 0.</param>
		/// <param name="width">Scissor width.</param>
		/// <param name="height">Scissor height.</param>
		static void SetScissor(u32 x, u32 y, u32 width, u32 height);
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set RenderTarget resolution.
		/// </summary>
		/// <param name="width">Width.</param>
		/// <param name="height">Height.</param>
		static void SetResolution(u32 width, u32 height);
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// <summary>
		/// Get RenderTarget resolution.
		/// </summary>
		/// <param name="width">Output: Width.</param>
		/// <param name="height">Output: Height.</param>
		static void GetResolution(u32& width, u32& height);
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// TODO EXPERIMENTAL
		/// Set the render scale for the given window.
		/// Note: This functon takes effect on the next frame.
		/// </summary>
		/// <param name="scale">Render scale value (valid range: 0.5f-1.0f inclusive).</param>
		/// <param name="window">Window to set render scale for. Default: Main Window.</param>
		static void SetRenderScale(f32 scale, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set the render scale.
		/// Note: This functon takes effect on the next frame.
		/// </summary>
		/// <param name="scale">Render scale value (valid range: 0.5f-1.0f inclusive).</param>
		static void SetRenderScale(f32 scale);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// TODO EXPERIMENTAL
		/// Retrieve the used render scale value of the given window.
		/// </summary>
		/// <param name="window">Window to retrieve render scale from. Default: Main Window.</param>
		/// <returns>Render scale (between 0.5f and 2.0f inclusive).</returns>
		[[nodiscard]] static f32 GetRenderScale(const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Retrieve the used render scale value.
		/// </summary>
		/// <returns>Render scale (between 0.5f and 2.0f inclusive).</returns>
		[[nodiscard]] static f32 GetRenderScale();
#endif /*TRAP_HEADLESS_MODE*/

		//Drawing functions

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw non-indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void Draw(u32 vertexCount, u32 firstVertex = 0, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Draw non-indexed, non-instanced geometry.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		static void Draw(u32 vertexCount, u32 firstVertex = 0);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw.</param>
		/// <param name="vertexOffset">Index of the first vertex to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void DrawIndexed(u32 indexCount, u32 firstIndex = 0, i32 vertexOffset = 0,
		                        const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Draw indexed, non-instanced geometry.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="vertexOffset">Index of the first vertex to draw. Default: 0.</param>
		static void DrawIndexed(u32 indexCount, u32 firstIndex = 0, i32 vertexOffset = 0);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw non-indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw.</param>
		/// <param name="firstInstance">Index of the first instance to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void DrawInstanced(u32 vertexCount, u32 instanceCount, u32 firstVertex = 0,
		                          u32 firstInstance = 0, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Draw non-indexed, instanced geometry.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		static void DrawInstanced(u32 vertexCount, u32 instanceCount, u32 firstVertex = 0,
		                          u32 firstInstance = 0);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw.</param>
		/// <param name="firstInstance">Index of the first instance to draw.</param>
		/// <param name="vertexOffset">Index of the first vertex to draw.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void DrawIndexedInstanced(u32 indexCount, u32 instanceCount, u32 firstIndex = 0,
		                                 u32 firstInstance = 0, i32 vertexOffset = 0,
										 const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Draw indexed, instanced geometry.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		/// <param name="vertexOffset">Index of the first vertex to draw. Default: 0.</param>
		static void DrawIndexedInstanced(u32 indexCount, u32 instanceCount,
		                                 u32 firstIndex = 0, u32 firstInstance = 0,
										 i32 vertexOffset = 0);
#endif /*TRAP_HEADLESS_MODE*/

		//Compute functions

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Dispatch compute work on the given window.
		/// </summary>
		/// <param name="workGroupElementSizes">
		/// Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// </param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		static void Dispatch(const std::array<u32, 3>& workGroupElementSizes, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Dispatch.
		/// </summary>
		/// <param name="workGroupElements">
		/// Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// </param>
		static void Dispatch(const std::array<u32, 3>& workGroupElements);
#endif /*TRAP_HEADLESS_MODE*/

		//TODO DispatchIndirect

		//CommandBuffer functions

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set push constant for the given window.
		/// Note: Minimum guaranteed size is 128 bytes.
		/// </summary>
		/// <param name="name">Name of the push constant.</param>
		/// <param name="data">Data to set push constant to.</param>
		/// <param name="length">Length in bytes of the provided data to set push constant to.</param>
		/// <param name="queueType">Queue type on which to perform the operation. Default: Graphics.</param>
		/// <param name="window">Window to set push constant for. Default: Main Window.</param>
		static void SetPushConstants(std::string_view name, const void* data, usize length,
		                             QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set push constant.
		/// Note: Minimum guaranteed size is 128 bytes.
		/// </summary>
		/// <param name="name">Name of the push constant.</param>
		/// <param name="data">Data to set push constant to.</param>
		/// <param name="length">Length in bytes of the provided data to set push constant to.</param>
		/// <param name="queueType">Queue type on which to perform the operation. Default: Graphics.</param>
		static void SetPushConstants(std::string_view name, const void* data, usize length,
		                             QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
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
									 const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set render target.
		/// </summary>
		/// <param name="colorTarget">Color target to set.</param>
		/// <param name="depthStencil">Depth stencil target to set.</param>
		/// <param name="loadActions">Load actions.</param>
		static void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil = nullptr,
									 const RendererAPI::LoadActionsDesc* loadActions = nullptr);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
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
									  const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Set multiple render targets.
		/// </summary>
		/// <param name="colorTargets">Color targets to set.</param>
		/// <param name="depthStencil">Depth stencil target to set.</param>
		/// <param name="loadActions">Load actions.</param>
		static void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                              const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
									  const RendererAPI::LoadActionsDesc* loadActions = nullptr);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Start a new render pass for the given window.
		///
		/// Note: This will bind the render target for the current frame again.
		/// </summary>
		/// <param name="window">Window to start render pass for. Default: Main Window.</param>
		static void StartRenderPass(const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Start a render pass.
		///
		/// Note: This will bind the render target for the current frame again.
		/// </summary>
		static void StartRenderPass();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Stop the running render pass for the given window.
		/// </summary>
		/// <param name="window">Window to stop render pass for. Default: Main Window.</param>
		static void StopRenderPass(const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Stop running render pass.
		/// </summary>
		static void StopRenderPass();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Buffer barrier used to synchronize and transition the buffer.
		/// </summary>
		/// <param name="bufferBarrier">Buffer barrier to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition buffer for. Default: Main Window.</param>
		static void BufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
		                          QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Buffer barrier used to synchronize and transition the buffer.
		/// </summary>
		/// <param name="bufferBarrier">Buffer barrier to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		static void BufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
		                          QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Buffer barrier used to synchronize and transition multiple buffers.
		/// </summary>
		/// <param name="bufferBarriers">Buffer barriers to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition buffers for. Default: Main Window.</param>
		static void BufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
								   QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Buffer barrier used to synchronize and transition multiple buffers.
		/// </summary>
		/// <param name="bufferBarriers">Buffer barriers to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		static void BufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
								   QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Texture barrier used to synchronize and transition the texture.
		/// </summary>
		/// <param name="textureBarrier">Texture barrier to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition texture for. Default: Main Window.</param>
		static void TextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
		                           QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Texture barrier used to synchronize and transition the texture.
		/// </summary>
		/// <param name="textureBarrier">Texture barrier to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		static void TextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
		                           QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Texture barrier used to synchronize and transition multiple textures.
		/// </summary>
		/// <param name="textureBarriers">Texture barriers to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to sync and transition textures for. Default: Main Window.</param>
		static void TextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
		                            QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Texture barrier used to synchronize and transition multiple textures.
		/// </summary>
		/// <param name="textureBarriers">Texture barriers to use.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		static void TextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
		                            QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// RenderTarget barrier used to synchronize and transition the RenderTarget.
		/// </summary>
		/// <param name="renderTargetBarrier">RenderTarget barrier to use.</param>
		/// <param name="window">Window to sync and transition RenderTarget for. Default: Main Window.</param>
		static void RenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// RenderTarget barrier used to synchronize and transition the RenderTarget.
		/// </summary>
		/// <param name="renderTargetBarrier">RenderTarget barrier to use.</param>
		static void RenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// RenderTarget barrier used to synchronize and transition multiple RenderTargets.
		/// </summary>
		/// <param name="renderTargetBarriers">RenderTarget barriers to use.</param>
		/// <param name="window">Window to sync and transition RenderTargets for. Default: Main Window.</param>
		static void RenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers, const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// RenderTarget barrier used to synchronize and transition multiple RenderTargets.
		/// </summary>
		/// <param name="renderTargetBarriers">RenderTarget barriers to use.</param>
		static void RenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers);
#endif /*TRAP_HEADLESS_MODE*/

		//Utility

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Capture a screenshot of the last presented frame.
		/// </summary>
		/// <param name="window">Window to take screenshot from. Default: Main Window.</param>
		/// <returns>Captured screenshot as TRAP::Image on success, nullptr otherwise.</returns>
		[[nodiscard]] static TRAP::Scope<TRAP::Image> CaptureScreenshot(const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Capture a screenshot of the last presented frame.
		/// </summary>
		/// <returns>Captured screenshot as TRAP::Image on success, nullptr otherwise.</returns>
		[[nodiscard]] static TRAP::Scope<TRAP::Image> CaptureScreenshot();
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Transition a texture from old layout to the new layout.
		/// The transition happens immediately and is guaranteed to be complete when the function returns.
		/// </summary>
		/// <param name="texture">Texture to transition layout.</param>
		/// <param name="oldLayout">Current resource state of the given texture.</param>
		/// <param name="newLayout">New resource state for the given texture.</param>
		/// <param name="queueType">Queue type on which to perform the transition. Default: Graphics.</param>
		static void Transition(const Ref<Texture>& texture, RendererAPI::ResourceState oldLayout,
		                       RendererAPI::ResourceState newLayout,
		                       QueueType queueType = QueueType::Graphics);

#ifndef TRAP_HEADLESS_MODE
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
		                            const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Resolve an MSAA render target to a non MSAA render target.
		/// Needed to transfer MSAA rendered image data to a presentable non-MSAA target.
		///
		/// Note: source and destination must be in ResourceState::RenderTarget.
		/// </summary>
		/// <param name="source">Source MSAA render target to resolve.</param>
		/// <param name="destination">Destination non MSAA render target to resolve into.</param>
		static void MSAAResolvePass(TRAP::Ref<RenderTarget> source, TRAP::Ref<RenderTarget> destination);
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Retrieve the CPU side frames per second.
		/// </summary>
		/// <returns>CPU frames per second.</returns>
		[[nodiscard]] static u32 GetCPUFPS();
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the GPU side frames per second.
		/// </summary>
		/// <param name="window">Window to get frames per second from.</param>
		/// <returns>GPU frames per second.</returns>
		[[nodiscard]] static u32 GetGPUFPS(const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Retrieve the GPU side frames per second.
		/// </summary>
		/// <returns>GPU frames per second.</returns>
		[[nodiscard]] static u32 GetGPUFPS();
#endif /*TRAP_HEADLESS_MODE*/
		/// <summary>
		/// Retrieve the CPU side frame time.
		/// </summary>
		/// <returns>CPU frame time in milliseconds.</returns>
		[[nodiscard]] static f32 GetCPUFrameTime();
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the GPU side frame time for the graphics queue.
		/// </summary>
		/// <param name="window">Window to get frame time from.</param>
		/// <returns>GPU Graphics frame time in milliseconds.</returns>
		[[nodiscard]] static f32 GetGPUGraphicsFrameTime(const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Retrieve the GPU side frame time for the graphics queue.
		/// </summary>
		/// <returns>GPU Graphics frame time in milliseconds.</returns>
		[[nodiscard]] static f32 GetGPUGraphicsFrameTime();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the GPU side frame time for the compute queue.
		/// </summary>
		/// <param name="window">Window to get frame time from.</param>
		/// <returns>GPU Compute frame time in milliseconds.</returns>
		[[nodiscard]] static f32 GetGPUComputeFrameTime(const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Retrieve the GPU side frame time for the compute queue.
		/// </summary>
		/// <returns>GPU Compute frame time in milliseconds.</returns>
		[[nodiscard]] static f32 GetGPUComputeFrameTime();
#endif /*TRAP_HEADLESS_MODE*/
		/// <summary>
		/// Retrieve the name of the GPU that is currently used by the RendererAPI.
		/// </summary>
		/// <returns>GPU name.</returns>
		[[nodiscard]] static std::string GetGPUName();

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
		static void SetLatencyMode(LatencyMode mode, const Window* window = TRAP::Application::GetWindow());
		/// <summary>
		/// Retrieve the currently used latency mode.
		/// Note: This may differ from the requested mode set with SetLatencyMode().
		/// </summary>
		/// <param name="window">Window to retrieve latency mode for.</param>
		/// <returns>Used latency mode.</returns>
		[[nodiscard]] static LatencyMode GetLatencyMode(const Window* window = TRAP::Application::GetWindow());
#endif /*TRAP_HEADLESS_MODE*/
	};
}

#endif /*TRAP_RENDERCOMMAND_H*/
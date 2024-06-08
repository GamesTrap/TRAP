#ifndef TRAP_RENDERCOMMAND_H
#define TRAP_RENDERCOMMAND_H

#include "Application.h"

namespace TRAP::Graphics
{
	/// @brief Different buffers for clearing.
	using ClearBuffer = RendererAPI::ClearBufferType;
	/// @brief Different compare modes for depth and stencil testing.
	using CompareMode = RendererAPI::CompareMode;
	/// @brief Different stencil operation options.
	using StencilOperation = RendererAPI::StencilOp;
	/// @brief Different cull modes.
	using CullMode = RendererAPI::CullMode;
	/// @brief Different fill modes.
	using FillMode = RendererAPI::FillMode;
	/// @brief Different primitive topologies.
	using PrimitiveTopology = RendererAPI::PrimitiveTopology;
	/// @brief Different front face options.
	using FrontFace = RendererAPI::FrontFace;
	/// @brief Different blend modes.
	using BlendMode = RendererAPI::BlendMode;
	/// @brief Different blend constants.
	using BlendConstant = RendererAPI::BlendConstant;
	/// @brief Different shading rates.
	using ShadingRate = RendererAPI::ShadingRate;
	/// @brief Different shading rate combiners.
	using ShadingRateCombiner = RendererAPI::ShadingRateCombiner;
	/// @brief Different queue types.
	using QueueType = RendererAPI::QueueType;
	/// @brief Different resource states.
	using ResourceState = RendererAPI::ResourceState;
	/// @brief Different anti aliasing methods.
	using AntiAliasing = RendererAPI::AntiAliasing;
	/// @brief Different sample counts for anti aliasing.
	using SampleCount = RendererAPI::SampleCount;
#ifndef TRAP_HEADLESS_MODE
	/// @brief Different latency modes.
	/// @remark @headless This is not available in headless mode.
	using LatencyMode = RendererAPI::LatencyMode;
#endif /*TRAP_HEADLESS_MODE*/
	/// @brief Different texture filtering modes.
	using FilterType = TRAP::Graphics::RendererAPI::FilterType;
	/// @brief Different texture addressing modes.
	using AddressMode = TRAP::Graphics::RendererAPI::AddressMode;
	/// @brief Different texture mip mapping modes.
	using MipMapMode = TRAP::Graphics::RendererAPI::MipMapMode;
	/// @brief Different update frequencies.
	using UpdateFrequency = RendererAPI::DescriptorUpdateFrequency;

	/// @brief Utility class for high level rendering commands.
	/// User with knowledge about OpenGL should be familiar with most of these commands.
	/// @note Most of these functions are just shortcuts for RendererAPI::* functions.
	namespace RenderCommand
	{
#ifndef TRAP_HEADLESS_MODE
		/// @brief Flush renderer for the given window.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Presents the rendered image to the screen.
		/// 4. Starts graphics and compute recording for the next frame.
		/// @param window Window to flush renderer. Default: Main Window.
		/// @note TRAP::Application automatically flushes for the main window.
		/// @remark @headless This function is not available in headless mode.
		void Flush(const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Flush renderer.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Starts graphics and compute recording for the next frame.
		/// @remark This function is only available in headless mode.
		void Flush();
#endif /*TRAP_HEADLESS_MODE*/

		//VSync functions

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set VSync for the given window.
		/// @param vsync Enable or disable VSync.
		/// @param window Window to set VSync for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetVSync(bool vsync, const Window* window = TRAP::Application::GetWindow());
		/// @brief Retrieve VSync state of the given window.
		/// @param window Window to get VSync state from. Default: Main Window.
		/// @return True if VSync is enabled, false otherwise.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] bool GetVSync(const Window* window = TRAP::Application::GetWindow());
#endif /*TRAP_HEADLESS_MODE*/

		//Clear functions

#ifndef TRAP_HEADLESS_MODE
		/// @brief Clear the RenderTarget of the given window.
		/// @param clearType Type of buffer to clear.
		/// @param window Window to clear RenderTarget for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void Clear(ClearBuffer clearType, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Clear the given render target.
		/// @param clearType Type of buffer to clear.
		/// @remark This function is only available in headless mode.
		void Clear(ClearBuffer clearType);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the clear color for the given window.
		/// @param color New clear color. Default: Very dark gray.
		/// @param window Window to set clear color for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetClearColor(const RendererAPI::Color& color = { 0.1, 0.1, 0.1, 1.0 }, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the clear color.
		/// @param color New clear color.
		/// @remark This function is only available in headless mode.
		void SetClearColor(const RendererAPI::Color& color = { 0.1, 0.1, 0.1, 1.0 });
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the clear depth value for the given window.
		/// @param depth New clear depth value (range [0,1]). Default: 1.
		/// @param window Window to set clear depth value for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetClearDepth(f32 depth = 0.0f, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the clear depth value.
		/// @param depth New clear depth value. Must be between 0.0f and 1.0f
		/// @remark This function is only available in headless mode.
		void SetClearDepth(f32 depth = 0.0f);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the clear stencil value for the given window.
		/// @param stencil New clear stencil value. Default: 0.
		/// @param window Window to set clear stencil value for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetClearStencil(u32 stencil = 0, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the clear stencil value.
		/// @param stencil New clear stencil value.
		/// @remark This function is only available in headless mode.
		void SetClearStencil(u32 stencil = 0);
#endif /*TRAP_HEADLESS_MODE*/

		//Depth functions

#ifndef TRAP_HEADLESS_MODE
		/// @brief Enable or disable depth testing for the given window.
		/// @param enabled Enable or disable depth testing.
		/// @param window Window to set depth testing for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthTesting(bool enabled, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Enable or disable depth testing.
		/// @param enabled Enable or disable depth testing.
		/// @remark This function is only available in headless mode.
		void SetDepthTesting(bool enabled);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Enable or disable depth writing for the given window.
		/// @param enabled Enable or disable depth writing.
		/// @param window Window to set depth writing for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthWriting(bool enabled, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Enable or disable depth writing.
		/// @param enabled Enable or disable depth writing.
		/// @remark This function is only available in headless mode.
		void SetDepthWriting(bool enabled);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set depth function for the given window.
		/// @param compareMode Depth compare mode. Default: CompareMode::Less.
		/// @param window Window to set depth function for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthFunction(CompareMode compareMode = CompareMode::Less, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the depth function.
		/// @param compareMode Depth compare mode. Default: CompareMode::Less.
		/// @remark This function is only available in headless mode.
		void SetDepthFunction(CompareMode compareMode = CompareMode::Less);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set operation for when depth testing fails for the given window.
		/// @param front Depth/Stencil operation.
		/// @param back Depth/Stencil operation.
		/// @param window Window to set depth fail operation for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthFail(StencilOperation front, StencilOperation back, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the depth action to perform when depth testing fails.
		/// @param front Depth action to perform when depth testing fails.
		/// @param back Depth action to perform when depth testing fails.
		/// @remark This function is only available in headless mode.
		void SetDepthFail(StencilOperation front, StencilOperation back);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set depth bias constant factor for the given window.
		/// @param bias Depth bias constant factor.
		/// @param window Window to set depth bias for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthBias(i32 bias, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the depth bias (scalar factor to add to each fragments depth value).
		/// @param bias Depth bias constant factor.
		/// @remark This function is only available in headless mode.
		void SetDepthBias(i32 bias);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set depth bias slope factor for the given window.
		/// @param slopeFactor Depth bias slope factor.
		/// @param window Window to set depth bias for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthBiasSlopeFactor(f32 slopeFactor, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation).
		/// @param slopeFactor Depth bias slope factor.
		/// @remark This function is only available in headless mode.
		void SetDepthBiasSlopeFactor(f32 slopeFactor);
#endif /*TRAP_HEADLESS_MODE*/

		//Stencil functions

#ifndef TRAP_HEADLESS_MODE
		/// @brief Enable or disable stencil testing for the given window.
		/// @param enabled Enable or disable stencil testing.
		/// @param window Window to set stencil testing for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetStencilTesting(bool enabled, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Enable or disable stencil testing.
		/// @param enabled Enable or disable stencil testing.
		/// @remark This function is only available in headless mode.
		void SetStencilTesting(bool enabled);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set operation for when stencil testing fails for the given window.
		/// @param front Stencil operation.
		/// @param back Stencil operation.
		/// @param window Window to set stencil fail operation for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetStencilFail(StencilOperation front, StencilOperation back, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the stencil action to perform when stencil testing fails.
		/// @param front Stencil action to perform when stencil testing fails.
		/// @param back Stencil action to perform when stencil testing fails.
		/// @remark This function is only available in headless mode.
		void SetStencilFail(StencilOperation front, StencilOperation back);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set operation for when stencil testing passes for the given window.
		/// @param front Stencil operation.
		/// @param back Stencil operation.
		/// @param window Window to set stencil pass operation for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetStencilPass(StencilOperation front, StencilOperation back, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set operation for when stencil testing passes.
		/// @param front Stencil action to perform when passed.
		/// @param back Stencil action to perform when passed.
		/// @remark This function is only available in headless mode.
		void SetStencilPass(StencilOperation front, StencilOperation back);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set stencil function for the given window.
		/// @param front Compare mode.
		/// @param back Compare mode.
		/// @param window Window to set stencil function for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetStencilFunction(CompareMode front, CompareMode back, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the stencil functions.
		/// @param front Function to use on the front for stencil testing.
		/// @param back Function to use on the back for stencil testing.
		/// @remark This function is only available in headless mode.
		void SetStencilFunction(CompareMode front, CompareMode back);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set stencil mask for the given window.
		/// @param read Bits to read/compare.
		/// @param write Bits to write/update.
		/// @param window Window to set stencil mask for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetStencilMask(u8 read, u8 write, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the stencil mask.
		/// @param read Select the bits of the stencil values to test.
		/// @param write Select the bits of the stencil values updated by the stencil test.
		/// @remark This function is only available in headless mode.
		void SetStencilMask(u8 read, u8 write);
#endif /*TRAP_HEADLESS_MODE*/

		//Miscellaneous functions

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set cull mode for the given window.
		/// @param cullMode Cull mode.
		/// @param window Window to set cull mode for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetCullMode(CullMode cullMode, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the cull mode.
		/// @param cullMode Cull mode to use.
		/// @remark This function is only available in headless mode.
		void SetCullMode(CullMode cullMode);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set fill mode for the given window.
		/// @param fillMode Fill mode.
		/// @param window Window to set fill mode for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetFillMode(FillMode fillMode, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the fill mode.
		/// @param fillMode Fill mode to use.
		/// @remark This function is only available in headless mode.
		void SetFillMode(FillMode fillMode);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set primitive topology for the given window.
		/// @param topology Primitive topology.
		/// @param window Window to set primitive topology for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetPrimitiveTopology(PrimitiveTopology topology, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the primitive topology.
		/// @param topology Primitive topology to use.
		/// @remark This function is only available in headless mode.
		void SetPrimitiveTopology(PrimitiveTopology topology);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set front face for the given window.
		/// @param face Front face.
		/// @param window Window to set front face for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetFrontFace(FrontFace face, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the front face winding order.
		/// @param face Front face winding order to use.
		/// @remark This function is only available in headless mode.
		void SetFrontFace(FrontFace face);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// @param shadingRate Shading rate to use.
		/// @param postRasterizerRate Shading rate combiner to use.
		/// @param finalRate Shading rate combiner to use.
		/// @param window Window to set shading rate for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetShadingRate(ShadingRate shadingRate,
		                    ShadingRateCombiner postRasterizerRate,
							ShadingRateCombiner finalRate, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// @param shadingRate Shading rate to use.
		/// @param postRasterizerRate Shading rate combiner to use.
		/// @param finalRate Shading rate combiner to use.
		/// @remark This function is only available in headless mode.
		void SetShadingRate(ShadingRate shadingRate,
		                    ShadingRateCombiner postRasterizerRate,
							ShadingRateCombiner finalRate);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// Set the pipeline fragment shading rate via texture.
		/// @param texture Shading rate texture to use.
		/// @param window Window to set shading rate for. Default: Main Window.
		/// @note The texture must be in ResourceState::ShadingRateSource resource state.
		/// @remark @headless This function is not available in headless mode.
		void SetShadingRate(Ref<RenderTarget> texture, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the pipeline fragment shading rate via texture.
		/// @param texture Shading rate texture to use.
		/// @note The texture must be in ResourceState::ShadingRateSource resource state.
		/// @remark This function is only available in headless mode.
		void SetShadingRate(Ref<RenderTarget> texture);
#endif /*TRAP_HEADLESS_MODE*/
		/// @brief Set the anti aliasing method and the sample count.
		/// Use AntiAliasing::Off to disable anti aliasing.
		/// @param antiAliasing Anti aliasing method to use.
		/// @param sampleCount Sample count to use.
		/// @note This applies to the next frame onwards.
		void SetAntiAliasing(AntiAliasing antiAliasing, SampleCount sampleCount);
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the currently used anti aliasing method and the sample count.
		/// @param outAntiAliasing Output: Used anti aliasing method.
		/// @param outSampleCount Output: Used sample count.
		/// @param window Window to get anti aliasing from. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount, const Window* window = TRAP::Application::GetWindow()) noexcept;
#else
		/// @brief Retrieve the currently used anti aliasing method and the sample count.
		/// @param outAntiAliasing Output: Used anti aliasing method.
		/// @param outSampleCount Output: Used sample count.
		/// @remark This function is only available in headless mode.
		void GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount) noexcept;
#endif /*TRAP_HEADLESS_MODE*/
		/// @brief Retrieve the currently used anisotropy level.
		/// @return Used anisotropy level.
		[[nodiscard]] SampleCount GetAnisotropyLevel() noexcept;
		/// @brief Set the anisotropy level.
		/// A value of SampleCount::One effectively disables anisotropic filtering.
		/// @param anisotropyLevel Anisotropy level to use.
		/// @note Samplers need to be recreated in order to use the new anisotropy level.
		void SetAnisotropyLevel(SampleCount anisotropyLevel);

		//Blending functions

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set blend mode for the given window.
		/// @param modeRGB Blend mode for RGB.
		/// @param modeAlpha Blend mode for alpha.
		/// @param window Window to set blend mode for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the blend mode.
		/// @param modeRGB Blend mode to use for the RGB channels.
		/// @param modeAlpha Blend mode to use for the alpha channel.
		/// @remark This function is only available in headless mode.
		void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set combined blend constant for the given window.
		/// @param sourceRGBA Blend constant for source RGBA.
		/// @param destinationRGBA Blend constant for destination RGBA.
		/// @param window Window to set blend constant for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetBlendConstant(BlendConstant sourceRGBA, BlendConstant destinationRGBA,
							  const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set combined blend constant.
		/// @param sourceRGBA Blend constant for source RGBA.
		/// @param destinationRGBA Blend constant for destination RGBA.
		/// @remark This function is only available in headless mode.
		void SetBlendConstant(BlendConstant sourceRGBA, BlendConstant destinationRGBA);
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set separate blend constant for the given window.
		/// @param sourceRGB Blend constant for source RGB.
		/// @param sourceAlpha Blend constant for source alpha.
		/// @param destinationRGB Blend constant for destination RGB.
		/// @param destinationAlpha Blend constant for destination alpha.
		/// @param window Window to set blend constant for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
							  BlendConstant destinationRGB, BlendConstant destinationAlpha,
							  const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the blend constants/factors.
		/// @param sourceRGB Specifies how the red, green, and blue blending factors are computed.
		/// @param sourceAlpha Specifies how the alpha source blending factor is computed.
		/// @param destinationRGB Specifies how the red, green, and blue destination blending factors are computed.
		/// @param destinationAlpha Specified how the alpha destination blending factor is computed.
		/// @remark This function is only available in headless mode.
		void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                  BlendConstant destinationRGB, BlendConstant destinationAlpha);
#endif /*TRAP_HEADLESS_MODE*/

		//Viewport/Scissor functions

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set viewport size for the given window.
		/// @param x Upper left corner. Default: 0.
		/// @param y Upper left corner. Default: 0.
		/// @param width Viewport width.
		/// @param height Viewport height.
		/// @param window Window to set viewport size for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetViewport(u32 x, u32 y, u32 width, u32 height, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set viewport size.
		/// @param x X coordinate of the top left corner of the viewport. Default: 0.
		/// @param y Y coordinate of the top left corner of the viewport. Default: 0.
		/// @param width Viewport width.
		/// @param height Viewport height.
		/// @remark This function is only available in headless mode.
		void SetViewport(u32 x, u32 y, u32 width, u32 height);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set scissor size for the given window.
		/// @param x Upper left corner. Default: 0.
		/// @param y Upper left corner. Default: 0.
		/// @param width Scissor width.
		/// @param height Scissor height.
		/// @param window Window to set scissor size for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void SetScissor(u32 x, u32 y, u32 width, u32 height, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set scissor size.
		/// @param x Upper left corner. Default: 0.
		/// @param y Upper left corner. Default: 0.
		/// @param width Scissor width.
		/// @param height Scissor height.
		/// @remark This function is only available in headless mode.
		void SetScissor(u32 x, u32 y, u32 width, u32 height);
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// @brief Set RenderTarget resolution.
		/// @param width Width.
		/// @param height Height.
		/// @remark This function is only available in headless mode.
		void SetResolution(u32 width, u32 height);
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// @brief Get RenderTarget resolution.
		/// @param width Output: Width.
		/// @param height Output: Height.
		/// @remark This function is only available in headless mode.
		void GetResolution(u32& width, u32& height);
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the render scale for the given window.
		/// @param scale Render scale value (valid range: 0.5f-1.0f inclusive).
		/// @param window Window to set render scale for. Default: Main Window.
		/// @note This functon takes effect on the next frame onwards.
		/// @remark @headless This function is not available in headless mode.
		void SetRenderScale(f32 scale, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set the render scale.
		/// @param scale Render scale value (valid range: 0.5f-1.0f inclusive).
		/// @note This functon takes effect on the next frame.
		/// @remark This function is only available in headless mode.
		void SetRenderScale(f32 scale);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the used render scale value of the given window.
		/// @param window Window to retrieve render scale from. Default: Main Window.
		/// @return Render scale (between 0.5f and 2.0f inclusive).
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] f32 GetRenderScale(const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Retrieve the used render scale value.
		/// @return Render scale (between 0.5f and 2.0f inclusive).
		/// @remark This function is only available in headless mode.
		[[nodiscard]] f32 GetRenderScale();
#endif /*TRAP_HEADLESS_MODE*/

		//Drawing functions

#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw non-indexed, non-instanced geometry for the given window.
		/// @param vertexCount Number of vertices to draw.
		/// @param firstVertex Index of the first vertex to draw.
		/// @param window Window to draw for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void Draw(u32 vertexCount, u32 firstVertex = 0, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Draw non-indexed, non-instanced geometry.
		/// @param vertexCount Number of vertices to draw.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		void Draw(u32 vertexCount, u32 firstVertex = 0);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw indexed, non-instanced geometry for the given window.
		/// @param indexCount Number of indices to draw.
		/// @param firstIndex Index of the first indice to draw.
		/// @param vertexOffset Index of the first vertex to draw.
		/// @param window Window to draw for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void DrawIndexed(u32 indexCount, u32 firstIndex = 0, i32 vertexOffset = 0,
		                 const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Draw indexed, non-instanced geometry.
		/// @param indexCount Number of indices to draw.
		/// @param firstIndex Index of the first indice to draw. Default: 0.
		/// @param vertexOffset Index of the first vertex to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		void DrawIndexed(u32 indexCount, u32 firstIndex = 0, i32 vertexOffset = 0);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw non-indexed, instanced geometry for the given window.
		/// @param vertexCount Number of vertices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstVertex Index of the first vertex to draw.
		/// @param firstInstance Index of the first instance to draw.
		/// @param window Window to draw for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void DrawInstanced(u32 vertexCount, u32 instanceCount, u32 firstVertex = 0,
		                          u32 firstInstance = 0, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Draw non-indexed, instanced geometry.
		/// @param vertexCount Number of vertices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @param firstInstance Index of the first instance to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		void DrawInstanced(u32 vertexCount, u32 instanceCount, u32 firstVertex = 0,
		                   u32 firstInstance = 0);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw indexed, instanced geometry for the given window.
		/// @param indexCount Number of indices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstIndex Index of the first indice to draw.
		/// @param firstInstance Index of the first instance to draw.
		/// @param vertexOffset Index of the first vertex to draw.
		/// @param window Window to draw for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void DrawIndexedInstanced(u32 indexCount, u32 instanceCount, u32 firstIndex = 0,
		                          u32 firstInstance = 0, i32 vertexOffset = 0,
								  const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Draw indexed, instanced geometry.
		/// @param indexCount Number of indices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstIndex Index of the first indice to draw. Default: 0.
		/// @param firstInstance Index of the first instance to draw. Default: 0.
		/// @param vertexOffset Index of the first vertex to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		void DrawIndexedInstanced(u32 indexCount, u32 instanceCount,
		                          u32 firstIndex = 0, u32 firstInstance = 0,
								  i32 vertexOffset = 0);
#endif /*TRAP_HEADLESS_MODE*/

		//Compute functions

#ifndef TRAP_HEADLESS_MODE
		/// @brief Dispatch compute work on the given window.
		/// @param workGroupElementSizes Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// @param window Window to draw for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void Dispatch(const std::array<u32, 3>& workGroupElementSizes, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Dispatch.
		/// @param workGroupElements Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// @remark This function is only available in headless mode.
		void Dispatch(const std::array<u32, 3>& workGroupElements);
#endif /*TRAP_HEADLESS_MODE*/

		//TODO DispatchIndirect

		//CommandBuffer functions

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set push constant for the given window.
		/// @param name Name of the push constant.
		/// @param data Data to set push constant to.
		/// @param queueType Queue type on which to perform the operation. Default: Graphics.
		/// @param window Window to set push constant for. Default: Main Window.
		/// @warning: The minimum guaranteed size from the specification is 128 bytes, everything beyond is hardware dependent.
		/// @remark @headless This function is not available in headless mode.
		void SetPushConstants(std::string_view name, std::span<const u8> data,
		                      QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set push constant.
		/// @param name Name of the push constant.
		/// @param data Data to set push constant to.
		/// @param queueType Queue type on which to perform the operation. Default: Graphics.
		/// @warning: The minimum guaranteed size from the specification is 128 bytes, everything beyond is hardware dependent.
		/// @remark This function is only available in headless mode.
		void SetPushConstants(std::string_view name, std::span<const u8> data,
		                      QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set push constant for the given window.
		/// @param name Name of the push constant.
		/// @param data Data to set push constant to.
		/// @param queueType Queue type on which to perform the operation. Default: Graphics.
		/// @param window Window to set push constant for. Default: Main Window.
		/// @warning: The minimum guaranteed size from the specification is 128 bytes, everything beyond is hardware dependent.
		/// @remark @headless This function is not available in headless mode.
		template<typename T>
		requires (!TRAP::Utils::IsStdSpan<T>::value && !std::is_pointer_v<T>)
		void SetPushConstants(std::string_view name, const T& data,
		                      QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set push constant.
		/// @param name Name of the push constant.
		/// @param data Data to set push constant to.
		/// @param queueType Queue type on which to perform the operation. Default: Graphics.
		/// @warning: The minimum guaranteed size from the specification is 128 bytes, everything beyond is hardware dependent.
		/// @remark This function is only available in headless mode.
		template<typename T>
		requires (!TRAP::Utils::IsStdSpan<T>::value && !std::is_pointer_v<T>)
		void SetPushConstants(std::string_view name, const T& data,
		                      QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set render target for the given window.
		/// @param colorTarget Color target to set.
		/// @param depthStencil Depth stencil target to set.
		/// @param loadActions Load actions.
		/// @param window Window to set render target for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                      const TRAP::Ref<Graphics::RenderTarget>& depthStencil = nullptr,
							  RendererAPI::LoadActionsDesc* loadActions = nullptr,
							  const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set render target.
		/// @param colorTarget Color target to set.
		/// @param depthStencil Depth stencil target to set.
		/// @param loadActions Load actions.
		/// @remark This function is only available in headless mode.
		void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                      const TRAP::Ref<Graphics::RenderTarget>& depthStencil = nullptr,
							  RendererAPI::LoadActionsDesc* loadActions = nullptr);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set multiple render targets for the given window.
		/// @param colorTargets Color targets to set.
		/// @param depthStencil Depth stencil target to set.
		/// @param loadActions Load actions.
		/// @param window Window to set render target for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                       const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
							   RendererAPI::LoadActionsDesc* loadActions = nullptr,
							   const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Set multiple render targets.
		/// @param colorTargets Color targets to set.
		/// @param depthStencil Depth stencil target to set.
		/// @param loadActions Load actions.
		/// @remark This function is only available in headless mode.
		void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                       const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
							   RendererAPI::LoadActionsDesc* loadActions = nullptr);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Start a new render pass for the given window.
		/// @param window Window to start render pass for. Default: Main Window.
		/// @note This will rebind the render target for the current frame.
		/// @remark @headless This function is not available in headless mode.
		void StartRenderPass(const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Start a render pass.
		/// @note This will rebind the render target for the current frame.
		/// @remark This function is only available in headless mode.
		void StartRenderPass();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Stop the running render pass for the given window.
		/// @param window Window to stop render pass for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void StopRenderPass(const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Stop running render pass.
		/// @remark This function is only available in headless mode.
		void StopRenderPass();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Buffer barrier used to synchronize and transition the buffer.
		/// @param bufferBarrier Buffer barrier to use.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to sync and transition buffer for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void BufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
		                   QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Buffer barrier used to synchronize and transition the buffer.
		/// @param bufferBarrier Buffer barrier to use.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		void BufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
		                   QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Buffer barrier used to synchronize and transition multiple buffers.
		/// @param bufferBarriers Buffer barriers to use.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to sync and transition buffers for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void BufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
							QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Buffer barrier used to synchronize and transition multiple buffers.
		/// @param bufferBarriers Buffer barriers to use.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		void BufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
							QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Texture barrier used to synchronize and transition the texture.
		/// @param textureBarrier Texture barrier to use.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to sync and transition texture for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void TextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
		                    QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Texture barrier used to synchronize and transition the texture.
		/// @param textureBarrier Texture barrier to use.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		void TextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
		                    QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Texture barrier used to synchronize and transition multiple textures.
		/// @param textureBarriers Texture barriers to use.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to sync and transition textures for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void TextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
		                     QueueType queueType = QueueType::Graphics, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Texture barrier used to synchronize and transition multiple textures.
		/// @param textureBarriers Texture barriers to use.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		void TextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
		                     QueueType queueType = QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief RenderTarget barrier used to synchronize and transition the RenderTarget.
		/// @param renderTargetBarrier RenderTarget barrier to use.
		/// @param window Window to sync and transition RenderTarget for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void RenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief RenderTarget barrier used to synchronize and transition the RenderTarget.
		/// @param renderTargetBarrier RenderTarget barrier to use.
		/// @remark This function is only available in headless mode.
		void RenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier);
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief RenderTarget barrier used to synchronize and transition multiple RenderTargets.
		/// @param renderTargetBarriers RenderTarget barriers to use.
		/// @param window Window to sync and transition RenderTargets for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void RenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers, const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief RenderTarget barrier used to synchronize and transition multiple RenderTargets.
		/// @param renderTargetBarriers RenderTarget barriers to use.
		/// @remark This function is only available in headless mode.
		void RenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers);
#endif /*TRAP_HEADLESS_MODE*/

		//Utility

#ifndef TRAP_HEADLESS_MODE
		/// @brief Capture a screenshot of the last presented frame.
		/// @param window Window to take screenshot from. Default: Main Window.
		/// @return Captured screenshot as TRAP::Image on success, nullptr otherwise.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] TRAP::Scope<TRAP::Image> CaptureScreenshot(const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Capture a screenshot of the last presented frame.
		/// @return Captured screenshot as TRAP::Image on success, nullptr otherwise.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] TRAP::Scope<TRAP::Image> CaptureScreenshot();
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Transition a texture from old layout to the new layout.
		/// The transition happens immediately and is guaranteed to be complete when the function returns.
		/// @param texture Texture to transition layout.
		/// @param oldLayout Current resource state of the given texture.
		/// @param newLayout New resource state for the given texture.
		/// @param queueType Queue type on which to perform the transition. Default: Graphics.
		void Transition(const Ref<Texture>& texture, RendererAPI::ResourceState oldLayout,
		                RendererAPI::ResourceState newLayout,
		                QueueType queueType = QueueType::Graphics);

#ifndef TRAP_HEADLESS_MODE
		/// @brief Resolve an MSAA render target to a non MSAA render target.
		/// Needed to transfer MSAA rendered image data to a presentable non-MSAA target.
		/// @param source Source MSAA render target to resolve.
		/// @param destination Destination non MSAA render target to resolve into.
		/// @param window Window to do the resolve pass on.
		/// @note source and destination must be in ResourceState::RenderTarget resource state.
		/// @remark @headless This function is not available in headless mode.
		void MSAAResolvePass(TRAP::Ref<RenderTarget> source, TRAP::Ref<RenderTarget> destination,
		                     const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Resolve an MSAA render target to a non MSAA render target.
		/// Needed to transfer MSAA rendered image data to a presentable non-MSAA target.
		/// @param source Source MSAA render target to resolve.
		/// @param destination Destination non MSAA render target to resolve into.
		/// @note source and destination must be in ResourceState::RenderTarget resource state.
		/// @remark This function is only available in headless mode.
		void MSAAResolvePass(TRAP::Ref<RenderTarget> source, TRAP::Ref<RenderTarget> destination);
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Retrieve the CPU side frames per second.
		/// @return CPU frames per second.
		[[nodiscard]] u32 GetCPUFPS();
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the GPU side frames per second.
		/// @param window Window to get frames per second from.
		/// @return GPU frames per second.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] u32 GetGPUFPS(const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Retrieve the GPU side frames per second.
		/// @return GPU frames per second.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] u32 GetGPUFPS();
#endif /*TRAP_HEADLESS_MODE*/
		/// @brief Retrieve the CPU side frame time.
		/// @return CPU frame time in milliseconds.
		[[nodiscard]] f32 GetCPUFrameTime();
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the GPU side frame time for the graphics queue.
		/// @param window Window to get frame time from.
		/// @return GPU Graphics frame time in milliseconds.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] f32 GetGPUGraphicsFrameTime(const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Retrieve the GPU side frame time for the graphics queue.
		/// @return GPU Graphics frame time in milliseconds.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] f32 GetGPUGraphicsFrameTime();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the GPU side frame time for the compute queue.
		/// @param window Window to get frame time from.
		/// @return GPU Compute frame time in milliseconds.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] f32 GetGPUComputeFrameTime(const Window* window = TRAP::Application::GetWindow());
#else
		/// @brief Retrieve the GPU side frame time for the compute queue.
		/// @return GPU Compute frame time in milliseconds.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] f32 GetGPUComputeFrameTime();
#endif /*TRAP_HEADLESS_MODE*/
		/// @brief Retrieve the name of the GPU that is currently used by the RendererAPI.
		/// @return GPU name.
		[[nodiscard]] std::string GetGPUName();

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the latency mode.
		/// @param mode LatencyMode to set.
		/// @param window Window to set latency mode for.
		/// @remark Only LatencyMode::Disabled is supported everywhere.
		/// @remark @win32 Other LatencyModes are only available on Windows 10 or newer with NVIDIA hardware.
		/// @remark @headless This function is not available in headless mode.
		void SetLatencyMode(LatencyMode mode, const Window* window = TRAP::Application::GetWindow());
		/// @brief Retrieve the currently used latency mode.
		/// @param window Window to retrieve latency mode for.
		/// @return Used latency mode.
		/// @note The returned value may differ from the requested mode set with SetLatencyMode().
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] LatencyMode GetLatencyMode(const Window* window = TRAP::Application::GetWindow());
#endif /*TRAP_HEADLESS_MODE*/
	};
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
template<typename T>
requires (!TRAP::Utils::IsStdSpan<T>::value && !std::is_pointer_v<T>)
void TRAP::Graphics::RenderCommand::SetPushConstants(const std::string_view name, const T& data,
                                                     const QueueType queueType, const Window* const window)
{
	const auto byteSpan = TRAP::Utils::AsBytes(std::span<const T, 1>(&data, 1));
	RendererAPI::GetRenderer()->BindPushConstants(name, byteSpan, queueType, window);
}
#else
template<typename T>
requires (!TRAP::Utils::IsStdSpan<T>::value && !std::is_pointer_v<T>)
void TRAP::Graphics::RenderCommand::SetPushConstants(const std::string_view name, const T& data,
                                                     const QueueType queueType)
{
	const auto byteSpan = TRAP::Utils::AsBytes(std::span<const T, 1>(&data, 1));
	RendererAPI::GetRenderer()->BindPushConstants(name, byteSpan, queueType);
}
#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_RENDERCOMMAND_H*/

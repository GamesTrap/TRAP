#ifndef TRAP_RENDERERAPI_H
#define TRAP_RENDERERAPI_H

#include <variant>
#include <unordered_map>

#include "Window/Window.h"
#include "ImageFormat.h"
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"
#include "Utils/Utils.h"
#include "Maths/Types.h"
#include "Maths/Vec3.h"

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
#include <NvLowLatencyVk.h>
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

namespace TRAP
{
	class Application;
	class Window;
	class Image;
}

namespace TRAP::Graphics
{
	class VertexBufferLayout;
	class CommandPool;
	class DescriptorPool;
	class DescriptorSet;
	class Shader;
	class Pipeline;
	class PipelineCache;
	class SwapChain;
	class Fence;
	class Semaphore;
	class QueryPool;
	class Queue;
	class Buffer;
	class CommandBuffer;
	class RootSignature;
	class Sampler;
	class RenderTarget;
	class Texture;
}

namespace TRAP::Graphics::API
{
	class ResourceLoader;

	namespace ShaderReflection
	{
		enum class TextureDimension : u32;
	}
}

namespace TRAP::Graphics
{
	/// <summary>
	/// Supported render APIs.
	/// </summary>
	enum class RenderAPI
	{
		NONE = 0,
		Vulkan
	};

	class RendererAPI
	{
	public:
		enum class CompareMode : u32;
		enum class StencilOp : u32;
		enum class CullMode : u32;
		enum class FillMode : u32;
		enum class PrimitiveTopology;
		enum class FrontFace : u32;
		enum class BlendMode : u32;
		enum class BlendConstant : u32;
		enum class IndexType;
		enum class ClearBufferType : u32;
		enum class ShadingRate : u32;
		enum class ShadingRateCombiner : u32;
		enum class ResourceState : u32;
		enum class AntiAliasing;
		enum class SampleCount;
		enum class QueueType : u32;
		enum class GPUVendor;
		enum class LatencyMode : u32;
		struct Color;
		struct LoadActionsDesc;
		struct BufferBarrier;
		struct TextureBarrier;
		struct RenderTargetBarrier;
	//protected:
		struct PerViewportData;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		constexpr RendererAPI() noexcept = default;
	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr virtual ~RendererAPI() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr RendererAPI(const RendererAPI&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr RendererAPI& operator=(const RendererAPI&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr RendererAPI(RendererAPI&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr RendererAPI& operator=(RendererAPI&&) = delete;

		/// <summary>
		/// Initialize the Renderer.
		/// </summary>
		/// <param name="gameName">Name of the game.</param>
		/// <param name="renderAPI">Render API to use.</param>
		static void Init(std::string_view gameName, RenderAPI renderAPI);
		/// <summary>
		/// Shutdown the Renderer.
		/// </summary>
		static void Shutdown();

		/// <summary>
		/// Retrieve the Renderer singleton.
		/// </summary>
		/// <returns>Renderer.</returns>
		[[nodiscard]] static RendererAPI* GetRenderer();
		/// <summary>
		/// Retrieve the resource loader singleton.
		/// </summary>
		/// <returns>Resource loader.</returns>
		[[nodiscard]] static API::ResourceLoader* GetResourceLoader() noexcept;

		/// <summary>
		/// Auto select a supported render API.
		/// </summary>
		/// <returns>Auto selected render API.</returns>
		[[nodiscard]] static RenderAPI AutoSelectRenderAPI();
		/// <summary>
		/// Check whether a render API is supported by the system.
		/// </summary>
		/// <returns>True if supported, false otherwise.</returns>
		[[nodiscard]] static bool IsSupported(RenderAPI api);
		/// <summary>
		/// Retrieve the currently used render API.
		/// </summary>
		/// <returns>Currently used render API.</returns>
		[[nodiscard]] static RenderAPI GetRenderAPI() noexcept;

		/// <summary>
		/// Set a new GPU to use.
		///
		/// Note: This only takes effect after a restart of the engine.
		/// </summary>
		/// <param name="GPUUUID">UUID of the GPU to use.</param>
		static constexpr void SetNewGPU(const TRAP::Utils::UUID& GPUUUID) noexcept;
		/// <summary>
		/// Get the UUID of the new GPU to use.
		///
		/// Note: This will return an empty UUID if no new GPU was set.
		/// </summary>
		/// <returns>UUID of the new GPU to use.</returns>
		[[nodiscard]] static TRAP::Utils::UUID GetNewGPU() noexcept;

		/// <summary>
		/// On post update function.
		/// This function performs several tasks that need to be done after LayerStack::OnUpdate() calls.
		/// Currently this only performs scaling of the render targets, dependening on the current render scale.
		/// </summary>
		static void OnPostUpdate();

		/// <summary>
		/// Initialize the internal renderer.
		/// </summary>
		/// <param name="gameName">Name of the game.</param>
		virtual void InitInternal(std::string_view gameName) = 0;

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
		virtual void Flush(const Window* window) const = 0;
#else
		/// <summary>
		/// Flush renderer.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Starts graphics and compute recording for the next frame.
		/// </summary>
		virtual void Flush() const = 0;
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
		virtual void Dispatch(std::array<u32, 3> workGroupElements, const Window* window) const = 0;
#else
		/// <summary>
		/// Dispatch.
		/// </summary>
		/// <param name="workGroupElements">
		/// Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// </param>
		virtual void Dispatch(std::array<u32, 3> workGroupElements) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the VSync state for the given window.
		/// </summary>
		/// <param name="vsync">Enable or disable VSync.</param>
		/// <param name="window">Window to set VSync for.</param>
		virtual void SetVSync(bool vsync, const Window* window) const = 0;
		/// <summary>
		/// Retrieve whether VSync is enabled or not for the given window.
		/// </summary>
		/// <param name="window">Window to retrieve VSync for.</param>
		/// <returns>True if VSync is enabled, false otherwise.</returns>
		[[nodiscard]] virtual bool GetVSync(const Window* window) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the FPS limit for NVIDIA-Reflex.
		/// Note: This function affects all windows.
		/// Note: Do not call this function in user code! Use TRAP::Application::SetFPSLimit() instead.
		///       This function is only used internally for NVIDIA-Reflex.
		/// </summary>
		/// <param name="limit">FPS target to limit to.</param>
		virtual void SetReflexFPSLimit(u32 limit) = 0;
#endif /*TRAP_HEADLESS_MODE*/

		//RenderTarget Stuff

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the render scale for the given window.
		/// Note: This functon takes effect on the next frame.
		/// </summary>
		/// <param name="scale">Render scale value (valid range: 0.5f-1.0f inclusive).</param>
		/// <param name="window">Window to set render scale for.</param>
		virtual void SetRenderScale(f32 scale, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the render scale.
		/// Note: This functon takes effect on the next frame.
		/// </summary>
		/// <param name="scale">Render scale value (valid range: 0.5f-1.0f inclusive).</param>
		virtual void SetRenderScale(f32 scale) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the used render scale value of the given window.
		/// </summary>
		/// <param name="window">Window to retrieve render scale from.</param>
		/// <returns>Render scale (between 0.5f and 2.0f inclusive).</returns>
		[[nodiscard]] virtual f32 GetRenderScale(const Window* window) const = 0;
#else
		/// <summary>
		/// Retrieve the used render scale value.
		/// </summary>
		/// <returns>Render scale (between 0.5f and 2.0f inclusive).</returns>
		[[nodiscard]] virtual f32 GetRenderScale() const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the clear color to be used by the given window.
		/// </summary>
		/// <param name="color">New clear color.</param>
		/// <param name="window">Window to set clear color for.</param>
		virtual void SetClearColor(const Color& color /*= { 0.1f, 0.1f, 0.1f, 1.0f }*/,
		                           const Window* window) const = 0;
#else
		/// <summary>
		/// Set the clear color.
		/// </summary>
		/// <param name="color">New clear color.</param>
		virtual void SetClearColor(const Color& color /*= { 0.1f, 0.1f, 0.1f, 1.0f }*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the clear depth value to be used by the given window.
		/// </summary>
		/// <param name="depth">New clear depth value. Must be between 0.0f and 1.0f</param>
		/// <param name="window">Window to set clear depth value for.</param>
		virtual void SetClearDepth(f32 depth /*= 0.0f*/, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the clear depth value.
		/// </summary>
		/// <param name="depth">New clear depth value. Must be between 0.0f and 1.0f</param>
		virtual void SetClearDepth(f32 depth /*= 0.0f*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the clear stencil value to be used by the given window.
		/// </summary>
		/// <param name="stencil">New clear stencil value.</param>
		/// <param name="window">Window to set clear stencil value for.</param>
		virtual void SetClearStencil(u32 stencil /*= 0*/, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the clear stencil value.
		/// </summary>
		/// <param name="stencil">New clear stencil value.</param>
		virtual void SetClearStencil(u32 stencil /*= 0*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the resolution of the render targets.
		/// </summary>
		/// <param name="width">New width.</param>
		/// <param name="height">New height.</param>
		virtual void SetResolution(u32 width, u32 height) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		//Pipeline Stuff

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Enable or disable depth testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth testing.</param>
		/// <param name="window">Window to set depth testing for.</param>
		virtual void SetDepthTesting(bool enabled, const Window* window) const = 0;
#else
		/// <summary>
		/// Enable or disable depth testing.
		/// </summary>
		/// <param name="enabled">Enable or disable depth testing.</param>
		virtual void SetDepthTesting(bool enabled) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Enable or disable depth writing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth writing.</param>
		/// <param name="window">Window to set depth writing for.</param>
		virtual void SetDepthWriting(bool enabled, const Window* window) const = 0;
#else
		/// <summary>
		/// Enable or disable depth writing.
		/// </summary>
		/// <param name="enabled">Enable or disable depth writing.</param>
		virtual void SetDepthWriting(bool enabled) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the depth function for the given window.
		/// </summary>
		/// <param name="function">Function to use for depth testing.</param>
		/// <param name="window">Window to set depth function for.</param>
		virtual void SetDepthFunction(CompareMode function, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the depth function.
		/// </summary>
		/// <param name="function">Function to use for depth testing.</param>
		virtual void SetDepthFunction(CompareMode function) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the depth action to perform when depth testing fails for the given window.
		/// </summary>
		/// <param name="front">Depth action to perform when depth testing fails.</param>
		/// <param name="back">Depth action to perform when depth testing fails.</param>
		/// <param name="window">Window to set the depth fail action for.</param>
		virtual void SetDepthFail(StencilOp front, StencilOp back, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the depth action to perform when depth testing fails.
		/// </summary>
		/// <param name="front">Depth action to perform when depth testing fails.</param>
		/// <param name="back">Depth action to perform when depth testing fails.</param>
		virtual void SetDepthFail(StencilOp front, StencilOp back) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the depth bias (scalar factor to add to each fragments depth value) for the given window.
		/// </summary>
		/// <param name="depthBias">Depth bias.</param>
		/// <param name="window">Window to set the depth bias for.</param>
		virtual void SetDepthBias(i32 depthBias, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the depth bias (scalar factor to add to each fragments depth value).
		/// </summary>
		/// <param name="depthBias">Depth bias.</param>
		virtual void SetDepthBias(i32 depthBias) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation) for the given window.
		/// </summary>
		/// <param name="factor">Depth bias slope factor.</param>
		/// <param name="window">Window to set the depth bias slope factor for.</param>
		virtual void SetDepthBiasSlopeFactor(f32 factor, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation).
		/// </summary>
		/// <param name="factor">Depth bias slope factor.</param>
		virtual void SetDepthBiasSlopeFactor(f32 factor) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Enable or disable stencil testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable stencil testing.</param>
		/// <param name="window">Window to set stencil testing for.</param>
		virtual void SetStencilTesting(bool enabled, const Window* window) const = 0;
#else
		/// <summary>
		/// Enable or disable stencil testing.
		/// </summary>
		/// <param name="enabled">Enable or disable stencil testing.</param>
		virtual void SetStencilTesting(bool enabled) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the stencil action to perform when stencil testing fails for the given window.
		/// </summary>
		/// <param name="front">Stencil action to perform when stencil testing fails.</param>
		/// <param name="back">Stencil action to perform when stencil testing fails.</param>
		/// <param name="window">Window to set the stencil fail action for.</param>
		virtual void SetStencilFail(StencilOp front, StencilOp back, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the stencil action to perform when stencil testing fails.
		/// </summary>
		/// <param name="front">Stencil action to perform when stencil testing fails.</param>
		/// <param name="back">Stencil action to perform when stencil testing fails.</param>
		virtual void SetStencilFail(StencilOp front, StencilOp back) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the stencil action to perform when stencil testing passes for the given window.
		/// </summary>
		/// <param name="front">Stencil action to perform when passed.</param>
		/// <param name="back">Stencil action to perform when passed.</param>
		/// <param name="window">Window to set the stencil pass action for.</param>
		virtual void SetStencilPass(StencilOp front, StencilOp back, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the stencil action to perform when stencil testing passes.
		/// </summary>
		/// <param name="front">Stencil action to perform when passed.</param>
		/// <param name="back">Stencil action to perform when passed.</param>
		virtual void SetStencilPass(StencilOp front, StencilOp back) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the stencil functions for the given window.
		/// </summary>
		/// <param name="front">Function to use on the front for stencil testing.</param>
		/// <param name="back">Function to use on the back for stencil testing.</param>
		/// <param name="window">Window to set stencil functions for.</param>
		virtual void SetStencilFunction(CompareMode front, CompareMode back, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the stencil functions.
		/// </summary>
		/// <param name="front">Function to use on the front for stencil testing.</param>
		/// <param name="back">Function to use on the back for stencil testing.</param>
		virtual void SetStencilFunction(CompareMode front, CompareMode back) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the stencil mask for the given window.
		/// </summary>
		/// <param name="read">Select the bits of the stencil values to test.</param>
		/// <param name="write">Select the bits of the stencil values updated by the stencil test.</param>
		/// <param name="window">Window to set stencil mask for.</param>
		virtual void SetStencilMask(u8 read, u8 write, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the stencil mask.
		/// </summary>
		/// <param name="read">Select the bits of the stencil values to test.</param>
		/// <param name="write">Select the bits of the stencil values updated by the stencil test.</param>
		virtual void SetStencilMask(u8 read, u8 write) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the cull mode for the given window.
		/// </summary>
		/// <param name="mode">Cull mode to use.</param>
		/// <param name="window">Window to set cull mode for.</param>
		virtual void SetCullMode(CullMode mode, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the cull mode.
		/// </summary>
		/// <param name="mode">Cull mode to use.</param>
		virtual void SetCullMode(CullMode mode) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the fill mode for the given window.
		/// </summary>
		/// <param name="mode">Fill mode to use.</param>
		/// <param name="window">Window to set fill mode for.</param>
		virtual void SetFillMode(FillMode mode, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the fill mode.
		/// </summary>
		/// <param name="mode">Fill mode to use.</param>
		virtual void SetFillMode(FillMode mode) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the primitive topology for the given window.
		/// </summary>
		/// <param name="topology">Primitive topology to use.</param>
		/// <param name="window">Window to set primitive topology for.</param>
		virtual void SetPrimitiveTopology(PrimitiveTopology topology, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the primitive topology.
		/// </summary>
		/// <param name="topology">Primitive topology to use.</param>
		virtual void SetPrimitiveTopology(PrimitiveTopology topology) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the front face winding order for the given window.
		/// </summary>
		/// <param name="face">Front face winding order to use.</param>
		/// <param name="window">Window to set front face winding order for.</param>
		virtual void SetFrontFace(FrontFace face, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the front face winding order.
		/// </summary>
		/// <param name="face">Front face winding order to use.</param>
		virtual void SetFrontFace(FrontFace face) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the blend mode for the given window.
		/// </summary>
		/// <param name="modeRGB">Blend mode to use for the RGB channels.</param>
		/// <param name="modeAlpha">Blend mode to use for the alpha channel.</param>
		/// <param name="window">Window to set the blend mode for.</param>
		virtual void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the blend mode.
		/// </summary>
		/// <param name="modeRGB">Blend mode to use for the RGB channels.</param>
		/// <param name="modeAlpha">Blend mode to use for the alpha channel.</param>
		virtual void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha) const = 0;
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
		virtual void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                          BlendConstant destinationRGB, BlendConstant destinationAlpha,
									  const Window* window) const = 0;
#else
		/// <summary>
		/// Set the blend constants/factors.
		/// </summary>
		/// <param name="sourceRGB">Specifies how the red, green, and blue blending factors are computed.</param>
		/// <param name="sourceAlpha">Specifies how the alpha source blending factor is computed.</param>
		/// <param name="destinationRGB">Specifies how the red, green, and blue destination blending factors are computed.</param>
		/// <param name="destinationAlpha">Specified how the alpha destination blending factor is computed.</param>
		virtual void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                          BlendConstant destinationRGB, BlendConstant destinationAlpha) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		/// <param name="window">Window to set the shading rate for.</param>
		virtual void SetShadingRate(ShadingRate shadingRate,
		                            ShadingRateCombiner postRasterizerRate,
							        ShadingRateCombiner finalRate, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		virtual void SetShadingRate(ShadingRate shadingRate,
		                            ShadingRateCombiner postRasterizerRate,
							        ShadingRateCombiner finalRate) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the pipeline fragment shading rate via texture.
		/// </summary>
		/// <param name="texture">
		/// Shading rate texture to use.
		/// Note: The texture must be in ResourceState::ShadingRateSource.
		/// </param>
		/// <param name="window">Window to set shading rate for.</param>
		virtual void SetShadingRate(Ref<RenderTarget> texture, const Window* window) const = 0;
#else
		/// <summary>
		/// Set the pipeline fragment shading rate via texture.
		/// </summary>
		/// <param name="texture">
		/// Shading rate texture to use.
		/// Note: The texture must be in ResourceState::ShadingRateSource.
		/// </param>
		virtual void SetShadingRate(Ref<RenderTarget> texture) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Clear the given window's render target.
		/// </summary>
		/// <param name="clearType">Type of buffer to clear.</param>
		/// <param name="window">Window to clear.</param>
		virtual void Clear(ClearBufferType clearType, const Window* window) const = 0;
#else
		/// <summary>
		/// Clear the given render target.
		/// </summary>
		/// <param name="clearType">Type of buffer to clear.</param>
		virtual void Clear(ClearBufferType clearType) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		//CommandBuffer Stuff

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
		virtual void SetViewport(u32 x, u32 y, u32 width, u32 height, f32 minDepth /*= 0.0f*/,
		                         f32 maxDepth /*= 1.0f*/, const Window* window) const = 0;
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
		virtual void SetViewport(u32 x, u32 y, u32 width, u32 height, f32 minDepth /*= 0.0f*/,
		                         f32 maxDepth /*= 1.0f*/) const = 0;
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
		virtual void SetScissor(u32 x, u32 y, u32 width, u32 height,
		                        const Window* window) const = 0;
#else
		/// <summary>
		/// Set scissor size.
		/// </summary>
		/// <param name="x">Upper left corner.</param>
		/// <param name="y">Upper left corner.</param>
		/// <param name="width">New scissor width.</param>
		/// <param name="height">New scissor height.</param>
		virtual void SetScissor(u32 x, u32 y, u32 width, u32 height) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw non-indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for.</param>
		virtual void Draw(u32 vertexCount, u32 firstVertex /*= 0*/, const Window* window) const = 0;
#else
		/// <summary>
		/// Draw non-indexed, non-instanced geometry.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		virtual void Draw(u32 vertexCount, u32 firstVertex /*= 0*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="vertexOffset">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for.</param>
		virtual void DrawIndexed(u32 indexCount, u32 firstIndex /*= 0*/, i32 vertexOffset /*= 0*/,
		                         const Window* window) const = 0;
#else
		/// <summary>
		/// Draw indexed, non-instanced geometry.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="vertexOffset">Index of the first vertex to draw. Default: 0.</param>
		virtual void DrawIndexed(u32 indexCount, u32 firstIndex /*= 0*/, i32 vertexOffset /*= 0*/) const = 0;
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
		virtual void DrawInstanced(u32 vertexCount, u32 instanceCount, u32 firstVertex /*= 0*/,
		                           u32 firstInstance /*= 0*/, const Window* window) const = 0;
#else
		/// <summary>
		/// Draw non-indexed, instanced geometry.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		virtual void DrawInstanced(u32 vertexCount, u32 instanceCount, u32 firstVertex /*= 0*/,
		                           u32 firstInstance /*= 0*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Draw indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		/// <param name="vertexOffset">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for.</param>
		virtual void DrawIndexedInstanced(u32 indexCount, u32 instanceCount,
		                                  u32 firstIndex /*= 0*/, u32 firstInstance /*= 0*/,
										  i32 vertexOffset /*= 0*/, const Window* window) const = 0;
#else
		/// <summary>
		/// Draw indexed, instanced geometry.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		/// <param name="vertexOffset">Index of the first vertex to draw. Default: 0.</param>
		virtual void DrawIndexedInstanced(u32 indexCount, u32 instanceCount,
		                                  u32 firstIndex /*= 0*/, u32 firstInstance /*= 0*/,
										  i32 vertexOffset /*= 0*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind vertex buffer on the given window.
		/// </summary>
		/// <param name="vBuffer">Vertex buffer to bind.</param>
		/// <param name="layout">Layout of the vertex buffer.</param>
		/// <param name="window">Window to bind the vertex buffer for.</param>
		virtual void BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer, const VertexBufferLayout& layout,
		                              const Window* window) const = 0;
#else
		/// <summary>
		/// Bind vertex buffer.
		/// </summary>
		/// <param name="vBuffer">Vertex buffer to bind.</param>
		/// <param name="layout">Layout of the vertex buffer.</param>
		virtual void BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer, const VertexBufferLayout& layout) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind an index buffer on the given window.
		/// </summary>
		/// <param name="iBuffer">Index buffer to bind.</param>
		/// <param name="indexType">Data type used by the index buffer.</param>
		/// <param name="window">Window to bind the vertex buffer for.</param>
		virtual void BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer, IndexType indexType,
		                             const Window* window) const = 0;
#else
		/// <summary>
		/// Bind an index buffer.
		/// </summary>
		/// <param name="iBuffer">Index buffer to bind.</param>
		/// <param name="indexType">Data type used by the index buffer.</param>
		virtual void BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer, IndexType indexType) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind a descriptor set on the given window.
		/// </summary>
		/// <param name="dSet">Descriptor set to bind.</param>
		/// <param name="index">Index for which descriptor set to bind.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the descriptor set for.</param>
		virtual void BindDescriptorSet(DescriptorSet& dSet, u32 index,
		                               QueueType queueType /*= QueueType::Graphics*/,
									   const Window* window) const = 0;
#else
		/// <summary>
		/// Bind a descriptor set.
		/// </summary>
		/// <param name="dSet">Descriptor set to bind.</param>
		/// <param name="index">Index for which descriptor set to bind.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		virtual void BindDescriptorSet(DescriptorSet& dSet, u32 index,
		                               QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind push constant buffer data on the given window.
		/// Note: There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// </summary>
		/// <param name="name">Name of the push constant block.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="constantsLength">Length in bytes of the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the push constants for.</param>
		virtual void BindPushConstants(std::string_view name, const void* constantsData,
							           usize constantsLength, QueueType queueType /*= QueueType::Graphics*/,
									   const Window* window) const = 0;
#else
		/// <summary>
		/// Bind push constant buffer data.
		/// Note: There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// </summary>
		/// <param name="name">Name of the push constant block.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="constantsLength">Length in bytes of the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		virtual void BindPushConstants(std::string_view name, const void* constantsData,
		                               usize constantsLength,
		                               QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Bind push constant buffer data on the given window.
		/// </summary>
		/// <param name="paramIndex">Index of the push constant block in the RootSignatures descriptors array.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="constantsLength">Length in bytes of the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the push constants for.</param>
		virtual void BindPushConstantsByIndex(u32 paramIndex, const void* constantsData,
										      usize constantsLength, QueueType queueType /*= QueueType::Graphics*/,
											  const Window* window) const = 0;
#else
		/// <summary>
		/// Bind push constant buffer data.
		/// </summary>
		/// <param name="paramIndex">Index of the push constant block in the RootSignatures descriptors array.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="constantsLength">Length in bytes of the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		virtual void BindPushConstantsByIndex(u32 paramIndex, const void* constantsData,
		                                      usize constantsLength,
											  QueueType queueType /*= QueueType::Graphics*/) const = 0;
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
		virtual void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                              const TRAP::Ref<Graphics::RenderTarget>& depthStencil /*= nullptr*/,
									  const RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
									  std::vector<u32>* colorArraySlices /*= nullptr*/,
									  std::vector<u32>* colorMipSlices /*= nullptr*/,
									  u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/,
									  const Window* window) const = 0;
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
		virtual void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                              const TRAP::Ref<Graphics::RenderTarget>& depthStencil /*= nullptr*/,
									  const RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
									  std::vector<u32>* colorArraySlices /*= nullptr*/,
									  std::vector<u32>* colorMipSlices /*= nullptr*/,
									  u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/) const = 0;
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
		virtual void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                               const TRAP::Ref<Graphics::RenderTarget>& depthStencil /*= nullptr*/,
									   const RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
									   std::vector<u32>* colorArraySlices /*= nullptr*/,
									   std::vector<u32>* colorMipSlices /*= nullptr*/,
									   u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/,
									   const Window* window) const = 0;
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
		virtual void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                               const TRAP::Ref<Graphics::RenderTarget>& depthStencil /*= nullptr*/,
									   const RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
									   std::vector<u32>* colorArraySlices /*= nullptr*/,
									   std::vector<u32>* colorMipSlices /*= nullptr*/,
									   u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="bufferBarrier">Buffer barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barrier for.</param>
		virtual void ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
										   QueueType queueType /*= QueueType::Graphics*/,
								           const Window* window) const = 0;
#else
		/// <summary>
		/// Add a resource barrier (memory dependency).
		/// </summary>
		/// <param name="bufferBarrier">Buffer barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		virtual void ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
										   QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="bufferBarriers">Buffer barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barriers for.</param>
		virtual void ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
											QueueType queueType /*= QueueType::Graphics*/,
									        const Window* window) const = 0;
#else
		/// <summary>
		/// Add resource barriers (memory dependencies).
		/// </summary>
		/// <param name="bufferBarriers">Buffer barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		virtual void ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
											QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="textureBarrier">Texture barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barrier for.</param>
		virtual void ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
											QueueType queueType /*= QueueType::Graphics*/,
									        const Window* window) const = 0;
#else
		/// <summary>
		/// Add a resource barrier (memory dependency).
		/// </summary>
		/// <param name="textureBarrier">Texture barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		virtual void ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
											QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="textureBarriers">Texture barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barriers for.</param>
		virtual void ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
											 QueueType queueType /*= QueueType::Graphics*/,
									         const Window* window) const = 0;
#else
		/// <summary>
		/// Add resource barriers (memory dependencies).
		/// </summary>
		/// <param name="textureBarriers">Texture barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		virtual void ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
											 QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="renderTargetBarrier">Render target barrier.</param>
		/// <param name="window">Window to add the barrier for.</param>
		virtual void ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
									             const Window* window) const = 0;
#else
		/// <summary>
		/// Add a resource barrier (memory dependency).
		/// </summary>
		/// <param name="renderTargetBarrier">Render target barrier.</param>
		virtual void ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="renderTargetBarriers">Render target barriers.</param>
		/// <param name="window">Window to add the barriers for.</param>
		virtual void ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
									              const Window* window) const = 0;
#else
		/// <summary>
		/// Add resource barriers (memory dependencies).
		/// </summary>
		/// <param name="renderTargetBarriers">Render target barriers.</param>
		virtual void ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// NVIDIA-Reflex Sleep/synchronize on the given window.
		/// </summary>
		virtual void ReflexSleep() const = 0;
		/// <summary>
		/// NVIDIA-Reflex latency marker.
		/// </summary>
		/// <param name="frame">Frame to set marker for. Must be unique for each frame!</param>
		/// <param name="marker">Enum value of the marker to set.</param>
		virtual void ReflexMarker(u32 frame, u32 marker) const = 0;
#ifdef NVIDIA_REFLEX_AVAILABLE
		/// <summary>
		/// Retrieve the latency report from NVIDIA-Reflex.
		/// </summary>
		/// <returns>Latency report.</returns>
		[[nodiscard]] virtual NVLL_VK_LATENCY_RESULT_PARAMS ReflexGetLatency() const = 0;
#endif /*NVIDIA_REFLEX_AVAILABLE*/
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Retrieve the renderer title.
		/// Example title: "[Vulkan 1.3.0]".
		/// </summary>
		/// <returns>Renderer title.</returns>
		[[nodiscard]] constexpr virtual std::string GetTitle() const noexcept = 0;

		/// <summary>
		/// Retrieve the currently used GPUs UUID.
		/// </summary>
		/// <returns>GPU's UUID.</returns>
		[[nodiscard]] virtual TRAP::Utils::UUID GetCurrentGPUUUID() const noexcept = 0;
		/// <summary>
		/// Retrieve the name of the currently used GPU.
		/// </summary>
		/// <returns>GPU's name.</returns>
		[[nodiscard]] virtual std::string GetCurrentGPUName() const noexcept = 0;
		/// <summary>
		/// Retrieve the vendor of the currently used GPU.
		/// </summary>
		/// <returns>GPU vendor.</returns>
		[[nodiscard]] virtual GPUVendor GetCurrentGPUVendor() const noexcept = 0;
		/// <summary>
		/// Retrieve a list of all supported GPUs.
		/// The list contains the GPUs name and UUID.
		/// </summary>
		/// <returns>List of all supported GPUs.</returns>
		[[nodiscard]] virtual std::vector<std::pair<std::string, TRAP::Utils::UUID>> GetAllGPUs() const = 0;

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Capture a screenshot of the last presented frame.
		/// </summary>
		/// <param name="window">Window to capture screenshot on.</param>
		/// <returns>Captured screenshot as TRAP::Image on success, nullptr otherwise.</returns>
		[[nodiscard]] virtual TRAP::Scope<TRAP::Image> CaptureScreenshot(const Window* window) const = 0;
#else
		/// <summary>
		/// Capture a screenshot of the last presented frame.
		/// </summary>
		/// <returns>Captured screenshot as TRAP::Image on success, nullptr otherwise.</returns>
		[[nodiscard]] virtual TRAP::Scope<TRAP::Image> CaptureScreenshot() const = 0;
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
		virtual void MSAAResolvePass(TRAP::Ref<RenderTarget> source, TRAP::Ref<RenderTarget> destination,
		                             CommandBuffer* cmd) const = 0;

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Scale image from internal resolution to the final output resolution.
		///
		/// Note: source and destination must be in ResourceState::RenderTarget.
		/// </summary>
		/// <param name="source">Source render target to resolve.</param>
		/// <param name="destination">Destination render target to resolve into.</param>
		/// <param name="window">Window to do the scaling pass on.</param>
		virtual void RenderScalePass(TRAP::Ref<RenderTarget> source,
									 TRAP::Ref<RenderTarget> destination,
		                             const Window* window) const = 0;
#else
		/// <summary>
		/// Scale image from internal resolution to the final output resolution.
		///
		/// Note: source and destination must be in ResourceState::RenderTarget.
		/// </summary>
		/// <param name="source">Source render target to resolve.</param>
		/// <param name="destination">Destination render target to resolve into.</param>
		virtual void RenderScalePass(TRAP::Ref<RenderTarget> source,
									 TRAP::Ref<RenderTarget> destination) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the latency mode.
		/// The actual latency mode may differ from the requested one so check
		/// the actual used mode with GetLatencyMode().
		/// Note: Only LatencyMode::Disabled is supported everywhere.
		///       Other LatencyModes are only available on Windows 10 or
		///       newer with NVIDIA hardware.
		/// </summary>
		/// <param name="mode">LatencyMode to set.</param>
		/// <param name="window">Window to set latency mode for.</param>
		virtual void SetLatencyMode(LatencyMode mode, const Window* window) = 0;
		/// <summary>
		/// Retrieve the currently used latency mode.
		/// Note: This may differ from the requested mode set with SetLatencyMode().
		/// </summary>
		/// <param name="window">Window to retrieve latency mode for.</param>
		/// <returns>Used latency mode.</returns>
		[[nodiscard]] virtual LatencyMode GetLatencyMode(const Window* window) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Retrieve the used descriptor pool.
		/// </summary>
		/// <returns>Descriptor pool.</returns>
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::DescriptorPool> GetDescriptorPool() noexcept;
		/// <summary>
		/// Retrieve the used graphics queue.
		/// </summary>
		/// <returns>Graphics queue.</returns>
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::Queue> GetGraphicsQueue() noexcept;
		/// <summary>
		/// Retrieve the used compute queue.
		/// </summary>
		/// <returns>Compute queue.</returns>
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::Queue> GetComputeQueue() noexcept;
		/// <summary>
		/// Retrieve the used transfer queue.
		/// </summary>
		/// <returns>Transfer queue.</returns>
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::Queue> GetTransferQueue() noexcept;
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the currently used graphics root signature of the given window.
		/// </summary>
		/// <param name="window">Window to retrieve the graphics root signature from.</param>
		/// <returns>Graphics root signature.</returns>
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::RootSignature> GetGraphicsRootSignature(const Window* window);
#else
		/// <summary>
		/// Retrieve the currently used graphics root signature.
		/// </summary>
		/// <returns>Graphics root signature.</returns>
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::RootSignature> GetGraphicsRootSignature();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the currently used internal render resolution of the given window.
		/// </summary>
		/// <param name="window">Window to get internal render resolution from.</param>
		/// <returns>Internal render resolution.</returns>
		[[nodiscard]] static TRAP::Math::Vec2ui GetInternalRenderResolution(const Window* window);
#else
		/// <summary>
		/// Retrieve the currently used internal render resolution.
		/// </summary>
		/// <returns>Internal render resolution.</returns>
		[[nodiscard]] static TRAP::Math::Vec2ui GetInternalRenderResolution();
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// <summary>
		/// Get the resolution of the render targets.
		/// </summary>
		/// <param name="width">Output: Width.</param>
		/// <param name="height">Output: Height.</param>
		virtual void GetResolution(u32& width, u32& height) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Start a render pass for the given window.
		///
		/// Note: This will bind the render target for the current frame again.
		/// </summary>
		/// <param name="window">Window to start render pass for.</param>
		static void StartRenderPass(const Window* window);
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
		/// Stop running render pass of the given window.
		/// </summary>
		/// <param name="window">Window to stop render pass on.</param>
		static void StopRenderPass(const Window* window);
#else
		/// <summary>
		/// Stop running render pass.
		/// </summary>
		static void StopRenderPass();
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Transition a texture from old layout to the new layout.
		/// The transition happens immediately and is guaranteed to be complete when the function returns.
		/// </summary>
		/// <param name="texture">Texture to transition layout.</param>
		/// <param name="oldLayout">Current resource state of the given texture.</param>
		/// <param name="newLayout">New resource state for the given texture.</param>
		/// <param name="queueType">Queue type on which to perform the transition. Default: Graphics.</param>
		static void Transition(const Ref<TRAP::Graphics::Texture>& texture,
							   TRAP::Graphics::RendererAPI::ResourceState oldLayout,
							   TRAP::Graphics::RendererAPI::ResourceState newLayout,
							   TRAP::Graphics::RendererAPI::QueueType queueType = QueueType::Graphics);
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the currently used anti aliasing method and the sample count.
		/// </summary>
		/// <param name="outAntiAliasing">Output: Used anti aliasing method.</param>
		/// <param name="outSampleCount">Output: Used sample count.</param>
		/// <param name="window">Window to get anti aliasing from.</param>
		static void GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount, const Window* window) noexcept;
#else
		/// <summary>
		/// Retrieve the currently used anti aliasing method and the sample count.
		/// </summary>
		/// <param name="outAntiAliasing">Output: Used anti aliasing method.</param>
		/// <param name="outSampleCount">Output: Used sample count.</param>
		static void GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount) noexcept;
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Set the anti aliasing method and the sample count.
		/// Use AntiAliasing::Off to disable anti aliasing.
		///
		/// Note: This won't affect the currently recorded frame.
		/// Note: A sample count of 1 is only valid if anti aliasing is disabled.
		/// </summary>
		/// <param name="antiAliasing">Anti aliasing method to use.</param>
		/// <param name="sampleCount">Sample count to use.</param>
		static void SetAntiAliasing(AntiAliasing antiAliasing, SampleCount sampleCount);

		/// <summary>
		/// Retrieve the currently used anisotropy level.
		/// </summary>
		/// <returns>Used anisotropy level.</returns>
		[[nodiscard]] static SampleCount GetAnisotropyLevel() noexcept;

		/// <summary>
		/// Set the anisotropy level.
		/// A value of SampleCount::One effectively disables anisotropic filtering.
		///
		/// Note: User created samplers need to be recreated in order to use the new anisotropy level.
		/// </summary>
		/// <param name="anisotropyLevel">Anisotropy level to use.</param>
		static void SetAnisotropyLevel(SampleCount anisotropyLevel);

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Notify the RendererAPI that the SwapChain needs to be resized.
		/// This function should be called inside FrameBufferResizeEvent callbacks.
		/// </summary>
		/// <param name="window">Window that needs an updated SwapChain.</param>
		static void ResizeSwapChain(const Window* window);
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the GPU side frame time for the graphics queue.
		/// </summary>
		/// <param name="window">Window to get frame time from.</param>
		/// <returns>GPU Graphics frame time in milliseconds.</returns>
		[[nodiscard]] static f32 GetGPUGraphicsFrameTime(const Window* window);
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
		[[nodiscard]] static f32 GetGPUComputeFrameTime(const Window* window);
#else
		/// <summary>
		/// Retrieve the GPU side frame time for the compute queue.
		/// </summary>
		/// <returns>GPU Compute frame time in milliseconds.</returns>
		[[nodiscard]] static f32 GetGPUComputeFrameTime();
#endif /*TRAP_HEADLESS_MODE*/

	//protected:
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve internal rendering data.
		/// </summary>
		/// <returns>Windows internal rendering data.</returns>
		[[nodiscard]] static PerViewportData& GetViewportData(const Window* window);
#else
		/// <summary>
		/// Retrieve internal rendering data.
		/// </summary>
		/// <returns>Internal rendering data.</returns>
		[[nodiscard]] static PerViewportData& GetViewportData();
#endif /*TRAP_HEADLESS_MODE*/

	public:
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Initialize the internal rendering data of the given window.
		/// </summary>
		/// <param name="window">Window to initialize the internal rendering data for.</param>
		/// <param name="VSyncEnabled">Whether to enable or disable VSync.</param>
		virtual void InitPerViewportData(Window* window, bool VSyncEnabled) const = 0;
#else
		/// <summary>
		/// Initialize the internal rendering data.
		/// </summary>
		/// <param name="width">Width for the viewport.</param>
		/// <param name="height">Height for the viewport.</param>
		virtual void InitPerViewportData(u32 width, u32 height) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Remove the internal rendering data of the given window.
		/// </summary>
		/// <param name="window">Window to remove the internal rendering data from.</param>
		virtual void RemovePerViewportData(const Window* window) const = 0;
#else
		/// <summary>
		/// Remove the internal rendering data.
		/// </summary>
		virtual void RemovePerViewportData() const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Wait for the GPU to idle.
		/// </summary>
		virtual void WaitIdle() const = 0;

		/// <summary>
		/// Check if the system is Vulkan API capable.
		///
		/// Note: The first call to this function will run the Vulkan capability tester.
		/// </summary>
		/// <returns>True if the system is Vulkan API capable, false otherwise.</returns>
		[[nodiscard]] static bool IsVulkanCapable();

		/// <summary>
		/// Enum bit flag for the different wave operations.
		/// </summary>
		enum class WaveOpsSupportFlags : u32
		{
			None = 0x0,
			Basic = BIT(0u),
			Vote = BIT(1u),
			Arithmetic = BIT(2u),
			Ballot = BIT(3u),
			Shuffle = BIT(4u),
			ShuffleRelative = BIT(5u),
			Clustered = BIT(6u),
			Quad = BIT(7u),
			PartitionedNV = BIT(8u),

			WAVE_OPS_SUPPORT_FLAG_ALL = 0x7FFFFFFF
		};

		/// <summary>
		/// Enum describing the different types of queues.
		/// </summary>
		enum class QueueType : u32
		{
			Graphics = 0,
			Transfer,
			Compute,

			MAX_QUEUE_TYPE
		};

		/// <summary>
		/// Enum describing queue flags.
		/// </summary>
		enum class QueueFlag : u32
		{
			None = 0x0,
			DisableGPUTimeout = BIT(0u), //Disable the GPU timeout for this command queue (DirectX 12)

			MAX_QUEUE_FLAG = 0xFFFFFFFF
		};

		/// <summary>
		/// Enum describing the different priorities for a queue (DirectX 12).
		/// </summary>
		enum class QueuePriority
		{
			Normal,
			High,
			GlobalRealtime,

			MAX_QUEUE_PRIORITY
		};

		/// <summary>
		/// Enum describing the status of a fence.
		/// </summary>
		enum class FenceStatus
		{
			Complete = 0,
			Incomplete,
			NotSubmitted
		};

		/// <summary>
		/// Enum describing the different types of load actions.
		/// </summary>
		enum class LoadActionType : u32
		{
			DontCare,
			Load,
			Clear,

			MAX_LOAD_ACTION_TYPE
		};

		/// <summary>
		/// Enum describing the different types of store actions.
		/// </summary>
		enum class StoreActionType : u32
		{
			//Store is the most common use case so keep that as default
			Store,
			DontCare,
			None,

			MAX_STORE_ACTION_TYPE
		};

		/// <summary>
		/// Enum describing the different types of cube textures.
		/// </summary>
		enum class TextureCubeType
		{
			NONE = 0,

			MultiFile,
			Cross,
			//TODO
			//Equirectangular
		};

		/// <summary>
		/// Enum describing the different sample counts.
		/// </summary>
		enum class SampleCount
		{
			One = BIT(0u),
			Two = BIT(1u),
			Four = BIT(2u),
			Eight = BIT(3u),
			Sixteen = BIT(4u)
		};

		/// <summary>
		/// Enum describing the different anti aliasing methods.
		/// </summary>
		enum class AntiAliasing
		{
			Off,
			MSAA
		};

		/// <summary>
		/// Enum describing the different GPU vendors.
		/// </summary>
		enum class GPUVendor : i32
		{
			Unknown     = -1,
			AMD         = 0x1002,
			ImgTec      = 0x1010,
			NVIDIA      = 0x10DE,
			ARM         = 0x13B5,
			Broadcom    = 0x14E4,
			Qualcomm    = 0x5143,
			Intel       = 0x8086,
			Apple       = 0x106B,
			Vivante     = 0x7A05,
			VeriSilicon = 0x1EB1,
			Kazan       = 0x10003,
			Codeplay    = 0x10004,
			Mesa        = 0x10005
		};

		/// <summary>
		/// Enum bit flags used by texture creation.
		/// </summary>
		enum class TextureCreationFlags : u32
		{
			//Default flag (Texture will use default allocation strategy decided by the API specific allocator)
			None = 0x0,
			//Texture will allocate its own memory (COMMITTED resource)
			OwnMemory = BIT(0u),
			//Texture will be allocated in memory which can be shared among multiple processes
			Export = BIT(1u),
			//Texture will be allocated in memory which can be shared among multiple GPUs
			ExportAdapter = BIT(2u),
			//Texture will be imported from a handle created in another process
			Import = BIT(3u),
			//Use ESRAM to store this texture
			ESRAM = BIT(4u),
			//Use on-tile memory to store this texture
			OnTile = BIT(5u),
			//Force 2D instead of automatically determining dimension based on width, height, depth
			Force2D = BIT(7u),
			//Force 3D instead of automatically determining dimension based on width, height, depth
			Force3D = BIT(8u),
			//Display target
			AllowDisplayTarget = BIT(9u),
			//Create an sRGB texture
			SRGB = BIT(10u),
			//Create a normal map texture
			NormalMap = BIT(11u),
			//Fast clear
			FastClear = BIT(12u),
			//Fragment mask
			FragMask = BIT(13u),
			//Create a storage texture
			Storage = BIT(14u)
		};

		/// <summary>
		/// Enum describing the state of a resource.
		/// </summary>
		enum class ResourceState : u32
		{
			Undefined = 0x0,
			VertexAndConstantBuffer = BIT(0u),
			IndexBuffer = BIT(1u),
			RenderTarget = BIT(2u),
			UnorderedAccess = BIT(3u),
			DepthWrite = BIT(4u),
			DepthRead = BIT(5u),
			NonPixelShaderResource = BIT(6u),
			PixelShaderResource = BIT(7u),
			ShaderResource = BIT(6u) | BIT(7u),
			StreamOut = BIT(8u),
			IndirectArgument = BIT(9u),
			CopyDestination = BIT(10u),
			CopySource = BIT(11u),
			GenericRead = (((((BIT(0u) | BIT(1u)) | BIT(6u)) | BIT(7u)) | BIT(9u)) | BIT(11u)),
			Present = BIT(12u),
			Common = BIT(13u),
			RayTracingAccelerationStructure = BIT(14u),
			ShadingRateSource = BIT(15u)
		};

		/// <summary>
		/// Enum describing the type of a descriptor.
		/// </summary>
		enum class DescriptorType : u32
		{
			Undefined = 0,
			Sampler = BIT(0u),
			//SRV read only Texture
			Texture = BIT(1u),
			//UAV Texture
			RWTexture = BIT(2u),
			//SRV read only Buffer
			Buffer = BIT(3u),
			BufferRaw = (Buffer | (Buffer << 1u)),
			//UAV Buffer
			RWBuffer = BIT(5u),
			RWBufferRaw = (RWBuffer | (RWBuffer << 1u)),
			//Uniform buffer
			UniformBuffer = BIT(7u),
			//Push constant / Root constant
			RootConstant = BIT(8u),
			//IA
			VertexBuffer = BIT(9u),
			IndexBuffer = BIT(10u),
			IndirectBuffer = BIT(11u),
			//Cubemap SRV
			TextureCube = (Texture | (IndirectBuffer << 1u)),
			//RTV / DSV per mip slice
			RenderTargetMipSlices = BIT(13u),
			//RTV / DSV per array slice
			RenderTargetArraySlices = BIT(14u),
			//RTV / DSV per depth sice
			RenderTargetDepthSlices = BIT(15u),
			RayTracing = BIT(16u),

			//Vulkan
			//Subpass input (descriptor type only available in Vulkan)
			InputAttachment = BIT(17u),
			TexelBuffer = BIT(18u),
			RWTexelBuffer = BIT(19u),
			CombinedImageSampler = BIT(20u),

			//Khronos ray tracing extension
			AccelerationStructure = BIT(21u),
			AccelerationStructureBuildInput = BIT(22u),
			ShaderDeviceAddress  = BIT(23u),
			ShaderBindingTable = BIT(24u)
		};

		//Choosing Memory Type

		/// <summary>
		/// Enum describing the different memory usage types for a resource.
		/// </summary>
		enum class ResourceMemoryUsage
		{
			//No intended memory usage specified
			Unknown = 0,
			//Memory will be used on device only, no need to be mapped on host.
			GPUOnly = 1,
			//Memory will be mapped on host.
			//Could be used for transfer to device.
			CPUOnly = 2,
			//Memory will be used for frequent (dynamic) updates from host and reads on device.
			CPUToGPU = 3,
			//Memory will be used for writing on device and readback on host.
			GPUToCPU = 4,

			RESOURCE_MEMORY_USAGE_COUNT,
			RESOURCE_MEMORY_USAGE_MAX_ENUM = 0x7FFFFFFF
		};

		/// <summary>
		/// Enum describing flags for the buffer creation.
		/// </summary>
		enum class BufferCreationFlags : u32
		{
			//Default flag (Buffer will use aliased memory, buffer will not be CPU accessible until MapBuffer
			//is called)
			None = 0x0,
			//Buffer will allocate its own memory (COMMITTED resource)
			OwnMemory = BIT(0u),
			//Buffer will be persistently mapped
			PersistentMap = BIT(1u),
			//Use ESRAM to store this buffer
			ESRAM = BIT(2u),
			//Flag to specify not to allocate descriptors for the resource
			NoDescriptorViewCreation = BIT(3u),

			//Vulkan
			HostVisible = 0x100,
			HostCoherent = 0x200
		};

		/// <summary>
		/// Enum describing the different types of buffers for clearing.
		/// </summary>
		enum class ClearBufferType : u32
		{
			NONE = 0,
			Color = BIT(0u),
			Depth = BIT(1u),
			Stencil = BIT(2u),
			Color_Depth = Color | Depth,
			Color_Stencil = Color | Stencil,
			Color_Depth_Stencil = Color | Stencil | Depth,
			Depth_Stencil = Depth | Stencil
		};

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Enum describing the different latency modes.
		/// </summary>
		enum class LatencyMode : u32
		{
			Disabled,
			Enabled,
			EnabledBoost
		};
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Enum describing the type of an indirect argument.
		/// </summary>
		enum class IndirectArgumentType
		{
			IndirectDraw,
			IndirectDrawIndex,
			IndirectDispatch,
			IndirectVertexBuffer,
			IndirectIndexBuffer,
			IndirectConstant,
			IndirectDescriptorTable,
			IndirectPipeline
		};

		/// <summary>
		/// Enum describing the update frequency of a descriptor.
		/// Note: The same update frequency order should be used by shaders.
		/// </summary>
		enum class DescriptorUpdateFrequency : u32
		{
			Static = 0,
			Dynamic
		};

		/// <summary>
		/// The maximum amount of descriptor sets that can be used.
		/// </summary>
		inline static constexpr u32 MaxDescriptorSets = 4;

		/// <summary>
		/// Enum describing the different types of filtering used by samplers.
		/// </summary>
		enum class FilterType
		{
			Nearest = 0,
			Linear
		};

		/// <summary>
		/// Enum describing the different modes of mip mapping used by samplers.
		/// </summary>
		enum class MipMapMode
		{
			Nearest = 0,
			Linear
		};

		/// <summary>
		/// Enum describing the different address modes used by samplers.
		/// </summary>
		enum class AddressMode
		{
			Mirror,
			Repeat,
			ClampToEdge,
			ClampToBorder
		};

		/// <summary>
		/// Enum describing the different compare modes used by depth/stencil testing and samplers.
		/// </summary>
		enum class CompareMode : u32
		{
			Never,
			Less,
			Equal,
			LessOrEqual,
			Greater,
			NotEqual,
			GreaterOrEqual,
			Always,

			MAX_COMPARE_MODES
		};

		/// <summary>
		/// Enum describing the different shader stages making up a shader.
		/// </summary>
		enum class ShaderStage : u32
		{
			None = 0,
			Vertex = BIT(0u),
			TessellationControl = BIT(1u),
			TessellationEvaluation = BIT(2u),
			Geometry = BIT(3u),
			Fragment = BIT(4u),
			Compute = BIT(5u),
			RayTracing = BIT(6u),

			AllGraphics = (static_cast<u32>(Vertex) | static_cast<u32>(TessellationControl) |
			               static_cast<u32>(TessellationEvaluation) | static_cast<u32>(Geometry) |
				           static_cast<u32>(Fragment)),
			Hull = TessellationControl,
			Domain = TessellationEvaluation,

			SHADER_STAGE_COUNT = 7
		};

		/// <summary>
		/// Enum describing the different flags for root signature creation.
		/// </summary>
		enum class RootSignatureFlags : u32
		{
			//Default flag
			None = 0,
			//Local root signature used mainly in raytracing shaders
			Local = BIT(0u)
		};

		/// <summary>
		/// Enum describing the different pipeline types.
		/// </summary>
		enum class PipelineType : u32
		{
			Undefined = 0,
			Compute,
			Graphics,
			RayTracing,

			PIPELINE_TYPE_COUNT
		};

		/// <summary>
		/// Enum describing the different data types used by index buffers.
		/// </summary>
		enum class IndexType
		{
			UInt32 = 0,
			UInt16
		};

		/// <summary>
		/// Enum describing the different blend constants used for blending.
		/// </summary>
		enum class BlendConstant : u32
		{
			Zero = 0,
			One,
			SrcColor,
			OneMinusSrcColor,
			DstColor,
			OneMinusDstColor,
			SrcAlpha,
			OneMinusSrcAlpha,
			DstAlpha,
			OneMinusDstAlpha,
			SrcAlphaSaturate,
			BlendFactor,
			OneMinusBlendFactor,

			MAX_BLEND_CONSTANTS
		};

		/// <summary>
		/// Enum describing the different blend modes/functions used for blending.
		/// </summary>
		enum class BlendMode : u32
		{
			Add,
			Subtract,
			ReverseSubtract,
			Min,
			Max,

			MAX_BLEND_MODES
		};

		/// <summary>
		/// Enum describing which render target to affect with the blend state.
		/// </summary>
		enum class BlendStateTargets : u32
		{
			BlendStateTarget0 = BIT(0u),
			BlendStateTarget1 = BIT(1u),
			BlendStateTarget2 = BIT(2u),
			BlendStateTarget3 = BIT(3u),
			BlendStateTarget4 = BIT(4u),
			BlendStateTarget5 = BIT(5u),
			BlendStateTarget6 = BIT(6u),
			BlendStateTarget7 = BIT(7u),

			BlendStateTargetAll = 0xFF,
		};

		/// <summary>
		/// Enum describing the different stencil/depth operations.
		/// </summary>
		enum class StencilOp : u32
		{
			Keep,
			SetZero,
			Replace,
			Invert,
			Increment,
			Decrement,
			IncrementSaturation,
			DecrementSaturation,

			MAX_STENCIL_OPS
		};

		/// <summary>
		/// Enum describing the different cull modes.
		/// </summary>
		enum class CullMode : u32
		{
			None = 0,
			Back,
			Front,
			Both,

			MAX_CULL_MODES
		};

		/// <summary>
		/// Enum describing the different front face winding orders.
		/// </summary>
		enum class FrontFace : u32
		{
			CounterClockwise = 0,
			Clockwise
		};

		/// <summary>
		/// Enum describing the different fill modes.
		/// </summary>
		enum class FillMode : u32
		{
			Solid,
			Line,
			Point,

			MAX_FILL_MODES
		};

		/// <summary>
		/// Enum describing flags for pipeline cache creation.
		/// </summary>
		enum class PipelineCacheFlags : u32
		{
			None = 0x0,
			ExternallySynchronized = BIT(0u)
		};

		/// <summary>
		/// Enum describing the different primitive topologies.
		/// </summary>
		enum class PrimitiveTopology
		{
			PointList = 0,
			LineList,
			LineStrip,
			TriangleList,
			TriangleStrip,
			PatchList,

			PRIMITIVE_TOPOLOGY_COUNT
		};

		/// <summary>
		/// Enum describing the different vertex attribute rates.
		/// </summary>
		enum class VertexAttributeRate
		{
			Vertex = 0,
			Instance = 1,

			VERTEX_ATTRIBUTE_RATE_COUNT
		};

		/// <summary>
		/// Enum describing the different types of queries.
		/// </summary>
		enum class QueryType
		{
			Timestamp = 0,
			PipelineStatistics,
			Occlusion,

			QUERY_TYPE_COUNT
		};

		/// <summary>
		/// Enum describing the different presentation statuses.
		/// </summary>
		enum class PresentStatus
		{
			Success = 0,
			DeviceReset = 1,
			Failed = 2,
			OutOfDate = 3
		};

		/// <summary>
		/// Enum describing the different sampler ranges used by YUV conversion samplers.
		/// </summary>
		enum class SamplerRange
		{
			Full = 0,
			Narrow = 1
		};

		/// <summary>
		/// Enum describing the different sampler models used by YUV conversion samplers.
		/// </summary>
		enum class SamplerModelConversion
		{
			RGBIdentity = 0,
			YCBCRIdentity = 1,
			YCBCR709 = 2,
			YCBCR601 = 3,
			YCBCR2020 = 4
		};

		/// <summary>
		/// Enum describing the different sample locations used by YUV conversion samplers.
		/// </summary>
		enum class SampleLocation
		{
			Cosited = 0,
			Midpoint = 1
		};

		/// <summary>
		/// Enum describing the shading rates used by fragment/pixel shaders.
		/// </summary>
		enum class ShadingRate : u32
		{
			NotSupported = 0x0,
			Full = BIT(0u),
			Half = BIT(1u),
			Quarter = BIT(2u),
			Eighth = BIT(3u),
			OneXTwo = BIT(4u),
			TwoXOne = BIT(5u),
			TwoXFour = BIT(6u),
			FourXTwo = BIT(7u)
		};

		/// <summary>
		/// Enum describing how to combine different shading rates.
		/// </summary>
		enum class ShadingRateCombiner : u32
		{
			Passthrough = BIT(0u),
			Override = BIT(1u),
			Min = BIT(2u),
			Max = BIT(3u),
			Sum = BIT(4u)
		};

		/// <summary>
		/// Enum describing the shading rate capabilities supported by the GPU.
		/// </summary>
		enum class ShadingRateCaps : u32
		{
			NotSupported = 0x0,
			PerDraw = BIT(0u),
			PerTile = BIT(1u),

			//Vulkan only
			PerPrimitive = BIT(2u)
		};


		struct Color
		{
			//Doubles are used because they are large enough to precisely
			//hold 32-bit signed/unsigned integers and single-precision floats.

			f64 Red;
			f64 Green;
			f64 Blue;
			f64 Alpha;
		};

		struct DepthStencil
		{
			f32 Depth;
			u32 Stencil;
		};

		using ClearValue = std::variant<Color, DepthStencil>;

		/// <summary>
		/// Description of a subresource.
		/// Used to update a existing resource.
		/// </summary>
		struct SubresourceDataDesc
		{
			u64 SrcOffset;
			u32 MipLevel;
			u32 ArrayLayer;

			u32 RowPitch;
			u32 SlicePitch;
		};

		/// <summary>
		/// Description of a render target.
		/// </summary>
		struct RenderTargetDesc
		{
			//Texture creation flags (decides memory allocation strategy, sharing access, ...)
			TextureCreationFlags Flags{};
			//Width
			u32 Width{};
			//Height
			u32 Height{};
			//Depth (should be 1 if type is not TextureType::TextureCube)
			u32 Depth{};
			//Texture array size (should be 1 if texture is not a texture array or cubemap)
			u32 ArraySize{};
			//Number of mip levels
			u32 MipLevels{};
			//Multisample anti-aliasing (MSAA)
			TRAP::Graphics::RendererAPI::SampleCount SampleCount{};
			//Internal image format
			TRAP::Graphics::API::ImageFormat Format{};
			//What state will the texture get created in
			ResourceState StartState{};
			//Optimized clear value (recommended to use the same value when clearing the renderTarget)
			RendererAPI::ClearValue ClearValue{};
			//The image quality level.
			//The higher the quality, the lower the performance.
			//The valid range is between 0 and the value appropriate for SampleCount
			u32 SampleQuality{};
			//Descriptor creation
			DescriptorType Descriptors{};
			//Debug name used in GPU profile
			std::string Name{};

			void* NativeHandle{};
		};

		/// <summary>
		/// Description of a texture.
		/// </summary>
		struct TextureDesc
		{
			//Texture creation flags (decides memory allocation strategy, sharing access, ...)
			TextureCreationFlags Flags{};
			//Width
			u32 Width = 1;
			//Height;
			u32 Height = 1;
			//Depth (should be 1 if Type is not TextureType::TextureCube)
			u32 Depth = 1;
			//Texture array size (should be 1 if texture is not a texture array or cubemap)
			u32 ArraySize = 1;
			//Number of mip levels
			u32 MipLevels = 1;
			//Number of multisamples per pixel (currently Textures created with Usage TextureUsage::SampledImage
			//only support One).
			TRAP::Graphics::RendererAPI::SampleCount SampleCount{};
			//The image quality level.
			//The higher the quality, the lower the performance.
			//The valid range is between 0 and the value appropriate for SampleCount.
			u32 SampleQuality{};
			//Image format
			TRAP::Graphics::API::ImageFormat Format{};
			//Optimized clear value (recommended to use the same value when clearing the renderTarget)
			RendererAPI::ClearValue ClearValue{};
			//What state will the texture get created in
			ResourceState StartState{};
			//Descriptor creation
			DescriptorType Descriptors{};
			//Pointer to native texture handle if the texture does not own underlying resource
			void* NativeHandle{};
			//Debug name used in GPU profile
			std::string Name{};

			//Optional Vulkan YCbCr conversion info
			::VkSamplerYcbcrConversionInfo* VkSamplerYcbcrConversionInfo{};
		};

		/// <summary>
		/// Description for a texture load.
		/// </summary>
		struct TextureLoadDesc
		{
			//Target to load texture info into.
			TRAP::Graphics::Texture* Texture{};
			//Load empty texture
			TextureDesc* Desc{};
			//Filepath with extension.
			std::array<std::filesystem::path, 6> Filepaths;
			//Loaded images (used to load in memory images, if set Filepaths are ignored)
			std::array<const Image*, 6> Images{};
			//Following is ignored if Desc != nullptr.
			//Desc->Flags will be considered instead.
			TextureCreationFlags CreationFlag = TextureCreationFlags::None;
			//Is Texture cubemap?
			bool IsCubemap{};
			//Type of cubemap texture
			TextureCubeType Type = TextureCubeType::NONE;
		};

		/// <summary>
		/// Description of a buffer.
		/// </summary>
		struct BufferDesc
		{
			//Size of the buffer (in bytes)
			u64 Size{};
			//Alignment
			u32 Alignment{};
			//Decides which heap buffer will be used (default, upload, readback)
			ResourceMemoryUsage MemoryUsage{};
			//Creation flags of the buffer
			BufferCreationFlags Flags{};
			//What type of queue the buffer is owned by
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
			//What state will the buffer get created in
			ResourceState StartState{};
			//Index of the first element accessible by the SRV/UAV
			u64 FirstElement{};
			//Number of elements in the buffer
			u64 ElementCount{};
			//Size of each element (in bytes) in the buffer
			u64 StructStride{};
			//ICB draw type
			IndirectArgumentType ICBDrawType{};
			//ICB max vertex buffers slots count
			u32 ICBMaxVertexBufferBind{};
			//ICB max fragment buffers slots count
			u32 ICBMaxFragmentBufferBind{};
			//Set this to specify a counter buffer for this buffer
			//Buffer* CounterBuffer;
			//Format of the buffer
			TRAP::Graphics::API::ImageFormat Format{};
			//Flags specifying the suitable usage of this buffer (Uniform Buffer, Vertex Buffer, Index Buffer, ...)
			DescriptorType Descriptors{};
			//Debug name used in GPU-profile
			std::string Name{};
		};

		/// <summary>
		/// Description of a sampler.
		/// </summary>
		struct SamplerDesc
		{
			//Minification filter
			FilterType MinFilter{};
			//Magnification filter
			FilterType MagFilter{};
			//Mip mapping mode.
			TRAP::Graphics::RendererAPI::MipMapMode MipMapMode{};
			//Address mode for U coordinate
			AddressMode AddressU{};
			//Address mode for V coordinate
			AddressMode AddressV{};
			//Address mode for W coordinate
			AddressMode AddressW{};
			//Mip lod bias (offset from the calculated mip map level)
			f32 MipLodBias{};
			//Whether to use the given lod range or not
			bool SetLodRange{};
			//Minimum lod value
			f32 MinLod{};
			//Maximum lod value
			f32 MaxLod{};
			//Enable/Disable Anisotropic filtering
			bool EnableAnisotropy = true;
			//Override Anisotropic filtering level (0.0f = auto)
			f32 OverrideAnisotropyLevel{};
			//Comparison function compares sampled data against existing sampled data
			CompareMode CompareFunc{};

			/// <summary>
			/// Description of YCbCr(YUV) conversion sampler.
			/// </summary>
			struct SamplerConversionDesc
			{
				//YCbCr(YUV) image format
				TRAP::Graphics::API::ImageFormat Format;
				//Conversion model
				SamplerModelConversion Model;
				//Range
				SamplerRange Range;
				//X offset for chroma
				SampleLocation ChromaOffsetX;
				//Y offset for chroma
				SampleLocation ChromaOffsetY;
				//Chroma filter
				FilterType ChromaFilter;
				//Explicitly force the reconstruction
				bool ForceExplicitReconstruction;

				[[nodiscard]] constexpr bool operator==(const SamplerConversionDesc& other) const noexcept = default;
				[[nodiscard]] constexpr bool operator!=(const SamplerConversionDesc& other) const noexcept = default;
			} SamplerConversionDesc{};

			[[nodiscard]] constexpr bool operator==(const SamplerDesc& other) const noexcept = default;
			[[nodiscard]] constexpr bool operator!=(const SamplerDesc& other) const noexcept = default;
		};

		/// <summary>
		/// Description of a binary shader stage.
		/// </summary>
		struct BinaryShaderStageDesc
		{
			//Shader data
			std::vector<u32> ByteCode{};
			//Entry point of shader
			std::string EntryPoint{};
		};

		/// <summary>
		/// Description of a binary shader.
		/// </summary>
		struct BinaryShaderDesc
		{
			//Shader stages contained in the binary shader
			ShaderStage Stages{};
			//Vertex shader stage data
			BinaryShaderStageDesc Vertex{};
			//Fragment shader stage data
			BinaryShaderStageDesc Fragment{};
			//Geometry shader stage data
			BinaryShaderStageDesc Geometry{};
			//TessellationContol shader stage data
			BinaryShaderStageDesc TessellationControl{};
			//TessellationEvaluation shader stage data
			BinaryShaderStageDesc TessellationEvaluation{};
			//Compute shader stage data
			BinaryShaderStageDesc Compute{};
		};

		/// <summary>
		/// Description of a root signature.
		/// </summary>
		struct RootSignatureDesc
		{
			//Shaders to manage
			std::vector<Shader*> Shaders{};
			//Max number of bindless texture (DirectX 12)
			u32 MaxBindlessTextures{};
			//Static sampler names
			std::vector<const char*> StaticSamplerNames{};
			//Static samplers
			std::vector<TRAP::Ref<Sampler>> StaticSamplers{};
			//Root signature creation flags
			RootSignatureFlags Flags{};
		};

		/// <summary>
		/// Struct containing information about a descriptor.
		/// </summary>
		struct DescriptorInfo
		{
			//Name of descriptor
			std::string Name{};
			//Type of descriptor
			DescriptorType Type{};
			//Dimension of texture
			API::ShaderReflection::TextureDimension Dimension{};
			//Is root descriptor?
			bool RootDescriptor{};
			//Descriptor set
			u32 Set{};
			//Size of descriptor
			u32 Size{};
			//Index in the descriptor set
			u32 IndexInParent{};
			//Index of the handle in the root signature
			u32 HandleIndex{};

			//Descriptor type (Vulkan)
			VkDescriptorType VkType{};
			//Resource binding location
			u32 Reg{};
			//Index into the root descriptor.
			u32 RootDescriptorIndex{};
			//Shader stages using the descriptor (Vulkan)
			u32 VkStages{};
		};

		/// <summary>
		/// Description of a descriptor set.
		/// </summary>
		struct DescriptorSetDesc
		{
			//Root signature to use
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			//Descriptor set index
			u32 Set;
			//Max amount of sets to allocate
			u32 MaxSets{};
		};

		/// <summary>
		/// Description of a command pool.
		/// </summary>
		struct CommandPoolDesc
		{
			//Queue to be used by the command pool
			TRAP::Ref<TRAP::Graphics::Queue> Queue;
			//Indicate that the allocated command buffers will be short lived
			bool Transient;
		};

		/// <summary>
		/// Description of a queue.
		/// </summary>
		struct QueueDesc
		{
			//Type of queue
			QueueType Type{};
			//Queue creation flags
			QueueFlag Flag{};
			//Priority for the queue (DirectX 12)
			QueuePriority Priority{};
		};

		/// <summary>
		/// Read range used for buffer mapping.
		/// </summary>
		struct ReadRange
		{
			//Offset from the start of the buffer
			u64 Offset{};
			//Range to read (DirectX 12)
			u64 Range{};
		};

		/// <summary>
		/// Description for a queue submission.
		/// </summary>
		struct QueueSubmitDesc
		{
			//Command buffers to submit
			std::vector<CommandBuffer*> Cmds{};
			//Fence to signal
			TRAP::Ref<Fence> SignalFence{};
			//Semaphores to wait on
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
			//Semaphores to signal
			std::vector<TRAP::Ref<Semaphore>> SignalSemaphores{};
		};

		/// <summary>
		/// Description of a blend state.
		/// </summary>
		struct BlendStateDesc
		{
			//Source blend factor per render target.
			std::array<BlendConstant, 8> SrcFactors{};
			//Destination blend factor per render target.
			std::array<BlendConstant, 8> DstFactors{};
			//Source alpha blend factor per render target.
			std::array<BlendConstant, 8> SrcAlphaFactors{};
			//Destination alpha blend factor per render target.
			std::array<BlendConstant, 8> DstAlphaFactors{};
			//Blend mode per render target.
			std::array<BlendMode, 8> BlendModes{};
			//Alpha blend mode per render target.
			std::array<BlendMode, 8> BlendAlphaModes{};
			//Write mask per render target.
			std::array<i32, 8> Masks{};
			//Mask that identifies the render targets affected by the blend state.
			BlendStateTargets RenderTargetMask{};
			//Set whether each render target has an unique blend function.
			//When false the blend function in slot 0 will be used for all render targets.
			bool IndependentBlend{};
		};

		/// <summary>
		/// Description of a depth state.
		/// </summary>
		struct DepthStateDesc
		{
			//Enable depth testing?
			bool DepthTest{};
			//Enable depth writing?
			bool DepthWrite{};
			//Depth comparison function
			CompareMode DepthFunc{};
			//Enable stencil testing?
			bool StencilTest{};
			//Stencil read mask
			u8 StencilReadMask{};
			//Stencil write mask
			u8 StencilWriteMask{};
			//Function to use on the front for stencil testing
			CompareMode StencilFrontFunc{};
			//Stencil action to perform when stencil testing fails
			StencilOp StencilFrontFail{};
			//Depth action to perform when depth testing fails
			StencilOp DepthFrontFail{};
			//Stencil action to perform when passed
			StencilOp StencilFrontPass{};
			//Function to use on the back for stencil testing
			CompareMode StencilBackFunc{};
			//Stencil action to perform when stencil testing fails
			StencilOp StencilBackFail{};
			//Depth action to perform when depth testing fails
			StencilOp DepthBackFail{};
			//Stencil action to perform when passed
			StencilOp StencilBackPass{};
		};

		/// <summary>
		/// Description of a rasterizer state.
		/// </summary>
		struct RasterizerStateDesc
		{
			//Cull mode
			TRAP::Graphics::RendererAPI::CullMode CullMode{};
			//Depth bias
			i32 DepthBias{};
			//Depth bias slope factor.
			f32 SlopeScaledDepthBias{};
			//Fill mode
			TRAP::Graphics::RendererAPI::FillMode FillMode{};
			//Front face winding order
			TRAP::Graphics::RendererAPI::FrontFace FrontFace{};
			//Clamp depth to min/max depth
			bool DepthClampEnable{};
		};

		/// <summary>
		/// Description of a pipeline cache.
		/// </summary>
		struct PipelineCacheDesc
		{
			//Pipeline data
			std::vector<u8> Data{};
			//Pipeline cache creation flags
			PipelineCacheFlags Flags{};
		};

		/// <summary>
		/// Description of a pipeline cache to load.
		/// </summary>
		struct PipelineCacheLoadDesc
		{
			//Path to pipeline cache
			std::filesystem::path Path{};
			//Pipeline cache creation flags
			PipelineCacheFlags Flags{};
		};

		/// <summary>
		/// Description of a compute pipeline.
		/// </summary>
		struct ComputePipelineDesc
		{
			//Shader to use
			Shader* ShaderProgram{};
			//Root signature to use
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
		};

		/// <summary>
		/// Description of a single vertex attribute.
		/// </summary>
		struct VertexAttribute
		{
			//Attribute format
			TRAP::Graphics::API::ImageFormat Format{};
			//Attribute binding
			u32 Binding{};
			//Attribute location
			u32 Location{};
			//Attribute offset
			u32 Offset{};
			//Attribute rate
			VertexAttributeRate Rate{};
		};

		/// <summary>
		/// Description of a vertex layout.
		/// </summary>
		struct VertexLayout
		{
			//Amount of attributes in the layout
			u32 AttributeCount{};
			//Attributes
			std::array<VertexAttribute, 15> Attributes{};
		};

		/// <summary>
		/// Description of a graphics pipeline.
		/// </summary>
		struct GraphicsPipelineDesc
		{
			//Shader to use
			Shader* ShaderProgram{};
			//Root signature to use
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			//Vertex layou to use
			TRAP::Ref<TRAP::Graphics::RendererAPI::VertexLayout> VertexLayout{};
			//Blend state to use
			TRAP::Ref<BlendStateDesc> BlendState{};
			//Depth state to use
			TRAP::Ref<DepthStateDesc> DepthState{};
			//Rasterizer state to use
			TRAP::Ref<RasterizerStateDesc> RasterizerState{};
			//Color formats for the render targets
			std::vector<TRAP::Graphics::API::ImageFormat> ColorFormats{};
			//How many render targets to use
			u32 RenderTargetCount{};
			//Sample count to use by render targets
			TRAP::Graphics::RendererAPI::SampleCount SampleCount{};
			//Sample quality to use by render targets (DirectX 12)
			u32 SampleQuality{};
			//Depth/Stencil image format to use
			TRAP::Graphics::API::ImageFormat DepthStencilFormat{};
			//Primitive topology to use
			TRAP::Graphics::RendererAPI::PrimitiveTopology PrimitiveTopology{};
			//Shading rate to use (only if supported)
			TRAP::Graphics::RendererAPI::ShadingRate ShadingRate{};
			//Shading rate combiners to use (only if supported)
			std::array<TRAP::Graphics::RendererAPI::ShadingRateCombiner, 2> ShadingRateCombiners{};
			//Shading rate texture to use (only if ShadingRateCaps::PerTile is supported, disables fixed ShadingRate)
			TRAP::Ref<TRAP::Graphics::RenderTarget> ShadingRateTexture{};
		};

		/// <summary>
		/// Description of a RayTracing pipeline.
		/// </summary>
		struct RayTracingPipelineDesc
		{
		};

		/// <summary>
		/// Description of a pipeline.
		/// </summary>
		struct PipelineDesc
		{
			//Type of pipeline
			PipelineType Type{};
			//Pipeline
			std::variant<ComputePipelineDesc,
			             GraphicsPipelineDesc,
						 RayTracingPipelineDesc> Pipeline{GraphicsPipelineDesc()};
			//Pipeline cache
			TRAP::Ref<PipelineCache> Cache{};
			//Pipeline extensions (DirectX 12)
			void* PipelineExtensions{};
			//Pipeline extensions count (DirectX 12)
			u32 PipelineExtensionCount{};

			//Name for the pipeline
			std::string Name{};
		};

		/// <summary>
		/// Description of a query pool.
		/// </summary>
		struct QueryPoolDesc
		{
			//Type of query to hold
			QueryType Type{};
			//Max amount of queries to manage
			u32 QueryCount{};
		};

		/// <summary>
		/// Description of a query.
		/// </summary>
		struct QueryDesc
		{
			//Index of the query into the query pool
			u32 Index{};
		};

		/// <summary>
		/// Struct holding indirect draw arguments.
		/// </summary>
		struct IndirectDrawArguments
		{
			//How many vertices to draw
			u32 VertexCount{};
			//How many instances to draw
			u32 InstanceCount{};
			//Vertex offset to start at
			u32 StartVertex{};
			//Instance offset to start at
			u32 StartInstance{};
		};

		/// <summary>
		/// Struct holding indirect indexed draw arguments.
		/// </summary>
		struct IndirectDrawIndexArguments
		{
			//How many indices to draw
			u32 IndexCount{};
			//How many instances to draw
			u32 InstanceCount{};
			//Index buffer offset to start at
			u32 StartIndex{};
			//Vertex offset to start at
			u32 VertexOffset{};
			//Instance offset to start at
			u32 StartInstance{};
		};

		/// <summary>
		/// Struct holding indirect dispatch arguments.
		/// </summary>
		struct IndirectDispatchArguments
		{
			//X work group size
			u32 GroupCountX{};
			//Y work group size
			u32 GroupCountY{};
			//Z work group size
			u32 GroupCountZ{};
		};

		/// <summary>
		/// Struct holding indirect descriptor argument.
		/// </summary>
		struct IndirectArgumentDescriptor
		{
			//Type of indirect argument
			IndirectArgumentType Type{};
			//Name of descriptor
			std::string Name{};
			//Index of descriptor
			u32 Index{};
		};

		/// <summary>
		/// Description of a command signature.
		/// </summary>
		struct CommandSignatureDesc
		{
			//Root signature.
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			//Indirect argument descriptions
			std::vector<IndirectArgumentDescriptor> ArgDescs{};
			//Set to true if indirect argument struct should not be aligned to 16 bytes
			bool Packed{};
		};

		/// <summary>
		/// Description of a swapchain.
		/// </summary>
		struct SwapChainDesc
		{
			//Window handle
			TRAP::Window* Window{};
			//Queues which should be allowed to present
			std::vector<TRAP::Ref<Queue>> PresentQueues{};
			//Number of back buffers in this swapchain
			u32 ImageCount{};
			//Width of the swapchain
			u32 Width{};
			//Height of the swapchain
			u32 Height{};
			//Color format of the swapchain
			TRAP::Graphics::API::ImageFormat ColorFormat{};
			//Clear value
			RendererAPI::ClearValue ClearValue{};
			//Set whether swapchain will be presented using VSync
			bool EnableVSync{};
			//Anti aliasing sample count (1 = no AA)
			RendererAPI::SampleCount SampleCount = SampleCount::One;

			//Old swapchain, may aid in resource reusage (Vulkan)
			SwapChain* OldSwapChain = nullptr;
		};

		/// <summary>
		/// Struct holding a render target barrier.
		/// </summary>
		struct RenderTargetBarrier
		{
			//Render target
			TRAP::Ref<TRAP::Graphics::RenderTarget> RenderTarget{};
			//Current resource state of the render target
			ResourceState CurrentState{};
			//Target resource state of the render target
			ResourceState NewState{};
			//Starts a barrier transition in a new state, putting a resource in a temporary no-access condition (DirectX 12)
			bool BeginOnly{};
			//This barrier completes a transition, setting a new state and restoring active access to a resource (DirectX 12)
			bool EndOnly{};
			//Acquire ownershop of the queue family
			bool Acquire{};
			//Release ownership of the queue family
			bool Release{};
			//Type of queue to acquire or release
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
			//Specify whether following barrier targets particular subresource
			bool SubresourceBarrier{};

			//Following values are ignored if SubresourceBarrier is false

			//Mip level
			u8 MipLevel{};
			//Array layer
			u16 ArrayLayer{};
		};

		/// <summary>
		/// Struct holding a buffer barrier.
		/// </summary>
		struct BufferBarrier
		{
			//Buffer
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer{};
			//Current resource state of the buffer
			ResourceState CurrentState{};
			//Target resource state of the buffer
			ResourceState NewState{};
			//Starts a barrier transition in a new state, putting a resource in a temporary no-access condition (DirectX 12)
			bool BeginOnly{};
			//This barrier completes a transition, setting a new state and restoring active access to a resource (DirectX 12)
			bool EndOnly{};
			//Acquire ownership of the queue family
			bool Acquire{};
			//Release ownership of the queue family
			bool Release{};
			//Type of queue to acquire or release
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
		};

		/// <summary>
		/// Struct holding a texture barrier.
		/// </summary>
		struct TextureBarrier
		{
			//Texture
			TRAP::Graphics::Texture* Texture{};
			//Current resource state of the texture
			ResourceState CurrentState{};
			//Target resource state of the texture
			ResourceState NewState{};
			//Starts a barrier transition in a new state, putting a resource in a temporary no-access condition (DirectX 12)
			bool BeginOnly{};
			//This barrier completes a transition, setting a new state and restoring active access to a resource (DirectX 12)
			bool EndOnly{};
			//Acquire ownership of the queue family
			bool Acquire{};
			//Release ownership of the queue family
			bool Release{};
			//Type of queue to acquire or release
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
			//Specify whether the following barrier targets particular subresource
			bool SubresourceBarrier{};

			//Following values are ignored if SubresourceBarrier is false

			//Mip level
			u8 MipLevel{};
			//Array layer
			u16 ArrayLayer{};
		};

		/// <summary>
		/// Struct holding a data of a descriptor.
		/// </summary>
		struct DescriptorData
		{
			//User can either set name of descriptor or index (index in RootSignature->Descriptors array)
			//Name of descriptor
			std::string Name{};
			/// <summary>
			/// Range(s) to bind (buffer, offset, size)
			/// </summary>
			struct BufferOffset
			{
				//Offset to bind the buffer descriptor
				std::vector<u64> Offsets{};
				//Size of the buffer to bind
				std::vector<u64> Sizes{};
			};

			/// <summary>
			/// Descriptor set buffer extraction options
			/// </summary>
			struct DescriptorSetExtraction
			{
				//Index of the descriptor set to extract
				u32 DescriptorSetBufferIndex{};
				Ref<Shader> DescriptorSetShader{};
				ShaderStage DescriptorSetShaderStage{};
			};

			struct TextureSlice
			{
				//When binding UAV, control the mip slice to bind for UAV (example - generating mipmaps in a compute shader)
				u32 UAVMipSlice{};
				//Binds entire mip chain as array of UAV
				bool BindMipChain{};
			};
			std::variant<BufferOffset, DescriptorSetExtraction, TextureSlice, bool> Offset{false};
			//Array of resources containing descriptor handles or constant to be used in ring buffer memory
			//DescriptorRange can hold only one resource type array
			//Can be one of the following:
			//Array of texture descriptors (SRV and UAV textures)
			//Array of sampler descriptors
			//Array of buffer descriptors (SRV, UAV and CBV buffers)
			//Array of pipeline descriptors
			//DescriptorSet buffer extraction
			//Custom binding (RayTracing acceleration structure ...)
			std::variant<std::vector<Ref<TRAP::Graphics::Texture>>, std::vector<Sampler*>,
				std::vector<Buffer*>, std::vector<Pipeline*>,
				std::vector<DescriptorSet*>> Resource{std::vector<Ref<TRAP::Graphics::Texture>>()}; //TODO RayTracing acceleration structure

			//Number of resources in the descriptor(applies to array of textures, buffers, ...)
			u32 Count{};
			//Index into RootSignature->Descriptors array
			u32 Index = std::numeric_limits<u32>::max();
		};

		/// <summary>
		/// Description of a queue presentation.
		/// </summary>
		struct QueuePresentDesc
		{
			//Swapchain to preesent
			TRAP::Ref<TRAP::Graphics::SwapChain> SwapChain{};
			//Semaphores to wait on before presenting
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
			//Render target of the swapchain to present
			u32 Index{};
		};

		/// <summary>
		/// Description of actions to perform on load.
		/// </summary>
		struct LoadActionsDesc
		{
			//Action to perform on the color attachment(s) on load.
			std::array<LoadActionType, 8> LoadActionsColor{};
			//Action to perform on the depth attachment on load.
			LoadActionType LoadActionDepth{};
			//Action to perform on the stencil attachment on load.
			LoadActionType LoadActionStencil{};
			//Clear color(s)
			std::array<RendererAPI::Color, 8> ClearColorValues{};
			//Clear depth/stencil value
			RendererAPI::DepthStencil ClearDepthStencil{};
			//Action to perform on the color attachment(s) on store.
			std::array<StoreActionType, 8> StoreActionsColor{};
			//Action to perform on the depth attachment on store.
			StoreActionType StoreActionDepth{};
			//Action to perform on the stencil attachment on store.
			StoreActionType StoreActionStencil{};
		};

		/// <summary>
		/// Map resolving a name to its descriptor index in a root signature.
		/// </summary>
		using DescriptorIndexMap = std::unordered_map<std::string, u32>;

		/// <summary>
		/// Struct holding data about a mapped memory range.
		/// </summary>
		struct MappedMemoryRange
		{
			//Pointer to mapped buffer data with offset already applied to
			u8* Data = nullptr;
			//Mapped buffer
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer = nullptr;
			//Offset in the buffer to start at
			u64 Offset = 0;
			//Mapped size
			u64 Size = 0;
			u32 Flags = 0;
		};

		/// <summary>
		/// Description for a texture update.
		/// Note: Only use for procedural textures which are created on CPU (noise textures, font texture, ...)
		/// </summary>
		struct TextureUpdateDesc
		{
			//Texture to update
			TRAP::Graphics::Texture* Texture = nullptr;
			//Mip level to update
			u32 MipLevel = 0;
			//Array layer to update
			u32 ArrayLayer = 0;

			//To be filled by the caller
			//Example:
			//TRAP::Graphics::RendererAPI::TextureUpdateDesc update = {Texture};
			//TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(update);
			//Row by row copy is required if DstRowStride > SrcRowStride.
			//Single memcpy will work if DstRowStride == SrcRowStride.
			//2D:
			//for (u32 r = 0; r < update.RowCount; ++r)
			//    memcpy(update.MappedData + r * update.DstRowStride, srcPixels + r * update.SrcRowStride, update.SrcRowStride);
			//3D:
			//for (u32 z = 0; z < depth; ++z)
			//{
			//    u8* dstData = update.MappedData + update.DstSliceStride * z;
			//    u8* srcData = srcPixels + update.SrcSliceStride * z;
			//    for (u32 r = 0; r < update.RowCount; ++r)
			//        memcpy(dstData + r * update.DstRowStride, srcData + r * update.SrcRowStride, update.SrcRowStride);
			//}
			//TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(update, &token);
			u8* MappedData{};
			//Size of each row in destination including padding - Needs to be respected
			//otherwise texture data will be corrupted if dst row stride is not the same as src row stride
			u32 DstRowStride{};
			//Number of rows in this slice of the texture
			u32 RowCount{};
			//Src row stride for convenience (RowCount * width * texture format size)
			u32 SrcRowStride{};
			//Size of each slice in destination including padding - Use for offsetting dst data updating 3D textures
			u32 DstSliceStride{};
			//Size of each slice in src - Use for offsetting src data when updating 3D textures
			u32 SrcSliceStride{};

			//Internal
			struct
			{
				MappedMemoryRange MappedRange;
			} Internal;
		};

		struct TextureCopyDesc
		{
			//Source texture to copy from
			TRAP::Ref<TRAP::Graphics::Texture> Texture;
			//Destination buffer to copy to
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
			//Semaphore to synchronize graphics/compute operations that write
			//to the textures with the texture -> buffer copy.
			TRAP::Ref<Semaphore> WaitSemaphore;
			//Mip level to copy from
			u32 MipLevel;
			//Array layer to copy from
			u32 ArrayLayer;
			//Current texture state
			ResourceState TextureState;
			//Queue the texture is copied from
			RendererAPI::QueueType QueueType;
			//Offset into the destination buffer to start at
			u64 BufferOffset;
		};

		/// <summary>
		/// Description for the resource loader.
		/// </summary>
		struct ResourceLoaderDesc
		{
			//Size for each staging buffer
			u64 BufferSize;
			//Amount of staging buffers to create
			u32 BufferCount;
		};

		/// <summary>
		/// Description for a buffer load.
		/// </summary>
		struct BufferLoadDesc
		{
			//Output buffer with uploaded data.
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
			//Data to upload
			const void* Data;
			//Description for the buffer
			BufferDesc Desc;

			//Force Reset Buffer to 0
			bool ForceReset;
		};

		/// <summary>
		/// Description for a buffer update.
		/// </summary>
		struct BufferUpdateDesc
		{
			//Buffer to update
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
			//Offset into buffer to update data at
			u64 DstOffset;
			//Size of data to update
			u64 Size;

			//To be filled by the caller
			void* MappedData;

			//Internal
			struct
			{
				MappedMemoryRange MappedRange;
			} Internal;
		};

		/// <summary>
		/// Struct holding data about a GPUs features, limits and other properties.
		/// </summary>
		inline static struct GPUSettings
		{
			u64 UniformBufferAlignment;
			u32 MaxUniformBufferRange;
			u64 StorageBufferAlignment;
			u64 MaxStorageBufferRange;
			u32 UploadBufferTextureAlignment;
			u32 UploadBufferTextureRowAlignment;
			u32 MaxVertexInputBindings;
			u32 MaxVertexInputAttributes;
			u32 MaxRootSignatureDWORDS;
			u32 MaxImageDimension2D;
			u32 MaxImageDimensionCube;
			u32 MaxPushConstantSize;
			u32 MaxSamplerAllocationCount;
			u32 MaxTessellationControlPoints;
			SampleCount MaxMSAASampleCount;
			f32 MaxAnisotropy;

			u32 WaveLaneCount;
			TRAP::Graphics::RendererAPI::WaveOpsSupportFlags WaveOpsSupportFlags;

			u32 ROVsSupported;
			bool MultiDrawIndirectSupported;
			bool TessellationSupported;
			bool GeometryShaderSupported;
			bool FillModeNonSolid;
			bool SurfaceSupported;
			bool PresentSupported;
			bool SampleRateShadingSupported;

			//Variable rate shading capabilities
			TRAP::Graphics::RendererAPI::ShadingRate ShadingRates;
			TRAP::Graphics::RendererAPI::ShadingRateCaps ShadingRateCaps;
			TRAP::Graphics::RendererAPI::ShadingRateCombiner ShadingRateCombiner;
			u32 ShadingRateTexelWidth;
			u32 ShadingRateTexelHeight;

#ifndef TRAP_HEADLESS_MODE
			//NVIDIA Reflex
			bool ReflexSupported;
#endif /*TRAP_HEADLESS_MODE*/
		} GPUSettings{};

		inline static constexpr u32 ImageCount = 3; //Triple Buffered

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve the image index currently used for rendering from the given window.
		/// </summary>
		/// <param name="window">Window to retrieve image index from.</param>
		/// <returns>Image index.</returns>
		[[nodiscard]] static u32 GetCurrentImageIndex(const TRAP::Window* window);
#else
		/// <summary>
		/// Retrieve the image index currently used for rendering.
		/// </summary>
		/// <returns>Image index.</returns>
		[[nodiscard]] static u32 GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

#ifdef ENABLE_NSIGHT_AFTERMATH
		//GPU crash dump tracker using Nsight Aftermath instrumentation
		inline constinit static bool s_aftermathSupport = false;
		inline constinit static bool s_diagnosticsConfigSupport = false;
		inline constinit static bool s_diagnosticCheckPointsSupport = false;
#endif /*ENABLE_NSIGHT_AFTERMATH*/

	protected:
		static TRAP::Scope<RendererAPI> s_Renderer;
		inline constinit static RenderAPI s_RenderAPI = RenderAPI::NONE;
		static TRAP::Scope<API::ResourceLoader> s_ResourceLoader;

		static TRAP::Ref<DescriptorPool> s_descriptorPool;
		static TRAP::Ref<Queue> s_graphicQueue;
		static TRAP::Ref<Queue> s_computeQueue;
		static TRAP::Ref<Queue> s_transferQueue;

		inline constinit static RendererAPI::SampleCount s_newSampleCount = RendererAPI::SampleCount::Two;
		inline constinit static RendererAPI::AntiAliasing s_newAntiAliasing = RendererAPI::AntiAliasing::Off;
		inline constinit static RendererAPI::SampleCount s_Anisotropy = RendererAPI::SampleCount::Sixteen;

		inline constinit static TRAP::Utils::UUID s_newGPUUUID{};

	public:
		enum class PerWindowState
		{
			PreUpdate,
			PostUpdate,
		};

		/// <summary>
		/// Per window data used for rendering.
		/// </summary>
		struct PerViewportData
		{
			/// <summary>
			/// Constructor.
			/// </summary>
			constexpr PerViewportData() = default;

			/// <summary>
			/// Destructor.
			/// </summary>
			~PerViewportData();

			constexpr PerViewportData(const PerViewportData&) = delete;
			PerViewportData(PerViewportData&&) = default;
			constexpr PerViewportData& operator=(const PerViewportData &) = delete;
			PerViewportData& operator=(PerViewportData &&) = default;

#ifndef TRAP_HEADLESS_MODE
			TRAP::Window* Window{};
#endif /*TRAP_HEADLESS_MODE*/

			PerWindowState State{};

			//Swapchain/Graphics stuff
			u32 ImageIndex = 0;
			std::array<TRAP::Ref<CommandPool>, ImageCount> GraphicCommandPools;
			std::array<CommandBuffer*, ImageCount> GraphicCommandBuffers{};
			std::array<TRAP::Ref<Fence>, ImageCount> RenderCompleteFences;
#ifndef TRAP_HEADLESS_MODE
			TRAP::Ref<Semaphore> ImageAcquiredSemaphore;
#endif /*TRAP_HEADLESS_MODE*/
			std::array<TRAP::Ref<Semaphore>, ImageCount> RenderCompleteSemaphores;
			std::array<TRAP::Ref<Semaphore>, ImageCount> GraphicsCompleteSemaphores;
			std::array<TRAP::Ref<QueryPool>, ImageCount> GraphicsTimestampQueryPools;
			std::array<TRAP::Ref<Buffer>, ImageCount> GraphicsTimestampReadbackBuffers;
			PipelineDesc GraphicsPipelineDesc;
			TRAP::Ref<Pipeline> CurrentGraphicsPipeline;
			f32 GraphicsFrameTime{};
			bool Recording{};
			TRAP::Ref<RenderTarget> NewShadingRateTexture;
			std::array<TRAP::Ref<TRAP::Graphics::RenderTarget>, 3> CachedShadingRateTextures{};

			f32 NewRenderScale = 1.0f;
			f32 RenderScale = 1.0f;
#ifndef TRAP_HEADLESS_MODE
			TRAP::Ref<TRAP::Graphics::SwapChain> SwapChain;
			bool ResizeSwapChain = false;
#endif /*TRAP_HEADLESS_MODE*/
			std::array<TRAP::Ref<RenderTarget>, ImageCount> TemporaryResolveRenderTargets; //Used to resolve MSAA RenderTarget before applying RenderScale
			std::vector<TRAP::Ref<RenderTarget>> InternalRenderTargets{}; //Used when RenderScale is not 1.0f
#ifdef TRAP_HEADLESS_MODE
			std::array<TRAP::Ref<RenderTarget>, ImageCount> RenderTargets;
			bool Resize = false;
			u32 NewWidth = 1920, NewHeight = 1080; //Default RenderTargets to use Full HD
#endif /*TRAP_HEADLESS_MODE*/
			u32 CurrentSwapChainImageIndex{};
			RendererAPI::AntiAliasing CurrentAntiAliasing = RendererAPI::AntiAliasing::Off;
			RendererAPI::SampleCount CurrentSampleCount = RendererAPI::SampleCount::One;

			RendererAPI::Color ClearColor{0.1, 0.1, 0.1, 1.0};
			RendererAPI::DepthStencil ClearDepthStencil{0.0, 0};

#ifndef TRAP_HEADLESS_MODE
			bool CurrentVSync{};
			bool NewVSync{};
#endif /*TRAP_HEADLESS_MODE*/

			//Compute stuff
			std::array<TRAP::Ref<CommandPool>, ImageCount> ComputeCommandPools;
			std::array<CommandBuffer*, ImageCount>  ComputeCommandBuffers{};
			std::array<TRAP::Ref<Fence>, ImageCount> ComputeCompleteFences;
			std::array<TRAP::Ref<Semaphore>, ImageCount> ComputeCompleteSemaphores;
			std::array<TRAP::Ref<QueryPool>, ImageCount> ComputeTimestampQueryPools;
			std::array<TRAP::Ref<Buffer>, ImageCount> ComputeTimestampReadbackBuffers;
			TRAP::Math::Vec3ui CurrentComputeWorkGroupSize{};
			PipelineDesc ComputePipelineDesc;
			TRAP::Ref<Pipeline> CurrentComputePipeline;
			f32 ComputeFrameTime{};
			bool RecordingCompute{};

			//NVIDIA Reflex
#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
			NVLL_VK_SET_SLEEP_MODE_PARAMS SleepModeParams{};
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
		};

	protected:
#ifndef TRAP_HEADLESS_MODE
		static std::unordered_map<const Window*, TRAP::Scope<PerViewportData>> s_perViewportDataMap;
#else
		static TRAP::Scope<PerViewportData> s_perViewportData;
#endif /*TRAP_HEADLESS_MODE*/

	private:
		inline constinit static bool s_isVulkanCapable = true;
		inline constinit static bool s_isVulkanCapableFirstTest = true;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::WaveOpsSupportFlags)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::TextureCreationFlags)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ResourceState)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::DescriptorType)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::QueueFlag)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::BufferCreationFlags)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ShaderStage)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::RootSignatureFlags)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::BlendStateTargets)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::PipelineCacheFlags)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ShadingRate)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ShadingRateCaps)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ShadingRateCombiner)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ClearBufferType)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::LatencyMode)

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::RendererAPI::SetNewGPU(const TRAP::Utils::UUID& GPUUUID) noexcept
{
	s_newGPUUUID = GPUUUID;
}

#endif /*TRAP_RENDERERAPI_H*/

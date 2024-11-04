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
#include "Utils/Optional.h"

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
	/// @brief Supported render APIs.
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
		enum class NVIDIAReflexLatencyMode : u32;
		enum class NVIDIAReflexLatencyMarker : u8;
		struct Color;
		struct LoadActionsDesc;
		struct BufferBarrier;
		struct TextureBarrier;
		struct RenderTargetBarrier;
	//protected:
		struct PerViewportData;

	protected:
		/// @brief Constructor.
		constexpr RendererAPI() noexcept = default;
	public:
		/// @brief Destructor.
		constexpr virtual ~RendererAPI() = default;
		/// @brief Copy constructor.
		consteval RendererAPI(const RendererAPI&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval RendererAPI& operator=(const RendererAPI&) noexcept = delete;
		/// @brief Move constructor.
		consteval RendererAPI(RendererAPI&&) noexcept = delete;
		/// @brief Move assignment operator.
		consteval RendererAPI& operator=(RendererAPI&&) noexcept = delete;

		/// @brief Initialize the Renderer.
		/// @param gameName Name of the game.
		/// @param renderAPI Render API to use.
		static void Init(std::string_view gameName, RenderAPI renderAPI);
		/// @brief Shutdown the Renderer.
		static void Shutdown();

		/// @brief Retrieve the Renderer singleton.
		/// @return Renderer.
		[[nodiscard]] static RendererAPI* GetRenderer();
		/// @brief Retrieve the resource loader singleton.
		/// @return Resource loader.
		[[nodiscard]] static API::ResourceLoader* GetResourceLoader() noexcept;

		/// @brief Auto select a supported render API.
		/// @return Auto selected render API.
		[[nodiscard]] static RenderAPI AutoSelectRenderAPI();
		/// @brief Check whether a render API is supported by the system.
		/// @return True if supported, false otherwise.
		[[nodiscard]] static bool IsSupported(RenderAPI api);
		/// @brief Retrieve the currently used render API.
		/// @return Currently used render API.
		[[nodiscard]] static RenderAPI GetRenderAPI() noexcept;

		/// @brief Set a new GPU to use.
		/// @param GPUUUID UUID of the GPU to use.
		/// @note This only takes effect after a restart of the engine.
		static constexpr void SetNewGPU(const TRAP::Utils::UUID& GPUUUID) noexcept;
		/// @brief Get the UUID of the new GPU to use.
		/// @return UUID of the new GPU to use.
		/// @note This will return an empty UUID if no new GPU was set.
		[[nodiscard]] static TRAP::Utils::UUID GetNewGPU() noexcept;

		/// @brief On post update function.
		/// This function performs several tasks that need to be done after LayerStack::OnUpdate() calls.
		/// Currently this only performs scaling of the render targets, dependening on the current render scale.
		static void OnPostUpdate();

		/// @brief Initialize the internal renderer.
		/// @param gameName Name of the game.
		virtual void InitInternal(std::string_view gameName) = 0;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Flush renderer for the given window.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Presents the rendered image to the screen.
		/// 4. Starts graphics and compute recording for the next frame.
		/// @param window Window to flush.
		/// @remark @headless This function is not available in headless mode.
		virtual void Flush(const Window& window) const = 0;
#else
		/// @brief Flush renderer.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Starts graphics and compute recording for the next frame.
		/// @remark This function is only available in headless mode.
		virtual void Flush() const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Dispatch to the given window.
		/// @param workGroupElements Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// @param window Window to Dispatch.
		/// @remark @headless This function is not available in headless mode.
		virtual void Dispatch(std::array<u32, 3> workGroupElements, const Window& window) const = 0;
#else
		/// @brief Dispatch.
		/// @param workGroupElements Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// @remark This function is only available in headless mode.
		virtual void Dispatch(std::array<u32, 3> workGroupElements) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the VSync state for the given window.
		/// @param vsync Enable or disable VSync.
		/// @param window Window to set VSync for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetVSync(bool vsync, const Window& window) const = 0;
		/// @brief Retrieve whether VSync is enabled or not for the given window.
		/// @param window Window to retrieve VSync for.
		/// @return True if VSync is enabled, false otherwise.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] virtual bool GetVSync(const Window& window) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#if !defined(TRAP_HEADLESS_MODE)
		/// @brief Set the FPS limit for NVIDIA-Reflex.
		/// @param limit FPS target to limit to.
		/// @note This function affects all windows.
		/// @warning Do not call this function in user code!
		///          Use TRAP::Application::SetFPSLimit() instead.
		///          This function is only used internally for NVIDIA-Reflex.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetReflexFPSLimit(u32 limit) = 0;
#endif /*!defined(TRAP_HEADLESS_MODE)*/

		//RenderTarget Stuff

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the render scale for the given window.
		/// @param scale Render scale value (valid range: 0.5f-1.0f inclusive).
		/// @param window Window to set render scale for.
		/// @note This functon takes effect on the next frame.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetRenderScale(f32 scale, const Window& window) const = 0;
#else
		/// @brief Set the render scale.
		/// @param scale Render scale value (valid range: 0.5f-1.0f inclusive).
		/// @note This functon takes effect on the next frame.
		/// @remark This function is only available in headless mode.
		virtual void SetRenderScale(f32 scale) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the used render scale value of the given window.
		/// @param window Window to retrieve render scale from.
		/// @return Render scale (between 0.5f and 2.0f inclusive).
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] virtual f32 GetRenderScale(const Window& window) const = 0;
#else
		/// @brief Retrieve the used render scale value.
		/// @return Render scale (between 0.5f and 2.0f inclusive).
		/// @remark This function is only available in headless mode.
		[[nodiscard]] virtual f32 GetRenderScale() const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the clear color to be used by the given window.
		/// @param color New clear color.
		/// @param window Window to set clear color for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetClearColor(const Color& color /*= { 0.1f, 0.1f, 0.1f, 1.0f }*/,
		                           const Window& window) const = 0;
#else
		/// @brief Set the clear color.
		/// @param color New clear color.
		/// @remark This function is only available in headless mode.
		virtual void SetClearColor(const Color& color /*= { 0.1f, 0.1f, 0.1f, 1.0f }*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the clear depth value to be used by the given window.
		/// @param depth New clear depth value. Must be between 0.0f and 1.0f
		/// @param window Window to set clear depth value for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetClearDepth(f32 depth /*= 0.0f*/, const Window& window) const = 0;
#else
		/// @brief Set the clear depth value.
		/// @param depth New clear depth value. Must be between 0.0f and 1.0f
		/// @remark This function is only available in headless mode.
		virtual void SetClearDepth(f32 depth /*= 0.0f*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the clear stencil value to be used by the given window.
		/// @param stencil New clear stencil value.
		/// @param window Window to set clear stencil value for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetClearStencil(u32 stencil /*= 0*/, const Window& window) const = 0;
#else
		/// @brief Set the clear stencil value.
		/// @param stencil New clear stencil value.
		/// @remark This function is only available in headless mode.
		virtual void SetClearStencil(u32 stencil /*= 0*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// @brief Set the resolution of the render targets.
		/// @param width New width.
		/// @param height New height.
		/// @remark This function is only available in headless mode.
		virtual void SetResolution(u32 width, u32 height) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		//Pipeline Stuff

#ifndef TRAP_HEADLESS_MODE
		/// @brief Enable or disable depth testing for the given window.
		/// @param enabled Enable or disable depth testing.
		/// @param window Window to set depth testing for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetDepthTesting(bool enabled, const Window& window) const = 0;
#else
		/// @brief Enable or disable depth testing.
		/// @param enabled Enable or disable depth testing.
		/// @remark This function is only available in headless mode.
		virtual void SetDepthTesting(bool enabled) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Enable or disable depth writing for the given window.
		/// @param enabled Enable or disable depth writing.
		/// @param window Window to set depth writing for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetDepthWriting(bool enabled, const Window& window) const = 0;
#else
		/// @brief Enable or disable depth writing.
		/// @param enabled Enable or disable depth writing.
		/// @remark This function is only available in headless mode.
		virtual void SetDepthWriting(bool enabled) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the depth function for the given window.
		/// @param function Function to use for depth testing.
		/// @param window Window to set depth function for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetDepthFunction(CompareMode function, const Window& window) const = 0;
#else
		/// @brief Set the depth function.
		/// @param function Function to use for depth testing.
		/// @remark This function is only available in headless mode.
		virtual void SetDepthFunction(CompareMode function) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the depth action to perform when depth testing fails for the given window.
		/// @param front Depth action to perform when depth testing fails.
		/// @param back Depth action to perform when depth testing fails.
		/// @param window Window to set the depth fail action for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetDepthFail(StencilOp front, StencilOp back, const Window& window) const = 0;
#else
		/// @brief Set the depth action to perform when depth testing fails.
		/// @param front Depth action to perform when depth testing fails.
		/// @param back Depth action to perform when depth testing fails.
		/// @remark This function is only available in headless mode.
		virtual void SetDepthFail(StencilOp front, StencilOp back) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the depth bias (scalar factor to add to each fragments depth value) for the given window.
		/// @param depthBias Depth bias.
		/// @param window Window to set the depth bias for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetDepthBias(i32 depthBias, const Window& window) const = 0;
#else
		/// @brief Set the depth bias (scalar factor to add to each fragments depth value).
		/// @param depthBias Depth bias.
		/// @remark This function is only available in headless mode.
		virtual void SetDepthBias(i32 depthBias) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation) for the given window.
		/// @param factor Depth bias slope factor.
		/// @param window Window to set the depth bias slope factor for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetDepthBiasSlopeFactor(f32 factor, const Window& window) const = 0;
#else
		/// @brief Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation).
		/// @param factor Depth bias slope factor.
		/// @remark This function is only available in headless mode.
		virtual void SetDepthBiasSlopeFactor(f32 factor) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Enable or disable stencil testing for the given window.
		/// @param enabled Enable or disable stencil testing.
		/// @param window Window to set stencil testing for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetStencilTesting(bool enabled, const Window& window) const = 0;
#else
		/// @brief Enable or disable stencil testing.
		/// @param enabled Enable or disable stencil testing.
		/// @remark This function is only available in headless mode.
		virtual void SetStencilTesting(bool enabled) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the stencil action to perform when stencil testing fails for the given window.
		/// @param front Stencil action to perform when stencil testing fails.
		/// @param back Stencil action to perform when stencil testing fails.
		/// @param window Window to set the stencil fail action for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetStencilFail(StencilOp front, StencilOp back, const Window& window) const = 0;
#else
		/// @brief Set the stencil action to perform when stencil testing fails.
		/// @param front Stencil action to perform when stencil testing fails.
		/// @param back Stencil action to perform when stencil testing fails.
		/// @remark This function is only available in headless mode.
		virtual void SetStencilFail(StencilOp front, StencilOp back) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the stencil action to perform when stencil testing passes for the given window.
		/// @param front Stencil action to perform when passed.
		/// @param back Stencil action to perform when passed.
		/// @param window Window to set the stencil pass action for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetStencilPass(StencilOp front, StencilOp back, const Window& window) const = 0;
#else
		/// @brief Set the stencil action to perform when stencil testing passes.
		/// @param front Stencil action to perform when passed.
		/// @param back Stencil action to perform when passed.
		/// @remark This function is only available in headless mode.
		virtual void SetStencilPass(StencilOp front, StencilOp back) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the stencil functions for the given window.
		/// @param front Function to use on the front for stencil testing.
		/// @param back Function to use on the back for stencil testing.
		/// @param window Window to set stencil functions for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetStencilFunction(CompareMode front, CompareMode back, const Window& window) const = 0;
#else
		/// @brief Set the stencil functions.
		/// @param front Function to use on the front for stencil testing.
		/// @param back Function to use on the back for stencil testing.
		/// @remark This function is only available in headless mode.
		virtual void SetStencilFunction(CompareMode front, CompareMode back) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the stencil mask for the given window.
		/// @param read Select the bits of the stencil values to test.
		/// @param write Select the bits of the stencil values updated by the stencil test.
		/// @param window Window to set stencil mask for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetStencilMask(u8 read, u8 write, const Window& window) const = 0;
#else
		/// @brief Set the stencil mask.
		/// @param read Select the bits of the stencil values to test.
		/// @param write Select the bits of the stencil values updated by the stencil test.
		/// @remark This function is only available in headless mode.
		virtual void SetStencilMask(u8 read, u8 write) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the cull mode for the given window.
		/// @param mode Cull mode to use.
		/// @param window Window to set cull mode for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetCullMode(CullMode mode, const Window& window) const = 0;
#else
		/// @brief Set the cull mode.
		/// @param mode Cull mode to use.
		/// @remark This function is only available in headless mode.
		virtual void SetCullMode(CullMode mode) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the fill mode for the given window.
		/// @param mode Fill mode to use.
		/// @param window Window to set fill mode for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetFillMode(FillMode mode, const Window& window) const = 0;
#else
		/// @brief Set the fill mode.
		/// @param mode Fill mode to use.
		/// @remark This function is only available in headless mode.
		virtual void SetFillMode(FillMode mode) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the primitive topology for the given window.
		/// @param topology Primitive topology to use.
		/// @param window Window to set primitive topology for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetPrimitiveTopology(PrimitiveTopology topology, const Window& window) const = 0;
#else
		/// @brief Set the primitive topology.
		/// @param topology Primitive topology to use.
		/// @remark This function is only available in headless mode.
		virtual void SetPrimitiveTopology(PrimitiveTopology topology) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the front face winding order for the given window.
		/// @param face Front face winding order to use.
		/// @param window Window to set front face winding order for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetFrontFace(FrontFace face, const Window& window) const = 0;
#else
		/// @brief Set the front face winding order.
		/// @param face Front face winding order to use.
		/// @remark This function is only available in headless mode.
		virtual void SetFrontFace(FrontFace face) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the blend mode for the given window.
		/// @param modeRGB Blend mode to use for the RGB channels.
		/// @param modeAlpha Blend mode to use for the alpha channel.
		/// @param window Window to set the blend mode for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, const Window& window) const = 0;
#else
		/// @brief Set the blend mode.
		/// @param modeRGB Blend mode to use for the RGB channels.
		/// @param modeAlpha Blend mode to use for the alpha channel.
		/// @remark This function is only available in headless mode.
		virtual void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the blend constants/factors for the given window.
		/// @param sourceRGB Specifies how the red, green, and blue blending factors are computed.
		/// @param sourceAlpha Specifies how the alpha source blending factor is computed.
		/// @param destinationRGB Specifies how the red, green, and blue destination blending factors are computed.
		/// @param destinationAlpha Specified how the alpha destination blending factor is computed.
		/// @param window Window to set the blend constants for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                          BlendConstant destinationRGB, BlendConstant destinationAlpha,
									  const Window& window) const = 0;
#else
		/// @brief Set the blend constants/factors.
		/// @param sourceRGB Specifies how the red, green, and blue blending factors are computed.
		/// @param sourceAlpha Specifies how the alpha source blending factor is computed.
		/// @param destinationRGB Specifies how the red, green, and blue destination blending factors are computed.
		/// @param destinationAlpha Specified how the alpha destination blending factor is computed.
		/// @remark This function is only available in headless mode.
		virtual void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                          BlendConstant destinationRGB, BlendConstant destinationAlpha) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// @param shadingRate Shading rate to use.
		/// @param postRasterizerRate Shading rate combiner to use.
		/// @param finalRate Shading rate combiner to use.
		/// @param window Window to set the shading rate for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetShadingRate(ShadingRate shadingRate,
		                            ShadingRateCombiner postRasterizerRate,
							        ShadingRateCombiner finalRate, const Window& window) const = 0;
#else
		/// @brief Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// @param shadingRate Shading rate to use.
		/// @param postRasterizerRate Shading rate combiner to use.
		/// @param finalRate Shading rate combiner to use.
		/// @remark This function is only available in headless mode.
		virtual void SetShadingRate(ShadingRate shadingRate,
		                            ShadingRateCombiner postRasterizerRate,
							        ShadingRateCombiner finalRate) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the pipeline fragment shading rate via texture.
		/// @param texture Shading rate texture to use.
		/// @param window Window to set shading rate for.
		/// @note The texture must be in ResourceState::ShadingRateSource resource state.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetShadingRate(Ref<RenderTarget> texture, const Window& window) const = 0;
#else
		/// @brief Set the pipeline fragment shading rate via texture.
		/// @param texture Shading rate texture to use.
		/// @note The texture must be in ResourceState::ShadingRateSource resource state.
		/// @remark This function is only available in headless mode.
		virtual void SetShadingRate(Ref<RenderTarget> texture) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Clear the given window's render target.
		/// @param clearType Type of buffer to clear.
		/// @param window Window to clear.
		/// @remark @headless This function is not available in headless mode.
		virtual void Clear(ClearBufferType clearType, const Window& window) const = 0;
#else
		/// @brief Clear the given render target.
		/// @param clearType Type of buffer to clear.
		/// @remark This function is only available in headless mode.
		virtual void Clear(ClearBufferType clearType) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		//CommandBuffer Stuff

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set viewport size for the given window.
		/// @param x X coordinate of the top left corner of the viewport.
		/// @param y Y coordinate of the top left corner of the viewport.
		/// @param width New viewport width.
		/// @param height New viewport height.
		/// @param minDepth New min depth value. Default: 0.0f.
		/// @param maxDepth New max depth value. Default: 1.0f.
		/// @param window Window to set viewport for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetViewport(u32 x, u32 y, u32 width, u32 height, f32 minDepth /*= 0.0f*/,
		                         f32 maxDepth /*= 1.0f*/, const Window& window) const = 0;
#else
		/// @brief Set viewport size.
		/// @param x X coordinate of the top left corner of the viewport.
		/// @param y Y coordinate of the top left corner of the viewport.
		/// @param width New viewport width.
		/// @param height New viewport height.
		/// @param minDepth New min depth value. Default: 0.0f.
		/// @param maxDepth New max depth value. Default: 1.0f.
		/// @remark This function is only available in headless mode.
		virtual void SetViewport(u32 x, u32 y, u32 width, u32 height, f32 minDepth /*= 0.0f*/,
		                         f32 maxDepth /*= 1.0f*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set scissor size for the given window.
		/// @param x Upper left corner.
		/// @param y Upper left corner.
		/// @param width New scissor width.
		/// @param height New scissor height.
		/// @param window Window to set scissor size for.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetScissor(u32 x, u32 y, u32 width, u32 height,
		                        const Window& window) const = 0;
#else
		/// @brief Set scissor size.
		/// @param x Upper left corner.
		/// @param y Upper left corner.
		/// @param width New scissor width.
		/// @param height New scissor height.
		/// @remark This function is only available in headless mode.
		virtual void SetScissor(u32 x, u32 y, u32 width, u32 height) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw non-indexed, non-instanced geometry for the given window.
		/// @param vertexCount Number of vertices to draw.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @param window Window to draw for.
		/// @remark @headless This function is not available in headless mode.
		virtual void Draw(u32 vertexCount, u32 firstVertex /*= 0*/, const Window& window) const = 0;
#else
		/// @brief Draw non-indexed, non-instanced geometry.
		/// @param vertexCount Number of vertices to draw.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		virtual void Draw(u32 vertexCount, u32 firstVertex /*= 0*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw indexed, non-instanced geometry for the given window.
		/// @param indexCount Number of indices to draw.
		/// @param firstIndex Index of the first indice to draw. Default: 0.
		/// @param vertexOffset Index of the first vertex to draw. Default: 0.
		/// @param window Window to draw for.
		/// @remark @headless This function is not available in headless mode.
		virtual void DrawIndexed(u32 indexCount, u32 firstIndex /*= 0*/, i32 vertexOffset /*= 0*/,
		                         const Window& window) const = 0;
#else
		/// @brief Draw indexed, non-instanced geometry.
		/// @param indexCount Number of indices to draw.
		/// @param firstIndex Index of the first indice to draw. Default: 0.
		/// @param vertexOffset Index of the first vertex to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		virtual void DrawIndexed(u32 indexCount, u32 firstIndex /*= 0*/, i32 vertexOffset /*= 0*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw non-indexed, instanced geometry for the given window.
		/// @param vertexCount Number of vertices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @param firstInstance Index of the first instance to draw. Default: 0.
		/// @param window Window to draw for.
		/// @remark @headless This function is not available in headless mode.
		virtual void DrawInstanced(u32 vertexCount, u32 instanceCount, u32 firstVertex /*= 0*/,
		                           u32 firstInstance /*= 0*/, const Window& window) const = 0;
#else
		/// @brief Draw non-indexed, instanced geometry.
		/// @param vertexCount Number of vertices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @param firstInstance Index of the first instance to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		virtual void DrawInstanced(u32 vertexCount, u32 instanceCount, u32 firstVertex /*= 0*/,
		                           u32 firstInstance /*= 0*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw indexed, instanced geometry for the given window.
		/// @param indexCount Number of indices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstIndex Index of the first indice to draw. Default: 0.
		/// @param firstInstance Index of the first instance to draw. Default: 0.
		/// @param vertexOffset Index of the first vertex to draw. Default: 0.
		/// @param window Window to draw for.
		/// @remark @headless This function is not available in headless mode.
		virtual void DrawIndexedInstanced(u32 indexCount, u32 instanceCount,
		                                  u32 firstIndex /*= 0*/, u32 firstInstance /*= 0*/,
										  i32 vertexOffset /*= 0*/, const Window& window) const = 0;
#else
		/// @brief Draw indexed, instanced geometry.
		/// @param indexCount Number of indices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstIndex Index of the first indice to draw. Default: 0.
		/// @param firstInstance Index of the first instance to draw. Default: 0.
		/// @param vertexOffset Index of the first vertex to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		virtual void DrawIndexedInstanced(u32 indexCount, u32 instanceCount,
		                                  u32 firstIndex /*= 0*/, u32 firstInstance /*= 0*/,
										  i32 vertexOffset /*= 0*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind vertex buffer on the given window.
		/// @param vBuffer Vertex buffer to bind.
		/// @param layout Layout of the vertex buffer.
		/// @param window Window to bind the vertex buffer for.
		/// @remark @headless This function is not available in headless mode.
		virtual void BindVertexBuffer(const Buffer& vBuffer, const VertexBufferLayout& layout,
		                              const Window& window) const = 0;
#else
		/// @brief Bind vertex buffer.
		/// @param vBuffer Vertex buffer to bind.
		/// @param layout Layout of the vertex buffer.
		/// @remark This function is only available in headless mode.
		virtual void BindVertexBuffer(const Buffer& vBuffer, const VertexBufferLayout& layout) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind an index buffer on the given window.
		/// @param iBuffer Index buffer to bind.
		/// @param indexType Data type used by the index buffer.
		/// @param window Window to bind the vertex buffer for.
		/// @remark @headless This function is not available in headless mode.
		virtual void BindIndexBuffer(const Buffer& iBuffer, IndexType indexType,
		                             const Window& window) const = 0;
#else
		/// @brief Bind an index buffer.
		/// @param iBuffer Index buffer to bind.
		/// @param indexType Data type used by the index buffer.
		/// @remark This function is only available in headless mode.
		virtual void BindIndexBuffer(const Buffer& iBuffer, IndexType indexType) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind a descriptor set on the given window.
		/// @param dSet Descriptor set to bind.
		/// @param index Index for which descriptor set to bind.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @param window Window to bind the descriptor set for.
		/// @remark @headless This function is not available in headless mode.
		virtual void BindDescriptorSet(DescriptorSet& dSet, u32 index,
		                               QueueType queueType /*= QueueType::Graphics*/,
									   const Window& window) const = 0;
#else
		/// @brief Bind a descriptor set.
		/// @param dSet Descriptor set to bind.
		/// @param index Index for which descriptor set to bind.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		virtual void BindDescriptorSet(DescriptorSet& dSet, u32 index,
		                               QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind push constant buffer data on the given window.
		/// @param name Name of the push constant block.
		/// @param constants Constant buffer data.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @param window Window to bind the push constants for.
		/// @note There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// @remark @headless This function is not available in headless mode.
		virtual void BindPushConstants(std::string_view name, std::span<const u8> constants,
							           QueueType queueType /*= QueueType::Graphics*/,
									   const Window& window) const = 0;
#else
		/// @brief Bind push constant buffer data.
		/// @param name Name of the push constant block.
		/// @param constants Constant buffer data.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @note There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// @remark This function is only available in headless mode.
		virtual void BindPushConstants(std::string_view name, std::span<const u8> constants,
		                               QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind push constant buffer data on the given window.
		/// @param paramIndex Index of the push constant block in the RootSignatures descriptors array.
		/// @param constants Constant buffer data.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @param window Window to bind the push constants for.
		/// @remark @headless This function is not available in headless mode.
		virtual void BindPushConstantsByIndex(u32 paramIndex, std::span<const u8> constants,
										      QueueType queueType /*= QueueType::Graphics*/,
											  const Window& window) const = 0;
#else
		/// @brief Bind push constant buffer data.
		/// @param paramIndex Index of the push constant block in the RootSignatures descriptors array.
		/// @param constants Constant buffer data.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		virtual void BindPushConstantsByIndex(u32 paramIndex, std::span<const u8> constants,
											  QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind render target(s) on the given window.
		/// @param colorTarget Color render target to bind.
		/// @param depthStencil Optional depth stencil target to bind. Default: nullptr.
		/// @param loadActions Optional load actions for each render target. Default: nullptr.
		/// @param colorArraySlices Optional color array slices for each render target. Default: nullptr.
		/// @param colorMipSlices Optional color mip slices for each render target. Default: nullptr.
		/// @param depthArraySlice Optional depth array slice for the depth stencil target. Default: -1.
		/// @param depthMipSlice Optional depth mip slice for the depth stencil target. Default: -1.
		/// @param window Window to bind the render target(s) for.
		/// @note This functions ends the currently running render pass and starts a new one.
		/// @remark @headless This function is not available in headless mode.
		virtual void BindRenderTarget(const Graphics::RenderTarget* colorTarget,
		                              const Graphics::RenderTarget* depthStencil /*= nullptr*/,
									  RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
									  std::vector<u32>* colorArraySlices /*= nullptr*/,
									  std::vector<u32>* colorMipSlices /*= nullptr*/,
									  u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/,
									  const Window& window) const = 0;
#else
		/// @brief Bind render target(s).
		/// @param colorTarget Color render target to bind.
		/// @param depthStencil Optional depth stencil target to bind. Default: nullptr.
		/// @param loadActions Optional load actions for each render target. Default: nullptr.
		/// @param colorArraySlices Optional color array slices for each render target. Default: nullptr.
		/// @param colorMipSlices Optional color mip slices for each render target. Default: nullptr.
		/// @param depthArraySlice Optional depth array slice for the depth stencil target. Default: -1.
		/// @param depthMipSlice Optional depth mip slice for the depth stencil target. Default: -1.
		/// @note This functions ends the currently running render pass and starts a new one.
		/// @remark This function is only available in headless mode.
		virtual void BindRenderTarget(const Graphics::RenderTarget* colorTarget,
		                              const Graphics::RenderTarget* depthStencil /*= nullptr*/,
									  RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
									  std::vector<u32>* colorArraySlices /*= nullptr*/,
									  std::vector<u32>* colorMipSlices /*= nullptr*/,
									  u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind render target(s) on the given window.
		/// @param colorTargets Color render target(s) to bind.
		/// @param depthStencil Optional depth stencil target to bind. Default: nullptr.
		/// @param loadActions Optional load actions for each render target. Default: nullptr.
		/// @param colorArraySlices Optional color array slices for each render target. Default: nullptr.
		/// @param colorMipSlices Optional color mip slices for each render target. Default: nullptr.
		/// @param depthArraySlice Optional depth array slice for the depth stencil target. Default: -1.
		/// @param depthMipSlice Optional depth mip slice for the depth stencil target. Default: -1.
		/// @param window Window to bind the render target(s) for.
		/// @note This functions ends the currently running render pass and starts a new one.
		/// @remark @headless This function is not available in headless mode.
		virtual void BindRenderTargets(const std::vector<std::reference_wrapper<const Graphics::RenderTarget>>& colorTargets,
		                               const Graphics::RenderTarget* depthStencil /*= nullptr*/,
									   RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
									   std::vector<u32>* colorArraySlices /*= nullptr*/,
									   std::vector<u32>* colorMipSlices /*= nullptr*/,
									   u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/,
									   const Window& window) const = 0;
#else
		/// @brief Bind render target(s).
		/// @param colorTargets Color render target(s) to bind.
		/// @param depthStencil Optional depth stencil target to bind. Default: nullptr.
		/// @param loadActions Optional load actions for each render target. Default: nullptr.
		/// @param colorArraySlices Optional color array slices for each render target. Default: nullptr.
		/// @param colorMipSlices Optional color mip slices for each render target. Default: nullptr.
		/// @param depthArraySlice Optional depth array slice for the depth stencil target. Default: -1.
		/// @param depthMipSlice Optional depth mip slice for the depth stencil target. Default: -1.
		/// @note This functions ends the currently running render pass and starts a new one.
		/// @remark This function is only available in headless mode.
		virtual void BindRenderTargets(const std::vector<std::reference_wrapper<const Graphics::RenderTarget>>& colorTargets,
		                               const Graphics::RenderTarget* depthStencil /*= nullptr*/,
									   RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
									   std::vector<u32>* colorArraySlices /*= nullptr*/,
									   std::vector<u32>* colorMipSlices /*= nullptr*/,
									   u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Add a resource barrier (memory dependency) for the given window.
		/// @param bufferBarrier Buffer barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barrier for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
										   QueueType queueType /*= QueueType::Graphics*/,
								           const Window& window) const = 0;
#else
		/// @brief Add a resource barrier (memory dependency).
		/// @param bufferBarrier Buffer barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		virtual void ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
										   QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add resource barriers (memory dependencies) for the given window.
		/// @param bufferBarriers Buffer barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barriers for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
											QueueType queueType /*= QueueType::Graphics*/,
									        const Window& window) const = 0;
#else
		/// @brief Add resource barriers (memory dependencies).
		/// @param bufferBarriers Buffer barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		virtual void ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
											QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add a resource barrier (memory dependency) for the given window.
		/// @param textureBarrier Texture barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barrier for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
											QueueType queueType /*= QueueType::Graphics*/,
									        const Window& window) const = 0;
#else
		/// @brief Add a resource barrier (memory dependency).
		/// @param textureBarrier Texture barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		virtual void ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
											QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add resource barriers (memory dependencies) for the given window.
		/// @param textureBarriers Texture barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barriers for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
											 QueueType queueType /*= QueueType::Graphics*/,
									         const Window& window) const = 0;
#else
		/// @brief Add resource barriers (memory dependencies).
		/// @param textureBarriers Texture barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		virtual void ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
											 QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add a resource barrier (memory dependency) for the given window.
		/// @param renderTargetBarrier Render target barrier.
		/// @param window Window to add the barrier for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
									             const Window& window) const = 0;
#else
		/// @brief Add a resource barrier (memory dependency).
		/// @param renderTargetBarrier Render target barrier.
		/// @remark This function is only available in headless mode.
		virtual void ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add resource barriers (memory dependencies) for the given window.
		/// @param renderTargetBarriers Render target barriers.
		/// @param window Window to add the barriers for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
									              const Window& window) const = 0;
#else
		/// @brief Add resource barriers (memory dependencies).
		/// @param renderTargetBarriers Render target barriers.
		/// @remark This function is only available in headless mode.
		virtual void ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief NVIDIA-Reflex Sleep/synchronize on the given window.
		/// @param window Window to sleep for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ReflexSleep(const Window& window) const = 0;
		/// @brief Set a timestamp for NVIDIA-Reflex.
		/// @param marker Enum value of the marker to set.
		/// @param window Window to set the timestamp for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ReflexMarker(NVIDIAReflexLatencyMarker marker, const Window& window) const = 0;
		/// @brief Retrieve latency reports from NVIDIA-Reflex.
		/// @param numLatencyData Number of latency data points to return. Set to 0 to retrieve the maximum queryable frame data.
		/// @param window Window to get latency data for.
		/// @return Latency reports.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] virtual std::vector<VkLatencyTimingsFrameReportNV> ReflexGetLatency(u32 numLatencyData, const Window& window) const = 0;
		/// @brief Retrieve all queryable latency reports from NVIDIA-Reflex.
		/// @param window Window to get latency data for.
		/// @return Latency reports.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] std::vector<VkLatencyTimingsFrameReportNV> ReflexGetLatency(const Window& window) const;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Retrieve the renderer title.
		/// Example title: "[Vulkan 1.3.0]".
		/// @return Renderer title.
		[[nodiscard]] constexpr virtual std::string GetTitle() const noexcept = 0;

		/// @brief Retrieve the currently used GPUs UUID.
		/// @return GPU's UUID.
		[[nodiscard]] virtual TRAP::Utils::UUID GetCurrentGPUUUID() const noexcept = 0;
		/// @brief Retrieve the name of the currently used GPU.
		/// @return GPU's name.
		[[nodiscard]] virtual std::string GetCurrentGPUName() const noexcept = 0;
		/// @brief Retrieve the vendor of the currently used GPU.
		/// @return GPU vendor.
		[[nodiscard]] virtual GPUVendor GetCurrentGPUVendor() const noexcept = 0;
		/// @brief Retrieve a list of all supported GPUs.
		/// The list contains the GPUs name and UUID.
		/// @return List of all supported GPUs.
		[[nodiscard]] virtual std::vector<std::pair<std::string, TRAP::Utils::UUID>> GetAllGPUs() const = 0;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Capture a screenshot of the last presented frame.
		/// @param window Window to capture screenshot on.
		/// @return Captured screenshot as TRAP::Image on success, nullptr otherwise.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] virtual TRAP::Scope<TRAP::Image> CaptureScreenshot(const Window& window) const = 0;
#else
		/// @brief Capture a screenshot of the last presented frame.
		/// @return Captured screenshot as TRAP::Image on success, nullptr otherwise.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] virtual TRAP::Scope<TRAP::Image> CaptureScreenshot() const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Resolve a MSAA render target to a non MSAA render target.
		/// Needed to transfer MSAA rendered image data to a presentable non-MSAA target.
		/// @param source Source MSAA render target to resolve.
		/// @param destination Destination non MSAA render target to resolve into.
		/// @param cmd CommadBuffer to resolve on.
		/// @note source and destination must be in ResourceState::RenderTarget resource state.
		virtual void MSAAResolvePass(const RenderTarget& source, const RenderTarget& destination,
		                             CommandBuffer* cmd) const = 0;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Scale image from internal resolution to the final output resolution.
		/// @param source Source render target to resolve.
		/// @param destination Destination render target to resolve into.
		/// @param window Window to do the scaling pass on.
		/// @note source and destination must be in ResourceState::RenderTarget resource state.
		/// @remark @headless This function is not available in headless mode.
		virtual void RenderScalePass(const RenderTarget& source,
									 const RenderTarget& destination,
		                             const Window& window) const = 0;
#else
		/// @brief Scale image from internal resolution to the final output resolution.
		/// @param source Source render target to resolve.
		/// @param destination Destination render target to resolve into.
		/// @note source and destination must be in ResourceState::RenderTarget resource state.
		/// @remark This function is only available in headless mode.
		virtual void RenderScalePass(const RenderTarget& source,
									 const RenderTarget& destination) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the latency mode.
		/// The actual latency mode may differ from the requested one so check
		/// the actual used mode with GetReflexLatencyMode().
		/// @param mode Latency mode to set.
		/// @param window Window to set latency mode for.
		/// @remark @win32 NVIDIAReflexLatencyModes are only available on Windows 10 or newer with NVIDIA hardware.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetReflexLatencyMode(NVIDIAReflexLatencyMode mode, const Window& window) = 0;
		/// @brief Retrieve the currently used latency mode for NVIDIA Reflex.
		/// @param window Window to retrieve latency mode for.
		/// @return Used latency mode.
		/// @note The returned value may differ from the requested mode set with SetReflexLatencyMode().
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] virtual NVIDIAReflexLatencyMode GetReflexLatencyMode(const Window& window) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Retrieve the used descriptor pool.
		/// @return Descriptor pool.
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::DescriptorPool> GetDescriptorPool() noexcept;
		/// @brief Retrieve the used graphics queue.
		/// @return Graphics queue.
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::Queue> GetGraphicsQueue() noexcept;
		/// @brief Retrieve the used compute queue.
		/// @return Compute queue.
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::Queue> GetComputeQueue() noexcept;
		/// @brief Retrieve the used transfer queue.
		/// @return Transfer queue.
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::Queue> GetTransferQueue() noexcept;
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the currently used graphics root signature of the given window.
		/// @param window Window to retrieve the graphics root signature from.
		/// @return Graphics root signature.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::RootSignature> GetGraphicsRootSignature(const Window& window);
#else
		/// @brief Retrieve the currently used graphics root signature.
		/// @return Graphics root signature.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] static TRAP::Ref<TRAP::Graphics::RootSignature> GetGraphicsRootSignature();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the currently used internal render resolution of the given window.
		/// @param window Window to get internal render resolution from.
		/// @return Internal render resolution.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static TRAP::Math::Vec2ui GetInternalRenderResolution(const Window& window);
#else
		/// @brief Retrieve the currently used internal render resolution.
		/// @return Internal render resolution.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] static TRAP::Math::Vec2ui GetInternalRenderResolution();
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// @brief Get the current viewport resolution.
		/// @return Resolution of the viewport.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] virtual TRAP::Math::Vec2ui GetResolution() const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Start a render pass for the given window.
		/// @param window Window to start render pass for.
		/// @note This will rebind the render target for the current frame.
		/// @remark @headless This function is not available in headless mode.
		static void StartRenderPass(const Window& window);
#else
		/// @brief Start a render pass.
		/// @note This will rebind the render target for the current frame again.
		/// @remark This function is only available in headless mode.
		static void StartRenderPass();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Stop running render pass of the given window.
		/// @param window Window to stop render pass on.
		/// @remark @headless This function is not available in headless mode.
		static void StopRenderPass(const Window& window);
#else
		/// @brief Stop running render pass.
		/// @remark This function is only available in headless mode.
		static void StopRenderPass();
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Transition a texture from old layout to the new layout.
		/// The transition happens immediately and is guaranteed to be complete when the function returns.
		/// @param texture Texture to transition layout.
		/// @param oldLayout Current resource state of the given texture.
		/// @param newLayout New resource state for the given texture.
		/// @param queueType Queue type on which to perform the transition. Default: Graphics.
		static void Transition(const Ref<TRAP::Graphics::Texture>& texture,
							   TRAP::Graphics::RendererAPI::ResourceState oldLayout,
							   TRAP::Graphics::RendererAPI::ResourceState newLayout,
							   TRAP::Graphics::RendererAPI::QueueType queueType = QueueType::Graphics);
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the currently used anti aliasing method and the sample count.
		/// @param outAntiAliasing Output: Used anti aliasing method.
		/// @param outSampleCount Output: Used sample count.
		/// @param window Window to get anti aliasing from.
		/// @remark @headless This function is not available in headless mode.
		static void GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount, const Window& window) noexcept;
#else
		/// @brief Retrieve the currently used anti aliasing method and the sample count.
		/// @param outAntiAliasing Output: Used anti aliasing method.
		/// @param outSampleCount Output: Used sample count.
		/// @remark This function is only available in headless mode.
		static void GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount) noexcept;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Set the anti aliasing method and the sample count.
		/// Use AntiAliasing::Off to disable anti aliasing.
		/// @param antiAliasing Anti aliasing method to use.
		/// @param sampleCount Sample count to use.
		/// @note This takes effect on the next frame onwards.
		/// @note A sample count of 1 is only valid if anti aliasing is disabled.
		static void SetAntiAliasing(AntiAliasing antiAliasing, SampleCount sampleCount);

		/// @brief Retrieve the currently used anisotropy level.
		/// @return Used anisotropy level.
		[[nodiscard]] static SampleCount GetAnisotropyLevel() noexcept;

		/// @brief Set the anisotropy level.
		/// A value of SampleCount::One effectively disables anisotropic filtering.
		/// @param anisotropyLevel Anisotropy level to use.
		/// @note User created samplers need to be recreated in order to use the new anisotropy level.
		static void SetAnisotropyLevel(SampleCount anisotropyLevel);

#ifndef TRAP_HEADLESS_MODE
		/// @brief Notify the RendererAPI that the SwapChain needs to be resized.
		/// This function should be called inside FrameBufferResizeEvent callbacks.
		/// @param window Window that needs an updated SwapChain.
		/// @remark @headless This function is not available in headless mode.
		static void ResizeSwapChain(const Window& window);
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the GPU side frame time for the graphics queue.
		/// @param window Window to get frame time from.
		/// @return GPU Graphics frame time in milliseconds.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static f32 GetGPUGraphicsFrameTime(const Window& window);
#else
		/// @brief Retrieve the GPU side frame time for the graphics queue.
		/// @return GPU Graphics frame time in milliseconds.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] static f32 GetGPUGraphicsFrameTime();
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the GPU side frame time for the compute queue.
		/// @param window Window to get frame time from.
		/// @return GPU Compute frame time in milliseconds.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static f32 GetGPUComputeFrameTime(const Window& window);
#else
		/// @brief Retrieve the GPU side frame time for the compute queue.
		/// @return GPU Compute frame time in milliseconds.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] static f32 GetGPUComputeFrameTime();
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve internal rendering data.
		/// @return Windows internal rendering data.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static PerViewportData& GetViewportData(const Window& window);
#else
		/// @brief Retrieve internal rendering data.
		/// @return Internal rendering data.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] static PerViewportData& GetViewportData();
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Initialize the internal rendering data of the given window.
		/// @param window Window to initialize the internal rendering data for.
		/// @param VSyncEnabled Whether to enable or disable VSync.
		/// @remark @headless This function is not available in headless mode.
		virtual void InitPerViewportData(Window& window, bool VSyncEnabled) const = 0;
#else
		/// @brief Initialize the internal rendering data.
		/// @param width Width for the viewport.
		/// @param height Height for the viewport.
		/// @remark This function is only available in headless mode.
		virtual void InitPerViewportData(u32 width, u32 height) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Remove the internal rendering data of the given window.
		/// @param window Window to remove the internal rendering data from.
		/// @remark @headless This function is not available in headless mode.
		virtual void RemovePerViewportData(const Window& window) const = 0;
#else
		/// @brief Remove the internal rendering data.
		/// @remark This function is only available in headless mode.
		virtual void RemovePerViewportData() const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Wait for the GPU to idle.
		virtual void WaitIdle() const = 0;

		/// @brief Check if the system is Vulkan API capable.
		/// @return True if the system is Vulkan API capable, false otherwise.
		/// @note The first call to this function will run the Vulkan capability tester.
		[[nodiscard]] static bool IsVulkanCapable();

		/// @brief Enum bit flag for the different wave operations.
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
			PartitionedNV = BIT(8u)
		};

		/// @brief Enum describing the different types of queues.
		enum class QueueType : u32
		{
			Graphics = 0,
			Transfer,
			Compute
		};

		/// @brief Enum describing queue flags.
		enum class QueueFlag : u32
		{
			None = 0x0,
			DisableGPUTimeout = BIT(0u), //Disable the GPU timeout for this command queue (DirectX 12)
		};

		/// @brief Enum describing the different priorities for a queue (DirectX 12).
		enum class QueuePriority
		{
			Normal,
			High,
			GlobalRealtime
		};

		/// @brief Enum describing the status of a fence.
		enum class FenceStatus
		{
			Complete = 0,
			Incomplete,
			NotSubmitted
		};

		/// @brief Enum describing the different types of load actions.
		enum class LoadActionType : u32
		{
			DontCare,
			Load,
			Clear
		};

		/// @brief Enum describing the different types of store actions.
		enum class StoreActionType : u32
		{
			//Store is the most common use case so keep that as default
			Store,
			DontCare,
			None
		};

		/// @brief Enum describing the different types of cube textures.
		enum class TextureCubeType
		{
			Cross,
			//TODO
			//Equirectangular
		};

		/// @brief Enum describing the different sample counts.
		enum class SampleCount
		{
			One = BIT(0u),
			Two = BIT(1u),
			Four = BIT(2u),
			Eight = BIT(3u),
			Sixteen = BIT(4u)
		};

		/// @brief Enum describing the different anti aliasing methods.
		enum class AntiAliasing
		{
			Off,
			MSAA
		};

		/// @brief Enum describing the different GPU vendors.
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

		/// @brief Enum bit flags used by texture creation.
		enum class TextureCreationFlags : u32
		{
			//Default flag (Texture will use default allocation strategy decided by the API specific allocator)
			None = 0x0,
			//Texture will allocate its own memory (COMMITTED resource)
			OwnMemory = BIT(0u),
			//Use on-tile memory to store this texture
			OnTile = BIT(5u),
			//Force 2D instead of automatically determining dimension based on width, height, depth
			Force2D = BIT(7u),
			//Force 3D instead of automatically determining dimension based on width, height, depth
			Force3D = BIT(8u),
			//Display target //TODO Implement
			AllowDisplayTarget = BIT(9u),
			//Create an sRGB texture //TODO Implement
			SRGB = BIT(10u),
			//Create a normal map texture //TODO Implement
			NormalMap = BIT(11u),
			//Fast clear //TODO Implement
			FastClear = BIT(12u),
			//Fragment mask //TODO Implement
			FragMask = BIT(13u),
			//Create a storage texture
			Storage = BIT(14u)
		};

		/// @brief Enum describing the state of a resource.
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

		/// @brief Enum describing the type of a descriptor.
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

		/// @brief Enum describing the different memory usage types for a resource.
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
			GPUToCPU = 4
		};

		/// @brief Enum describing flags for the buffer creation.
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

		/// @brief Enum describing the different types of buffers for clearing.
		enum class ClearBufferType : u32
		{
			Color = BIT(0u),
			Depth = BIT(1u),
			Stencil = BIT(2u),
			Color_Depth = Color | Depth,
			Color_Stencil = Color | Stencil,
			Color_Depth_Stencil = Color | Stencil | Depth,
			Depth_Stencil = Depth | Stencil
		};

#ifndef TRAP_HEADLESS_MODE
		/// @brief Enum describing the different latency modes for NVIDIA Reflex.
		/// @remark @headless This function is not available in headless mode.
		enum class NVIDIAReflexLatencyMode : u32
		{
			Disabled,
			Enabled,
			EnabledBoost
		};

		enum class NVIDIAReflexLatencyMarker : u8
		{
			SimulationStart = 0u,
			SimulationEnd = 1u,
			RenderSubmitStart = 2u,
			RenderSubmitEnd = 3u,
			PresentStart = 4u,
			PresentEnd = 5u,
			InputSample = 6u,
			TriggerFlash = 7u,
			OutOfBandRenderSubmitStart = 8u,
			OutOfBandRenderSubmitEnd = 9u,
			OutOfBandPresentStart = 10u,
			OutOfBandPresentEnd = 11u
		};
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Enum describing the type of an indirect argument.
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

		/// @brief Enum describing the update frequency of a descriptor.
		/// @note: The same update frequency order should be used by shaders.
		enum class DescriptorUpdateFrequency : u32
		{
			Static = 0,
			Dynamic
		};

		/// @brief The maximum amount of descriptor sets that can be used.
		static constexpr u32 MaxDescriptorSets = 4;

		/// @brief Enum describing the different types of filtering used by samplers.
		enum class FilterType
		{
			Nearest = 0,
			Linear
		};

		/// @brief Enum describing the different modes of mip mapping used by samplers.
		enum class MipMapMode
		{
			Nearest = 0,
			Linear
		};

		/// @brief Enum describing the different address modes used by samplers.
		enum class AddressMode
		{
			Mirror,
			Repeat,
			ClampToEdge,
			ClampToBorder
		};

		/// @brief Enum describing the different compare modes used by depth/stencil testing and samplers.
		enum class CompareMode : u32
		{
			Never,
			Less,
			Equal,
			LessOrEqual,
			Greater,
			NotEqual,
			GreaterOrEqual,
			Always
		};

		/// @brief Enum describing the different shader stages making up a shader.
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

			AllGraphics = (Vertex | TessellationControl | TessellationEvaluation | Geometry | Fragment),

			//DirectX 12 name aliases
			Hull = TessellationControl,
			Domain = TessellationEvaluation,
			Pixel = Fragment
		};

		/// @brief Enum describing the overall type of a shader.
		enum class ShaderType
		{
			Graphics,
			Compute,
			// RayTracing //TODO
		};

		/// @brief Enum describing the different flags for root signature creation.
		enum class RootSignatureFlags : u32
		{
			//Default flag
			None = 0,
			//Local root signature used mainly in raytracing shaders
			Local = BIT(0u)
		};

		/// @brief Enum describing the different pipeline types.
		enum class PipelineType : u32
		{
			Compute,
			Graphics,
			RayTracing
		};

		/// @brief Enum describing the different data types used by index buffers.
		enum class IndexType
		{
			UInt32 = 0,
			UInt16
		};

		/// @brief Enum describing the different blend constants used for blending.
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
			OneMinusBlendFactor
		};

		/// @brief Enum describing the different blend modes/functions used for blending.
		enum class BlendMode : u32
		{
			Add,
			Subtract,
			ReverseSubtract,
			Min,
			Max
		};

		/// @brief Enum describing which render target to affect with the blend state.
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

		/// @brief Enum describing the different stencil/depth operations.
		enum class StencilOp : u32
		{
			Keep,
			SetZero,
			Replace,
			Invert,
			Increment,
			Decrement,
			IncrementSaturation,
			DecrementSaturation
		};

		/// @brief Enum describing the different cull modes.
		enum class CullMode : u32
		{
			None = 0,
			Back,
			Front,
			Both
		};

		/// @brief Enum describing the different front face winding orders.
		enum class FrontFace : u32
		{
			CounterClockwise = 0,
			Clockwise
		};

		/// @brief Enum describing the different fill modes.
		enum class FillMode : u32
		{
			Solid,
			Line,
			Point
		};

		/// @brief Enum describing flags for pipeline cache creation.
		enum class PipelineCacheFlags : u32
		{
			None = 0x0,
			ExternallySynchronized = BIT(0u)
		};

		/// @brief Enum describing the different primitive topologies.
		enum class PrimitiveTopology
		{
			PointList = 0,
			LineList,
			LineStrip,
			TriangleList,
			TriangleStrip,
			PatchList
		};

		/// @brief Enum describing the different vertex attribute rates.
		enum class VertexAttributeRate
		{
			Vertex = 0,
			Instance = 1
		};

		/// @brief Enum describing the different types of queries.
		enum class QueryType
		{
			Timestamp = 0,
			PipelineStatistics,
			Occlusion
		};

		/// @brief Enum describing the different presentation statuses.
		enum class PresentStatus
		{
			Success = 0,
			DeviceReset = 1,
			Failed = 2,
			OutOfDate = 3
		};

		/// @brief Enum describing the different sampler ranges used by YUV conversion samplers.
		enum class SamplerRange
		{
			Full = 0,
			Narrow = 1
		};

		/// @brief Enum describing the different sampler models used by YUV conversion samplers.
		enum class SamplerModelConversion
		{
			RGBIdentity = 0,
			YCBCRIdentity = 1,
			YCBCR709 = 2,
			YCBCR601 = 3,
			YCBCR2020 = 4
		};

		/// @brief Enum describing the different sample locations used by YUV conversion samplers.
		enum class SampleLocation
		{
			Cosited = 0,
			Midpoint = 1
		};

		/// @brief Enum describing the shading rates used by fragment/pixel shaders.
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

		/// @brief Enum describing how to combine different shading rates.
		enum class ShadingRateCombiner : u32
		{
			Passthrough = BIT(0u),
			Override = BIT(1u),
			Min = BIT(2u),
			Max = BIT(3u),
			Sum = BIT(4u)
		};

		/// @brief Enum describing the shading rate capabilities supported by the GPU.
		enum class ShadingRateCaps : u32
		{
			NotSupported = 0x0,
			PerDraw = BIT(0u),
			PerTile = BIT(1u),

			//Vulkan only
			PerPrimitive = BIT(2u)
		};

		/// @brief Enum describing flags that change the behaviour of a command pool.
		enum class CommandPoolCreateFlags : u32
		{
			None = 0x0,
			Transient = BIT(0u), //Indicate that the allocated command buffers will be short lived
			ResetCommandBuffer = BIT(1u) //Allows to reset command buffers individually to initial state.
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

		/// @brief Description of a subresource.
		/// Used to update a existing resource.
		struct SubresourceDataDesc
		{
			u64 SrcOffset;
			u32 MipLevel;
			u32 ArrayLayer;

			u32 RowPitch;
			u32 SlicePitch;
		};

		/// @brief Description of a render target.
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
			TRAP::Graphics::RendererAPI::SampleCount SampleCount = RendererAPI::SampleCount::One;
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

		/// @brief Description of a texture.
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
			TRAP::Graphics::RendererAPI::SampleCount SampleCount = RendererAPI::SampleCount::One;
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

		/// @brief Description for a texture load.
		struct TextureLoadDesc
		{
			//Target to load texture info into.
			TRAP::Graphics::Texture* Texture{};
			//Load empty texture
			TextureDesc* Desc{};
			//Filepath with extension
			std::vector<std::filesystem::path> Filepaths;
			//Loaded images (used to load in memory images, if set Filepaths are ignored)
			std::vector<const Image*> Images{};
			//Following is ignored if Desc != nullptr.
			//Desc->Flags will be considered instead.
			TextureCreationFlags CreationFlag = TextureCreationFlags::None;
			//Is Texture cubemap?
			bool IsCubemap{};
			//Type of cubemap texture (NullOpt + IsCubemap == true means Multi-file cube texture)
			TRAP::Optional<TextureCubeType> Type = TRAP::NullOpt;
		};

		/// @brief Description of a buffer.
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

		/// @brief Description of a sampler.
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
			//Debug name used in GPU-profile
			std::string Name{};

			/// @brief Description of YCbCr(YUV) conversion sampler.
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

		/// @brief Description of a binary shader stage.
		struct BinaryShaderStageDesc
		{
			//Shader data
			std::vector<u32> ByteCode{};
		};

		/// @brief Description of a binary shader.
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

		/// @brief Description of a root signature.
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

		/// @brief Struct containing information about a descriptor.
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

		/// @brief Description of a descriptor set.
		struct DescriptorSetDesc
		{
			//Root signature to use
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			//Descriptor set index
			u32 Set;
			//Max amount of sets to allocate
			u32 MaxSets{};
		};

		/// @brief Description of a command pool.
		struct CommandPoolDesc
		{
			//Queue to be used by the command pool
			TRAP::Ref<TRAP::Graphics::Queue> Queue;
			CommandPoolCreateFlags CreateFlags = CommandPoolCreateFlags::None;
			//Debug name used in GPU-profile
			std::string Name{};
		};

		/// @brief Description of a queue.
		struct QueueDesc
		{
			//Type of queue
			QueueType Type{};
			//Queue creation flags
			QueueFlag Flag{};
			//Priority for the queue (DirectX 12)
			QueuePriority Priority{};
			//Debug name used in GPU-profile
			std::string Name{};
		};

		/// @brief Read range used for buffer mapping.
		struct ReadRange
		{
			//Offset from the start of the buffer
			u64 Offset{};
			//Range to read (DirectX 12)
			u64 Range{};
		};

		/// @brief Description for a queue submission.
		struct QueueSubmitDesc
		{
			//Command buffers to submit
			std::vector<std::reference_wrapper<CommandBuffer>> Cmds{};
			//Fence to signal
			TRAP::Ref<Fence> SignalFence{};
			//Semaphores to wait on
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
			//Semaphores to signal
			std::vector<TRAP::Ref<Semaphore>> SignalSemaphores{};

			//NVIDIA-Reflex Present ID (Optional)
			TRAP::Optional<u64> ReflexPresentID = TRAP::NullOpt;
		};

		/// @brief Description of a blend state.
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

		/// @brief Description of a depth state.
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

		/// @brief Description of a rasterizer state.
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

		/// @brief Description of a pipeline cache.
		struct PipelineCacheDesc
		{
			//Pipeline data
			std::vector<u8> Data{};
			//Pipeline cache creation flags
			PipelineCacheFlags Flags{};
			//Debug name used in GPU profile
			std::string Name{};
		};

		/// @brief Description of a pipeline cache to load.
		struct PipelineCacheLoadDesc
		{
			//Path to pipeline cache
			std::filesystem::path Path{};
			//Pipeline cache creation flags
			PipelineCacheFlags Flags{};
			//Debug name used in GPU profile
			std::string Name{};
		};

		/// @brief Description of a compute pipeline.
		struct ComputePipelineDesc
		{
			//Shader to use
			Shader* ShaderProgram{};
			//Root signature to use
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
		};

		/// @brief Description of a single vertex attribute.
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

		/// @brief Description of a vertex layout.
		struct VertexLayout
		{
			//Amount of attributes in the layout
			u32 AttributeCount{};
			//Attributes
			std::array<VertexAttribute, 15> Attributes{};
		};

		/// @brief Description of a graphics pipeline.
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
			TRAP::Graphics::RendererAPI::SampleCount SampleCount = RendererAPI::SampleCount::One;
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

		/// @brief Description of a RayTracing pipeline.
		struct RayTracingPipelineDesc
		{
			//TODO
		};

		/// @brief Description of a pipeline.
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

		/// @brief Description of a query pool.
		struct QueryPoolDesc
		{
			//Type of query to hold
			QueryType Type{};
			//Max amount of queries to manage
			u32 QueryCount{};
			//Debug name used in GPU profile
			std::string Name{};
		};

		/// @brief Description of a query.
		struct QueryDesc
		{
			//Index of the query into the query pool
			u32 Index{};
		};

		/// @brief Struct holding indirect draw arguments.
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

		/// @brief Struct holding indirect indexed draw arguments.
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

		/// @brief Struct holding indirect dispatch arguments.
		struct IndirectDispatchArguments
		{
			//X work group size
			u32 GroupCountX{};
			//Y work group size
			u32 GroupCountY{};
			//Z work group size
			u32 GroupCountZ{};
		};

		/// @brief Struct holding indirect descriptor argument.
		struct IndirectArgumentDescriptor
		{
			//Type of indirect argument
			IndirectArgumentType Type{};
			//Name of descriptor
			std::string Name{};
			//Index of descriptor
			u32 Index{};
		};

		/// @brief Description of a command signature.
		struct CommandSignatureDesc
		{
			//Root signature.
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			//Indirect argument descriptions
			std::vector<IndirectArgumentDescriptor> ArgDescs{};
			//Set to true if indirect argument struct should not be aligned to 16 bytes
			bool Packed{};
		};

		/// @brief Description of a swapchain.
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

		/// @brief Struct holding a render target barrier.
		struct RenderTargetBarrier
		{
			//Render target
			std::reference_wrapper<const TRAP::Graphics::RenderTarget> RenderTarget;
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

		/// @brief Struct holding a buffer barrier.
		struct BufferBarrier
		{
			//Buffer
			std::reference_wrapper<const TRAP::Graphics::Buffer> Buffer;
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

		/// @brief Struct holding a texture barrier.
		struct TextureBarrier
		{
			//Texture
			std::reference_wrapper<const TRAP::Graphics::Texture> Texture;
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

		using DescriptorResource = std::variant<std::vector<const TRAP::Graphics::Texture*>,
		                                        std::vector<const Sampler*>,
				                                std::vector<const Buffer*>,
												std::vector<Pipeline*>,
				                                std::vector<DescriptorSet*>>;

		/// @brief Struct holding a data of a descriptor.
		struct DescriptorData
		{
			//User can either set name of descriptor or index (index in RootSignature->Descriptors array)
			//Name of descriptor
			std::string Name{};
			/// @brief Range(s) to bind (buffer, offset, size)
			struct BufferOffset
			{
				//Offset to bind the buffer descriptor
				std::vector<u64> Offsets{};
				//Size of the buffer to bind
				std::vector<u64> Sizes{};
			};

			/// @brief Descriptor set buffer extraction options
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
			DescriptorResource Resource{std::vector<const TRAP::Graphics::Texture*>()}; //TODO RayTracing acceleration structure

			//Number of resources in the descriptor(applies to array of textures, buffers, ...)
			u32 Count{};
			//Index into RootSignature->Descriptors array
			u32 Index = std::numeric_limits<u32>::max();
		};

		/// @brief Description of a queue presentation.
		struct QueuePresentDesc
		{
			//Swapchain to preesent
			TRAP::Ref<TRAP::Graphics::SwapChain> SwapChain{};
			//Semaphores to wait on before presenting
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
			//Render target of the swapchain to present
			u32 Index{};
		};

		/// @brief Description of actions to perform on load.
		struct LoadActionsDesc
		{
			//Action to perform on the color attachment(s) on load.
			std::vector<LoadActionType> LoadActionsColor{};
			//Action to perform on the depth attachment on load.
			LoadActionType LoadActionDepth{};
			//Action to perform on the stencil attachment on load.
			LoadActionType LoadActionStencil{};
			//Clear color(s)
			std::vector<RendererAPI::Color> ClearColorValues{};
			//Clear depth/stencil value
			RendererAPI::DepthStencil ClearDepthStencil{};
			//Action to perform on the color attachment(s) on store.
			std::vector<StoreActionType> StoreActionsColor{};
			//Action to perform on the depth attachment on store.
			StoreActionType StoreActionDepth{};
			//Action to perform on the stencil attachment on store.
			StoreActionType StoreActionStencil{};
		};

		/// @brief Map resolving a name to its descriptor index in a root signature.
		using DescriptorIndexMap = TRAP::Utils::UnorderedStringMap<u32>;

		enum class MappedRangeFlags : u32
		{
			None = 0,
			UnMapBuffer = BIT(0u),
			TempBuffer = BIT(1u)
		};

		/// @brief Struct holding data about a mapped memory range.
		struct MappedMemoryRange
		{
			//Pointer to mapped buffer data with offset already applied to
			std::span<u8> Data{};
			//Mapped buffer
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer = nullptr;
			//Offset in the buffer to start at
			u64 Offset = 0; //TODO This may be unnecessary
			//Mapped size
			u64 Size = 0; //TODO This may be unnecessary
			MappedRangeFlags Flags = MappedRangeFlags::None;
		};

		/// @brief Description for a texture update.
		/// @note Only use for procedural textures which are created on CPU (noise textures, font texture, ...)
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
			std::span<u8> MappedData{};
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

		/// @brief Description for the resource loader.
		struct ResourceLoaderDesc
		{
			//Size for each staging buffer
			u64 BufferSize;
			//Amount of staging buffers to create
			u32 BufferCount;
		};

		/// @brief Description for a buffer load.
		struct BufferLoadDesc
		{
			//Output buffer with uploaded data.
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer = nullptr;
			//Data to upload
			const void* Data = nullptr;
			//Description for the buffer
			BufferDesc Desc{};

			//Force Reset Buffer to 0
			bool ForceReset = false;
		};

		/// @brief Description for a buffer update.
		struct BufferUpdateDesc
		{
			//Buffer to update
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
			//Offset into buffer to update data at
			u64 DstOffset;
			//Size of data to update
			u64 Size;

			//To be filled by the caller
			std::span<u8> MappedData;

			//Internal
			struct
			{
				MappedMemoryRange MappedRange;
			} Internal;
		};

		/// @brief Struct holding data about a GPUs features, limits and other properties.
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
			u32 MaxColorRenderTargets;

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

			//NVIDIA Reflex
			bool ReflexSupported;
		} GPUSettings{};

		static constexpr u32 ImageCount = 3; //Triple Buffered

#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the image index currently used for rendering from the given window.
		/// @param window Window to retrieve image index from.
		/// @return Image index.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] static u32 GetCurrentImageIndex(const TRAP::Window& window);
#else
		/// @brief Retrieve the image index currently used for rendering.
		/// @return Image index.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] static u32 GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	protected:
		static TRAP::Scope<RendererAPI> s_Renderer;
		inline constinit static RenderAPI s_RenderAPI = RenderAPI::NONE;
		static TRAP::Scope<API::ResourceLoader> s_ResourceLoader;

		static TRAP::Ref<DescriptorPool> s_descriptorPool;
		static TRAP::Ref<Queue> s_graphicQueue;
		static TRAP::Ref<Queue> s_computeQueue;
		static TRAP::Ref<Queue> s_transferQueue;

		inline constinit static RendererAPI::SampleCount s_newSampleCount = RendererAPI::SampleCount::One;
		inline constinit static RendererAPI::AntiAliasing s_newAntiAliasing = RendererAPI::AntiAliasing::Off;
		inline constinit static RendererAPI::SampleCount s_Anisotropy = RendererAPI::SampleCount::Sixteen;

		inline constinit static TRAP::Utils::UUID s_newGPUUUID{};

	public:
		enum class PerWindowState
		{
			PreUpdate,
			PostUpdate,
		};

		/// @brief Per window data used for rendering.
		//TODO This needs a refactoring the struct got way too big!
		struct PerViewportData
		{
			/// @brief Constructor.
			PerViewportData() = default;

			/// @brief Destructor.
			~PerViewportData();

			/// @brief Copy constructor.
			consteval PerViewportData(const PerViewportData&) noexcept = delete;
			/// @brief Move constructor.
			PerViewportData(PerViewportData&&) noexcept = default;
			/// @brief Copy assignment operator.
			consteval PerViewportData& operator=(const PerViewportData &) noexcept = delete;
			/// @brief Move assignment operator.
			PerViewportData& operator=(PerViewportData &&) noexcept = default;

			[[nodiscard]] TRAP::Ref<TRAP::Graphics::RenderTarget> GetCurrentSwapchainRenderTarget() const;

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
			std::array<TRAP::Ref<Semaphore>, ImageCount> ImageAcquiredSemaphores;
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
			TRAP::Ref<RenderTarget> DepthStencilTarget = nullptr;

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
			u32 PreviousSwapChainImageIndex{};
			RendererAPI::AntiAliasing CurrentAntiAliasing = RendererAPI::AntiAliasing::Off;
			RendererAPI::SampleCount CurrentSampleCount = RendererAPI::SampleCount::One;

			RendererAPI::Color ClearColor{0.1, 0.1, 0.1, 1.0};
			RendererAPI::DepthStencil ClearDepthStencil{0.0f, 0};

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

#if!defined(TRAP_HEADLESS_MODE)
			//NVIDIA Reflex
			NVIDIAReflexLatencyMode ReflexLatencyMode = NVIDIAReflexLatencyMode::Disabled;
			u32 ReflexFPSLimit = 0u;
			TRAP::Ref<Semaphore> ReflexSemaphore{};
#endif /*!TRAP_HEADLESS_MODE*/
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
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::CommandPoolCreateFlags)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::MappedRangeFlags)

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::RendererAPI::SetNewGPU(const TRAP::Utils::UUID& GPUUUID) noexcept
{
	s_newGPUUUID = GPUUUID;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::RenderAPI>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RenderAPI renderAPI,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(renderAPI)
        {
        case TRAP::Graphics::RenderAPI::Vulkan:
            enumStr = "Vulkan";
            break;
        case TRAP::Graphics::RenderAPI::NONE:
            enumStr = "NONE";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::RendererAPI::AntiAliasing>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RendererAPI::AntiAliasing antiAliasing,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(antiAliasing)
        {
        case TRAP::Graphics::RendererAPI::AntiAliasing::Off:
            enumStr = "Off";
            break;
        case TRAP::Graphics::RendererAPI::AntiAliasing::MSAA:
            enumStr = "MSAA";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::RendererAPI::SampleCount>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RendererAPI::SampleCount sampleCount,
	                                            fmt::format_context& ctx)
    {
		u32 samples = 1;

        switch(sampleCount)
        {
        case TRAP::Graphics::RendererAPI::SampleCount::One:
			[[fallthrough]];
        case TRAP::Graphics::RendererAPI::SampleCount::Two:
			[[fallthrough]];
        case TRAP::Graphics::RendererAPI::SampleCount::Four:
			[[fallthrough]];
        case TRAP::Graphics::RendererAPI::SampleCount::Eight:
			[[fallthrough]];
        case TRAP::Graphics::RendererAPI::SampleCount::Sixteen:
			samples = std::to_underlying(sampleCount);
			break;
        }

		return fmt::format_to(ctx.out(), "{}", samples);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::RendererAPI::GPUVendor>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RendererAPI::GPUVendor gpuVendor,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(gpuVendor)
        {
        case TRAP::Graphics::RendererAPI::GPUVendor::Unknown:
            enumStr = "Unknown";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::AMD:
            enumStr = "AMD";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::ImgTec:
            enumStr = "ImgTec";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::NVIDIA:
            enumStr = "NVIDIA";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::ARM:
            enumStr = "ARM";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::Broadcom:
            enumStr = "Broadcom";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::Qualcomm:
            enumStr = "Qualcomm";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::Intel:
            enumStr = "Intel";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::Apple:
            enumStr = "Apple";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::Vivante:
            enumStr = "Vivante";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::VeriSilicon:
            enumStr = "VeriSilicon";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::Kazan:
            enumStr = "Kazan";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::Codeplay:
            enumStr = "Codeplay";
            break;
		case TRAP::Graphics::RendererAPI::GPUVendor::Mesa:
            enumStr = "Mesa";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
template<>
struct fmt::formatter<TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode latencyMode,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(latencyMode)
        {
        case TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode::Disabled:
            enumStr = "Disabled";
            break;
        case TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode::Enabled:
            enumStr = "Enabled";
            break;
        case TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode::EnabledBoost:
            enumStr = "Enabled+Boost";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::RendererAPI::ResourceState>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RendererAPI::ResourceState resourceState,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(resourceState)
        {
        case TRAP::Graphics::RendererAPI::ResourceState::Undefined:
            enumStr = "Undefined";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::VertexAndConstantBuffer:
            enumStr = "VertexAndConstantBuffer";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::IndexBuffer:
            enumStr = "IndexBuffer";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::RenderTarget:
            enumStr = "RenderTarget";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess:
            enumStr = "UnorderedAccess";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::DepthWrite:
            enumStr = "DepthWrite";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::DepthRead:
            enumStr = "DepthRead";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::NonPixelShaderResource:
            enumStr = "NonPixelShaderResource";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource:
            enumStr = "PixelShaderResource";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::ShaderResource:
            enumStr = "ShaderResource";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::StreamOut:
            enumStr = "StreamOut";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::IndirectArgument:
            enumStr = "IndirectArgument";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::CopyDestination:
            enumStr = "CopyDestination";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::CopySource:
            enumStr = "CopySource";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::GenericRead:
            enumStr = "GenericRead";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::Present:
            enumStr = "Present";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::Common:
            enumStr = "Common";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::RayTracingAccelerationStructure:
            enumStr = "RayTracingAccelerationStructure";
            break;
        case TRAP::Graphics::RendererAPI::ResourceState::ShadingRateSource:
            enumStr = "ShadingRateSource";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::RendererAPI::QueueType>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RendererAPI::QueueType queueType,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(queueType)
        {
        case TRAP::Graphics::RendererAPI::QueueType::Graphics:
            enumStr = "Graphics";
            break;
        case TRAP::Graphics::RendererAPI::QueueType::Transfer:
            enumStr = "Transfer";
            break;
        case TRAP::Graphics::RendererAPI::QueueType::Compute:
            enumStr = "Compute";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::RendererAPI::ShaderStage>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RendererAPI::ShaderStage shaderStage,
	                                            fmt::format_context& ctx)
    {
		if(shaderStage == TRAP::Graphics::RendererAPI::ShaderStage::None)
			return ctx.out();

        std::string enumStr{};

		if((shaderStage & TRAP::Graphics::RendererAPI::ShaderStage::Vertex) != TRAP::Graphics::RendererAPI::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "Vertex";
		}
		if((shaderStage & TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl) != TRAP::Graphics::RendererAPI::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "TessellationControl";
		}
		if((shaderStage & TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation) != TRAP::Graphics::RendererAPI::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "TessellationEvaluation";
		}
		if((shaderStage & TRAP::Graphics::RendererAPI::ShaderStage::Geometry) != TRAP::Graphics::RendererAPI::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "Geometry";
		}
		if((shaderStage & TRAP::Graphics::RendererAPI::ShaderStage::Fragment) != TRAP::Graphics::RendererAPI::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "Fragment";
		}
		if((shaderStage & TRAP::Graphics::RendererAPI::ShaderStage::Compute) != TRAP::Graphics::RendererAPI::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "Compute";
		}
		if((shaderStage & TRAP::Graphics::RendererAPI::ShaderStage::RayTracing) != TRAP::Graphics::RendererAPI::ShaderStage::None)
		{
			if(!enumStr.empty())
				enumStr += '/';
			enumStr += "RayTracing";
		}

		if(enumStr.empty())
		{
			TRAP_ASSERT(false, "fmt::formatter<TRAP::Graphics::RendererAPI::ShaderStage>: Missing enum value!");
			enumStr = "<MISSING ENUM VALUE>";
		}

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::RendererAPI::ShaderType>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RendererAPI::ShaderType shaderType,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};

		switch(shaderType)
		{
		case TRAP::Graphics::RendererAPI::ShaderType::Graphics:
			enumStr = "Graphics";
			break;

		case TRAP::Graphics::RendererAPI::ShaderType::Compute:
			enumStr = "Compute";
			break;
		}

		if(enumStr.empty())
		{
			TRAP_ASSERT(false, "fmt::formatter<TRAP::Graphics::RendererAPI::ShaderType>: Missing enum value!");
			enumStr = "<MISSING ENUM VALUE>";
		}

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Graphics::RendererAPI::ShadingRate>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Graphics::RendererAPI::ShadingRate shadingRate,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};

		switch(shadingRate)
		{
		case TRAP::Graphics::RendererAPI::ShadingRate::NotSupported:
			enumStr = "1x1";
			break;
		case TRAP::Graphics::RendererAPI::ShadingRate::Full:
			enumStr = "1x1";
			break;
		case TRAP::Graphics::RendererAPI::ShadingRate::Half:
			enumStr = "2x2";
			break;
		case TRAP::Graphics::RendererAPI::ShadingRate::Quarter:
			enumStr = "4x4";
			break;
		case TRAP::Graphics::RendererAPI::ShadingRate::Eighth:
			enumStr = "8x8";
			break;
		case TRAP::Graphics::RendererAPI::ShadingRate::OneXTwo:
			enumStr = "1x2";
			break;
		case TRAP::Graphics::RendererAPI::ShadingRate::TwoXOne:
			enumStr = "2x1";
			break;
		case TRAP::Graphics::RendererAPI::ShadingRate::TwoXFour:
			enumStr = "2x4";
			break;
		case TRAP::Graphics::RendererAPI::ShadingRate::FourXTwo:
			enumStr = "4x2";
			break;
		}

		if(enumStr.empty())
		{
			TRAP_ASSERT(false, "fmt::formatter<TRAP::Graphics::RendererAPI::ShadingRate>: Missing enum value!");
			enumStr = "<MISSING ENUM VALUE>";
		}

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
template<>
[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode TRAP::Utils::String::ConvertToType(const std::string& input)
{
    if(Utils::String::CompareAnyCase("Enabled", input))
        return Graphics::RendererAPI::NVIDIAReflexLatencyMode::Enabled;
    if(Utils::String::CompareAnyCase("Enabled+Boost", input))
        return Graphics::RendererAPI::NVIDIAReflexLatencyMode::EnabledBoost;
    if(Utils::String::CompareAnyCase("Disabled", input))
        return Graphics::RendererAPI::NVIDIAReflexLatencyMode::Disabled;

    TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::RendererAPI::NVIDIAReflexLatencyMode!");
    return Graphics::RendererAPI::NVIDIAReflexLatencyMode::Disabled;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] constexpr TRAP::Graphics::RenderAPI TRAP::Utils::String::ConvertToType(const std::string& input)
{
    if (Utils::String::CompareAnyCase("Vulkan", input) || Utils::String::CompareAnyCase("VulkanAPI", input))
        return Graphics::RenderAPI::Vulkan;

    if (Utils::String::CompareAnyCase("NONE", input))
        return Graphics::RenderAPI::NONE;

    TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::RenderAPI!");
    return Graphics::RenderAPI::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::AntiAliasing TRAP::Utils::String::ConvertToType(const std::string& input)
{
    if (Utils::String::CompareAnyCase("Off", input))
        return Graphics::RendererAPI::AntiAliasing::Off;

    if (Utils::String::CompareAnyCase("MSAA", input))
        return Graphics::RendererAPI::AntiAliasing::MSAA;

    TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::RendererAPI::AntiAliasing!");
    return Graphics::RendererAPI::AntiAliasing::Off;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::SampleCount TRAP::Utils::String::ConvertToType(const std::string& input)
{
    if (Utils::String::CompareAnyCase("1", input))
        return Graphics::RendererAPI::SampleCount::One;

    if (Utils::String::CompareAnyCase("2", input))
        return Graphics::RendererAPI::SampleCount::Two;

    if (Utils::String::CompareAnyCase("4", input))
        return Graphics::RendererAPI::SampleCount::Four;

    if (Utils::String::CompareAnyCase("8", input))
        return Graphics::RendererAPI::SampleCount::Eight;

    if (Utils::String::CompareAnyCase("16", input))
        return Graphics::RendererAPI::SampleCount::Sixteen;

    TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::RendererAPI::SampleCount!");
    return Graphics::RendererAPI::SampleCount::One;
}

#endif /*TRAP_RENDERERAPI_H*/

#ifndef TRAP_RENDERERAPI_H
#define TRAP_RENDERERAPI_H

#include <variant>
#include <unordered_map>

#include "Utils/Bit.h"
#include "Utils/Map.h"
#include "Utils/SmartPtr.h"
#include "Window/Window.h"
#include "ImageFormat.h"
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"
#include "Utils/Utils.h"
#include "Maths/Types.h"
#include "Maths/Vec3.h"
#include "Utils/Optional.h"
#include "Graphics/API/RendererAPI/Types.h"

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
	class RendererAPI
	{
	public:
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

		/// @brief Set the FPS limit for AMD Anti Lag.
		/// @param limit FPS target to limit to.
		/// @note This function affects all windows.
		/// @warning Do not call this function in user code!
		///          Use TRAP::Applicaton::SetFPSLimit() instead.
		///          This function is only used internally for AMD Anti Lag.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetAntiLagFPSLimit(u32 limit) = 0;
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
									  LoadActionsDesc* loadActions /*= nullptr*/,
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
									  LoadActionsDesc* loadActions /*= nullptr*/,
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
									   LoadActionsDesc* loadActions /*= nullptr*/,
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
									   LoadActionsDesc* loadActions /*= nullptr*/,
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
		virtual void ResourceBufferBarrier(const BufferBarrier& bufferBarrier,
										   QueueType queueType /*= QueueType::Graphics*/,
								           const Window& window) const = 0;
#else
		/// @brief Add a resource barrier (memory dependency).
		/// @param bufferBarrier Buffer barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		virtual void ResourceBufferBarrier(const BufferBarrier& bufferBarrier,
										   QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add resource barriers (memory dependencies) for the given window.
		/// @param bufferBarriers Buffer barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barriers for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceBufferBarriers(const std::vector<BufferBarrier>& bufferBarriers,
											QueueType queueType /*= QueueType::Graphics*/,
									        const Window& window) const = 0;
#else
		/// @brief Add resource barriers (memory dependencies).
		/// @param bufferBarriers Buffer barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		virtual void ResourceBufferBarriers(const std::vector<BufferBarrier>& bufferBarriers,
											QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add a resource barrier (memory dependency) for the given window.
		/// @param textureBarrier Texture barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barrier for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceTextureBarrier(const TextureBarrier& textureBarrier,
											QueueType queueType /*= QueueType::Graphics*/,
									        const Window& window) const = 0;
#else
		/// @brief Add a resource barrier (memory dependency).
		/// @param textureBarrier Texture barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		virtual void ResourceTextureBarrier(const TextureBarrier& textureBarrier,
											QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add resource barriers (memory dependencies) for the given window.
		/// @param textureBarriers Texture barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barriers for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceTextureBarriers(const std::vector<TextureBarrier>& textureBarriers,
											 QueueType queueType /*= QueueType::Graphics*/,
									         const Window& window) const = 0;
#else
		/// @brief Add resource barriers (memory dependencies).
		/// @param textureBarriers Texture barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		virtual void ResourceTextureBarriers(const std::vector<TextureBarrier>& textureBarriers,
											 QueueType queueType /*= QueueType::Graphics*/) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add a resource barrier (memory dependency) for the given window.
		/// @param renderTargetBarrier Render target barrier.
		/// @param window Window to add the barrier for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceRenderTargetBarrier(const RenderTargetBarrier& renderTargetBarrier,
									             const Window& window) const = 0;
#else
		/// @brief Add a resource barrier (memory dependency).
		/// @param renderTargetBarrier Render target barrier.
		/// @remark This function is only available in headless mode.
		virtual void ResourceRenderTargetBarrier(const RenderTargetBarrier& renderTargetBarrier) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add resource barriers (memory dependencies) for the given window.
		/// @param renderTargetBarriers Render target barriers.
		/// @param window Window to add the barriers for.
		/// @remark @headless This function is not available in headless mode.
		virtual void ResourceRenderTargetBarriers(const std::vector<RenderTargetBarrier>& renderTargetBarriers,
									              const Window& window) const = 0;
#else
		/// @brief Add resource barriers (memory dependencies).
		/// @param renderTargetBarriers Render target barriers.
		/// @remark This function is only available in headless mode.
		virtual void ResourceRenderTargetBarriers(const std::vector<RenderTargetBarrier>& renderTargetBarriers) const = 0;
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

		/// @brief Set a timestamp for AMD Anti Lag.
		/// @param marker Enum value of the marker to set.
		/// @param window Window to set the timestamp for.
		/// @remark @headless This function is not available in headless mode.
		virtual void AntiLagMarker(AMDAntiLagMarker marker, const Window& window) const = 0;
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
		/// @brief Set the NVIDIA-Reflex latency mode.
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

		/// @brief Set the AMD Anti Lag latency mode.
		/// @param mode Anti lag mode.
		/// @param window Window to set mode for.
		/// @remark @win32 AMDAntiLagModes are only available on AMD hardware.
		/// @remark @headless This function is not available in headless mode.
		virtual void SetAntiLagMode(AMDAntiLagMode mode, const Window& window) = 0;
		/// @brief Retrieve the currently used mode for AMD Anti Lag.
		/// @param window Window to retrieve mode for.
		/// @return Used mode.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] virtual AMDAntiLagMode GetAntiLagMode(const Window& window) const = 0;
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
							   TRAP::Graphics::ResourceState oldLayout,
							   TRAP::Graphics::ResourceState newLayout,
							   TRAP::Graphics::QueueType queueType = QueueType::Graphics);
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

		/// @brief The maximum amount of descriptor sets that can be used.
		static constexpr u32 MaxDescriptorSets = 4;

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
			TRAP::Graphics::WaveOpsSupportFlags WaveOpsSupportFlags;

			u32 ROVsSupported;
			bool MultiDrawIndirectSupported;
			bool TessellationSupported;
			bool GeometryShaderSupported;
			bool FillModeNonSolid;
			bool SurfaceSupported;
			bool PresentSupported;
			bool SampleRateShadingSupported;

			//Variable rate shading capabilities
			TRAP::Graphics::ShadingRate ShadingRates;
			TRAP::Graphics::ShadingRateCaps ShadingRateCaps;
			TRAP::Graphics::ShadingRateCombiner ShadingRateCombiner;
			u32 ShadingRateTexelWidth;
			u32 ShadingRateTexelHeight;

			bool PresentIDSupported;

			//NVIDIA Reflex
			bool ReflexSupported;
			//AMD Anti Lag
			bool AntiLagSupported;
		} GPUSettings{};

		static constexpr u32 ImageCount = 3u; //Triple Buffered

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

		inline constinit static SampleCount s_newSampleCount = SampleCount::One;
		inline constinit static AntiAliasing s_newAntiAliasing = AntiAliasing::Off;
		inline constinit static SampleCount s_Anisotropy = SampleCount::Sixteen;

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
			AntiAliasing CurrentAntiAliasing = AntiAliasing::Off;
			SampleCount CurrentSampleCount = SampleCount::One;

			Color ClearColor{0.1, 0.1, 0.1, 1.0};
			DepthStencil ClearDepthStencil{0.0f, 0};

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
			TRAP::Ref<Semaphore> ReflexSemaphore{};
			//AMD Anti Lag
			AMDAntiLagMode AntiLagMode = AMDAntiLagMode::DriverControl;
			//NVIDIA Reflex / AMD Anti Lag
			u32 FPSLimit = 0u;
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

constexpr void TRAP::Graphics::RendererAPI::SetNewGPU(const TRAP::Utils::UUID& GPUUUID) noexcept
{
	s_newGPUUUID = GPUUUID;
}

#endif /*TRAP_RENDERERAPI_H*/

#ifndef TRAP_VULKANRENDERER_H
#define TRAP_VULKANRENDERER_H

#include <thread>

#include "Graphics/API/ShaderReflection.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Objects/VulkanDebug.h"
#include "Utils/Concurrency/Safe.h"

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
		/// @brief Constructor.
		constexpr VulkanRenderer() noexcept;
		/// @brief Destructor.
		~VulkanRenderer() override;

		/// @brief Copy constructor.
		consteval VulkanRenderer(const VulkanRenderer&) = delete;
		/// @brief Copy assignment operator.
		consteval VulkanRenderer& operator=(const VulkanRenderer&) = delete;
		/// @brief Move constructor.
		constexpr VulkanRenderer(VulkanRenderer&&) noexcept = delete;
		/// @brief Move assignment operator.
		constexpr VulkanRenderer& operator=(VulkanRenderer&&) noexcept = delete;

		/// @brief Initialize the internal Vulkan renderer.
		/// @param gameName Name of the game.
		void InitInternal(std::string_view gameName) override;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Flush renderer for the given window.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Presents the rendered image to the screen.
		/// 4. Starts graphics and compute recording for the next frame.
		/// @param window Window to flush.
		/// @remark @headless This function is not available in headless mode.
		void Flush(const Window& window) const override;
#else
		/// @brief Flush renderer.
		///
		/// 1. Stops graphics and compute recording.
		/// 2. Submits the graphics and compute commands.
		/// 3. Starts graphics and compute recording for the next frame.
		/// @remark This function is only available in headless mode.
		void Flush() const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Dispatch to the given window.
		/// @param workGroupElements Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		///
		/// @param window Window to Dispatch.
		/// @remark @headless This function is not available in headless mode.
		void Dispatch(std::array<u32, 3> workGroupElements, const Window& window) const override;
#else
		/// @brief Dispatch.
		/// @param workGroupElements Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// @remark This function is only available in headless mode.
		void Dispatch(std::array<u32, 3> workGroupElements) const override;
#endif /*TRAP_HEADLESS_MODE*/
		//TODO DispatchIndirect

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the VSync state for the given window.
		/// @param vsync Enable or disable VSync.
		/// @param window Window to set VSync for.
		/// @remark @headless This function is not available in headless mode.
		void SetVSync(bool vsync, const Window& window) const override;
#endif /*TRAP_HEADLESS_MODE*/

#if !defined(TRAP_HEADLESS_MODE)
		/// @brief Set the FPS limit for NVIDIA-Reflex.
		/// @param limit FPS target to limit to.
		/// @note This function affects all windows.
		/// @warning Do not call this function in user code! Use TRAP::Application::SetFPSLimit() instead.
		///          This function is only used internally for NVIDIA-Reflex.
		/// @remark @headless This function is not available in headless mode.
		void SetReflexFPSLimit(u32 limit) override;
#endif /*!defined(TRAP_HEADLESS_MODE)*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the render scale for the given window.
		/// @param scale Render scale value (valid range: 0.5f-1.0f inclusive).
		/// @param window Window to set render scale for.
		/// @note This functon takes effect on the next frame.
		/// @remark @headless This function is not available in headless mode.
		void SetRenderScale(f32 scale, const Window& window) const override;
#else
		/// @brief Set the render scale.
		/// @param scale Render scale value (valid range: 0.5f-1.0f inclusive).
		/// @note This functon takes effect on the next frame.
		/// @remark This function is only available in headless mode.
		void SetRenderScale(f32 scale) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve the used render scale value of the given window.
		/// @param window Window to retrieve render scale from.
		/// @return Render scale (between 0.5f and 2.0f inclusive).
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] f32 GetRenderScale(const Window& window) const override;
#else
		/// @brief Retrieve the used render scale value.
		/// @return Render scale (between 0.5f and 2.0f inclusive).
		/// @remark This function is only available in headless mode.
		[[nodiscard]] f32 GetRenderScale() const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the clear color to be used by the given window.
		/// @param color New clear color.
		/// @param window Window to set clear color for.
		/// @remark @headless This function is not available in headless mode.
		void SetClearColor(const Color& color /*= { 0.1f, 0.1f, 0.1f, 1.0f }*/,
		                   const Window& window) const override;
#else
		/// @brief Set the clear color.
		/// @param color New clear color.
		/// @remark This function is only available in headless mode.
		void SetClearColor(const Color& color /*= { 0.1f, 0.1f, 0.1f, 1.0f }*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the clear depth value to be used by the given window.
		/// @param depth New clear depth value. Must be between 0.0f and 1.0f
		/// @param window Window to set clear depth value for.
		/// @remark @headless This function is not available in headless mode.
		void SetClearDepth(f32 depth /*= 0.0f*/, const Window& window) const override;
#else
		/// @brief Set the clear depth value.
		/// @param depth New clear depth value. Must be between 0.0f and 1.0f
		/// @remark This function is only available in headless mode.
		void SetClearDepth(f32 depth /*= 0.0f*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the clear stencil value to be used by the given window.
		/// @param stencil New clear stencil value.
		/// @param window Window to set clear stencil value for.
		/// @remark @headless This function is not available in headless mode.
		void SetClearStencil(u32 stencil /*= 0*/, const Window& window) const override;
#else
		/// @brief Set the clear stencil value.
		/// @param stencil New clear stencil value.
		/// @remark This function is only available in headless mode.
		void SetClearStencil(u32 stencil /*= 0*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// @brief Set the resolution of the viewport.
		/// @param width New width.
		/// @param height New height.
		/// @remark This function is only available in headless mode.
		void SetResolution(u32 width, u32 height) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifdef TRAP_HEADLESS_MODE
		/// @brief Get the current viewport resolution.
		/// @return Resolution of the viewport.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] TRAP::Math::Vec2ui GetResolution() const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Enable or disable depth testing for the given window.
		/// @param enabled Enable or disable depth testing.
		/// @param window Window to set depth testing for.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthTesting(bool enabled, const Window& window) const override;
#else
		/// @brief Enable or disable depth testing.
		/// @param enabled Enable or disable depth testing.
		/// @remark This function is only available in headless mode.
		void SetDepthTesting(bool enabled) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Enable or disable depth writing for the given window.
		/// @param enabled Enable or disable depth writing.
		/// @param window Window to set depth writing for.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthWriting(bool enabled, const Window& window) const override;
#else
		/// @brief Enable or disable depth writing.
		/// @param enabled Enable or disable depth writing.
		/// @remark This function is only available in headless mode.
		void SetDepthWriting(bool enabled) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the depth function for the given window.
		/// @param function Function to use for depth testing.
		/// @param window Window to set depth function for.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthFunction(CompareMode function, const Window& window) const override;
#else
		/// @brief Set the depth function.
		/// @param function Function to use for depth testing.
		/// @remark This function is only available in headless mode.
		void SetDepthFunction(CompareMode function) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the depth action to perform when depth testing fails for the given window.
		/// @param front Depth action to perform when depth testing fails.
		/// @param back Depth action to perform when depth testing fails.
		/// @param window Window to set the depth fail action for.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthFail(StencilOp front, StencilOp back, const Window& window) const override;
#else
		/// @brief Set the depth action to perform when depth testing fails.
		/// @param front Depth action to perform when depth testing fails.
		/// @param back Depth action to perform when depth testing fails.
		/// @remark This function is only available in headless mode.
		void SetDepthFail(StencilOp front, StencilOp back) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the depth bias (scalar factor to add to each fragments depth value) for the given window.
		/// @param depthBias Depth bias.
		/// @param window Window to set the depth bias for.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthBias(i32 depthBias, const Window& window) const override;
#else
		/// @brief Set the depth bias (scalar factor to add to each fragments depth value).
		/// @param depthBias Depth bias.
		/// @remark This function is only available in headless mode.
		void SetDepthBias(i32 depthBias) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation) for the given window.
		/// @param factor Depth bias slope factor.
		/// @param window Window to set the depth bias slope factor for.
		/// @remark @headless This function is not available in headless mode.
		void SetDepthBiasSlopeFactor(f32 factor, const Window& window) const override;
#else
		/// @brief Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation).
		/// @param factor Depth bias slope factor.
		/// @remark This function is only available in headless mode.
		void SetDepthBiasSlopeFactor(f32 factor) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Enable or disable stencil testing for the given window.
		/// @param enabled Enable or disable stencil testing.
		/// @param window Window to set stencil testing for.
		/// @remark @headless This function is not available in headless mode.
		void SetStencilTesting(bool enabled, const Window& window) const override;
#else
		/// @brief Enable or disable stencil testing.
		/// @param enabled Enable or disable stencil testing.
		/// @remark This function is only available in headless mode.
		void SetStencilTesting(bool enabled) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the stencil action to perform when stencil testing fails for the given window.
		/// @param front Stencil action to perform when stencil testing fails.
		/// @param back Stencil action to perform when stencil testing fails.
		/// @param window Window to set the stencil fail action for.
		/// @remark @headless This function is not available in headless mode.
		void SetStencilFail(StencilOp front, StencilOp back, const Window& window) const override;
#else
		/// @brief Set the stencil action to perform when stencil testing fails.
		/// @param front Stencil action to perform when stencil testing fails.
		/// @param back Stencil action to perform when stencil testing fails.
		/// @remark This function is only available in headless mode.
		void SetStencilFail(StencilOp front, StencilOp back) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the stencil action to perform when stencil testing and depth testing passes for the given window.
		/// @param front Stencil action to perform when passed.
		/// @param back Stencil action to perform when passed.
		/// @param window Window to set the stencil pass action for.
		/// @remark @headless This function is not available in headless mode.
		void SetStencilPass(StencilOp front, StencilOp back, const Window& window) const override;
#else
		/// @brief Set the stencil action to perform when stencil testing and depth testing passes.
		/// @param front Stencil action to perform when passed.
		/// @param back Stencil action to perform when passed.
		/// @remark This function is only available in headless mode.
		void SetStencilPass(StencilOp front, StencilOp back) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the stencil functions for the given window.
		/// @param front Function to use on the front for stencil testing.
		/// @param back Function to use on the back for stencil testing.
		/// @param window Window to set stencil functions for.
		/// @remark @headless This function is not available in headless mode.
		void SetStencilFunction(CompareMode front, CompareMode back, const Window& window) const override;
#else
		/// @brief Set the stencil functions.
		/// @param front Function to use on the front for stencil testing.
		/// @param back Function to use on the back for stencil testing.
		/// @remark This function is only available in headless mode.
		void SetStencilFunction(CompareMode front, CompareMode back) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the stencil mask for the given window.
		/// @param read Select the bits of the stencil values to test.
		/// @param write Select the bits of the stencil values updated by the stencil test.
		/// @param window Window to set stencil mask for.
		/// @remark @headless This function is not available in headless mode.
		void SetStencilMask(u8 read, u8 write, const Window& window) const override;
#else
		/// @brief Set the stencil mask.
		/// @param read Select the bits of the stencil values to test.
		/// @param write Select the bits of the stencil values updated by the stencil test.
		/// @remark This function is only available in headless mode.
		void SetStencilMask(u8 read, u8 write) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the cull mode for the given window.
		/// @param mode Cull mode to use.
		/// @param window Window to set cull mode for.
		/// @remark @headless This function is not available in headless mode.
		void SetCullMode(CullMode mode, const Window& window) const override;
#else
		/// @brief Set the cull mode.
		/// @param mode Cull mode to use.
		/// @remark This function is only available in headless mode.
		void SetCullMode(CullMode mode) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the fill mode for the given window.
		/// @param mode Fill mode to use.
		/// @param window Window to set fill mode for.
		/// @remark @headless This function is not available in headless mode.
		void SetFillMode(FillMode mode, const Window& window) const override;
#else
		/// @brief Set the fill mode.
		/// @param mode Fill mode to use.
		/// @remark This function is only available in headless mode.
		void SetFillMode(FillMode mode) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the primitive topology for the given window.
		/// @param topology Primitive topology to use.
		/// @param window Window to set primitive topology for.
		/// @remark @headless This function is not available in headless mode.
		void SetPrimitiveTopology(PrimitiveTopology topology, const Window& window) const override;
#else
		/// @brief Set the primitive topology.
		/// @param topology Primitive topology to use.
		/// @remark This function is only available in headless mode.
		void SetPrimitiveTopology(PrimitiveTopology topology) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the front face winding order for the given window.
		/// @param face Front face winding order to use.
		/// @param window Window to set front face winding order for.
		/// @remark @headless This function is not available in headless mode.
		void SetFrontFace(FrontFace face, const Window& window) const override;
#else
		/// @brief Set the front face winding order.
		/// @param face Front face winding order to use.
		/// @remark This function is only available in headless mode.
		void SetFrontFace(FrontFace face) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the blend mode for the given window.
		/// @param modeRGB Blend mode to use for the RGB channels.
		/// @param modeAlpha Blend mode to use for the alpha channel.
		/// @param window Window to set the blend mode for.
		/// @remark @headless This function is not available in headless mode.
		void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, const Window& window) const override;
#else
		/// @brief Set the blend mode.
		/// @param modeRGB Blend mode to use for the RGB channels.
		/// @param modeAlpha Blend mode to use for the alpha channel.
		/// @remark This function is only available in headless mode.
		void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the blend constants/factors for the given window.
		/// @param sourceRGB Specifies how the red, green, and blue blending factors are computed.
		/// @param sourceAlpha Specifies how the alpha source blending factor is computed.
		/// @param destinationRGB Specifies how the red, green, and blue destination blending factors are computed.
		/// @param destinationAlpha Specified how the alpha destination blending factor is computed.
		/// @param window Window to set the blend constants for.
		/// @remark @headless This function is not available in headless mode.
		void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                  BlendConstant destinationRGB, BlendConstant destinationAlpha,
							  const Window& window) const override;
#else
		/// @brief Set the blend constants/factors.
		/// @param sourceRGB Specifies how the red, green, and blue blending factors are computed.
		/// @param sourceAlpha Specifies how the alpha source blending factor is computed.
		/// @param destinationRGB Specifies how the red, green, and blue destination blending factors are computed.
		/// @param destinationAlpha Specified how the alpha destination blending factor is computed.
		/// @remark This function is only available in headless mode.
		void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                  BlendConstant destinationRGB, BlendConstant destinationAlpha) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// @param shadingRate Shading rate to use.
		/// @param postRasterizerRate Shading rate combiner to use.
		/// @param finalRate Shading rate combiner to use.
		/// @param window Window to set the shading rate for.
		/// @remark @headless This function is not available in headless mode.
		void SetShadingRate(ShadingRate shadingRate,
		                    ShadingRateCombiner postRasterizerRate,
							ShadingRateCombiner finalRate, const Window& window) const override;
#else
		/// @brief Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// @param shadingRate Shading rate to use.
		/// @param postRasterizerRate Shading rate combiner to use.
		/// @param finalRate Shading rate combiner to use.
		/// @remark This function is only available in headless mode.
		void SetShadingRate(ShadingRate shadingRate,
		                    ShadingRateCombiner postRasterizerRate,
							ShadingRateCombiner finalRate) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the pipeline fragment shading rate via texture.
		/// @param texture Shading rate texture to use.
		/// @param window Window to set shading rate for.
		/// @note The texture must be in ResourceState::ShadingRateSource resource state.
		/// @remark @headless This function is not available in headless mode.
		void SetShadingRate(Ref<RenderTarget> texture, const Window& window) const override;
#else
		/// @brief Set the pipeline fragment shading rate via texture.
		/// @param texture Shading rate texture to use.
		/// @note The texture must be in ResourceState::ShadingRateSource resource state.
		/// @remark This function is only available in headless mode.
		void SetShadingRate(Ref<RenderTarget> texture) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Clear the given window's render target.
		/// @param clearType Type of buffer to clear.
		/// @param window Window to clear.
		/// @remark @headless This function is not available in headless mode.
		void Clear(ClearBufferType clearType, const Window& window) const override;
#else
		/// @brief Clear the given render target.
		/// @param clearType Type of buffer to clear.
		/// @remark This function is only available in headless mode.
		void Clear(ClearBufferType clearType) const override;
#endif /*TRAP_HEADLESS_MODE*/

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
		void SetViewport(u32 x, u32 y, u32 width, u32 height, f32 minDepth /*= 0.0f*/,
		                 f32 maxDepth /*= 1.0f*/, const Window& window) const override;
#else
		/// @brief Set viewport size.
		/// @param x X coordinate of the top left corner of the viewport.
		/// @param y Y coordinate of the top left corner of the viewport.
		/// @param width New viewport width.
		/// @param height New viewport height.
		/// @param minDepth New min depth value. Default: 0.0f.
		/// @param maxDepth New max depth value. Default: 1.0f.
		/// @remark This function is only available in headless mode.
		void SetViewport(u32 x, u32 y, u32 width, u32 height, f32 minDepth /*= 0.0f*/,
		                 f32 maxDepth /*= 1.0f*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Set scissor size for the given window.
		/// @param x Upper left corner.
		/// @param y Upper left corner.
		/// @param width New scissor width.
		/// @param height New scissor height.
		/// @param window Window to set scissor size for.
		/// @remark @headless This function is not available in headless mode.
		void SetScissor(u32 x, u32 y, u32 width, u32 height,
		                const Window& window) const override;
#else
		/// @brief Set scissor size.
		/// @param x Upper left corner.
		/// @param y Upper left corner.
		/// @param width New scissor width.
		/// @param height New scissor height.
		/// @remark This function is only available in headless mode.
		void SetScissor(u32 x, u32 y, u32 width, u32 height) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw non-indexed, non-instanced geometry for the given window.
		/// @param vertexCount Number of vertices to draw.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @param window Window to draw for.
		/// @remark @headless This function is not available in headless mode.
		void Draw(u32 vertexCount, u32 firstVertex /*= 0*/, const Window& window) const override;
#else
		/// @brief Draw non-indexed, non-instanced geometry.
		/// @param vertexCount Number of vertices to draw.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		void Draw(u32 vertexCount, u32 firstVertex /*= 0*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw indexed, non-instanced geometry for the given window.
		/// @param indexCount Number of indices to draw.
		/// @param firstIndex Index of the first indice to draw. Default: 0.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @param window Window to draw for.
		/// @remark @headless This function is not available in headless mode.
		void DrawIndexed(u32 indexCount, u32 firstIndex /*= 0*/, i32 firstVertex /*= 0*/,
		                 const Window& window) const override;
#else
		/// @brief Draw indexed, non-instanced geometry.
		/// @param indexCount Number of indices to draw.
		/// @param firstIndex Index of the first indice to draw. Default: 0.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		void DrawIndexed(u32 indexCount, u32 firstIndex /*= 0*/, i32 firstVertex /*= 0*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw non-indexed, instanced geometry for the given window.
		/// @param vertexCount Number of vertices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @param firstInstance Index of the first instance to draw. Default: 0.
		/// @param window Window to draw for.
		/// @remark @headless This function is not available in headless mode.
		void DrawInstanced(u32 vertexCount, u32 instanceCount, u32 firstVertex /*= 0*/,
		                   u32 firstInstance /*= 0*/, const Window& window) const override;
#else
		/// @brief Draw non-indexed, instanced geometry.
		/// @param vertexCount Number of vertices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @param firstInstance Index of the first instance to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		void DrawInstanced(u32 vertexCount, u32 instanceCount, u32 firstVertex /*= 0*/,
		                   u32 firstInstance /*= 0*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Draw indexed, instanced geometry for the given window.
		/// @param indexCount Number of indices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstIndex Index of the first indice to draw. Default: 0.
		/// @param firstInstance Index of the first instance to draw. Default: 0.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @param window Window to draw for.
		/// @remark @headless This function is not available in headless mode.
		void DrawIndexedInstanced(u32 indexCount, u32 instanceCount,
		                          u32 firstIndex /*= 0*/, u32 firstInstance /*= 0*/,
								  i32 firstVertex /*= 0*/, const Window& window) const override;
#else
		/// @brief Draw indexed, instanced geometry.
		/// @param indexCount Number of indices to draw.
		/// @param instanceCount Number of instances to draw.
		/// @param firstIndex Index of the first indice to draw. Default: 0.
		/// @param firstInstance Index of the first instance to draw. Default: 0.
		/// @param firstVertex Index of the first vertex to draw. Default: 0.
		/// @remark This function is only available in headless mode.
		void DrawIndexedInstanced(u32 indexCount, u32 instanceCount,
		                          u32 firstIndex /*= 0*/, u32 firstInstance /*= 0*/,
								  i32 firstVertex /*= 0*/) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind shader on the given window.
		/// @param shader Shader to bind.
		/// @param window Window to bind the shader for.
		/// @remark @headless This function is not available in headless mode.
		void BindShader(Shader& shader, const Window& window) const;
#else
		/// @brief Bind shader on the given window.
		/// @param shader Shader to bind.
		/// @remark This function is only available in headless mode.
		void BindShader(Shader& shader) const;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind vertex buffer on the given window.
		/// @param vBuffer Vertex buffer to bind.
		/// @param layout Layout of the vertex buffer.
		/// @param window Window to bind the vertex buffer for.
		/// @remark @headless This function is not available in headless mode.
		void BindVertexBuffer(const Buffer& vBuffer, const VertexBufferLayout& layout,
		                      const Window& window) const override;
#else
		/// @brief Bind vertex buffer.
		/// @param vBuffer Vertex buffer to bind.
		/// @param layout Layout of the vertex buffer.
		/// @remark This function is only available in headless mode.
		void BindVertexBuffer(const Buffer& vBuffer, const VertexBufferLayout& layout) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind an index buffer on the given window.
		/// @param iBuffer Index buffer to bind.
		/// @param indexType Data type used by the index buffer.
		/// @param window Window to bind the vertex buffer for.
		/// @remark @headless This function is not available in headless mode.
		void BindIndexBuffer(const Buffer& iBuffer, IndexType indexType,
		                     const Window& window) const override;
#else
		/// @brief Bind an index buffer.
		/// @param iBuffer Index buffer to bind.
		/// @param indexType Data type used by the index buffer.
		/// @remark This function is only available in headless mode.
		void BindIndexBuffer(const Buffer& iBuffer, IndexType indexType) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind a descriptor set on the given window.
		/// @param dSet Descriptor set to bind.
		/// @param index Index for which descriptor set to bind.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @param window Window to bind the descriptor set for.
		/// @remark @headless This function is not available in headless mode.
		void BindDescriptorSet(DescriptorSet& dSet, u32 index,
		                       QueueType queueType /*= QueueType::Graphics*/,
							   const Window& window) const override;
#else
		/// @brief Bind a descriptor set.
		/// @param dSet Descriptor set to bind.
		/// @param index Index for which descriptor set to bind.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		void BindDescriptorSet(DescriptorSet& dSet, u32 index,
		                       QueueType queueType /*= QueueType::Graphics*/) const override;
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
		void BindPushConstants(std::string_view name, std::span<const u8> constants,
		                       QueueType queueType /*= QueueType::Graphics*/,
							   const Window& window) const override;
#else
		/// @brief Bind push constant buffer data.
		/// @param name Name of the push constant block.
		/// @param constants Constant buffer data.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @note There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// @remark This function is only available in headless mode.
		void BindPushConstants(std::string_view name, std::span<const u8> constants,
		                       QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Bind push constant buffer data on the given window.
		/// @param paramIndex Index of the push constant block in the RootSignatures descriptors array.
		/// @param constants Constant buffer data.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @param window Window to bind the push constants for.
		/// @remark @headless This function is not available in headless mode.
		void BindPushConstantsByIndex(u32 paramIndex, std::span<const u8> constants,
		                              QueueType queueType /*= QueueType::Graphics*/,
									  const Window& window) const override;
#else
		/// @brief Bind push constant buffer data.
		/// @param paramIndex Index of the push constant block in the RootSignatures descriptors array.
		/// @param constantsData Constant buffer data.
		/// @param queueType Queue type on which to perform the bind operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		void BindPushConstantsByIndex(u32 paramIndex, std::span<const u8> constants,
		                              QueueType queueType /*= QueueType::Graphics*/) const override;
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
		void BindRenderTarget(const Graphics::RenderTarget* colorTarget,
		                      const Graphics::RenderTarget* depthStencil /*= nullptr*/,
							  RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
							  std::vector<u32>* colorArraySlices /*= nullptr*/,
							  std::vector<u32>* colorMipSlices /*= nullptr*/,
							  u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/,
							  const Window& window) const override;
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
		void BindRenderTarget(const Graphics::RenderTarget* colorTarget,
		                      const Graphics::RenderTarget* depthStencil /*= nullptr*/,
							  RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
							  std::vector<u32>* colorArraySlices /*= nullptr*/,
							  std::vector<u32>* colorMipSlices /*= nullptr*/,
							  u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/) const override;
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
		void BindRenderTargets(const std::vector<std::reference_wrapper<const Graphics::RenderTarget>>& colorTargets,
		                       const Graphics::RenderTarget* depthStencil /*= nullptr*/,
							   RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
							   std::vector<u32>* colorArraySlices /*= nullptr*/,
							   std::vector<u32>* colorMipSlices /*= nullptr*/,
							   u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/,
							   const Window& window) const override;
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
		void BindRenderTargets(const std::vector<std::reference_wrapper<const Graphics::RenderTarget>>& colorTargets,
		                       const Graphics::RenderTarget* depthStencil /*= nullptr*/,
							   RendererAPI::LoadActionsDesc* loadActions /*= nullptr*/,
							   std::vector<u32>* colorArraySlices /*= nullptr*/,
							   std::vector<u32>* colorMipSlices /*= nullptr*/,
							   u32 depthArraySlice /*= -1*/, u32 depthMipSlice /*= -1*/) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Add a resource barrier (memory dependency) for the given window.
		/// @param bufferBarrier Buffer barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barrier for.
		/// @remark @headless This function is not available in headless mode.
		void ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
								   QueueType queueType /*= QueueType::Graphics*/,
								   const Window& window) const override;
#else
		/// @brief Add a resource barrier (memory dependency).
		/// @param bufferBarrier Buffer barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		void ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
								   QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add resource barriers (memory dependencies) for the given window.
		/// @param bufferBarriers Buffer barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barriers for.
		/// @remark @headless This function is not available in headless mode.
		void ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
									QueueType queueType /*= QueueType::Graphics*/,
									const Window& window) const override;
#else
		/// @brief Add resource barriers (memory dependencies).
		/// @param bufferBarriers Buffer barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		void ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
									QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add a resource barrier (memory dependency) for the given window.
		/// @param textureBarrier Texture barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barrier for.
		/// @remark @headless This function is not available in headless mode.
		void ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
									QueueType queueType /*= QueueType::Graphics*/,
									const Window& window) const override;
#else
		/// @brief Add a resource barrier (memory dependency).
		/// @param textureBarrier Texture barrier.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		void ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
									QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add resource barriers (memory dependencies) for the given window.
		/// @param textureBarriers Texture barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @param window Window to add the barriers for.
		/// @remark @headless This function is not available in headless mode.
		void ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
									 QueueType queueType /*= QueueType::Graphics*/,
									 const Window& window) const override;
#else
		/// @brief Add resource barriers (memory dependencies).
		/// @param textureBarriers Texture barriers.
		/// @param queueType Queue type on which to perform the barrier operation. Default: Graphics.
		/// @remark This function is only available in headless mode.
		void ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
									 QueueType queueType /*= QueueType::Graphics*/) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add a resource barrier (memory dependency) for the given window.
		/// @param renderTargetBarrier Render target barrier.
		/// @param window Window to add the barrier for.
		/// @remark @headless This function is not available in headless mode.
		void ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
									     const Window& window) const override;
#else
		/// @brief Add a resource barrier (memory dependency).
		/// @param renderTargetBarrier Render target barrier.
		/// @remark This function is only available in headless mode.
		void ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Add resource barriers (memory dependencies) for the given window.
		/// @param renderTargetBarriers Render target barriers.
		/// @param window Window to add the barriers for.
		/// @remark @headless This function is not available in headless mode.
		void ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
									      const Window& window) const override;
#else
		/// @brief Add resource barriers (memory dependencies).
		/// @param renderTargetBarriers Render target barriers.
		/// @remark This function is only available in headless mode.
		void ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief NVIDIA-Reflex Sleep/synchronize.
		/// @param window Window to sleep for.
		/// @remark @headless This function is not available in headless mode.
		void ReflexSleep(const Window& window) const override;
		/// @brief Set a timestamp for NVIDIA-Reflex.
		/// @param marker Enum value of the marker to set.
		/// @param window Window to set the timestamp for.
		/// @remark @headless This function is not available in headless mode.
		void ReflexMarker(NVIDIAReflexLatencyMarker marker, const Window& window) const override;
		/// @brief Retrieve the latency report from NVIDIA-Reflex.
		/// @param numLatencyData Number of latency data points to return. Set to 0 to retrieve the maximum queryable frame data.
		/// @param window Window to get latency data for.
		/// @return Latency report.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] std::vector<VkLatencyTimingsFrameReportNV> ReflexGetLatency(u32 numLatencyData, const Window& window) const override;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Retrieve the renderer title.
		/// Example title: "[Vulkan 1.3.0]".
		/// @return Renderer title.
		[[nodiscard]] constexpr std::string GetTitle() const noexcept override;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve whether VSync is enabled or not for the given window.
		/// @param window Window to retrieve VSync for.
		/// @return True if VSync is enabled, false otherwise.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] bool GetVSync(const Window& window) const override;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Retrieve the currently used GPUs UUID.
		/// @return GPU's UUID.
		[[nodiscard]] TRAP::Utils::UUID GetCurrentGPUUUID() const noexcept override;
		/// @brief Retrieve the name of the currently used GPU.
		/// @return GPU's name.
		[[nodiscard]] std::string GetCurrentGPUName() const noexcept override;
		/// @brief Retrieve the vendor of the currently used GPU.
		/// @return GPU vendor.
		[[nodiscard]] GPUVendor GetCurrentGPUVendor() const noexcept override;
		/// @brief Retrieve a list of all supported GPUs.
		/// The list contains the GPUs name and UUID.
		/// @return List of all supported GPUs.
		[[nodiscard]] std::vector<std::pair<std::string, TRAP::Utils::UUID>> GetAllGPUs() const override;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Capture a screenshot of the last presented frame.
		/// @param window Window to capture screenshot on.
		/// @return Captured screenshot as TRAP::Image on success, nullptr.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] TRAP::Scope<TRAP::Image> CaptureScreenshot(const Window& window) const override;
#else
		/// @brief Capture a screenshot of the last presented frame.
		/// @return Captured screenshot as TRAP::Image on success, nullptr.
		/// @remark This function is only available in headless mode.
		[[nodiscard]] TRAP::Scope<TRAP::Image> CaptureScreenshot() const override;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Resolve a MSAA render target to a non MSAA render target.
		/// Needed to transfer MSAA rendered image data to a presentable non-MSAA target.
		/// @param source Source MSAA render target to resolve.
		/// @param destination Destination non MSAA render target to resolve into.
		/// @param cmd CommadBuffer to resolve on.
		/// @note source and destination must be in ResourceState::RenderTarget resource state.
		void MSAAResolvePass(const RenderTarget& source, const RenderTarget& destination,
		                     CommandBuffer* cmd) const override;

		/// @brief Update the internal RenderTargets used for render scaling.
		/// @param viewportData PerViewportData to update.
		void UpdateInternalRenderTargets(PerViewportData& viewportData) const;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Scale image from internal resolution to the final output resolution.
		/// @param source Source render target to resolve.
		/// @param destination Destination render target to resolve into.
		/// @param window Window to do the scaling pass on.
		/// @note source and destination must be in ResourceState::RenderTarget resource state.
		/// @remark @headless This function is not available in headless mode.
		void RenderScalePass(const RenderTarget& source,
							 const RenderTarget& destination,
		                     const Window& window) const override;
#else
		/// @brief Scale image from internal resolution to the final output resolution.
		/// @param source Source render target to resolve.
		/// @param destination Destination render target to resolve into.
		/// @note source and destination must be in ResourceState::RenderTarget resource state.
		/// @remark This function is only available in headless mode.
		void RenderScalePass(const RenderTarget& source,
							 const RenderTarget& destination) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Set the NVIDIA Reflex latency mode.
		/// @param mode Latency mode to set.
		/// @param window Window to set latency mode for.
		/// @remark @win32 NVIDIAReflexLatencyModes are only available on Windows 10 or newer with NVIDIA hardware.
		/// @remark @headless This function is not available in headless mode.
		void SetReflexLatencyMode(NVIDIAReflexLatencyMode mode, const Window& window) override;
		/// @brief Retrieve the currently used latency mode for NVIDIA Reflex.
		/// @param window Window to retrieve latency mode for.
		/// @return Used latency mode.
		/// @note The returned value may differ from the requested mode set with SetReflexLatencyMode().
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] NVIDIAReflexLatencyMode GetReflexLatencyMode(const Window& window) const override;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Initialize the internal rendering data of the given window.
		/// @param window Window to initialize the internal rendering data for.
		/// @param VSyncEnabled Whether to enable or disable VSync.
		/// @remark @headless This function is not available in headless mode.
		void InitPerViewportData(Window& window, bool VSyncEnabled) const override;
#else
		/// @brief Initialize the internal rendering data.
		/// @param width Width for the viewport.
		/// @param height Height for the viewport.
		/// @remark This function is only available in headless mode.
		void InitPerViewportData(u32 width, u32 height) const override;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Remove the internal rendering data of the given window.
		/// @param window Window to remove the internal rendering data from.
		/// @remark @headless This function is not available in headless mode.
		void RemovePerViewportData(const Window& window) const override;
#else
		/// @brief Remove the internal rendering data.
		/// @remark This function is only available in headless mode.
		void RemovePerViewportData() const override;
#endif /*TRAP_HEADLESS_MODE*/

		void WaitIdle() const override;

		inline constinit static struct GPUCapBits
		{
			std::array<bool, std::to_underlying(ImageFormat::IMAGE_FORMAT_COUNT)> CanShaderReadFrom{};
			std::array<bool, std::to_underlying(ImageFormat::IMAGE_FORMAT_COUNT)> CanShaderWriteTo{};
			std::array<bool, std::to_underlying(ImageFormat::IMAGE_FORMAT_COUNT)> CanRenderTargetWriteTo{};
		} s_GPUCapBits{{}, {}, {}};

		struct SizeOffset
		{
			u32 Size;
			u32 Offset;
		};

		union DescriptorUpdateData
		{
			VkDescriptorImageInfo ImageInfo;
			VkDescriptorBufferInfo BufferInfo;
			VkBufferView BufferView;
		};

		/// @brief Description for framebuffer creation.
		struct FrameBufferDesc
		{
			//Render pass to use
			TRAP::Ref<VulkanRenderPass> RenderPass;
			//Color render targets to use
			std::vector<std::reference_wrapper<const RenderTarget>> RenderTargets;
			//Depth/Stencil render target to use
			const RenderTarget* DepthStencil;
			//Shading rate texture to use
			const RenderTarget* ShadingRate;
			//Array layer to use from color render targets
			std::vector<u32> ColorArraySlices;
			//Mip level to use from color render targets
			std::vector<u32> ColorMipSlices;
			//Array layer of depth/stencil render target to use
			u32 DepthArraySlice;
			//Mip level of depth/stencil render target to use
			u32 DepthMipSlice;
		};

		/// @brief Description for render pass creation.
		struct RenderPassDesc
		{
			std::vector<ImageFormat> ColorFormats;
			std::vector<LoadActionType> LoadActionsColor;
			std::vector<StoreActionType> StoreActionsColor;
			u32 RenderTargetCount;
			TRAP::Graphics::RendererAPI::SampleCount SampleCount = RendererAPI::SampleCount::One;
			ImageFormat DepthStencilFormat;
			ImageFormat ShadingRateFormat;
			LoadActionType LoadActionDepth;
			LoadActionType LoadActionStencil;
			StoreActionType StoreActionDepth;
			StoreActionType StoreActionStencil;
		};

		/// @brief Struct holding data about a descriptor set layout.
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
			std::unordered_map<DescriptorInfo*, u64> DescriptorIndexMap{};
		};

		/// @brief Create default resources to be used as null descriptors in case user does not specify some descriptors
		struct NullDescriptors
		{
			std::array<TRAP::Ref<TRAP::Graphics::Texture>,
			           std::to_underlying(ShaderReflection::TextureDimension::TextureDimCount)> DefaultTextureSRV;
			std::array<TRAP::Ref<TRAP::Graphics::Texture>,
			           std::to_underlying(ShaderReflection::TextureDimension::TextureDimCount)> DefaultTextureUAV;
			TRAP::Ref<VulkanBuffer> DefaultBufferSRV;
			TRAP::Ref<VulkanBuffer> DefaultBufferUAV;
			TRAP::Ref<VulkanSampler> DefaultSampler;

			//Unlike DirectX 12, Vulkan textures start in undefined layout.
			//With this, we transition them to the specified layout so app code doesn't
			//have to worry about this
			struct NullDescriptorsObjs
			{
				TRAP::Ref<VulkanQueue> InitialTransitionQueue{};
				TRAP::Ref<VulkanCommandPool> InitialTransitionCmdPool{};
				VulkanCommandBuffer* InitialTransitionCmd{};
				TRAP::Ref<VulkanFence> InitialTransitionFence{};
			};
			Utils::Safe<NullDescriptorsObjs> SafeNullDescriptorsObjs{};
		};
		static TRAP::Scope<NullDescriptors> s_NullDescriptors;

		/// @brief Transition the given texture from undefined resource state to the given resource state.
		/// @param texture Texture to transition.
		/// @param startState Start state for the texture.
		static void UtilInitialTransition(const TRAP::Graphics::Texture& texture, RendererAPI::ResourceState startState);

		//Per Thread Render Pass synchronization logic
		//Render-passes are not exposed to the engine code since they are not available on all APIs
		//This map takes care of hashing a render pass based on the render targets passed to
		//CommandBuffer::Begin()
		using RenderPassMap = std::unordered_map<u64, TRAP::Ref<VulkanRenderPass>>;
		using RenderPassMapNode = RenderPassMap::value_type;
		using RenderPassMapIt = RenderPassMap::iterator;
		using FrameBufferMap = std::unordered_map<u64, TRAP::Ref<VulkanFrameBuffer>>;
		using FrameBufferMapNode = FrameBufferMap::value_type;
		using FrameBufferMapIt = FrameBufferMap::iterator;

		/// @brief Retrieve the render pass hash map.
		/// @return Render pass hash map.
		[[nodiscard]] static RenderPassMap& GetRenderPassMap();
		/// @brief Retrieve the framebuffer hash map.
		/// @return Framebuffer hash map.
		[[nodiscard]] static FrameBufferMap& GetFrameBufferMap();

		/// @brief Retrieve the vulkan instance used by the renderer.
		/// @return Vulkan instance.
		[[nodiscard]] TRAP::Ref<VulkanInstance> GetInstance() const noexcept;
		/// @brief Retrieve the vulkan device used by the renderer.
		/// @return Vulkan device.
		[[nodiscard]] TRAP::Ref<VulkanDevice> GetDevice() const noexcept;
		/// @brief Retrieve the vulkan memory allocator used by the renderer.
		/// @return Vulkan memory allocator.
		[[nodiscard]] TRAP::Ref<VulkanMemoryAllocator> GetVMA() const noexcept;

		/// @brief Utility to retrieve a pipeline.
		/// If a pipeline with the same description was already created,
		/// it will be retrieved from the cache.
		/// @param desc Pipeline description.
		/// @return Pipeline.
		[[nodiscard]] static TRAP::Ref<Pipeline> GetPipeline(PipelineDesc& desc);

	private:
		/// @brief Add the default resources to the renderer.
		static void AddDefaultResources();
		/// @brief Remove the default resources from the renderer.
		static void RemoveDefaultResources();

		/// @brief Start recording the graphics pipeline.
		/// @param p Per window data used for recording.
		void StartGraphicRecording(PerViewportData& p) const;
		/// @brief Stop recording the graphics pipeline.
		/// @param p Per window data used for recording.
		static void EndGraphicRecording(PerViewportData& p);

		/// @brief Start recording the compute pipeline.
		/// @param p Per window data used for recording.
		static void StartComputeRecording(PerViewportData& p);
		/// @brief Stop recording the compute pipeline.
		/// @param p Per window data used for recording.
		static void EndComputeRecording(PerViewportData& p);

		/// @brief Present the currently recorded frame.
		/// @param p Per widow data used for presentation.
		void Present(PerViewportData& p) const;

		/// @brief Start frame time profiling of the GPU.
		/// @param type Queue type to profile.
		/// @param p Per window data to profile for.
		static void BeginGPUFrameProfile(QueueType type, const PerViewportData& p);
		/// @brief End frame time profiling of the GPU.
		/// @param type Queue type to profile.
		/// @param p Per window data to profile for.
		static void EndGPUFrameProfile(QueueType type, const PerViewportData& p);
		/// @brief Retrieve the result from GPU frame time profiling.
		/// @param type Queue type to profile.
		/// @param p Per window data to profile for.
		[[nodiscard]] static f32 ResolveGPUFrameProfile(QueueType type, const PerViewportData& p);

		std::string m_rendererTitle;

		TRAP::Ref<VulkanInstance> m_instance = nullptr;
		TRAP::Scope<VulkanDebug> m_debug = nullptr;
		TRAP::Ref<VulkanDevice> m_device = nullptr;
		TRAP::Ref<VulkanMemoryAllocator> m_vma = nullptr;

		inline static std::vector<std::pair<std::string, TRAP::Utils::UUID>> s_usableGPUs{};

		inline constinit static VulkanRenderer* s_renderer = nullptr;

		friend VulkanDevice;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::API::VulkanRenderer::VulkanRenderer() noexcept
{
	s_renderer = this;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Graphics::API::VulkanRenderer::GetTitle() const noexcept
{
	return m_rendererTitle;
}

#endif /*TRAP_VULKANRENDERER_H*/

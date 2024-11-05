#include "TRAPPCH.h"
#include "RenderCommand.h"

#include "Application.h"
#include "ImageLoader/Image.h"
#include "Maths/Math.h"

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::Flush(const Window& window)
{
	RendererAPI::GetRenderer()->Flush(window);
}
#else
void TRAP::Graphics::RenderCommand::Flush()
{
	RendererAPI::GetRenderer()->Flush();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::Clear(const ClearBuffer clearType, const Window& window)
{
	RendererAPI::GetRenderer()->Clear(clearType, window);
}
#else
void TRAP::Graphics::RenderCommand::Clear(const ClearBuffer clearType)
{
	RendererAPI::GetRenderer()->Clear(clearType);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetClearColor(const RendererAPI::Color& color, const Window& window)
{
	RendererAPI::GetRenderer()->SetClearColor(color, window);
}
#else
void TRAP::Graphics::RenderCommand::SetClearColor(const RendererAPI::Color& color)
{
	RendererAPI::GetRenderer()->SetClearColor(color);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetClearDepth(const f32 depth, const Window& window)
{
	RendererAPI::GetRenderer()->SetClearDepth(depth, window);
}
#else
void TRAP::Graphics::RenderCommand::SetClearDepth(const f32 depth)
{
	RendererAPI::GetRenderer()->SetClearDepth(depth);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetClearStencil(const u32 stencil, const Window& window)
{
	RendererAPI::GetRenderer()->SetClearStencil(stencil, window);
}
#else
void TRAP::Graphics::RenderCommand::SetClearStencil(const u32 stencil)
{
	RendererAPI::GetRenderer()->SetClearStencil(stencil);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetDepthTesting(const bool enabled, const Window& window)
{
	RendererAPI::GetRenderer()->SetDepthTesting(enabled, window);
}
#else
void TRAP::Graphics::RenderCommand::SetDepthTesting(const bool enabled)
{
	RendererAPI::GetRenderer()->SetDepthTesting(enabled);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetDepthWriting(const bool enabled, const Window& window)
{
	RendererAPI::GetRenderer()->SetDepthWriting(enabled, window);
}
#else
void TRAP::Graphics::RenderCommand::SetDepthWriting(const bool enabled)
{
	RendererAPI::GetRenderer()->SetDepthWriting(enabled);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetDepthFunction(const CompareMode compareMode, const Window& window)
{
	RendererAPI::GetRenderer()->SetDepthFunction(compareMode, window);
}
#else
void TRAP::Graphics::RenderCommand::SetDepthFunction(const CompareMode compareMode)
{
	RendererAPI::GetRenderer()->SetDepthFunction(compareMode);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetDepthFail(const StencilOperation front, const StencilOperation back,
												 const Window& window)
{
	RendererAPI::GetRenderer()->SetDepthFail(front, back, window);
}
#else
void TRAP::Graphics::RenderCommand::SetDepthFail(const StencilOperation front, const StencilOperation back)
{
	RendererAPI::GetRenderer()->SetDepthFail(front, back);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetDepthBias(const i32 bias, const Window& window)
{
	RendererAPI::GetRenderer()->SetDepthBias(bias, window);
}
#else
void TRAP::Graphics::RenderCommand::SetDepthBias(const i32 bias)
{
	RendererAPI::GetRenderer()->SetDepthBias(bias);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetDepthBiasSlopeFactor(const f32 slopeFactor, const Window& window)
{
	RendererAPI::GetRenderer()->SetDepthBiasSlopeFactor(slopeFactor, window);
}
#else
void TRAP::Graphics::RenderCommand::SetDepthBiasSlopeFactor(const f32 slopeFactor)
{
	RendererAPI::GetRenderer()->SetDepthBiasSlopeFactor(slopeFactor);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetStencilTesting(const bool enabled, const Window& window)
{
	RendererAPI::GetRenderer()->SetStencilTesting(enabled, window);
}
#else
void TRAP::Graphics::RenderCommand::SetStencilTesting(const bool enabled)
{
	RendererAPI::GetRenderer()->SetStencilTesting(enabled);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetStencilFail(const StencilOperation front, const StencilOperation back,
												   const Window& window)
{
	RendererAPI::GetRenderer()->SetStencilFail(front, back, window);
}
#else
void TRAP::Graphics::RenderCommand::SetStencilFail(const StencilOperation front, const StencilOperation back)
{
	RendererAPI::GetRenderer()->SetStencilFail(front, back);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetStencilPass(const StencilOperation front, const StencilOperation back,
												   const Window& window)
{
	RendererAPI::GetRenderer()->SetStencilPass(front, back, window);
}
#else
void TRAP::Graphics::RenderCommand::SetStencilPass(const StencilOperation front, const StencilOperation back)
{
	RendererAPI::GetRenderer()->SetStencilPass(front, back);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetStencilFunction(const CompareMode front, const CompareMode back,
													   const Window& window)
{
	RendererAPI::GetRenderer()->SetStencilFunction(front, back, window);
}
#else
void TRAP::Graphics::RenderCommand::SetStencilFunction(const CompareMode front, const CompareMode back)
{
	RendererAPI::GetRenderer()->SetStencilFunction(front, back);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetStencilMask(const u8 read, const u8 write, const Window& window)
{
	RendererAPI::GetRenderer()->SetStencilMask(read, write, window);
}
#else
void TRAP::Graphics::RenderCommand::SetStencilMask(const u8 read, const u8 write)
{
	RendererAPI::GetRenderer()->SetStencilMask(read, write);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetCullMode(const CullMode cullMode, const Window& window)
{
	RendererAPI::GetRenderer()->SetCullMode(cullMode, window);
}
#else
void TRAP::Graphics::RenderCommand::SetCullMode(const CullMode cullMode)
{
	RendererAPI::GetRenderer()->SetCullMode(cullMode);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetFillMode(const FillMode fillMode, const Window& window)
{
	RendererAPI::GetRenderer()->SetFillMode(fillMode, window);
}
#else
void TRAP::Graphics::RenderCommand::SetFillMode(const FillMode fillMode)
{
	RendererAPI::GetRenderer()->SetFillMode(fillMode);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetPrimitiveTopology(const PrimitiveTopology topology, const Window& window)
{
	RendererAPI::GetRenderer()->SetPrimitiveTopology(topology, window);
}
#else
void TRAP::Graphics::RenderCommand::SetPrimitiveTopology(const PrimitiveTopology topology)
{
	RendererAPI::GetRenderer()->SetPrimitiveTopology(topology);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetFrontFace(const FrontFace face, const Window& window)
{
	RendererAPI::GetRenderer()->SetFrontFace(face, window);
}
#else
void TRAP::Graphics::RenderCommand::SetFrontFace(const FrontFace face)
{
	RendererAPI::GetRenderer()->SetFrontFace(face);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetShadingRate(const ShadingRate shadingRate,
												   const ShadingRateCombiner postRasterizerRate,
												   const ShadingRateCombiner finalRate, const Window& window)
{
	RendererAPI::GetRenderer()->SetShadingRate(shadingRate, postRasterizerRate, finalRate, window);
}
#else
void TRAP::Graphics::RenderCommand::SetShadingRate(const ShadingRate shadingRate,
												   const ShadingRateCombiner postRasterizerRate,
												   const ShadingRateCombiner finalRate)
{
	RendererAPI::GetRenderer()->SetShadingRate(shadingRate, postRasterizerRate, finalRate);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetShadingRate(TRAP::Ref<TRAP::Graphics::RenderTarget> texture,
                                                   const Window& window)
{
	RendererAPI::GetRenderer()->SetShadingRate(std::move(texture), window);
}
#else
void TRAP::Graphics::RenderCommand::SetShadingRate(TRAP::Ref<TRAP::Graphics::RenderTarget> texture)
{
	RendererAPI::GetRenderer()->SetShadingRate(std::move(texture));
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetAntiAliasing(const AntiAliasing antiAliasing, const SampleCount sampleCount)
{
	RendererAPI::SetAntiAliasing(antiAliasing, sampleCount);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount,
                                                    const Window& window) noexcept
{
	RendererAPI::GetAntiAliasing(outAntiAliasing, outSampleCount, window);
}
#else
void TRAP::Graphics::RenderCommand::GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount) noexcept
{
	RendererAPI::GetAntiAliasing(outAntiAliasing, outSampleCount);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetAnisotropyLevel(const SampleCount anisotropyLevel)
{
	RendererAPI::SetAnisotropyLevel(anisotropyLevel);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::SampleCount TRAP::Graphics::RenderCommand::GetAnisotropyLevel() noexcept
{
	return RendererAPI::GetAnisotropyLevel();
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha, const Window& window)
{
	RendererAPI::GetRenderer()->SetBlendMode(modeRGB, modeAlpha, window);
}
#else
void TRAP::Graphics::RenderCommand::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha)
{
	RendererAPI::GetRenderer()->SetBlendMode(modeRGB, modeAlpha);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetBlendConstant(const BlendConstant sourceRGBA,
                                                     const BlendConstant destinationRGBA, const Window& window)
{
	RendererAPI::GetRenderer()->SetBlendConstant(sourceRGBA, sourceRGBA, destinationRGBA, destinationRGBA, window);
}
#else
void TRAP::Graphics::RenderCommand::SetBlendConstant(const BlendConstant sourceRGBA,
                                                     const BlendConstant destinationRGBA)
{
	RendererAPI::GetRenderer()->SetBlendConstant(sourceRGBA, sourceRGBA, destinationRGBA, destinationRGBA);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetBlendConstant(const BlendConstant sourceRGB, const BlendConstant sourceAlpha,
	  												 const BlendConstant destinationRGB,
													 const BlendConstant destinationAlpha, const Window& window)
{
	RendererAPI::GetRenderer()->SetBlendConstant(sourceRGB, sourceAlpha, destinationRGB, destinationAlpha, window);
}
#else
void TRAP::Graphics::RenderCommand::SetBlendConstant(const BlendConstant sourceRGB, const BlendConstant sourceAlpha,
	  												 const BlendConstant destinationRGB,
													 const BlendConstant destinationAlpha)
{
	RendererAPI::GetRenderer()->SetBlendConstant(sourceRGB, sourceAlpha, destinationRGB, destinationAlpha);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetViewport(const u32 x, const u32 y, const u32 width,
                                                const u32 height, const Window& window)
{
	RendererAPI::GetRenderer()->SetViewport(x, y, width, height, 0.0f, 1.0f, window);
}
#else
void TRAP::Graphics::RenderCommand::SetViewport(const u32 x, const u32 y, const u32 width,
                                                const u32 height)
{
	RendererAPI::GetRenderer()->SetViewport(x, y, width, height, 0.0f, 1.0f);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetScissor(const u32 x, const u32 y, const u32 width,
											   const u32 height, const Window& window)
{
	RendererAPI::GetRenderer()->SetScissor(x, y, width, height, window);
}
#else
void TRAP::Graphics::RenderCommand::SetScissor(const u32 x, const u32 y, const u32 width,
											   const u32 height)
{
	RendererAPI::GetRenderer()->SetScissor(x, y, width, height);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetResolution(const u32 width, const u32 height)
{
	RendererAPI::GetRenderer()->SetResolution(width, height);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Math::Vec2ui TRAP::Graphics::RenderCommand::GetResolution()
{
	return RendererAPI::GetRenderer()->GetResolution();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetRenderScale(const f32 scale, const Window& window)
{
	RendererAPI::GetRenderer()->SetRenderScale(scale, window);
}
#else
void TRAP::Graphics::RenderCommand::SetRenderScale(const f32 scale)
{
	RendererAPI::GetRenderer()->SetRenderScale(scale);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] f32 TRAP::Graphics::RenderCommand::GetRenderScale(const Window& window)
{
	return RendererAPI::GetRenderer()->GetRenderScale(window);
}
#else
[[nodiscard]] f32 TRAP::Graphics::RenderCommand::GetRenderScale()
{
	return RendererAPI::GetRenderer()->GetRenderScale();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::Draw(const u32 vertexCount, const u32 firstVertex, const Window& window)
{
	RendererAPI::GetRenderer()->Draw(vertexCount, firstVertex, window);
}
#else
void TRAP::Graphics::RenderCommand::Draw(const u32 vertexCount, const u32 firstVertex)
{
	RendererAPI::GetRenderer()->Draw(vertexCount, firstVertex);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::DrawIndexed(const u32 indexCount, const u32 firstIndex,
												const i32 vertexOffset, const Window& window)
{
	RendererAPI::GetRenderer()->DrawIndexed(indexCount, firstIndex, vertexOffset, window);
}
#else
void TRAP::Graphics::RenderCommand::DrawIndexed(const u32 indexCount, const u32 firstIndex,
												const i32 vertexOffset)
{
	RendererAPI::GetRenderer()->DrawIndexed(indexCount, firstIndex, vertexOffset);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::DrawInstanced(const u32 vertexCount, const u32 instanceCount,
	                                              const u32 firstVertex, const u32 firstInstance,
												  const Window& window)
{
	RendererAPI::GetRenderer()->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance, window);
}
#else
void TRAP::Graphics::RenderCommand::DrawInstanced(const u32 vertexCount, const u32 instanceCount,
	                                              const u32 firstVertex, const u32 firstInstance)
{
	RendererAPI::GetRenderer()->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::DrawIndexedInstanced(const u32 indexCount, const u32 instanceCount,
														 const u32 firstIndex, const u32 firstInstance,
														 const i32 vertexOffset, const Window& window)
{
	RendererAPI::GetRenderer()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, firstInstance,
	                                                 vertexOffset, window);
}
#else
void TRAP::Graphics::RenderCommand::DrawIndexedInstanced(const u32 indexCount, const u32 instanceCount,
														 const u32 firstIndex, const u32 firstInstance,
														 const i32 vertexOffset)
{
	RendererAPI::GetRenderer()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, firstInstance,
	                                                 vertexOffset);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::Dispatch(const std::array<u32, 3>& workGroupElementSizes, const Window& window)
{
	RendererAPI::GetRenderer()->Dispatch(workGroupElementSizes, window);
}
#else
void TRAP::Graphics::RenderCommand::Dispatch(const std::array<u32, 3>& workGroupElementSizes)
{
	RendererAPI::GetRenderer()->Dispatch(workGroupElementSizes);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetPushConstants(const std::string_view name, const std::span<const u8> data,
                                                     const QueueType queueType, const Window& window)
{
	RendererAPI::GetRenderer()->BindPushConstants(name, data, queueType, window);
}
#else
void TRAP::Graphics::RenderCommand::SetPushConstants(const std::string_view name, const std::span<const u8> data,
                                                     const QueueType queueType)
{
	RendererAPI::GetRenderer()->BindPushConstants(name, data, queueType);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::BindRenderTarget(const Graphics::RenderTarget* const colorTarget,
		                                             const Graphics::RenderTarget* const depthStencil,
									                 RendererAPI::LoadActionsDesc* const loadActions,
									                 const Window& window)
{
	RendererAPI::GetRenderer()->BindRenderTarget(colorTarget, depthStencil, loadActions, nullptr, nullptr,
												 std::numeric_limits<u32>::max(),
												 std::numeric_limits<u32>::max(), window);
}
#else
void TRAP::Graphics::RenderCommand::BindRenderTarget(const Graphics::RenderTarget* const colorTarget,
		                                             const Graphics::RenderTarget* const depthStencil,
									                 RendererAPI::LoadActionsDesc* const loadActions)
{
	RendererAPI::GetRenderer()->BindRenderTarget(colorTarget, depthStencil, loadActions, nullptr, nullptr,
												 std::numeric_limits<u32>::max(),
												 std::numeric_limits<u32>::max());
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::BindRenderTargets(const std::vector<std::reference_wrapper<const Graphics::RenderTarget>>& colorTargets,
		                                              const Graphics::RenderTarget* const depthStencil,
									                  RendererAPI::LoadActionsDesc* const loadActions,
									                  const Window& window)
{
	RendererAPI::GetRenderer()->BindRenderTargets(colorTargets, depthStencil, loadActions, nullptr, nullptr,
												  std::numeric_limits<u32>::max(),
												  std::numeric_limits<u32>::max(), window);
}
#else
void TRAP::Graphics::RenderCommand::BindRenderTargets(const std::vector<std::reference_wrapper<const Graphics::RenderTarget>>& colorTargets,
		                                             const Graphics::RenderTarget* const depthStencil,
									                 RendererAPI::LoadActionsDesc* const loadActions)
{
	RendererAPI::GetRenderer()->BindRenderTargets(colorTargets, depthStencil, loadActions, nullptr, nullptr,
												  std::numeric_limits<u32>::max(),
												  std::numeric_limits<u32>::max());
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::StartRenderPass(const Window& window)
{
	RendererAPI::StartRenderPass(window);
}
#else
void TRAP::Graphics::RenderCommand::StartRenderPass()
{
	RendererAPI::StartRenderPass();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::StopRenderPass(const Window& window)
{
	RendererAPI::StopRenderPass(window);
}
#else
void TRAP::Graphics::RenderCommand::StopRenderPass()
{
	RendererAPI::StopRenderPass();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::BufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
                                                  const QueueType queueType, const Window& window)
{
	RendererAPI::GetRenderer()->ResourceBufferBarrier(bufferBarrier, queueType, window);
}
#else
void TRAP::Graphics::RenderCommand::BufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
                                                  const QueueType queueType)
{
	RendererAPI::GetRenderer()->ResourceBufferBarrier(bufferBarrier, queueType);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::BufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
                                                   const QueueType queueType, const Window& window)
{
	RendererAPI::GetRenderer()->ResourceBufferBarriers(bufferBarriers, queueType, window);
}
#else
void TRAP::Graphics::RenderCommand::BufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
                                                   const QueueType queueType)
{
	RendererAPI::GetRenderer()->ResourceBufferBarriers(bufferBarriers, queueType);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::TextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
                                                   const QueueType queueType, const Window& window)
{
	RendererAPI::GetRenderer()->ResourceTextureBarrier(textureBarrier, queueType, window);
}
#else
void TRAP::Graphics::RenderCommand::TextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
                                                   const QueueType queueType)
{
	RendererAPI::GetRenderer()->ResourceTextureBarrier(textureBarrier, queueType);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::TextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
           										    const QueueType queueType, const Window& window)
{
	RendererAPI::GetRenderer()->ResourceTextureBarriers(textureBarriers, queueType, window);
}
#else
void TRAP::Graphics::RenderCommand::TextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
           										    const QueueType queueType)
{
	RendererAPI::GetRenderer()->ResourceTextureBarriers(textureBarriers, queueType);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::RenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
 														const Window& window)
{
	RendererAPI::GetRenderer()->ResourceRenderTargetBarrier(renderTargetBarrier, window);
}
#else
void TRAP::Graphics::RenderCommand::RenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier)
{
	RendererAPI::GetRenderer()->ResourceRenderTargetBarrier(renderTargetBarrier);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::RenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
 														 const Window& window)
{
	RendererAPI::GetRenderer()->ResourceRenderTargetBarriers(renderTargetBarriers, window);
}
#else
void TRAP::Graphics::RenderCommand::RenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers)
{
	RendererAPI::GetRenderer()->ResourceRenderTargetBarriers(renderTargetBarriers);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Graphics::RenderCommand::CaptureScreenshot(const Window& window)
{
	return RendererAPI::GetRenderer()->CaptureScreenshot(window);
}
#else
[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Graphics::RenderCommand::CaptureScreenshot()
{
	return RendererAPI::GetRenderer()->CaptureScreenshot();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Transition(const Ref<Texture>& texture, const RendererAPI::ResourceState oldLayout,
											   const RendererAPI::ResourceState newLayout, const QueueType queueType)
{
	RendererAPI::Transition(texture, oldLayout, newLayout, queueType);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::MSAAResolvePass(const RenderTarget& source,
                                                    const RenderTarget& destination, const Window& window)
{
	const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData(window);
	CommandBuffer* const cmd = viewportData.GraphicCommandBuffers[viewportData.ImageIndex];

	RendererAPI::GetRenderer()->MSAAResolvePass(source, destination, cmd);
}
#else
void TRAP::Graphics::RenderCommand::MSAAResolvePass(const RenderTarget& source,
                                                    const RenderTarget& destination)
{
	const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData();
	CommandBuffer* const cmd = viewportData.GraphicCommandBuffers[viewportData.ImageIndex];

	RendererAPI::GetRenderer()->MSAAResolvePass(source, destination, cmd);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Graphics::RenderCommand::GetCPUFPS()
{
	const f32 cpuFrameTime = Application::GetCPUFrameTime();
	if(cpuFrameTime == 0.0f)
		return 0;

	return NumericCast<u32>(1000.0f / cpuFrameTime);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] u32 TRAP::Graphics::RenderCommand::GetGPUFPS(const Window& window)
{
	const f32 maxGPUFrameTime = TRAP::Math::Max(RendererAPI::GetGPUGraphicsFrameTime(window),
	                                              RendererAPI::GetGPUComputeFrameTime(window));
	if(maxGPUFrameTime == 0.0f)
		return 0;

	return NumericCast<u32>(1000.0f / maxGPUFrameTime);
}
#else
[[nodiscard]] u32 TRAP::Graphics::RenderCommand::GetGPUFPS()
{
	const f32 maxGPUFrameTime = TRAP::Math::Max(RendererAPI::GetGPUGraphicsFrameTime(),
	                                              RendererAPI::GetGPUComputeFrameTime());
	if(maxGPUFrameTime == 0.0f)
		return 0;

	return NumericCast<u32>(1000.0f / maxGPUFrameTime);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Graphics::RenderCommand::GetCPUFrameTime()
{
	return Application::GetCPUFrameTime();
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] f32 TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime(const Window& window)
{
	return RendererAPI::GetGPUGraphicsFrameTime(window);
}
#else
[[nodiscard]] f32 TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime()
{
	return RendererAPI::GetGPUGraphicsFrameTime();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] f32 TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime(const Window& window)
{
	return RendererAPI::GetGPUComputeFrameTime(window);
}
#else
[[nodiscard]] f32 TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime()
{
	return RendererAPI::GetGPUComputeFrameTime();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Graphics::RenderCommand::GetGPUName()
{
	return RendererAPI::GetRenderer()->GetCurrentGPUName();
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetReflexLatencyMode(const NVIDIAReflexLatencyMode mode, const Window& window)
{
	RendererAPI::GetRenderer()->SetReflexLatencyMode(mode, window);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Graphics::NVIDIAReflexLatencyMode TRAP::Graphics::RenderCommand::GetReflexLatencyMode(const Window& window)
{
	return RendererAPI::GetRenderer()->GetReflexLatencyMode(window);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetAntiLagMode(const AMDAntiLagMode mode, const Window& window)
{
	RendererAPI::GetRenderer()->SetAntiLagMode(mode, window);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Graphics::AMDAntiLagMode TRAP::Graphics::RenderCommand::GetAntiLagMode(const Window& window)
{
	return RendererAPI::GetRenderer()->GetAntiLagMode(window);
}
#endif /*TRAP_HEADLESS_MODE*/

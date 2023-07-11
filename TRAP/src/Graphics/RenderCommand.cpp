#include "TRAPPCH.h"
#include "RenderCommand.h"

#include "Application.h"
#include "ImageLoader/Image.h"

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::Flush(const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetVSync(const bool vsync, const Window* const window)
{
	RendererAPI::GetRenderer()->SetVSync(vsync, window);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] bool TRAP::Graphics::RenderCommand::GetVSync(const Window* const window)
{
	return RendererAPI::GetRenderer()->GetVSync(window);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::Clear(const ClearBuffer clearType, const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetClearColor(const RendererAPI::Color& color, const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetClearDepth(const float depth, const Window* const window)
{
	RendererAPI::GetRenderer()->SetClearDepth(depth, window);
}
#else
void TRAP::Graphics::RenderCommand::SetClearDepth(const float depth)
{
	RendererAPI::GetRenderer()->SetClearDepth(depth);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetClearStencil(const uint32_t stencil, const Window* const window)
{
	RendererAPI::GetRenderer()->SetClearStencil(stencil, window);
}
#else
void TRAP::Graphics::RenderCommand::SetClearStencil(const uint32_t stencil)
{
	RendererAPI::GetRenderer()->SetClearStencil(stencil);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetDepthTesting(const bool enabled, const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetDepthWriting(const bool enabled, const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetDepthFunction(const CompareMode compareMode, const Window* const window)
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
												 const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetDepthBias(const int32_t bias, const Window* const window)
{
	RendererAPI::GetRenderer()->SetDepthBias(bias, window);
}
#else
void TRAP::Graphics::RenderCommand::SetDepthBias(const int32_t bias)
{
	RendererAPI::GetRenderer()->SetDepthBias(bias);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetDepthBiasSlopeFactor(const float slopeFactor, const Window* const window)
{
	RendererAPI::GetRenderer()->SetDepthBiasSlopeFactor(slopeFactor, window);
}
#else
void TRAP::Graphics::RenderCommand::SetDepthBiasSlopeFactor(const float slopeFactor)
{
	RendererAPI::GetRenderer()->SetDepthBiasSlopeFactor(slopeFactor);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetStencilTesting(const bool enabled, const Window* const window)
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
												   const Window* const window)
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
												   const Window* const window)
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
													   const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetStencilMask(const uint8_t read, const uint8_t write, const Window* const window)
{
	RendererAPI::GetRenderer()->SetStencilMask(read, write, window);
}
#else
void TRAP::Graphics::RenderCommand::SetStencilMask(const uint8_t read, const uint8_t write)
{
	RendererAPI::GetRenderer()->SetStencilMask(read, write);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetCullMode(const CullMode cullMode, const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetFillMode(const FillMode fillMode, const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetPrimitiveTopology(const PrimitiveTopology topology, const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetFrontFace(const FrontFace face, const Window* const window)
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
												   const ShadingRateCombiner finalRate, const Window* const window)
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
                                                   const Window* const window)
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

void TRAP::Graphics::RenderCommand::GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount) noexcept
{
	RendererAPI::GetAntiAliasing(outAntiAliasing, outSampleCount);
}

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
void TRAP::Graphics::RenderCommand::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha, const Window* const window)
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
                                                     const BlendConstant destinationRGBA, const Window* const window)
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
													 const BlendConstant destinationAlpha, const Window* const window)
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
void TRAP::Graphics::RenderCommand::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width,
                                                const uint32_t height, const Window* const window)
{
	RendererAPI::GetRenderer()->SetViewport(x, y, width, height, 0.0f, 1.0f, window);
}
#else
void TRAP::Graphics::RenderCommand::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width,
                                                const uint32_t height)
{
	RendererAPI::GetRenderer()->SetViewport(x, y, width, height, 0.0f, 1.0f);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width,
											   const uint32_t height, const Window* const window)
{
	RendererAPI::GetRenderer()->SetScissor(x, y, width, height, window);
}
#else
void TRAP::Graphics::RenderCommand::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width,
											   const uint32_t height)
{
	RendererAPI::GetRenderer()->SetScissor(x, y, width, height);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetResolution(const uint32_t width, const uint32_t height)
{
	RendererAPI::GetRenderer()->SetResolution(width, height);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::GetResolution(uint32_t& width, uint32_t& height)
{
	RendererAPI::GetRenderer()->GetResolution(width, height);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetRenderScale(const float scale, const Window* const window)
{
	RendererAPI::GetRenderer()->SetRenderScale(scale, window);
}
#else
void TRAP::Graphics::RenderCommand::SetRenderScale(const float scale)
{
	RendererAPI::GetRenderer()->SetRenderScale(scale);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] float TRAP::Graphics::RenderCommand::GetRenderScale(const Window* const window)
{
	return RendererAPI::GetRenderer()->GetRenderScale(window);
}
#else
[[nodiscard]] float TRAP::Graphics::RenderCommand::GetRenderScale()
{
	return RendererAPI::GetRenderer()->GetRenderScale();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::Draw(const uint32_t vertexCount, const uint32_t firstVertex, const Window* const window)
{
	RendererAPI::GetRenderer()->Draw(vertexCount, firstVertex, window);
}
#else
void TRAP::Graphics::RenderCommand::Draw(const uint32_t vertexCount, const uint32_t firstVertex)
{
	RendererAPI::GetRenderer()->Draw(vertexCount, firstVertex);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::DrawIndexed(const uint32_t indexCount, const uint32_t firstIndex,
												const int32_t vertexOffset, const Window* const window)
{
	RendererAPI::GetRenderer()->DrawIndexed(indexCount, firstIndex, vertexOffset, window);
}
#else
void TRAP::Graphics::RenderCommand::DrawIndexed(const uint32_t indexCount, const uint32_t firstIndex,
												const int32_t vertexOffset)
{
	RendererAPI::GetRenderer()->DrawIndexed(indexCount, firstIndex, vertexOffset);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::DrawInstanced(const uint32_t vertexCount, const uint32_t instanceCount,
	                                              const uint32_t firstVertex, const uint32_t firstInstance,
												  const Window* const window)
{
	RendererAPI::GetRenderer()->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance, window);
}
#else
void TRAP::Graphics::RenderCommand::DrawInstanced(const uint32_t vertexCount, const uint32_t instanceCount,
	                                              const uint32_t firstVertex, const uint32_t firstInstance)
{
	RendererAPI::GetRenderer()->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::DrawIndexedInstanced(const uint32_t indexCount, const uint32_t instanceCount,
														 const uint32_t firstIndex, const uint32_t firstInstance,
														 const int32_t vertexOffset, const Window* const window)
{
	RendererAPI::GetRenderer()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, firstInstance,
	                                                 vertexOffset, window);
}
#else
void TRAP::Graphics::RenderCommand::DrawIndexedInstanced(const uint32_t indexCount, const uint32_t instanceCount,
														 const uint32_t firstIndex, const uint32_t firstInstance,
														 const int32_t vertexOffset)
{
	RendererAPI::GetRenderer()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, firstInstance,
	                                                 vertexOffset);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::Dispatch(const std::array<uint32_t, 3>& workGroupElementSizes, const Window* const window)
{
	RendererAPI::GetRenderer()->Dispatch(workGroupElementSizes, window);
}
#else
void TRAP::Graphics::RenderCommand::Dispatch(const std::array<uint32_t, 3>& workGroupElementSizes)
{
	RendererAPI::GetRenderer()->Dispatch(workGroupElementSizes);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetPushConstants(const std::string_view name, const void* data,
                                                     const std::size_t length, const QueueType queueType,
                                                     const Window* const window)
{
	RendererAPI::GetRenderer()->BindPushConstants(name, data, length, queueType, window);
}
#else
void TRAP::Graphics::RenderCommand::SetPushConstants(const std::string_view name, const void* data,
                                                     const std::size_t length, const QueueType queueType)
{
	RendererAPI::GetRenderer()->BindPushConstants(name, data, length, queueType);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
									                 const RendererAPI::LoadActionsDesc* const loadActions,
									                 const Window* const window)
{
	RendererAPI::GetRenderer()->BindRenderTarget(colorTarget, depthStencil, loadActions, nullptr, nullptr,
												 std::numeric_limits<uint32_t>::max(),
												 std::numeric_limits<uint32_t>::max(), window);
}
#else
void TRAP::Graphics::RenderCommand::BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
									                 const RendererAPI::LoadActionsDesc* const loadActions)
{
	RendererAPI::GetRenderer()->BindRenderTarget(colorTarget, depthStencil, loadActions, nullptr, nullptr,
												 std::numeric_limits<uint32_t>::max(),
												 std::numeric_limits<uint32_t>::max());
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
									                 const RendererAPI::LoadActionsDesc* const loadActions,
									                 const Window* const window)
{
	RendererAPI::GetRenderer()->BindRenderTargets(colorTargets, depthStencil, loadActions, nullptr, nullptr,
												  std::numeric_limits<uint32_t>::max(),
												  std::numeric_limits<uint32_t>::max(), window);
}
#else
void TRAP::Graphics::RenderCommand::BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
									                 const RendererAPI::LoadActionsDesc* const loadActions)
{
	RendererAPI::GetRenderer()->BindRenderTargets(colorTargets, depthStencil, loadActions, nullptr, nullptr,
												  std::numeric_limits<uint32_t>::max(),
												  std::numeric_limits<uint32_t>::max());
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::StartRenderPass(const Window* const window)
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
void TRAP::Graphics::RenderCommand::StopRenderPass(const Window* const window)
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
                                                  const QueueType queueType, const Window* const window)
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
                                                   const QueueType queueType, const Window* const window)
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
                                                   const QueueType queueType, const Window* const window)
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
           										    const QueueType queueType, const Window* const window)
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
 														const Window* const window)
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
 														 const Window* const window)
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
[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Graphics::RenderCommand::CaptureScreenshot(const Window* const window)
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
void TRAP::Graphics::RenderCommand::MSAAResolvePass(TRAP::Ref<RenderTarget> source,
                                                    TRAP::Ref<RenderTarget> destination, const Window* const window)
{
	TRAP_ASSERT(window, "RenderCommand::MSAAResolvePass(): Window is nullptr!");

	const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData(window);
	CommandBuffer* const cmd = viewportData.GraphicCommandBuffers[viewportData.ImageIndex];

	RendererAPI::GetRenderer()->MSAAResolvePass(std::move(source), std::move(destination), cmd);
}
#else
void TRAP::Graphics::RenderCommand::MSAAResolvePass(TRAP::Ref<RenderTarget> source,
                                                    TRAP::Ref<RenderTarget> destination)
{
	const auto& viewportData = TRAP::Graphics::RendererAPI::GetViewportData();
	CommandBuffer* const cmd = viewportData.GraphicCommandBuffers[viewportData.ImageIndex];

	RendererAPI::GetRenderer()->MSAAResolvePass(std::move(source), std::move(destination), cmd);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::RenderCommand::GetCPUFPS()
{
	return NumericCast<uint32_t>(1000.0f / Application::GetCPUFrameTime());
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] uint32_t TRAP::Graphics::RenderCommand::GetGPUFPS(const Window* const window)
{
	const float maxGPUFrameTime = TRAP::Math::Max(RendererAPI::GetGPUGraphicsFrameTime(window),
	                                              RendererAPI::GetGPUComputeFrameTime(window));

	return NumericCast<uint32_t>(1000.0f / maxGPUFrameTime);
}
#else
[[nodiscard]] uint32_t TRAP::Graphics::RenderCommand::GetGPUFPS()
{
	const float maxGPUFrameTime = TRAP::Math::Max(RendererAPI::GetGPUGraphicsFrameTime(),
	                                              RendererAPI::GetGPUComputeFrameTime());

	return NumericCast<uint32_t>(1000.0f / maxGPUFrameTime);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Graphics::RenderCommand::GetCPUFrameTime()
{
	return Application::GetCPUFrameTime();
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] float TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime(const Window* const window)
{
	return RendererAPI::GetGPUGraphicsFrameTime(window);
}
#else
[[nodiscard]] float TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime()
{
	return RendererAPI::GetGPUGraphicsFrameTime();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] float TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime(const Window* const window)
{
	return RendererAPI::GetGPUComputeFrameTime(window);
}
#else
[[nodiscard]] float TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime()
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
void TRAP::Graphics::RenderCommand::SetLatencyMode(const LatencyMode mode, const Window* const window)
{
	RendererAPI::GetRenderer()->SetLatencyMode(mode, window);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Graphics::LatencyMode TRAP::Graphics::RenderCommand::GetLatencyMode(const Window* const window)
{
	return RendererAPI::GetRenderer()->GetLatencyMode(window);
}
#endif /*TRAP_HEADLESS_MODE*/

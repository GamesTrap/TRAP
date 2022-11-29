#include "TRAPPCH.h"
#include "RenderCommand.h"

#include "Application.h"

void TRAP::Graphics::RenderCommand::Flush(const Window* const window)
{
	RendererAPI::GetRenderer()->Flush(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetVSync(const bool vsync, const Window* const window)
{
	RendererAPI::GetRenderer()->SetVSync(vsync, window);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RenderCommand::GetVSync(const Window* const window)
{
	return RendererAPI::GetRenderer()->GetVSync(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Clear(const ClearBuffer buffer, const Window* const window)
{
	RendererAPI::GetRenderer()->Clear(buffer, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearColor(const Math::Vec4& color, const Window* const window)
{
	RendererAPI::GetRenderer()->SetClearColor(color, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearDepth(const float depth, const Window* const window)
{
	RendererAPI::GetRenderer()->SetClearDepth(depth, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearStencil(const uint32_t stencil, const Window* const window)
{
	RendererAPI::GetRenderer()->SetClearStencil(stencil, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthTesting(const bool enabled, const Window* const window)
{
	RendererAPI::GetRenderer()->SetDepthTesting(enabled, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthWriting(const bool enabled, const Window* const window)
{
	RendererAPI::GetRenderer()->SetDepthWriting(enabled, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthFunction(const CompareMode compareMode, const Window* const window)
{
	RendererAPI::GetRenderer()->SetDepthFunction(compareMode, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthFail(const StencilOperation front, const StencilOperation back,
												 const Window* const window)
{
	RendererAPI::GetRenderer()->SetDepthFail(front, back, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthBias(const int32_t bias, const Window* const window)
{
	RendererAPI::GetRenderer()->SetDepthBias(bias, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthBiasSlopeFactor(const float slopeFactor, const Window* const window)
{
	RendererAPI::GetRenderer()->SetDepthBiasSlopeFactor(slopeFactor, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilTesting(const bool enabled, const Window* const window)
{
	RendererAPI::GetRenderer()->SetStencilTesting(enabled, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilFail(const StencilOperation front, const StencilOperation back,
												   const Window* const window)
{
	RendererAPI::GetRenderer()->SetStencilFail(front, back, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilPass(const StencilOperation front, const StencilOperation back,
												   const Window* const window)
{
	RendererAPI::GetRenderer()->SetStencilPass(front, back, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilFunction(const CompareMode front, const CompareMode back,
													   const Window* const window)
{
	RendererAPI::GetRenderer()->SetStencilFunction(front, back, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilMask(const uint8_t read, const uint8_t write, const Window* const window)
{
	RendererAPI::GetRenderer()->SetStencilMask(read, write, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetCullMode(const CullMode cullMode, const Window* const window)
{
	RendererAPI::GetRenderer()->SetCullMode(cullMode, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetFillMode(const FillMode fillMode, const Window* const window)
{
	RendererAPI::GetRenderer()->SetFillMode(fillMode, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetPrimitiveTopology(const PrimitiveTopology topology, const Window* const window)
{
	RendererAPI::GetRenderer()->SetPrimitiveTopology(topology, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetFrontFace(const FrontFace face, const Window* const window)
{
	RendererAPI::GetRenderer()->SetFrontFace(face, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetShadingRate(const ShadingRate shadingRate,
												   const ShadingRateCombiner postRasterizerRate,
												   const ShadingRateCombiner finalRate, const Window* const window)
{
	RendererAPI::GetRenderer()->SetShadingRate(shadingRate, postRasterizerRate, finalRate, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetShadingRate(TRAP::Ref<TRAP::Graphics::Texture> shadingRateTex,
                                                   const Window* const window)
{
	RendererAPI::GetRenderer()->SetShadingRate(shadingRateTex, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetAntiAliasing(const AntiAliasing antiAliasing, const SampleCount sampleCount)
{
	RendererAPI::SetAntiAliasing(antiAliasing, sampleCount);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::GetAntiAliasing(AntiAliasing& outAntiAliasing, SampleCount& outSampleCount)
{
	RendererAPI::GetAntiAliasing(outAntiAliasing, outSampleCount);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha, const Window* const window)
{
	RendererAPI::GetRenderer()->SetBlendMode(modeRGB, modeAlpha, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendConstant(const BlendConstant sourceRGBA,
                                                     const BlendConstant destinationRGBA, const Window* const window)
{
	RendererAPI::GetRenderer()->SetBlendConstant(sourceRGBA, sourceRGBA, destinationRGBA, destinationRGBA, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendConstant(const BlendConstant sourceRGB, const BlendConstant sourceAlpha,
	  												 const BlendConstant destinationRGB,
													 const BlendConstant destinationAlpha, const Window* const window)
{
	RendererAPI::GetRenderer()->SetBlendConstant(sourceRGB, sourceAlpha, destinationRGB, destinationAlpha, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width,
                                                const uint32_t height, const Window* const window)
{
	RendererAPI::GetRenderer()->SetViewport(x, y, width, height, 0.0f, 1.0f, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width,
											   const uint32_t height, const Window* const window)
{
	RendererAPI::GetRenderer()->SetScissor(x, y, width, height, window);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetResolution(const uint32_t width, const uint32_t height, const Window* const window)
{
	RendererAPI::GetRenderer()->SetResolution(width, height, window);
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Draw(const uint32_t vertexCount, const uint32_t firstVertex, const Window* const window)
{
	RendererAPI::GetRenderer()->Draw(vertexCount, firstVertex, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::DrawIndexed(const uint32_t indexCount, const uint32_t firstIndex,
												const uint32_t firstVertex, const Window* const window)
{
	RendererAPI::GetRenderer()->DrawIndexed(indexCount, firstIndex, firstVertex, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::DrawInstanced(const uint32_t vertexCount, const uint32_t instanceCount,
	                                              const uint32_t firstVertex, const uint32_t firstInstance,
												  const Window* const window)
{
	RendererAPI::GetRenderer()->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::DrawIndexedInstanced(const uint32_t indexCount, const uint32_t instanceCount,
														 const uint32_t firstIndex, const uint32_t firstInstance,
														 const uint32_t firstVertex, const Window* const window)
{
	RendererAPI::GetRenderer()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, firstInstance,
	                                                 firstVertex, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Dispatch(const std::array<uint32_t, 3>& workGroupElementSizes, const Window* const window)
{
	RendererAPI::GetRenderer()->Dispatch(workGroupElementSizes, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetPushConstants(const char* name, const void* data, const QueueType queueType,
                                                     const Window* const window)
{
	RendererAPI::GetRenderer()->BindPushConstants(name, data, queueType, window);
}

//-------------------------------------------------------------------------------------------------------------------//

// void TRAP::Graphics::RenderCommand::BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
// 		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
// 									                 const RendererAPI::LoadActionsDesc* loadActions,
// 									                 std::vector<uint32_t>* colorArraySlices,
// 									                 std::vector<uint32_t>* colorMipSlices,
// 									                 const uint32_t depthArraySlice, const uint32_t depthMipSlice,
// 									                 const Window* const window)
// {
// 	RendererAPI::GetRenderer()->BindRenderTarget(colorTarget, depthStencil, loadActions, colorArraySlices,
// 												 colorMipSlices, depthArraySlice, depthMipSlice, window);
// }

//-------------------------------------------------------------------------------------------------------------------//

// void TRAP::Graphics::RenderCommand::BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
// 		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
// 									                 const RendererAPI::LoadActionsDesc* const loadActions,
// 									                 std::vector<uint32_t>* const colorArraySlices,
// 									                 std::vector<uint32_t>* const colorMipSlices,
// 									                 const uint32_t depthArraySlice, const uint32_t depthMipSlice,
// 									                 const Window* const window)
// {
// 	RendererAPI::GetRenderer()->BindRenderTargets(colorTargets, depthStencil, loadActions, colorArraySlices,
// 												 colorMipSlices, depthArraySlice, depthMipSlice, window);
// }

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
									                 const RendererAPI::LoadActionsDesc* const loadActions,
									                 const Window* const window)
{
	RendererAPI::GetRenderer()->BindRenderTarget(colorTarget, depthStencil, loadActions, nullptr, nullptr,
												 static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
									                 const RendererAPI::LoadActionsDesc* const loadActions,
									                 const Window* const window)
{
	RendererAPI::GetRenderer()->BindRenderTargets(colorTargets, depthStencil, loadActions, nullptr, nullptr,
												  static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::StartRenderPass(const Window* const window)
{
	RendererAPI::GetRenderer()->StartRenderPass(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::StopRenderPass(const Window* const window)
{
	RendererAPI::GetRenderer()->StopRenderPass(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::BufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
                                                  const QueueType queueType, const Window* const window)
{
	RendererAPI::GetRenderer()->ResourceBufferBarrier(bufferBarrier, queueType, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::BufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
                                                   const QueueType queueType, const Window* const window)
{
	RendererAPI::GetRenderer()->ResourceBufferBarriers(bufferBarriers, queueType, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::TextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
                                                   const QueueType queueType, const Window* const window)
{
	RendererAPI::GetRenderer()->ResourceTextureBarrier(textureBarrier, queueType, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::TextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
           										    const QueueType queueType, const Window* const window)
{
	RendererAPI::GetRenderer()->ResourceTextureBarriers(textureBarriers, queueType, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::RenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
 														const Window* const window)
{
	RendererAPI::GetRenderer()->ResourceRenderTargetBarrier(renderTargetBarrier, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::RenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
 														 const Window* const window)
{
	RendererAPI::GetRenderer()->ResourceRenderTargetBarriers(renderTargetBarriers, window);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Graphics::RenderCommand::CaptureScreenshot(const Window* const window)
{
	return RendererAPI::GetRenderer()->CaptureScreenshot(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Transition(Ref<Texture> texture, const RendererAPI::ResourceState oldLayout,
											   const RendererAPI::ResourceState newLayout, const QueueType queueType)
{
	RendererAPI::Transition(texture, oldLayout, newLayout, queueType);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::MSAAResolvePass(TRAP::Ref<RenderTarget> source,
                                                    TRAP::Ref<RenderTarget> destination, const Window* const window)
{
	RendererAPI::GetRenderer()->MSAAResolvePass(source, destination, window);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderCommand::GetCPUFPS()
{
	return static_cast<uint32_t>(1000.0f / Application::GetCPUFrameTime());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::RenderCommand::GetGPUFPS(const Window* const window)
{
	const float maxGPUFrameTime = TRAP::Math::Max(RendererAPI::GetGPUGraphicsFrameTime(window), RendererAPI::GetGPUComputeFrameTime(window));

	return static_cast<uint32_t>(1000.0f / maxGPUFrameTime);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::RenderCommand::GetCPUFrameTime()
{
	return Application::GetCPUFrameTime();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime(const Window* const window)
{
	return RendererAPI::GetGPUGraphicsFrameTime(window);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime(const Window* const window)
{
	return RendererAPI::GetGPUComputeFrameTime(window);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::RenderCommand::GetGPUName()
{
	return RendererAPI::GetRenderer()->GetCurrentGPUName();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetLatencyMode(const LatencyMode mode, const Window* const window)
{
	RendererAPI::GetRenderer()->SetLatencyMode(mode, window);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::LatencyMode TRAP::Graphics::RenderCommand::GetLatencyMode(const Window* const window)
{
	return RendererAPI::GetRenderer()->GetLatencyMode(window);
}

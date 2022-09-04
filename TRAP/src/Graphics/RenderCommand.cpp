#include "TRAPPCH.h"
#include "RenderCommand.h"

#include "Application.h"

void TRAP::Graphics::RenderCommand::Present(Window* window)
{
	RendererAPI::GetRenderer()->Present(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetVSync(const bool vsync, Window* window)
{
	RendererAPI::GetRenderer()->SetVSync(vsync, window);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RenderCommand::GetVSync(Window* window)
{
	return RendererAPI::GetRenderer()->GetVSync(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Clear(const ClearBuffer buffer, Window* window)
{
	RendererAPI::GetRenderer()->Clear(buffer, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearColor(const Math::Vec4& color, Window* window)
{
	RendererAPI::GetRenderer()->SetClearColor(color, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearDepth(const float depth, Window* window)
{
	RendererAPI::GetRenderer()->SetClearDepth(depth, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearStencil(const uint32_t stencil, Window* window)
{
	RendererAPI::GetRenderer()->SetClearStencil(stencil, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthTesting(const bool enabled, Window* window)
{
	RendererAPI::GetRenderer()->SetDepthTesting(enabled, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthWriting(const bool enabled, Window* window)
{
	RendererAPI::GetRenderer()->SetDepthWriting(enabled, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthFunction(const CompareMode compareMode, Window* window)
{
	RendererAPI::GetRenderer()->SetDepthFunction(compareMode, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthFail(const StencilOperation front, const StencilOperation back,
												 Window* window)
{
	RendererAPI::GetRenderer()->SetDepthFail(front, back, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthBias(const int32_t bias, Window* window)
{
	RendererAPI::GetRenderer()->SetDepthBias(bias, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthBiasSlopeFactor(const float slopeFactor, Window* window)
{
	RendererAPI::GetRenderer()->SetDepthBiasSlopeFactor(slopeFactor, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilTesting(const bool enabled, Window* window)
{
	RendererAPI::GetRenderer()->SetStencilTesting(enabled, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilFail(const StencilOperation front, const StencilOperation back,
												   Window* window)
{
	RendererAPI::GetRenderer()->SetStencilFail(front, back, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilPass(const StencilOperation front, const StencilOperation back,
												   Window* window)
{
	RendererAPI::GetRenderer()->SetStencilPass(front, back, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilFunction(const CompareMode front, const CompareMode back,
													   Window* window)
{
	RendererAPI::GetRenderer()->SetStencilFunction(front, back, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilMask(const uint8_t read, const uint8_t write, Window* window)
{
	RendererAPI::GetRenderer()->SetStencilMask(read, write, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetCullMode(const CullMode cullMode, Window* window)
{
	RendererAPI::GetRenderer()->SetCullMode(cullMode, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetFillMode(const FillMode fillMode, Window* window)
{
	RendererAPI::GetRenderer()->SetFillMode(fillMode, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetPrimitiveTopology(const PrimitiveTopology topology, Window* window)
{
	RendererAPI::GetRenderer()->SetPrimitiveTopology(topology, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetFrontFace(const FrontFace face, Window* window)
{
	RendererAPI::GetRenderer()->SetFrontFace(face, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetShadingRate(const ShadingRate shadingRate,
												   Ref<Texture> texture,
												   const ShadingRateCombiner postRasterizerRate,
												   const ShadingRateCombiner finalRate, Window* window)
{
	RendererAPI::GetRenderer()->SetShadingRate(shadingRate, texture, postRasterizerRate, finalRate, window);
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

void TRAP::Graphics::RenderCommand::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha, Window* window)
{
	RendererAPI::GetRenderer()->SetBlendMode(modeRGB, modeAlpha, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendConstant(const BlendConstant sourceRGBA,
                                                     const BlendConstant destinationRGBA, Window* window)
{
	RendererAPI::GetRenderer()->SetBlendConstant(sourceRGBA, sourceRGBA, destinationRGBA, destinationRGBA, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendConstant(const BlendConstant sourceRGB, const BlendConstant sourceAlpha,
	  												 const BlendConstant destinationRGB,
													 const BlendConstant destinationAlpha, Window* window)
{
	RendererAPI::GetRenderer()->SetBlendConstant(sourceRGB, sourceAlpha, destinationRGB, destinationAlpha, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width,
                                                const uint32_t height, Window* window)
{
	RendererAPI::GetRenderer()->SetViewport(x, y, width, height, 0.0f, 1.0f, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width,
											   const uint32_t height, Window* window)
{
	RendererAPI::GetRenderer()->SetScissor(x, y, width, height, window);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Graphics::RenderCommand::SetResolution(const uint32_t width, const uint32_t height, Window* window)
{
	RendererAPI::GetRenderer()->SetResolution(width, height, window);
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Draw(const uint32_t vertexCount, const uint32_t firstVertex, Window* window)
{
	RendererAPI::GetRenderer()->Draw(vertexCount, firstVertex, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::DrawIndexed(const uint32_t indexCount, const uint32_t firstIndex,
												const uint32_t firstVertex, Window* window)
{
	RendererAPI::GetRenderer()->DrawIndexed(indexCount, firstIndex, firstVertex, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::DrawInstanced(const uint32_t vertexCount, const uint32_t instanceCount,
	                                              const uint32_t firstVertex, const uint32_t firstInstance,
												  Window* window)
{
	RendererAPI::GetRenderer()->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::DrawIndexedInstanced(const uint32_t indexCount, const uint32_t instanceCount,
														 const uint32_t firstIndex, const uint32_t firstInstance,
														 const uint32_t firstVertex, Window* window)
{
	RendererAPI::GetRenderer()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, firstInstance,
	                                                 firstVertex, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Dispatch(const std::array<uint32_t, 3>& workGroupElementSizes, Window* window)
{
	RendererAPI::GetRenderer()->Dispatch(workGroupElementSizes, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetPushConstants(const char* name, const void* data, const QueueType queueType,
                                                     Window* window)
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
// 									                 Window* window)
// {
// 	RendererAPI::GetRenderer()->BindRenderTarget(colorTarget, depthStencil, loadActions, colorArraySlices,
// 												 colorMipSlices, depthArraySlice, depthMipSlice, window);
// }

//-------------------------------------------------------------------------------------------------------------------//

// void TRAP::Graphics::RenderCommand::BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
// 		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
// 									                 const RendererAPI::LoadActionsDesc* loadActions,
// 									                 std::vector<uint32_t>* colorArraySlices,
// 									                 std::vector<uint32_t>* colorMipSlices,
// 									                 const uint32_t depthArraySlice, const uint32_t depthMipSlice,
// 									                 Window* window)
// {
// 	RendererAPI::GetRenderer()->BindRenderTargets(colorTargets, depthStencil, loadActions, colorArraySlices,
// 												 colorMipSlices, depthArraySlice, depthMipSlice, window);
// }

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
									                 const RendererAPI::LoadActionsDesc* loadActions,
									                 Window* window)
{
	RendererAPI::GetRenderer()->BindRenderTarget(colorTarget, depthStencil, loadActions, nullptr, nullptr,
												 static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                                             const TRAP::Ref<Graphics::RenderTarget>& depthStencil,
									                 const RendererAPI::LoadActionsDesc* loadActions,
									                 Window* window)
{
	RendererAPI::GetRenderer()->BindRenderTargets(colorTargets, depthStencil, loadActions, nullptr, nullptr,
												  static_cast<uint32_t>(-1), static_cast<uint32_t>(-1), window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::StartRenderPass(Window* window)
{
	RendererAPI::GetRenderer()->StartRenderPass(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::StopRenderPass(Window* window)
{
	RendererAPI::GetRenderer()->StopRenderPass(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::BufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
                                                  const QueueType queueType, Window* window)
{
	RendererAPI::GetRenderer()->ResourceBufferBarrier(bufferBarrier, queueType, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::BufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
                                                   const QueueType queueType, Window* window)
{
	RendererAPI::GetRenderer()->ResourceBufferBarriers(bufferBarriers, queueType, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::TextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
                                                   const QueueType queueType, Window* window)
{
	RendererAPI::GetRenderer()->ResourceTextureBarrier(textureBarrier, queueType, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::TextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
           										    const QueueType queueType, Window* window)
{
	RendererAPI::GetRenderer()->ResourceTextureBarriers(textureBarriers, queueType, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::RenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
 														Window* window)
{
	RendererAPI::GetRenderer()->ResourceRenderTargetBarrier(renderTargetBarrier, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::RenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
 														 Window* window)
{
	RendererAPI::GetRenderer()->ResourceRenderTargetBarriers(renderTargetBarriers, window);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Image> TRAP::Graphics::RenderCommand::CaptureScreenshot(Window* window)
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
                                                    TRAP::Ref<RenderTarget> destination, Window* window)
{
	RendererAPI::GetRenderer()->MSAAResolvePass(source, destination, window);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime(Window* window)
{
	return RendererAPI::GetGPUGraphicsFrameTime(window);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime(Window* window)
{
	return RendererAPI::GetGPUComputeFrameTime(window);
}

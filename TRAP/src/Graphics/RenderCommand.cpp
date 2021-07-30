#include "TRAPPCH.h"
#include "RenderCommand.h"

#include "Application.h"

void TRAP::Graphics::RenderCommand::Present(const Scope<Window>& window)
{
	RendererAPI::GetRenderer()->Present(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetVSync(const bool vsync)
{
	RendererAPI::GetRenderer()->SetVSync(vsync);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::RenderCommand::GetVSync()
{
	return RendererAPI::GetRenderer()->GetVSync();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Clear(const RendererBufferType buffer)
{
	RendererAPI::GetRenderer()->Clear(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearColor(const Math::Vec4& color)
{
	RendererAPI::GetRenderer()->SetClearColor(color);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearDepth(const float depth)
{
	RendererAPI::GetRenderer()->SetClearDepth(depth);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearStencil(const uint32_t stencil)
{
	RendererAPI::GetRenderer()->SetClearStencil(stencil);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthTesting(const bool enabled)
{
	RendererAPI::GetRenderer()->SetDepthTesting(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthWriting(const bool enabled)
{
	RendererAPI::GetRenderer()->SetDepthWriting(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthFunction(const CompareMode compareMode)
{
	RendererAPI::GetRenderer()->SetDepthFunction(compareMode);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthFail(const StencilOperation front, const StencilOperation back)
{
	RendererAPI::GetRenderer()->SetDepthFail(front, back);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthBias(const int32_t bias)
{
	RendererAPI::GetRenderer()->SetDepthBias(bias);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthBiasSlopeFactor(const float slopeFactor)
{
	RendererAPI::GetRenderer()->SetDepthBiasSlopeFactor(slopeFactor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilTesting(const bool enabled)
{
	RendererAPI::GetRenderer()->SetStencilTesting(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilFail(const StencilOperation front, const StencilOperation back)
{
	RendererAPI::GetRenderer()->SetStencilFail(front, back);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilPass(const StencilOperation front, const StencilOperation back)
{
	RendererAPI::GetRenderer()->SetStencilPass(front, back);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilFunction(const CompareMode front, const CompareMode back)
{
	RendererAPI::GetRenderer()->SetStencilFunction(front, back);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilMask(const uint8_t read, const uint8_t write)
{
	RendererAPI::GetRenderer()->SetStencilMask(read, write);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetCullMode(const CullMode cullMode)
{
	RendererAPI::GetRenderer()->SetCullMode(cullMode);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetFillMode(const FillMode fillMode)
{
	RendererAPI::GetRenderer()->SetFillMode(fillMode);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetPrimitiveTopology(const PrimitiveTopology topology)
{
	RendererAPI::GetRenderer()->SetPrimitiveTopology(topology);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetFrontFace(const FrontFace face)
{
	RendererAPI::GetRenderer()->SetFrontFace(face);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha)
{
	RendererAPI::GetRenderer()->SetBlendMode(modeRGB, modeAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendConstant(const BlendConstant sourceRGB, const BlendConstant sourceAlpha,
	  												 const BlendConstant destinationRGB,
													 const BlendConstant destinationAlpha)
{
	RendererAPI::GetRenderer()->SetBlendConstant(sourceRGB, sourceAlpha, destinationRGB, destinationAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width,
                                                const uint32_t height)
{
	RendererAPI::GetRenderer()->SetViewport(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width,
											   const uint32_t height)
{
	RendererAPI::GetRenderer()->SetScissor(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Draw(const uint32_t vertexCount, const uint32_t firstVertex)
{
	RendererAPI::GetRenderer()->Draw(vertexCount, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::DrawIndexed(const uint32_t indexCount, const uint32_t firstIndex,
												const uint32_t firstVertex)
{
	RendererAPI::GetRenderer()->DrawIndexed(indexCount, firstIndex, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetPushConstants(const char* name, const void* data)
{
	RendererAPI::GetRenderer()->BindPushConstants(name, data);
}
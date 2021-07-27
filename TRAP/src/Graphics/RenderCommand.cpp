#include "TRAPPCH.h"
#include "RenderCommand.h"

#include "Application.h"

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Clear(const RendererBufferType buffer)
{
	RendererAPI::GetRenderer()->Clear(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Present(const Scope<Window>& window)
{
	RendererAPI::GetRenderer()->Present(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearColor(const Math::Vec4& color)
{
	RendererAPI::GetRenderer()->SetClearColor(color);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearDepthStencil(const float depth, const uint32_t stencil)
{
	RendererAPI::GetRenderer()->SetClearDepthStencil(depth, stencil);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthTesting(const bool enabled)
{
	RendererAPI::GetRenderer()->SetDepthTesting(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthMasking(const bool enabled)
{
	RendererAPI::GetRenderer()->SetDepthWriting(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthFunction(const RendererFunction function)
{
	//RendererAPI::GetRenderer()->SetDepthFunction(function);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlend(const bool enabled)
{
	//RendererAPI::GetRenderer()->SetBlend(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilTesting(const bool enabled)
{
	RendererAPI::GetRenderer()->SetStencilTesting(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilMasking(const uint32_t mask)
{
	//RendererAPI::GetRenderer()->SetStencilMasking(mask);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilMaskingSeparate(const RendererFaceMode face, const uint32_t mask)
{
	//RendererAPI::GetRenderer()->SetStencilMaskingSeparate(face, mask);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilFunction(const RendererFunction function, const int32_t reference,
                                                       const uint32_t mask)
{
	//RendererAPI::GetRenderer()->SetStencilFunction(function, reference, mask);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilFunctionSeparate(const RendererFaceMode face,
                                                               const RendererFunction function,
															   const int32_t reference, const uint32_t mask)
{
	//RendererAPI::GetRenderer()->SetStencilFunctionSeparate(face, function, reference, mask);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilOperation(const RendererOperation stencilFail,
                                                        const RendererOperation depthFail,
														const RendererOperation pass)
{
	//RendererAPI::GetRenderer()->SetStencilOperation(stencilFail, depthFail, pass);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetStencilOperationSeparate(const RendererFaceMode face,
                                                                const RendererOperation stencilFail,
																const RendererOperation depthFail,
																const RendererOperation pass)
{
	//RendererAPI::GetRenderer()->SetStencilOperationSeparate(face, stencilFail, depthFail, pass);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetCull(const bool enabled)
{
	//RendererAPI::GetRenderer()->SetCull(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetFrontFace(const RendererFrontFace frontFace)
{
	//RendererAPI::GetRenderer()->SetFrontFace(frontFace);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetWireFrame(const bool enabled)
{
	//RendererAPI::GetRenderer()->SetWireFrame(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width,
                                                const uint32_t height)
{
	RendererAPI::GetRenderer()->SetViewport(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendFunction(const RendererBlendFunction source,
                                                     const RendererBlendFunction destination)
{
	//RendererAPI::GetRenderer()->SetBlendFunction(source, destination);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendFunctionSeparate(const RendererBlendFunction sourceRGB,
															 const RendererBlendFunction sourceAlpha,
															 const RendererBlendFunction destinationRGB,
															 const RendererBlendFunction destinationAlpha)
{
	//RendererAPI::GetRenderer()->SetBlendFunctionSeparate(sourceRGB, sourceAlpha, destinationRGB, destinationAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendEquation(const RendererBlendEquation blendEquation)
{
	//RendererAPI::GetRenderer()->SetBlendEquation(blendEquation);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendEquationSeparate(const RendererBlendEquation blendEquationRGB,
                                                             const RendererBlendEquation blendEquationAlpha)
{
	//RendererAPI::GetRenderer()->SetBlendEquationSeparate(blendEquationRGB, blendEquationAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetCullMode(const RendererCullMode cullMode)
{
	//RendererAPI::GetRenderer()->SetCullMode(cullMode);
}

//-------------------------------------------------------------------------------------------------------------------//

/*void TRAP::Graphics::RenderCommand::DrawIndexed(const Scope<VertexArray>& vertexArray, const uint32_t indexCount)
{
	Application::AddSingleDrawCall();

	RendererAPI::GetRenderer()->DrawIndexed(vertexArray, indexCount);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Draw(const Scope<VertexArray>& vertexArray)
{
	Application::AddSingleDrawCall();

	RendererAPI::GetRenderer()->Draw(vertexArray);
}*/
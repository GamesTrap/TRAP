#include "TRAPPCH.h"
#include "RenderCommand.h"

#include "Application.h"

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Clear(const RendererBufferType buffer)
{
	API::RendererAPI::GetRenderer()->Clear(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Present(const std::unique_ptr<Window>& window)
{
	API::RendererAPI::GetRenderer()->Present(window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetClearColor(const Math::Vec4& color)
{
	API::RendererAPI::GetRenderer()->SetClearColor(color);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthTesting(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetDepthTesting(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthMasking(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetDepthMasking(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetDepthFunction(const RendererDepthFunction function)
{
	API::RendererAPI::GetRenderer()->SetDepthFunction(function);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlend(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetBlend(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetCull(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetCull(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetFrontFace(const RendererFrontFace frontFace)
{
	API::RendererAPI::GetRenderer()->SetFrontFace(frontFace);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetWireFrame(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetWireFrame(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
	API::RendererAPI::GetRenderer()->SetViewport(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination)
{
	API::RendererAPI::GetRenderer()->SetBlendFunction(source, destination);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendFunctionSeparate(const RendererBlendFunction sourceRGB,
	const RendererBlendFunction sourceAlpha,
	const RendererBlendFunction destinationRGB,
	const RendererBlendFunction destinationAlpha)
{
	API::RendererAPI::GetRenderer()->SetBlendFunctionSeparate(sourceRGB, sourceAlpha, destinationRGB, destinationAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendEquation(const RendererBlendEquation blendEquation)
{
	API::RendererAPI::GetRenderer()->SetBlendEquation(blendEquation);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetBlendEquationSeparate(const RendererBlendEquation blendEquationRGB, const RendererBlendEquation blendEquationAlpha)
{
	API::RendererAPI::GetRenderer()->SetBlendEquationSeparate(blendEquationRGB, blendEquationAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::SetCullMode(const RendererCullMode cullMode)
{
	API::RendererAPI::GetRenderer()->SetCullMode(cullMode);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::DrawIndexed(const Scope<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
	Application::AddSingleDrawCall();
	
	API::RendererAPI::GetRenderer()->DrawIndexed(vertexArray, primitive);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::RenderCommand::Draw(const Scope<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
	Application::AddSingleDrawCall();
	
	API::RendererAPI::GetRenderer()->Draw(vertexArray, primitive);
}
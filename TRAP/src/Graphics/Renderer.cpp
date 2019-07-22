#include "TRAPPCH.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Graphics/Buffers/VertexArray.h"

void TRAP::Graphics::Renderer::BeginScene()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::EndScene()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(const std::unique_ptr<VertexArray>& vertexArray)
{
	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}
#include "TRAPPCH.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Graphics/Buffers/VertexArray.h"
#include "OrthographicCamera.h"
#include "Graphics/Shaders/Shader.h"

std::unique_ptr<TRAP::Graphics::Renderer::SceneData> TRAP::Graphics::Renderer::s_sceneData = std::make_unique<SceneData>();

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::UniformBuffer> TRAP::Graphics::Renderer::s_uniformBuffer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(OrthographicCamera& camera)
{
	s_sceneData->m_projectionMatrix = Math::Mat4::Transpose(camera.GetProjectionMatrix());
	s_sceneData->m_viewMatrix = Math::Mat4::Transpose(camera.GetViewMatrix());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::EndScene()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(const Shader* shader, const std::unique_ptr<VertexArray>& vertexArray, const Math::Mat4& transform, const RendererPrimitive primitive)
{
	Application::Get().AddSingleDrawCall();

	s_sceneData->m_modelMatrix = Math::Mat4::Transpose(transform);
	if(shader)
	{
		shader->Bind();
		if (!s_uniformBuffer)
			s_uniformBuffer = UniformBuffer::Create("MatrixBuffer", s_sceneData.get(), sizeof(SceneData));
		else
		{
			s_uniformBuffer->Bind();
			s_uniformBuffer->UpdateData(s_sceneData.get(), sizeof(SceneData));
		}
	}

	vertexArray->Bind();
	if (vertexArray->GetIndexBuffer())
		RenderCommand::DrawIndexed(vertexArray, primitive);
	else
		RenderCommand::Draw(vertexArray, primitive);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Cleanup()
{
	s_uniformBuffer->Unbind();
	s_uniformBuffer.reset();
}
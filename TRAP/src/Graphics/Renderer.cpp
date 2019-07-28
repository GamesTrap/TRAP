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
	s_sceneData->m_projectionMatrix = Maths::Mat4::Transpose(camera.GetProjectionMatrix());
	s_sceneData->m_viewMatrix = Maths::Mat4::Transpose(camera.GetViewMatrix());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::EndScene()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(const API::Shader* shader, const std::unique_ptr<VertexArray>& vertexArray, const Maths::Mat4& transform)
{
	Application::Get().AddSingleDrawCall();

	s_sceneData->m_modelMatrix = Maths::Mat4::Transpose(transform);
	if(shader)
	{
		shader->Bind();
		if (!s_uniformBuffer)
			s_uniformBuffer = UniformBuffer::Create("MatrixBuffer", s_sceneData.get(), sizeof(SceneData), BufferUsage::DYNAMIC);
		else
			s_uniformBuffer->UpdateData(s_sceneData.get(), sizeof(SceneData), BufferUsage::DYNAMIC);
	}

	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Cleanup()
{
	s_uniformBuffer->Unbind();
	s_uniformBuffer.reset();
}
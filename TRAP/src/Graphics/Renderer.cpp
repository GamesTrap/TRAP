#include "TRAPPCH.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Graphics/Buffers/VertexArray.h"
#include "OrthographicCamera.h"
#include "Graphics/Shaders/Shader.h"

std::unique_ptr<TRAP::Graphics::Renderer::SceneData> TRAP::Graphics::Renderer::m_sceneData = std::make_unique<SceneData>();

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::UniformBuffer> TRAP::Graphics::Renderer::m_uniformBuffer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(OrthographicCamera& camera)
{
	m_sceneData->m_projectionMatrix = Maths::Mat4::Transpose(camera.GetProjectionMatrix());
	m_sceneData->m_viewMatrix = Maths::Mat4::Transpose(camera.GetViewMatrix());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::EndScene()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(const API::Shader* shader, const std::unique_ptr<VertexArray>& vertexArray)
{
	if(shader)
	{
		shader->Bind();
		if (!m_uniformBuffer)
			m_uniformBuffer = UniformBuffer::Create("MatrixBuffer", m_sceneData.get(), sizeof(SceneData), BufferUsage::DYNAMIC);
		else
			m_uniformBuffer->UpdateData(m_sceneData.get(), sizeof(SceneData), BufferUsage::DYNAMIC);
	}

	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Cleanup()
{
	m_uniformBuffer->Unbind();
	m_uniformBuffer.reset();
}
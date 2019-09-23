#include "TRAPPCH.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Cameras/Camera.h"
#include "Graphics/Shaders/Shader.h"

TRAP::Scope<TRAP::Graphics::Renderer::SceneData> TRAP::Graphics::Renderer::s_sceneData = MakeScope<SceneData>();

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::Renderer::s_uniformBuffer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(Camera& camera)
{
	s_sceneData->m_projectionMatrix = Math::Mat4::Transpose(camera.GetProjectionMatrix());
	s_sceneData->m_viewMatrix = Math::Mat4::Transpose(camera.GetViewMatrix());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::EndScene()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(const Scope<Shader>& shader, const Scope<VertexArray>& vertexArray, const Math::Mat4& transform, const RendererPrimitive primitive)
{
	Application::Get().AddSingleDrawCall();

	s_sceneData->m_modelMatrix = Math::Mat4::Transpose(transform);
	if(shader)
	{
		shader->Bind();
		if (!s_uniformBuffer)
			s_uniformBuffer = UniformBuffer::Create("MatrixBuffer", s_sceneData.get(), sizeof(SceneData), BufferUsage::Stream);
		else
		{
			s_uniformBuffer->UpdateData(s_sceneData.get());
			s_uniformBuffer->Bind(0);
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
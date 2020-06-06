#include "TRAPPCH.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Cameras/Camera.h"
#include "Graphics/Shaders/Shader.h"
#include "Renderer2D.h"

TRAP::Scope<TRAP::Graphics::Renderer::SceneData> TRAP::Graphics::Renderer::s_sceneData = MakeScope<SceneData>();

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::Renderer::s_uniformBuffer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Init()
{
	TP_PROFILE_FUNCTION();

	Renderer2D::Init();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::Renderer::GetTitle()
{
	return API::RendererAPI::GetRenderer()->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Renderer::GetDrawCalls()
{
	return Application::GetDrawCalls();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Renderer::GetFPS()
{
	return Application::GetFPS();
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::Renderer::GetFrameTime()
{
	return Application::GetFrameTime();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::Renderer::GetTickRate()
{
	return Application::GetTickRate();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::SetTickRate(const uint32_t tickRate)
{
	Application::SetTickRate(tickRate);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(Camera& camera)
{
	TP_PROFILE_FUNCTION();

	s_sceneData->m_projectionMatrix = camera.GetProjectionMatrix();
	s_sceneData->m_viewMatrix = camera.GetViewMatrix();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::EndScene()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(const Scope<Shader>& shader, const Scope<VertexArray>& vertexArray, const Math::Mat4& transform, const RendererPrimitive primitive)
{
	TP_PROFILE_FUNCTION();

	s_sceneData->m_modelMatrix = transform;
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
		RenderCommand::DrawIndexed(vertexArray, 0, primitive);
	else
		RenderCommand::Draw(vertexArray, primitive);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Shutdown()
{
	TP_PROFILE_FUNCTION();

	Renderer2D::Shutdown();
	
	if(s_uniformBuffer)
	{
		s_uniformBuffer->Unbind(0);
		s_uniformBuffer.reset();		
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Renderer::SceneData::SceneData()
	: m_projectionMatrix(0.0f), m_viewMatrix(0.0f), m_modelMatrix(0.0f)
{
}
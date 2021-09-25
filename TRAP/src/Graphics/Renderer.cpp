#include "TRAPPCH.h"
#include "Renderer.h"

#include "Buffers/VertexBufferLayout.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"
#include "Cameras/Camera.h"
#include "Cameras/Orthographic/OrthographicCamera.h"
#include "Graphics/Shaders/Shader.h"
#include "Renderer2D.h"
#include "Graphics/RenderCommand.h"

TRAP::Scope<TRAP::Graphics::Renderer::SceneData> TRAP::Graphics::Renderer::s_sceneData = MakeScope<SceneData>();

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::Renderer::s_uniformBuffer = nullptr;
TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::Renderer::s_modelStorageBuffer = nullptr;
uint32_t TRAP::Graphics::Renderer::s_maxDrawCalls = 1000;
uint32_t TRAP::Graphics::Renderer::s_currentDrawCalls = 0;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Init()
{
	TP_PROFILE_FUNCTION();

	s_maxDrawCalls = RendererAPI::GPUSettings.MaxUniformBufferRange /
	                 UniformBuffer::CalculateAlignedSize(sizeof(Math::Mat4));

	s_uniformBuffer = TRAP::Graphics::UniformBuffer::Create(s_sceneData.get(), sizeof(SceneData),
	 														TRAP::Graphics::UpdateFrequency::Dynamic);
	s_modelStorageBuffer = TRAP::Graphics::StorageBuffer::Create(sizeof(TRAP::Math::Mat4) * s_maxDrawCalls,
	                                                             TRAP::Graphics::UpdateFrequency::Dynamic);

	Renderer2D::Init();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::Renderer::GetTitle()
{
	return RendererAPI::GetRenderer()->GetTitle();
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

std::string TRAP::Graphics::Renderer::GetCurrentGPUName()
{
	return RendererAPI::GetRenderer()->GetCurrentGPUName();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::SetTickRate(const uint32_t tickRate)
{
	Application::SetTickRate(tickRate);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(const OrthographicCamera& camera)
{
	TP_PROFILE_FUNCTION();

	s_currentDrawCalls = 0;

	s_sceneData->m_projectionMatrix = camera.GetProjectionMatrix();
	s_sceneData->m_viewMatrix = camera.GetViewMatrix();
	s_uniformBuffer->SetData(s_sceneData.get(), sizeof(SceneData));
	s_uniformBuffer->AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(const Camera& camera, const Math::Mat4& transform)
{
	TP_PROFILE_FUNCTION();

	s_currentDrawCalls = 0;

	s_sceneData->m_projectionMatrix = camera.GetProjectionMatrix();
	s_sceneData->m_viewMatrix = Math::Inverse(transform);
	s_uniformBuffer->SetData(s_sceneData.get(), sizeof(SceneData));
	s_uniformBuffer->AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::EndScene()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(Shader* shader, VertexBuffer* vertexBuffer, const Math::Mat4& transform)
{
	TRAP_ASSERT(vertexBuffer, "VertexBuffer is nullptr!");

	TP_PROFILE_FUNCTION();

	if(s_currentDrawCalls >= s_maxDrawCalls)
		return;

	s_modelStorageBuffer->SetData(&transform, sizeof(Math::Mat4),
	                              s_currentDrawCalls * StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));
	s_modelStorageBuffer->AwaitLoading();

	vertexBuffer->Use();
	if(shader)
	{
		shader->UseSSBO(1, 1, s_modelStorageBuffer.get(), s_maxDrawCalls * sizeof(Math::Mat4));
		shader->UseUBO(1, 0, s_uniformBuffer.get());
		shader->Use();
	}

	RenderCommand::DrawInstanced(vertexBuffer->GetCount(), s_currentDrawCalls + 1, 0, s_currentDrawCalls);
	++s_currentDrawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(Shader* shader, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer,
									  const Math::Mat4& transform)
{
	TRAP_ASSERT(vertexBuffer, "VertexBuffer is nullptr!");
	TRAP_ASSERT(indexBuffer, "IndexBuffer is nullptr!");

	TP_PROFILE_FUNCTION();

	if(s_currentDrawCalls >= s_maxDrawCalls)
		return;

	s_modelStorageBuffer->SetData(&transform, sizeof(Math::Mat4),
	                              s_currentDrawCalls * StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));
	s_modelStorageBuffer->AwaitLoading();

	vertexBuffer->Use();
	if(shader)
	{
		shader->UseSSBO(1, 1, s_modelStorageBuffer.get(), s_maxDrawCalls * sizeof(Math::Mat4));
		shader->UseUBO(1, 0, s_uniformBuffer.get());
		shader->Use();
	}
	indexBuffer->Use();

	RenderCommand::DrawIndexedInstanced(indexBuffer->GetCount(), s_currentDrawCalls + 1, 0, s_currentDrawCalls);
	++s_currentDrawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Shutdown()
{
	TP_PROFILE_FUNCTION();

	Renderer2D::Shutdown();

	if(s_uniformBuffer)
		s_uniformBuffer.reset();

	if(s_modelStorageBuffer)
	   s_modelStorageBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Renderer::SceneData::SceneData()
	: m_projectionMatrix(0.0f), m_viewMatrix(0.0f)
{
}
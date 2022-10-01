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

TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::Renderer::s_sceneStorageBuffer = nullptr;
TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::Renderer::s_modelStorageBuffer = nullptr;
uint32_t TRAP::Graphics::Renderer::s_maxDrawCalls = 1000;
uint32_t TRAP::Graphics::Renderer::s_currentDrawCalls = 0;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Init()
{
	ZoneScopedC(tracy::Color::Red);

	s_maxDrawCalls = RendererAPI::GPUSettings.MaxStorageBufferRange /
	                 static_cast<uint32_t>(StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));
	s_maxDrawCalls = TRAP::Math::Min(s_maxDrawCalls, 1000000u); //Max 1 million draw calls (so we don't exceed heap sizes)

	s_sceneStorageBuffer = TRAP::Graphics::StorageBuffer::Create(s_sceneData.get(), sizeof(SceneData),
	 														     TRAP::Graphics::UpdateFrequency::Dynamic);
	s_modelStorageBuffer = TRAP::Graphics::StorageBuffer::Create(sizeof(TRAP::Math::Mat4) * s_maxDrawCalls,
	                                                             TRAP::Graphics::UpdateFrequency::Dynamic);

	Renderer2D::Init();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Shutdown()
{
	ZoneScopedC(tracy::Color::Red);

	Renderer2D::Shutdown();

	if(s_sceneStorageBuffer)
		s_sceneStorageBuffer.reset();

	if(s_modelStorageBuffer)
	   s_modelStorageBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(const OrthographicCamera& camera)
{
	ZoneScopedC(tracy::Color::Red);

	s_currentDrawCalls = 0;

	s_sceneData->m_projectionMatrix = camera.GetProjectionMatrix();
	s_sceneData->m_viewMatrix = camera.GetViewMatrix();
	s_sceneStorageBuffer->SetData(s_sceneData.get(), sizeof(SceneData));
	s_sceneStorageBuffer->AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(const Camera& camera, const Math::Mat4& transform)
{
	ZoneScopedC(tracy::Color::Red);

	s_currentDrawCalls = 0;

	s_sceneData->m_projectionMatrix = camera.GetProjectionMatrix();
	s_sceneData->m_viewMatrix = Math::Inverse(transform);
	s_sceneStorageBuffer->SetData(s_sceneData.get(), sizeof(SceneData));
	s_sceneStorageBuffer->AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::EndScene()
{
	ZoneScopedC(tracy::Color::Red);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(Ref<Shader> shader, const VertexBuffer* const vertexBuffer, const Math::Mat4& transform)
{
	ZoneScopedC(tracy::Color::Red);

	TRAP_ASSERT(vertexBuffer, "VertexBuffer is nullptr!");

	if(s_currentDrawCalls >= s_maxDrawCalls)
		return;

	s_modelStorageBuffer->SetData(&transform, sizeof(Math::Mat4),
	                              s_currentDrawCalls * StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));
	s_modelStorageBuffer->AwaitLoading();

	vertexBuffer->Use();
	if(shader)
	{
		shader->UseSSBO(1, 1, s_modelStorageBuffer.get(), s_maxDrawCalls * sizeof(Math::Mat4));
		shader->UseSSBO(1, 0, s_sceneStorageBuffer.get());
		shader->Use();
	}

	RenderCommand::DrawInstanced(vertexBuffer->GetCount(), s_currentDrawCalls + 1, 0, s_currentDrawCalls);
	++s_currentDrawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(Ref<Shader> shader, const VertexBuffer* const vertexBuffer, const IndexBuffer* const indexBuffer,
									  const Math::Mat4& transform)
{
	ZoneScopedC(tracy::Color::Red);

	TRAP_ASSERT(vertexBuffer, "VertexBuffer is nullptr!");
	TRAP_ASSERT(indexBuffer, "IndexBuffer is nullptr!");

	if(s_currentDrawCalls >= s_maxDrawCalls)
		return;

	s_modelStorageBuffer->SetData(&transform, sizeof(Math::Mat4),
	                              s_currentDrawCalls * StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));
	s_modelStorageBuffer->AwaitLoading();

	vertexBuffer->Use();
	indexBuffer->Use();
	if(shader)
	{
		shader->UseSSBO(1, 1, s_modelStorageBuffer.get(), s_maxDrawCalls * sizeof(Math::Mat4));
		shader->UseSSBO(1, 0, s_sceneStorageBuffer.get());
		shader->Use();
	}

	RenderCommand::DrawIndexedInstanced(indexBuffer->GetCount(), s_currentDrawCalls + 1, 0, s_currentDrawCalls);
	++s_currentDrawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Renderer::SceneData::SceneData() noexcept
	: m_projectionMatrix(0.0f), m_viewMatrix(0.0f)
{
	ZoneScopedC(tracy::Color::Red);
}
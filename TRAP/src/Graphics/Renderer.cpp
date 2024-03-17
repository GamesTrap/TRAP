#include "TRAPPCH.h"
#include "Renderer.h"

#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"
#include "Buffers/StorageBuffer.h"
#include "Cameras/Camera.h"
#include "Cameras/Orthographic/OrthographicCamera.h"
#include "Graphics/Shaders/Shader.h"
#include "Renderer2D.h"
#include "Graphics/RenderCommand.h"

TRAP::Scope<TRAP::Graphics::Renderer::SceneData> TRAP::Graphics::Renderer::s_sceneData = MakeScope<SceneData>();

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::Renderer::s_sceneStorageBuffer = nullptr;
TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::Renderer::s_modelStorageBuffer = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Init()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	s_maxDrawCalls = NumericCast<u32>(RendererAPI::GPUSettings.MaxStorageBufferRange /
	                                       StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));
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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Renderer2D::Shutdown();

	if(s_sceneStorageBuffer)
		s_sceneStorageBuffer.reset();

	if(s_modelStorageBuffer)
	   s_modelStorageBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(const OrthographicCamera& camera)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	s_currentDrawCalls = 0;

	s_sceneData->m_projectionMatrix = camera.GetProjectionMatrix();
	s_sceneData->m_viewMatrix = camera.GetViewMatrix();
	s_sceneStorageBuffer->SetData(s_sceneData.get(), sizeof(SceneData));
	s_sceneStorageBuffer->AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(const Camera& camera, const Math::Mat4& transform)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	s_currentDrawCalls = 0;

	s_sceneData->m_projectionMatrix = camera.GetProjectionMatrix();
	s_sceneData->m_viewMatrix = Math::Inverse(transform);
	s_sceneStorageBuffer->SetData(s_sceneData.get(), sizeof(SceneData));
	s_sceneStorageBuffer->AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(const Ref<Shader>& shader, const VertexBuffer* const vertexBuffer, const Math::Mat4& transform)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(vertexBuffer, "Renderer::Submit(): VertexBuffer is nullptr!");

	if(s_currentDrawCalls >= s_maxDrawCalls)
		return;

	s_modelStorageBuffer->SetData(&transform, sizeof(Math::Mat4),
	                              NumericCast<u64>(s_currentDrawCalls) * StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));
	s_modelStorageBuffer->AwaitLoading();

	vertexBuffer->Use();
	if(shader)
	{
		shader->UseSSBO(1, 1, s_modelStorageBuffer.get(), s_maxDrawCalls * sizeof(Math::Mat4));
		shader->UseSSBO(1, 0, s_sceneStorageBuffer.get());
		shader->Use();
	}

	RenderCommand::DrawInstanced(vertexBuffer->GetCount(), 1, 0, s_currentDrawCalls);
	++s_currentDrawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(const Ref<Shader>& shader, const VertexBuffer* const vertexBuffer, const IndexBuffer* const indexBuffer,
									  const Math::Mat4& transform)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(vertexBuffer, "Renderer::Submit(): VertexBuffer is nullptr!");
	TRAP_ASSERT(indexBuffer, "Renderer::Submit(): IndexBuffer is nullptr!");

	if(s_currentDrawCalls >= s_maxDrawCalls)
		return;

	s_modelStorageBuffer->SetData(&transform, sizeof(Math::Mat4),
	                              NumericCast<u64>(s_currentDrawCalls) * StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));
	s_modelStorageBuffer->AwaitLoading();

	vertexBuffer->Use();
	indexBuffer->Use();
	if(shader)
	{
		shader->UseSSBO(1, 1, s_modelStorageBuffer.get(), s_maxDrawCalls * sizeof(Math::Mat4));
		shader->UseSSBO(1, 0, s_sceneStorageBuffer.get());
		shader->Use();
	}

	RenderCommand::DrawIndexedInstanced(indexBuffer->GetCount(), 1, 0, s_currentDrawCalls);
	++s_currentDrawCalls;
}

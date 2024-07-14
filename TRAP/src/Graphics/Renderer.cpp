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

namespace
{
	struct SceneData
	{
		TRAP::Math::Mat4 m_projectionMatrix;
		TRAP::Math::Mat4 m_viewMatrix;
	};

	TRAP::Scope<SceneData> SceneDataObj = TRAP::MakeScope<SceneData>();
	TRAP::Scope<TRAP::Graphics::StorageBuffer> SceneStorageBuffer = nullptr;
	TRAP::Scope<TRAP::Graphics::StorageBuffer> ModelStorageBuffer = nullptr;

	constinit u32 MaxDrawCalls = 1000;
	constinit u32 CurrentDrawCalls = 0;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Init()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	MaxDrawCalls = NumericCast<u32>(RendererAPI::GPUSettings.MaxStorageBufferRange /
	                                StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));
	MaxDrawCalls = TRAP::Math::Min(MaxDrawCalls, 1000000u); //Max 1 million draw calls (so we don't exceed heap sizes)

	SceneStorageBuffer = TRAP::Graphics::StorageBuffer::Create(SceneDataObj.get(), sizeof(SceneData),
	 														   TRAP::Graphics::UpdateFrequency::Dynamic);
	ModelStorageBuffer = TRAP::Graphics::StorageBuffer::Create(sizeof(TRAP::Math::Mat4) * MaxDrawCalls,
	                                                           TRAP::Graphics::UpdateFrequency::Dynamic);

	Renderer2D::Init();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Renderer2D::Shutdown();

	if(SceneStorageBuffer)
		SceneStorageBuffer.reset();

	if(ModelStorageBuffer)
	   ModelStorageBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(const OrthographicCamera& camera)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	CurrentDrawCalls = 0;

	SceneDataObj->m_projectionMatrix = camera.GetProjectionMatrix();
	SceneDataObj->m_viewMatrix = camera.GetViewMatrix();
	SceneStorageBuffer->SetData(SceneDataObj.get(), sizeof(SceneData));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::BeginScene(const Camera& camera, const Math::Mat4& transform)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	CurrentDrawCalls = 0;

	SceneDataObj->m_projectionMatrix = camera.GetProjectionMatrix();
	SceneDataObj->m_viewMatrix = Math::Inverse(transform);
	SceneStorageBuffer->SetData(SceneDataObj.get(), sizeof(SceneData));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::EndScene()
{
	SceneStorageBuffer->AwaitLoading();
	ModelStorageBuffer->AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(Shader& shader, const VertexBuffer& vertexBuffer, const Math::Mat4& transform)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(CurrentDrawCalls >= MaxDrawCalls)
		return;

	ModelStorageBuffer->SetData(&transform, sizeof(Math::Mat4),
	                            NumericCast<u64>(CurrentDrawCalls) * StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));

	vertexBuffer.Use();
	shader.UseSSBO(1, 1, *ModelStorageBuffer, MaxDrawCalls * sizeof(Math::Mat4));
	shader.UseSSBO(1, 0, *SceneStorageBuffer);
	shader.Use();

	RenderCommand::DrawInstanced(vertexBuffer.GetCount(), 1, 0, CurrentDrawCalls);
	++CurrentDrawCalls;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Renderer::Submit(Shader& shader, const VertexBuffer& vertexBuffer,
                                      const IndexBuffer& indexBuffer, const Math::Mat4& transform)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(CurrentDrawCalls >= MaxDrawCalls)
		return;

	ModelStorageBuffer->SetData(&transform, sizeof(Math::Mat4),
	                            NumericCast<u64>(CurrentDrawCalls) * StorageBuffer::CalculateAlignedSize(sizeof(Math::Mat4)));

	vertexBuffer.Use();
	indexBuffer.Use();
	shader.UseSSBO(1, 1, *ModelStorageBuffer, MaxDrawCalls * sizeof(Math::Mat4));
	shader.UseSSBO(1, 0, *SceneStorageBuffer);
	shader.Use();

	RenderCommand::DrawIndexedInstanced(indexBuffer.GetCount(), 1, 0, CurrentDrawCalls);
	++CurrentDrawCalls;
}

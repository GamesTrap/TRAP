#ifndef TRAP_RENDERER_H
#define TRAP_RENDERER_H

#include "Application.h"

namespace TRAP::Graphics
{
	class OrthographicCamera; //TODO Remove when BeginSecene(OrthographicCamera) gets removed
	class Shader;
	class VertexBuffer;
	class IndexBuffer;
	class UniformBuffer;
	class StorageBuffer;

	class Camera;
	class Renderer
	{
	public:
		static void Init();

		static const std::string& GetTitle();
		static uint32_t GetFPS();
		static float GetFrameTime();
		static uint32_t GetTickRate();
		static std::string GetCurrentGPUName();

		static void SetTickRate(uint32_t tickRate);

		[[deprecated("Use BeginScene(Camera, Mat4) instead!")]]
		static void BeginScene(const OrthographicCamera& camera); //TODO
		static void BeginScene(const Camera& camera, const Math::Mat4& transform);
		static void EndScene();

		static void Submit(Shader* shader, VertexBuffer* vertexBuffer,
						   const Math::Mat4& transform = Math::Mat4(1.0f));
		static void Submit(Shader* shader, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer,
						   const Math::Mat4& transform = Math::Mat4(1.0f));

		static void Shutdown();

	private:
		struct SceneData
		{
			SceneData();

			Math::Mat4 m_projectionMatrix;
			Math::Mat4 m_viewMatrix;
		};

		static Scope<SceneData> s_sceneData;
		static Scope<UniformBuffer> s_uniformBuffer;
		static Scope<StorageBuffer> s_modelStorageBuffer;

		static uint32_t s_maxDrawCalls;
		static uint32_t s_currentDrawCalls;
	};
}

#endif /*TRAP_RENDERER_H*/
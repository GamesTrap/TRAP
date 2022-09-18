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
		/// <summary>
		/// Initialize Renderer.
		/// </summary>
		static void Init();
		/// <summary>
		/// Shutdown Renderer.
		/// </summary>
		static void Shutdown();

		/// <summary>
		/// Retrieve the renderer title.
		/// Example when using Vulkan: "[Vulkan 1.1.108]"
		/// </summary>
		/// <returns>Renderer title.</returns>
		static std::string GetTitle();
		/// <summary>
		/// Retrieve the frames per second.
		/// </summary>
		/// <returns>Frames per second.</returns>
		static uint32_t GetFPS();
		/// <summary>
		/// Retrieve the CPU frame time.
		/// </summary>
		/// <returns>CPU frame time in milliseconds.</returns>
		static float GetCPUFrameTime();
		/// <summary>
		/// Retrieve the current set tick rate.
		/// Default tick rate is 64.
		/// </summary>
		/// <returns>Tick rate.</returns>
		static uint32_t GetTickRate();
		/// <summary>
		/// Retrieve the name of the currently used GPU.
		/// </summary>
		/// <returns>Name of GPU.</returns>
		static std::string GetGPUName();

		/// <summary>
		/// Set the tick rate.
		/// </summary>
		/// <param name="tickRate">New tick rate.</param>
		static void SetTickRate(uint32_t tickRate);

		/// <summary>
		/// Begin a Renderer scene.
		/// </summary>
		/// <param name="camera">Orthographic camera.</param>
		[[deprecated("Use BeginScene(Camera, Mat4) instead!")]]
		static void BeginScene(const OrthographicCamera& camera); //TODO
		/// <summary>
		/// Begin a Renderer scene.
		/// </summary>
		/// <param name="camera">Main camera.</param>
		/// <param name="transform">Camera transform.</param>
		static void BeginScene(const Camera& camera, const Math::Mat4& transform);
		/// <summary>
		/// End a running Renderer scene.
		/// </summary>
		static void EndScene();

		/// <summary>
		/// Submit a non-indexed draw call.
		/// </summary>
		/// <param name="shader">Shader to use.</param>
		/// <param name="vertexBuffer">Vertex buffer to use.</param>
		/// <param name="transform">Model transform. Default is identity.</param>
		static void Submit(Ref<Shader> shader, const VertexBuffer* vertexBuffer,
						   const Math::Mat4& transform = Math::Mat4(1.0f));
		/// <summary>
		/// Submit an indexed draw call.
		/// </summary>
		/// <param name="shader">Shader to use.</param>
		/// <param name="vertexBuffer">Vertex buffer to use.</param>
		/// <param name="indexBuffer">Index buffer to use.</param>
		/// <param name="transform">Model transform. Default is identity.</param>
		static void Submit(Ref<Shader> shader, const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer,
						   const Math::Mat4& transform = Math::Mat4(1.0f));

	private:
		struct SceneData
		{
			/// <summary>
			/// Constructor.
			/// </summary>
			SceneData() noexcept;

			Math::Mat4 m_projectionMatrix;
			Math::Mat4 m_viewMatrix;
		};

		static Scope<SceneData> s_sceneData;
		static Scope<StorageBuffer> s_sceneStorageBuffer;
		static Scope<StorageBuffer> s_modelStorageBuffer;

		static uint32_t s_maxDrawCalls;
		static uint32_t s_currentDrawCalls;
	};
}

#endif /*TRAP_RENDERER_H*/
#ifndef TRAP_RENDERER_H
#define TRAP_RENDERER_H

#include "Maths/Types.h"
#include "Maths/Mat4.h"

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
		static constexpr void EndScene() noexcept;

		/// <summary>
		/// Submit a non-indexed draw call.
		/// </summary>
		/// <param name="shader">Shader to use.</param>
		/// <param name="vertexBuffer">Vertex buffer to use.</param>
		/// <param name="transform">Model transform. Default is identity.</param>
		static void Submit(const Ref<Shader>& shader, const VertexBuffer* vertexBuffer,
						   const Math::Mat4& transform = Math::Mat4(1.0f));
		/// <summary>
		/// Submit an indexed draw call.
		/// </summary>
		/// <param name="shader">Shader to use.</param>
		/// <param name="vertexBuffer">Vertex buffer to use.</param>
		/// <param name="indexBuffer">Index buffer to use.</param>
		/// <param name="transform">Model transform. Default is identity.</param>
		static void Submit(const Ref<Shader>& shader, const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer,
						   const Math::Mat4& transform = Math::Mat4(1.0f));

	private:
		struct SceneData
		{
			Math::Mat4 m_projectionMatrix;
			Math::Mat4 m_viewMatrix;
		};

		static Scope<SceneData> s_sceneData;
		static Scope<StorageBuffer> s_sceneStorageBuffer;
		static Scope<StorageBuffer> s_modelStorageBuffer;

		inline constinit static uint32_t s_maxDrawCalls = 1000;
		inline constinit static uint32_t s_currentDrawCalls = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::Renderer::EndScene() noexcept
{
}

#endif /*TRAP_RENDERER_H*/
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
		/// @brief Initialize Renderer.
		static void Init();
		/// @brief Shutdown Renderer.
		static void Shutdown();

		/// @brief Begin a Renderer scene.
		/// @param camera Orthographic camera.
		[[deprecated("Use BeginScene(Camera, Mat4) instead!")]]
		static void BeginScene(const OrthographicCamera& camera); //TODO
		/// @brief Begin a Renderer scene.
		/// @param camera Main camera.
		/// @param transform Camera transform.
		static void BeginScene(const Camera& camera, const Math::Mat4& transform);
		/// @brief End a running Renderer scene.
		static void EndScene();

		/// @brief Submit a non-indexed draw call.
		/// @param shader Shader to use.
		/// @param vertexBuffer Vertex buffer to use.
		/// @param transform Model transform. Default is identity.
		static void Submit(const Ref<Shader>& shader, const VertexBuffer* vertexBuffer,
						   const Math::Mat4& transform = Math::Mat4(1.0f));
		/// @brief Submit an indexed draw call.
		/// @param shader Shader to use.
		/// @param vertexBuffer Vertex buffer to use.
		/// @param indexBuffer Index buffer to use.
		/// @param transform Model transform. Default is identity.
		static void Submit(const Ref<Shader>& shader, const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer,
						   const Math::Mat4& transform = Math::Mat4(1.0f));
	};
}

#endif /*TRAP_RENDERER_H*/

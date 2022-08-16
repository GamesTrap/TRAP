#ifndef TRAP_RENDERER2D_H
#define TRAP_RENDERER2D_H

#include "Cameras/Orthographic/OrthographicCamera.h"
#include "Textures/Texture.h"
#include "Textures/SubTexture2D.h"

namespace TRAP::Graphics
{
	class Camera;
	struct Renderer2DData;

	class Renderer2D
	{
	public:
		/// <summary>
		/// Initialize Renderer2D.
		/// </summary>
		static void Init();
		/// <summary>
		/// Shutdown Renderer2D.
		/// </summary>
		static void Shutdown();

		/// <summary>
		/// Begin a Renderer2D scene.
		/// </summary>
		/// <param name="camera">Main camera.</param>
		/// <param name="transform">Camera transform.</param>
		static void BeginScene(const Camera& camera, const Math::Mat4& transform);
		/// <summary>
		/// Begin a Renderer2D scene.
		/// </summary>
		/// <param name="camera">Orthographic camera.</param>
		[[deprecated("Use BeginScene(Camera, Mat4) instead!")]]
		static void BeginScene(const OrthographicCamera& camera);
		/// <summary>
		/// End a running Renderer2D scene.
		/// </summary>
		static void EndScene();

		/// <summary>
		/// Struct to describe position, rotation and scale.
		/// </summary>
		struct Transform
		{
			Math::Vec3 Position = {0.0f, 0.0f, 0.0f};
			Math::Vec3 Rotation = {0.0f, 0.0f, 0.0f};
			Math::Vec3 Scale = {1.0f, 1.0f, 1.0f};
		};

		//Primitives

		/// <summary>
		/// Draw a colored quad.
		/// </summary>
		/// <param name="transform">Transform of the quad.</param>
		/// <param name="color">Color for the quad.</param>
		static void DrawQuad(const Transform& transform, const Math::Vec4& color);
		/// <summary>
		/// Draw a textured quad.
		/// </summary>
		/// <param name="transform">Transform of the quad.</param>
		/// <param name="texture">Texture for the quad.</param>
		static void DrawQuad(const Transform& transform, Texture* const texture);
		/// <summary>
		/// Draw a colored and textured quad.
		/// </summary>
		/// <param name="transform">Transform of the quad.</param>
		/// <param name="color">Color for the quad.</param>
		/// <param name="texture">Texture for the quad.</param>
		static void DrawQuad(const Transform& transform, const Math::Vec4& color,
		                     Texture* const texture);
		/// <summary>
		/// Draw a textured quad.
		/// </summary>
		/// <param name="transform">Transform of the quad.</param>
		/// <param name="texture">Texture for the quad.</param>
		static void DrawQuad(const Transform& transform, TRAP::Ref<SubTexture2D> texture);
		/// <summary>
		/// Draw a colored and textured quad.
		/// </summary>
		/// <param name="transform">Transform of the quad.</param>
		/// <param name="color">Color for the quad.</param>
		/// <param name="texture">Texture for the quad.</param>
		static void DrawQuad(const Transform& transform, const Math::Vec4& color,
		                     TRAP::Ref<SubTexture2D> texture);
		/// <summary>
		/// Draw a colored and textured quad.
		/// This is the base function for all the other DrawQuad functions.
		/// </summary>
		/// <param name="transform">Transform matrix for the quad.</param>
		/// <param name="color">Color for the quad.</param>
		/// <param name="texture">Texture for the quad.</param>
		/// <param name="texCoords">Optional: Texture coordinates for the quad.</param>
		static void DrawQuad(const TRAP::Math::Mat4& transform, const Math::Vec4& color,
		                     Texture* const texture, const std::array<Math::Vec2, 4>* texCoords);

		//Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			/// <summary>
			/// Retrieve the current total number of vertices.
			/// </summary>
			/// <returns>Total vertices count.</returns>
			uint32_t GetTotalVertexCount() const;
			/// <summary>
			/// Retrieve the current total number of indices.
			/// </summary>
			/// <returns>Total indices count.</returns>
			uint32_t GetTotalIndexCount() const;
		};
		/// <summary>
		/// Retrieve the current Renderer2D statistics.
		/// </summary>
		/// <returns>Renderer2D statistics.</returns>
		static Statistics GetStats();
		/// <summary>
		/// Reset the Renderer2D statistics.
		/// </summary>
		static void ResetStats();

	private:
		/// <summary>
		/// Reset pointers and current texture slot for a new draw call.
		/// </summary>
		static void Reset();
		/// <summary>
		/// Utility to retrieve the index for the given texture.
		/// </summary>
		/// <param name="texture">Texture to find index for.</param>
		/// <returns>Index to the texture.</returns>
		static float GetTextureIndex(Texture* texture);
		/// <summary>
		/// Allocated new buffers for a new draw call.
		/// Note: If the buffers are already allocated, this function will do nothing.
		/// </summary>
		static void ExtendBuffers();

		static Renderer2DData s_data;
	};
}

#endif /*TRAP_RENDERER2D_H*/
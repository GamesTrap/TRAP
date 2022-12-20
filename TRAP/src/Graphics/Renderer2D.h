#ifndef TRAP_RENDERER2D_H
#define TRAP_RENDERER2D_H

#include "Maths/Math.h"

namespace TRAP
{
	struct SpriteRendererComponent;
}
namespace TRAP::Graphics
{
	class Camera;
	class OrthographicCamera;
	class EditorCamera;
	class Texture;
	class Sampler;
	class SubTexture2D;
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
		/// Reset the Renderer2D for a new frame.
		///
		/// Note: This functions gets called by TRAP::Application
		/// </summary>
		static void Reset() noexcept;

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
		/// Begin a Renderer2D scene.
		/// </summary>
		/// <param name="camera">Editor camera.</param>
		static void BeginScene(const EditorCamera& camera);
		/// <summary>
		/// End a running Renderer2D scene.
		/// </summary>
		static void EndScene();

		/// <summary>
		/// Set a custom sampler to use by the 2D renderer.
		/// </summary>
		/// <param name="sampler">Sampler to use.</param>
		static void SetCustomSampler(TRAP::Ref<TRAP::Graphics::Sampler> sampler);

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
		static void DrawQuad(const Transform& transform, Ref<Texture> texture);
		/// <summary>
		/// Draw a colored and textured quad.
		/// </summary>
		/// <param name="transform">Transform of the quad.</param>
		/// <param name="color">Color for the quad.</param>
		/// <param name="texture">Texture for the quad.</param>
		static void DrawQuad(const Transform& transform, const Math::Vec4& color,
		                     Ref<Texture> texture);
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
		/// <param name="entityID">Optional: Entity ID of the quad.</param>
		static void DrawQuad(const TRAP::Math::Mat4& transform, const Math::Vec4& color,
		                     Ref<Texture> texture, const std::array<Math::Vec2, 4>* texCoords, int32_t entityID = -1);

		/// <summary>
		/// Draw a colored circle with given thickness and fading.
		/// This is the base function for all the other DrawCircle functions.
		/// </summary>
		/// <param name="transform">Transform matrix for the circle.</param>
		/// <param name="color">Color for the circle.</param>
		/// <param name="thickness">Optional: Thickness for the circle.</param>
		/// <param name="fade">Optional: Fade for the circle.</param>
		/// <param name="entityID">Optional: Entity ID of the quad.</param>
		static void DrawCircle(const TRAP::Math::Mat4& transform, const Math::Vec4& color,
		                       float thickness = 1.0f, float fade = 0.005f, int32_t entityID = -1);

		/// <summary>
		/// Draw a sprite.
		/// </summary>
		/// <param name="transform">Transform matrix for the sprite.</param>
		/// <param name="sprite">Sprite component data.</param>
		/// <param name="entityID">Entity ID of this sprite.</param>
		static void DrawSprite(const TRAP::Math::Mat4& transform, const SpriteRendererComponent& sprite, int32_t entityID = -1);

		/// <summary>
		/// Draw a colored line from point p0 to point p1.
		/// </summary>
		/// <param name="p0">First point of the line.</param>
		/// <param name="p1">Second point of the line.</param>
		/// <param name="color">Color for the line.</param>
		/// <param name="entityID">Entity ID of this line.</param>
		static void DrawLine(const TRAP::Math::Vec3& p0, const TRAP::Math::Vec3& p1, const TRAP::Math::Vec4& color, int32_t entityID = -1);

		/// <summary>
		/// Draw a rectangle (not filled).
		/// </summary>
		/// <param name="position">Position of the rectangle.</param>
		/// <param name="size">Size of the rectangle.</param>
		/// <param name="color">Color for the rectangle.</param>
		/// <param name="entityID">Entity ID of this rectangle.</param>
		static void DrawRect(const TRAP::Math::Vec3& position, const TRAP::Math::Vec2& size, const TRAP::Math::Vec4& color, int32_t entityID = -1);
		/// <summary>
		/// Draw a rectangle (not filled).
		/// </summary>
		/// <param name="transform">Transformation of the rectangle.</param>
		/// <param name="color">Color for the rectangle.</param>
		/// <param name="entityID">Entity ID of this rectangle.</param>
		static void DrawRect(const TRAP::Math::Mat4& transform, const TRAP::Math::Vec4& color, int32_t entityID = -1);

		//Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t CircleCount = 0;
			uint32_t LineCount = 0;

			/// <summary>
			/// Retrieve the current total number of vertices.
			/// </summary>
			/// <returns>Total vertices count.</returns>
			uint32_t GetTotalVertexCount() const noexcept;
			/// <summary>
			/// Retrieve the current total number of indices.
			/// </summary>
			/// <returns>Total indices count.</returns>
			uint32_t GetTotalIndexCount() const noexcept;
		};
		/// <summary>
		/// Retrieve the current Renderer2D statistics.
		/// </summary>
		/// <returns>Renderer2D statistics.</returns>
		static Statistics GetStats() noexcept;
		/// <summary>
		/// Reset the Renderer2D statistics.
		/// </summary>
		static void ResetStats() noexcept;

	private:
		static std::vector<Renderer2DData> s_data;
		static uint32_t s_dataIndex;
	};
}

#endif /*TRAP_RENDERER2D_H*/
#ifndef TRAP_RENDERER2D_H
#define TRAP_RENDERER2D_H

#include "Maths/Types.h"
#include "Maths/Mat4.h"
#include "Maths/Vec3.h"

namespace TRAP
{
	struct SpriteRendererComponent;
}
namespace TRAP::Graphics
{
#ifndef TRAP_HEADLESS_MODE
	class EditorCamera;
#endif /*TRAP_HEADLESS_MODE*/
	class Camera;
	class OrthographicCamera;
	class Texture;
	class Sampler;
	class SubTexture2D;
	struct Renderer2DData;

	class Renderer2D
	{
	public:
		/// @brief Initialize Renderer2D.
		static void Init();
		/// @brief Shutdown Renderer2D.
		static void Shutdown();
		/// @brief Reset the Renderer2D for a new frame.
		/// @note This functions gets called by TRAP::Application
		static void Reset() noexcept;

		/// @brief Begin a Renderer2D scene.
		/// @param camera Main camera.
		/// @param transform Camera transform.
		static void BeginScene(const Camera& camera, const Math::Mat4& transform);
		/// @brief Begin a Renderer2D scene.
		/// @param camera Orthographic camera.
		[[deprecated("Use BeginScene(Camera, Mat4) instead!")]]
		static void BeginScene(const OrthographicCamera& camera);
#ifndef TRAP_HEADLESS_MODE
		/// @brief Begin a Renderer2D scene.
		/// @param camera Editor camera.
		/// @remark @headless This function is not available in headless mode.
		static void BeginScene(const EditorCamera& camera);
#endif /*TRAP_HEADLESS_MODE*/
		/// @brief End a running Renderer2D scene.
		static void EndScene();

		/// @brief Set a custom sampler to use by the 2D renderer.
		/// @param sampler Sampler to use.
		static void SetCustomSampler(const TRAP::Ref<TRAP::Graphics::Sampler>& sampler);

		/// @brief Struct to describe position, rotation and scale.
		struct Transform
		{
			Math::Vec3 Position = {0.0f, 0.0f, 0.0f};
			Math::Vec3 Rotation = {0.0f, 0.0f, 0.0f};
			Math::Vec3 Scale = {1.0f, 1.0f, 1.0f};
		};

		//Primitives

		/// @brief Draw a colored quad.
		/// @param transform Transform of the quad.
		/// @param color Color for the quad.
		static void DrawQuad(const Transform& transform, const Math::Vec4& color);
		/// @brief Draw a textured quad.
		/// @param transform Transform of the quad.
		/// @param texture Texture for the quad.
		static void DrawQuad(const Transform& transform, const Ref<Texture>& texture);
		/// @brief Draw a colored and textured quad.
		/// @param transform Transform of the quad.
		/// @param color Color for the quad.
		/// @param texture Texture for the quad.
		static void DrawQuad(const Transform& transform, const Math::Vec4& color,
		                     const Ref<Texture>& texture);
		/// @brief Draw a textured quad.
		/// @param transform Transform of the quad.
		/// @param texture Texture for the quad.
		static void DrawQuad(const Transform& transform, const TRAP::Ref<SubTexture2D>& texture);
		/// @brief Draw a colored and textured quad.
		/// @param transform Transform of the quad.
		/// @param color Color for the quad.
		/// @param texture Texture for the quad.
		static void DrawQuad(const Transform& transform, const Math::Vec4& color,
		                     const TRAP::Ref<SubTexture2D>& texture);
		/// @brief Draw a colored and textured quad.
		/// This is the base function for all the other DrawQuad functions.
		/// @param transform Transform matrix for the quad.
		/// @param color Color for the quad.
		/// @param texture Texture for the quad.
		/// @param texCoords Optional: Texture coordinates for the quad.
		/// @param entityID Optional: Entity ID of the quad.
		static void DrawQuad(const TRAP::Math::Mat4& transform, const Math::Vec4& color,
		                     const Ref<Texture>& texture, const std::array<Math::Vec2, 4>* texCoords, i32 entityID = -1);

		/// @brief Draw a colored circle with given thickness and fading.
		/// This is the base function for all the other DrawCircle functions.
		/// @param transform Transform matrix for the circle.
		/// @param color Color for the circle.
		/// @param thickness Optional: Thickness for the circle.
		/// @param fade Optional: Fade for the circle.
		/// @param entityID Optional: Entity ID of the quad.
		static void DrawCircle(const TRAP::Math::Mat4& transform, const Math::Vec4& color,
		                       f32 thickness = 1.0f, f32 fade = 0.005f, i32 entityID = -1);

		/// @brief Draw a sprite.
		/// @param transform Transform matrix for the sprite.
		/// @param sprite Sprite component data.
		/// @param entityID Entity ID of this sprite.
		static void DrawSprite(const TRAP::Math::Mat4& transform, const SpriteRendererComponent& sprite, i32 entityID = -1);

		/// @brief Draw a colored line from point p0 to point p1.
		/// @param p0 First point of the line.
		/// @param p1 Second point of the line.
		/// @param color Color for the line.
		/// @param entityID Entity ID of this line.
		static void DrawLine(const TRAP::Math::Vec3& p0, const TRAP::Math::Vec3& p1, const TRAP::Math::Vec4& color, i32 entityID = -1);

		/// @brief Draw a rectangle (not filled).
		/// @param position Position of the rectangle.
		/// @param size Size of the rectangle.
		/// @param color Color for the rectangle.
		/// @param entityID Entity ID of this rectangle.
		static void DrawRect(const TRAP::Math::Vec3& position, const TRAP::Math::Vec2& size, const TRAP::Math::Vec4& color, i32 entityID = -1);
		/// @brief Draw a rectangle (not filled).
		/// @param transform Transformation of the rectangle.
		/// @param color Color for the rectangle.
		/// @param entityID Entity ID of this rectangle.
		static void DrawRect(const TRAP::Math::Mat4& transform, const TRAP::Math::Vec4& color, i32 entityID = -1);

		//Stats
		struct Statistics
		{
			u32 DrawCalls = 0;
			u32 QuadCount = 0;
			u32 CircleCount = 0;
			u32 LineCount = 0;

			/// @brief Retrieve the current total number of vertices.
			/// @return Total vertices count.
			[[nodiscard]] constexpr u32 GetTotalVertexCount() const noexcept;
			/// @brief Retrieve the current total number of indices.
			/// @return Total indices count.
			[[nodiscard]] constexpr u32 GetTotalIndexCount() const noexcept;
		};
		/// @brief Retrieve the current Renderer2D statistics.
		/// @return Renderer2D statistics.
		[[nodiscard]] static Statistics GetStats() noexcept;
		/// @brief Reset the Renderer2D statistics.
		static void ResetStats() noexcept;

	private:
		static std::vector<Renderer2DData> s_data;
		inline constinit static u32 s_dataIndex = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Renderer2D::Statistics::GetTotalVertexCount() const noexcept
{
	return (QuadCount * 4) + (CircleCount * 4) + (LineCount * 2);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::Renderer2D::Statistics::GetTotalIndexCount() const noexcept
{
	return (QuadCount * 6) + (CircleCount * 6);
}

#endif /*TRAP_RENDERER2D_H*/
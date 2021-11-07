#ifndef TRAP_RENDERER2D_H
#define TRAP_RENDERER2D_H

#include "Cameras/Orthographic/OrthographicCamera.h"
#include "Textures/Texture2D.h"

namespace TRAP::Graphics
{
	class Camera;
	struct Renderer2DData;

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const Math::Mat4& transform);
		static void BeginScene(const OrthographicCamera& camera); //Deprecated
		static void EndScene();

		//Transform struct
		struct Transform
		{
			Math::Vec3 Position = {0.0f, 0.0f, 0.0f};
			Math::Vec3 Rotation = {0.0f, 0.0f, 0.0f};
			Math::Vec3 Scale = {1.0f, 1.0f, 1.0f};
		};

		//Primitives
		static void DrawQuad(const Transform& transform, const Math::Vec4& color);
		static void DrawQuad(const Transform& transform, const Scope<Texture2D>& texture);
		static void DrawQuad(const Transform& transform, const Math::Vec4& color, const Scope<Texture2D>& texture);
		static void DrawQuad(const TRAP::Math::Mat4& transform, const Math::Vec4& color,
		                     const Scope<Texture2D>& texture);

		//Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() const;
			uint32_t GetTotalIndexCount() const;
		};
		static Statistics GetStats();
		static void ResetStats();

		static void ClearTextures();

	private:
		static void FlushAndReset();
		static float GetTextureIndex(const Scope<Texture2D>& texture);

		static Renderer2DData s_data;
	};
}

#endif /*TRAP_RENDERER2D_H*/
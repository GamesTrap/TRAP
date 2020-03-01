#ifndef _TRAP_RENDERER2D_H_
#define _TRAP_RENDERER2D_H_

#include "Cameras/Orthographic/OrthographicCamera.h"
#include "Textures/Texture2D.h"

namespace TRAP::Graphics
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();		

		//First Component
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

	private:
		static void DrawQuad(const Transform& transform);
	};
}

#endif /*_TRAP_RENDERER2D_H_*/
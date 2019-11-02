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

		//Primitives
		static void DrawQuad(const Math::Vec2& position, const Math::Vec2& size, const Math::Vec4& color); //TODO Rotation
		static void DrawQuad(const Math::Vec3& position, const Math::Vec2& size, const Math::Vec4& color); //TODO Rotation
		static void DrawQuad(const Math::Vec2& position, const Math::Vec2& size, const Scope<Texture2D>& texture); //TODO Rotation
		static void DrawQuad(const Math::Vec3& position, const Math::Vec2& size, const Scope<Texture2D>& texture); //TODO Rotation
		static void DrawQuad(const Math::Vec2& position, const Math::Vec2& size, const Math::Vec4& color, const Scope<Texture2D>& texture); //TODO Rotation
		static void DrawQuad(const Math::Vec3& position, const Math::Vec2& size, const Math::Vec4& color, const Scope<Texture2D>& texture); //TODO Rotation
	};
}

#endif /*_TRAP_RENDERER2D_H_*/
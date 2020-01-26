#ifndef _TRAP_RENDERER_H_
#define _TRAP_RENDERER_H_

#include "Buffers/UniformBuffer.h"
#include "Application.h"
#include "RenderCommand.h"

namespace TRAP::Graphics
{
	class Shader;

	class Camera;

	class Renderer
	{
	public:
		static void Init();
		
		static std::string_view GetTitle();
		static uint32_t GetDrawCalls();
		static uint32_t GetFPS();
		static float GetFrameTime();
		static uint32_t GetTickRate();

		static void SetTickRate(uint32_t tickRate);

		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Submit(const Scope<Shader>& shader, const Scope<VertexArray>& vertexArray, const Math::Mat4& transform = Math::Mat4(1.0f), RendererPrimitive primitive = RendererPrimitive::Triangle);
		
		static void Shutdown();

	private:
		struct SceneData
		{
			SceneData();

			Math::Mat4 m_projectionMatrix;
			Math::Mat4 m_viewMatrix;
			Math::Mat4 m_modelMatrix;
		};

		static Scope<SceneData> s_sceneData;
		static Scope<UniformBuffer> s_uniformBuffer;
	};
}

#endif /*_TRAP_RENDERER_H_*/
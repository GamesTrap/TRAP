#ifndef _TRAP_RENDERER_H_
#define _TRAP_RENDERER_H_

#include "Buffers/UniformBuffer.h"
#include "Application.h"
#include "RenderCommand.h"

namespace TRAP::Graphics
{
	class OrthographicCamera;
	class Shader;

	class Camera;

	class Renderer
	{
	public:
		static void Init();
		
		static const std::string& GetTitle();
		static uint32_t GetFPS();
		static float GetFrameTime();
		static uint32_t GetTickRate();
		static std::string GetCurrentGPUName();

		static void SetTickRate(uint32_t tickRate);

		static void BeginScene(const OrthographicCamera& camera); //Deprecated
		static void BeginScene(const Camera& camera, const Math::Mat4& transform);
		static void EndScene();

		//static void Submit(const Scope<Shader>& shader, const Scope<VertexArray>& vertexArray, const Math::Mat4& transform = Math::Mat4(1.0f));
		
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
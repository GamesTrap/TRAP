#ifndef _TRAP_RENDERER_H_
#define _TRAP_RENDERER_H_

#include "API/RendererAPI.h"
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
		static std::string_view GetTitle();
		static uint32_t GetDrawCalls();
		static uint32_t GetFPS();
		static float GetFrameTime();
		static uint32_t GetTickRate();

		static void SetTickRate(uint32_t tickRate);

		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Submit(const Scope<Shader>& shader, const Scope<VertexArray>& vertexArray, const Math::Mat4& transform = Math::Mat4::Identity(), RendererPrimitive primitive = RendererPrimitive::Triangle);
		
		static void Cleanup();

	private:
		struct SceneData
		{
			SceneData()
				: m_projectionMatrix(0.0f), m_viewMatrix(0.0f), m_modelMatrix(0.0f)
			{
			}

			Math::Mat4 m_projectionMatrix;
			Math::Mat4 m_viewMatrix;
			Math::Mat4 m_modelMatrix;
		};

		static Scope<SceneData> s_sceneData;
		static Scope<UniformBuffer> s_uniformBuffer;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::Renderer::GetTitle()
{
	return API::RendererAPI::GetRenderer()->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::Renderer::GetDrawCalls()
{
	return Application::Get().GetDrawCalls();
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::Renderer::GetFPS()
{
	return Application::Get().GetFPS();
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::Graphics::Renderer::GetFrameTime()
{
	return Application::Get().GetFrameTime();
}

//-------------------------------------------------------------------------------------------------------------------//

inline uint32_t TRAP::Graphics::Renderer::GetTickRate()
{
	return Application::Get().GetTickRate();
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Renderer::SetTickRate(const uint32_t tickRate)
{
	Application::Get().SetTickRate(tickRate);
}

#endif /*_TRAP_RENDERER_H_*/